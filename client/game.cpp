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

Effect::Effect(QObject *parent, Effect::Target target, Effect::Action action, int amount, int secondaryAmount)
    : QObject(parent)
{

}

int Effect::amount() const {
    return m_amount;
}

int Effect::secondaryAmount() const {
    return m_secondaryAmount;
}

Card::Card(QObject *parent, const QString &text, Effect::Target target, Effect::Action action, int amount, int secondaryAmount)
    : Effect(parent, target, action, amount, secondaryAmount)
    , m_text(text)
{

}

Field::Field(QObject *parent, const QString &name, int price, QList<Effect *> effect, QColor color, int upgrade)
    : QObject(parent)
    , m_name(name)
    , m_effect(effect)
    , m_price(price)
    , m_color(color)
    , m_upgrade(upgrade)
{
    emit feeChanged();
}

QString Field::name() const {
    return m_name;
}

int Field::price() const {
    return m_price;
}

int Field::fee() const {
    if (m_effect.count() > 0)
        return m_effect.first()->amount();
    return 0;
}

QColor Field::color() const {
    return m_color;
}

Board::Board(QObject *parent)
    : QObject(parent)
{

}

QQmlListProperty<Field> Board::fields() {
    return QQmlListProperty<Field>(this, m_fields);
}

int Board::fieldCount() const {
    return m_fields.count();
}

Game::Game(QObject *parent)
    : QObject(parent)
{
    m_players.append(new Player(this, "Martin", 30000, Qt::blue));
    m_players.append(new Player(this, "Vendula", 30000, Qt::red));
}

Board *Game::board() {
    return m_board;
}

QQmlListProperty<Player> Game::players() {
    return QQmlListProperty<Player>(this, m_players);
}
