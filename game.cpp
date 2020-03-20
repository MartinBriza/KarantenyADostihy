#include "game.h"

Player::Player(Game *parent, const QString &name, int money, const QColor &color)
    : QObject(parent)
    , m_name(name)
    , m_money(money)
    , m_color(color)
{

}

Game *Player::game() {
    return qobject_cast<Game*>(parent());
}

Board *Player::board() {
    return game()->board();
}

int Player::position() const {
    return m_position;
}

QString Player::name() const {
    return m_name;
}

int Player::money() const {
    return m_money;
}

QColor Player::color() const {
    return m_color;
}

void Player::move(int by) {
    m_position += by;
    m_position %= board()->fieldCount();
    emit positionChanged();
}
