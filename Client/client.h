#ifndef CLIENT_H
#define CLIENT_H

#include <QtCore>
#include <QtNetwork>
#include "audiooutput.h"
#include "../Server/sclient.h"
#include <QStringList>


class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QString host, quint16 port, QObject *parent = 0);

    bool    connectToSrv(QString host, quint16 port);

signals:
    void    succLogin();
    void    unSuccLogin();
    void    succReg();
    void    unSuccReg();
    void    isBanned();
    void    newMessage(QString uname, QString msg, QString clr);
    void    nUser(QString uname);
    void    dUser(QString uname);
    void    userList(QStringList usrs);
    void    disc();

public slots:
    void    login(QString login, QString password);
    void    reg(QString login, QString password);
    void    voiceSay(QByteArray data);
    void    readVoice();
    void    stringParser(QString str);

private slots:
    void    readyRead();
    void    onDisconnect();

private:

    template <class dataBlock>
    void    sendBlock(quint8 command, dataBlock data);

    void    addVoiceSock();

    QTcpSocket  *socket;
    QTcpSocket  *voiceSock;
    AudioOutput output;
    quint16     blockSize;
    bool        isLoggedIn;

    QString     cHost;
    quint16     cPort;

    QString     userName;
};

#endif // CLIENT_H
