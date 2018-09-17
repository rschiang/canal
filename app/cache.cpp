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

Plurq::Post& Cache::response(int responseId)
{
    return responses[responseId];
}

Plurq::Profile& Cache::currentUser()
{
    return users[m_currentUserId];
}

bool Cache::userExists(int userId) const
{
    return users.contains(userId);
}

bool Cache::postExists(int postId) const
{
    return posts.contains(postId);
}

int Cache::setUser(Plurq::Profile profile)
{
    Q_ASSERT(profile.valid());

    int userId = profile.id();
    if (users.contains(userId)) {
        // Do not replace entity directly
        // as user might be partially updated
        QJsonObject& entity = profile.entity();
        QJsonObject& cached = users[userId].entity();
        for (QString key : entity.keys())
            cached[key] = entity[key];
    } else {
        users[userId] = profile;
    }

    // Publish cache update
    emit userChanged(userId);
    return userId;
}

int Cache::setPost(Plurq::Post post)
{
    Q_ASSERT(post.valid());

    int postId = post.id();
    // We always receive full plurk posts in comet,
    // so go ahead and replace it.
    posts[postId] = post;

    // Publish cache update
    emit postChanged(postId);
    return postId;
}

int Cache::setResponse(Plurq::Post response)
{
    Q_ASSERT(response.valid());
    int responseId = response.id();
    // TODO: Associate plurk with response
    responses[responseId] = response;
    return responseId;
}

void Cache::setCurrentUserId(int userId)
{
    m_currentUserId = userId;
}
