#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    client = new Client();
    client->connectToServer();
}

Widget::~Widget()
{
    delete ui;
}
