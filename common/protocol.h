#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QDebug>
#include <QDataStream>
#include <QColor>
#include <QTime>

struct Effect {
    enum Target {
        NO_TARGET = 0,
        PLAYER,
        OTHER_PLAYERS,
        ALL_PLAYERS,
    };
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
    Target target { NO_TARGET };
    Action action { NO_ACTION };
    int amount { 0 };
    int secondaryAmount { 0 };
};
inline QDataStream &operator<<(QDataStream &str, const Effect &item) {
    str << (int&) item.target << (int) item.action << item.amount << item.secondaryAmount;
    return str;
}
inline QDataStream &operator>>(QDataStream &str, Effect &item) {
    str >> (int&) item.target >> (int&) item.action >> item.amount >> item.secondaryAmount;
    return str;
}

struct Field {
    enum Type {
        BASIC = 0,
        HORSE,
        TRAINER,
        TRANSPORT,
        DECK
    };
    Field(int id = 0, const QString &name = {}, int price = 0, const QList<Effect> &effects = {}, const QColor &color = Qt::white, int upgradePrice = 0, Type type = BASIC)
        : id(id), name(name), price(price), effects(effects), color(color), upgradePrice(upgradePrice), type(type)
    {

    }
    QString name {};
    QList<Effect> effects {};
    QColor color { Qt::white };
    int id { 0 };
    int price { 0 };
    int upgradePrice { 0 };
    Type type { BASIC };
};
inline QDataStream &operator<<(QDataStream &str, const Field &item) {
    str << item.id << item.name << item.effects << item.color << item.price << item.upgradePrice << (int) item.type;
    return str;
}
inline QDataStream &operator>>(QDataStream &str, Field &item) {
    str >> item.id >> item.name >> item.effects >> item.color >> item.price >> item.upgradePrice >> (int &) item.type;
    return str;
}

struct Ownership {
    int player;
    int card;
    int upgradeLevel;
};
inline QDataStream &operator<<(QDataStream &str, const Ownership &item) {
    str << item.player << item.card << item.upgradeLevel;
    return str;
}
inline QDataStream &operator>>(QDataStream &str, Ownership &item) {
    str >> item.player >> item.card >> item.upgradeLevel;
    return str;
}

struct Ahoj {
    QString ahoj { };
};
inline QDataStream &operator<<(QDataStream &str, const Ahoj &item) {
    str << item.ahoj;
    return str;
}
inline QDataStream &operator>>(QDataStream &str, Ahoj &item) {
    str >> item.ahoj;
    return str;
}

struct Match {
    QString password { };
    QString name { };
    QString owner { };
    int id { -1 };
    int players { 0 };
    int maximumPlayers { 8 };
    bool running { false };
    bool strict { true };
};
inline QDataStream &operator<<(QDataStream &str, const Match &item) {
    str << item.id << QString((item.password.isEmpty() ? QString("") : QString("***"))) << item.name << item.owner << item.players << item.maximumPlayers << item.running << item.strict;
    return str;
}
inline QDataStream &operator>>(QDataStream &str, Match &item) {
    str >> item.id >> item.password >> item.name >> item.owner >> item.players >> item.maximumPlayers >> item.running >> item.strict;
    return str;
}

struct Roster {
    QList<Match> matches {};
};
inline QDataStream &operator<<(QDataStream &str, const Roster &item) {
    str << item.matches;
    return str;
}
inline QDataStream &operator>>(QDataStream &str, Roster &item) {
    str >> item.matches;
    return str;
}

struct Join {
    int id { -1 };
    QString password { };
};
inline QDataStream &operator<<(QDataStream &str, const Join &item) {
    str << item.id << item.password;
    return str;
}
inline QDataStream &operator>>(QDataStream &str, Join &item) {
    str >> item.id >> item.password;
    return str;
}

struct Create {
    QString name {};
    QString password {};
    int capacity { 8 };
};
inline QDataStream &operator<<(QDataStream &str, const Create &item) {
    str << item.name << item.password << item.capacity;
    return str;
}
inline QDataStream &operator>>(QDataStream &str, Create &item) {
    str >> item.name >> item.password >> item.capacity;
    return str;
}

struct Entered {
    int id;
    QString name;
};
inline QDataStream &operator<<(QDataStream &str, const Entered &item) {
    str << item.id << item.name;
    return str;
}
inline QDataStream &operator>>(QDataStream &str, Entered &item) {
    str >> item.id >> item.name;
    return str;
}

