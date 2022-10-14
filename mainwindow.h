#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void send_joypad_acc(float x, float y);
    void server_New_Connect();
    void socket_Read_Data();
    void socket_Disconnected();
    void on_pushButton_clicked();
    void on_verticalSlider_valueChanged(int value);

private:
    Ui::MainWindow *ui;
    QTcpServer* server;
    QTcpSocket* socket{};
    int heading = 180;
    int pitch = 180; // чтобы задать константное кол-во символов.
    int zoom = 1;
};
#endif // MAINWINDOW_H
