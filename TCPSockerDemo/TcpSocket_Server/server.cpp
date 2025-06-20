#include "server.h"
#include <QDataStream>

Server::Server(QObject *parent)
    : QTcpServer(parent){
    //监听任意IP地址的指定端口
    if (!listen(QHostAddress::Any, PORT)) {
        qCritical() << "服务器启动失败:" << errorString();
        exit(EXIT_FAILURE);
    }
    //启动成功
    qInfo() << "已启动服务器端口" << PORT;
}

Server::~Server()
{
    //关闭所有连接
    foreach (auto socket, m_buffers.keys()) {
        socket->close();
        socket->deleteLater();
    }
};

// 链接成立时立即执行
void Server::incomingConnection(qintptr socketDescriptor){
    QTcpSocket *socket = new QTcpSocket(this);
    //绑定系统socket描述符
    socket->setSocketDescriptor(socketDescriptor);

    connect(socket, &QTcpSocket::readyRead, this, [this, socket]() { onReadyRead(socket); });
    connect(socket, &QTcpSocket::disconnected, this, [this, socket]() { onDisconnected(socket); });
    connect(socket, &QTcpSocket::errorOccurred, this, [this, socket](QAbstractSocket::SocketError error) {
        onErrorOccurred(socket, error);
    });

    m_buffers.insert(socket, QByteArray());
    qInfo() << "新链接:" << socket->peerAddress().toString();
}

//处理客户端数据到达
void Server::onReadyRead(QTcpSocket *socket){
    //获取该socket的缓冲区引用
    QByteArray &buffer = m_buffers[socket];
    //将数据全部读取并追加到缓冲区
    buffer.append(socket->readAll());

    //处理粘包
    while (buffer.size() >= sizeof(quint32)){
        QDataStream stream(buffer); //数据流
        stream.setByteOrder(QDataStream::BigEndian); //大端序

        quint32 msgSize;
        stream >> msgSize;  // 读取消息头（4字节长度）

        //检测数据完整性
        if (buffer.size() < msgSize + sizeof(quint32))
            return; // 等待完整数据

        // 提取完整消息
        QByteArray message = buffer.mid(sizeof(quint32), msgSize);
        buffer.remove(0, sizeof(quint32) + msgSize); //移除已处理数据

        processData(socket, message);
    }
}

//业务逻辑处理函数
void Server::processData(QTcpSocket *socket, const QByteArray &data)
{
    QString msg = QString::fromUtf8(data); //转换成字符串
    qInfo() << "接收:" << msg << "来自" << socket->peerAddress().toString();

    // 回显处理
    QString response = QString("[%1] Echo: %2").arg(
        QDateTime::currentDateTime().toString("hh:mm:ss.zzz"), msg);

    // 发送响应（添加消息头）
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);
    out << static_cast<quint32>(response.toUtf8().size()); // 消息头
    out.writeRawData(response.toUtf8().constData(), response.size()); // 消息体

    socket->write(block);
}

//断开连接处理
void Server::onDisconnected(QTcpSocket *socket)
{
    qInfo() << "已从客户端断开:" << socket->peerAddress().toString();
    m_buffers.remove(socket); //删除缓冲区映射
    socket->deleteLater(); //删除对象
}

//错误处理
void Server::onErrorOccurred(QTcpSocket *socket, QAbstractSocket::SocketError error)
{
    if (error == QAbstractSocket::RemoteHostClosedError) {
        qInfo() << "客户端已主动关闭连接";
    } else {
        qWarning() << "Socket错误:" << error << "-" << socket->errorString();
    }
    socket->close();
}
