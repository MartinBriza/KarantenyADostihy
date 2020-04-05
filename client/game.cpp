#include "game.h"


QString Client::randomName(int count) {
    const QString alphabet("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");

    QString name = "Guest ";
    for(int i = 0; i < count; ++i) {
        int index = qrand() % alphabet.count();
        QChar next = alphabet.at(index);
        name.append(next);
    }
    return name;
}

void Client::join(int id, const QString &password) {
    sendPacket(Packet(Join{id, password}));
}

void Client::leave() {
    sendPacket(Packet(Join{ -1, {}}));
}

void Client::create(const QString &name) {
    if (name.isEmpty())
        sendPacket(Packet(Create{nameGet() + "'s game"}));
    else
        sendPacket(Packet(Create{name}));
}

void Client::sendMessage(const QString &message) {
    sendPacket(Packet(Chat { QDateTime().time().toString(), nameGet(), message, 0 }));
}

void Client::refreshRoster() {
    if (m_socket->state() == QAbstractSocket::ConnectedState && m_state == ROSTER) {
        sendPacket(Packet(Roster {}));
    }
    else if (m_socket->state() == QAbstractSocket::UnconnectedState) {
        m_socket->open(QUrl(QString("ws://%1:%2").arg(settings.value("server", DEFAULT_SERVER).toString()).arg(settings.value("port", DEFAULT_PORT).toInt())));
    }
}

void Client::setReady(bool val) {
    if (m_thisPlayerId > 0) {
        sendPacket(Packet(QList<::Player>{{m_thisPlayerId, {}, {}, -1, -1, {}, val, {}, {}, {}}}));
    }
}

void Client::startGame() {
    sendPacket(Packet(GameState{}));
}

void Client::move(int id, int position) {
    sendPacket(Packet(QList<::Player>{{id, {}, {}, -1, position, {}, {}, {}, {}, {}, {}}}));
}

void Client::buy(int id) {
    sendPacket(Packet(QList<Ownership>{Ownership{m_thisPlayerId, id, -1}}));
}

void Client::upgrade(int id) {
    for (auto i : m_board->fieldList()) {
        if (i->idGet() == id) {
            sendPacket(Packet(QList<Ownership>{{m_thisPlayerId, id, i->upgradeLevelGet() + 1}}));
        }
    }
}

void Client::drawCard(int type) {
    if (type == Effect::DRAW_CHANCE) {
        sendPacket(Packet(Card{{}, "chance", {}}));
    }
    else if (type == Effect::DRAW_FINANCE) {
        sendPacket(Packet(Card{{}, "finance", {}}));
    }
    else {
        qWarning() << "DrawCard called with type" << type << "instead of" << (int)Effect::DRAW_CHANCE << "or" << (int)Effect::DRAW_FINANCE;
    }
}

void Client::giveMoney(int amount) {
    int currentMoney = 0;
    for (auto i : m_players) {
        if (i->you)
            currentMoney = i->money;
    }
    sendPacket(Packet(QList<::Player>{{m_thisPlayerId, {}, {}, currentMoney - amount, -1, {}, {}, {}, {}, {}}}));
}

void Client::takeMoney(int amount) {
    int currentMoney = 0;
    for (auto i : m_players) {
        if (i->you)
            currentMoney = i->money;
    }
    sendPacket(Packet(QList<::Player>{{m_thisPlayerId, {}, {}, currentMoney + amount, -1, {}, {}, {}, {}, {}}}));
}

void Client::dice() {
    sendPacket(Packet{Dice{}});
}

void Client::diceMove() {
    sendPacket(Packet{Dice{{}, true}});
}

void Client::endTurn() {
    sendPacket(Packet{Dice{{-1},false}});
}

void Client::reset() {
    m_lobby->deleteLater();
    m_lobby = nullptr;
    emit lobbyChanged();
    for (auto i : m_chat)
        i->deleteLater();
    m_chat.clear();
    emit chatChanged();
    for (auto i : m_players)
        i->deleteLater();
    m_players.clear();
    emit playersChanged();
    m_status = "Init";
    m_state = ROSTER;
    emit stateChanged();
}
