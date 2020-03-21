#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QDataStream>

struct Ahoj {
    QString ahoj { "ahoj" };
};

struct Match {
    int id;
    QString name;
    QString owner;
};

struct Roster {
    QList<Match> matches;
};

struct Packet {
    enum Type {
        NONE,
        ERROR,
        AHOJ,
        MATCH,
        ROSTER
    } type { NONE };
    union {
        QString error;
        Ahoj ahoj;
        Match match;
        Roster roster;
    };
    Packet() {}
    Packet(Packet &&o) {
        *this = std::move(o);
    }
    Packet &operator=(Packet &&o) {
        clear();
        type = o.type;
        o.type = NONE;
        switch (type) {
        case NONE:
            break;
        case ERROR:
            error = std::move(o.error);
            break;
        case AHOJ:
            ahoj = std::move(o.ahoj);
            break;
        case MATCH:
            match = std::move(o.match);
            break;
        case ROSTER:
            roster = std::move(o.roster);
            break;
        }
        return *this;
    }
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
        }
        type = NONE;
    }
    ~Packet() {
        clear();
    }
};

inline QDataStream &operator<<(QDataStream &str, const Ahoj &item) {
    str << item.ahoj;
    return str;
}
inline QDataStream &operator>>(QDataStream &str, Ahoj &item) {
    str >> item.ahoj;
    return str;
}

inline QDataStream &operator<<(QDataStream &str, const Match &item) {
    str << item.id << item.name << item.owner;
    return str;
}
inline QDataStream &operator>>(QDataStream &str, Match &item) {
    str >> item.id >> item.name >> item.owner;
    return str;
}

inline QDataStream &operator<<(QDataStream &str, const Roster &item) {
    str << item.matches;
    return str;
}
inline QDataStream &operator>>(QDataStream &str, Roster &item) {
    str >> item.matches;
    return str;
}

inline QDataStream &operator<<(QDataStream &str, const Packet &item) {
    if (item.type == Packet::NONE)
        return str;
    str << (int&) item.type;
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
    default:
        str.setStatus(QDataStream::WriteFailed);
    }
    return str;
}
inline QDataStream &operator>>(QDataStream &str, Packet &item) {
    int type;
    str >> type;
    switch (type) {
    case Packet::NONE:
        break;
    case Packet::AHOJ: {
        Ahoj d;
        str >> d;
        item = Packet(d);
        break;
    }
    case Packet::ERROR: {
        QString d;
        str >> d;
        item = Packet(Packet::ERROR, d);
        break;
    }
    case Packet::MATCH: {
        Match d;
        str >> d;
        item = Packet(d);
        break;
    }
    case Packet::ROSTER: {
        Roster d;
        str >> d;
        item = Packet(d);
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
