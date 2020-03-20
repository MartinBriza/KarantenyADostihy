#ifndef GAME_H
#define GAME_H

#include <QObject>
#include <QtQml>
#include <QColor>

class Effect;
class Card;
class Field;
class Player;
class Board;
class Game;

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
    Effect(QObject *parent = nullptr, Target target = NO_TARGET, Action action = NO_ACTION, int amount = 0, int secondaryAmount = 0)
        : QObject(parent)
    {

    }
    int amount() const {
        return m_amount;
    }
    int secondaryAmount() const {
        return m_secondaryAmount;
    }
private:
    Target m_target;
    Action m_effect;
    int m_amount;
    int m_secondaryAmount;
};

class Card : public Effect {
    Q_OBJECT
public:
    Card(QObject *parent = nullptr, const QString &text = {}, Target target = NO_TARGET, Action action = NO_ACTION, int amount = 0, int secondaryAmount = 0)
        : Effect(parent, target, action, amount, secondaryAmount)
        , m_text(text)
    {

    }
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
    Field(QObject *parent = nullptr, const QString &name = {}, int price = 0, QList<Effect*> effect = {}, QColor color = Qt::transparent, int upgrade = -1)
        : QObject(parent)
        , m_name(name)
        , m_effect(effect)
        , m_price(price)
        , m_color(color)
        , m_upgrade(upgrade)
    {
        emit feeChanged();
    }

    QString name() const {
        return m_name;
    }
    int price() const {
        return m_price;
    }
    int fee() const {
        if (m_effect.count() > 0)
            return m_effect.first()->amount();
        return 0;
    }
    QColor color() const {
        return m_color;
    }
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
    Board(QObject *parent = nullptr)
        : QObject(parent)
    {

    }

    QQmlListProperty<Field> fields() {
        return QQmlListProperty<Field>(this, m_fields);
    }

    int fieldCount() const {
        return m_fields.count();
    }

private:
    QList<Field*> m_fields {
        new Field(this, "START", 0, {
            new Effect { this, Effect::PLAYER, Effect::GAIN, 4000 }
        }),

        new Field(this, "Fantome", 1200, {
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 40 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 200 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 600 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 1800 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 3200 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 5000 },
        }, QColor("orange")),

        new Field(this, "Finance", 0, {
            new Effect { this, Effect::PLAYER, Effect::DRAW_FINANCE }
        }),

