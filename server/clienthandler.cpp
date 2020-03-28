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
    #include "../def/fields.h"
};
inline QVector<Card> chanceCards {
    #include "../def/chance.h"
};
inline QVector<Card> financeCards {
    #include "../def/finance.h"
};


static int lastPlayerID { 1 };

struct PlayerData : public Player {
    PlayerData(ClientHandler *client, int money = 30000) : Player { lastPlayerID++ }, client(client)
    {
        Player::money = money;
        Player::name = client->m_name;
        client->setPlayer(this);
    }
    ClientHandler *client { nullptr };
};

struct Game : public Match {
    Game(const Match& match)
        : Match(match) {
        for (auto &i : fields) {
            ownerships.insert(&i, 0);
            upgrades.insert(&i, 0);
        }
    }
    QList<PlayerData*> clients;
    int activePlayer { 0 };
    GameState state {{
        { {"AHOJ"}, 0 }
    }};
    QMap<const Field*, int> ownerships;
    QMap<const Field*, int> upgrades;

    PlayerData *playerById(int id) {
        for (auto i : clients) {
            if (i->id == id)
                return i;
        }
        return nullptr;
    }
    PlayerData *ownerOf(int field) {
        for (auto i : ownerships.keys()) {
            if (i->id == field) {
                if (i->id <= 0)
                    return nullptr;
                for (auto c : clients) {
                    if (c->id == i->id) {
                        return c;
                    }
                }
            }
        }
        return nullptr;
    }
    int upgradeLevelOf(int field) {
        for (auto i : upgrades.keys()) {
            if (i->id == field) {
                return upgrades[i];
            }
        }
        return -1;
    }
    // this is probably (uh, definitely) wasteful
    const Field *fieldById(int id) {
        for (auto &i : fields) {
            if (i.id == id)
                return &i;
        }
        return nullptr;
    };
    QList<const Field*> fieldsOwnedByPlayer(int id) {
        QList<const Field*> ret;
        for (auto i : ownerships.keys()) {
            if (ownerships[i] == id)
                ret.append(i);
        }
        return ret;
    }
    int totalUpgradesByPlayer(int id) {
        auto fields = fieldsOwnedByPlayer(id);
        int ret { 0 };
        for (auto i : fields) {
            ret += upgrades[i];
        }
        return ret;
    }
};

inline QSet<Game*> games {};


ClientHandler::ClientHandler(QObject *parent, QWebSocket *socket)
    : QObject(parent)
    , m_socket(socket)
{
    //connect(m_socket, &QWebSocket::readyRead, this, &ClientHandler::onReadyRead);
    connect(m_socket, &QWebSocket::binaryMessageReceived, this, &ClientHandler::onReadyRead);
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this, &ClientHandler::onSocketError);
    connect(m_socket, &QWebSocket::stateChanged, this, &ClientHandler::onSocketStateChanged);
    sendPacket(Packet{Ahoj{}});
}

ClientHandler::~ClientHandler() {

}

void ClientHandler::setPlayer(PlayerData *player) {
    m_player = player;
}

void ClientHandler::onSocketStateChanged() {
    qCritical() << "SOCKET STATE: " << m_socket->state();
    if (m_socket->state() == QAbstractSocket::UnconnectedState) {
        leaveGame();
        deleteLater();
    }
}

void ClientHandler::onReadyRead(const QByteArray &data) {
    auto dcopy(data);
    QDataStream dataStream(&dcopy, QIODevice::ReadWrite);
    Packet p;
    dataStream >> p;
    if (dataStream.status() != QDataStream::Ok) {
        return;
    }
    else {
        switch (p.type) {
        case Packet::AHOJ: onAhoj(p.ahoj); break;
        case Packet::ROSTER: onRoster(p.roster); break;
        case Packet::CREATE: onCreate(p.create); break;
        case Packet::JOIN: onJoin(p.join); break;
        case Packet::CHAT: onChat(p.chat); break;
        case Packet::PLAYERS: onPlayers(p.players); break;
        case Packet::GAMESTATE: onGameState(p.gameState); break;
        case Packet::OWNERSHIPS: onOwnerships(p.ownerships); break;
        case Packet::CARD: onCard(p.card); break;
        case Packet::DICE: onDice(p.dice); break;
        }
    }
}

