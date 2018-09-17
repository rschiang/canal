#include "notification.h"

Notification::Notification(QObject *parent)
    : QObject(parent) {}

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
                tr("%1 %2").arg(owner.displayName()).arg(post.translatedQualifier()),
                post.rawContent());
        break;
    }
    case NewResponse: {
        Plurq::Post &post = cache->post(postId);
        Plurq::Profile &owner = cache->user(userId);
        Plurq::Post &response = cache->response(responseId);
        Plurq::Profile &responder = cache->user(response.userId());

        display(tr("Response on %1 %2 %3")
                .arg(owner.displayName()).arg(post.translatedQualifier()).arg(post.rawContent()),
                tr("%1 %2")
                .arg(responder.displayName()).arg(response.qualifier()), // TODO: Translate
                response.rawContent());
        break;
    }
    default:
        qDebug() << "Notification: unknown value" << type;
        return;
    }
}
