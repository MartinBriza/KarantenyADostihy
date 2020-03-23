#ifndef SERVER_H
#define SERVER_H

#include "protocol.h"

#include "clienthandler.h"

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class Server : public QObject {
    Q_OBJECT
public:
    Server(QObject *parent = nullptr);
private slots:
    void onNewConnection();
private:
    QTcpServer m_server;
};

#endif // SERVER_H
