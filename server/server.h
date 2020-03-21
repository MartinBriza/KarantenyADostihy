#ifndef SERVER_H
#define SERVER_H

#include "protocol.h"

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class ClientConnection;

struct Game {
    int id;
    QString name;
    QList<ClientConnection*> clients;
    int capacity;
    QString password;
};

inline Roster roster {{
}};

inline QMap<int, Game> games {
};

inline int lastID { 2 };

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
            m_clientName = p.ahoj.ahoj;
            qCritical() << "Client is called" << m_clientName;
            m_dataStream << Packet(roster);
        }
        else if (m_clientName.isEmpty()) {
            m_dataStream << Packet(Packet::ERROR, "You should've sent AHOJ");
            m_socket->flush();
            m_socket->close();
            deleteLater();
        }
        else {
            switch (p.type) {
            case Packet::CREATE: {
                roster.matches.append(Match { ++lastID, false, p.create.name, m_clientName, 1, 8 });
                auto gameIt = games.insert(lastID, Game { lastID, p.create.name, { this }, p.create.capacity, p.create.password });
                auto game = *gameIt;
                m_dataStream << Packet(Entered{ lastID, p.create.name });
                QList<Opponent> opponents;
                bool first;
                for (auto i : game.clients) {
                    Opponent o;
                    o.name = i->m_clientName;
                    o.color = i->m_clientColor;
                    o.ready = i->m_clientReady;
                    o.you = i == this;
                    o.money = i->m_money;
                    o.leader = first;
                    first = false;
                    opponents.append(o);
                }
                m_dataStream << Packet(opponents);
                break;
            }
            case Packet::JOIN: {
                if (games.contains(p.join.id)) {
                    auto &game = games[p.join.id];
                    if (game.clients.count() >= game.capacity)
                        m_dataStream << Packet(Packet::ERROR, "This game is full");
                    else if (!game.password.isEmpty() && game.password != p.join.password)
                        m_dataStream << Packet(Packet::ERROR, "Wrong password");
                    else {
                        game.clients.append(this);
                        m_dataStream << Packet(Entered{ game.id, game.name });
                        QList<Opponent> opponents;
                        bool first;
                        for (auto i : game.clients) {
                            Opponent o;
                            o.name = i->m_clientName;
                            o.color = i->m_clientColor;
                            o.ready = i->m_clientReady;
                            o.you = i == this;
                            o.money = i->m_money;
                            o.leader = first;
                            first = false;
                            opponents.append(o);
                        }
                        m_dataStream << Packet(opponents);
                    }
                }
                break;
            }
            }
        }
    }
    void onError(QAbstractSocket::SocketError err) {

    }
private:
    QTcpSocket *m_socket;
    QDataStream m_dataStream;
    State m_state { PRE_AHOJ };
    QString m_clientName {};
    int m_clientRoom { 0 };
    bool m_clientReady { false };
    int m_money { 30000 };
    QColor m_clientColor { Qt::red };
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
