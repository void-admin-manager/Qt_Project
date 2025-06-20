#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QObject>
#include <QMap>
#include <QDateTime>
#include <QDebug>

class Server : public QTcpServer
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);
    ~Server();

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void onReadyRead(QTcpSocket *socket);
    void onDisconnected(QTcpSocket *socket);
    void onErrorOccurred(QTcpSocket *socket, QAbstractSocket::SocketError error);

private:
    void processData(QTcpSocket *socket, const QByteArray &data);

    QMap<QTcpSocket*, QByteArray> m_buffers; // 每个socket的缓冲区
    //演示端口
    const quint16 PORT = 12345; // 0-65535
};

#endif // SERVER_H
