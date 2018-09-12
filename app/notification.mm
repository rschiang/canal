#include "notification.h"
#include <Foundation/NSUserNotification.h>
#include <Foundation/NSString.h>

#define toNSString(x) [[NSString alloc] initWithUTF8String: x.toUtf8().data()]

void Notification::display(const QString &title, const QString &subtitle, const QString &content)
{
    NSUserNotification* notification = [[[NSUserNotification alloc] init] autorelease];
    notification.title = toNSString(title);
    notification.subtitle = toNSString(subtitle);
    notification.informativeText = toNSString(content);

    [[NSUserNotificationCenter defaultUserNotificationCenter] deliverNotification: notification];
}
