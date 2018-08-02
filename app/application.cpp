#include "application.h"
#include "application_p.h"

Application::Application(int &argc, char** argv)
    : QApplication(argc, argv)
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    initializeComponents();
}

void Application::initializeComponents()
{
    this->plurk = new Plurk(APP_KEY, APP_SECRET);
    connect(plurk, &QAbstractOAuth::granted, [&]() {
        qDebug() << plurk->token() << plurk->tokenSecret();
    });
    //plurk->grant();

    // Create a global menu bar on mac
    this->menu = new QMenuBar();

    // Create the tray icon
    this->trayIcon = new QSystemTrayIcon();
    trayIcon->setToolTip(tr("Canal"));
    trayIcon->show();
}
