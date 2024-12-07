#ifndef CW2_THE_PLAYER_H
#define CW2_THE_PLAYER_H

#include <QApplication>
#include <QMediaPlayer>
#include "the_button.h"
#include <vector>
#include <QTimer>
#include <QSlider>
#include <set>  // 引入set容器

class ThePlayer : public QMediaPlayer {
    Q_OBJECT

private:
    std::vector<TheButtonInfo>* infos;
    std::vector<TheButton*>* buttons;
    QTimer* mTimer;
    long updateCount = 0;
    QSlider* slider;
    bool isSliderBeingDragged;
    bool isPlaying = false;

    std::set<int> usedIndices;  // 用于记录已分配的按钮索引，确保每个按钮内容不重复

public:
    ThePlayer(QSlider* slider) : QMediaPlayer(NULL), slider(slider), isSliderBeingDragged(false) {
        setVolume(0);  // 设置音量为0
        connect(this, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(playStateChanged(QMediaPlayer::State)));

        mTimer = new QTimer(this);
        mTimer->setInterval(100);  // 设置定时器为0.1秒
        mTimer->start();
        connect(mTimer, SIGNAL(timeout()), SLOT(onTimeout()));  // 每0.1秒调用一次onTimeout方法

        connect(slider, &QSlider::sliderPressed, this, [this]() {
            isSliderBeingDragged = true;
        });

        connect(slider, &QSlider::sliderReleased, this, [this]() {
            isSliderBeingDragged = false;
        });
    }

    void setContent(std::vector<TheButton*>* b, std::vector<TheButtonInfo>* i);

    void updateSlider();

private slots:
    void shuffle();  // 刷新按钮内容
    void playStateChanged(QMediaPlayer::State ms);
    void onTimeout();  // 定时器超时时更新进度条

signals:
    void progressUpdated(int value);
    void updateSliderPosition(int value);

public slots:
    void onSliderValueChanged(int value);
    void jumpTo(TheButtonInfo* button);
    void togglePlayPause();
};

#endif //CW2_THE_PLAYER_H
