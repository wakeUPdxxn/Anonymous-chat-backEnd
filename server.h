#pragma once
#include "client.h"
#include <QNetworkAccessManager>
#include <QWebSocket>
#include <QHttpServer>
#include <QWebSocketServer>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonArray>
#include <QRandomGenerator>
#include <QtConcurrent>
#include <requesthandler.h>

using CompanionPos = QList<Client*>::Iterator;

class Server:public QWebSocketServer
{
public:
    explicit Server(QObject *parent = nullptr);
    CompanionPos findCompanion();
    ~Server();
public slots:
    void onNewClient();
    void disconnectedEvent();
    void textMessageReceived(const QString &message);
private:
    enum apiNums{
        getMembers = 1,
        getCompanion = 2,
        postNick = 3,
        putInQueue = 4,
    };
    Client *сlient;
    QHttpServer *rest;
    QWebSocket *socket;
    QHash<QHostAddress,Client*>clients;
    QVector<QHash<QWebSocket*,QWebSocket*>>currentDialogs;
    QNetworkAccessManager *m_networkAccessManager;
    QList<Client*>freeUsers;
    QList<Client*>beasyUsers;
    size_t membersCounter=0;
    QRandomGenerator *rg;
    QHttpServerResponse makeResponse(qint16 apiNum, QString result);
};

