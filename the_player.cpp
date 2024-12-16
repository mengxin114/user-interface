//
//
//

#include "the_player.h"

// all buttons have been setup, store pointers here
void ThePlayer::setContent(std::vector<TheButton*>* b, std::vector<TheButtonInfo>* i) {
    buttons = b;
    infos = i;
    jumpTo(buttons -> at(0) -> info);

     // 初始化未播放视频的索引列表
    unplayedIndices.clear();
    for (int index = 0; index < infos->size(); ++index) {
        unplayedIndices.push_back(index);
    }
    std::random_shuffle(unplayedIndices.begin(), unplayedIndices.end()); // 随机打乱索引
}

// change the image and video for one button every one second
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
    // 更新按钮的显示内容
    updateButtons();
}

void ThePlayer::playStateChanged (QMediaPlayer::State ms) {
    switch (ms) {
        case QMediaPlayer::StoppedState:
            // 播放器已停止
            qDebug() << "Playback stopped.";
            shuffle(); // 调用 shuffle() 播放下一个视频
            break;
        case QMediaPlayer::PlayingState:
            // 播放器正在播放
            qDebug() << "Playback started.";
            isPlaying = true;
            break;
        case QMediaPlayer::PausedState:
            // 播放器已暂停
            qDebug() << "Playback paused.";
            isPlaying = false;
            break;
        default:
            break;
    }
}

void ThePlayer::jumpTo (TheButtonInfo* button) {
    setMedia( * button -> url);
    play();
}

void ThePlayer::updateSlider() {
    static qint64 lastUpdateTime = 0;
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();

    if (currentTime - lastUpdateTime > 100) { // 每 100ms 更新一次
        lastUpdateTime = currentTime;

        if (duration() > 0) {
            int currentPos = position();
            int durationVal = duration();
            int progress = static_cast<int>((static_cast<double>(currentPos) / durationVal) * 100);
            slider->setValue(progress);
            emit progressUpdated(progress);
        }
    }
}

void ThePlayer::onTimeout() {
    if (!isSliderBeingDragged){ // 如果进度条没有被拖动
        updateSlider(); // 更新进度条
    }
    count++;
}

// 新增的槽函数，用于处理进度条的拖动事件
void ThePlayer::onSliderValueChanged(int value) {
if (isSliderBeingDragged && duration() > 0) {
        int newPosition = static_cast<int>((static_cast<double>(value) / 100) * duration());

        // 显示拖动时的提示信息
        QTime currentTime = QTime::fromMSecsSinceStartOfDay(newPosition);
        QTime totalTime = QTime::fromMSecsSinceStartOfDay(duration());
        QString tooltip = QString("%1 / %2").arg(currentTime.toString("mm:ss")).arg(totalTime.toString("mm:ss"));
        QToolTip::showText(QCursor::pos(), tooltip, slider);

        // 延迟更新播放位置
        QTimer *dragTimer = new QTimer(this);
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

void ThePlayer::togglePlayPause() {
    if (isPlaying) {
        play();
    } else {
        pause();
    }
    isPlaying = !isPlaying;
}

void ThePlayer::onVolumeChanged(int value) {
    setVolume(value);
}

void ThePlayer::updateButtons() {
    if (buttons->size() < 4 || infos->size() < 4) return; // 确保有足够的按钮和视频

    // 随机选择四个视频，并更新按钮的显示内容
    std::vector<int> randomIndices;
    for (int i = 0; i < 4; ++i) {
        int randomIndex = rand() % infos->size();
        randomIndices.push_back(randomIndex);
    }

    for (int i = 0; i < 4; ++i) {
        TheButton *button = buttons->at(i);
        TheButtonInfo *info = &infos->at(randomIndices[i]);
        button->init(info); // 更新按钮的显示内容
    }
}