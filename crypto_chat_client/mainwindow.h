#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <cstdlib>
#include <stdlib.h>
#include <ctime>
#include <math.h>

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
    void on_pushButton_clicked();
    void SendToServer(QString str);

    void on_pushButton_2_clicked();

    void on_lineEdit_returnPressed();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

private:
    int gen();
    int openkey(int a, int g, int p);
    int secretkey(int B, int a, int p);
    int a, b, g, p, A, B, K;
    Ui::MainWindow *ui;
    QTcpSocket *socket;
    QByteArray Data;
    QString username, to;
public slots:
    void slotReadyRead();
};
#endif // MAINWINDOW_H
