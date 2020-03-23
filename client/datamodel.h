#ifndef DATAMODEL_H
#define DATAMODEL_H

#include "protocol.h"

#include <QtQml>

class Client;

namespace UI {

class Effect;
class Card;
class Field;
class Player;
class Board;
class Game;
class Roster;
class Opponent;

inline int lastCardID { 1 };

class Effect : public QObject, public ::Effect {
    Q_OBJECT
    Q_PROPERTY(int amount READ amountGet CONSTANT)
    Q_PROPERTY(int secondaryAmount READ secondaryAmountGet CONSTANT)
public:
    Q_ENUM(Target);
    Q_ENUM(Action);
    Effect(QObject *parent = nullptr, const ::Effect &effect = {});
    int amountGet() const;
    int secondaryAmountGet() const;
};


class Card : public QObject, public ::Effect {
    Q_OBJECT
public:
    Card(QObject *parent = nullptr, const QString &text = {}, const Effect &effect = {});
private:
    QString text;
    Effect *uiEffect { nullptr };
};


class Field : public QObject, public ::Field {
    Q_OBJECT
    Q_PROPERTY(int id READ idGet CONSTANT)
    Q_PROPERTY(QString name READ nameGet CONSTANT)
    Q_PROPERTY(QColor color READ colorGet CONSTANT)
    Q_PROPERTY(int price READ priceGet CONSTANT)
    Q_PROPERTY(int upgradePrice READ upgradePriceGet CONSTANT)
    Q_PROPERTY(Type type READ typeGet CONSTANT)
    Q_PROPERTY(Opponent* owner READ ownerGet NOTIFY ownerChanged)
    //Q_PROPERTY(int fee READ feeGet NOTIFY feeChanged)
    Q_PROPERTY(QQmlListProperty<UI::Effect> effects READ effectsGet CONSTANT)
public:
    Field(QObject *parent = nullptr, const ::Field &field = {});
    // This has to be the same as Field::Type because I couldn't figure out how to pass it to QML
    // (or ideally, it could be just fixed)
    enum Type {
        BASIC = 0,
        HORSE,
        TRAINER,
        TRANSPORT,
        DECK
    };
    Q_ENUM(Type)

    int idGet() const;
    QString nameGet() const;
    int priceGet() const;
    int upgradePriceGet() const;
    QColor colorGet() const;
    Type typeGet() const;
    UI::Opponent *ownerGet();
    void ownerSet(UI::Opponent *owner);
    QQmlListProperty<UI::Effect> effectsGet() {
        return QQmlListProperty<UI::Effect>(this, m_effects);
    }
signals:
    void feeChanged();
    void ownerChanged();
private:
    QList<Effect*> m_effects;
    int m_upgrade;
    QPointer<Opponent> m_owner { nullptr };
};


class Board : public QObject {
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<UI::Field> fields READ fields CONSTANT)
    Q_PROPERTY(int fieldCount READ fieldCount CONSTANT)
public:
    Board(QObject *parent = nullptr);

    QQmlListProperty<UI::Field> fields();
    QList<Field*> &fieldList();
    int fieldCount() const;

private:
    QList<Field*> m_fields {
    };
};

class Match : public QObject, protected ::Match {
    Q_OBJECT
    Q_PROPERTY(int id READ idGet CONSTANT)
    Q_PROPERTY(bool password READ passwordGet CONSTANT)
    Q_PROPERTY(QString name READ nameGet CONSTANT)
    Q_PROPERTY(QString owner READ ownerGet CONSTANT)
    Q_PROPERTY(int players READ playersGet CONSTANT)
    Q_PROPERTY(int maximumPlayers READ maximumPlayersGet CONSTANT)
public:
    Match(Roster *parent = nullptr, const QString &name = {}, const QString &owner = {});
    Match(Roster *parent, const ::Match &data);

    Roster *roster();
    Client *client();

    int idGet() const;
    bool passwordGet() const;
    QString nameGet() const;
    QString ownerGet() const;
    int playersGet() const;
    int maximumPlayersGet() const;

public slots:
    void join(const QString password);

private:
};

class Roster : public QObject {
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<UI::Match> matches READ matches NOTIFY matchesChanged)
    Q_PROPERTY(int matchCount READ matchCount NOTIFY matchesChanged)
public:
    Roster(Client *parent);

    Client *client();

    QQmlListProperty<UI::Match> matches();
    int matchCount() const;
public slots:
    void create();

public slots:
    void regenerate(const ::Roster &data);

signals:
    void matchesChanged();

private:
    QList<Match*> m_matches { };
};