        new Field(this, "Gavora", 1200, {
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 40 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 200 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 600 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 1800 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 3200 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 5000 },
        }, QColor("orange")),

        new Field(this, "Veterinární vyšetření", 0, {
            new Effect { this, Effect::PLAYER, Effect::FEE, 500 }
        }),

        new Field(this, "1. trenér", 4000, {
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 1000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 2000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 3000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 4000 },
        }),

        new Field(this, "Lady Anne", 2000, {
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 120 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 600 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 1800 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 5400 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 8000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 11000 },
        }, QColor("brown")),

        new Field(this, "Náhoda", 0, {
            new Effect { this, Effect::PLAYER, Effect::DRAW_FINANCE }
        }),

        new Field(this, "Pasek", 2000, {
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 120 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 600 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 1200 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 5400 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 8000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 11000 },
        }, QColor("brown")),

        new Field(this, "Koran", 2400, {
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 160 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 800 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 2000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 6000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 9000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 12000 },
        }, QColor("brown")),

        new Field(this, "Distanc", 0, {
            new Effect { this, Effect::PLAYER, Effect::WAIT, -1 }
        }),

        new Field(this, "Neklan", 2800, {
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 200 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 1000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 3000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 9000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 12500 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 15000 },
        }, QColor("lightblue")),

        new Field(this, "Přeprava", 3000, {
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 80 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 200 },
        }),

        new Field(this, "Portlancl", 2800, {
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 200 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 1000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 3000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 9000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 12500 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 15000 },
        }, QColor("lightblue")),

        new Field(this, "Japan", 2800, {
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 240 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 1200 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 3600 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 10000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 14000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 18000 },
        }, QColor("lightblue")),

        new Field(this, "2. trenér", 4000, {
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 1000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 2000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 3000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 4000 },
        }),

        new Field(this, "Kostrava", 3600, {
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 280 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 1400 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 4000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 11000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 15000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 19000 },
        }, QColor("greenyellow")),

        new Field(this, "Finance", 0, {
            new Effect { this, Effect::PLAYER, Effect::DRAW_FINANCE }
        }),

        new Field(this, "Lukava", 3600, {
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 280 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 1400 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 4000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 11000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 15000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 19000 },
        }, QColor("greenyellow")),

        new Field(this, "Melák", 4000, {
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 320 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 1600 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 4400 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 12000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 16000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 20000 },
        }, QColor("greenyellow")),

        new Field(this, "Parkoviště"),

        new Field(this, "Grifel", 4400, {
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 360 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 1800 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 5000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 14000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 17000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 21000 },
        }, QColor("red")),

        new Field(this, "Náhoda", 0, {
            new Effect { this, Effect::PLAYER, Effect::DRAW_FINANCE }
        }),

        new Field(this, "Mohyla", 4400, {
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 360 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 1800 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 5000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 14000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 17000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 21000 },
        }, QColor("red")),

        new Field(this, "Metál", 4800, {
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 400 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 2000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 6000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 15000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 18000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 22000 },
        }, QColor("red")),

        new Field(this, "3. trenér", 4000, {
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 1000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 2000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 3000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 4000 },
        }),

        new Field(this, "Tara", 5200, {
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 440 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 2200 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 6600 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 16000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 19500 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 23000 },
        }, QColor("yellow")),

        new Field(this, "Furioso", 5200, {
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 440 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 2200 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 6600 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 16000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 19500 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 23000 },
        }, QColor("yellow")),

        new Field(this, "Stáje", 3000, {
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 80 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 200 },
        }),

        new Field(this, "Genius", 5600, {
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 580 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 2400 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 7200 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 17000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 20500 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 24000 },
        }, QColor("yellow")),

        new Field(this, "Podezření z dopingu"),

        new Field(this, "Shagga", 6000, {
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 500 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 2600 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 7800 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 18000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 22000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 25500 },
        }, QColor("green")),

        new Field(this, "Dahoman", 6000, {
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 500 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 2600 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 7800 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 18000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 22000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 25500 },
        }, QColor("green")),

        new Field(this, "Finance", 0, {
            new Effect { this, Effect::PLAYER, Effect::DRAW_FINANCE }
        }),

        new Field(this, "Gira", 6400, {
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 560 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 3000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 9000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 20000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 24000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 28000 },
        }, QColor("green")),

        new Field(this, "4. trenér", 4000, {
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 1000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 2000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 3000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 4000 },
        }),

        new Field(this, "Náhoda", 0, {
            new Effect { this, Effect::PLAYER, Effect::DRAW_FINANCE }
        }),

        new Field(this, "Narcius", 7000, {
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 700 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 3500 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 10000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 22000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 26000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 30000 },
        }, QColor("slateblue")),

        new Field(this, "Veterinární vyšetření", 0,  {
            new Effect { this, Effect::PLAYER, Effect::FEE, 500 }
        }),

        new Field(this, "Napoli", 8000, {
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 1000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 4000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 12000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 28000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 34000 },
            new Effect { this, Effect::PLAYER, Effect::TRANSFER, 40000 },
        }, QColor("slateblue"))
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
    Game(QObject *parent = nullptr)
        : QObject(parent)
    {
        m_players.append(new Player(this, "Martin", 30000, Qt::blue));
        m_players.append(new Player(this, "Vendula", 30000, Qt::red));
    }

    Board *board() {
        return m_board;
    }

    QQmlListProperty<Player> players() {
        return QQmlListProperty<Player>(this, m_players);
    }

signals:
    void playersChanged();

