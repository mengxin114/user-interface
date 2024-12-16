//
//
//

#ifndef CW2_THE_BUTTON_H
#define CW2_THE_BUTTON_H

#include <QPushButton>
#include <QUrl>
#include <QToolButton>
#include <QIcon>
#include <QString>
#include <QDateTime>
#include <QToolTip>
#include <QPixmap>
#include <QPainter>
#include <QColor>

class TheButtonInfo {

public:
    QUrl* url; // video file to play
    QIcon* icon; // icon to display

    TheButtonInfo ( QUrl* url, QIcon* icon) : url (url), icon (icon) {}
};

class TheButton : public QPushButton {
    Q_OBJECT

public:
    TheButtonInfo* info;

     TheButton(QWidget *parent) :  QPushButton(parent) {
         setIconSize(QSize(200,110));
         connect(this, SIGNAL(released()), this, SLOT (clicked() )); // if QPushButton clicked...then run clicked() below
    }

    void init(TheButtonInfo* i);

private slots:
    void clicked();

signals:
    void jumpTo(TheButtonInfo*);

};

// 新增 VolumeIconButton 类
class VolumeIconButton : public QToolButton {
    Q_OBJECT

    QIcon createColoredIcon(const QString& path, const QColor& color) {
        QPixmap pixmap(path); // 加载原始单色图标
        QPixmap coloredPixmap(pixmap.size());
        coloredPixmap.fill(Qt::transparent);

        QPainter painter(&coloredPixmap);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.drawPixmap(0, 0, pixmap);

        // 使用指定颜色填充图标
        painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
        painter.fillRect(coloredPixmap.rect(), color);
        painter.end();

        return QIcon(coloredPixmap);
    }


public:
    VolumeIconButton(QWidget* parent = nullptr) : QToolButton(parent) {
        QIcon coloredIcon = createColoredIcon(":/icon/a/voice.png", QColor("#00A8FF")); // 修改颜色为亮蓝色
        setIcon(coloredIcon);
        setIconSize(QSize(40, 40));

        setStyleSheet(
            "QToolButton {"
            "   background: rgba(255, 255, 255, 0.3);"
            "   border: none;"
            "   padding: 5px;"
            "} "
            "QToolButton:hover {"
            "   background: rgba(255, 255, 255, 0.5);"
            "} "
        );
    }


    void setMuteIcon() {
        setIcon(QIcon(":/icon/a/no_voice.png")); // 设置静音图标
    }

    void setHighVolumeIcon() {
        setIcon(QIcon(":/icon/a/voice.png")); // 设置高音量图标
    }

signals:
    void muteToggled(bool isMuted); // 信号，用于通知音量状态变化
};
#endif //CW2_THE_BUTTON_H
