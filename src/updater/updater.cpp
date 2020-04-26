#include "updater.h"
#include "../ui/update_window.h"
#if defined(Q_OS_LINUX)
#include <QDesktopServices>
#endif

const unsigned char publicKey[] = {
    0xce, 0x40, 0x64, 0x03, 0x31, 0xeb, 0x50, 0x15,
    0x29, 0x11, 0x2e, 0x5a, 0xab, 0xb2, 0xd5, 0x24,
    0x7e, 0xbe, 0x4f, 0x23, 0x77, 0x45, 0xbd, 0xa2,
    0xa0, 0xb9, 0x5d, 0x74, 0x65, 0xba, 0x99, 0x3a
};

Updater::Updater(QObject *parent) : QObject(parent)
{
    updateSettings();
}

void Updater::checkForUpdates(bool manual)
{
#if defined(Q_OS_WIN)
    QUrl url("https://alexandrevicente.net/autoupdater/talkie/metadata/windows.json");
#elif defined(Q_OS_MACOS)
    QUrl url("https://alexandrevicente.net/autoupdater/talkie/metadata/macos.json");
#else
    QUrl url("https://alexandrevicente.net/autoupdater/talkie/metadata/linux.json");
#endif
    QNetworkRequest request(url);
    QNetworkReply *reply = manager.get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply, manual] {
        handleResponse(reply, manual);
    });
}

void Updater::handleResponse(QNetworkReply *reply, bool manual)
{
    if(reply->error() != QNetworkReply::NoError){
        if(manual){
            QMessageBox *msg = new QMessageBox();
            msg->setAttribute(Qt::WA_DeleteOnClose);
            msg->setText("Error checking for updates.");
            msg->show();
        }
        return;
    }

    QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
    QJsonObject object = document.object();
    QString latestVersionString = object.value("version").toString();
    QString changelog = object.value("changelog").toString();
    QString url = object.value("url").toString();
    QString signature = object.value("signature").toString();

    QVersionNumber latestVersion = QVersionNumber::fromString(latestVersionString);
    QVersionNumber currentVersion = QVersionNumber::fromString(QCoreApplication::applicationVersion());
    if(QVersionNumber::compare(currentVersion, latestVersion) < 0){
        QSettings settings;
        if(settings.value("skippedUpdateVersion").toString() == latestVersionString && !manual){
            return;
        }

        UpdateWindow *updateWindow = new UpdateWindow(this, latestVersionString, changelog, url, signature);
        updateWindow->setAttribute(Qt::WA_DeleteOnClose);
        updateWindow->show();
    } else if(manual) {
        QMessageBox *msg = new QMessageBox();
        msg->setAttribute(Qt::WA_DeleteOnClose);
        msg->setText("No updates are currently available.");
        msg->show();
    }
}

void Updater::updateSettings()
{
    QSettings settings;
    if(settings.value("autoUpdateCheck", false).toBool()){
        timer.setInterval(86400000);
        connect(&timer, &QTimer::timeout, this, [this] () {
            checkForUpdates();
        });
        timer.start();
        checkForUpdates();
    } else {
        timer.stop();
    }
}

void Updater::downloadAndInstall(QString url, QString signature)
{
#if defined(Q_OS_LINUX)
    QString appImage = QProcessEnvironment::systemEnvironment().value("APPIMAGE", "");
    if(appImage == ""){
        QDesktopServices::openUrl(QUrl("https://github.com/alexandrvicente/talkie/releases"));
        return;
    }
#endif

    QProgressDialog *dialog = new QProgressDialog("Downloading update...", "Cancel", 0, 1000, nullptr, Qt::WindowStaysOnTopHint);
    dialog->setMinimumDuration(0);
    dialog->setCancelButton(nullptr);
    dialog->show();

    QTemporaryDir *dir = new QTemporaryDir();
#if defined(Q_OS_WIN)
    dir->setAutoRemove(false);
#endif

    QFile *file = new QFile(dir->filePath("update"));
    file->open(QIODevice::ReadWrite);

    QNetworkRequest request((QUrl(url)));
    QNetworkReply *reply = manager.get(request);

    connect(reply, &QNetworkReply::downloadProgress, this, [dialog] (qint64 bytesReceived, qint64 bytesTotal) {
       dialog->setValue(bytesReceived  * 900.0 / bytesTotal);
    });
    connect(reply, &QNetworkReply::readyRead, this, [reply, file] () {
        file->write(reply->readAll());
    });
    connect(reply, &QNetworkReply::finished, this, [this, reply, file, dir, dialog, signature] () {
        if(reply->error() != QNetworkReply::NoError){
            dialog->hide();
            QMessageBox *msg = new QMessageBox();
            msg->setAttribute(Qt::WA_DeleteOnClose);
            msg->setText("Error downloading update.");
            msg->show();
        } else {
            file->flush();
            file->seek(0);
            dialog->setLabelText("Verifying signature...");

            crypto_sign_state state;
            crypto_sign_init(&state);
            unsigned char buffer[16834];
            int totalBytesRead = 0;
            while(!file->atEnd()){
                qint64 bytesRead = file->read(reinterpret_cast<char*>(buffer), 16834);
                crypto_sign_update(&state, buffer, bytesRead);
                totalBytesRead += bytesRead;
                dialog->setValue(900 + (totalBytesRead * 95.0 / file->size()));
            }
            unsigned char sig[crypto_sign_BYTES];
            for(uint i=0; i < crypto_sign_BYTES; i++){
                bool ok = false;
                sig[i] = static_cast<unsigned char>(signature.mid(i*2, 2).toUInt(&ok, 16));
            }

            bool validSignature = crypto_sign_final_verify(&state, sig, publicKey) == 0;
            file->close();
            if(validSignature){
                dialog->setLabelText("Installing update...");
                delete file;
                this->installUpdate(dir);
            } else {
                delete file;
                delete dir;
                dialog->hide();
                QMessageBox *msg = new QMessageBox();
                msg->setAttribute(Qt::WA_DeleteOnClose);
                msg->setText("Error verifying signature.");
                msg->show();
            }
        }

        delete reply;
        delete dialog;
    });
}

