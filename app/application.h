#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenuBar>
#include <plurk.h>

class Application : public QApplication
{
    Q_OBJECT
public:
    Application(int &argc, char** argv);

protected:
    void initializeComponents();
    void authorize();

    Plurk *plurk;
    QSystemTrayIcon *trayIcon;
    QMenu *contextMenu;
    QMenuBar *menuBar;  // macOS/Unix only

signals:
    void profileUpdated(QString &name);

protected slots:
    void authorized();
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
};

#endif // APPLICATION_H
