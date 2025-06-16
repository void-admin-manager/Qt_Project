#include "widget.h"
#include "./ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    //视频播放按钮
    playButton = new QPushButton();
    ButtonStyleSet(playButton, ":/images/play.png");

    //设置按钮
    settingButton = new QPushButton();
    ButtonStyleSet(settingButton, ":/images/Setting.png");

    // 创建设置菜单
    settingMenu = new QMenu(this);
    settingMenu->addAction("显示信息", [this]() {
        QMessageBox::information(this, "信息", "视频播放器demo v0.6.0");
    });
    settingMenu->addAction("Qt版本", []() {
        QMessageBox::aboutQt(nullptr, "Qt版本");
    });
    settingButton->setMenu(settingMenu);
    settingButton->setDefault(true);

    //文件选择按钮
    selectButton = new QPushButton();
    ButtonStyleSet(selectButton, ":/images/folder_1.png");

    //全屏按钮
    fullScreen = new QPushButton();
    ButtonStyleSet(fullScreen, ":/images/Fullscreen.png");

    //初始化
    Init();

    hLayout = new QHBoxLayout();

    //声音控件
    voiceButton = new QPushButton();
    //设置样式
    ButtonStyleSet(voiceButton, ":/images/sound.png");
    //设置声音滚动条
    voiceSlider = new VideoSlider(Qt::Horizontal); //水平

    //设置滚动条默认值
    voiceSlider->setValue(50);
    //设置滚动条范围值
    voiceSlider->setRange(0, 100);
    // 固定声音滚动条长度
    voiceSlider->setFixedWidth(100);

    //倍速控件
    speedButton = new QPushButton();
    ButtonStyleSet(speedButton, ":/images/double_forward_arrow.png");

    // 创建倍速菜单
    speedMenu = new QMenu(this);
    QActionGroup* speedGroup = new QActionGroup(this);
    speedGroup->setExclusive(true);

    QStringList speeds = {"0.5x", "1.0x", "1.5x", "2.0x"};
    QList<float> speedValues = {0.5f, 1.0f, 1.5f, 2.0f};

    //遍历假如菜单
    for (int i = 0; i < speeds.size(); ++i) {
        QAction* action = speedMenu->addAction(speeds[i]);
        action->setCheckable(true);
        action->setData(speedValues[i]);
        speedGroup->addAction(action);
        if (i == 1) action->setChecked(true); // 默认1.0x
        connect(action, &QAction::triggered, this, [=]() {
            setPlaybackSpeed(speedValues[i]);
        });
    }
    speedButton->setMenu(speedMenu);

    //视频进度条
    ProgressSlider = new VideoSlider(Qt::Horizontal);
    ProgressSlider->setMinimum(0);
    ProgressSlider->setMaximum(100);
    ProgressSlider->setSingleStep(1);
    ProgressSlider->setTickInterval(2);
    ProgressSlider->setValue(0);

    //时间标签
    timeLabel = new QLabel("00:00/00:00");

    //将控件添加到水平布局
    //播放
    hLayout->addWidget(playButton);
    //声音
    hLayout->addWidget(voiceButton);
    //声音进度条
    hLayout->addWidget(voiceSlider);
    //时间标签
    hLayout->addWidget(timeLabel);
    //结尾
    hLayout->addStretch();

    //倍速按钮
    hLayout->addWidget(speedButton);
    //设置按钮
    hLayout->addWidget(settingButton);
    //文件选择按钮
    hLayout->addWidget(selectButton);
    //全屏按钮
    hLayout->addWidget(fullScreen);

    //依序放入垂直布局
    vLayout->addWidget(videoWidget, 8);
    vLayout->addWidget(ProgressSlider, 1);
    vLayout->addLayout(hLayout, 0);

    connect(player, &QMediaPlayer::durationChanged, this, &Widget::getduration);
    connect(player, &QMediaPlayer::positionChanged, this, &Widget::VideoPosChange);

    //点击声音按钮触发
    connect(voiceButton, SIGNAL(clicked(bool)), this, SLOT(voiceClick()));
    //进度条值变更的时候触发
    connect(voiceSlider, SIGNAL(valueChanged(int)), this, SLOT(voiceChange(int)));
    //点击播放按钮触发
    connect(playButton, SIGNAL(clicked(bool)), this, SLOT(playClick()));
    //点击选择文件按钮时触发
    connect(selectButton, SIGNAL(clicked(bool)), this, SLOT(GetPlayResource()));
    // 全屏按钮点击
    connect(fullScreen, SIGNAL(clicked(bool)), this, SLOT(fullScreenClick()));
    //进度条值变更的时候触发
    connect(ProgressSlider, SIGNAL(valueChanged(int)), this, SLOT(ProgressChange(int)));
    //防止相互触发
    connect(ProgressSlider, &QSlider::sliderPressed, this, &Widget::onSliderPressed);
    connect(ProgressSlider, &QSlider::sliderReleased, this, &Widget::onSliderReleased);
    connect(ProgressSlider, &VideoSlider::sliderJumpRequested, this, &Widget::onSliderJumpRequested);

    // 使用定时器减少位置更新频率
    m_positionTimer = new QTimer(this);
    m_positionTimer->setInterval(100);
    connect(m_positionTimer, &QTimer::timeout, this, [=]{
        if(player->isPlaying()) {
            VideoPosChange(player->position());
        }
    });
}