struct Dice {
    QList<int> values;
    bool moved;
};
inline QDataStream &operator<<(QDataStream &str, const Dice &item) {
    str << item.values << item.moved;
    return str;
}
inline QDataStream &operator>>(QDataStream &str, Dice &item) {
    str >> item.values >> item.moved;
    return str;
}

struct Player {
    int id { 0 };
    QString name { "" };
    QColor color { Qt::red };
    int money { 0 };
    int position { 0 };
    bool leader { false };
    bool ready { false };
    bool you { false };
    bool onTurn { false };
    Dice dice;
    int waiting { 0 };
    bool ownsCancelSuspension { false };
};
inline QDataStream &operator<<(QDataStream &str, const Player &item) {
    str << item.id << item.name << item.color << item.money << item.leader << item.ready << item.you << item.position << item.onTurn << item.dice << item.waiting << item.ownsCancelSuspension;
    return str;
}
inline QDataStream &operator>>(QDataStream &str, Player &item) {
    str >> item.id >> item.name >> item.color >> item.money >> item.leader >> item.ready >> item.you >> item.position >> item.onTurn >> item.dice >> item.waiting >> item.ownsCancelSuspension;
    return str;
}

struct Chat {
    Chat(const QString &time, const QString &from, const QString &message, int urgency)
        : time(time), from(from), message(message), urgency(urgency)
    {}
    Chat(const QString &message = {})
        : time(QTime().toString())
        , from(QString())
        , message(message)
        , urgency(0)
    {

    }
    QString time {};
    QString from {};
    QString message {};
    int urgency {1};
};
inline QDataStream &operator<<(QDataStream &str, const Chat &item) {
    str << item.time << item.from << item.message << item.urgency;
    return str;
}
inline QDataStream &operator>>(QDataStream &str, Chat &item) {
    str >> item.time >> item.from >> item.message >> item.urgency;
    return str;
}

struct Card {
    QString header;
    QString name;
    Effect effect;
    bool operator<(const Card &r) const {
        return name < r.name;
    }
};
inline QDataStream &operator<<(QDataStream &str, const Card &item) {
    str << item.header << item.name << item.effect;
    return str;
}
inline QDataStream &operator>>(QDataStream &str, Card &item) {
    str >> item.header >> item.name >> item.effect;
    return str;
}

struct GameState {
    QMap<Card, int> cards;
};
inline QDataStream &operator<<(QDataStream &str, const GameState &item) {
    str << item.cards;
    return str;
}
inline QDataStream &operator>>(QDataStream &str, GameState &item) {
    str >> item.cards;
    return str;
}

struct Packet {
    enum Type {
        NONE = 0,
        ERROR,
        AHOJ,
        MATCH,
        ROSTER,
        JOIN,
        CREATE,
        ENTERED,
        PLAYERS,
        CHAT,
        GAMESTATE,
        OWNERSHIPS,
        CARD,
        DICE,
        EFFECT,
        _LAST_TYPE
    } type { NONE };
    union {
        QString error;
        Ahoj ahoj;
        Match match;
        Roster roster;
        Join join;
        Create create;
        Entered entered;
        QList<Player> players;
        Chat chat;
        GameState gameState;
        QList<Ownership> ownerships;
        Card card;
        Dice dice;
        Effect effect;
    };
    Packet() {}
    Packet(Type type, const QString &text) {
        setType(type);
        switch (type) {
        case ERROR:
            error = text;
            break;
        }
    }
    Packet(const Ahoj &d) : type(AHOJ), ahoj(d) { }
    Packet(const Match &d) : type(MATCH), match(d) { }
    Packet(const Roster &d) : type(ROSTER), roster(d) { }
    Packet(const Join &d) : type(JOIN), join(d) { }
    Packet(const Create &d) : type(CREATE), create(d) { }
    Packet(const Entered &d) : type(ENTERED), entered(d) { }
    Packet(const Chat &d) : type(CHAT), chat(d) { }
    Packet(const QList<Player> &d) : type(PLAYERS), players(d) { }
    Packet(const GameState &d) : type(GAMESTATE), gameState(d) { }
    Packet(const QList<Ownership> &d) : type(OWNERSHIPS), ownerships(d) { }
    Packet(const Card &d) : type(CARD), card(d) { }
    Packet(const Dice &d) : type(DICE), dice(d) { }
    Packet(const Effect &d) : type(EFFECT), effect(d) { }
    void setType(Type type) {
        clear();
        this->type = type;
        switch(this->type) {
        case NONE:
            break;
        case ERROR:
            new (&error) QString();
            break;
        case AHOJ:
            new (&ahoj) Ahoj();
            break;
        case MATCH:
            new (&match) Match();
            break;
        case ROSTER:
            new (&roster) Roster();
            break;
        case JOIN:
            new (&join) Join();
            break;
        case CREATE:
            new (&create) Create();
            break;
        case ENTERED:
            new (&entered) Entered();
            break;
        case PLAYERS:
            new (&players) QList<Player>();
            break;
        case CHAT:
            new (&chat) Chat();
            break;
        case GAMESTATE:
            new (&gameState) GameState();
            break;
        case OWNERSHIPS:
            new (&ownerships) QList<Ownership>();
            break;
        case CARD:
            new (&card) Card();
            break;
        case DICE:
            new (&dice) Dice();
            break;
        case EFFECT:
            new (&effect) Effect();
            break;
        }
    }
    void clear() {
        switch(type) {
        case NONE:
            break;
        case ERROR:
            error.~QString();
            break;
        case AHOJ:
            ahoj.~Ahoj();
            break;
        case MATCH:
            match.~Match();
            break;
        case ROSTER:
            roster.~Roster();
            break;
        case JOIN:
            join.~Join();
            break;
        case CREATE:
            create.~Create();
            break;
        case ENTERED:
            entered.~Entered();
            break;
        case PLAYERS:
            players.~QList<Player>();
            break;
        case CHAT:
            chat.~Chat();
            break;
        case GAMESTATE:
            gameState.~GameState();
            break;
        case OWNERSHIPS:
            ownerships.~QList<Ownership>();
            break;
        case CARD:
            card.~Card();
            break;
        case DICE:
            dice.~Dice();
            break;
        case EFFECT:
            effect.~Effect();
            break;
        }
        type = NONE;
    }
    ~Packet() {
        clear();
    }
};

