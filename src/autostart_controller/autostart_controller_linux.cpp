#include "autostart_controller.h"
#include <QDesktopServices>
#include <QProcessEnvironment>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDebug>

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
    return QProcessEnvironment::systemEnvironment().value(
        "APPIMAGE",
        QCoreApplication::arguments().first()
    );
}

QString getAutostartDir()
{
    return QProcessEnvironment::systemEnvironment().value(
        "XDG_CONFIG_HOME",
        QProcessEnvironment::systemEnvironment().value("HOME") + "/.config"
    ) + "/autostart";
}

QString getDesktopFilePath()
{
    return getAutostartDir() + "/net.alexandrevicente.Talkie.desktop";
}

bool AutostartController::enabled()
{
    QString desktopFilePath = getDesktopFilePath();
    QFile file(desktopFilePath);
    if(!file.exists()) {
        return false;
    }
    file.open(QIODevice::ReadOnly);
    bool disabled = file.readAll().contains("Hidden=true");
    file.close();
    return !disabled;
}

void AutostartController::setEnabled(bool enabled)
{
    QDir().mkpath(getAutostartDir());
    QFile file(getDesktopFilePath());
    file.open(QIODevice::ReadWrite);
    if(file.size() == 0){
        QFile fileTemplate(":/misc/talkie.desktop");
        fileTemplate.open(QIODevice::ReadOnly);
        file.write(fileTemplate.readAll());
        fileTemplate.close();
        file.seek(0);
    }
    QStringList content;
    QTextStream stream(&file);
    bool hasHiddenLine = false;
    bool hasExecLine = false;
    while(!stream.atEnd()) {
        QString line = stream.readLine();
        if(line.startsWith("Hidden=")) {
            line = "Hidden=";
            line += (enabled ? "false" : "true");
            hasHiddenLine = true;
        } else if(line.startsWith("Exec=")) {
            line = "Exec=";
            line += getAppPath();
            hasExecLine = true;
        } else if(line.startsWith("Version="))  {
            line = "Version=";
            line += QCoreApplication::applicationVersion();
        }
        content.append(line);
    }
    if(!hasExecLine) {
        QString line = "Exec=";
        line += getAppPath();
        content.insert(content.indexOf("[Desktop Entry]") + 1, line);
    }
    if(!hasHiddenLine) {
        QString line = "Hidden=";
        line += (enabled ? "true" : "false");
        content.insert(content.indexOf("[Desktop Entry]") + 1, line);
    }
    file.seek(0);
    file.write(content.join("\n").toUtf8());
    file.close();
}