class Lobby : public QObject {
    Q_OBJECT
    Q_PROPERTY(int id READ idGet CONSTANT)
    Q_PROPERTY(QString name READ nameGet WRITE nameSet NOTIFY nameChanged)
public:
    Lobby(QObject *parent, int id = {}, const QString &name = {})
        : QObject(parent)
        , m_id(id)
        , m_name(name)
    {

    }
    int idGet() const {
        return m_id;
    }
    QString nameGet() const {
        return m_name;
    }
    void nameSet(const QString &val) {
        if (m_name != val) {
            m_name = val;
            emit nameChanged();
        }
    }

signals:
    void nameChanged();

private:
    int m_id;
    QString m_name;
};

class Chat : public QObject, public ::Chat {
    Q_OBJECT
    Q_PROPERTY(QString time READ timeGet CONSTANT)
    Q_PROPERTY(QString from READ fromGet CONSTANT)
    Q_PROPERTY(QString message READ messageGet CONSTANT)
    Q_PROPERTY(int urgency READ urgencyGet CONSTANT)
public:
    Chat(QObject *parent, const ::Chat &data)
        : QObject(parent)
        , ::Chat(data)
    {

    }
    QString timeGet() const {
        return time;
    }
    QString fromGet() const {
        return from;
    }
    QString messageGet() const {
        return message;
    }
    int urgencyGet() const {
        return urgency;
    }
};

class Ownership : public QObject {
    Q_OBJECT
    Q_PROPERTY(int id READ idGet CONSTANT)
    Q_PROPERTY(QColor color READ colorGet CONSTANT)
    Q_PROPERTY(bool owns READ ownsGet NOTIFY ownsChanged)
public:
    Ownership(Opponent *parent = nullptr, int id = 0, QColor color = Qt::white, bool owns = false);

    int idGet() {
        return m_id;
    }
    QColor colorGet() {
        return m_color;
    }
    bool ownsGet() {
        return m_owns;
    }
    void ownsSet(bool val) {
        if (m_owns != val) {
            m_owns = val;
            emit ownsChanged();
        }
    }

signals:
    void ownsChanged();
private:
    int m_id { 0 };
    QColor m_color {};
    bool m_owns { false };
};

class Opponent : public QObject, public ::Opponent {
    Q_OBJECT
    Q_PROPERTY(int id READ idGet CONSTANT)
    Q_PROPERTY(QString name READ nameGet NOTIFY nameChanged)
    Q_PROPERTY(QColor color READ colorGet NOTIFY colorChanged)
    Q_PROPERTY(int money READ moneyGet NOTIFY moneyChanged)
    Q_PROPERTY(int position READ positionGet NOTIFY positionChanged)
    Q_PROPERTY(bool leader READ leaderGet NOTIFY leaderChanged)
    Q_PROPERTY(bool ready READ readyGet NOTIFY readyChanged)
    Q_PROPERTY(bool you READ youGet NOTIFY youChanged)
    Q_PROPERTY(QQmlListProperty<UI::Ownership> owns READ ownsGet CONSTANT)
    Q_PROPERTY(int ownsCount READ ownsCountGet CONSTANT)
public:
    Opponent(Client *parent, const ::Opponent &data);

    Client *client();

    void update(const ::Opponent &data) {
        if (name != data.name) {
            name = data.name;
            emit nameChanged();
        }
        if (color != data.color) {
            color = data.color;
            emit colorChanged();
        }
        if (money != data.money) {
            money = data.money;
            emit moneyChanged();
        }
        if (position != data.position) {
            position = data.position;
            emit positionChanged();
        }
        if (leader != data.leader) {
            leader = data.leader;
            emit leaderChanged();
        }
        if (ready != data.ready) {
            ready = data.ready;
            emit readyChanged();
        }
        if (you != data.you) {
            you = data.you;
            emit youChanged();
        }
    }
    int idGet() const {
        return id;
    }
    QString nameGet() const {
        return name;
    }
    QColor colorGet() const {
        return color;
    }
    int moneyGet() const {
        return money;
    }
    int positionGet() const {
        return position;
    }
    bool leaderGet() const {
        return leader;
    }
    bool readyGet() const {
        return ready;
    }
    bool youGet() const {
        return you;
    }
    QQmlListProperty<UI::Ownership> ownsGet() {
        return QQmlListProperty<UI::Ownership>(this, m_owns);
    }
    int ownsCountGet() {
        return m_owns.count();
    }
public slots:
    void updateOwnerships(const QList<::Ownership> &list) {
        for (auto i : m_owns) {
            i->ownsSet(false);
        }
        for (auto &i : list) {
            if (i.player == idGet()) {
                for (auto o : m_owns) {
                    if (o->idGet() == i.card) {
                        o->ownsSet(true);
                    }
                }
            }
        }
    }
signals:
    void nameChanged();
    void colorChanged();
    void moneyChanged();
    void positionChanged();
    void leaderChanged();
    void readyChanged();
    void youChanged();
private:
    QList<Ownership*> m_owns { };
};

};
Q_DECLARE_METATYPE(UI::Field::Type);

#endif // DATAMODEL_H
