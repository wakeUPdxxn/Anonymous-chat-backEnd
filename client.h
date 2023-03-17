#pragma once
#include <QObject>
#include <QWebSocket>
#include <QPair>

class Client
{
public:
    explicit Client(const QWebSocket *clientSock);
    ~Client();
    QString getNick();
    QWebSocket* getSocket();
    QHostAddress getAddress();
    void setNick(const QString& newNick);
    void setCompanion(const QString& companionNick,const QWebSocket* companionSocket);
    void setPosInQueue(const size_t pos);
    size_t getPosInQueue();
    bool isHasCompanion();
private:
    QPair<QString,QWebSocket*>companion;
    QHostAddress clientAddress;
    QString clientNickName;
    QWebSocket *clientSocket;
    size_t posInQueue;
};


