#ifndef PREFERENCES_WINDOW_H
#define PREFERENCES_WINDOW_H

#include <QDialog>
#include <QObject>
#include <QSettings>

#include "hotkey_input.h"
#include "../autostart_controller/autostart_controller.h"

namespace Ui { class PreferencesWindow; }

class PreferencesWindow : public QDialog
{
    Q_OBJECT

private:
    Ui::PreferencesWindow *ui;
    bool supportsAutoUpdate;
    QSettings settings;
public:
    explicit PreferencesWindow(QWidget *parent = nullptr);
    ~PreferencesWindow();
signals:
    void startAtLoginChanged(bool enabled);
    void autoUpdateCheckChanged(bool enabled);
    void hotkeyChanged(QKeySequence newHotkey);
    void modeChanged(uint mode);
private:
    void onStartAtLoginChange();
    void onAutoUpdateCheckChange();
    void onAudioCuesChange();
    void onCueVolumeChange();
    void onHotkeyChange();
    void setMode(uint mode);
    void onModeUnmuteSelect();
    void onModeMuteSelect();
};
#endif // PREFERENCES_WINDOW_H
