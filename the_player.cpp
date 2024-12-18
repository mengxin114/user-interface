//
//
//

#include "the_player.h"

// 所有按钮和视频内容的初始化
void ThePlayer::setContent(std::vector<TheButton*>* b, std::vector<TheButtonInfo>* i) {
    buttons = b;
    infos = i;

    // 初始化未播放列表
    unplayedIndices.clear();
    for (int index = 0; index < infos->size(); ++index) {
        unplayedIndices.push_back(index);
    }
    std::random_shuffle(unplayedIndices.begin(), unplayedIndices.end());

    // 播放第一个视频
    jumpTo(buttons->at(0)->info);
}

// 随机选择并播放未播放的视频
void ThePlayer::shuffle() {
    static int currentVideoIndex = -1;

    if (infos->size() <= 1) {
        currentVideoIndex = 0;
        jumpTo(&infos->at(currentVideoIndex));
        pause(); // 切换到新视频后暂停播放
        return;
    }

    int nextIndex;
    do {
        nextIndex = rand() % infos->size();
    } while (nextIndex == currentVideoIndex);

    currentVideoIndex = nextIndex;

    TheButtonInfo* nextVideo = &infos->at(currentVideoIndex);
    jumpTo(nextVideo);
    pause(); // 切换到新视频后暂停播放
}






// 播放状态改变事件
void ThePlayer::playStateChanged(QMediaPlayer::State ms) {
    switch (ms) {
        case QMediaPlayer::StoppedState:
            qDebug() << "Playback stopped.";
            shuffle(); // 切换到下一个视频而不是重播当前视频
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
    if (!button) {
        qDebug() << "jumpTo called with null button!";
        return;
    }
    qDebug() << "Playing new video:" << button->url->toString();
    setMedia(*button->url); // 设置新的视频文件
    play();                 // 开始播放
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
    if (!isSliderBeingDragged) {
        updateSlider();
    }

    // 视频播放结束后自动切换
    if (state() == QMediaPlayer::StoppedState) {
        shuffle();
    }

    // 定时刷新侧边栏按钮
    static int buttonRefreshCounter = 0;
    if (++buttonRefreshCounter >= 80) { // 每 10 秒刷新一次
        updateButtons();
        buttonRefreshCounter = 0;
    }
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
    if (state() == QMediaPlayer::PlayingState) {
        pause(); // 如果播放器正在播放，则暂停
    } else {
        play();  // 如果播放器已暂停或停止，则开始播放
    }
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

    // 随机生成 4 个唯一索引
    std::set<int> uniqueIndices;
    while (uniqueIndices.size() < 4) {
        uniqueIndices.insert(rand() % infos->size());
    }

    // 更新按钮内容
    int i = 0;
    for (int index : uniqueIndices) {
        TheButton* button = buttons->at(i++);
        TheButtonInfo* info = &infos->at(index);
        button->init(info);
    }
}



