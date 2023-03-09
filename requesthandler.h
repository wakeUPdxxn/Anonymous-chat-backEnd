#pragma once
#include <QObject>
#include <QHttpServerRequest>
#include <QJsonDocument>
#include <QJsonObject>

using method=QHttpServerRequest::Method;

class RequestHandler
{
public:
    explicit RequestHandler();
    QString parseRequest(const QHttpServerRequest &request);
    bool checkRequest(const QHttpServerRequest &request);
};

