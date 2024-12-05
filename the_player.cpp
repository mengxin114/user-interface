//
//
//

#include "the_player.h"

// all buttons have been setup, store pointers here
void ThePlayer::setContent(std::vector<TheButton*>* b, std::vector<TheButtonInfo>* i) {
    buttons = b;
    infos = i;
    jumpTo(buttons -> at(0) -> info);
}

// change the image and video for one button every one second
void ThePlayer::shuffle() {
    TheButtonInfo* i = & infos -> at (rand() % infos->size() );
//        setMedia(*i->url);
    buttons -> at( updateCount++ % buttons->size() ) -> init( i );
}

void ThePlayer::playStateChanged (QMediaPlayer::State ms) {
    switch (ms) {
        case QMediaPlayer::State::StoppedState:
            play(); // starting playing again...
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
    if (duration() > 0) {
        int currentPos = position(); // 获取当前播放位置
        int durationVal = duration(); // 获取总时长
        int progress = static_cast<int>((static_cast<double>(currentPos) / durationVal) * 100); // 计算进度百分比
        slider->setValue(progress); // 更新进度条值
        emit progressUpdated(progress); // 发出进度更新信号，只发出1个参数
    }
}

void ThePlayer::onTimeout() {
    updateSlider(); // 每次超时都更新进度条

}

// 新增的槽函数，用于处理进度条的拖动事件
void ThePlayer::onSliderValueChanged(int value) {
    if (isSliderBeingDragged && duration() > 0) {
        // 计算新播放位置
        int newPosition = static_cast<int>((static_cast<double>(value) / 100) * duration());
        setPosition(newPosition); // 设置新的播放位置
    }
}