void ClientHandler::onSocketError(QAbstractSocket::SocketError err) {
    leaveGame();
    deleteLater();
}

void ClientHandler::onError(const QString &err) {

}

void ClientHandler::onAhoj(const Ahoj &ahoj) {
    m_name = ahoj.ahoj;
    Roster roster;
    for (auto i : games) {
        roster.matches.append(*i);
    }
    sendPacket(Packet(roster));
}

void ClientHandler::onMatch(const Match &match) {

}

void ClientHandler::onRoster(const Roster &roster) {
    Roster r;
    // put the running games first so no sorting is needed on the client side
    for (auto &i : games) {
        if (!i->running)
            r.matches.append(*i);
    }
    for (auto &i : games) {
        if (i->running)
            r.matches.append(*i);
    }
    sendPacket(Packet(r));
}

void ClientHandler::onJoin(const Join &join) {
    if (join.id < 0) {
        leaveGame();
    } else {
        joinGame(join.id, join.password);
    }
}

void ClientHandler::onCreate(const Create &create) {
    auto gameIt = games.insert(new Game(Match{create.password, create.name, m_name, lastGameID++, 0, create.capacity, false}));
    auto game = *gameIt;
    joinGame(game->id, create.password);
}

void ClientHandler::onEntered(const Entered &entered) {

}

void ClientHandler::onPlayers(const QList<Player> &players) {
    auto game = clientGame();
    if (!game) {
        return;
    }
    bool changed = false;
    for (auto i : players) {
        auto c = game->playerById(i.id);
        if (!c)
            continue;
        if (!i.name.isEmpty()) {
            sendMessage(game, QString("<%1> changed the name of <%2> to \"%3\".").arg(m_player->name).arg(c->name).arg(i.name));
            m_name = c->name;
            c->name = i.name;
            changed = true;
        }
        if (i.color.isValid()) {
            sendMessage(game, QString("<%1> changed the color of <%2> to \"%3\".").arg(m_player->name).arg(c->name).arg(i.color.name()));
            c->color = i.color;
            changed = true;
        }
        if (i.money >= 0) {
            if (m_player->id == c->id) {
                int diff = i.money - c->money;
                if (diff > 0)
                    sendMessage(game, Chat{QString("<%1> took %2 from the bank").arg(m_player->name).arg(diff)});
                else
                    sendMessage(game, Chat{QString("<%1> gave %2 to the bank").arg(m_player->name).arg(diff)});
            }
            else {
                sendMessage(game, QString("<%1> changed the money of <%2> to \"%3\" from \"%4\".").arg(m_player->name).arg(c->name).arg(i.money).arg(c->money));
            }
            c->money = i.money;
            changed = true;
        }
        if (i.position >= 0) {
            sendMessage(game, QString("<%1> changed the position of <%2> to \"%3\" from \"%4\".").arg(m_player->name).arg(c->name).arg(i.position % 40).arg(c->position));
            c->position = i.position % 40;
            changed = true;
        }
        if (c->ready != i.ready) {
            sendMessage(game, QString("<%1> is now %2 ready.").arg(m_player->name).arg(i.ready ? "" : "not"));
            c->ready = i.ready;
            changed = true;
        }
    }
    if (changed)
        updateOpponents(game);
}

void ClientHandler::onChat(const Chat &chat) {
    auto game = clientGame();
    if (game)
        sendMessage(game, Chat{ chat.time, chat.from, chat.message, 0 });
}

void ClientHandler::onGameState(const GameState &gameState) {
    auto game = clientGame();
    if (game) {
        game->running = true;
        for (auto client : game->clients) {
            client->client->sendPacket(Packet(game->state));
        }
        updateOwnerships(game);
    }
}

