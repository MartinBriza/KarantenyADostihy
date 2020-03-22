#ifndef GAME_H
#define GAME_H

#include "protocol.h"

#include <QObject>
#include <QtQml>
#include <QColor>

#include <QTcpSocket>

class Effect;
class UICard;
class Field;
class Player;
class Board;
class Game;
class UIRoster;
class Client;
class UIOpponent;

inline int lastCardID { 1 };

class UIEffect : public QObject, public Effect {
    Q_OBJECT
    Q_PROPERTY(int amount READ amountGet CONSTANT)
    Q_PROPERTY(int secondaryAmount READ secondaryAmountGet CONSTANT)
public:
    Q_ENUM(Target);
    Q_ENUM(Action);
    UIEffect(QObject *parent = nullptr, const Effect &effect = {});
    int amountGet() const;
    int secondaryAmountGet() const;
};


class UICard : public QObject, public Effect {
    Q_OBJECT
public:
    UICard(QObject *parent = nullptr, const QString &text = {}, const Effect &effect = {});
private:
    QString text;
    UIEffect *uiEffect { nullptr };
};


class UIField : public QObject, public Field {
    Q_OBJECT
    Q_PROPERTY(int id READ idGet CONSTANT)
    Q_PROPERTY(QString name READ nameGet CONSTANT)
    Q_PROPERTY(QColor color READ colorGet CONSTANT)
    Q_PROPERTY(int price READ priceGet CONSTANT)
    Q_PROPERTY(int upgradePrice READ upgradePriceGet CONSTANT)
    Q_PROPERTY(Type type READ typeGet CONSTANT)
    Q_PROPERTY(UIOpponent* owner READ ownerGet NOTIFY ownerChanged)
    //Q_PROPERTY(int fee READ feeGet NOTIFY feeChanged)
    Q_PROPERTY(QQmlListProperty<UIEffect> effects READ effectsGet CONSTANT)
public:
    UIField(QObject *parent = nullptr, const Field &field = {});
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
    UIOpponent *ownerGet();
    void ownerSet(UIOpponent *owner);
    QQmlListProperty<UIEffect> effectsGet() {
        return QQmlListProperty<UIEffect>(this, m_effects);
    }
signals:
    void feeChanged();
    void ownerChanged();
private:
    QList<UIEffect*> m_effects;
    int m_upgrade;
    QPointer<UIOpponent> m_owner { nullptr };
};
Q_DECLARE_METATYPE(UIField::Type);


class Board : public QObject {
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<UIField> fields READ fields CONSTANT)
    Q_PROPERTY(int fieldCount READ fieldCount CONSTANT)
public:
    Board(QObject *parent = nullptr);

    QQmlListProperty<UIField> fields();
    QList<UIField*> &fieldList();
    int fieldCount() const;

private:
    QList<UIField*> m_fields {
    };
};

class UIMatch : public QObject, protected Match {
    Q_OBJECT
    Q_PROPERTY(int id READ idGet CONSTANT)
    Q_PROPERTY(bool password READ passwordGet CONSTANT)
    Q_PROPERTY(QString name READ nameGet CONSTANT)
    Q_PROPERTY(QString owner READ ownerGet CONSTANT)
    Q_PROPERTY(int players READ playersGet CONSTANT)
    Q_PROPERTY(int maximumPlayers READ maximumPlayersGet CONSTANT)
public:
    UIMatch(UIRoster *parent = nullptr, const QString &name = {}, const QString &owner = {});
    UIMatch(UIRoster *parent, const Match &data);

    UIRoster *roster();
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

class UIRoster : public QObject {
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<UIMatch> matches READ matches NOTIFY matchesChanged)
    Q_PROPERTY(int matchCount READ matchCount NOTIFY matchesChanged)
public:
    UIRoster(Client *parent);

    Client *client();

    QQmlListProperty<UIMatch> matches();
    int matchCount() const;
public slots:
    void create();

public slots:
    void regenerate(const Roster &data);

signals:
    void matchesChanged();

private:
    QList<UIMatch*> m_matches { };
};

