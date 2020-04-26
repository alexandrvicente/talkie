#include "preferences_window.h"
#include "ui_preferences_window.h"

PreferencesWindow::PreferencesWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PreferencesWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Tool);
    setWindowFlags(Qt::WindowStaysOnTopHint);
    supportsAutoUpdate = false;

    ui->startAtLogin->setChecked(AutostartController::shared()->enabled());
    ui->autoUpdateCheck->setChecked(settings.value("autoUpdateCheck", false).toBool());

    bool audioCues = settings.value("audioCues", true).toBool();
    ui->audioCues->setChecked(audioCues);
    ui->cueVolume->setValue(settings.value("cueVolume", 0.1).toReal() * 100.0);
    ui->cueVolume->setDisabled(!audioCues);

    ui->hotkey->setKeySequence(QKeySequence(settings.value("hotkey", "").toString()));

    uint mode = settings.value("mode", 0).toUInt();

    ui->modeUnmute->setChecked(mode == 0);
    ui->modeMute->setChecked(mode == 1);

    connect(ui->startAtLogin, &QCheckBox::toggled, this, &PreferencesWindow::onStartAtLoginChange);
    connect(ui->autoUpdateCheck, &QCheckBox::toggled, this, &PreferencesWindow::onAutoUpdateCheckChange);
    connect(ui->audioCues, &QCheckBox::toggled, this, &PreferencesWindow::onAudioCuesChange);
    connect(ui->cueVolume, &QSlider::valueChanged, this, &PreferencesWindow::onCueVolumeChange);
    connect(ui->hotkey, &HotkeyInput::keySequenceChanged, this, &PreferencesWindow::onHotkeyChange);
    connect(ui->modeMute, &QCheckBox::toggled, this, &PreferencesWindow::onModeMuteSelect);
    connect(ui->modeUnmute, &QCheckBox::toggled, this, &PreferencesWindow::onModeUnmuteSelect);
}

PreferencesWindow::~PreferencesWindow()
{
    delete ui;
}

void PreferencesWindow::onStartAtLoginChange()
{
    bool enabled = ui->startAtLogin->isChecked();
    AutostartController::shared()->setEnabled(enabled);
    emit startAtLoginChanged(enabled);
}

void PreferencesWindow::onAutoUpdateCheckChange()
{
    bool enabled = ui->autoUpdateCheck->isChecked();
    settings.setValue("autoUpdateCheck", enabled);
    emit autoUpdateCheckChanged(enabled);
}

void PreferencesWindow::onAudioCuesChange()
{
    bool checked = ui->audioCues->isChecked();
    settings.setValue("audioCues", checked);
    ui->cueVolume->setDisabled(!checked);
}

void PreferencesWindow::onCueVolumeChange(){
    settings.setValue("cueVolume", ui->cueVolume->value() / 100.0);
}

void PreferencesWindow::onHotkeyChange()
{
    settings.setValue("hotkey", ui->hotkey->getKeySequence().toString());
    emit hotkeyChanged(ui->hotkey->getKeySequence());
}

void PreferencesWindow::setMode(uint mode)
{
    settings.setValue("mode", mode);
    emit modeChanged(mode);
}

void PreferencesWindow::onModeMuteSelect()
{
    setMode(1);
    ui->modeUnmute->setChecked(false);
}

void PreferencesWindow::onModeUnmuteSelect()
{
    setMode(0);
    ui->modeMute->setChecked(false);
}
