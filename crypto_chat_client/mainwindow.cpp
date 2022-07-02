#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    socket = new QTcpSocket(this);
    username = "";
    ui->lineEdit_3->setVisible(false);
    ui->pushButton_3->setVisible(false);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    ui->lineEdit_4->setVisible(false);
    ui->lineEdit_5->setVisible(false);
    ui->label_2->setVisible(false);
    ui->pushButton_4->setVisible(false);
    ui->lineEdit_6->setVisible(false);
    ui->pushButton_5->setVisible(false);
    ui->label_4->setVisible(false);
    ui->label_5->setVisible(false);
    ui->label_6->setVisible(false);
    ui->label_7->setVisible(false);
    ui->lineEdit_8->setVisible(false);
    ui->pushButton_7->setVisible(false);
    ui->label_8->setVisible(false);
    ui->pushButton_8->setVisible(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    if(ui->lineEdit_2->text() != ""){
        ui->infoLabel->clear();
        socket->connectToHost(ui->lineEdit_7->text(), 2323);
        QString str = ui->lineEdit_2->text();
        if(str!="DEFAULT" && str != ""){
                username = str;
            str.prepend("USIN");
            SendToServer(str);
            ui->pushButton->setVisible(false);
            ui->lineEdit_7->setVisible(false);
            ui->lineEdit_2->setVisible(false);
            ui->lineEdit_3->setVisible(true);
            ui->pushButton_3->setVisible(true);
        }else{

        }
    }
}

void MainWindow::SendToServer(QString str)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly );
    out.setVersion(QDataStream::Qt_6_2);
    out<<str;
    socket->write(Data);
}

void MainWindow::slotReadyRead()
{
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_2);
    if(in.status() == QDataStream::Ok){
        QString str, kek;
        while( in.atEnd() == false  ){
            in >> str;
            if(str == "CHOOSE ANOTHER USERNAME"){
                ui->infoLabel->setText("choose another username");
                ui->pushButton->setVisible(true);
                ui->lineEdit_2->setVisible(true);
                ui->lineEdit_3->setVisible(false);
                ui->pushButton_3->setVisible(false);
                username = "";


            }else if( str.contains("CONNECTION OK" )){
                str.remove(0,14);
                if(str != username)
                    to = str;
                ui->pushButton->setVisible(false);
                ui->lineEdit_2->setVisible(false);
                ui->lineEdit_3->setVisible(false);
                ui->pushButton_3->setVisible(false);
            }else if(str.indexOf("HANDOK", 0) == 0){
                ui->label->setText(username);
            }else if(str == "READYTOREADKEYS"){
                ui->lineEdit_4->setVisible(true);
                ui->lineEdit_5->setVisible(true);
                ui->label_2->setVisible(true);
                ui->lineEdit_6->setVisible(true);
                ui->pushButton_4->setVisible(true);
                ui->lineEdit->setVisible(false);
                ui->pushButton_5->setVisible(true);
                ui->pushButton_2->setVisible(false);
                ui->label_4->setVisible(true);
                ui->label_5->setVisible(true);
                ui->label_6->setVisible(true);

            }else if( str.indexOf("A: ", 0) == 0 ){
                ui->label_7->setVisible(true);
                ui->label_8->setVisible(true);
                ui->lineEdit_8->setVisible(true);
                ui->pushButton_7->setVisible(true);
                ui->pushButton_8->setVisible(true);
                str.remove(0, 3);
                A = str.toInt();
                B = -1;
                while(B<0){
                    b = gen();
                    B = openkey(b, g, p);
                }
                ui->lineEdit_8->setText(QString::number(b));
                ui->label_8->setText(QString::number(B));
            }else if( str.indexOf("p: ", 0) == 0 ){
                str.remove(0, 3);
                p = str.toInt();
            }else if( str.indexOf("g: ", 0) == 0 ){
                str.remove(0, 3);
                g = str.toInt();

            }else if( str.indexOf("B: ", 0) == 0 ){
                str.remove(0, 3);
                B = str.toInt();
                K = secretkey(B, a, p);
                ui->label_3->setText(QString::number(K));
            }else if(str == "KEYSOK"){
                ui->lineEdit_4->setVisible(false);
                ui->lineEdit_5->setVisible(false);
                ui->label_2->setVisible(false);
                ui->lineEdit_6->setVisible(false);
                ui->pushButton_4->setVisible(false);
                ui->lineEdit->setVisible(true);
                ui->pushButton_5->setVisible(false);
                ui->pushButton_2->setVisible(true);
                ui->label_4->setVisible(false);
                ui->label_5->setVisible(false);
                ui->label_6->setVisible(false);
                ui->label_7->setVisible(false);
                ui->label_8->setVisible(false);
                ui->lineEdit_8->setVisible(false);
                ui->pushButton_7->setVisible(false);
                ui->pushButton_8->setVisible(false);

            }else if(str.contains("SPECTATINGCONNS")){
                str.remove(0,15);
                QString value = str.mid(0, str.indexOf("->"));
                qDebug()<<value;
                ui->comboBox->addItem(str, value);
            }else if( str == "SPECTATINGSTART" ){
                ui->comboBox->clear();
            }else if(str.contains("SPECTATINGMESSAGE")){
                qDebug()<<str;
                str.remove(0, 17);
                ui->textBrowser_2->append(str);
            }else if(str == "SPECTATINGUPDATE"){
                ui->textBrowser_2->clear();
            }else if(str == "CHOOSE ANOTHER USER"){
                ui->infoLabel->setText("choose another user for connection");
            }else if(str.contains("USERSFORCONN")){
                str.remove(0, 12);
                ui->textBrowser->append(str);
            }
            else
                ui->textBrowser->append(to+": " +str);
        }

    }else{
        ui->textBrowser->append("read error");
    }

}


