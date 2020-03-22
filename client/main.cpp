#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "game.h"

int main(int argc, char *argv[]) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    qsrand(time(nullptr));

    qRegisterMetaType<UIField::Type>();

    qmlRegisterUncreatableType<Game>("monopoly", 1, 0, "GameData", "Created by core");
    qmlRegisterUncreatableType<Board>("monopoly", 1, 0, "BoardData", "Created by core");
    qmlRegisterUncreatableType<Player>("monopoly", 1, 0, "PlayerData", "Created by core");
    qmlRegisterUncreatableType<UIField>("monopoly", 1, 0, "FieldData", "Created by core");
    qmlRegisterUncreatableType<Client>("monopoly", 1, 0, "ClientData", "Created by core");
    qmlRegisterUncreatableType<UIRoster>("monopoly", 1, 0, "RosterData", "Created by core");
    qmlRegisterUncreatableType<UIMatch>("monopoly", 1, 0, "MatchData", "Created by core");
    qmlRegisterUncreatableType<UILobby>("monopoly", 1, 0, "LobbyData", "Created by core");
    qmlRegisterUncreatableType<UIChat>("monopoly", 1, 0, "ChatData", "Created by core");
    qmlRegisterUncreatableType<UIOpponent>("monopoly", 1, 0, "OpponentData", "Created by core");

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("client", new Client());
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}

