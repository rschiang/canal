#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QSysInfo>
#include <QDebug>
#include <plurk.h>
#include "main.h"

int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    Plurk plurk(APP_KEY, APP_SECRET);
    QObject::connect(&plurk, &QAbstractOAuth::granted, [&]() {
        qDebug() << plurk.token() << plurk.tokenSecret();
    });
    plurk.grant();

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
