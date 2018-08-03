#include "application.h"
#include "application_p.h"
#include <keychain.h>
#include <QJsonDocument>
#include <QMessageBox>

#define CANAL_KEYSTORE_ID "tw.poren.canal"
#define CANAL_KEYSTORE_TOKEN "token"

Application::Application(int &argc, char** argv)
    : QApplication(argc, argv)
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    initializeComponents();
}

void Application::initializeComponents()
{
    // Initialize Plurk API client
    this->plurk = new Plurk(APP_KEY, APP_SECRET);
    connect(plurk, &QAbstractOAuth::granted, this, &Application::authorized);

    // Read token from keychain
    auto readJob = new QKeychain::ReadPasswordJob(CANAL_KEYSTORE_ID);
    readJob->setInsecureFallback(false);
    readJob->setKey(CANAL_KEYSTORE_TOKEN);
    connect(readJob, &QKeychain::Job::finished, [&](QKeychain::Job *job) {
        auto error = job->error();
        if (!error) {
            QString data = static_cast<QKeychain::ReadPasswordJob *>(job)->textData();
            plurk->restoreTokenCredentials(data.section('&', 0, 0), data.section('&', 1));
            this->authorized();
        } else {
            if (error == QKeychain::Error::EntryNotFound) {
                qDebug() << "No token saved in keychain. Start authorization.";
            } else {
                qDebug() << "Error reading keychain" << job->errorString();
                QMessageBox::critical(nullptr, tr("Canal"),
                                      tr("Unable to read credentials from keychain: %1.").arg(job->errorString()),
                                      QMessageBox::Ok);
            }
            this->authorize();
        }
    });
    readJob->start();

    // Create a global menu bar on mac
    this->menuBar = new QMenuBar();

    // Create context menu
    this->contextMenu = new QMenu();

    QAction *accountItem = contextMenu->addAction(tr("Not logged in"));
    accountItem->setDisabled(true);
    connect(this, &Application::profileUpdated, [=](QString &name) {
        accountItem->setText(name);
    });

    contextMenu->addAction(tr("Plurk"));
    contextMenu->addAction(tr("Timeline"));
    contextMenu->addSeparator();

    QAction *notificationItem = contextMenu->addAction(tr("Notification"));
    notificationItem->setCheckable(true);
    notificationItem->setChecked(true);

    contextMenu->addAction(tr("Settings"));
    contextMenu->addSeparator();
    contextMenu->addAction(tr("Quit"), [&] { this->quit(); });

    // Create the tray icon
    this->trayIcon = new QSystemTrayIcon();
    QIcon icon(":/res/mac/tray_icon.png");
    icon.setIsMask(true);
    trayIcon->setIcon(icon);
    trayIcon->setToolTip(tr("Canal"));
    trayIcon->setContextMenu(contextMenu);
    connect(trayIcon, &QSystemTrayIcon::activated, this, &Application::trayIconActivated);
    trayIcon->show();
}

void Application::authorize()
{
    // Do bootstrapping and authorization stuff
#ifndef RELEASE
    plurk->restoreTokenCredentials(TEST_TOKEN, TEST_SECRET);
    this->authorized();
#else
    this->plurk->grant();
#endif
}

void Application::authorized()
{
    // Do stuff related to Plurk
    QNetworkReply *reply = plurk->get(Plurk::apiUrl("Users/me"));
    connect(reply, &QNetworkReply::finished, [=]() {
        QJsonParseError parseError;
        const auto data = reply->readAll();
        const auto doc = QJsonDocument::fromJson(data, &parseError);
        if (parseError.error) {
            qDebug() << "JSON error" << parseError.errorString();
        } else {
            const auto obj = doc.object();
            QString name = obj.value("display_name").toString();
            this->profileUpdated(name);
        }
    });
}

void Application::trayIconActivated(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::DoubleClick)
        this->quit();
}
