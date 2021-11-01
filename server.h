#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QObject>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);

    void inicbuff1();
    void addcbuff1(char c);
    void processCmd();
    void writeData(QByteArray data, int client);
    void showSockets();
    void clientOffline(QTcpSocket *client);
    int  clientToWrite(QTcpSocket *client);


signals:
    void messageReceived(QByteArray message);
    void messageWritten(QByteArray message);
    void clientToAttend(int client);

public slots:
    void start();
    void quit();
    void newConnetion();
    void disconnected();
    void readyRead();
    void disconnectFromHost(int i);
private:
    QTcpServer server;
    QByteArray m_message;
    int xbuff1 = 0x00;                      // Índice: siguiente CHAR en cbuff
    QChar cbuff1[1024];                     // Buffer
    QTimer *delayForBuffer;
    int socketCount = 0;
    int socketindex = 0;
    QTcpSocket *socketList[10];
    int clientList[10];
    int clientCount = 0;
};

#endif // SERVER_H
