#ifndef GAME_H
#define GAME_H

#include "protocol.h"

#include "datamodel.h"

#include <QObject>
#include <QtQml>
#include <QColor>

#include <QtWebSockets/QWebSocket>

#define DEFAULT_SERVER "46.36.35.81"
#define DEFAULT_PORT 16543

class Client : public QObject {
    Q_OBJECT
    Q_PROPERTY(UI::Roster* roster READ roster CONSTANT)
    Q_PROPERTY(QString name READ nameGet WRITE nameSet NOTIFY nameChanged)
    Q_PROPERTY(State state READ stateGet NOTIFY stateChanged)
    Q_PROPERTY(UI::Lobby *lobby READ lobbyGet NOTIFY lobbyChanged)
    Q_PROPERTY(QQmlListProperty<UI::Chat> chat READ chatGet NOTIFY chatChanged)
    Q_PROPERTY(QQmlListProperty<UI::Player> players READ playersGet NOTIFY playersChanged)
    Q_PROPERTY(int thisPlayerId READ thisPlayerId NOTIFY thisPlayerIdChanged)
    Q_PROPERTY(UI::Board *board READ boardGet CONSTANT)
    Q_PROPERTY(bool strictGame READ strictGameGet CONSTANT)

    Q_PROPERTY(QString server READ serverGet WRITE serverSet NOTIFY serverChanged)
    Q_PROPERTY(int port READ portGet WRITE portSet NOTIFY portChanged)

