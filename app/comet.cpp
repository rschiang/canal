#include "comet.h"

Comet::Comet(Plurq::Plurk *plurk, QObject *parent) : QObject(parent)
{
    this->plurk = plurk;
    this->timer = new QTimer(this);
    timer->setInterval(60 * 1000);
    timer->setSingleShot(false);
    timer->setTimerType(Qt::VeryCoarseTimer);

    this->manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::networkAccessibleChanged, this, &Comet::networkStateChanged);
}

void Comet::start()
{
    timer->start();
}

void Comet::stop()
{
    timer->stop();
    abort();
}

void Comet::initiate()
{
    QNetworkReply *reply = plurk->get("Realtime/getUserChannel");
    connect(reply, &QNetworkReply::finished, [=]() {
        Plurq::Entity entity(reply);
        if (entity.valid()) {
            this->name = entity[QLatin1String("channel_name")].toString();
            this->url = QUrl(entity[QLatin1String("comet_server")].toString());
        } else {
            this->name = QString::null;
        }
    });
}

void Comet::send()
{
    if (manager->networkAccessible() != QNetworkAccessManager::NotAccessible) {
        // Do some magic
    }
}

void Comet::abort()
{
    // Abort and release unreturned reply
    if (activeReply != nullptr) {
        activeReply->abort();
        activeReply->deleteLater();
        this->activeReply = nullptr;
    }
}

void Comet::finished()
{
    // Read response
    // Clear out the active reply
    activeReply->deleteLater();
    this->activeReply = nullptr;
}

void Comet::timeout()
{
    abort();
    if (timer->isActive())
        send();
}

void Comet::networkStateChanged(QNetworkAccessManager::NetworkAccessibility status)
{
    if (status == QNetworkAccessManager::NotAccessible)
        abort();
    else
        timeout();
}
