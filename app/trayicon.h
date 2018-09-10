#ifndef TRAYICON_H
#define TRAYICON_H

#include <QSystemTrayIcon>
#include <QMenu>
#include <Plurk>

class TrayIcon : public QSystemTrayIcon
{
    Q_OBJECT
public:
    TrayIcon();
    ~TrayIcon();

public slots:
    void updateProfile(Plurq::Profile *profile);

signals:
    void quitRequested();

protected:
    QMenu *m_menu = nullptr;
    QAction *accountMenuItem;
    QAction *notificationMenuItem;

protected slots:
    void onActivated(QSystemTrayIcon::ActivationReason reason);
};

#endif // TRAYICON_H
