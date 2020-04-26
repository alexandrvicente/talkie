#include "tray_icon.h"

TrayIcon::TrayIcon() : QSystemTrayIcon()
{
    mutedIcon = QIcon(":/icons/talkie_off.svg");
    mutedIcon.setIsMask(true);

    unmutedIcon = QIcon(":/icons/talkie_on.svg");
    unmutedIcon.setIsMask(true);

    QMenu *menu = new QMenu();

    QAction *aboutAction = menu->addAction("About Talkie");
    connect(aboutAction, &QAction::triggered, this, &TrayIcon::openAbout);

    QAction *preferencesAction = menu->addAction("Preferences...");
    connect(preferencesAction, &QAction::triggered, this, &TrayIcon::openPreferences);

    menu->addSeparator();

    QAction *updateAction = menu->addAction("Check for updates...");
    connect(updateAction, &QAction::triggered, this, &TrayIcon::checkForUpdates);

    QAction *quitAction = menu->addAction("Quit Talkie");
    connect(quitAction, &QAction::triggered, this, &TrayIcon::quit);

    setContextMenu(menu);

    QKeySequence keySequence = QKeySequence(settings.value("hotkey", "").toString());

    hotkey = new QHotkey(keySequence, true, this);

    connect(hotkey, &QHotkey::activated, this, &TrayIcon::hotkeyPressed);
    connect(hotkey, &QHotkey::released, this, &TrayIcon::hotkeyReleased);

    uint mode = settings.value("mode", 0).toUInt();
    setMute(mode == 0);
}

void TrayIcon::setMute(bool mute)
{
    if(mute){
        MicrophoneController::shared()->mute();
        setIcon(mutedIcon);
    } else {
        MicrophoneController::shared()->unmute();
        setIcon(unmutedIcon);
    }
}

void TrayIcon::playCue(bool open)
{
    if(settings.value("audioCues", true).toBool()){
        sfx.setVolume(settings.value("cueVolume", 0.1).toReal());
        QUrl source = QUrl(open ? "qrc:/sounds/open.wav" : "qrc:/sounds/close.wav");
        sfx.setSource(source);
        sfx.play();
    }
}

void TrayIcon::hotkeyPressed()
{
    uint mode = settings.value("mode", 0).toUInt();
    setMute(mode == 1);
    playCue(true);
}

void TrayIcon::hotkeyReleased()
{
    uint mode = settings.value("mode", 0).toUInt();
    setMute(mode == 0);
    playCue(false);
}

void TrayIcon::openAbout(){
    if(aboutWindow.isNull()){
        aboutWindow.reset(new AboutWindow());
    }
    aboutWindow->show();
}

void TrayIcon::openPreferences()
{
    if(preferencesWindow.isNull()){
        PreferencesWindow *ptr = new PreferencesWindow();
        preferencesWindow.reset(ptr);
        connect(ptr, &PreferencesWindow::autoUpdateCheckChanged, this, [this] () {
            updater.updateSettings();
        });
        connect(ptr, &PreferencesWindow::hotkeyChanged, this, [this] (QKeySequence shortcut) {
            hotkey->setShortcut(shortcut, true);
        });
        connect(ptr, &PreferencesWindow::modeChanged, this, [this] (uint mode) {
            setMute(mode == 0);
        });
    }
    preferencesWindow->show();
}

void TrayIcon::checkForUpdates(){
    updater.checkForUpdates(true);
}

void TrayIcon::quit(){
    QApplication::quit();
}