#if defined(Q_OS_WIN)

void Updater::installUpdate(QTemporaryDir *dir)
{
    QProcess::startDetached("msiexec.exe /i " + dir->filePath("update").replace("/", "\\") + " /passive LAUNCH_NEW_VERSION=1");
    qApp->quit();
}

#elif defined(Q_OS_MACOS)

void Updater::installUpdate(QTemporaryDir *dir)
{
    QString appPath = QCoreApplication::applicationDirPath();
    appPath.truncate(appPath.lastIndexOf(".app") + 4);

    QString oldVersionPath = appPath;
    oldVersionPath.truncate(oldVersionPath.lastIndexOf(".app"));
    oldVersionPath += " (" + QCoreApplication::applicationVersion() + ").app";

    appPath = appPath.replace("'", "\\'");
    oldVersionPath = oldVersionPath.replace("'", "\\'");

    QString command =
        "volume=$(hdiutil attach -nobrowse '" + dir->filePath("update") + "' | awk 'END {print $3}'; exit ${PIPESTATUS[0]})"
        "&& mv '" + appPath + "' '" + oldVersionPath + "' "
        "&& cp -R $volume/Talkie.app '" + appPath + "' "
        "&& chmod +r '" + appPath + "'"
        "&& hdiutil detach -quiet $volume || exit 0";
    int mvExitcode = system(command.toStdString().c_str());
    if(mvExitcode != 0){
        command = "osascript -e \"do shell script \\\"" + command.replace("\"", "\\\\\\\"") + "\\\" with administrator privileges\"";
        mvExitcode = system(command.toStdString().c_str());

        if(mvExitcode != 0){
            QMessageBox *msg = new QMessageBox();
            msg->setAttribute(Qt::WA_DeleteOnClose);
            msg->setText("Error installing update.");
            msg->show();
            return;
        }
    }

    QProcess::startDetached("sh -c \"sleep 1 && rm -r '" + oldVersionPath + "' && open '" + appPath + "'");
    delete dir;
    qApp->quit();
}

#else

void Updater::installUpdate(QTemporaryDir *dir)
{
    QString appPath = QProcessEnvironment::systemEnvironment().value("APPIMAGE");
    QString oldVersionPath = appPath + "-" + QCoreApplication::applicationVersion();

    appPath = appPath.replace("'", "\\'");
    oldVersionPath = oldVersionPath.replace("'", "\\'");

    QString command =
        "mv '" + appPath + "' '" + oldVersionPath + "'"
        "&& mv '" + dir->filePath("update") + "' '" + appPath + "' "
        "&& chmod +rx '" + appPath + "'";
    int mvExitcode = system(command.toStdString().c_str());
    if(mvExitcode != 0){
        command = "pkexec sh -c \"" + command.replace("\"", "\\\"") + "\"";
        mvExitcode = system(command.toStdString().c_str());

        if(mvExitcode != 0){
            QMessageBox *msg = new QMessageBox();
            msg->setAttribute(Qt::WA_DeleteOnClose);
            msg->setText("Error installing update.");
            msg->show();
            return;
        }
    }

    QProcess::startDetached("sh -c \"sleep 1 && rm -r '" + oldVersionPath + "' && '" + appPath + "'");
    delete dir;
    qApp->quit();
}

#endif
