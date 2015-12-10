#include "client.h"

Client::Client(QObject *parent) : QObject(parent)
{

    socket = new QTcpSocket(this);

    voiceSock = new QTcpSocket(this);

    blockSize = 0;
    isLoggedIn = false;

    connect(socket, SIGNAL(readyRead()),this, SLOT(readyRead()));
    connect(socket,SIGNAL(disconnected()),this,SLOT(onDisconnect()));
    connect(voiceSock, SIGNAL(readyRead()),this,SLOT(readVoice()));

    commandMap["/ban"] = new comm(sClient::c_ban, 2);
    commandMap["/unban"] = new comm(sClient::c_unban, 1);
    commandMap["/mute"] = new comm(sClient::c_mute, 2);
    commandMap["/unmute"] = new comm(sClient::c_unmute, 1);
    commandMap["/kick"] = new comm(sClient::c_kick, 1);
    commandMap["/chperm"] = new comm(sClient::c_chperm, 2);

    smilesMap["cat"] = "<img src=qrc:/smiles/cat_head.png>";
}

void Client::readyRead()
{
    QDataStream in(socket);
    if (blockSize == 0) {
        if (socket->bytesAvailable() < (int)sizeof(quint16))
        {
            return;
        }
        in >> blockSize;
        qDebug() << "block " << blockSize;
        qDebug() << socket->bytesAvailable();
    }
    if (socket->bytesAvailable() < blockSize)
        return;
    else

        blockSize = 0;

    quint8 command;
    command = 0;
    in >> command;
    qDebug() << command;
    switch (command) {
    case sClient::c_SuccLogin:
    {
        emit this->succLogin();
        isLoggedIn = true;
        voiceSock->connectToHost(cHost,cPort);
        addVoiceSock();
        sendBlock(sClient::c_onList,NULL);
        break;
    }
    case sClient::c_unSucc_L:
    {
        emit this->unSuccLogin();
        userName = "NULL";
        break;
    }
    case sClient::c_Succ_Reg:
    {
        emit this->succReg();
        break;
    }
    case sClient::c_unSucc_R:
    {
        emit this->unSuccReg();
        break;
    }
    case sClient::c_message:
    {
        QString unme;
        QStringList mslist;
        in >> unme;
        mslist = unme.split(mssep);
        this->newMessage(mslist.at(0), mslist.at(1), mslist.at(2));
        break;
    }
    case sClient::c_userConn:
    {
        QString uname;
        in >> uname;

        emit this->nUser(uname);
        break;
    }
    case sClient::c_userDisc:
    {
        QString uname;
        in >> uname;

        emit this->dUser(uname);
        break;
    }
    case sClient::c_onList:
    {
        QString usrs;
        in >> usrs;
        qDebug() << usrs;
        QStringList uList;
        uList = usrs.split(",");

        emit this->userList(uList);
        break;
    }
    case sClient::c_ban:
    {
        emit this->isBanned();
        break;
    }
    default:
        break;
    }
}

void Client::onDisconnect()
{
    if(voiceSock->isOpen())
        voiceSock->disconnectFromHost();
    emit this->disc();
}

template <class dataBlock>
void Client::sendBlock(quint8 command, dataBlock data)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << (quint16)0;
    out << command;
    if (data!=NULL){
        out << data;
    }
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    socket->write(block);
}

bool Client::connectToSrv(QString host, quint16 port)
{
    cHost = host;
    cPort = port;
    socket->connectToHost(QHostAddress(host), port);
}

void Client::addVoiceSock()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << (quint16)0;
    out << sClient::c_voice_say;
    out << userName;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    voiceSock->write(block);
}

void Client::login(QString login, QString password)
{
    userName = login;
    QString lp(login);
    lp.append(lpsep);
    lp.append(password);

    sendBlock(sClient::c_login, lp);
}
void Client::reg(QString login, QString password)
{
    QString lp(login);
    lp.append(lpsep);
    lp.append(password);

    sendBlock(sClient::c_reg, lp);
}

void Client::voiceSay(QByteArray data)
{
    if (voiceSock)
        voiceSock->write(data);
}

void Client::readVoice()
{
    QByteArray voice;

    if(voiceSock->bytesAvailable() > 0)
    {
        qDebug() << voiceSock->bytesAvailable();
        voice.append(voiceSock->readAll());
        output.writeData(voice);
    }
}

void Client::handleCommand(QString mess)
{
    QStringList commsyn;
    commsyn = mess.split(" ");
    if(commandMap.contains(commsyn.at(0))){
    QMap<QString, comm*>::iterator i = commandMap.find(commsyn.at(0));
        if(commsyn.length()==i.value()->getCount()+1){
            QString comst;
            for(uint j=1; j<=i.value()->getCount(); j++){
                comst.append(commsyn.at(j));
                comst.append(",");
            }
            comst.remove(comst.length()-1,1);
            qDebug() << comst;
            sendBlock(i.value()->getId(), comst);
        }
    }
}

void Client::replaceSmiles(QString mess)
{
    QString endStr = mess;

    QMapIterator<QString, QString> i(smilesMap);
    while (i.hasNext()){
        i.next();
        endStr.replace(tr("<sm=%1>").arg(i.key()),i.value());
    }
    sendBlock(sClient::c_message, endStr);
}

void Client::stringParser(QString str)
{
    QString endStr = str;
    if(endStr[0]=='/')
    {
        handleCommand(endStr);
    }else{
        replaceSmiles(str);
    }
}
