#ifndef ANTENA_H
#define ANTENA_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include <QDebug>

class antena : public QObject
{
    Q_OBJECT
public:
    explicit antena(QObject *parent = nullptr,QString portName = "ttyAMA0",
                    int baudRate = 57600, int bitData = 8,
                    int stopBit = 1,int parity = 0);

     void addcbuff(QChar c);
     void inicbuff();
     void setAntenaEnable(bool value);
     bool isAntenaEnabled();
 signals:
     void tagsAvailable(QStringList tags);
     void errordetected(QString error);
 public slots:
     void start();
     void open();
     void close();
     void errorOcurred(QSerialPort::SerialPortError error);
     void readData();

     void cmdProcess();
     void cmdSend();

 private:
     QTimer *timerProcess;
     QTimer *timerPort;
     QSerialPort *port;
     QString portName;
     int baudRate;
     int bitData;
     int stopBit;
     int parity;
     bool antenaEnabled = true;
     bool antenaAvailable;

     int xbuff = 0x00;                      // Índice: siguiente CHAR en cbuff
     QChar cbuff[512];                      // Buffer

     int dataInStart = 0;
     int consoleResponse = 0;
     int tagFound = 0;
     QString tags[20];
};

#endif // ANTENA_H
