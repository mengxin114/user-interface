#include "the_player.h"

// 所有按钮已经初始化，存储指针
void ThePlayer::setContent(std::vector<TheButton*>* b, std::vector<TheButtonInfo>* i) {
    buttons = b;
    infos = i;
    jumpTo(buttons->at(0)->info);  // 默认播放第一个视频
}

// 每0.1秒刷新一次按钮的内容，确保不重复
void ThePlayer::shuffle() {
    if (buttons && infos) {
        usedIndices.clear();  // 每次刷新时清空已使用的索引
        for (auto& button : *buttons) {
            int newIndex;
            do {
                newIndex = rand() % infos->size();  // 随机选择一个视频
            } while (usedIndices.find(newIndex) != usedIndices.end());  // 保证不重复
            usedIndices.insert(newIndex);  // 记录已使用的索引
            button->init(&infos->at(newIndex));  // 更新按钮内容
        }
    }
}

// 播放状态变化
void ThePlayer::playStateChanged(QMediaPlayer::State ms) {
    if (ms == QMediaPlayer::State::StoppedState) {
        play();  // 播放停止后重新开始播放
    }
}

// 跳转到指定按钮对应的视频
void ThePlayer::jumpTo(TheButtonInfo* button) {
    setMedia(*button->url);  // 设置视频内容
    play();  // 播放
}

// 更新进度条
void ThePlayer::updateSlider() {
    if (duration() > 0) {
        int currentPos = position();  // 获取当前播放位置
        int durationVal = duration();  // 获取总时长
        int progress = static_cast<int>((static_cast<double>(currentPos) / durationVal) * 100);  // 计算进度百分比
        slider->setValue(progress);  // 更新进度条值
    }
}

// 定时器超时时更新进度条
void ThePlayer::onTimeout() {
    updateSlider();  // 每0.1秒调用一次，更新进度条
}

// 处理滑块变化
void ThePlayer::onSliderValueChanged(int value) {
    if (isSliderBeingDragged && duration() > 0) {
        int newPosition = static_cast<int>((static_cast<double>(value) / 100) * duration());  // 计算新位置
        setPosition(newPosition);  // 设置新的播放位置
        if (isPlaying) {
            play();  // 如果视频正在播放，继续播放
        }
    }
}

// 播放或暂停
void ThePlayer::togglePlayPause() {
    if (isPlaying) {
        pause();  // 暂停播放
    } else {
        play();  // 播放
    }
    isPlaying = !isPlaying;
}
