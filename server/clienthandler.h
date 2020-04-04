#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include "protocol.h"

#include <QtWebSockets/QWebSocket>
#include <QList>

struct Game;
struct PlayerData;

class ClientHandler : public QObject {
    Q_OBJECT
public:
    friend struct PlayerData; //I'm lazy

    ClientHandler(QObject *parent = nullptr, QWebSocket *socket = nullptr);
    virtual ~ClientHandler();

    void setPlayer(PlayerData *player);

private slots:
    void onSocketStateChanged();
    void onReadyRead(const QByteArray &message);
    void onSocketError(QAbstractSocket::SocketError err);
private:
    // packet handlers
    void onError(const QString &err);
    void onAhoj(const Ahoj &ahoj);
    void onMatch(const Match &match);
    void onRoster(const Roster &roster);
    void onJoin(const Join &join);
    void onCreate(const Create &create);
    void onEntered(const Entered &entered);
    void onPlayers(const QList<Player> &players);
    void onChat(const Chat &chat);
    void onGameState(const GameState &gameState);
    void onOwnerships(const QList<Ownership> &ownerships);
    void onCard(const Card &card);
    void onDice(const Dice &dice);

    void onBullshit();


    // generic methods
    void sendMessage(Game *game, const Chat &message);
    void sendMessage(Game *game, const QString &message);
    Game *clientGame();
    void joinGame(int id, const QString &password);
    void leaveGame(bool notify = true);
    void handleEmptyGame(Game *game);
    void updateOwnerships(Game *game);
    void updateOpponents(Game *game);
    void modifyPlayerMoney(PlayerData *player, int diff);
    void movePlayerTo(PlayerData *player, int position);
    void handleFieldEffect(PlayerData *player);

    void handleEffect(Game *game, const Effect &effect);

    void sendError(const QString &message);
    void sendPacket(const Packet &p);

    inline static uint64_t lastId { 1 };
    uint64_t id { lastId++ };

    QWebSocket *m_socket;
    PlayerData *m_player;
    QString m_name;
};

#endif // CLIENTHANDLER_H
