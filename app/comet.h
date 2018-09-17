#ifndef COMET_H
#define COMET_H

#include <QObject>
#include <QTimer>
#include <QNetworkAccessManager>
#include <Plurk>
#include "cache.h"

#define EMPTY_OFFSET -1
#define BAD_OFFSET -3

class Comet : public QObject
{
    Q_OBJECT
public:
    explicit Comet(Plurq::Plurk *plurk, QObject *parent = nullptr);
    ~Comet();
    void setCache(Cache *cache);

signals:
    void newPlurk(int postId);
    void newResponse(int postId, int responseId);
    void profileUpdated(Plurq::Profile *profile);

public slots:
    void start();
    void stop();
    void updateProfile();
    void updateAlerts();
    void updateUser(int userId);
    void updatePost(int postId);

protected slots:
    void initiate();
    void send();
    void abort();
    void timeout();
    void networkStateChanged(QNetworkAccessManager::NetworkAccessibility status);
    void powerStateChanged(bool suspend);

protected:
    Plurq::Plurk* plurk;
    Cache *cache;
    QTimer *timer;
    QNetworkAccessManager *manager;
    QNetworkReply *activeReply = nullptr;

    QString url;
    QString name;
    bool enabled = false;
    int offset = BAD_OFFSET;
};

#endif // COMET_H
