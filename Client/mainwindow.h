#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QString>
#include <QLineEdit>
#include <QListWidget>
#include "audioinput.h"
#include "audiooutput.h"
#include "logindialog.h"
#include <QTextBrowser>
#include <exception>
#include <QException>
class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Client *client;
    AudioInput *input;
    QWidget *cw;
    QListWidget *userWidget;
    QHBoxLayout *ml;
    QGridLayout *mlay;
    LoginDialog *ldialog;
    QTextBrowser *chatWidget;

    QLineEdit *chatLine;
    QPushButton *chatBut;


public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void sendMessage();
public slots:
    void succLogin();
    void newMessage(QString username, QString message, QString col);
    void newUser(QString username);
    void userDisc(QString username);
    void drawUserList(QStringList ulist);
    //void error(QString text);
protected:
};

#endif // MAINWINDOW_H
