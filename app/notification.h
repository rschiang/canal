#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QObject>
#include <QList>
#include <QString>
#include "cache.h"

class Notification : public QObject
{
    Q_OBJECT
public:
    explicit Notification(QObject *parent = nullptr);

    // Static methods
    static QString sanitize(const QString &source);
    static void display(const QString &title, const QString &subtitle, const QString &content);

    // Types and instance methods
    enum Type {
        Invalid = 0,
        NewPost,
        NewPrivatePost,
        NewResponse
    };

    class Item {
    public:
        explicit Item(Type type = Invalid, int postId = 0, int userId = 0, int responseId = 0)
            : type(type), postId(postId), userId(userId), responseId(responseId) {}

        Type type;
        int postId;
        int userId;
        int responseId;
    };

    void setCache(Cache *cache);
    void post(Type type, int postId = 0, int userId = 0, int responseId = 0);

signals:
    void queryUser(int userId);
    void queryPost(int postId);

public slots:
    void onUserChanged(int userId);
    void onPostChanged(int postId);

protected:
    void publish(Type type, int postId = 0, int userId = 0, int responseId = 0) const;

private:
    Cache* cache = nullptr;
    QList<Item> queue;
};

#endif // NOTIFICATION_H
