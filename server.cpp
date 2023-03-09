#include "server.h"
#include <requesthandler.h>
Server::Server(QObject *parent)
    :QWebSocketServer(QString("Anonymous-chat-server#1"),SslMode::NonSecureMode,parent)
{
    RequestHandler m_requestHandler;
    rest=new QHttpServer();
    rest->listen(QHostAddress::Any,3232);

    rest->route("/api/members",QHttpServerRequest::Method::Get,[this, &m_requestHandler](const QHttpServerRequest &request){
        return QtConcurrent::run([this,&m_requestHandler, &request] () {
            if(m_requestHandler.checkRequest(request)){
                return makeResponse(apiNums::getMembers,QString("Ok"));
            }
            else{
                return makeResponse(apiNums::getMembers,QString("Error"));
            }
        });
    });

    rest->route("/api/setNick",QHttpServerRequest::Method::Post,[this,&m_requestHandler](const QHttpServerRequest &request){
        return QtConcurrent::run([this,&m_requestHandler, &request] () {
            if(m_requestHandler.checkRequest(request)){
                QString currentNick=m_requestHandler.parseRequest(request);
                for(auto it=clients.begin();it!=clients.end();++it){
                    if(it.value()==currentNick){
                        return makeResponse(apiNums::getMembers,QString("Error"));
                    }
                }
                clients.insert(request.remoteAddress(),currentNick);
                return makeResponse(apiNums::getMembers,QString("Ok"));
            }
            else{
                return makeResponse(apiNums::getMembers,QString("Error"));
            }
        });
    });

    /*rest->route("api/getCompanion",[this](QWebSocket sender){
        auto companion=findCompanion();
        if(companion!=clients.end()){
            QHttpServerResponse response(companion.value(),QHttpServerResponse::StatusCode::Ok);
            response.setHeader("Access-Control-Allow-Origin","*");
            return response;
        }
        else{
            QHttpServerResponse response(QString("NoCompanionFound"),QHttpServerResponse::StatusCode::Ok);
            response.setHeader("Access-Control-Allow-Origin","*");
            return response;
        }
    });*/

    if(this->listen(QHostAddress::Any,2323)){
        qDebug() << "Server started";
    }
    else{
        qDebug() << "Error occurred while starting";
    }
    connect(this,&Server::newConnection,this,&Server::newClient);
}

companionPos Server::findCompanion()
{
    QRandomGenerator *rg = QRandomGenerator::global();
    if(freeUsers.size()!=0){
        qint64 clientNum = rg->bounded(0, freeUsers.size());
        return std::next(clients.begin(),clientNum);
    }
    else{
        return clients.end();
    }
}

void Server::newClient(){
    socket = new QWebSocket;
    socket = this->nextPendingConnection();
    connect(socket,&QWebSocket::textMessageReceived,this,&Server::textMessageReceived);
    connect(socket,&QWebSocket::disconnected,this,&Server::disconnectedEvent);
    connect(socket,&QWebSocket::disconnected,socket,&QWebSocket::deleteLater); //Очистка сокета при получении сигнала об отключении клиента

    QString userNick="";
    clientsNetworkData.insert(socket->peerAddress(),socket);
    clients.insert(socket->peerAddress(),userNick);
    qDebug() << "Client connected" << socket->peerAddress();
    ++membersCounter;
}

void Server::textMessageReceived(const QString &message)
{
    socket=(QWebSocket*)sender();
    qDebug() << "Message from" << socket->peerAddress() << message;
}

QHttpServerResponse Server::makeResponse(qint16 apiNum,QString result)
{
    if(apiNum==apiNums::getMembers){
        if(result=="Ok"){
            QHttpServerResponse response(QString(QString::number(membersCounter)),QHttpServerResponse::StatusCode::Ok);
            response.setHeader("Access-Control-Allow-Origin","*");
            return response;
        }
        else{
            QHttpServerResponse response(QString("Error"),QHttpServerResponse::StatusCode::BadRequest);
            response.setHeader("Access-Control-Allow-Origin","*");
            return response;
        }
    }
    if(apiNum==apiNums::postNick){
        if(result=="Ok"){
            QHttpServerResponse response(QString("Success"),QHttpServerResponse::StatusCode::Ok);
            response.setHeader("Access-Control-Allow-Origin","*");
            return response;
        }
        else{
            QHttpServerResponse response(QString("Error"),QHttpServerResponse::StatusCode::BadRequest);
            response.setHeader("Access-Control-Allow-Origin","*");
            return response;
        }
    }
}

void Server::disconnectedEvent()
{
    QWebSocket *clientSock= qobject_cast<QWebSocket *>(sender());
    if (clientSock==NULL) {
        return;
    }
    qDebug() << "Client disconnected" << clientSock->peerAddress();
     --membersCounter;
}
