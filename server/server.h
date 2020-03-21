#ifndef SERVER_H
#define SERVER_H

#include "protocol.h"

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

inline Roster roster {{
    { 1, "HRA", "Bizon" }
}};

class ClientConnection : public QObject {
    Q_OBJECT
public:
    enum State {
        PRE_AHOJ,
        AHOJ_SENT,
        AHOJ_RECEIVED,
        ROSTER_SENT,
    };
    ClientConnection(QObject *parent = nullptr, QTcpSocket *socket = nullptr)
        : QObject(parent)
        , m_socket(socket)
        , m_dataStream(m_socket)
    {
        connect(m_socket, &QTcpSocket::readyRead, this, &ClientConnection::onReadyRead);
        connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &ClientConnection::onError);
        m_dataStream << Packet{Ahoj{}};
        m_state = AHOJ_SENT;
        qCritical() << "Sent ahoj, error:" << m_dataStream.status() << m_socket->errorString();
    }
private slots:
    void onReadyRead() {
        Packet p;
        m_dataStream >> p;
        qCritical() << "Got packet type" << p.type;
        if (p.type == Packet::AHOJ) {
            m_state = AHOJ_RECEIVED;
            m_dataStream << Packet(roster);
        }
        else {
            m_dataStream << Packet(Packet::ERROR, "You should've sent AHOJ");
            m_socket->close();
            deleteLater();
        }
    }
    void onError(QAbstractSocket::SocketError err) {

    }
private:
    QTcpSocket *m_socket;
    QDataStream m_dataStream;
    State m_state { PRE_AHOJ };
};

class Server : public QObject {
    Q_OBJECT
public:
    Server(QObject *parent = nullptr) {
        connect(&m_server, &QTcpServer::newConnection, this, &Server::onNewConnection);
        m_server.listen(QHostAddress::Any, 16543);
    }
private slots:
    void onNewConnection() {
        while(m_server.hasPendingConnections()) {
            auto socket = m_server.nextPendingConnection();
            new ClientConnection(this, socket);
        }
    }
private:
    QTcpServer m_server;
};

#endif // SERVER_H
