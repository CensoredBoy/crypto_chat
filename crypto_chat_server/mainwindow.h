#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QQueue>
#include <QSet>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Server : public QTcpServer{
    Q_OBJECT
public:
    Server();
    QTcpSocket *socket;
    QMap<QString, QString>* getChats(){
        return &Chats;
    }
    QList<QString>* getList( QString first, QString second ){
        QString pair = "";
        if( DataBase.value(first+second, nullptr) != nullptr){
            pair = first+second;
        }
        else if(DataBase.value(second+first, nullptr) != nullptr ){
            pair = second+first;
        }
        return DataBase[pair];
    }
    void SendToConcreteClientProxyMessage( QString username, QString str );
    QQueue<QString>* get_queue(){
        return ProxyQueue;
    }
    void SendToClient(QString str);
    QSet<QString>* getCommands(){
     return &CommonCommands;
    }
private:
    QVector <QTcpSocket*> Sockets;
    QQueue<QString>* ProxyQueue;
    QMap<QTcpSocket*,QString> SocketsMap;
    QMap<QString, QTcpSocket*> ClientsMap;
    QMap<QString, QString> Chats;
    QMap<QString, QList<QString>*> DataBase;
    QMap<QTcpSocket*, QList<QTcpSocket*>> SpectatorsMap;
    QString currentProxyPair;
    QSet<QString> CommonCommands = {"SPECTATINGSTART", "SPECTATING", "READYTOREADKEYS", "KEYSOK"};
    QByteArray Data;

    //void SendToSpectators(QTcpSocket* sock);
    QString CurConnection(QString from, QString to);
    QString CurProxy;
    void SendToConcreteClient(QString username, QString str);
    void SendToOneClient(QString username, QString str);
    void SendToSocketClient(QTcpSocket* sock, QString str);
    void WriteToDatabase(QString str, QString from, QString to){
        QString con  = CurConnection(from, to);
        if(!CommonCommands.contains(str))
            DataBase[con]->push_back(from+" sended: " + str);
    }
public slots:
    void incomingConnection(qintptr socketDescriptor);
    void slotReadyRead();
    void setCurProxy( QString from, QString to ){
        CurProxy = CurConnection(from, to);
    }
signals:
    void needUpdate();
    void proxyUpd();
    void ProxyEmitter(QString from, QString str);


};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private:
    QString fr;
    Server *s;
    QString first, second;
    Ui::MainWindow *ui;
public slots:
    void ProxyEmitted(QString from, QString str);
    void upd();
private slots:
    void on_pushButton_clicked();
    void updProxy();
    void on_pushButton_2_clicked();
    void on_checkBox_stateChanged(int arg1);
};
#endif // MAINWINDOW_H
