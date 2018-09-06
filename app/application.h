#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenuBar>
#include <Plurk>
#include "comet.h"

class Application : public QApplication
{
    Q_OBJECT
public:
    Application(int &argc, char** argv);

protected:
    void initializeComponents();
    void authorize();

    Plurq::Plurk *plurk;
    Plurq::Profile *profile;
    Comet *comet;
    QSystemTrayIcon *trayIcon;
    QMenu *contextMenu;
    QMenuBar *menuBar;  // macOS/Unix only

signals:
    void profileUpdated();

protected slots:
    void authorized();
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
};

#endif // APPLICATION_H
