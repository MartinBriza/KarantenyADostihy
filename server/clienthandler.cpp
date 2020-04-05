#include "clienthandler.h"

#include <random>

std::random_device rd;
std::mt19937 rng(rd());

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


static int lastPlayerID { 1 };

struct PlayerData : public Player {
    PlayerData(ClientHandler *client, int money = 30000) : Player { lastPlayerID++ }, client(client)
    {
        Player::money = money;
        Player::name = client->m_name;
        client->setPlayer(this);
    }
    ClientHandler *client { nullptr };
    bool upgradedThisTurn { false };
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
    GameState state {{
        { {"AHOJ"}, 0 }
    }};
    QMap<const Field*, int> ownerships;
    QMap<const Field*, int> upgrades;
    int playerOnTurn { 0 };

    PlayerData *playerById(int id) {
        for (auto i : clients) {
            if (i->id == id)
                return i;
        }
        return nullptr;
    }
    PlayerData *ownerOf(int field) {
        int pos { 0 };
        for (auto i : ownerships.keys()) {
            if (pos == field) {
                for (auto c : clients) {
                    if (c->id == ownerships[i]) {
                        return c;
                    }
                }
            }
            pos++;
        }
        return nullptr;
    }
    bool isSuspended(PlayerData *player) {
        return player && player->position == 10;
    }
    PlayerData *suspensionCancelOwner() {
        for (auto i : clients) {
            if (i->ownsCancelSuspension)
                return i;
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
    PlayerData *getPlayerOnTurn() {
        int idx { 0 };
        for (auto i : clients) {
            if (idx == playerOnTurn)
                return i;
            idx++;
        }
        return nullptr;
    }
    bool isPlayerOnTurn(ClientHandler *handler) {
        auto data = getPlayerOnTurn();
        if (data && data->client == handler)
            return true;
        return false;
    }
};

inline QSet<Game*> games {};

static Game *gameByPlayer(PlayerData *player) {
    for (auto g : games) {
        if (g->clients.contains(player))
            return g;
    }
    return nullptr;
}

static Game *gameById(int id) {
    for (auto g : games) {
        if (g->id == id)
            return g;
    }
    return nullptr;
}


ClientHandler::ClientHandler(QObject *parent, QWebSocket *socket)
    : QObject(parent)
    , m_socket(socket)
{
    qWarning() << "Client" << id << "connected from" << m_socket->peerAddress().toString();
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
    if (m_socket->state() == QAbstractSocket::UnconnectedState) {
        qWarning() << "Client" << id << "disconnected";
        leaveGame();
        deleteLater();
    }
}

void ClientHandler::onReadyRead(const QByteArray &message) {
    if (!m_socket->isValid())
        return;
    Packet p;
    QDataStream stream(message);
    stream >> p;
    if (stream.status() != QDataStream::Ok) {
        qCritical() << "GOT GARBAGE:" << message;
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
        case Packet::EFFECT: onEffect(p.effect); break;
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
    qWarning() << "Client" << id << "sent Ahoj: " << ahoj.ahoj;

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
    qWarning() << "Client" << id << "requested Roster";
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
    qWarning() << "Client" << id << "requested to Join: " << join.id << " (" << (gameById(join.id) ? gameById(join.id)->name : "INVALID") << ")";
    if (join.id < 0) {
        leaveGame();
    } else {
        joinGame(join.id, join.password);
    }
}

void ClientHandler::onCreate(const Create &create) {
    auto gameIt = games.insert(new Game(Match{create.password, create.name, m_name, lastGameID++, 0, create.capacity, false}));
    auto game = *gameIt;
    qWarning() << "Client" << id << "Created a game called" << create.name << ". It was assigned ID" << game->id;
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
            if (game->strict) {
                sendError("Hele, tohle v týhle verzi hry nejde");
            }
            else {
                qWarning() << "Client" << id << QString("(<%1>) changed the name of <%2> to \"%3\".").arg(m_player->name).arg(c->name).arg(i.name);
                sendMessage(game, QString("<%1> changed the name of <%2> to \"%3\".").arg(m_player->name).arg(c->name).arg(i.name));
                m_name = c->name;
                c->name = i.name;
                changed = true;
            }
        }
        if (i.color.isValid()) {
            if (game->strict) {
                sendError("Hele, tohle v týhle verzi hry nejde");
            }
            else {
                qWarning() << "Client" << id << QString("(<%1>) changed the color of <%2> to \"%3\".").arg(m_player->name).arg(c->name).arg(i.color.name());
                sendMessage(game, QString("<%1> changed the color of <%2> to \"%3\".").arg(m_player->name).arg(c->name).arg(i.color.name()));
                c->color = i.color;
                changed = true;
            }
        }
        if (i.money >= 0) {
            if (game->strict) {
                sendError("Hele, tohle v týhle verzi hry nejde");
            }
            else {
                if (m_player->id == c->id) {
                    int diff = i.money - c->money;
                    if (diff > 0) {
                        qWarning() << "Client" << id << QString("(<%1>) took %2 from the bank").arg(m_player->name).arg(diff);
                        sendMessage(game, Chat{QString("<%1> took %2 from the bank").arg(m_player->name).arg(diff)});
                    }
                    else {
                        qWarning() << "Client" << id << QString("(<%1>) gave %2 to the bank").arg(m_player->name).arg(diff);
                        sendMessage(game, Chat{QString("<%1> gave %2 to the bank").arg(m_player->name).arg(diff)});
                    }
                }
                else {
                    qWarning() << "Client" << id << QString("(<%1>) changed the money of <%2> to \"%3\" from \"%4\".").arg(m_player->name).arg(c->name).arg(i.money).arg(c->money);
                    sendMessage(game, QString("<%1> changed the money of <%2> to \"%3\" from \"%4\".").arg(m_player->name).arg(c->name).arg(i.money).arg(c->money));
                }
                c->money = i.money;
                changed = true;
            }
        }
        if (i.position >= 0) {
            if (game->strict) {
                sendError("Hele, tohle v týhle verzi hry nejde");
            }
            else {
                qWarning() << "Client" << id << QString("(<%1>) changed the position of <%2> from \"%3\" to \"%4\".").arg(m_player->name).arg(c->name).arg(c->position).arg(i.position % 40);
                sendMessage(game, QString("<%1> changed the position of <%2> from \"%3\" to \"%4\".").arg(m_player->name).arg(c->name).arg(c->position).arg(i.position % 40));
                movePlayerTo(c, i.position % 40);
                changed = true;
            }
        }
        if (c->ready != i.ready && c->id == i.id) {
            qWarning() << "Client" << id << QString("(<%1>) is now %2 ready.").arg(m_player->name).arg(i.ready ? "" : "not");
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
        for (auto client : game->clients) {
            if (!client->ready) {
                sendPacket(Packet(Packet::ERROR, "Všichni ještě nejsou ready"));
                return;
            }
        }
        game->running = true;
        qWarning() << "Client" << id << "Started the game" << game->id;
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
            m_player->upgradedThisTurn = true;
            game->ownerships[card] = i.player;
            qWarning() << "Client" << id << QString("<%1> bought %2 for %3.").arg(m_player->name).arg(card->name).arg(card->price);
            sendMessage(game, QString("<%1> bought %2 for %3.").arg(m_player->name).arg(card->name).arg(card->price));
            updateOwnerships(game);
            updateOpponents(game);
        } else if (game->ownerships[card] == m_player->id) {
            if (m_player->id != i.player) {
                game->ownerships[card] = i.player;
                qWarning() << "Client" << id << QString("<%1> gave %2 to <%3>.").arg(m_player->name).arg(card->name).arg(client->name);
                sendMessage(game, QString("<%1> gave %2 to <%3>.").arg(m_player->name).arg(card->name).arg(client->name));
                updateOwnerships(game);
            }
            else {
                auto color = card->color;
                bool match = true;
                for (auto &c : fields) {
                    if (c.color == color) {
                        if (game->ownerships[&c] != m_player->id) {
                            match = false;
                            break;
                        }
                    }
                }
                if (m_player->upgradedThisTurn && (game->upgrades[card] == 0 || game->upgrades[card] == 4)) {
                    sendError("Kupovat další dostihy jde až když sem dojdeš znova.");
                }
                else if (m_player->position != card->id - 1) {
                    sendError("Nemůžeš kupovat dostihy na karty na kterejch nestojíš");
                }
                else if (match) {
                    int difference = i.upgradeLevel - game->upgrades[card];
                    game->upgrades[card] += difference;
                    updateOwnerships(game);
                }
                else {
                    sendError("Nemůžeš kupovat dostihy, protože ještě nemáš všechny koně z týhle stáje.");
                }
            }
        } else {
            sendPacket(Packet(Packet::ERROR, "Nemůžeš koupit nebo prodat cizí kusy"));
            break;
        }
    }
}

void ClientHandler::onCard(const Card &card) {
    auto game = clientGame();
    if (game->strict) {
        return;
    } else if (card.name == "chance") {
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
    if (game->isPlayerOnTurn(this)) {
        for (auto i : game->clients) {
            if (i->client != this) {
                i->dice.values.clear();
            }
        }
        if (dice.values.startsWith(-1)) {
            // end turn
            qWarning() << "Client" << id << "wants to end their turn";
            while (true) {
                game->playerOnTurn++;
                game->playerOnTurn %= game->clients.count();
                if (game->clients[game->playerOnTurn]->waiting <= 0) {
                    break;
                }
                else {
                    game->clients[game->playerOnTurn]->waiting--;
                }
            }
            for (auto i : game->clients) {
                i->dice.values.clear();
                i->dice.moved = false;
            }
            updateOpponents(game);
            return;
        }
        else if (dice.values.isEmpty() && !dice.moved && !m_player->dice.moved) {
            // throws
            if (m_player->dice.values.empty() || m_player->dice.values.last() == 6) {
                std::uniform_int_distribution<> distribution(1, 6);
                auto value = distribution(rng);
                qWarning() << "Client" << id << "threw" << value << "on his turn to dice";
                m_player->dice.values.append(value);
                sendMessage(game, QString("<%1> just threw %2").arg(m_player->name).arg(value));
                if ((!game->isSuspended(m_player) || m_player->ownsCancelSuspension) && m_player->dice.values == QList<int>{6, 6}) {
                    if (game->isSuspended(m_player) && m_player->ownsCancelSuspension)
                        m_player->ownsCancelSuspension = false;
                    handleEffect(game, Effect { Effect::PLAYER, Effect::MOVE_TO_SUSPENSION });
                    m_player->dice.moved = true;
                }
                if (game->isSuspended(m_player) && !m_player->ownsCancelSuspension && !m_player->dice.values.startsWith(6))
                    m_player->dice.moved = true;
                updateOpponents(game);
                return;
            }
        }
        else if (!m_player->dice.moved) {
            // moves
            qWarning() << "Client" << id << "wants to move after throwing dice";
            m_player->dice.moved = true;
            m_player->upgradedThisTurn = false;
            if (game->isSuspended(m_player) && !m_player->ownsCancelSuspension) {
                if (m_player->dice.values.startsWith(6)) {
                    m_player->dice.values.removeFirst();
                    for (auto i : m_player->dice.values) {
                        movePlayerTo(m_player, m_player->position + i);
                        updateOpponents(game);
                    }
                }
            }
            else {
                if (game->isSuspended(m_player) && m_player->ownsCancelSuspension)
                    m_player->ownsCancelSuspension = false;
                for (auto i : m_player->dice.values) {
                    movePlayerTo(m_player, m_player->position + i);
                    updateOpponents(game);
                }
            }
            return;
        }
    }
    // gets here only when something is wrong
    sendError("Hele, nemám teď čas zjišťovat, cos udělal/a blbě, ale tohle je prostě NELEGÁLNÍ");
}

void ClientHandler::onEffect(const Effect &effect) {
}

void ClientHandler::onBullshit() {
    sendPacket(Packet(Packet::ERROR, "Tvůj klient posílá kraviny."));
}

void ClientHandler::sendMessage(Game *game, const Chat &message) {
    if (!game)
        return;
    for (auto c : game->clients) {
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
        data.append({game->ownerships[i], i->id, game->upgrades[i]});
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
        int order = 0;
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
            o.onTurn = (order == game->playerOnTurn);
            o.dice = i->dice;
            o.ownsCancelSuspension = i->ownsCancelSuspension;
            first = false;
            opponents.append(o);
            order++;
        }
        client->client->sendPacket(Packet(opponents));
    }
}

void ClientHandler::modifyPlayerMoney(PlayerData *player, int diff) {
    m_player->money += diff;
}

void ClientHandler::movePlayerTo(PlayerData *player, int position) {
    // TODO
    if (position < 0)
        position = 40 - position;
    position %= 40;
    player->position = position % fields.count();
    handleFieldEffect(player);
}

void ClientHandler::handleFieldEffect(PlayerData *player) {
    auto game = gameByPlayer(player);
    const auto &field = fields[player->position];
    switch (field.type) {
    case Field::BASIC:
        for (auto effect : field.effects) {
            player->client->handleEffect(game, effect);
        }
        break;
    case Field::HORSE:
        if (game->ownerOf(player->position)) {
            auto owner = game->ownerOf(player->position);
            qWarning() << field.name << "is owned by" << owner->name;
            if (owner == player)
                return;
            auto currentUpgrade = game->upgradeLevelOf(player->position);
            auto currentEffect = field.effects[currentUpgrade];

            player->client->handleEffect(game, Effect { Effect::PLAYER, Effect::FEE, currentEffect.amount });
            owner->client->handleEffect(game, Effect { Effect::PLAYER, Effect::GAIN, currentEffect.amount });
        }
        break;
    case Field::TRAINER:
        break;
    case Field::TRANSPORT:
        break;
    case Field::DECK:
        for (auto effect : field.effects) {
            player->client->handleEffect(game, effect);
        }
        break;
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
        Card &c = chanceCards[qrand() % chanceCards.count()];
        while (game->suspensionCancelOwner() && c.effect.action == Effect::CANCEL_SUSPENSION)
            c = chanceCards[qrand() % chanceCards.count()];
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
        break;
    }
    case Effect::MOVE_STEPS: {
        for (auto i : clients) {
            movePlayerTo(i, i->position + effect.amount);
        }
        updateOpponents(game);
        break;
    }
    case Effect::FEE_PER_RACE: {
        for (auto i : clients) {
            auto upgrades = game->totalUpgradesByPlayer(i->id);
            i->money -= upgrades * effect.amount;
        }
        updateOpponents(game);
        break;
    }
    case Effect::MOVE_TO_SUSPENSION: {
        // TODO
        for (auto i : clients) {
            if (effect.amount != 0 && i->position < 10)
                modifyPlayerMoney(i, 4000);
            movePlayerTo(i, 10);
        }
        updateOpponents(game);
        break;
    }
    case Effect::MOVE_TO_LAST_FIELD: {
        // TODO
        for (auto i : clients) {
            if (effect.amount != 0)
                modifyPlayerMoney(i, 4000);
            movePlayerTo(i, 39);
        }
        updateOpponents(game);
        break;
    }
    case Effect::MOVE_TO_PARKING_LOT: {
        // TODO
        for (auto i : clients) {
            if (effect.amount != 0 && i->position < 20)
                modifyPlayerMoney(i, 4000);
            movePlayerTo(i, 20);
        }
        updateOpponents(game);
        break;
    }
    case Effect::MOVE_TO_FINANCE: {
        for (auto i : clients) {
            int position = i->position;
            int startPosition = position; // to be safe
            if (effect.amount != 0) {
                while (fields[position].effects.count() == 0 || fields[position].effects.first().action != Effect::DRAW_FINANCE) {
                    position += effect.amount;
                    if (position >= 40)
                        position = 0;
                    if (position < 0)
                        position = 39;
                    if (position == startPosition) {
                        qCritical() << "ERROR! Something is wrong about the field data, there are no finance cards";
                        return;
                    }
                }
                movePlayerTo(i, position);
            }
        }
        updateOpponents(game);
        break;
    }
    case Effect::MOVE_TO_TRAINER: {
        for (auto i : clients) {
            int position = i->position;
            int startPosition = position; // to be safe
            if (effect.amount != 0) {
                while (fields[position].effects.count() == 0 || fields[position].type != Field::TRAINER) {
                    position += effect.amount;
                    if (position >= 40) {
                        position = 0;
                        modifyPlayerMoney(i, 4000);
                    }
                    if (position < 0) {
                        position = 39;
                        modifyPlayerMoney(i, 4000);
                    }
                    if (position == startPosition) {
                        qCritical() << "ERROR! Something is wrong about the field data, there are no finance cards";
                        return;
                    }
                }
                movePlayerTo(i, position);
            }
        }
        updateOpponents(game);
        break;
    }
    case Effect::MOVE_TO_FIRST_FIELD: {
        for (auto i : clients) {
            movePlayerTo(i, 0);
            if (effect.amount != 0)
                modifyPlayerMoney(i, 4000);
        }
        updateOpponents(game);
        break;
    }
    case Effect::FEE_PER_STEP: {
        for (auto i : clients) {
            int steps = 0;
            for (auto s : i->dice.values)
                steps += s;
            modifyPlayerMoney(i, steps * effect.amount);
        }
        updateOpponents(game);
        break;
    }
    case Effect::WAIT: {
        for (auto i : clients) {
            i->waiting += effect.amount;
        }
        updateOpponents(game);
        break;
    }
    case Effect::CANCEL_SUSPENSION: {
        for (auto i : clients) {
            auto o = game->suspensionCancelOwner();
            if (o)
                return;
            else
                i->ownsCancelSuspension = true;
        }
        updateOpponents(game);
        break;
    }
    }
}

void ClientHandler::sendError(const QString &message) {
    sendPacket(Packet{Packet::ERROR, message});
}

void ClientHandler::sendPacket(const Packet &p) {
    QByteArray frame;
    QDataStream stream(&frame, QIODevice::WriteOnly);
    stream << p;
    m_socket->sendBinaryMessage(frame);
}
