#ifndef COMET_H
#define COMET_H

#include <QObject>
#include <QTimer>
#include <QNetworkAccessManager>
#include <Plurk>

#define EMPTY_OFFSET -1
#define BAD_OFFSET -3

class Comet : public QObject
{
    Q_OBJECT
public:
    explicit Comet(Plurq::Plurk *plurk, QObject *parent = nullptr);

signals:
    void newPlurk();
    void newResponse();

public slots:
    void start();
    void stop();

protected slots:
    void initiate();
    void send();
    void abort();
    void timeout();
    void networkStateChanged(QNetworkAccessManager::NetworkAccessibility status);
    void powerStateChanged(bool suspend);

protected:
    Plurq::Plurk* plurk;
    QTimer *timer;
    QNetworkAccessManager *manager;
    QNetworkReply *activeReply = nullptr;

    QString url;
    QString name;
    bool enabled = false;
    int offset = BAD_OFFSET;
};

#endif // COMET_H