class UILobby : public QObject {
    Q_OBJECT
    Q_PROPERTY(int id READ idGet CONSTANT)
    Q_PROPERTY(QString name READ nameGet WRITE nameSet NOTIFY nameChanged)
public:
    UILobby(QObject *parent, int id = {}, const QString &name = {})
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

class UIChat : public QObject, public Chat {
    Q_OBJECT
    Q_PROPERTY(QString time READ timeGet CONSTANT)
    Q_PROPERTY(QString from READ fromGet CONSTANT)
    Q_PROPERTY(QString message READ messageGet CONSTANT)
    Q_PROPERTY(int urgency READ urgencyGet CONSTANT)
public:
    UIChat(QObject *parent, const Chat &data)
        : QObject(parent)
        , Chat(data)
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

class UIOwnership : public QObject {
    Q_OBJECT
    Q_PROPERTY(int id READ idGet CONSTANT)
    Q_PROPERTY(QColor color READ colorGet CONSTANT)
    Q_PROPERTY(bool owns READ ownsGet NOTIFY ownsChanged)
public:
    UIOwnership(UIOpponent *parent = nullptr, int id = 0, QColor color = Qt::white, bool owns = false);

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

class UIOpponent : public QObject, public Opponent {
    Q_OBJECT
    Q_PROPERTY(int id READ idGet CONSTANT)
    Q_PROPERTY(QString name READ nameGet NOTIFY nameChanged)
    Q_PROPERTY(QColor color READ colorGet NOTIFY colorChanged)
    Q_PROPERTY(int money READ moneyGet NOTIFY moneyChanged)
    Q_PROPERTY(int position READ positionGet NOTIFY positionChanged)
    Q_PROPERTY(bool leader READ leaderGet NOTIFY leaderChanged)
    Q_PROPERTY(bool ready READ readyGet NOTIFY readyChanged)
    Q_PROPERTY(bool you READ youGet NOTIFY youChanged)
    Q_PROPERTY(QQmlListProperty<UIOwnership> owns READ ownsGet CONSTANT)
    Q_PROPERTY(int ownsCount READ ownsCountGet CONSTANT)
public:
    UIOpponent(Client *parent, const Opponent &data);

    Client *client();

