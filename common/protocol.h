#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QDebug>
#include <QDataStream>
#include <QColor>

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
    int id { -1 };
    QString password { };
    QString name { };
    QString owner { };
    int players { 0 };
    int maximumPlayers { 8 };
};
inline QDataStream &operator<<(QDataStream &str, const Match &item) {
    str << item.id << (item.password.isEmpty() ? QString() : QString("***")) << item.name << item.owner << item.players << item.maximumPlayers;
    return str;
}
inline QDataStream &operator>>(QDataStream &str, Match &item) {
    str >> item.id >> item.password >> item.name >> item.owner >> item.players >> item.maximumPlayers;
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

struct Opponent {
    int id;
    QString name;
    QColor color;
    int money;
    int position;
    bool leader;
    bool ready;
    bool you;
};
inline QDataStream &operator<<(QDataStream &str, const Opponent &item) {
    str << item.id << item.name << item.color << item.money << item.leader << item.ready << item.you << item.position;
    return str;
}
inline QDataStream &operator>>(QDataStream &str, Opponent &item) {
    str >> item.id >> item.name >> item.color >> item.money >> item.leader >> item.ready >> item.you >> item.position;
    return str;
}

struct Chat {
    QString time;
    QString from;
    QString message;
    int urgency;
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
    QString name;
    bool operator<(const Card &r) const {
        return name < r.name;
    }
};
inline QDataStream &operator<<(QDataStream &str, const Card &item) {
    str << item.name;
    return str;
}
inline QDataStream &operator>>(QDataStream &str, Card &item) {
    str >> item.name;
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
        OPPONENTS,
        CHAT,
        GAMESTATE,
    } type { NONE };
    union {
        QString error;
        Ahoj ahoj;
        Match match;
        Roster roster;
        Join join;
        Create create;
        Entered entered;
        QList<Opponent> opponents;
        Chat chat;
        GameState gameState;
    };
    Packet() {}
    Packet(Type type, const QString &text) : type(type) {
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
    Packet(const QList<Opponent> &d) : type(OPPONENTS), opponents(d) { }
    Packet(const GameState &d) : type(GAMESTATE), gameState(d) { }
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
        case OPPONENTS:
            new (&opponents) QList<Opponent>();
            break;
        case CHAT:
            new (&chat) Chat();
            break;
        case GAMESTATE:
            new (&gameState) GameState();
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
        case OPPONENTS:
            opponents.~QList<Opponent>();
            break;
        case CHAT:
            chat.~Chat();
            break;
        case GAMESTATE:
            gameState.~GameState();
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
    case Packet::OPPONENTS:
        str << item.opponents;
        break;
    case Packet::CHAT:
        str << item.chat;
        break;
    case Packet::GAMESTATE:
        str << item.gameState;
        break;
    default:
        str.setStatus(QDataStream::WriteFailed);
    }
    return str;
}
inline QDataStream &operator>>(QDataStream &str, Packet &item) {
    int type;
    str >> type;
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
    case Packet::OPPONENTS: {
        str >> item.opponents;
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
    default: {
        str.setStatus(QDataStream::ReadCorruptData);
        break;
    }
    }
    return str;
}

#endif // PROTOCOL_H
