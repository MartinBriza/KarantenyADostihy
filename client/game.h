#ifndef GAME_H
#define GAME_H

#include "protocol.h"

#include "datamodel.h"

#include <QObject>
#include <QtQml>
#include <QColor>

#include <QTcpSocket>

class Client : public QObject {
    Q_OBJECT
    Q_PROPERTY(UI::Roster* roster READ roster CONSTANT)
    Q_PROPERTY(QString name READ nameGet WRITE nameSet NOTIFY nameChanged)
    Q_PROPERTY(State state READ stateGet NOTIFY stateChanged)
    Q_PROPERTY(UI::Lobby *lobby READ lobbyGet NOTIFY lobbyChanged)
    Q_PROPERTY(QQmlListProperty<UI::Chat> chat READ chatGet NOTIFY chatChanged)
    Q_PROPERTY(QQmlListProperty<UI::Opponent> opponents READ opponentsGet NOTIFY opponentsChanged)
    Q_PROPERTY(int thisPlayerId READ thisPlayerId NOTIFY thisPlayerIdChanged)
    Q_PROPERTY(UI::Board *board READ boardGet CONSTANT)
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
        , m_roster(new UI::Roster(this))
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

    UI::Board *boardGet() {
        return m_board;
    }

    UI::Roster *roster() {
        return m_roster;
    }
    UI::Lobby *lobbyGet() {
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

    QQmlListProperty<UI::Chat> chatGet() {
        return QQmlListProperty<UI::Chat>(this, m_chat);
    }
    QQmlListProperty<UI::Opponent> opponentsGet() {
        return QQmlListProperty<UI::Opponent>(this, m_opponents);
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
                    m_lobby = new UI::Lobby(this, p.entered.id, p.entered.name);
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
                QSet<int> oldIDs;
                QSet<int> newIDs;
                for (auto i : m_opponents)
                    oldIDs.insert(i->id);
                for (auto i : p.opponents)
                    newIDs.insert(i.id);
                if (newIDs != oldIDs) {
                    m_opponents.clear();
                    for (auto &i : p.opponents) {
                        m_opponents.append(new UI::Opponent(this, i));
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
                m_chat.append(new UI::Chat(this, p.chat));
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
                emit displayCard(p.card.header, p.card.name);
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
    void displayCard(const QString &header, const QString &message);

private:
    QTcpSocket *m_socket { nullptr };
    QDataStream m_dataStream;
    UI::Roster *m_roster { nullptr };
    QString m_name { randomName() };
    UI::Lobby *m_lobby { nullptr };
    QList<UI::Chat*> m_chat {};
    QList<UI::Opponent*> m_opponents {};
    QTimer m_refreshTimer;
    int m_thisPlayerId { -1 };
    UI::Board *m_board { new UI::Board(this) };
};

#endif // GAME_H
