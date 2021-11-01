#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QRandomGenerator>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_server = new Server(this);
    ladoA = new antena(this,"ttyAMA1");
    xbees = new xbee(this,"ttyAMA3");
    xbees->start();
    m_server->start();
    ladoA->start();
    connect(m_server,&Server::messageReceived,this,&MainWindow::messageReceived);
    connect(m_server,&Server::clientToAttend,this,&MainWindow::clientToAttend);
    connect(ladoA,&antena::tagsAvailable,this,&MainWindow::getTagsAntena);
    connect(xbees,&xbee::xbeeStatus,this,&MainWindow::getSensorStatus);
    defaultValues();

    setMangueraValue(ui->SideA_manguera1,false);
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::messageReceived(QByteArray message)
{
    qDebug() << message;
    QJsonDocument doc;
    QJsonObject obj;
    doc = doc.fromJson(message);
    obj = doc.object();
    QString token = obj["token"].toString();
    QString cmd = obj["cmd"].toString();
    if(tokenIsValid(token)){
        qDebug() << "Token valido" << "token: " << token;
        m_server->writeData("hola cliente "+token.toUtf8()+"como estas",getClient());
    }else {
        qDebug() << "Token no valido";
        m_server->writeData("cliente token no valido",getClient());
        m_server->disconnectFromHost(getClient());

    }
}

void MainWindow::getTagsAntena(QStringList tags)
{
    int tagcount = tags.length();
    ui->TagsSideA->clear();
    for (int i = 0; i < tagcount; i++) {
            ui->TagsSideA->insertPlainText(tags[i] +"\r\n");
    }
}

void MainWindow::setMangueraEnable(QLabel *manguera, bool value, QString lado, int index)
{
    if(value){
        if(lado == "A"){
            setMangueraValue(manguera,getLadoAManguera(index));
        }else {
            setMangueraValue(manguera,getLadoBManguera(index));
        }
    }else {
        manguera->setStyleSheet("QLabel{background-color: gray; border-radius:10;}");
    }
    manguera->setEnabled(value);
}



void MainWindow::setMangueraValue(QLabel *manguera, bool value)
{   if(manguera->isEnabled()){
        if(value){manguera->setStyleSheet("QLabel{background-color: green; border-radius:10;}");}
        else {    manguera->setStyleSheet("QLabel{background-color: blue; border-radius:10;}");}
    }else {
        manguera->setStyleSheet("QLabel{background-color: gray; border-radius:10;}");
    }
}

void MainWindow::setLadoAManguera(int index, bool value)
{
    ladoA_manguera[index] = value;
    switch(index){
    case 1: setMangueraValue(ui->SideA_manguera1,value);break;
    case 2: setMangueraValue(ui->SideA_manguera2,value);break;
    case 3: setMangueraValue(ui->SideA_manguera3,value);break;
    case 4: setMangueraValue(ui->SideA_manguera4,value);break;
    }
}

bool MainWindow::getLadoAManguera(int index)
{
    return  ladoA_manguera[index];
}

void MainWindow::setLadoBManguera(int index, bool value)
{
    ladoB_manguera[index] = value;
    switch(index){
    case 1: setMangueraValue(ui->SideB_manguera1,value);break;
    case 2: setMangueraValue(ui->SideB_manguera2,value);break;
    case 3: setMangueraValue(ui->SideB_manguera3,value);break;
    case 4: setMangueraValue(ui->SideB_manguera4,value);break;
    }
}

bool MainWindow::getLadoBManguera(int index)
{
    return  ladoB_manguera[index];
}

void MainWindow::defaultValues()
{
    setMangueraEnable(ui->SideA_manguera1,true,"A",1);
    setMangueraEnable(ui->SideA_manguera2,false,"A",2);
    setMangueraEnable(ui->SideA_manguera3,false,"A",3);
    setMangueraEnable(ui->SideA_manguera4,false,"A",4);
    setMangueraEnable(ui->SideB_manguera1,false,"B",1);
    setMangueraEnable(ui->SideB_manguera2,false,"B",2);
    setMangueraEnable(ui->SideB_manguera3,false,"B",3);
    setMangueraEnable(ui->SideB_manguera4,false,"B",4);

// Esto se va al momento de que la Db este disponible
    quint64 array1[5]; //
    quint32 array2[5]; //
    QRandomGenerator::global()->fillRange(array1); //
    QRandomGenerator::global()->fillRange(array2); //

    for (int i = 0; i <= 4; i++) {
        m_token[i].append(QString::number(array1[i]).leftJustified(20,'1'));
        m_token[i].append(QString::number(array2[i]).leftJustified(10,'1'));
        qDebug() << "Token" << i << ": " << m_token[i];
    }
}

bool MainWindow::tokenIsValid(QString token)
{
    bool valido = false;
    for (int i = 0; i <= 4; i++) {
        if(token == m_token[i]){
            valido = true;
            break;
        }else {
            valido = false;
        }
    }
    return valido;
}

void MainWindow::openDatabase(QSqlDatabase db)
{
        db = QSqlDatabase::addDatabase(dataBaseDriver);
        db.setHostName(dataBaseHost);
        db.setDatabaseName(dataBaseName);
        db.setPort(dataBasePort);
        db.setUserName(dataBaseUser);
        db.setPassword(dataBasePwd);
        if(!db.open()){
            //QMessageBox::critical(this, "Error", DB.lastError().text());
            return;
        }
}

void MainWindow::getSensorStatus(QByteArray message)
{
    qDebug() << "recepcion de  xbee: "<< message;
    QJsonDocument doc;
    QJsonObject obj;
    doc = doc.fromJson(message);
    obj = doc.object();
    QString id = obj["id"].toString();
    QString tag = obj["tag "].toString();
    qDebug() << "recepcion de  xbee ID: "<< id << "  tag: " << tag;
    if(id == "41D4C48B\r" && tag != "") setMangueraValue(ui->SideA_manguera1,true);
    else setMangueraValue(ui->SideA_manguera1,false);
    ui->Tag_mangera_ladoA->setText("tag: " + tag);
}


void MainWindow::on_pushButton_clicked()
{
        m_server->showSockets();
        //xbees->enviardato();
}

void MainWindow::clientToAttend(int client)
{
    m_client = client;
}

int MainWindow::getClient()
{
    return m_client;
}
