     #include "mainwindow.h"
#include "ui_mainwindow.h"

Server::Server(){
    if(this->listen(QHostAddress::Any, 2323)){
        qDebug()<<"start";
    }else{
        qDebug()<<"error";
    }
    ProxyQueue = new QQueue<QString>;
}

void Server::SendToConcreteClientProxyMessage(QString username, QString str)
{
    QString to = Chats[username];
    //str.prepend(username+": ");
    WriteToDatabase(str, username, to);
    SendToOneClient(to, str);
    emit proxyUpd();
}

void Server::incomingConnection(qintptr socketDescriptor){
    socket = new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);
    connect(socket, &QTcpSocket::readyRead, this, &Server::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    //Sockets.push_back(socket);
    qDebug()<<"client connected!"<<socketDescriptor;
}

void Server::slotReadyRead(){
    socket = (QTcpSocket*)sender();
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_2);
    if(in.status() == QDataStream::Ok){
        qDebug()<<"read";
        QString str;
        while( in.atEnd() == false ){
            in>>str;
            if(str.indexOf("USIN", 0) == 0){
                str.remove(0, 4);
                if(str != "" && !SocketsMap.contains(socket)  && !ClientsMap.contains(str)){
                    SocketsMap[socket] = str;
                    ClientsMap[str] = socket;
                    Sockets.append(socket);
                    SendToConcreteClient(str,"HANDOK");
                    qDebug()<<Sockets.size();
                    for(int i = 0; i<Sockets.size(); ++i){

                        if(!Chats.contains(SocketsMap[Sockets[i]]) && SocketsMap[Sockets[i]] != SocketsMap[socket]){
                            SendToOneClient(str, "USERSFORCONN"+SocketsMap[Sockets[i]] );
                        }
                    }
                    qDebug()<<"OK";

                }else{
                    SendToSocketClient(socket, "CHOOSE ANOTHER USERNAME");
                }


            } else if(str.indexOf("USTO", 0) == 0){
                str.remove(0, 4);
                if( ClientsMap.contains(str) && SocketsMap.contains(socket) && !Chats.contains(str) && SocketsMap.value(socket, "") != str){
                    Chats[str] = SocketsMap[socket];
                    Chats[SocketsMap[socket]] = str;
                    DataBase[str+SocketsMap[socket]] = new QList<QString>;

                    SendToConcreteClient(str, "CONNECTION OK "+SocketsMap[socket]);
                    SendToOneClient(SocketsMap[socket], "READYTOREADKEYS");
                    emit needUpdate();
                    qDebug()<<"CONNECTED";

                }else{
                    SendToOneClient(SocketsMap.value(socket, ""),"CHOOSE ANOTHER USER");
                }
            }else if(str.contains("SPECTATING")){
                str.remove(0,10);
                SpectatorsMap[socket].append(ClientsMap[str]);
                QString another = Chats[str];
                SendToOneClient(SocketsMap[socket], "SPECTATINGUPDATE");
                foreach(QString str, *getList(str, another)){
                    qDebug()<<str;
                    SendToOneClient(SocketsMap[socket], "SPECTATINGMESSAGE" + str);
                }
            }

            else if(SocketsMap.value(socket, "")!="" && Chats.value(SocketsMap.value(socket, ""), "") != ""){
                    QString from = SocketsMap.value(socket, "");
                    QString to = Chats[from];
                    QString res = str;
                    //res.prepend(from+": ");
                    if(str.contains("OPENKEY")){
                        str.remove(0, 7);
                        res = str;
                    }
                    if( CurConnection(from, to) == CurProxy ){
                        ProxyQueue->enqueue(res);
                        if( in.atEnd() == true ){
                            emit proxyUpd();
                            emit ProxyEmitter(from, ProxyQueue->dequeue());
                        }

                    }else{
                       WriteToDatabase(res, from, to);

                        SendToOneClient(to, res);
                    }



                }
        }


    }else{
        qDebug()<<"DataStream error";
    }

}

