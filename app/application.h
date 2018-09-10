#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QMenuBar>
#include <Plurk>
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

    Plurq::Plurk *plurk;
    Plurq::Profile *profile;
    Comet *comet;
    TrayIcon *trayIcon;
    QMenuBar *menuBar;  // macOS/Unix only

signals:
    void profileUpdated(Plurq::Profile *profile);

protected slots:
    void authorized();
};

#endif // APPLICATION_H
