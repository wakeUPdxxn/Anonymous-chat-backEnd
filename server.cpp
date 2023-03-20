#include "server.h"
#include <QFile>
#include <QSslKey>
#include <QSslCertificate>

Server::Server(QObject *parent)
    :QWebSocketServer(QString("Anonymous-chat-server#1"),SslMode::NonSecureMode,parent)
{
    RequestHandler m_requestHandler;
    rg=QRandomGenerator::global();
    /*QFile keyFile("/home/dxxn/ssl/anonymous-chat-privateKey.key");
    QFile certFile("/home/dxxn/ssl/anonymous-chat-certificate.crt");
    QByteArray key,cert;
    if(!keyFile.open(QIODevice::ReadOnly)){
        qDebug() << "Couldn't Open key File.";
        qDebug() << "Error: " << keyFile.errorString();
    }
    else {
        key = keyFile.readAll();
        keyFile.close();
    }
    if(!certFile.open(QIODevice::ReadOnly)){
        qDebug() << "Couldn't Open certificate File.";
        qDebug() << "Error: " << certFile.errorString();
    }
    else {
        cert = certFile.readAll();
        certFile.close();
    }
    QSslCertificate ssl_cert(cert);
    QSslKey ssl_key(key,QSsl::Rsa,QSsl::Pem,QSsl::PrivateKey,(QByteArray)"");*/

    rest=new QHttpServer();
    //rest->sslSetup(ssl_cert,ssl_key,QSsl::SslProtocol::SecureProtocols);
    rest->listen(QHostAddress::Any,3232);

    rest->route("/api/members",QHttpServerRequest::Method::Get,[this,&m_requestHandler](const QHttpServerRequest &request){
        return QtConcurrent::run([this, &request,&m_requestHandler] () {
            if(m_requestHandler.checkRequest(request)){
                return makeResponse(apiNums::getMembers,QString("true"));
            }
            else{
                return makeResponse(apiNums::getMembers,QString("false"));
            }
        });
    });
    rest->route("/api/setNick",QHttpServerRequest::Method::Post,[this,&m_requestHandler](const QHttpServerRequest &request){
        return QtConcurrent::run([this, &request,&m_requestHandler] () {
            if(m_requestHandler.checkRequest(request)){
                QString currentNick=m_requestHandler.parseRequest(request);
                for(auto client=clients.begin();client!=clients.end();++client){
                    if(client.value()->getNick()==currentNick){
                        return makeResponse(apiNums::postNick,QString("NickAlreadyExist"));
                    }
                }
                clients[request.remoteAddress()]->setNick(currentNick);
                return makeResponse(apiNums::postNick,QString("true"));
            }
            else {
                return makeResponse(apiNums::postNick,QString("false"));
            }
        });
    });
    rest->route("/api/putInQueue",QHttpServerRequest::Method::Post,[this,&m_requestHandler](const QHttpServerRequest &request){
        return QtConcurrent::run([this,&request,&m_requestHandler] () {
            if(m_requestHandler.checkRequest(request)){
                clients[request.remoteAddress()]->setPosInQueue(freeUsers.size());
                mt.lock();
                freeUsers.push_back(clients[request.remoteAddress()]);
                mt.unlock();
                return makeResponse(apiNums::putInQueue,QString("true"));
            }
            else{
                return makeResponse(apiNums::putInQueue,QString("false"));
            }
        });
    });
    rest->route("/api/getCompanion",QHttpServerRequest::Method::Get,[this,&m_requestHandler](const QHttpServerRequest &request){
        return QtConcurrent::run([this,&request,&m_requestHandler] () {
            if(m_requestHandler.checkRequest(request)){
                while(true){
                    auto companionPos=findCompanion();
                    if(companionPos!=freeUsers.end()){
                        Client *companion=*companionPos;
                        if(companion->isHasCompanion()){
                            continue;
                        }
                        else{
                            clients[request.remoteAddress()]->setCompanion(companion);
                            clients[companion->getAddress()]->setCompanion(clients[request.remoteAddress()]);
                            mt.lock();
                            //freeUsers.remove(companion->getPosInQueue());  Commented while testing at localhost cuz in hash map(Clients) key - it's address.What means that companion's address is the same with client's
                            freeUsers.remove(clients[request.remoteAddress()]->getPosInQueue());
                            mt.unlock();
                            return makeResponse(apiNums::getCompanion,companion->getNick());
                        }
                    }
                    else{
                        return makeResponse(apiNums::getCompanion,QString("NoCompanion"));
                    }
                }
            }
            else{
                return makeResponse(apiNums::getCompanion,QString("Error")); }
        });
    });

    if(this->listen(QHostAddress::Any,2323)){
        qDebug() << "Server started";
    }
    else{
        qDebug() << "Error occurred while starting:";
        qDebug() << this->errorString();
    }
    connect(this,&Server::newConnection,this,&Server::onNewClient);
}

