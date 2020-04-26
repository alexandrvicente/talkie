#ifndef UPDATER_H
#define UPDATER_H

#include <QObject>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVersionNumber>
#include <QCoreApplication>
#include <QSettings>
#include <QProgressDialog>
#include <QTemporaryDir>
#include <QFile>
#include <QProcess>
#include <QDesktopServices>

extern "C"
{
    #include <sodium.h>
}

class Updater : public QObject
{
    Q_OBJECT
private:
    QTimer timer;
    QNetworkAccessManager manager;
public:
    explicit Updater(QObject *parent = nullptr);
    void checkForUpdates(bool manual = false);
    void updateSettings();
    void downloadAndInstall(QString url, QString signature);
private:
    void handleResponse(QNetworkReply *reply, bool manual);
    void installUpdate(QTemporaryDir *dir);
};

#endif // UPDATER_H
