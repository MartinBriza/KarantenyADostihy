#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "game.h"

int main(int argc, char *argv[]) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    qsrand(time(nullptr));

    qRegisterMetaType<UI::Field::Type>();

    qmlRegisterUncreatableType<Client>("monopoly", 1, 0, "ClientData", "Created by core");

    qmlRegisterUncreatableType<UI::Board>("monopoly", 1, 0, "BoardData", "Created by core");
    qmlRegisterUncreatableType<UI::Effect>("monopoly", 1, 0, "EffectData", "Created by core");
    qmlRegisterUncreatableType<UI::Field>("monopoly", 1, 0, "FieldData", "Created by core");
    qmlRegisterUncreatableType<UI::Roster>("monopoly", 1, 0, "RosterData", "Created by core");
    qmlRegisterUncreatableType<UI::Match>("monopoly", 1, 0, "MatchData", "Created by core");
    qmlRegisterUncreatableType<UI::Lobby>("monopoly", 1, 0, "LobbyData", "Created by core");
    qmlRegisterUncreatableType<UI::Chat>("monopoly", 1, 0, "ChatData", "Created by core");
    qmlRegisterUncreatableType<UI::Opponent>("monopoly", 1, 0, "OpponentData", "Created by core");
    qmlRegisterUncreatableType<UI::Ownership>("monopoly", 1, 0, "OwnershipData", "Created by core");

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

