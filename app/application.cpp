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
    connect(this, &Application::authorized, this, &Application::updateProfile);
    initializeComponents();
}

void Application::initializeComponents()
{
    // Initialize Plurk API client
    this->plurk = new Plurq::Plurk(APP_KEY, APP_SECRET);
    connect(plurk, &QAbstractOAuth::granted, this, &Application::authorized);
    loadCredentials();

    // Create a global menu bar on mac
    this->menuBar = new QMenuBar();

    // Create the tray icon
    this->trayIcon = new TrayIcon();
    connect(trayIcon, &TrayIcon::quitRequested, this, &Application::quit);
    connect(this, &Application::profileUpdated, trayIcon, &TrayIcon::updateProfile);
    trayIcon->show();

    // Create comet
    this->comet = new Comet(plurk, this);
    comet->setCache(&cache);
    connect(this, &Application::authorized, comet, &Comet::start);
    connect(comet, &Comet::newPlurk, [=](int postId) {
        // Get item from cache
        Plurq::Post post = cache.post(postId);
        Plurq::Profile owner = cache.user(post.ownerId());
        trayIcon->showMessage(
                    tr("%1 %2").arg(owner.displayName()).arg(post.translatedQualifier()),
                    post.rawContent(),
                    QSystemTrayIcon::NoIcon, 5000);
    });
    connect(comet, &Comet::newResponse, [=](Plurq::Entity e) {
        Plurq::Post post = e.objectValue(QLatin1String("plurk"));
        if (!(post.responded() || post.mentioned() || post.ownerId() == post.intValue(QLatin1String("user_id"))))
            return; // Skip anything that isn't noteworthy
        Plurq::Post response = e.objectValue(QLatin1String("response"));
        Plurq::Profile responder = e.objectValue(QLatin1String("user"))[QString::number(response.intValue(QLatin1String("user_id")))].toObject();
        QString title = tr("%1 responded to %2").arg(responder.displayName()).arg(post.rawContent());
        trayIcon->showMessage(title, response.rawContent(), QSystemTrayIcon::NoIcon, 2500);
    });
}

Application::~Application()
{
    delete comet;
    delete trayIcon;
    delete menuBar;
    delete plurk;
}

void Application::authorize()
{
    // Do bootstrapping and authorization stuff
#ifndef RELEASE
    plurk->restoreTokenCredentials(TEST_TOKEN, TEST_SECRET);
    emit authorized();
#else
    plurk->grant();
    saveCredentials();
#endif
}

void Application::loadCredentials()
{
    // Read token from keychain
    auto readJob = new QKeychain::ReadPasswordJob(CANAL_KEYSTORE_ID);
    readJob->setInsecureFallback(false);
    readJob->setKey(CANAL_KEYSTORE_TOKEN);
    connect(readJob, &QKeychain::Job::finished, [&](QKeychain::Job *job) {
        auto error = job->error();
        if (!error) {
            QString data = static_cast<QKeychain::ReadPasswordJob *>(job)->textData();
            plurk->restoreTokenCredentials(data.section('&', 0, 0), data.section('&', 1));
            emit authorized();
        } else {
            if (error == QKeychain::Error::EntryNotFound) {
                qDebug() << "No token saved in keychain. Start authorization.";
            } else {
                qDebug() << "Error reading keychain" << job->errorString();
                QMessageBox::critical(nullptr, tr("Canal"),
                                      tr("Unable to read credentials from keychain: %1.").arg(job->errorString()),
                                      QMessageBox::Ok);
            }
            authorize();
        }
    });
    readJob->start();
}

void Application::saveCredentials()
{
    // Save the token to keychain
    auto writeJob = new QKeychain::WritePasswordJob(CANAL_KEYSTORE_ID);
    writeJob->setInsecureFallback(false);
    writeJob->setKey(CANAL_KEYSTORE_TOKEN);
    writeJob->setTextData(plurk->token() + "&" + plurk->tokenSecret());
    connect(writeJob, &QKeychain::Job::finished, [&](QKeychain::Job *job) {
        auto error = job->error();
        if (!error) {
            qDebug() << "Token saved to keychain";
        } else {
            qDebug() << "Error reading keychain" << job->errorString();
            QMessageBox::critical(nullptr, tr("Canal"),
                                  tr("Unable to save credentials to keychain: %1.").arg(job->errorString()),
                                  QMessageBox::Ok);
        }
    });
    writeJob->start();
}

void Application::updateProfile()
{
    QNetworkReply *reply = plurk->get("Profile/getOwnProfile");
    connect(reply, &QNetworkReply::finished, [=]() {
        Plurq::Entity entity(reply);
        if (entity.valid()) {
            // Read user profile
            Plurq::Profile profile = entity.objectValue(QLatin1String("user_info"));
            cache.setCurrentUserId(profile.id());
            cache.setUser(profile);
            emit profileUpdated(&profile);

            // Store acquired users and plurks in cache
            for (auto u : entity.objectValue(QLatin1String("plurk_users")))
                cache.setUser(u.toObject());

            // Store plurks too
            for (auto p : entity.objectValue(QLatin1String("plurks")))
                cache.setPost(p.toObject());
        }
    });
}