CompanionPos Server::findCompanion()
{
    if(freeUsers.size()>1){
        qint64 clientNum = rg->bounded(0, freeUsers.size());
        return freeUsers.begin()+clientNum;
    }
    else{
        return freeUsers.end();
    }
}

Server::~Server()
{
    rest->deleteLater();
    socket->deleteLater();
    delete rg;
}

void Server::onNewClient(){
    socket = new QWebSocket;
    socket = this->nextPendingConnection();
    connect(socket,&QWebSocket::textMessageReceived,this,&Server::textMessageReceived);
    connect(socket,&QWebSocket::disconnected,this,&Server::disconnectedEvent);
    connect(socket,&QWebSocket::disconnected,socket,&QWebSocket::deleteLater); //Очистка сокета при получении сигнала об отключении клиента

    сlient = new Client(socket);
    mt.lock();
    clients.insert(socket->peerAddress(),сlient);
    mt.unlock();
    qDebug() << "Client connected" << socket->peerAddress();
}

void Server::textMessageReceived(const QString &message)
{
    socket=(QWebSocket*)sender();
    qDebug() << "Message from" << socket->peerAddress() << message;
}

QHttpServerResponse Server::makeResponse(qint16 apiNum,QString result)
{
    if(apiNum==apiNums::getMembers){
        if(result=="true"){
            QHttpServerResponse response(QString(QString::number(clients.size())),QHttpServerResponse::StatusCode::Ok);
            response.setHeader("Access-Control-Allow-Origin","*");
            return response;
        }
        else{
            QHttpServerResponse response(result,QHttpServerResponse::StatusCode::BadRequest);
            response.setHeader("Access-Control-Allow-Origin","*");
            return response;
        }
    }
    if(apiNum==apiNums::postNick){
        if(result=="true"){
            QHttpServerResponse response(result,QHttpServerResponse::StatusCode::Ok);
            response.setHeader("Access-Control-Allow-Origin","*");
            return response;
        }
        else if(result=="NickAlreadyExist"){
            QHttpServerResponse response(QString("false"),QHttpServerResponse::StatusCode::Ok);
            response.setHeader("Access-Control-Allow-Origin","*");
            return response;
        }
        else{
            QHttpServerResponse response(result,QHttpServerResponse::StatusCode::BadRequest);
            response.setHeader("Access-Control-Allow-Origin","*");
            return response;
        }
    }
    if(apiNum==apiNums::putInQueue){
        if(result=="true"){
            QHttpServerResponse response(result,QHttpServerResponse::StatusCode::Ok);
            response.setHeader("Access-Control-Allow-Origin","*");
            return response;
        }
        else{
            QHttpServerResponse response(result,QHttpServerResponse::StatusCode::BadRequest);
            response.setHeader("Access-Control-Allow-Origin","*");
            return response;
        }
    }
    if(apiNum==apiNums::getCompanion){
        if(result=="Error"){
            QHttpServerResponse response(result,QHttpServerResponse::StatusCode::BadRequest);
            response.setHeader("Access-Control-Allow-Origin","*");
            return response;
        }
        else{
            QHttpServerResponse response(result,QHttpServerResponse::StatusCode::Ok);
            response.setHeader("Access-Control-Allow-Origin","*");
            return response;
        }
    }
}

void Server::disconnectedEvent()
{
    QWebSocket *disconnectedClientSock= qobject_cast<QWebSocket *>(sender());
    if (disconnectedClientSock==NULL) {
        return;
    }
    clients[disconnectedClientSock->peerAddress()]->~Client();
    clients.remove(disconnectedClientSock->peerAddress());
    qDebug() << "Client disconnected" << disconnectedClientSock->peerAddress();
}