    void update(const Opponent &data) {
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
    QQmlListProperty<UIOwnership> ownsGet() {
        return QQmlListProperty<UIOwnership>(this, m_owns);
    }
    int ownsCountGet() {
        return m_owns.count();
    }
public slots:
    void updateOwnerships(const QList<Ownership> &list) {
        for (auto i : m_owns) {
            i->ownsSet(false);
        }
        for (auto i : list) {
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
    QList<UIOwnership*> m_owns { };
};

class Client : public QObject {
    Q_OBJECT
    Q_PROPERTY(UIRoster* roster READ roster CONSTANT)
    Q_PROPERTY(QString name READ nameGet WRITE nameSet NOTIFY nameChanged)
    Q_PROPERTY(State state READ stateGet NOTIFY stateChanged)
    Q_PROPERTY(UILobby *lobby READ lobbyGet NOTIFY lobbyChanged)
    Q_PROPERTY(QQmlListProperty<UIChat> chat READ chatGet NOTIFY chatChanged)
    Q_PROPERTY(QQmlListProperty<UIOpponent> opponents READ opponentsGet NOTIFY opponentsChanged)
    Q_PROPERTY(int thisPlayerId READ thisPlayerId NOTIFY thisPlayerIdChanged)
    Q_PROPERTY(Board *board READ boardGet CONSTANT)
public:
    enum State {
        ROSTER,
        LOBBY,
        GAME
    } m_state { ROSTER };
    Q_ENUM(State)
    State stateGet() { return m_state; }

    Client(QObject *parent = nullptr)
        : QObject(parent)
        , m_socket(new QTcpSocket(this))
        , m_dataStream(m_socket)
        , m_roster(new UIRoster(this))
        , m_refreshTimer()
    {
        m_refreshTimer.setInterval(5000);
        m_refreshTimer.setSingleShot(false);
        connect(&m_refreshTimer, &QTimer::timeout, this, &Client::refreshRoster);
        connect(m_socket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
        connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &Client::onError);
        connect(m_socket, &QTcpSocket::stateChanged, this, &Client::onSocketStateChanged);
        QTimer::singleShot(0, [this]() {
            m_socket->connectToHost("46.36.35.81", 16543);
            m_refreshTimer.start();
        });
    }

    Board *boardGet() {
        return m_board;
    }

    UIRoster *roster() {
        return m_roster;
    }
    UILobby *lobbyGet() {
        return m_lobby;
    }

    QString nameGet() const {
        return m_name;
    }
    void nameSet(const QString &name) {
        if (m_name != name) {
            m_name = name;
            emit nameChanged();
            m_dataStream << Packet(Ahoj{m_name});
        }
    }

    int thisPlayerId() const {
        return m_thisPlayerId;
    }

    QQmlListProperty<UIChat> chatGet() {
        return QQmlListProperty<UIChat>(this, m_chat);
    }
    QQmlListProperty<UIOpponent> opponentsGet() {
        return QQmlListProperty<UIOpponent>(this, m_opponents);
    }

    static QString randomName(int count = 8);

public slots:
    void join(int id, const QString &password);
    void leave();
    void create(const QString &name = {});
    void sendMessage(const QString &message);
    void refreshRoster();
    void setReady(bool val);
    void startGame();
    void move(int id, int position);
    void buy(int id);
    void drawCard(int type);
    void giveMoney(int amount);
    void takeMoney(int amount);

private slots:
    void onSocketStateChanged() {
        if (m_socket->state() == QAbstractSocket::ConnectedState) {
            m_dataStream << Packet(Ahoj{m_name});
        }
    }
    void onReadyRead() {
        while(m_socket->bytesAvailable() > 0) {
            Packet p;
            m_dataStream >> p;
            switch (p.type) {
            case Packet::AHOJ:
                m_dataStream << Packet(Ahoj{m_name});
                break;
            case Packet::ERROR:
                qCritical() << "SERVER ERROR: " << p.error;
                emit serverError(p.error);
                break;
            case Packet::ROSTER:
                if (m_dataStream.status() == QDataStream::Ok)
                    m_roster->regenerate(p.roster);
                break;
            case Packet::ENTERED: {
                if (p.entered.id >= 0) {
                    qCritical() << "Entering a room";
                    m_lobby = new UILobby(this, p.entered.id, p.entered.name);
                    emit lobbyChanged();
                    m_state = LOBBY;
                    emit stateChanged();
                }
                else {
                    m_lobby->deleteLater();
                    m_lobby = nullptr;
                    emit lobbyChanged();
                    m_state = ROSTER;
                    emit stateChanged();
                }
                break;
            }
            case Packet::OPPONENTS: {
                qCritical() << "OPPONENTS!";
                QSet<int> oldIDs;
                QSet<int> newIDs;
                for (auto i : m_opponents)
                    oldIDs.insert(i->id);
                for (auto i : p.opponents)
                    newIDs.insert(i.id);
                if (newIDs != oldIDs) {
                    m_opponents.clear();
                    for (auto &i : p.opponents) {
                        m_opponents.append(new UIOpponent(this, i));
                        if (i.you && m_thisPlayerId != i.id) {
                            m_thisPlayerId = i.id;
                            emit thisPlayerIdChanged();
                        }
                    }
                    emit opponentsChanged();
                }
                else {
                    for (auto old : m_opponents) {
                        for (auto refresh : p.opponents) {
                            if (old->id == refresh.id) {
                                old->update(refresh);
                            }
                        }
                    }
                }
                break;
            }
            case Packet::CHAT:
                qCritical()  << "SPAM";
                m_chat.append(new UIChat(this, p.chat));
                emit chatChanged();
                break;
            case Packet::GAMESTATE: {
                m_state = GAME;
                emit stateChanged();
                break;
            }
            case Packet::OWNERSHIPS: {
                for (auto ownership : p.ownerships) {
                    for (auto field : m_board->fieldList()) {
                        if (ownership.card == field->id) {
                            for (auto opponent : m_opponents) {
                                if (ownership.player == opponent->id) {
                                    field->ownerSet(opponent);
                                }
                            }
                        }
                    }
                }
                for (auto opponent : m_opponents) {
                    opponent->updateOwnerships(p.ownerships);
                }
                break;
            }
            case Packet::CARD: {
                emit displayCard(p.card.name);
                break;
            }
            }
        }
    }
    void onError(QAbstractSocket::SocketError err) {
        qCritical() << "ERROR:" << m_socket->errorString();
    }
signals:
    void nameChanged();
    void stateChanged();
    void lobbyChanged();
    void chatChanged();
    void opponentsChanged();
    void thisPlayerIdChanged();
    void serverError(const QString &message);
    void displayCard(const QString &message);

private:
    QTcpSocket *m_socket { nullptr };
    QDataStream m_dataStream;
    UIRoster *m_roster { nullptr };
    QString m_name { randomName() };
    UILobby *m_lobby { nullptr };
    QList<UIChat*> m_chat {};
    QList<UIOpponent*> m_opponents {};
    QTimer m_refreshTimer;
    int m_thisPlayerId { -1 };
    Board *m_board { new Board(this) };
};

#endif // GAME_H
