//
//
//

#include "the_player.h"

// 所有按钮和视频内容的初始化
void ThePlayer::setContent(std::vector<TheButton*>* b, std::vector<TheButtonInfo>* i) {
    buttons = b;
    infos = i;
    jumpTo(buttons->at(0)->info);

    // 初始化未播放视频的索引列表
    unplayedIndices.clear();
    for (int index = 0; index < infos->size(); ++index) {
        unplayedIndices.push_back(index);
    }
    std::random_shuffle(unplayedIndices.begin(), unplayedIndices.end()); // 随机打乱索引
}

// 随机选择并播放未播放的视频
void ThePlayer::shuffle() {
    if (infos->empty()) return; // 如果没有视频，直接返回

    if (unplayedIndices.empty()) {
        // 如果未播放的视频索引为空，重新填充
        for (int index = 0; index < infos->size(); ++index) {
            unplayedIndices.push_back(index);
        }
        std::random_shuffle(unplayedIndices.begin(), unplayedIndices.end()); // 随机打乱索引
    }

    // 随机选择一个未播放的视频
    int randomIndex = rand() % unplayedIndices.size();
    int videoIndex = unplayedIndices[randomIndex];

    // 跳转到随机选择的视频
    TheButtonInfo *randomInfo = &infos->at(videoIndex);
    jumpTo(randomInfo);

    // 从未播放列表中移除已播放的视频
    unplayedIndices.erase(unplayedIndices.begin() + randomIndex);
    updateButtons();
}

// 播放状态改变事件
void ThePlayer::playStateChanged(QMediaPlayer::State ms) {
    switch (ms) {
        case QMediaPlayer::StoppedState:
            shuffle();
            break;
        case QMediaPlayer::PlayingState:
            isPlaying = true;
            break;
        case QMediaPlayer::PausedState:
            isPlaying = false;
            break;
        default:
            break;
    }
}

// 跳转到指定的视频
void ThePlayer::jumpTo(TheButtonInfo* button) {
    setMedia(*button->url);
    play();
}

// 更新进度条
void ThePlayer::updateSlider() {
    static qint64 lastUpdateTime = 0;
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();

    if (currentTime - lastUpdateTime > 100) { // 每 100ms 更新一次
        lastUpdateTime = currentTime;

        if (duration() > 0) {
            int progress = static_cast<int>((static_cast<double>(position()) / duration()) * 100);
            slider->setValue(progress);
            emit progressUpdated(progress);
        }
    }
}

// 定时器超时处理
void ThePlayer::onTimeout() {
    if (!isSliderBeingDragged) { // 如果进度条没有被拖动
        updateSlider(); // 更新进度条
    }
    count++;
}

// 进度条拖动事件处理
void ThePlayer::onSliderValueChanged(int value) {
    if (isSliderBeingDragged && duration() > 0) {
        int newPosition = static_cast<int>((static_cast<double>(value) / 100) * duration());

        // 显示拖动时的提示信息
        QTime currentTime = QTime::fromMSecsSinceStartOfDay(newPosition);
        QTime totalTime = QTime::fromMSecsSinceStartOfDay(duration());
        QString tooltip = QString("%1 / %2").arg(currentTime.toString("mm:ss")).arg(totalTime.toString("mm:ss"));
        QToolTip::showText(QCursor::pos(), tooltip, slider);

        QTimer* dragTimer = new QTimer(this);
        dragTimer->setSingleShot(true);
        dragTimer->setInterval(200);

        connect(dragTimer, &QTimer::timeout, this, [this, newPosition, dragTimer]() {
            setPosition(newPosition);
            dragTimer->deleteLater();
            if (isPlaying) {
                play();
            }
        });

        dragTimer->start();
    }
}

// 切换播放和暂停
void ThePlayer::togglePlayPause() {
    if (isPlaying) {
        pause();
    } else {
        play();
    }
    isPlaying = !isPlaying;
}

// 音量变化处理
void ThePlayer::onVolumeChanged(int value) {
    if (value == 0) {
        // 如果音量设置为 0，进入静音状态
        setMute(true);
    } else {
        if (volume() == 0) {
            // 如果当前是静音状态，取消静音
            setMute(false);
        }
        setVolume(value); // 更新音量
        previousVolume = value; // 记录当前音量
    }
}


// 静音或取消静音
void ThePlayer::setMute(bool isMuted) {
    if (isMuted) {
        previousVolume = volumeSlider->value(); // 保存静音前的音量
        setVolume(0);                           // 设置音量为 0
        volumeSlider->setValue(0);              // 同步更新滑块
    } else {
        setVolume(previousVolume);              // 恢复静音前的音量
        volumeSlider->setValue(previousVolume); // 同步更新滑块
    }
}

// 更新按钮显示内容
void ThePlayer::updateButtons() {
    if (buttons->size() < 4 || infos->size() < 4) return;

    std::vector<int> randomIndices;
    for (int i = 0; i < 4; ++i) {
        randomIndices.push_back(rand() % infos->size());
    }

    for (int i = 0; i < 4; ++i) {
        TheButton* button = buttons->at(i);
        TheButtonInfo* info = &infos->at(randomIndices[i]);
        button->init(info);
    }
}