Widget::~Widget()
{
    delete ui;
}

bool Widget::eventFilter(QObject *obj, QEvent *event){
    //指定框架(看你喜欢，可以指定整个widget也行，我这里指定的时videoWidget，但是如果不在videoWidget上时不会触发该事件)
    if (obj == videoWidget){
        //单击
        if(event->type() == QEvent::MouseButtonPress){
            playClick();
            return true;
        }
        //双击
        else if(event->type() == QEvent::MouseButtonDblClick){
            fullScreenClick();
            return true;
        }
        //退出全屏
        else if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Escape && isFullScreen()) {
                showNormal();
                ButtonStyleSet(fullScreen, ":/images/Fullscreen.png");
                return true;
            }
        }
    }
    //其他事件
    return QWidget::eventFilter(obj, event);
}

void Widget::Init(){
    //初始化变量
    currentFormattedTime = "00:00";
    m_voice = 50;

    //初始化对象
    audioOutput = new QAudioOutput(this);
    vLayout = new QVBoxLayout(this); //绑定widget对象

    player = new QMediaPlayer;
    player->setPlaybackRate(1.0); //设置默认播放速度

    videoWidget = new QVideoWidget(this); //创建播放窗口
    videoWidget->setAspectRatioMode(Qt::KeepAspectRatio);//保持宽高比

    player->setVideoOutput(videoWidget); //设置播放窗口
    player->setAudioOutput(audioOutput); //设置声音
    audioOutput->setVolume(m_voice / 100.0f); //初始化声音设置

    //初始化播放按钮
    playButton->setEnabled(false);

    //默认显示视频界面
    videoWidget->show();

    //设置默认大小
    resize(1024, 600);

    //设置最小窗口大小
    this->setMinimumSize(800, 600);

    // this->setStyleSheet("background-color: black;");

    // 安装事件过滤器
    videoWidget->installEventFilter(this);
    videoWidget->setFocusPolicy(Qt::StrongFocus); // 允许接收键盘事件

}

void Widget::getduration(qint64 duration){
    //创建并初始化总时长格式
    QTime totalTime = QTime(0, 0, 0, 0); //h,m,s,ms
    //获取总时长
    totalTime = totalTime.addMSecs(duration);
    //转换总时长格式
    totalFormattedTime = totalTime.toString("mm:ss");
    //设置timelabel
    timeLabel->setText(currentFormattedTime + "/" + totalFormattedTime);
}

void Widget::VideoPosChange(qint64 position)
{
    if (m_isDraggingProgress) return;

    if (player->duration() <= 0) return;

    int progress = static_cast<int>(position * 100.0 / player->duration());

    // 阻塞信号防止循环
    ProgressSlider->blockSignals(true);
    ProgressSlider->setValue(progress);
    ProgressSlider->blockSignals(false);

    QTime currentTime = QTime::fromMSecsSinceStartOfDay(position);
    currentFormattedTime = currentTime.toString("mm:ss");
    timeLabel->setText(currentFormattedTime + "/" + totalFormattedTime);
}

