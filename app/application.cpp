#include "application.h"
#include "application_p.h"
#include <keychain.h>
#include <QJsonDocument>
#include <QMessageBox>
#include "notification.h"

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
    this->plurk = new Plurq::Plurk(APP_KEY, APP_SECRET);
    connect(plurk, &QAbstractOAuth::granted, this, &Application::authorized);

    // Create a global menu bar on mac
    this->menuBar = new QMenuBar();

    // Create the tray icon
    this->trayIcon = new TrayIcon();
    connect(trayIcon, &TrayIcon::quitRequested, this, &Application::quit);
    trayIcon->show();

    // Create comet
    this->comet = new Comet(plurk, this);
    comet->setCache(&cache);
    connect(this, &Application::authorized, comet, &Comet::updateProfile);
    connect(this, &Application::authorized, comet, &Comet::start);
    connect(comet, &Comet::profileUpdated, trayIcon, &TrayIcon::updateProfile);
    connect(comet, &Comet::newPlurk, [=](int postId) {
        // Get item from cache
        Plurq::Post post = cache.post(postId);
        Plurq::Profile owner = cache.user(post.ownerId());

        // Skip own posts
        if (post.ownerId() == cache.current().id())
            return;

        Notification::display(
                    tr("Plurk from your timeline"),
                    tr("%1 %2").arg(owner.displayName()).arg(post.translatedQualifier()),
                    post.rawContent());
    });
    connect(comet, &Comet::newResponse, [=](int postId, int responseId) {
        // Get item from cache
        Plurq::Post post = cache.post(postId);
        Plurq::Post response = cache.response(responseId);
        Plurq::Profile owner = cache.user(post.ownerId());
        Plurq::Profile responder = cache.user(response.intValue(QLatin1String("user_id")));

         // Skip anything that isn't noteworthy
        if (!(post.responded() || post.mentioned() || post.ownerId() == cache.current().id()))
            return;

        Notification::display(
                    tr("Response on %1's plurk").arg(owner.displayName()),
                    tr("%1 %2").arg(responder.displayName()).arg(response.qualifier()), // TODO: Translate
                    response.rawContent());
    });

    // Authorize application
    loadCredentials();
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