private:
    Board *m_board { new Board(this) };
    QList<Player*> m_players;

    QList<Card*> m_financeCards {
        new Card { this, "Zaplať pojistku 1000.", Effect::PLAYER, Effect::FEE, 1000 },
        new Card { this, "Pokuta za nedodržení předpisů 400.", Effect::PLAYER, Effect::FEE, 400 },
        new Card { this, "Renovuješ všechny stáje. Za každý svůj obsazený dostih zaplať 500.", Effect::PLAYER, Effect::FEE_PER_RACE, 500, 500 },
        new Card { this, "Mimořádný zisk z dostihů obdržíš 2.000.", Effect::PLAYER, Effect::GAIN, 2000 },
        new Card { this, "Jako dárek k narozeninám obdržíš od každého 200.", Effect::OTHER_PLAYERS, Effect::TRANSFER, 200 },
        new Card { this, "Mimořádná prémie 500.", Effect::PLAYER, Effect::GAIN, 500 },
        new Card { this, "Obdržíš dotaci 4.000.", Effect::PLAYER, Effect::GAIN, 4000 },
        new Card { this, "Zaplať dluh 3.000.", Effect::PLAYER, Effect::FEE, 3000 },
        new Card { this, "Za každý svůj obsazený dostih zaplať 800, za každý svůj obsazený hlavní dostih sezóny zaplať 2.300.", Effect::PLAYER, Effect::FEE_PER_RACE, 800, 2300 },
        new Card { this, "Zaplať příspěvek 2.000.", Effect::PLAYER, Effect::FEE, 2000 },
        new Card { this, "Nákup materiálu na opravu 100.", Effect::PLAYER, Effect::FEE, 100 },
        new Card { this, "Výhra v loterii 1.000.", Effect::PLAYER, Effect::GAIN, 1000 },
        new Card { this, "Obdržíš dotaci 2.000.", Effect::PLAYER, Effect::GAIN, 2000 },
        new Card { this, "Z banky obdržíš přeplatek 3.000.", Effect::PLAYER, Effect::GAIN, 3000 }
    };

    QList<Card*> m_chanceCards {
        new Card { this, "Jdi o 3 pole zpět.", Effect::PLAYER, Effect::MOVE_STEPS, -3 },
        new Card { this, "Zrušen distanc (kartu lze zachovat pro pozdější použití, nebo prodat).", Effect::PLAYER, Effect::CANCEL_SUSPENSION },
        new Card { this, "Jedeš se zúčastnit trenérského kurzu. Postoupíš na nejbližší pole Trenér. Dostaneš 4.000, pokud jedeš dopředu přes Start.", Effect::PLAYER, Effect::MOVE_TO_TRAINER },
        new Card { this, "Zdržíš se na 2 kola.", Effect::PLAYER, Effect::WAIT, 2 },
        new Card { this, "Distanc (bez 4.000).", Effect::PLAYER, Effect::MOVE_TO_SUSPENSION },
        new Card { this, "Zpět na nejbližší pole Finance.", Effect::PLAYER, Effect::MOVE_TO_FINANCE},
        new Card { this, "Zpět na poslední pole ve hře (kůň Napoli), hráč obdrží 4.000.", Effect::PLAYER, Effect::MOVE_TO_LAST_FIELD },
        new Card { this, "Zpět na pole Distanc. Obdržíš 4.000, pokud jsi cestou zpět prošel Start.", Effect::PLAYER, Effect::MOVE_TO_SUSPENSION, -1 },
        new Card { this, "Zpět na nejbližší pole Finance.", Effect::PLAYER, Effect::MOVE_TO_FINANCE, -1 },
        new Card { this, "Zpět na start (hráč obdrží 4.000).", Effect::PLAYER, Effect::MOVE_TO_FIRST_FIELD, -1 },
        new Card { this, "Zpět na start (bez 4.000).", Effect::PLAYER, Effect::MOVE_TO_FIRST_FIELD, 0 },
        new Card { this, "Zdržíš se na 2 kola.", Effect::PLAYER, Effect::WAIT, 2 },
        new Card { this, "Zdržíš se na 1 kolo.", Effect::PLAYER, Effect::WAIT, 1 },
        new Card { this, "Zpět na pole Parkoviště. Dostaneš 4.000, pokud jsi cestou zpět prošel start.", Effect::PLAYER, Effect::MOVE_TO_PARKING_LOT, -1 },
    };
};

#endif // GAME_H
