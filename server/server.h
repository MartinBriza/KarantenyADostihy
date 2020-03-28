#ifndef SERVER_H
#define SERVER_H

#include "protocol.h"

#include "clienthandler.h"

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QtWebSockets/QWebSocketServer>

class Server : public QObject {
    Q_OBJECT
public:
    Server(QObject *parent = nullptr);
private slots:
    void onNewConnection();
private:
    QWebSocketServer m_server { "karanteny", QWebSocketServer::NonSecureMode };
};

#endif // SERVER_H
