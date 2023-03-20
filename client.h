#pragma once
#include <QObject>
#include <QWebSocket>

class Client
{
public:
    explicit Client(QWebSocket *clientSock = nullptr);
    ~Client();
    QString getNick();
    QWebSocket* getSocket();
    QHostAddress getAddress();
    void setNick(const QString& newNick);
    void setPosInQueue(const size_t pos);
    void setCompanion(Client *currentCompanion);
    bool isHasCompanion();
    size_t getPosInQueue();
private:
    Client *companion;
    QHostAddress clientAddress;
    QString clientNickName;
    QWebSocket *clientSocket;
    size_t posInQueue;
};


