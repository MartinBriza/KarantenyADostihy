#include "datamodel.h"

#include "game.h"

namespace UI {

Effect::Effect(QObject *parent, const ::Effect &effect)
    : QObject(parent)
    , ::Effect(effect)
{

}

int Effect::amountGet() const {
    return amount;
}

int Effect::secondaryAmountGet() const {
    return secondaryAmount;
}

Card::Card(QObject *parent, const QString &text, const ::Effect &effect)
    : QObject(parent)
    , ::Effect(effect)
    , text(text)
{
   uiEffect = new UI::Effect(this, effect);
}

Field::Field(QObject *parent, const ::Field &field)
    : QObject(parent)
    , ::Field(field)
{
    for (auto i : field.effects) {
        this->m_effects.append(new Effect(this, i));
    }
    // non-ui effects are not necessary
    //effects.clear();
}

int Field::idGet() const {
    return id;
}

QString Field::nameGet() const {
    return name;
}

int Field::priceGet() const {
    return price;
}

int Field::upgradePriceGet() const {
    return upgradePrice;
}

QColor Field::colorGet() const {
    return color;
}

Field::Type Field::typeGet() const {
    return (Field::Type) type;
}

Opponent *Field::ownerGet() {
    return m_owner;
}

void Field::ownerSet(Opponent *owner) {
    if (m_owner != owner) {
        m_owner = owner;
        emit ownerChanged();
    }
}

Board::Board(QObject *parent)
    : QObject(parent)
{
    QList<::Field> f {
        #include "../def/fields.def"
    };
    for (auto i : f) {
        m_fields.append(new Field(this, i));
    }
}

QQmlListProperty<Field> Board::fields() {
    return QQmlListProperty<Field>(this, m_fields);
}

QList<Field *> &Board::fieldList() {
    return m_fields;
}

int Board::fieldCount() const {
    return m_fields.count();
}

Match::Match(Roster *parent, const QString &name, const QString &owner)
    : QObject(parent)
    , ::Match()
{
    this->name = name;
    this->owner = owner;
}

Match::Match(Roster *parent, const ::Match &data)
    : QObject(parent)
    , ::Match(data)
{

}

Roster *Match::roster() {
    return qobject_cast<Roster*>(parent());
}

Client *Match::client() {
    return roster()->client();
}

int Match::idGet() const {
    return id;
}

bool Match::passwordGet() const {
    return !password.isEmpty();
}

QString Match::nameGet() const {
    return name;
}

QString Match::ownerGet() const {
    return owner;
}

int Match::playersGet() const {
    return players;
}

int Match::maximumPlayersGet() const {
    return maximumPlayers;
}

void Match::join(const QString password) {
    client()->join(id, password);
}

Roster::Roster(Client *parent)
    : QObject(parent)
{

}

Client *Roster::client() {
    return qobject_cast<Client*>(parent());
}

QQmlListProperty<Match> Roster::matches() {
    return QQmlListProperty<Match>(this, m_matches);
}

int Roster::matchCount() const {
    return m_matches.count();
}

void Roster::create() {
    client()->create();
}

void Roster::regenerate(const ::Roster &data) {
    for (auto i : m_matches)
        i->deleteLater();
    m_matches.clear();
    emit matchesChanged();
    for (auto &i : data.matches) {
        m_matches.append(new Match(this, i));
    }
    emit matchesChanged();
}

Ownership::Ownership(Opponent *parent, int id, QColor color, bool owns)
    : QObject(parent)
    , m_id(id)
    , m_color(color)
    , m_owns(owns)
{
}

Opponent::Opponent(Client *parent, const ::Opponent &data)
    : QObject(parent)
    , ::Opponent(data)
{
    for (auto &i : client()->boardGet()->fieldList()) {
        if (i->typeGet() == Field::TRAINER) {
            m_owns.append(new Ownership(this, i->id, i->colorGet(), false));
        }
    }
    for (auto &i : client()->boardGet()->fieldList()) {
        if (i->typeGet() == Field::HORSE) {
            m_owns.append(new Ownership(this, i->id, i->colorGet(), false));
        }
    }
    for (auto &i : client()->boardGet()->fieldList()) {
        if (i->typeGet() == Field::TRANSPORT) {
            m_owns.append(new Ownership(this, i->id, i->colorGet(), false));
        }
    }
}

Client *Opponent::client() {
    return qobject_cast<Client*>(parent());
}

};