void ClientHandler::onOwnerships(const QList<Ownership> &ownerships) {
    auto game = clientGame();
    if (!game)
        return;
    for (auto i : ownerships) {
        auto client = game->playerById(i.player);
        auto card = game->fieldById(i.card);
        if (!client | !card)
            continue;
        if (game->ownerships[card] == 0) {
            if (m_player->position != card->id - 1) {
                sendPacket(Packet(Packet::ERROR, "Koupit si můžeš jen kartu, na který stojíš"));
                break;
            }
            if (m_player->money < card->price) {
                sendPacket(Packet(Packet::ERROR, "Na tuhle kartu seš moc velká socka"));
                break;
            }
            m_player->money -= card->price;
            game->ownerships[card] = i.player;
            sendMessage(game, QString("<%1> bought %2 for %3.").arg(m_player->name).arg(card->name).arg(card->price));
            updateOwnerships(game);
            updateOpponents(game);
        } else if (game->ownerships[card] == m_player->id && m_player->id != i.player) {
            if (m_player->id != i.player) {
                game->ownerships[card] = i.player;
                sendMessage(game, QString("<%1> gave %2 to <%3>.").arg(m_player->name).arg(card->name).arg(client->name));
                updateOwnerships(game);
            }
            else {
            }
        } else {
            sendPacket(Packet(Packet::ERROR, "Nemůžeš koupit nebo prodat cizí kusy"));
            break;
        }
    }
}

void ClientHandler::onCard(const Card &card) {
    auto game = clientGame();
    if (card.name == "chance") {
        handleEffect(game, Effect {Effect::PLAYER, Effect::DRAW_CHANCE});
    }
    else if (card.name == "finance") {
        handleEffect(game, Effect {Effect::PLAYER, Effect::DRAW_FINANCE});
    }
    else if (m_player->position >= 0 && m_player->position < fields.count()) {
        if (fields[m_player->position].effects.count() > 0 && fields[m_player->position].effects.first().action == Effect::DRAW_CHANCE) {
            handleEffect(game, Effect {Effect::PLAYER, Effect::DRAW_CHANCE});
        }
        else if (fields[m_player->position].effects.count() > 0 && fields[m_player->position].effects.first().action == Effect::DRAW_FINANCE) {
            handleEffect(game, Effect {Effect::PLAYER, Effect::DRAW_FINANCE});
        }
        else {
            sendPacket(Packet(Packet::ERROR, "Hele, nevim, co tam nacvičuješ, ale o karty se takhle neříká"));
        }
    }
    else {
        sendPacket(Packet(Packet::ERROR, "Hele, nevim, co tam nacvičuješ, ale o karty se takhle neříká"));
    }
}

void ClientHandler::onDice(const Dice &dice) {
    auto game = clientGame();
    auto value = qrand() % 6 + 1;
    sendPacket(Packet(Dice { value }));
    sendMessage(game, QString("<%1> just threw %2").arg(m_player->name).arg(value));
}

void ClientHandler::onBullshit() {
    sendPacket(Packet(Packet::ERROR, "Tvůj klient posílá kraviny."));
}

void ClientHandler::sendMessage(Game *game, const Chat &message) {
    if (!game)
        return;
    for (auto c : game->clients) {
        if (c->client->m_socket->isValid())
            c->client->sendPacket(Packet(message));
    }
}

void ClientHandler::sendMessage(Game *game, const QString &message) {
    sendMessage(game, Chat { message });
}

Game *ClientHandler::clientGame() {
    for (auto i : games) {
        for (auto c : i->clients)
            if (c->client == this)
                return i;
    }
    return nullptr;
}

