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
  QSlider *volumeSlider;     // 音量滑块指针
  std::vector<int> unplayedIndices; // 未播放视频的索引列表
  int currentVideoIndex = 0; // 当前播放的视频索引

private:
    int previousVolume = 50; // 记录静音前的音量，默认为 50

public:
  ThePlayer(QSlider *slider, QSlider *volumeSlider)
      : QMediaPlayer(NULL), volumeSlider(volumeSlider), slider(slider),
        isSliderBeingDragged(false) {
    setVolume(0); // 初始音量为 0，避免突然的声音

    // 播放状态改变信号连接到槽函数
    connect(this, SIGNAL(stateChanged(QMediaPlayer::State)), this,
            SLOT(playStateChanged(QMediaPlayer::State)));

    // 定时器，用于更新界面
    mTimer = new QTimer(this);
    mTimer->setInterval(50);
    mTimer->start();

    connect(mTimer, &QTimer::timeout, this, &ThePlayer::onTimeout);

    // 进度条事件
    connect(slider, &QSlider::sliderPressed, this, [this]() {
      isSliderBeingDragged = true;
    });
    connect(slider, &QSlider::sliderReleased, this, [this]() {
      isSliderBeingDragged = false;
    });

    // 音量滑块的改变事件
    connect(volumeSlider, &QSlider::valueChanged, this, &ThePlayer::onVolumeChanged);
  }

  // 设置按钮和视频内容
  void setContent(std::vector<TheButton *> *b, std::vector<TheButtonInfo> *i);

  void updateSlider();
  void updateButtons(); // 用于更新按钮的显示内容

private slots:

  // 随机播放视频
  void shuffle();
  void onTimeout(); // 更新进度条
  void playStateChanged(QMediaPlayer::State ms);

signals:
  void progressUpdated(int value); // 进度更新信号
  void updateSliderPosition(int value);

public slots:
  void onSliderValueChanged(int value); // 滑块值变化事件
  void jumpTo(TheButtonInfo *button);  // 播放指定视频
  void togglePlayPause();              // 切换播放和暂停
  void onVolumeChanged(int value);     // 音量更改
  void setMute(bool isMuted);          // 设置静音或取消静音

};

#endif // CW2_THE_PLAYER_H
