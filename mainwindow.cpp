#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "math.h"
#include "QDebug"
#include <QPixmap>
#include "joypad.h"
//#include <QJsonObject>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QPixmap pix("/home/tehnokrat/CLionProjects/UAV_Animus/plane_no_transpr.png");
    ui->label->setPixmap(pix);

    server = new QTcpServer();
    connect(server,&QTcpServer::newConnection, this, &MainWindow::server_New_Connect);
    connect(ui->widget, &JoyPad::xChanged, this, [this](float x){
            MainWindow::send_joypad_acc(x, ui->widget->y());
    });
    connect(ui->widget, &JoyPad::yChanged, this, [this](float y){
            MainWindow::send_joypad_acc(ui->widget->x(), y);
    });
}


MainWindow::~MainWindow()
{
    socket->close();
    server->close();
    delete ui;
}


void MainWindow::server_New_Connect(){
    socket = server->nextPendingConnection();
    qDebug()<<"Connected!";
    ui->textBrowser->append("Connected!");
    QObject::connect(socket, &QTcpSocket::readyRead, this, &MainWindow::socket_Read_Data);
    QObject::connect(socket, &QTcpSocket::disconnected, this, &MainWindow::socket_Disconnected);
}


void MainWindow::socket_Disconnected(){
    ui->textBrowser->append("Disconnected!");
    qDebug()<<"Disconnected!";
}


void MainWindow::socket_Read_Data(){
    QByteArray data;
    data = socket->readAll();
    if (data.isEmpty()){
        qDebug() << data;
        ui->textBrowser->append(data);
    }
}


void MainWindow::on_pushButton_clicked()
{
    if(!server->listen(QHostAddress("127.0.0.1"), 12345)) {
        qDebug()<<server->errorString();
        ui->textBrowser->append(server->errorString());
        return;
    }
    ui->textBrowser->append("Start Listening!");
    qDebug("Start Listening!");
}


void MainWindow::on_verticalSlider_valueChanged(int value) // send zoom
{
    zoom = value;
    QString sending = QString::number(heading+180) + QString::number(abs(pitch)+180) + QString::number(zoom) + '\0';

    socket->write(sending.toLocal8Bit());
    if (!socket->flush()){
        ui->textBrowser->append("Something went wrong!");
    }
    qDebug()<<sending;
}


void MainWindow::send_joypad_acc(float x, float y){
    (heading >= 0 && heading <= 360)? heading += 1.3 * x : heading -= 360 * x/abs(x); // if heading in [0, 360] => grow up, else start from 0
    (pitch >= -90 && pitch <= 0)? pitch += 1.3*y : (pitch>=0)? pitch = 0 : pitch = -90;

    QString sending = QString::number(heading+180)+QString::number(abs(pitch)+180)+QString::number(zoom)+'\0';
    qDebug()<<sending;
    socket->write(sending.toLocal8Bit());

    if (socket->flush()){
        ui->view->move((400 + (200 * -sin(heading*3.14/180))), (300 + (200 * cos(heading*3.14/180))));
    }
}
