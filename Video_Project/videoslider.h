#ifndef VIDEOSLIDER_H
#define VIDEOSLIDER_H

#include <QObject>
#include <QSlider>
#include <QMouseEvent>
#include <QStyle>
#include <QStyleOptionSlider>
#include <QRect>
#include <QMediaPlayer>
#include <QPainter>
#include <QPainterPath>

class VideoSlider : public QSlider
{
    //预防要扩展槽函数，预先导入
    Q_OBJECT

public:
    using QSlider::QSlider; //继承构造函数
    VideoSlider(QSlider* Slider = nullptr);

signals:
    //发信
    void sliderJumpRequested(int value);

protected:
    //重写鼠标事件
    void mousePressEvent(QMouseEvent* event) override;
    //重新绘制
    virtual void paintEvent(QPaintEvent *event) override;
};

#endif // VIDEOSLIDER_H
