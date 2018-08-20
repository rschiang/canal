#include "comet.h"

Comet::Comet(Plurq::Plurk *plurk, QObject *parent) : QObject(parent)
{
    this->plurk = plurk;

    this->timer = new QTimer(this);
    timer->setInterval(60 * 1000);
    timer->setSingleShot(false);
    timer->setTimerType(Qt::VeryCoarseTimer);
    connect(timer, &QTimer::timeout, this, &Comet::timeout);

    this->manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::networkAccessibleChanged, this, &Comet::networkStateChanged);

    // TODO: Listen to OS power events
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
            this->url = entity[QLatin1String("comet_server")].toString();
        } else {
            qDebug() << "Unable to fetch comet channel URL";
            this->name = QString::null;
        }
    });
}

void Comet::send()
{
    if (name.isEmpty() || (manager->networkAccessible() == QNetworkAccessManager::NotAccessible))
        return; // Do nothing if channel URL or network isn't ready

    // Build request
    QUrl url(QStringLiteral("%1&offset=%2").arg(this->url, this->offset));
    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::BackgroundRequestAttribute, true);
    request.setPriority(QNetworkRequest::LowPriority);

    // Send the request
    QNetworkReply *reply = manager->get(request);
    connect(reply, &QNetworkReply::finished, [=]() {
        // Read comet response
        Plurq::Entity entity(reply);
        if (entity.valid()) {
            int offset = entity[QLatin1String("new_offset")].toInt(BAD_OFFSET);
            if (offset == BAD_OFFSET) {
                this->offset = EMPTY_OFFSET;
                // TODO: Resync data
            } else {
                this->offset = offset;
                auto data = entity[QLatin1String("data")].toArray();
                for (auto i : data) {
                    QJsonObject item = i.toObject();
                    QString type = item[QLatin1String("type")].toString();
                    if (type == QLatin1String("new_plurk"))
                        this->newPlurk(); // TODO: Emit New Plurk
                    else if (type == QLatin1String("new_response"))
                        this->newResponse(); // TODO: Emit New Response
                }
            }
        }

        // Clear out the active reply
        reply->deleteLater();
        this->activeReply = nullptr;
    });
    this->activeReply = reply;
}

void Comet::abort()
{
    // Abort and release previous reply
    if (activeReply != nullptr) {
        activeReply->abort();
        activeReply->deleteLater();
        this->activeReply = nullptr;
    }
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

void Comet::powerStateChanged(bool suspend)
{
    // Do not attempt to reconnect as power state might change
    if (suspend) abort();
}
