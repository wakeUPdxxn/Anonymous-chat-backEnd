#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QWebSocket>
#include <QHttpServer>
#include <QWebSocketServer>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonArray>
#include <QRandomGenerator>
#include <QtConcurrent>

using companionPos = QHash<QHostAddress,QString>::Iterator;

class Server:public QWebSocketServer
{
public:
    explicit Server(QObject *parent = nullptr);
    companionPos findCompanion();
public slots:
    void newClient();
    void disconnectedEvent();
    void textMessageReceived(const QString &message);
private:
    enum apiNums{
        getMembers = 1,
        getCompanion = 2,
        postNick = 3,
    };
    QHttpServer *rest;
    QWebSocket *socket;
    QHash<QHostAddress,QWebSocket*> clientsNetworkData;
    QHash<QHostAddress,QString>clients;
    QNetworkAccessManager *m_networkAccessManager;
    QSet<QString>freeUsers;
    size_t membersCounter=0;
    QHttpServerResponse makeResponse(qint16 apiNum, QString result);
};

