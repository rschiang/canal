#include "application.h"
#include "application_p.h"
#include <keychain.h>

#define CANAL_KEYSTORE_ID "tw.poren.canal"
#define CANAL_KEYSTORE_TOKEN "token"
#define CANAL_KEYSTORE_SECRET "tokenSecret"

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

    // Try keystore
    auto readJob = new QKeychain::ReadPasswordJob(CANAL_KEYSTORE_ID);
    readJob->setInsecureFallback(false);
    readJob->setKey(CANAL_KEYSTORE_TOKEN);
    readJob->setAutoDelete(false);
    connect(readJob, &QKeychain::Job::finished, [&](QKeychain::Job *job) {
        qDebug() << "Read keystore completed";
        if (job->error())
            qDebug() << "Error reading keystore" << job->errorString();
        else
            qDebug() << static_cast<QKeychain::ReadPasswordJob *>(job)->textData();
    });
    readJob->start();

    // Create a global menu bar on mac
    this->menu = new QMenuBar();

    // Create the tray icon
    QIcon icon(":/res/mac/tray_icon.png");
    icon.setIsMask(true);

    this->trayIcon = new QSystemTrayIcon();
    trayIcon->setIcon(icon);
    trayIcon->setToolTip(tr("Canal"));
    trayIcon->show();
    connect(trayIcon, &QSystemTrayIcon::activated, [&](QSystemTrayIcon::ActivationReason reason) {
        qDebug() << reason;
        if (reason == QSystemTrayIcon::DoubleClick)
            this->quit();
    });
}
