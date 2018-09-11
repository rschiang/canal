#include "cache.h"

Cache::Cache(QObject *parent) : QObject(parent)
{
    users.reserve(50);
    posts.reserve(200);
}

Plurq::Profile& Cache::user(int userId)
{
    return users[userId];
}

Plurq::Post& Cache::post(int postId)
{
    return posts[postId];
}

Plurq::Profile& Cache::current()
{
    return users[currentUserId];
}

void Cache::setUser(Plurq::Profile profile)
{
    Q_ASSERT(profile.valid());

    int userId = profile.id();
    if (users.contains(userId)) {
        // TODO: Update necessary fields only
    } else {
        users[userId] = profile;
    }
}

void Cache::setPost(Plurq::Post post)
{
    Q_ASSERT(post.valid());
    // We always receive full plurk posts in comet,
    // so go ahead and replace it.
    posts[post.id()] = post;
}

void Cache::setCurrentUserId(int userId)
{
    currentUserId = userId;
}
