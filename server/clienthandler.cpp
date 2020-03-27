#include "clienthandler.h"

inline int lastGameID { 1 };
inline int lastCardID { 1 };

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

inline QVector<Field> fields {
    #include "../def/fields.def"
};
inline QVector<Card> chanceCards {
    #include "../def/chance.def"
};
inline QVector<Card> financeCards {
    #include "../def/finance.def"
};

struct Game : public Match {
    Game(const Match& match)
        : Match(match) {
        for (auto &i : fields) {
            ownerships.insert(&i, 0);
            upgrades.insert(&i, 0);
        }
    }
    QList<ClientHandler*> clients;
    GameState state {{
        { {"AHOJ"}, 0 }
    }};
    QMap<Field*, int> ownerships;
    QMap<Field*, int> upgrades;
};

inline QSet<Game*> games {};


ClientHandler::ClientHandler(QObject *parent, QTcpSocket *socket)
    : QObject(parent)
    , m_socket(socket)
    , m_dataStream(m_socket)
{
    connect(m_socket, &QTcpSocket::readyRead, this, &ClientHandler::onReadyRead);
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &ClientHandler::onError);
    connect(m_socket, &QTcpSocket::stateChanged, this, &ClientHandler::onSocketStateChanged);
    m_dataStream << Packet{Ahoj{}};
    m_state = AHOJ_SENT;
}

ClientHandler::~ClientHandler() {

}

void ClientHandler::onSocketStateChanged() {
    qCritical() << "SOCKET STATE: " << m_socket->state();
    if (m_socket->state() == QAbstractSocket::UnconnectedState) {
        leaveGame();
        deleteLater();
    }
}

