#include "notification.h"
#include <QRegularExpression>

Notification::Notification(QObject *parent)
    : QObject(parent), queue() {}

inline QString qualifier(Plurq::Post &post)
{
    QString qualifier = post.translatedQualifier();
    if (qualifier == QStringLiteral(":"))
        return QString::null;
    else
        return qualifier + " ";
}

QString Notification::sanitize(const QString &source)
{
    // Define regular expressions
    static QRegularExpression imgAltExpr("<img\\s+[^>]+\\s+alt=\"([^\"]+)\"\\s*>");
    static QRegularExpression imgTagExpr("<img\\s+[^>]+>");
    static QRegularExpression brTagExpr("<br\\s*[^>]*>");
    static QRegularExpression htmlTagExpr("<[^>]+>");
    static QRegularExpression whitespaceExpr("\\s+");
    static QString whitespace = QStringLiteral(" ");

    QString result(source);

    // Replace all images with alt text
    QRegularExpressionMatch match;
    while ((match = imgAltExpr.match(result)).hasMatch()) {
        result.replace(match.capturedStart(), match.capturedLength(), match.captured(1));
    }

    // Strip the remaining tags and whitespaces
    result.replace(imgTagExpr, tr("[image]"));
    result.replace(brTagExpr, whitespace);
    result.replace(htmlTagExpr, QString::null);
    result.replace(whitespaceExpr, whitespace);

    return result;
}

void Notification::setCache(Cache *cache)
{
    this->cache = cache;
}

void Notification::post(Type type, int postId, int userId, int responseId)
{
    // Make sure all data are populated
    bool cached = true;
    if (!(postId == 0 || cache->postExists(postId))) {
        cached = false;
        qDebug() << "Notification: cache missed on post" << postId;
        emit queryPost(postId);
    }

    if (!(userId == 0 || cache->userExists(userId))) {
        cached = false;
        qDebug() << "Notification: cache missed on user" << userId;
        emit queryUser(userId);
    }

    if (cached) {
        // Post the notification directly if nothing blocks
        publish(type, postId, userId, responseId);
    } else {
        // Queue the notification until data received
        queue.append(Item(type, postId, userId, responseId));
        qDebug() << "Notification: enqueued" << type << postId << userId << responseId;
    }
}

void Notification::onPostChanged(int postId)
{
    QMutableListIterator<Item> it(queue);
    while (it.hasNext()) {
        Item &i = it.next();
        // Publish the notification if cache fulfilled
        if ((i.postId == postId) && (i.userId == 0 || cache->userExists(i.userId))) {
            publish(i.type, i.postId, i.userId, i.responseId);
            it.remove();
        }
    }
}

void Notification::onUserChanged(int userId)
{
    QMutableListIterator<Item> it(queue);
    while (it.hasNext()) {
        Item &i = it.next();
        // Publish the notification if cache fulfilled
        if ((i.userId == userId) && (i.postId == 0 || cache->postExists(i.postId))) {
            publish(i.type, i.postId, i.userId, i.responseId);
            it.remove();
        }
    }
}

void Notification::publish(Type type, int postId, int userId, int responseId) const
{
    switch (type) {
    case NewPost: {
        Plurq::Post &post = cache->post(postId);
        Plurq::Profile &owner = cache->user(userId);
        display(tr("Plurk from your timeline"),
                owner.name(),
                qualifier(post) + sanitize(post.content()));
        break;
    }
    case NewResponse: {
        Plurq::Post &post = cache->post(postId);
        Plurq::Profile &owner = cache->user(userId);
        Plurq::Post &response = cache->response(responseId);
        Plurq::Profile &responder = cache->user(response.userId());

        display(tr("Response on %1 %2%3")
                .arg(owner.name())
                .arg(qualifier(post))
                .arg(sanitize(post.content())),
                responder.name(),
                qualifier(response) + sanitize(response.content()));
        break;
    }
    default:
        qDebug() << "Notification: unknown value" << type;
        return;
    }
}
