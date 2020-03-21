#ifndef GAME_H
#define GAME_H

#include "protocol.h"

#include <QObject>
#include <QtQml>
#include <QColor>

#include <QTcpSocket>

class Effect;
class Card;
class Field;
class Player;
class Board;
class Game;
class UIRoster;
class Client;

class Effect : public QObject {
    Q_OBJECT
    Q_PROPERTY(int amount READ amount CONSTANT)
    Q_PROPERTY(int secondaryAmount READ secondaryAmount CONSTANT)
public:
    enum Target {
        NO_TARGET = 0,
        PLAYER,
        OTHER_PLAYERS,
        ALL_PLAYERS,
    };
    Q_ENUMS(Target);
    enum Action {
        NO_ACTION = 0,
        DRAW_CHANCE,
        DRAW_FINANCE,
        FEE,
        FEE_PER_RACE,
        FEE_PER_STEP,
        GAIN,
        TRANSFER,
        WAIT,
        MOVE_STEPS,
        CANCEL_SUSPENSION,
        MOVE_TO_TRAINER,
        MOVE_TO_SUSPENSION,
        MOVE_TO_FINANCE,
        MOVE_TO_LAST_FIELD,
        MOVE_TO_FIRST_FIELD,
        MOVE_TO_PARKING_LOT
    };
    Q_ENUMS(Action);
    Effect(QObject *parent = nullptr, Target target = NO_TARGET, Action action = NO_ACTION, int amount = 0, int secondaryAmount = 0);
    int amount() const;
    int secondaryAmount() const;
private:
    Target m_target;
    Action m_effect;
    int m_amount;
    int m_secondaryAmount;
};


class Card : public Effect {
    Q_OBJECT
public:
    Card(QObject *parent = nullptr, const QString &text = {}, Target target = NO_TARGET, Action action = NO_ACTION, int amount = 0, int secondaryAmount = 0);
private:
    QString m_text;
};


class Field : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QColor color READ color CONSTANT)
    Q_PROPERTY(int price READ price CONSTANT)
    Q_PROPERTY(int fee READ fee NOTIFY feeChanged)
public:
    Field(QObject *parent = nullptr, const QString &name = {}, int price = 0, QList<Effect*> effect = {}, QColor color = Qt::white, int upgrade = -1);

    QString name() const;
    int price() const;
    int fee() const;
    QColor color() const;
signals:
    void feeChanged();
private:
    QString m_name;
    QList<Effect*> m_effect;
    int m_price;
    QColor m_color;
    int m_upgrade;
};


class Board : public QObject {
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Field> fields READ fields CONSTANT)
    Q_PROPERTY(int fieldCount READ fieldCount CONSTANT)
public:
    Board(QObject *parent = nullptr);

    QQmlListProperty<Field> fields();
    int fieldCount() const;

private:
    QList<Field*> m_fields {
        #include "../def/fields.def"
    };
};

class Player : public QObject {
    Q_OBJECT
    Q_PROPERTY(int position READ position NOTIFY positionChanged)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QColor color READ color NOTIFY colorChanged)
    Q_PROPERTY(int money READ money NOTIFY moneyChanged)
public:
    Player(Game *parent = nullptr, const QString &name = {}, int money = 30000, const QColor &color = Qt::red);

    Game *game();
    Board *board();

    int position() const;
    QString name() const;
    int money() const;
    QColor color() const;

public slots:
    void move(int by);

signals:
    void positionChanged();
    void nameChanged();
    void moneyChanged();
    void colorChanged();

private:
    int m_position { 0 };
    QString m_name;
    int m_money;
    QColor m_color;
};

class Game : public QObject {
    Q_OBJECT
    Q_PROPERTY(Board *board READ board CONSTANT)
    Q_PROPERTY(QQmlListProperty<Player> players READ players NOTIFY playersChanged)
public:
    Game(QObject *parent = nullptr);

    Board *board();
    QQmlListProperty<Player> players();

signals:
    void playersChanged();

private:
    Board *m_board { new Board(this) };
    QList<Player*> m_players;

