#include "client.h"

Client::Client(QWebSocket *clientSock)
{
    clientSocket=clientSock;
    clientAddress=clientSock->peerAddress();
    companion=nullptr;
}

Client::~Client()
{
    clientSocket->deleteLater();
}
size_t Client::getPosInQueue() { return posInQueue; }

QString Client::getNick(){ return clientNickName; }

QWebSocket *Client::getSocket(){ return clientSocket; }

QHostAddress Client::getAddress(){ return clientAddress; }

void Client::setNick(const QString &newNick){ clientNickName=newNick; }

void Client::setCompanion(Client *currentCompanion){ this->companion=currentCompanion; }

void Client::setPosInQueue(const size_t pos){ posInQueue=pos; }

bool Client::isHasCompanion()
{
    if(companion==nullptr) return false;
    else return true;
}
