#include "xbee.h"

#define STX 0x02
#define ETX 0x03

xbee::xbee(QObject *parent, QString portName,
           int baudRate, int bitData,
           int stopBit, int parity) : QObject(parent)
{
    port = new QSerialPort(this);
    timerProcess = new QTimer(this);
    this->portName = portName;
    this->baudRate = baudRate;
    this->bitData = bitData;
    this->stopBit = stopBit;
    this->parity = parity;

    connect(timerProcess, &QTimer::timeout, this, &xbee::cmdProcess);
}

void xbee::addcbuff(QChar c)
{
    if((xbuff + 1) >= 512){ inicbuff(); qDebug() << "buffer saturado";}
    cbuff[xbuff++] = c;
}

void xbee::inicbuff()
{
    for (int i = 0; i < 512; i++){ cbuff[i] = 0x00; } xbuff = 0x00;
}

void xbee::setXbeeEnable(bool value)
{
    if(value != xbeeEnabled){
        xbeeEnabled = value;
    }
}

bool xbee::isXbeeEnabled()
{
    return xbeeEnabled;
}

QByteArray xbee::xBeeCmd(QString dataIn)
{
    QByteArray dataOut;
    for (int i = 0; i < dataIn.length(); i++) {
        dataOut.append(dataIn.at(i));
    }
    return dataOut;
}

void xbee::start()
{
    open();
    qDebug() << "xbee iniciado";
}

void xbee::open()
{
    if(isXbeeEnabled()){
        foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
            qDebug() << serialPortInfo.portName();
            if(serialPortInfo.portName() == portName){
                xbeeAvailable = true;
            }
        }
        if(xbeeAvailable){
            // open and configure the serialport
            port->setPortName(portName);
            port->setBaudRate(baudRate);
            port->setDataBits(static_cast<QSerialPort::DataBits>(bitData));
            port->setParity(static_cast<QSerialPort::Parity>(parity));
            port->setStopBits(static_cast<QSerialPort::StopBits>(stopBit));
            port->setFlowControl(QSerialPort::NoFlowControl);
            port->open(QSerialPort::ReadWrite);
            qDebug() << "COM2" << portName << baudRate << bitData  << stopBit << parity;
            connect(port,&QSerialPort::readyRead,this,&xbee::readData);
            connect(port,&QSerialPort::errorOccurred,this,&xbee::errorOcurred);
        }
    }
}

void xbee::close()
{
    port->close();
    disconnect(port,&QSerialPort::readyRead,this,&xbee::readData);
    disconnect(port,&QSerialPort::errorOccurred,this,&xbee::errorOcurred);
}

void xbee::errorOcurred(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError)
    {
        qDebug() << "Esto es el debug: " << port->errorString();
        emit errordetected(port->errorString());
    }
}

void xbee::readData()
{
    //Debug() << "llego un dato";
    timerProcess->start(250);
    QChar dato = 0x00;
    while(port->bytesAvailable()){
        dato = port->read(1).at(0); addcbuff(dato);
    }
}

void xbee::enviardato()
{
    qDebug() << "enviando dato: +++";
    port->putChar('+');port->putChar('+');port->putChar('+');
}

void xbee::cmdProcess()
{ 
    emit xbeeStatus(xBeeCmd(QString(cbuff)));
    timerProcess->stop();
    inicbuff();
}