void ClientHandler::joinGame(int id, const QString &password) {
    for (auto game : games) {
        if (game->id == id) {
            bool contains = false;
            for (auto i : game->clients) {
                if (i->client == this) {
                    contains = true;
                    return;
                }
            }
            if (game->clients.count() >= game->maximumPlayers) {
                sendPacket(Packet(Packet::ERROR, "This game is full"));
                return;
            }
            else if (!game->password.isEmpty() && game->password != password) {
                sendPacket(Packet(Packet::ERROR, "Wrong password"));
                return;
            }
            leaveGame(false);
            game->clients.append(new PlayerData(this));
            game->players++;
            if (game->running)
                sendPacket(Packet( game->state ));
            else
                sendPacket(Packet(Entered{ game->id, game->name }));
            sendMessage(game, Chat{QString("<%1> joined this game.").arg(m_player->name)});
            updateOpponents(game);
        }
    }
}

void ClientHandler::leaveGame(bool notify) {
    sendPacket(Packet(Entered{ -1, {} }));
    for (auto game : games) {
        for (auto i : game->clients) {
            if (i->client == this) {
                game->clients.removeOne(i);
                game->players--;
                sendMessage(game, QString("<%1> left this game.").arg(m_player->name));
                if (notify)
                    updateOpponents(game);
                handleEmptyGame(game);
                return;
            }
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
        i->client->sendPacket(Packet(data));
    }
}

void ClientHandler::updateOpponents(Game *game) {
    // totally clean
    int position = 0;
    for (auto client : game->clients) {
        client->color = colors[position % 8];
        position++;
    }
    for (auto client : game->clients) {
        QList<Player> opponents;
        bool first = true;
        for (auto i : game->clients) {
            Player o;
            o.id = i->id;
            o.name = i->name;
            o.color = i->color;
            o.ready = i->ready;
            o.you = i == client;
            o.position = i->position;
            o.money = i->money;
            o.leader = first;
            first = false;
            opponents.append(o);
        }
        client->client->sendPacket(Packet(opponents));
    }
}

void ClientHandler::handleEffect(Game *game, const Effect &effect) {
    if (!game)
        return;
    QList<PlayerData*> clients;

    switch (effect.target) {
    case Effect::PLAYER:
        clients.append(m_player);
        break;
    case Effect::OTHER_PLAYERS:
        for (auto i : game->clients) {
            if (i->client != this)
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
        sendMessage(game, Chat{QString("Hráč %1 vytáhl kartu Náhoda - \"%2\"").arg(m_player->name).arg(c.name)});
        handleEffect(game, c.effect);
        sendPacket(Packet(c));
        break;
    }
    case Effect::DRAW_FINANCE: {
        auto &c = financeCards[qrand() % financeCards.count()];
        sendMessage(game, QString("Hráč %1 vytáhl kartu Finance - \"%2\"").arg(m_player->name).arg(c.name));
        handleEffect(game, c.effect);
        sendPacket(Packet(c));
        break;
    }
    case Effect::GAIN:
        for (auto i : clients)
            i->money += effect.amount;
        updateOpponents(game);
        break;
    case Effect::FEE:
        for (auto i : clients)
            i->money -= effect.amount;
        updateOpponents(game);
        break;
    case Effect::TRANSFER: {
        int total = 0;
        for (auto i : clients) {
            i->money -= effect.amount;
            total += effect.amount;
        }
        m_player->money += total;
        updateOpponents(game);
    }
    case Effect::MOVE_STEPS: {
        for (auto i : clients) {
            i->position += effect.amount;
            if (i->position < 0)
                i->position += 40;
            i->position %= 40;
        }
        updateOpponents(game);
    }
    case Effect::FEE_PER_RACE: {
        for (auto i : clients) {
            auto upgrades = game->totalUpgradesByPlayer(i->id);
            i->money -= upgrades * effect.amount;
        }
        updateOpponents(game);
    }
    }
}

void ClientHandler::sendPacket(const Packet &packet) {
    QByteArray array;
    QDataStream stream(&array, QIODevice::ReadWrite);
    stream << packet;
    m_socket->sendBinaryMessage(array);
}