    QList<Card*> m_financeCards {
        #include "../def/finance.def"
    };

    QList<Card*> m_chanceCards {
        #include "../def/chance.def"
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

class UIOpponent : public QObject, public Opponent {
    Q_OBJECT
    Q_PROPERTY(QString name READ nameGet CONSTANT)
    Q_PROPERTY(QColor color READ colorGet CONSTANT)
    Q_PROPERTY(int money READ moneyGet CONSTANT)
    Q_PROPERTY(bool leader READ leaderGet CONSTANT)
    Q_PROPERTY(bool ready READ readyGet CONSTANT)
    Q_PROPERTY(bool you READ youGet CONSTANT)
public:
    UIOpponent(QObject *parent, const Opponent &data)
        : QObject(parent)
        , Opponent(data)
    {

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
    bool leaderGet() const {
        return leader;
    }
    bool readyGet() const {
        return ready;
    }
    bool youGet() const {
        return you;
    }
};

class Client : public QObject {
    Q_OBJECT
    Q_PROPERTY(UIRoster* roster READ roster CONSTANT)
    Q_PROPERTY(QString name READ nameGet WRITE nameSet NOTIFY nameChanged)
    Q_PROPERTY(State state READ stateGet NOTIFY stateChanged)
    Q_PROPERTY(UILobby *lobby READ lobbyGet NOTIFY lobbyChanged)
    Q_PROPERTY(QQmlListProperty<UIChat> chat READ chatGet NOTIFY chatChanged)
    Q_PROPERTY(QQmlListProperty<UIOpponent> opponents READ opponentsGet NOTIFY opponentsChanged)
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
    {
        connect(m_socket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
        connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &Client::onError);
        QTimer::singleShot(1000, [this]() {
            m_socket->connectToHost("127.0.0.1", 16543);
        });
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

    QQmlListProperty<UIChat> chatGet() {
        return QQmlListProperty<UIChat>(this, m_chat);
    }
    QQmlListProperty<UIOpponent> opponentsGet() {
        return QQmlListProperty<UIOpponent>(this, m_opponents);
    }

    static QString randomName(int count = 8);

public slots:
    void join(int id, const QString &password);
    void create(const QString &name = {});
    Q_INVOKABLE void sendMessage(const QString &message);

private slots:
    void onReadyRead() {
        while(m_socket->bytesAvailable() > 0) {
            qCritical() << "READY READ";
            Packet p;
            m_dataStream >> p;
            qCritical() << "Packet type " << p.type;
            switch (p.type) {
            case Packet::AHOJ:
                qCritical() << "Got ahoj, sending ahoj";
                m_dataStream << Packet(Ahoj{m_name});
                break;
            case Packet::ERROR:
                qCritical() << "SERVER ERROR: " << p.error;
                break;
            case Packet::ROSTER:
                qCritical() << "GOT ROSTER";
                if (m_dataStream.status() == QDataStream::Ok)
                    m_roster->regenerate(p.roster);
                else
                    qCritical() << "SUMTIN WRONG: " << m_dataStream.status() << m_socket->errorString();
                break;
            case Packet::ENTERED:
                qCritical() << "Entering a room";
                m_lobby = new UILobby(this, p.entered.id, p.entered.name);
                emit lobbyChanged();
                m_state = LOBBY;
                emit stateChanged();
                break;
            case Packet::OPPONENTS:
                qCritical() << "OPPONENTS!";
                m_opponents.clear();
                for (auto &i : p.opponents) {
                    m_opponents.append(new UIOpponent(this, i));
                }
                emit opponentsChanged();
                break;
            case Packet::CHAT:
                qCritical()  << "SPAM";
                m_chat.append(new UIChat(this, p.chat));
                emit chatChanged();
                break;
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

private:
    QTcpSocket *m_socket { nullptr };
    QDataStream m_dataStream;
    UIRoster *m_roster { nullptr };
    QString m_name { randomName() };
    UILobby *m_lobby { nullptr };
    QList<UIChat*> m_chat {};
    QList<UIOpponent*> m_opponents {};
};

#endif // GAME_H