void ClientHandler::onReadyRead() {
    if (!m_socket->isReadable())
        return;
    Packet p;
    m_dataStream >> p;
    if (m_dataStream.status() != QDataStream::Ok) {
        qCritical() << "GOT GARBAGE:" << m_socket->readAll();
        return;
    }
    if (p.type == Packet::AHOJ) {
        m_state = AHOJ_RECEIVED;
        m_clientName = p.ahoj.ahoj;
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
            break;
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
                    sendMessage(i, Chat{ p.chat.time, p.chat.from, p.chat.message, 0 });
                }
            }
            break;
        }
        case Packet::PLAYERS: {
            auto game = clientGame();
            if (!game) {
                return;
            }
            bool changed = false;
            for (auto i : p.players) {
                for (auto c : game->clients) {
                    if (c->m_clientId == i.id) {
                        if (!i.name.isEmpty()) {
                            sendMessage(game, Chat{QString("<%1> changed the name of <%2> to \"%3\".").arg(m_clientName).arg(c->m_clientName).arg(i.name)});
                            c->m_clientName = i.name;
                            changed = true;
                        }
                        if (i.color.isValid()) {
                            sendMessage(game, Chat{QString("<%1> changed the color of <%2> to \"%3\".").arg(m_clientName).arg(c->m_clientName).arg(i.color.name())});
                            c->m_clientColor = i.color;
                            changed = true;
                        }
                        if (i.money >= 0) {
                            if (m_clientId == c->m_clientId) {
                                int diff = i.money - c->m_money;
                                if (diff > 0)
                                    sendMessage(game, Chat{QString("<%1> took %2 from the bank").arg(m_clientName).arg(diff)});
                                else
                                    sendMessage(game, Chat{QString("<%1> gave %2 to the bank").arg(m_clientName).arg(diff)});
                            }
                            else {
                                sendMessage(game, Chat{QString("<%1> changed the money of <%2> to \"%3\" from \"%4\".").arg(m_clientName).arg(c->m_clientName).arg(i.money).arg(c->m_money)});
                            }
                            c->m_money = i.money;
                            changed = true;
                        }
                        if (i.position >= 0) {
                            sendMessage(game, Chat{QString("<%1> changed the position of <%2> to \"%3\" from \"%4\".").arg(m_clientName).arg(c->m_clientName).arg(i.position % 40).arg(c->m_position)});
                            c->m_position = i.position % 40;
                            changed = true;
                        }
                        if (c->m_clientReady != i.ready) {
                            sendMessage(game, Chat{QString("<%1> is now %2 ready.").arg(m_clientName).arg(i.ready ? "" : "not")});
                            c->m_clientReady = i.ready;
                            changed = true;
                        }
                    }
                }
            }
            if (changed)
                updateOpponents(game);
            break;
        }
        case Packet::GAMESTATE: {
            auto game = clientGame();
            if (game) {
                for (auto client : game->clients) {
                    client->m_dataStream << Packet(game->state);
                }
                updateOwnerships(game);
            }
            break;
        }
        case Packet::OWNERSHIPS: {
            auto game = clientGame();
            if (!game)
                return;
            for (auto i : p.ownerships) {
                for (auto client : game->clients) {
                    if (i.player == client->m_clientId) {
                        for (auto card : game->ownerships.keys()) {
                            if (i.card == card->id) {
                                if (game->ownerships[card] == 0) {
                                    if (m_position != card->id - 1) {
                                        m_dataStream << Packet(Packet::ERROR, "Koupit si můžeš jen kartu, na který stojíš");
                                        break;
                                    }
                                    if (m_money < card->price) {
                                        m_dataStream << Packet(Packet::ERROR, "Na tuhle kartu seš moc velká socka");
                                        break;
                                    }
                                    m_money -= card->price;
                                    game->ownerships[card] = i.player;
                                    sendMessage(game, Chat{QString("<%1> bought %2 for %3.").arg(m_clientName).arg(card->name).arg(card->price)});
                                    updateOwnerships(game);
                                    updateOpponents(game);
                                } else if (game->ownerships[card] == m_clientId) {
                                    game->ownerships[card] = i.player;
                                    sendMessage(game, Chat{QString("<%1> gave %2 to <%3>.").arg(m_clientName).arg(card->name).arg(client->m_clientName)});
                                    updateOwnerships(game);
                                } else {
                                    m_dataStream << Packet(Packet::ERROR, "Nemůžeš koupit nebo prodat cizí kusy");
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            break;
        }
        case Packet::CARD: {
            auto game = clientGame();
            if (p.card.name == "chance") {
                handleEffect(game, Effect {Effect::PLAYER, Effect::DRAW_CHANCE});
            }
            else if (p.card.name == "finance") {
                handleEffect(game, Effect {Effect::PLAYER, Effect::DRAW_FINANCE});
            }
            else if (m_position >= 0 && m_position < fields.count()) {
                if (fields[m_position].effects.count() > 0 && fields[m_position].effects.first().action == Effect::DRAW_CHANCE) {
                    handleEffect(game, Effect {Effect::PLAYER, Effect::DRAW_CHANCE});
                }
                else if (fields[m_position].effects.count() > 0 && fields[m_position].effects.first().action == Effect::DRAW_FINANCE) {
                    handleEffect(game, Effect {Effect::PLAYER, Effect::DRAW_FINANCE});
                }
                else {
                    m_dataStream << Packet(Packet::ERROR, "Hele, nevim, co tam nacvičuješ, ale o karty se takhle neříká");
                }
            }
            else {
                m_dataStream << Packet(Packet::ERROR, "Hele, nevim, co tam nacvičuješ, ale o karty se takhle neříká");
            }
            break;
        }
        case Packet::DICE: {
            auto game = clientGame();
            auto value = qrand() % 6 + 1;
            m_dataStream << Packet(Dice { value });
            sendMessage(game, Chat{ QString("<%1> just threw %2").arg(m_clientName).arg(value) });
            break;
        }
        }
    }
}

void ClientHandler::onError(QAbstractSocket::SocketError err) {
    leaveGame();
    deleteLater();
}

void ClientHandler::sendMessage(Game *game, Chat message) {
    if (!game)
        return;
    for (auto c : game->clients) {
        if (c->m_socket->isWritable())
            c->m_dataStream << Packet(message);
    }
}

Game *ClientHandler::clientGame() {
    for (auto i : games) {
        if (i->clients.contains(this))
            return i;
    }
    return nullptr;
}

void ClientHandler::joinGame(int id, const QString &password) {
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
            sendMessage(game, Chat{QString("<%1> joined this game.").arg(m_clientName)});
            updateOpponents(game);
        }
    }
}

void ClientHandler::leaveGame(bool notify) {
    m_dataStream << Packet(Entered{ -1, {} });
    for (auto game : games) {
        if (game->clients.contains(this)) {
            game->clients.removeOne(this);
            game->players--;
            sendMessage(game, Chat{QString("<%1> left this game.").arg(m_clientName)});
            if (notify)
                updateOpponents(game);
            handleEmptyGame(game);
            return;
        }
    }
}

void ClientHandler::handleEmptyGame(Game *game) {
    if (game->clients.count() == 0) {
        delete game;
        games.remove(game);
    }
}

void ClientHandler::updateOwnerships(Game *game) {
    QList<Ownership> data;
    for (auto i : game->ownerships.keys()) {
        data.append({game->ownerships[i], i->id});
    }
    for (auto i : game->clients) {
        i->m_dataStream << Packet(data);
    }
}

void ClientHandler::updateOpponents(Game *game) {
    // totally clean
    int position = 0;
    for (auto client : game->clients) {
        client->m_clientColor = colors[position % 8];
        position++;
    }
    for (auto client : game->clients) {
        QList<Player> opponents;
        bool first = true;
        for (auto i : game->clients) {
            Player o;
            o.id = i->m_clientId;
            o.name = i->m_clientName;
            o.color = i->m_clientColor;
            o.ready = i->m_clientReady;
            o.you = i == client;
            o.position = i->m_position;
            o.money = i->m_money;
            o.leader = first;
            first = false;
            opponents.append(o);
        }
        client->m_dataStream << Packet(opponents);
    }
}

void ClientHandler::handleEffect(Game *game, const Effect &effect) {
    if (!game)
        return;
    QList<ClientHandler*> clients;

    switch (effect.target) {
    case Effect::PLAYER:
        clients.append(this);
        break;
    case Effect::OTHER_PLAYERS:
        for (auto i : game->clients) {
            if (i != this)
                clients.append(i);
        }
        break;
    case Effect::ALL_PLAYERS:
        clients = game->clients;
        break;
    default:
        break;
    }

    switch (effect.action) {
    case Effect::NO_ACTION:
        break;
    case Effect::DRAW_CHANCE: {
        auto &c = chanceCards[qrand() % chanceCards.count()];
        sendMessage(game, Chat{QString("Hráč %1 vytáhl kartu Náhoda - \"%2\"").arg(m_clientName).arg(c.name)});
        handleEffect(game, c.effect);
        m_dataStream << Packet(c);
        break;
    }
    case Effect::DRAW_FINANCE: {
        auto &c = financeCards[qrand() % financeCards.count()];
        sendMessage(game, Chat{QString("Hráč %1 vytáhl kartu Finance - \"%2\"").arg(m_clientName).arg(c.name)});
        handleEffect(game, c.effect);
        m_dataStream << Packet(c);
        break;
    }
    case Effect::GAIN:
        for (auto i : clients)
            i->m_money += effect.amount;
        updateOpponents(game);
        break;
    case Effect::FEE:
        for (auto i : clients)
            i->m_money -= effect.amount;
        updateOpponents(game);
        break;
    case Effect::TRANSFER: {
        int total = 0;
        for (auto i : clients) {
            i->m_money -= effect.amount;
            total += effect.amount;
        }
        m_money += total;
        updateOpponents(game);
    }
    case Effect::MOVE_STEPS: {
        for (auto i : clients) {
            i->m_position += effect.amount;
            if (i->m_position < 0)
                i->m_position += 40;
            i->m_position %= 40;
        }
        updateOpponents(game);
    }
    }
}
