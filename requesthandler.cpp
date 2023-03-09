#include "requesthandler.h"

RequestHandler::RequestHandler()
{

}

QString RequestHandler::parseRequest(const QHttpServerRequest &request)
{
    QJsonObject bodyObj=QJsonDocument::fromJson(request.body()).object();
    return bodyObj["nickName"].toString();
}

bool RequestHandler::checkRequest(const QHttpServerRequest &request)
{
    return true;
}
