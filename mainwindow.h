#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QLabel>
#include <server.h>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonArray>
#include <antena.h>
#include <xbee.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    void setMangueraEnable(QLabel *manguera, bool value, QString c, int index);
    void setMangueraValue(QLabel *manguera, bool value);
    void setLadoAManguera(int index, bool value);
    bool getLadoAManguera(int index);
    void setLadoBManguera(int index, bool value);
    bool getLadoBManguera(int index);
    void defaultValues();
    bool tokenIsValid(QString token);
    void openDatabase(QSqlDatabase db);
    void getSensorStatus(QByteArray message);
    int  getClient();

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:


    void messageReceived(QByteArray message);
    void getTagsAntena(QStringList tags);
    void on_pushButton_clicked();
    void clientToAttend(int client);

private:
    Ui::MainWindow *ui;

    bool antenaAEnable = true;
    bool antenaBEnable = true;
    bool xBeeEnable = true;

    bool antenaAAvailable;
    bool antenaBAvailable;
    bool xBeeAvailable;

    bool ladoA_manguera[4]; // esto es el valor de la manguera
    bool ladoB_manguera[4];

    QString m_token[5];
    bool token_valido;
    int m_client;
    Server *m_server;
    antena *ladoA;
    xbee *xbees;

    QSqlDatabase DB;
    QString dataBaseDriver = "QMYSQL";
    QString dataBaseHost = "localhost";
    QString dataBaseName = "";
    QString dataBaseUser = "";
    QString dataBasePwd = "";
    int dataBasePort = 3306;
};

#endif // MAINWINDOW_H
