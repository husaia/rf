#include "server.h"

Server::Server(QObject *parent) : QObject(parent)
{
    delayForBuffer = new QTimer(this);
    connect(delayForBuffer,&QTimer::timeout,this,&Server::processCmd);
    connect(&server,&QTcpServer::newConnection,this,&Server::newConnetion);
}

void Server::inicbuff1()
{
    for (int i = 0; i < 1022; i++){ cbuff1[i] = 0x00; } xbuff1 = 0x00;
}

void Server::addcbuff1(char c)
{
    if((xbuff1 + 1) >= 1023){ inicbuff1(); qDebug() << "buffer saturado";}
    cbuff1[xbuff1++] = c;
}
void Server::processCmd()
{
    m_message.append(QString(cbuff1));
    emit messageReceived(m_message);
    delayForBuffer->stop();
    m_message.clear();
    inicbuff1();
}

void Server::start()
{
    qDebug() << "Server iniciando...";
    if(!server.listen(QHostAddress::Any,1234)){
        qDebug() << "Server no puede iniciar";
        server.setMaxPendingConnections(5);
    }else {
        qDebug() << "Server iniciado";
    }
}

void Server::quit()
{
    server.close();
}

void Server::newConnetion()
{
    QString saludo = "hola cliente: " + QString::number(socketCount);
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    socket = server.nextPendingConnection();
    connect(socket,&QTcpSocket::disconnected,this,&Server::disconnected);
    connect(socket,&QTcpSocket::readyRead,this,&Server::readyRead);
    socket->write(saludo.toUtf8());
    socket->flush();
    socket->waitForBytesWritten();
    qDebug() << "Conexion detectada" << socket;
    socketList[socketindex] = socket;
    socketCount++;
    socketindex++;
}

void Server::disconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    qDebug() << "socket disconnected: " << socket;
    clientOffline(socket);
    socketCount--;
}

void Server::readyRead()
{
    delayForBuffer->start(250);
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    //qDebug() << "me habla el socket: " << socket;
    while(socket->bytesAvailable()){
        addcbuff1(socket->read(1).at(0));
    }
    qDebug() << "a este cliente le responderas"  <<clientToWrite(socket);
    emit clientToAttend(clientToWrite(socket));


//    if( clientCount >= 0 && clientList[clientCount-1] != clientToWrite(socket)){
//        clientList[clientCount] = clientToWrite(socket);
//        clientCount++;
//    }
}

void Server::disconnectFromHost(int i)
{
    socketList[i]->disconnectFromHost();
}

void Server::writeData(QByteArray data,int client)
{
        socketList[client]->write(data);
        socketList[client]->flush();
        socketList[client]->waitForBytesWritten();
    emit messageWritten(data);
}

void Server::showSockets()
{
    if(socketCount > 0){
        for (int i = 0 ;i < socketCount;i++) {
            qDebug() << "socket: " << socketList[i];
        }
    }
}

void Server::clientOffline(QTcpSocket *client)
{
    for (int i = 0; i < socketCount;i++) {
        if(socketList[i] == client){
            if(i < socketCount){
                for (int j = i; j < socketCount; j++) {
                    socketList[j] = socketList[j+1];
                }
            }
            break;
        }
    }
}

int Server::clientToWrite(QTcpSocket *client)
{
    for (int i = 0; i < socketCount; i++) {
        if(socketList[i] == client){
            return i;
        }
    }
}
