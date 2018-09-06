#include "comet.h"

#define JS_CALLBACK_START "CometChannel.scriptCallback("
#define JS_CALLBACK_END ");"

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
    qDebug() << "Starting comet";
    if (name.isEmpty())
        initiate();
    timer->start();
}

void Comet::stop()
{
    qDebug() << "Stopping comet";
    timer->stop();
    abort();
}

void Comet::updateAlerts()
{
    // TODO: Fetch alerts
}

void Comet::initiate()
{
    QNetworkReply *reply = plurk->get("Realtime/getUserChannel");
    connect(reply, &QNetworkReply::finished, [=]() {
        Plurq::Entity entity(reply);
        if (entity.valid()) {
            this->name = entity.stringValue(QLatin1String("channel_name"));
            this->url = entity.stringValue(QLatin1String("comet_server"));
            qDebug() << "Comet channel" << this->name;
        } else {
            qDebug() << "Unable to fetch comet channel URL";
            this->name = QString::null;
        }
        reply->deleteLater();
    });
}

void Comet::send()
{
    if (name.isEmpty() || (manager->networkAccessible() == QNetworkAccessManager::NotAccessible))
        return; // Do nothing if channel URL or network isn't ready

    // Build request
    QUrl url(QStringLiteral("%1&offset=%2&js_callback=").arg(this->url, this->offset));
    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::BackgroundRequestAttribute, true);
    request.setPriority(QNetworkRequest::LowPriority);

    // Send the request
    QNetworkReply *reply = manager->get(request);
    connect(reply, &QNetworkReply::finished, [=]() {
        // Read comet response manually
        QByteArray json = reply->readAll();
        if (json.startsWith(JS_CALLBACK_START) && json.endsWith(JS_CALLBACK_END)) {
            // Chop off the JSONP wrapper as Qt cannot process it
            // -1 to exclude the string terminator (\0)
            json.remove(0, sizeof(JS_CALLBACK_START) - 1)
                .chop(sizeof(JS_CALLBACK_END) - 1);
        }

        Plurq::Entity entity(json);
        if (entity.valid()) {
            int offset = entity[QLatin1String("new_offset")].toInt(BAD_OFFSET);
            qDebug() << "New offset:" << offset;

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
                    else if (type == QLatin1String("update_notification"))
                        this->updateAlerts(); // TODO: Emit Notification
                    qDebug() << "Type:" << type;
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
