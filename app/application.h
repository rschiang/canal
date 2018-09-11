#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QMenuBar>
#include <Plurk>
#include "cache.h"
#include "comet.h"
#include "trayicon.h"

class Application : public QApplication
{
    Q_OBJECT
public:
    Application(int &argc, char** argv);
    ~Application();

protected:
    void initializeComponents();
    void authorize();
    void loadCredentials();
    void saveCredentials();

    Plurq::Plurk *plurk;
    Cache cache;
    Comet *comet;
    TrayIcon *trayIcon;
    QMenuBar *menuBar;  // macOS/Unix only

signals:
    void authorized();
};

#endif // APPLICATION_H
