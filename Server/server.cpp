#include "server.h"

Server::Server(quint16 port, QObject *parent) : QTcpServer(parent)
{
    if (listen(QHostAddress::Any,port))
        qDebug() << "Сервер запущен" << endl;
    else
        qDebug() << "NOt started" << endl;

    db = new database(this);
    if(db->initialize("127.0.0.1","alcpvc","root","alcpvc"))
        qDebug() << "БД подключено";
    else
        qDebug() << "БД не подключено!!!!";

    qDebug() << db->getPower("Alkor");
}

Server::~Server()
{

}
void Server::incomingConnection(qintptr handle)
{
     sClient *client = new sClient(handle, this, this);

     connect(client,SIGNAL(userDisconnected(sClient*)),this,SLOT(onUserDisconnected(sClient*)));

     cliList.append(client);

}
void Server::onUserDisconnected(sClient *client)
{
    cliList.removeAt(cliList.indexOf(client));

    qDebug() << "User " << client->getName() << " disconnected";
}

void Server::sendToAll(quint8 command, QByteArray data, QString senderName, bool exceptSender)
{
    sClient *cli;

    foreach (cli, cliList) {
            if((exceptSender)){
                if(cli->getName()!=senderName)
                    if(cli->getLoggedIn())
                        cli->sendBlock(command, data);
        }else{
            if(cli->getLoggedIn())
                cli->sendBlock(command, data);
        }
    }
}

