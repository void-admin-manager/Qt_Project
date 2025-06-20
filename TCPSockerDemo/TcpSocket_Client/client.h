#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include <QObject>

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);
    void connectToServer();
    void sendMessage(const QString &message);

signals:
    void messageReceived(const QString &msg);

private slots:
    void onConnected();
    void onReadyRead();
    void onDisconnected();
    void onErrorOccurred(QAbstractSocket::SocketError error);

private:
    void processData();

    QTcpSocket *m_socket;
    QByteArray m_buffer;
    //演示主机端口
    const QString HOST = "127.0.0.1";
    const quint16 PORT = 12345; // 0-65535
};

#endif // CLIENT_H