void Widget::ButtonStyleSet(QPushButton* button, QString IconPath){
    //设置图像
    button->setIcon(QIcon(IconPath));
    //去除边框
    button->setFlat(true);
    //设置CSS
    button->setStyleSheet("QPushButton:hover {background-color: grey;}"
                          "QPushButton:pressed {background-color: darkGrey;}"
                          );
}

void Widget::GetPlayResource(){
    m_resource = QFileDialog::getOpenFileName(this, tr("选择播放文件"), "D:/", tr("视频文件(*.mp4 *.avi *.mkv)"));
    //如果选择文件不为空
    if (!m_resource.isEmpty()) {
        //设置播放源
        player->setSource(QUrl::fromLocalFile(m_resource));
        //开播！
        player->play();
        //设置状态
        isPlaying = true;
        playButton->setEnabled(true);
        //设置样式
        ButtonStyleSet(playButton, ":/images/stop.png");
    }
}

void Widget::fullScreenClick(){
    if (isFullScreen()) {
        showNormal();
        ButtonStyleSet(fullScreen, ":/images/Fullscreen.png");
    } else {
        showFullScreen();
        ButtonStyleSet(fullScreen, ":/images/Fullscreen.png");
    }
}

//变换倍速
void Widget::setPlaybackSpeed(float speed) {
    player->setPlaybackRate(speed);
}

void Widget::voiceClick(){
    isMuted = !isMuted;

    if (isMuted) {
        audioOutput->setVolume(0.0f);
        ButtonStyleSet(voiceButton, ":/images/mute.png");
    } else {
        audioOutput->setVolume(m_voice / 100.0f);
        ButtonStyleSet(voiceButton, ":/images/sound.png");
    }
}

void Widget::voiceChange(int voice){
    m_voice = voice;

    // 转换为浮点数音量
    float volume = voice / 100.0f;

    if (!isMuted) {
        audioOutput->setVolume(volume);
    }

    // 更新静音状态和图标
    isMuted = (voice == 0);
    ButtonStyleSet(voiceButton, isMuted ? ":/images/mute.png" : ":/images/sound.png");
}

void Widget::playClick(){
    if (!player || !playButton->isEnabled()) return;

    isPlaying = !isPlaying;
    if(isPlaying) {
        player->play();
        ButtonStyleSet(playButton, ":/images/stop.png");
        m_positionTimer->start();  // 开始位置更新定时器
    } else {
        player->pause();
        ButtonStyleSet(playButton, ":/images/play.png");
        m_positionTimer->stop();   // 停止位置更新定时器
    }
}

void Widget::ProgressChange(int pos){
    qint64 duration = player->duration();
    if (duration <= 0) return;

    // 计算目标位置
    qint64 targetPos = static_cast<qint64>(pos * duration / 100.0);
    player->setPosition(targetPos);

    // 更新显示的时间
    QTime currentTime = QTime::fromMSecsSinceStartOfDay(targetPos);
    currentFormattedTime = currentTime.toString("mm:ss");
    timeLabel->setText(currentFormattedTime + "/" + totalFormattedTime);
}

void Widget::onSliderPressed()
{
    m_isDraggingProgress = true;
    m_wasPlaying = (player->playbackState() == QMediaPlayer::PlayingState);
    if (m_wasPlaying) {
        player->pause();
    }
}

void Widget::onSliderReleased()
{
    m_isDraggingProgress = false;
    if (m_wasPlaying) {
        player->play();
    }
}

void Widget::onSliderJumpRequested(int value)
{
    //时长小于等于0
    if(player->duration() <= 0) return;

    //获取并转换格式
    qint64 targetPos = static_cast<qint64>(value * player->duration() / 100.0);
    player->setPosition(targetPos);

    // 更新显示的时间
    QTime currentTime = QTime::fromMSecsSinceStartOfDay(targetPos);
    currentFormattedTime = currentTime.toString("mm:ss");
    timeLabel->setText(currentFormattedTime + "/" + totalFormattedTime);
}