void Server::SendToClient(QString str){
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly );
    out.setVersion(QDataStream::Qt_6_2);
    out<<str;
    for(int i = 0; i<Sockets.size(); ++i){
        Sockets[i]->write(Data);
    }

}


QString Server::CurConnection(QString from, QString to)
{
    QString res = "";
    if( DataBase.value(from+to, nullptr) != nullptr)
        res = from+to;
    else if(DataBase.value(to+from, nullptr) != nullptr )
        res = to+from;
    return res;
}

void Server::SendToConcreteClient(QString username, QString str)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly );
    out.setVersion(QDataStream::Qt_6_2);
    out<<str;
    if(ClientsMap.value(username, nullptr) != nullptr)
        ClientsMap[username]->write(Data);
    if(ClientsMap.value(Chats[username], nullptr) != nullptr)
        ClientsMap[Chats[username]]->write(Data);
}

void Server::SendToOneClient(QString username, QString str)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly );
    out.setVersion(QDataStream::Qt_6_2);
    out<<str;
    if(ClientsMap.value(username, nullptr) != nullptr){
        ClientsMap[username]->write(Data);
        qDebug()<<str;
    }
}

void Server::SendToSocketClient(QTcpSocket *sock, QString str)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly );
    out.setVersion(QDataStream::Qt_6_2);
    out<<str;
    sock->write(Data);
}



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    first = "";
    second = "";
    s = new Server();
    connect(s, SIGNAL(needUpdate()), this, SLOT(upd()));
    connect(s, SIGNAL(proxyUpd()), this, SLOT(updProxy()));
    connect(s, SIGNAL(ProxyEmitter(QString, QString)), this, SLOT(ProxyEmitted(QString, QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ProxyEmitted(QString from, QString str)
{
    //QString from = str.mid(0, str.indexOf(": "));
    //QString message = str.mid(str.indexOf(": ")+2, str.size()-str.indexOf(": ")-2);
    fr = from;
    if( !s->getCommands()->contains( str)){
        ui->lineEdit->clear();
        ui->lineEdit->setText(str);
        ui->label->setText(fr);

    }else{
        s->SendToConcreteClientProxyMessage(fr, str);
    }
}

void MainWindow::updProxy()
{
    if(first != "" && second != ""){
        ui->textBrowser->clear();
        QList<QString>* curList = s->getList(first,second);
        foreach(QString str, *curList){
            ui->textBrowser->append(str);
        }
    }
    //for (i = s->getList(currentProxy)->begin(); i != s->getList(currentProxy)->end(); ++i)
      //  ui->textBrowser->append(*i);
}

void MainWindow::upd()
{
    ui->comboBox->clear();
    QMap<QString, QString>::iterator i;
    s->SendToClient("SPECTATINGSTART");
    for (i = s->getChats()->begin(); i != s->getChats()->end(); i+=2){
        ui->comboBox->addItem(i.key()+"->"+i.value(), i.key());
        s->SendToClient("SPECTATINGCONNS" + i.key()+"->"+i.value());
        qDebug()<<i.key()+"->"+i.value();
    }
}


void MainWindow::on_pushButton_clicked()
{
    while(!s->get_queue()->isEmpty()){
        s->SendToConcreteClientProxyMessage(fr, s->get_queue()->dequeue());

    }

    first = ui->comboBox->currentData().toString();
    second = s->getChats()->value(first);
    s->setCurProxy(first, second);
    updProxy();
}


void MainWindow::on_pushButton_2_clicked()
{

    QString message = ui->lineEdit->text();
    QString from = ui->label->text();
    qDebug() << from;
    qDebug() << message;
    fr = from;
    s->SendToConcreteClientProxyMessage(from, message);
    ui->lineEdit->clear();
    if(!s->get_queue()->isEmpty())
        ProxyEmitted(from, s->get_queue()->dequeue());

}


void MainWindow::on_checkBox_stateChanged(int arg1)
{

}


