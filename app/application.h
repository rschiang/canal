#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenuBar>
#include <plurk.h>

class Application : public QApplication
{
public:
    Application(int &argc, char** argv);

protected:
    void initializeComponents();

    Plurk *plurk;
    QSystemTrayIcon *trayIcon;
    QMenuBar *menu;
};

#endif // APPLICATION_H
