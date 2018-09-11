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

Plurq::Profile& Cache::current()
{
    return users[currentUserId];
}

int Cache::setUser(Plurq::Profile profile)
{
    Q_ASSERT(profile.valid());

    int userId = profile.id();
    if (users.contains(userId)) {
        // TODO: Update necessary fields only
    } else {
        users[userId] = profile;
    }

    return userId;
}

int Cache::setPost(Plurq::Post post)
{
    Q_ASSERT(post.valid());
    int postId = post.id();
    // We always receive full plurk posts in comet,
    // so go ahead and replace it.
    posts[postId] = post;
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
    currentUserId = userId;
}
