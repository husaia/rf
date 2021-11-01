#ifndef XBEE_H
#define XBEE_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include <QDebug>

class xbee : public QObject
{
    Q_OBJECT
public:
    explicit xbee(QObject *parent = nullptr,QString portName = "ttyAMA0",
                  int baudRate = 9600, int bitData = 8,
                  int stopBit = 1,int parity = 0);

    void addcbuff(QChar c);
    void inicbuff();
    void setXbeeEnable(bool value);
    bool isXbeeEnabled();
    QByteArray xBeeCmd(QString dataIn);

signals:
    void xbeeStatus(QByteArray xbee);
    void errordetected(QString error);
public slots:

    void start();
    void open();
    void close();
    void errorOcurred(QSerialPort::SerialPortError error);
    void readData();
    void enviardato();
    void cmdProcess();
private:

    QTimer *timerProcess;
    QSerialPort *port;
    QString portName;
    int baudRate;
    int bitData;
    int stopBit;
    int parity;
    bool xbeeEnabled = true;
    bool xbeeAvailable;

    QString xBeeDataIn;
    int xbuff = 0x00;                      // Índice: siguiente CHAR en cbuff
    QChar cbuff[512];                      // Buffer
};

#endif // XBEE_H
