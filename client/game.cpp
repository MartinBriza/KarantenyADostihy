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

UIMatch::UIMatch(UIRoster *parent, const QString &name, const QString &owner)
    : QObject(parent)
    , Match()
{
    this->name = name;
    this->owner = owner;
}

UIMatch::UIMatch(UIRoster *parent, const Match &data)
    : QObject(parent)
    , Match(data)
{

}

UIRoster *UIMatch::roster() {
    return qobject_cast<UIRoster*>(parent());
}

Client *UIMatch::client() {
    return roster()->client();
}

int UIMatch::idGet() const {
    return id;
}

bool UIMatch::passwordGet() const {
    return password;
}

QString UIMatch::nameGet() const {
    return name;
}

QString UIMatch::ownerGet() const {
    return owner;
}

int UIMatch::playersGet() const {
    return players;
}

int UIMatch::maximumPlayersGet() const {
    return maximumPlayers;
}

void UIMatch::join(const QString password) {
    client()->join(id, password);
}

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
    m_dataStream << Packet(Join{id, password});
}

void Client::leave() {
    m_dataStream << Packet(Join{ -1, {}});
}

void Client::create(const QString &name) {
    if (name.isEmpty())
        m_dataStream << Packet(Create{m_name + "'s game"});
    else
        m_dataStream << Packet(Create{name});
}

void Client::sendMessage(const QString &message) {
    m_dataStream << Packet(Chat { QDateTime().time().toString(), m_name, message, 0 });
}

void Client::refreshRoster() {
    m_dataStream << Packet(Roster {});
}

UIRoster::UIRoster(Client *parent)
    : QObject(parent)
{

}

Client *UIRoster::client() {
    return qobject_cast<Client*>(parent());
}

QQmlListProperty<UIMatch> UIRoster::matches() {
    return QQmlListProperty<UIMatch>(this, m_matches);
}

int UIRoster::matchCount() const {
    return m_matches.count();
}

void UIRoster::create() {
    client()->create();
}

void UIRoster::regenerate(const Roster &data) {
    for (auto i : m_matches)
        i->deleteLater();
    m_matches.clear();
    emit matchesChanged();
    for (auto &i : data.matches) {
        m_matches.append(new UIMatch(this, i));
    }
    emit matchesChanged();
}
