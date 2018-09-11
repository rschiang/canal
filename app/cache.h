#ifndef CACHE_H
#define CACHE_H

#include <QObject>
#include <QHash>
#include <Plurk>

#define CACHE_INVALID_USER -1

class Cache : public QObject
{
    Q_OBJECT
public:
    explicit Cache(QObject *parent = nullptr);

    Plurq::Profile& user(int userId);
    Plurq::Post& post(int postId);
    Plurq::Post& response(int responseId);
    Plurq::Profile& current();

signals:
    // TODO: Are we implementing this?
    // userChanged
    // postChanged

public slots:
    int setUser(Plurq::Profile profile);
    int setPost(Plurq::Post post);
    int setResponse(Plurq::Post response);
    void setCurrentUserId(int userId);

protected:
    QHash<int, Plurq::Profile> users;
    QHash<int, Plurq::Post> posts;
    QHash<int, Plurq::Post> responses;
    int currentUserId = CACHE_INVALID_USER;
};

#endif // CACHE_H
