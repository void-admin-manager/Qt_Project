#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QAudioOutput>
#include <QPushButton>
#include <QSlider>
#include <QHBoxLayout>
#include <QLabel>
#include <QTime>
#include <QMenu>
#include <QAction>
#include <QActionGroup>
#include <QKeyEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include "videoslider.h"
#include <QTimer>
#include <QStyle>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

    //创建播放器指针
    QMediaPlayer* player;
    //创建播放器窗口
    QVideoWidget* videoWidget;
    //创建音频输出
    QAudioOutput* audioOutput;

    //创建音频按钮
    QPushButton* voiceButton;
    //创建音频滚动条
    VideoSlider* voiceSlider;

    //创建倍速按钮
    QPushButton* speedButton;
    //倍速菜单
    QMenu* speedMenu;

    QPushButton* selectButton;

    //垂直布局
    QVBoxLayout* vLayout;

    //创建视频进度条
    VideoSlider* ProgressSlider;
    //创建播放按钮
    QPushButton* playButton;
    //创建暂停按钮
    QPushButton* stopButton;

    //创建全屏按钮
    QPushButton* fullScreen;
    //设置按钮
    QPushButton* settingButton;
    //设置菜单
    QMenu* settingMenu;

    //创建时间标签
    QLabel* timeLabel;

    //创建水平布局
    QHBoxLayout* hLayout;

    QString m_resource;

    //音量值
    int m_voice;
    //停止状态
    bool isMuted = false;
    //播放状态
    bool isPlaying = false;
    //加载状态
    bool LoadedMedia = false;
    //当前播放状态
    bool m_wasPlaying = false;
    //拖动标识
    bool m_isDraggingProgress = false;

    //创建格式化总时长
    QString totalFormattedTime;
    //创建当前格式化时间
    QString currentFormattedTime;

    //创建初始化函数
    void Init();

    //计时器
    QTimer* m_positionTimer;

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;

    //创建按钮样式函数
    void ButtonStyleSet(QPushButton* button, QString IconPath);

protected:
    // 事件过滤器
    bool eventFilter(QObject *obj, QEvent *event) override;

protected slots:
    void getduration(qint64 duration);
    void VideoPosChange(qint64 position);
    void GetPlayResource();
    void fullScreenClick();
    void setPlaybackSpeed(float speed);
    void voiceClick();
    void voiceChange(int voice);
    void playClick();
    void ProgressChange(int pos);
    void onSliderPressed();
    void onSliderReleased();
    void onSliderJumpRequested(int value);
};
#endif // WIDGET_H
