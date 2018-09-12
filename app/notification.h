#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QString>

class Notification
{
public:
    static void display(const QString &title, const QString &subtitle, const QString &content);
};

#endif // NOTIFICATION_H
