#include "client.h"
#include <QDataStream>
#include <QThread>
#include <QDebug>

Client::Client(QObject *parent) : QObject(parent), m_socket(new QTcpSocket(this))
{
    //连接
    connect(m_socket, &QTcpSocket::connected, this, &Client::onConnected);
    //读取
    connect(m_socket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
    //断开链接
    connect(m_socket, &QTcpSocket::disconnected, this, &Client::onDisconnected);
    //错误
    connect(m_socket, &QTcpSocket::errorOccurred, this, &Client::onErrorOccurred);
}

void Client::connectToServer()
{
    qInfo() << "正在链接服务器...";
    m_socket->connectToHost(HOST, PORT);
}

// 连接成功
void Client::onConnected()
{
    qInfo() << "已连接到服务器";
    sendMessage("您好!");
}

//发送消息
void Client::sendMessage(const QString &message)
{
    //检查连接状态
    if (m_socket->state() != QAbstractSocket::ConnectedState) {
        qWarning() << "未连接服务器,请检查网络状态";
        return;
    }

    // 添加消息头 (4字节长度)
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian); //设置大端序
    out << static_cast<quint32>(message.toUtf8().size()); //写入消息长度
    out.writeRawData(message.toUtf8().constData(), message.size()); //写入消息

    m_socket->write(block); //发送数据
    qDebug() << "Sent:" << message; //调试输出
}

//数据到达回调
void Client::onReadyRead()
{
    m_buffer.append(m_socket->readAll()); //读取所有数据到缓冲区
    processData(); //处理接收到的数据
}

void Client::processData()
{
    // 处理粘包
    while (m_buffer.size() >= sizeof(quint32)) {
        QDataStream stream(m_buffer);
        stream.setByteOrder(QDataStream::BigEndian);

        quint32 msgSize;
        stream >> msgSize;

        if (m_buffer.size() < msgSize + sizeof(quint32))
            return; // 等待完整数据

        // 提取完整消息
        QByteArray message = m_buffer.mid(sizeof(quint32), msgSize);
        m_buffer.remove(0, sizeof(quint32) + msgSize); //移除已处理数据

        emit messageReceived(QString::fromUtf8(message));
    }
}

void Client::onDisconnected()
{
    qInfo() << "已从服务器断开";
}

void Client::onErrorOccurred(QAbstractSocket::SocketError error)
{
    if (error == QAbstractSocket::RemoteHostClosedError) {
        qInfo() << "服务器已主动关闭连接";
    } else {
        qWarning() << "Socket错误:" << error << "-" << m_socket->errorString();
    }
    m_socket->close();
}
