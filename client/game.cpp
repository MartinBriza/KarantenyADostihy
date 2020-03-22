#include "game.h"

UIEffect::UIEffect(QObject *parent, const Effect &effect)
    : QObject(parent)
    , Effect(effect)
{

}

int UIEffect::amountGet() const {
    return amount;
}

int UIEffect::secondaryAmountGet() const {
    return secondaryAmount;
}

UICard::UICard(QObject *parent, const QString &text, const Effect &effect)
    : QObject(parent)
    , Effect(effect)
    , text(text)
{
    uiEffect = new UIEffect(this, effect);
}

UIField::UIField(QObject *parent, const Field &field)
    : QObject(parent)
    , Field(field)
{
    for (auto i : field.effects) {
        this->m_effects.append(new UIEffect(this, i));
    }
    // non-ui effects are not necessary
    //effects.clear();
}

int UIField::idGet() const {
    return id;
}

QString UIField::nameGet() const {
    return name;
}

int UIField::priceGet() const {
    return price;
}

int UIField::upgradePriceGet() const {
    return upgradePrice;
}

QColor UIField::colorGet() const {
    return color;
}

UIField::Type UIField::typeGet() const {
    return (UIField::Type) type;
}

UIOpponent *UIField::ownerGet() {
    return m_owner;
}

void UIField::ownerSet(UIOpponent *owner) {
    if (m_owner != owner) {
        m_owner = owner;
        emit ownerChanged();
    }
}

Board::Board(QObject *parent)
    : QObject(parent)
{
    QList<Field> f {
        #include "../def/fields.def"
    };
    for (auto i : f) {
        m_fields.append(new UIField(this, i));
    }
}

QQmlListProperty<UIField> Board::fields() {
    return QQmlListProperty<UIField>(this, m_fields);
}

QList<UIField *> &Board::fieldList() {
    return m_fields;
}

int Board::fieldCount() const {
    return m_fields.count();
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
    return !password.isEmpty();
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

void Client::setReady(bool val) {
    qCritical() << "This player ID is" << m_thisPlayerId;
    if (m_thisPlayerId > 0) {
        qCritical() << "Trying to set ready to" << val;
        m_dataStream << Packet(QList<Opponent>{{m_thisPlayerId, {}, {}, -1, -1, {}, val, {}}});
    }
}

void Client::startGame() {
    m_dataStream << Packet(GameState{});
}

void Client::move(int id, int position) {
    m_dataStream << Packet(QList<Opponent>{{id, {}, {}, -1, position, {}, {}, {}}});
}

void Client::buy(int id) {
    m_dataStream << Packet(QList<Ownership>{Ownership{m_thisPlayerId, id}});
}

void Client::drawCard(int type) {
    if (type == Effect::DRAW_CHANCE) {
        m_dataStream << Packet(Card{"chance", {}});
    }
    else if (type == Effect::DRAW_FINANCE) {
        m_dataStream << Packet(Card{"finance", {}});
    }
    else {
        qCritical() << "DrawCard called with type" << type << "instead of" << (int)Effect::DRAW_CHANCE << "or" << (int)Effect::DRAW_FINANCE;
    }
}

void Client::giveMoney(int amount) {
    int currentMoney = 0;
    for (auto i : m_opponents) {
        if (i->you)
            currentMoney = i->money;
    }
    m_dataStream << Packet(QList<Opponent>{{m_thisPlayerId, {}, {}, currentMoney - amount, -1, {}, {}, {}}});
}

void Client::takeMoney(int amount) {
    int currentMoney = 0;
    for (auto i : m_opponents) {
        if (i->you)
            currentMoney = i->money;
    }
    m_dataStream << Packet(QList<Opponent>{{m_thisPlayerId, {}, {}, currentMoney + amount, -1, {}, {}, {}}});
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

UIOwnership::UIOwnership(UIOpponent *parent, int id, QColor color, bool owns)
    : QObject(parent)
    , m_id(id)
    , m_color(color)
    , m_owns(owns)
{
}

UIOpponent::UIOpponent(Client *parent, const Opponent &data)
    : QObject(parent)
    , Opponent(data)
{
    for (auto &i : client()->boardGet()->fieldList()) {
        if (i->typeGet() == UIField::TRAINER) {
            m_owns.append(new UIOwnership(this, i->id, i->colorGet(), false));
        }
    }
    for (auto &i : client()->boardGet()->fieldList()) {
        if (i->typeGet() == UIField::HORSE) {
            m_owns.append(new UIOwnership(this, i->id, i->colorGet(), false));
        }
    }
    for (auto &i : client()->boardGet()->fieldList()) {
        if (i->typeGet() == UIField::TRANSPORT) {
            m_owns.append(new UIOwnership(this, i->id, i->colorGet(), false));
        }
    }
}

Client *UIOpponent::client() {
    return qobject_cast<Client*>(parent());
}
