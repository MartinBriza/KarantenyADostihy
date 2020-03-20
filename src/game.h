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
    Field(QObject *parent = nullptr, const QString &name = {}, int price = 0, QList<Effect*> effect = {}, QColor color = Qt::transparent, int upgrade = -1);

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

#endif // GAME_H
