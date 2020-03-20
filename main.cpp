#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "game.h"

int main(int argc, char *argv[]) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    qmlRegisterUncreatableType<Game>("monopoly", 1, 0, "Game", "Created by core");
    qmlRegisterUncreatableType<Board>("monopoly", 1, 0, "Board", "Created by core");
    qmlRegisterUncreatableType<Player>("monopoly", 1, 0, "Player", "Created by core");
    qmlRegisterUncreatableType<Field>("monopoly", 1, 0, "Field", "Created by core");

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("game", new Game());
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}

