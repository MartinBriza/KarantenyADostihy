#include "server.h"

Server::Server(QObject *parent)
    : QObject(parent)
    , m_server("karanteny", QWebSocketServer::NonSecureMode)
{
    connect(&m_server, &QWebSocketServer::newConnection, this, &Server::onNewConnection);
    m_server.listen(QHostAddress::Any, 16543);
}

void Server::onNewConnection() {
    while(m_server.hasPendingConnections()) {
        auto socket = m_server.nextPendingConnection();
        new ClientHandler(this, socket);
    }
}
