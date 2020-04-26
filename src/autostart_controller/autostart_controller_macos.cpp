#include "autostart_controller.h"
#include <QCoreApplication>
#include <QProcess>
#include <QProcessEnvironment>
#include <QFile>

AutostartController* AutostartController::instance = new AutostartController();

AutostartController::AutostartController(QObject *parent) : QObject(parent)
{
}

AutostartController* AutostartController::shared()
{
    return instance;
}

QString getAppPath()
{
    QString appPath = QCoreApplication::applicationDirPath();
    appPath.truncate(appPath.lastIndexOf(".app") + 4);
    return appPath;
}

QString getLaunchAgentPath()
{
    return QProcessEnvironment::systemEnvironment().value("HOME")
            + "/Library/LaunchAgents/net.alexandrevicente.talkie.startatlogin.plist";
}

QFile* getLaunchAgentFile()
{
    return new QFile(getLaunchAgentPath());
}

bool AutostartController::enabled()
{
    QFile *file = getLaunchAgentFile();
    bool exists = file->exists();
    delete file;
    if(!exists) {
        return false;
    }
    QProcess process;
    process.start("launchctl", {"list"});
    process.waitForFinished();
    return process.readAllStandardOutput().contains("net.alexandrevicente.talkie.startatlogin");
}

void AutostartController::setEnabled(bool enabled)
{
    QFile *file = getLaunchAgentFile();
    file->open(QIODevice::WriteOnly);
    QString content(
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">"
        "<plist version=\"1.0\">"
        "<dict>"
              "<key>Label</key>"
              "<string>net.alexandrevicente.talkie.startatlogin</string>"
              "<key>ProgramArguments</key>"
              "<array>"
                  "<string>"
     );
     content += getAppPath();
     content += (
                  "</string>"
              "</array>"
              "<key>RunAtLoad</key>"
              "<true/>"
        "</dict>"
        "</plist>"
    );
    file->write(content.toUtf8());
    file->close();

    QProcess process;
    process.start("launchctl", {(enabled ? "load" : "unload"), getLaunchAgentPath()});
    process.waitForFinished();
}