inline QDataStream &operator<<(QDataStream &str, const Packet &item) {
    if (item.type == Packet::NONE)
        return str;
    int type = item.type;
    str << type;
    switch (item.type) {
    case Packet::AHOJ:
        str << item.ahoj;
        break;
    case Packet::ERROR:
        str << item.error;
        break;
    case Packet::MATCH:
        str << item.match;
        break;
    case Packet::ROSTER:
        str << item.roster;
        break;
    case Packet::JOIN:
        str << item.join;
        break;
    case Packet::CREATE:
        str << item.create;
        break;
    case Packet::ENTERED:
        str << item.entered;
        break;
    case Packet::PLAYERS:
        str << item.players;
        break;
    case Packet::CHAT:
        str << item.chat;
        break;
    case Packet::GAMESTATE:
        str << item.gameState;
        break;
    case Packet::OWNERSHIPS:
        str << item.ownerships;
        break;
    case Packet::CARD:
        str << item.card;
        break;
    case Packet::DICE:
        str << item.dice;
        break;
    case Packet::EFFECT:
        str << item.effect;
        break;
    default:
        str.setStatus(QDataStream::WriteFailed);
    }
    return str;
}
inline QDataStream &operator>>(QDataStream &str, Packet &item) {
    int type;
    str >> type;
    if (type >= Packet::_LAST_TYPE) {
        str.setStatus(QDataStream::ReadCorruptData);
        return str;
    }
    item.setType(Packet::Type(type));
    switch (type) {
    case Packet::NONE:
        break;
    case Packet::AHOJ: {
        str >> item.ahoj;
        break;
    }
    case Packet::ERROR: {
        str >> item.error;
        break;
    }
    case Packet::MATCH: {
        str >> item.match;
        break;
    }
    case Packet::ROSTER: {
        str >> item.roster;
        break;
    }
    case Packet::JOIN: {
        str >> item.join;
        break;
    }
    case Packet::CREATE: {
        str >> item.create;
        break;
    }
    case Packet::ENTERED: {
        str >> item.entered;
        break;
    }
    case Packet::PLAYERS: {
        str >> item.players;
        break;
    }
    case Packet::CHAT: {
        str >> item.chat;
        break;
    }
    case Packet::GAMESTATE: {
        str >> item.gameState;
        break;
    }
    case Packet::OWNERSHIPS: {
        str >> item.ownerships;
        break;
    }
    case Packet::CARD: {
        str >> item.card;
        break;
    }
    case Packet::DICE: {
        str >> item.dice;
        break;
    }
    case Packet::EFFECT: {
        str >> item.effect;
        break;
    }
    default: {
        str.setStatus(QDataStream::ReadCorruptData);
        break;
    }
    }
    return str;
}

#endif // PROTOCOL_H
