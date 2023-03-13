#include "requesthandler.h"

RequestHandler::RequestHandler()
{

}

QString RequestHandler::parseRequest(const QHttpServerRequest &request)
{
    QJsonObject requestBody=QJsonDocument::fromJson(request.body()).object();
    return requestBody["userNickName"].toString();
}

bool RequestHandler::checkRequest(const QHttpServerRequest &request)
{
    return true;
}
