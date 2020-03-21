#ifndef SERVER_H
#define SERVER_H

#include "protocol.h"

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class ClientConnection;

struct Game : public Match {
    Game(const Match& match)
        : Match(match) {}
    QList<ClientConnection*> clients;
};

inline QSet<Game*> games {};

inline int lastGameID { 1 };
inline int lastPlayerID { 1 };

inline QList<QColor> colors {
    Qt::red,
    Qt::blue,
    Qt::green,
    Qt::yellow,
    Qt::cyan,
    Qt::magenta,
    Qt::white,
    Qt::black,
};

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
        connect(m_socket, &QTcpSocket::stateChanged, this, &ClientConnection::onSocketStateChanged);
        m_dataStream << Packet{Ahoj{}};
        m_state = AHOJ_SENT;
        qCritical() << "Sent ahoj, error:" << m_dataStream.status() << m_socket->errorString();
    }
    ~ClientConnection() {

    }
private slots:
    void onSocketStateChanged() {
        qCritical() << "SOCKET STATE: " << m_socket->state();
        if (m_socket->state() == QAbstractSocket::UnconnectedState) {
            leaveGame();
            deleteLater();
        }
    }
    void onReadyRead() {
        Packet p;
        m_dataStream >> p;
        qCritical() << "Got packet type" << p.type;
        if (p.type == Packet::AHOJ) {
            m_state = AHOJ_RECEIVED;
            m_clientName = p.ahoj.ahoj;
            qCritical() << "Client is called" << m_clientName;
            Roster roster;
            for (auto i : games) {
                roster.matches.append(*i);
            }
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
            case Packet::ROSTER: {
                Roster roster;
                for (auto &i : games) {
                    roster.matches.append(*i);
                }
                m_dataStream << Packet(roster);
                break;
            }
            case Packet::CREATE: {
                auto gameIt = games.insert(new Game(Match{lastGameID++, p.create.password, p.create.name, m_clientName, 0, p.create.capacity}));
                auto game = *gameIt;
                joinGame(game->id, p.create.password);
            }
            case Packet::JOIN: {
                if (p.join.id < 0) {
                    leaveGame();
                } else {
                    joinGame(p.join.id, p.join.password);
                }
                break;
            }
            case Packet::CHAT: {
                for (auto *i : games) {
                    // another great design decision
                    if (i->clients.contains(this)) {
                        for (auto c : i->clients) {
                            if (c->m_socket->isWritable())
                                c->m_dataStream << Packet(Chat{ p.chat.time, p.chat.from, p.chat.message, 0 });
                        }
                    }
                }
                break;
            }
            case Packet::OPPONENTS: {
                auto game = clientGame();
                if (!game) {
                    qCritical() << "NOUGEJM";
                    return;
                }
                bool changed = false;
                for (auto i : p.opponents) {
                    for (auto c : game->clients) {
                        if (c->m_clientId == i.id) {
                            if (!i.name.isEmpty()) {
                                c->m_clientName = i.name;
                                changed = true;
                            }
                            if (i.color.isValid()) {
                                c->m_clientColor = i.color;
                                changed = true;
                            }
                            if (i.money >= 0) {
                                c->m_money = i.money;
                                changed = true;
                            }
                            if (c->m_clientReady != i.ready) {
                                qCritical() << "Changing ready to" << i.ready;
                                c->m_clientReady = i.ready;
                                changed = true;
                            }
                        }
                    }
                }
                qCritical() << "Jako snaha byla";
                if (changed)
                    updateOpponents(game);
            }
            }
        }
    }
    void onError(QAbstractSocket::SocketError err) {
        leaveGame();
        deleteLater();
    }
private:
    Game *clientGame() {
        for (auto i : games) {
            if (i->clients.contains(this))
                return i;
        }
        return nullptr;
    }
    void joinGame(int id, const QString &password) {
        for (auto game : games) {
            if (game->id == id) {
                if (game->clients.contains(this))
                    return;
                if (game->clients.count() >= game->maximumPlayers) {
                    m_dataStream << Packet(Packet::ERROR, "This game is full");
                    return;
                }
                else if (!game->password.isEmpty() && game->password != password) {
                    m_dataStream << Packet(Packet::ERROR, "Wrong password");
                    return;
                }
                leaveGame(false);
                game->clients.append(this);
                game->players++;
                m_dataStream << Packet(Entered{ game->id, game->name });
                updateOpponents(game);
            }
        }
    }
    void leaveGame(bool notify = true) {
        m_dataStream << Packet(Entered{ -1, {} });
        for (auto game : games) {
            if (game->clients.contains(this)) {
                game->clients.removeOne(this);
                game->players--;
                if (notify)
                    updateOpponents(game);
                handleEmptyGame(game);
                return;
            }
        }
    }
    void handleEmptyGame(Game *game) {
        if (game->clients.count() == 0) {
            delete game;
            games.remove(game);
        }
    }
    void updateOpponents(Game *game) {
        // totally clean
        int position = 0;
        for (auto client : game->clients) {
            QList<Opponent> opponents;
            bool first = true;
            for (auto i : game->clients) {
                Opponent o;
                o.id = i->m_clientId;
                o.name = i->m_clientName;
                o.color = i->m_clientColor;
                o.ready = i->m_clientReady;
                o.you = i == client;
                o.money = i->m_money;
                o.leader = first;
                first = false;
                opponents.append(o);
            }
            client->m_dataStream << Packet(opponents);
            client->m_clientColor = colors[position];
            position++;
        }
    }

    QTcpSocket *m_socket;
    QDataStream m_dataStream;
    State m_state { PRE_AHOJ };
    QString m_clientName {};
    bool m_clientReady { false };
    int m_clientId { lastPlayerID++ };
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
