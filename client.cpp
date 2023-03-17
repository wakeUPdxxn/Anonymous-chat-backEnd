#include "client.h"

Client::Client(const QWebSocket* clientSock)
{
    clientSocket=const_cast<QWebSocket*>(clientSock);
    clientAddress=clientSock->peerAddress();
}

Client::~Client(){
    clientSocket->deleteLater();
}

QString Client::getNick(){ return clientNickName; }

QWebSocket *Client::getSocket(){ return clientSocket; }

QHostAddress Client::getAddress(){ return clientAddress; }

void Client::setNick(const QString &newNick){ clientNickName=newNick; }

void Client::setCompanion(const QString &companionNick, const QWebSocket *companionSocket)
{
    companion.first=companionNick;
    companion.second=const_cast<QWebSocket*>(companionSocket);
}

void Client::setPosInQueue(const size_t pos){ posInQueue=pos; }

size_t Client::getPosInQueue() { return posInQueue; }

bool Client::isHasCompanion()
{
    if(companion.first != "") return true;
    else return false;
}