    Q_PROPERTY(QString status READ statusGet NOTIFY statusChanged)
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
        , m_socket(new QWebSocket("http://ma.rtinbriza.cz/karanteny", QWebSocketProtocol::Version13, this))
        , m_roster(new UI::Roster(this))
        , m_refreshTimer()
    {
        m_refreshTimer.setInterval(5000);
        m_refreshTimer.setSingleShot(false);

        connect(&m_refreshTimer, &QTimer::timeout, this, &Client::refreshRoster);
        connect(m_socket, &QWebSocket::binaryMessageReceived, this, &Client::onReadyRead);
        connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this, &Client::onError);
        connect(m_socket, &QWebSocket::stateChanged, this, &Client::onSocketStateChanged);
        QTimer::singleShot(0, [this]() {
            m_socket->open(QUrl(QString("ws://%1:%2").arg(settings.value("server", DEFAULT_SERVER).toString()).arg(settings.value("port", DEFAULT_PORT).toInt())));
            m_refreshTimer.start();
        });
    }

    QString serverGet() {
        return settings.value("server", DEFAULT_SERVER).toString();
    }
    void serverSet(const QString &val) {
        if (settings.value("server", DEFAULT_SERVER).toString() != val) {
            settings.setValue("server", val);
            emit serverChanged();
            m_socket->close();
            m_socket->open(QUrl(QString("ws://%1:%2").arg(settings.value("server", DEFAULT_SERVER).toString()).arg(settings.value("port", DEFAULT_PORT).toInt())));
        }
    }
    int portGet() {
        return settings.value("port", DEFAULT_PORT).toInt();
    }
    void portSet(int val) {
        if (settings.value("port", DEFAULT_PORT).toInt() != val) {
            settings.setValue("port", val);
            emit portChanged();
            m_socket->close();
            m_socket->open(QUrl(QString("ws://%1:%2").arg(settings.value("server", DEFAULT_SERVER).toString()).arg(settings.value("port", DEFAULT_PORT).toInt())));
        }
    }

    QString statusGet() const {
        return m_status;
    }
    void statusSet(const QString &val) {
        if (m_status != val) {
            m_status = val;
            emit statusChanged();
        }
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
        return settings.value("name", randomName()).toString();
    }
    void nameSet(const QString &name) {
        if (settings.value("name", randomName()).toString() != name) {
            return settings.setValue("name", name);
            emit nameChanged();
            sendPacket(Packet(Ahoj{name}));
        }
    }
    bool strictGameGet() const {
        return true;
    }

    int thisPlayerId() const {
        return m_thisPlayerId;
    }

    QQmlListProperty<UI::Chat> chatGet() {
        return QQmlListProperty<UI::Chat>(this, m_chat);
    }
    QQmlListProperty<UI::Player> playersGet() {
        return QQmlListProperty<UI::Player>(this, m_players);
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
    void dice();
    void diceMove();
    void endTurn();

    void reset();

private slots:
    void sendPacket(const Packet &p) {
        QByteArray frame;
        QDataStream stream(&frame, QIODevice::WriteOnly);
        stream << p;
        m_socket->sendBinaryMessage(frame);
    }
    void onSocketStateChanged() {
        if (m_socket->error() != QAbstractSocket::UnknownSocketError) {
            statusSet("Error: " + m_socket->errorString());
            reset();
            emit serverError("Buď ti vypadl net, ale (spíš) spadl server, protože jsem to napsal v C++ a ne v Rustu");
        }
        else {
            switch (m_socket->state()) {
            case QAbstractSocket::UnconnectedState: statusSet("Not connected"); break;
            case QAbstractSocket::HostLookupState: statusSet("Host lookup"); break;
            case QAbstractSocket::ConnectingState: statusSet("Connecting"); break;
            case QAbstractSocket::ConnectedState: statusSet("Connected"); break;
            case QAbstractSocket::ClosingState: statusSet("Closing"); break;
            }
        }

        if (m_socket->state() == QAbstractSocket::ConnectedState) {
            sendPacket(Packet(Ahoj{nameGet()}));
        }
    }
    void onReadyRead(const QByteArray &message) {
        QDataStream stream(message);
        Packet p;
        stream >> p;
        switch (p.type) {
        case Packet::AHOJ:
            sendPacket(Packet(Ahoj{nameGet()}));
            break;
        case Packet::ERROR:
            qCritical() << "SERVER ERROR: " << p.error;
            emit serverError(p.error);
            break;
        case Packet::ROSTER:
            if (stream.status() == QDataStream::Ok)
                m_roster->regenerate(p.roster);
            break;
        case Packet::ENTERED: {
            if (p.entered.id >= 0) {
                m_lobby = new UI::Lobby(this, p.entered.id, p.entered.name);
                emit lobbyChanged();
                m_state = LOBBY;
                emit stateChanged();
                m_chat.clear();
                emit chatChanged();
            }
            else {
                m_lobby->deleteLater();
                m_lobby = nullptr;
                emit lobbyChanged();
                m_state = ROSTER;
                emit stateChanged();
                m_chat.clear();
                emit chatChanged();
            }
            break;
        }
        case Packet::PLAYERS: {
            QSet<int> oldIDs;
            QSet<int> newIDs;
            for (auto i : m_players)
                oldIDs.insert(i->id);
            for (auto i : p.players)
                newIDs.insert(i.id);
            if (newIDs != oldIDs) {
                m_players.clear();
                for (auto &i : p.players) {
                    m_players.append(new UI::Player(this, i));
                    if (i.you && m_thisPlayerId != i.id) {
                        m_thisPlayerId = i.id;
                        emit thisPlayerIdChanged();
                    }
                }
                emit playersChanged();
            }
            else {
                for (auto old : m_players) {
                    for (auto refresh : p.players) {
                        if (old->id == refresh.id) {
                            old->update(refresh);
                        }
                    }
                }
            }
            break;
        }
        case Packet::CHAT:
            m_chat.prepend(new UI::Chat(this, p.chat));
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
                        for (auto player : m_players) {
                            if (ownership.player == player->id) {
                                field->ownerSet(player);
                            }
                        }
                    }
                }
            }
            for (auto player : m_players) {
                player->updateOwnerships(p.ownerships);
            }
            break;
        }
        case Packet::CARD: {
            emit displayCard(p.card.header, p.card.name);
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
    void playersChanged();
    void thisPlayerIdChanged();
    void serverChanged();
    void portChanged();
    void statusChanged();
    void serverError(const QString &message);
    void displayCard(const QString &header, const QString &message);

private:
    QSettings settings;
    QWebSocket *m_socket { nullptr };
    UI::Roster *m_roster { nullptr };
    UI::Lobby *m_lobby { nullptr };
    QList<UI::Chat*> m_chat {};
    QList<UI::Player*> m_players {};
    QTimer m_refreshTimer;
    int m_thisPlayerId { -1 };
    UI::Board *m_board { new UI::Board(this) };
    QString m_status { "Init" };
};

#endif // GAME_H
