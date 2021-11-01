#include "antena.h"
antena::antena(QObject *parent, QString portName,
               int baudRate, int bitData,
               int stopBit, int parity) : QObject(parent)
{
    qDebug() << "hola soy una antena";
    this->portName = portName;
    this->baudRate = baudRate;
    this->bitData = bitData;
    this->stopBit = stopBit;
    this->parity = parity;

    port = new QSerialPort(this);
    timerProcess = new QTimer(this);
    timerPort = new QTimer(this);
    connect(timerProcess, &QTimer::timeout, this, &antena::cmdProcess);
    connect(timerPort, &QTimer::timeout, this, &antena::cmdSend);
}

void antena::addcbuff(QChar c)
{
    consoleResponse = 0;
    cbuff[xbuff++] = c;
}

void antena::inicbuff()
{
   for (int i = 0; i < 512; i++){ cbuff[i] = 0x00; } xbuff = 0x00;
}

void antena::setAntenaEnable(bool value)
{
    if(value != antenaEnabled){
        antenaEnabled = value;
    }
}

bool antena::isAntenaEnabled()
{
    return antenaEnabled;
}

void antena::start()
{
    open();
    if(port->isOpen()) timerPort->start(1000);
}

void antena::open()
{
    if(isAntenaEnabled()){
        foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
            qDebug() << serialPortInfo.portName();
            if(serialPortInfo.portName() == portName){
                antenaAvailable = true;
            }
        }
        if(antenaAvailable){
            // open and configure the serialport
            port->setPortName(portName);
            port->setBaudRate(baudRate);
            port->setDataBits(static_cast<QSerialPort::DataBits>(bitData));
            port->setParity(static_cast<QSerialPort::Parity>(parity));
            port->setStopBits(static_cast<QSerialPort::StopBits>(stopBit));
            port->setFlowControl(QSerialPort::NoFlowControl);
            port->open(QSerialPort::ReadWrite);
            qDebug() << "COM2" << portName << baudRate << bitData  << stopBit << parity;
            connect(port,&QSerialPort::readyRead,this,&antena::readData);
            connect(port,&QSerialPort::errorOccurred,this,&antena::errorOcurred);
        }
    }
}

void antena::close()
{
    port->close();
    disconnect(port,&QSerialPort::readyRead,this,&antena::readData);
    disconnect(port,&QSerialPort::errorOccurred,this,&antena::errorOcurred);
}

void antena::errorOcurred(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError)
    {
        qDebug() << "Esto es el debug: " << port->errorString();
        emit errordetected(port->errorString());
    }
}

void antena::readData()
{
    if(dataInStart == 0){
        timerProcess->start(150);
        dataInStart = 1;
    }
    QChar dato = 0x00;
    while(port->bytesAvailable()){
        dato = port->read(1).at(0); addcbuff(dato);
    }
}

void antena::cmdProcess()
{
    QString trama = QString(cbuff);
    QStringList taglist;
    if(cbuff[0] == 0x07 && xbuff == 8){
        qDebug() << "NO SE IDENTIFICO NINGUN TAG";
    }
    if(xbuff >= 21){
        char buffer[4]; QString s = "", tag = "";
        tagFound = cbuff[5].toLatin1();
        for(int i = 0; i < cbuff[5]; i++){
            for(int j = 0; j < 12; j++){
                s = cbuff[7 + (i * 14) + j].toLatin1();
                sprintf(buffer, "%02X", s[0].toLatin1());
                tag += buffer;
            }
            tags[i] = tag; tag = "";
            //qDebug() << i << tags[i];
            taglist << tags[i];
        }
    }
    emit tagsAvailable(taglist);
    timerProcess->stop(); dataInStart = 0;
    timerPort->start(1000);
    inicbuff();
}

void antena::cmdSend()
{
    port->putChar(0x09); port->putChar(0x00); port->putChar(0x01); port->putChar(0x01); port->putChar(0x00);
    port->putChar(0x00); port->putChar(0x80); port->putChar(0x0A); port->putChar(0x76); port->putChar(0xFC);
    consoleResponse++;
    if(consoleResponse >= 3){
       consoleResponse = 3;
       //qDebug() << "NO HAY COMUNICACION CON EL LECTOR";
    }
}
