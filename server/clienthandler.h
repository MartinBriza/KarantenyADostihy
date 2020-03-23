#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include "protocol.h"

#include <QTcpSocket>
#include <QList>

class Game;

class ClientHandler : public QObject {
    Q_OBJECT
public:
    enum State {
        PRE_AHOJ,
        AHOJ_SENT,
        AHOJ_RECEIVED,
        ROSTER_SENT,
    };
    ClientHandler(QObject *parent = nullptr, QTcpSocket *socket = nullptr);
    virtual ~ClientHandler();
private slots:
    void onSocketStateChanged();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError err);
private:
    void sendMessage(Game *game, Chat message);
    Game *clientGame();
    void joinGame(int id, const QString &password);
    void leaveGame(bool notify = true);
    void handleEmptyGame(Game *game);
    void updateOwnerships(Game *game);
    void updateOpponents(Game *game);

    inline static int lastPlayerID { 1 };

    QTcpSocket *m_socket;
    QDataStream m_dataStream;
    State m_state { PRE_AHOJ };
    QString m_clientName {};
    bool m_clientReady { false };
    int m_clientId { lastPlayerID++ };
    int m_money { 30000 };
    QColor m_clientColor { Qt::red };
    int m_position { 0 };
};

#endif // CLIENTHANDLER_H
