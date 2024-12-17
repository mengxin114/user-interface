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
    TheButtonInfo* i = & infos -> at (rand() % infos->size() );
//        setMedia(*i->url);
    buttons -> at( updateCount++ % buttons->size() ) -> init( i );
}

// 播放状态改变事件
void ThePlayer::playStateChanged(QMediaPlayer::State ms) {
    switch (ms) {
        case QMediaPlayer::StoppedState:
            // 播放器已停止
            qDebug() << "Playback stopped.";
            play();
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
    setMedia(*button->url); // 设置新的视频文件
    play();                 // 播放新视频

    // 发出状态改变信号，让按钮同步到正确的状态
    emit stateChanged(QMediaPlayer::PlayingState);
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
    if (count >= 10) {  // 如果计数器达到10  
            shuffle();      // 调用 shuffle 方法  
            count = 0;     // 重置计数器  
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

    // 确保索引列表中有足够的视频来选择
    if (unplayedIndices.size() < 4) {
        // 如果未使用视频索引不足，重新填充并打乱
        unplayedIndices.clear();
        for (int index = 0; index < infos->size(); ++index) {
            unplayedIndices.push_back(index);
        }
        std::random_shuffle(unplayedIndices.begin(), unplayedIndices.end());
    }

    // 从未使用的视频索引列表中选择前4个
    std::vector<int> selectedIndices;
    for (int i = 0; i < 4; ++i) {
        selectedIndices.push_back(unplayedIndices.back());
        unplayedIndices.pop_back(); // 从未使用列表中移除已选索引
    }

    // 更新按钮内容
    for (int i = 0; i < 4; ++i) {
        TheButton* button = buttons->at(i);
        TheButtonInfo* info = &infos->at(selectedIndices[i]);
        button->init(info); // 重新初始化按钮内容
    }
}