void MainWindow::on_pushButton_2_clicked()
{
    SendToServer(ui->lineEdit->text());
    ui->textBrowser->append(username + ": " + ui->lineEdit->text());

}


void MainWindow::on_lineEdit_returnPressed()
{
     SendToServer(ui->lineEdit->text());
}


void MainWindow::on_pushButton_3_clicked()
{
    if(ui->lineEdit_3->text() != ""){
        ui->infoLabel->clear();
        QString str = ui->lineEdit_3->text();
        if(str!="DEFAULT" && str != ""){
            to = str;
            str.prepend("USTO");
            SendToServer(str);
        }else{

        }
    }
}


void MainWindow::on_pushButton_4_clicked()
{
    A = -1;
    while(A<0){
        a = gen();
        g = gen();
        p = gen();
        A = openkey(a, g, p);
    }


    ui->lineEdit_4->setText(QString::number(a));
    ui->lineEdit_5->setText(QString::number(p));
    ui->lineEdit_6->setText(QString::number(g));
    a = ui->lineEdit_4->text().toInt();
    p = ui->lineEdit_5->text().toInt();
    g = ui->lineEdit_6->text().toInt();
    A = openkey(a, g, p);
    ui->label_2->setText(QString::number(A));
    //long int gsimple = pow(g, p - 1);
    /*while (gsimple % p != 1)
    {
        gsimple = pow(g, p - 1);
        g++;
        if (g % 2 ==1)
        {
            g++;
        }
    }*/
}

int MainWindow::gen()
{
    int a;
    static time_t tval = time(0);
    tval += 10;
    srand(tval);
    while(1)
    {
        int z=0,kol=0;
        a=1+rand()%100;
        for(int i=2;i<a;i++)
        {if(a%i==1) continue; if(a%i==0) {z=1; break;}} if(z==0) {break;} else;}
    return a;
}

int MainWindow::openkey(int a, int g, int p)
{

    double A;
    A = pow((double)g, (double)a);
    A = (int)A % p;
    return A;
}

int MainWindow::secretkey(int B, int a, int p)
{
    double K;
    K = pow((double)B, (double)a);
    K = (int)K % p;
    return K;
}


void MainWindow::on_pushButton_5_clicked()
{

    SendToServer("OPENKEYp: " + QString::number(p));
    SendToServer("OPENKEYg: " + QString::number(g));
    SendToServer("OPENKEYA: " + QString::number(A));
}


void MainWindow::on_pushButton_6_clicked()
{
    QString  connection = ui->comboBox->currentData().toString();
    SendToServer("SPECTATING"+connection);
}


void MainWindow::on_pushButton_8_clicked()
{
    b = ui->lineEdit_8->text().toInt();
    B = openkey(b, g, p);
    K = secretkey(A, b, p);
    ui->label_3->setText(QString::number(K));
    SendToServer("OPENKEYB: " + QString::number(B));
    SendToServer("OPENKEYKEYSOK");
    ui->label_7->setVisible(false);
    ui->lineEdit_8->setVisible(false);
    ui->pushButton_7->setVisible(false);
    ui->label_8->setVisible(false);
    ui->pushButton_8->setVisible(false);
}


void MainWindow::on_pushButton_7_clicked()
{
    b = gen();
    ui->lineEdit_8->setText(QString::number(b));
    B = openkey(b, g, p);
    ui->label_8->setText(QString::number(B));
}

