#ifndef TRAY_ICON_H
#define TRAY_ICON_H

#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QObject>
#include <QApplication>
#include <QScopedPointer>
#include <QHotkey>
#include <QSoundEffect>

#include "about_window.h"
#include "preferences_window.h"
#include "../microphone_controller/microphone_controller.h"
#include "../updater/updater.h"

class TrayIcon: public QSystemTrayIcon
{
private:
    QSettings settings;
    QIcon mutedIcon;
    QIcon unmutedIcon;
    QSoundEffect sfx;
    QScopedPointer<AboutWindow> aboutWindow;
    QScopedPointer<PreferencesWindow> preferencesWindow;
    Updater updater;
public:
    QHotkey *hotkey;

    TrayIcon();
    MicrophoneController *getMicrophoneController();
private:
    Q_DISABLE_COPY(TrayIcon)
    void setMute(bool mute);
    void playCue(bool open);
    void hotkeyPressed();
    void hotkeyReleased();
    void openAbout();
    void openPreferences();
    void checkForUpdates();
    void quit();
};

#endif // TRAY_ICON_H
