#include "videoslider.h"

VideoSlider::VideoSlider(QSlider* parent)
    :QSlider(parent){

}

void VideoSlider::mousePressEvent(QMouseEvent* event){
    //获取鼠标事件
    if(event->button() == Qt::LeftButton){
        QStyleOptionSlider opt;
        //初始化滑动跳样式
        initStyleOption(&opt);

        //可以自行添加设定

        //滑轨区域
        QRect grooveRect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, this);
        //滑块区域
        QRect handleRect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);

        //如果点击在滑块上，保留默认行为
        if(handleRect.contains(event->pos())){
            QSlider::mousePressEvent(event);
            return;
        }

        //计算点击位置对应的值
        int newValue;
        //水平
        if(orientation() == Qt::Horizontal){
            //获取滑块宽度
            int handleWidth = handleRect.width();
            //计算相对于滑轨起点的点击位置
            int clickPos = event->position().x() - grooveRect.x() - handleWidth / 2;
            //计算滑轨有效宽度
            int grooveWidth = grooveRect.width() - handleWidth;
            //转换对应的进度值
            newValue = QStyle::sliderValueFromPosition(
                minimum(), maximum(),
                clickPos, grooveWidth,
                opt.upsideDown
                );
        }
        else{
            int handleHeight = handleRect.height();
            int clickPos = event->position().y() - grooveRect.y() - handleHeight / 2;
            int grooveHeight = grooveRect.height() - handleHeight;
            newValue = QStyle::sliderValueFromPosition(
                minimum(), maximum(),
                clickPos, grooveHeight,
                opt.upsideDown
                );
        }

        //发送信号
        emit sliderJumpRequested(newValue);

        //已完成事件
        event->accept();
    }
    //其他鼠标事件
    else{
        //保持原有操作
        QSlider::mousePressEvent(event);
    }
}

void VideoSlider::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect grooveRect = rect().adjusted(2, 10, -2, -10);
    qreal progress = qreal(value()-minimum()) / qreal(maximum()-minimum());
    QRectF progressRect = grooveRect.adjusted(0, 0,
                                              (grooveRect.width()-12)*progress - grooveRect.width(), 0);

    // 背景轨道
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(225,225,225,150));
    painter.drawRoundedRect(grooveRect, 2, 2);

    // 进度条
    QLinearGradient progGrad(progressRect.topLeft(), progressRect.topRight());
    progGrad.setColorAt(0, QColor(0,122,255));
    progGrad.setColorAt(1, QColor(10,132,255));
    painter.setBrush(progGrad);
    painter.drawRoundedRect(progressRect, 2, 2);

    // 滑块
    QRectF handleRect(grooveRect.left() + (grooveRect.width()-12)*progress,
                      grooveRect.center().y()-6, 12, 12);
    QPainterPath path;
    path.addEllipse(handleRect);
    painter.setBrush(QBrush(Qt::white));
    painter.setPen(QPen(QColor(200,200,200), 0.5));
    painter.drawPath(path);
}
