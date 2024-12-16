//
//
//

#ifndef CW2_THE_PLAYER_H
#define CW2_THE_PLAYER_H

#include "the_button.h"
#include <QApplication>
#include <QMediaPlayer>
#include <QProgressBar>
#include <QSlider>
#include <QTimer>
#include <vector>


class ThePlayer : public QMediaPlayer {

  Q_OBJECT

private:
  std::vector<TheButtonInfo> *infos;
  std::vector<TheButton *> *buttons;
  QTimer *mTimer;
  long updateCount = 0;
  QSlider *slider;           // 使用指针接收外部的 slider
  bool isSliderBeingDragged; // 用于指示进度条是否正在被拖动
  bool isPlaying = false;
  int count;
  QSlider *volumeSlider;
  std::vector<int> unplayedIndices; // 未播放视频的索引列表

public:
  ThePlayer(QSlider *slider, QSlider *volumeSlider)
      : QMediaPlayer(NULL), volumeSlider(volumeSlider), slider(slider),
        isSliderBeingDragged(false) {
    setVolume(0); // be slightly less annoying
    connect(this, SIGNAL(stateChanged(QMediaPlayer::State)), this,
            SLOT(playStateChanged(QMediaPlayer::State)));

    mTimer = new QTimer(this);
    mTimer->setInterval(50);
    mTimer->start();

    connect(mTimer, &QTimer::timeout, this,
            &ThePlayer::onTimeout); // 将定时器的超时信号连接到槽
    connect(slider, &QSlider::sliderPressed, this, [this]() {
      isSliderBeingDragged = true; // 标记进度条正在被拖动
    });

    connect(slider, &QSlider::sliderReleased, this, [this]() {
      isSliderBeingDragged = false; // 标记拖动结束
    });

    connect(volumeSlider, &QSlider::valueChanged, this,
            &ThePlayer::onVolumeChanged);
  }

  // all buttons have been setup, store pointers here
  void setContent(std::vector<TheButton *> *b, std::vector<TheButtonInfo> *i);

  void updateSlider();
  void updateButtons(); // 用于更新按钮的显示内容

private slots:

  // change the image and video for one button every one second
  void shuffle();
  void onTimeout(); // 新增的槽，用于更新进度条
  void playStateChanged(QMediaPlayer::State ms);
signals:
  void progressUpdated(int value); // 定义信号
  void updateSliderPosition(int value);
public slots:
  void onSliderValueChanged(int value);
  // start playing this ButtonInfo
  void jumpTo(TheButtonInfo *button);

  void togglePlayPause();

  void onVolumeChanged(int value); // 更改音量
};

#endif // CW2_THE_PLAYER_H
