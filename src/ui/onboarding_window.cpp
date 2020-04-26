#include "onboarding_window.h"
#include "ui_onboarding_window.h"

OnboardingWindow::OnboardingWindow(QWidget *parent, TrayIcon *trayIcon) :
    QWizard(parent),
    ui(new Ui::OnboardingWindow)
{
    ui->setupUi(this);
    //setWindowFlags(Qt::Tool);
    setWindowFlags(Qt::WindowStaysOnTopHint);

    this->trayIcon = trayIcon;

    ui->wizardPage2->setHotkeyInput(ui->hotkey);

    connect(button(WizardButton::FinishButton), &QPushButton::clicked, this, &OnboardingWindow::handleCompletion);
}

OnboardingWindow::~OnboardingWindow()
{
    delete ui;
}

void OnboardingWindow::handleCompletion()
{
    QSettings settings;
    settings.setValue("hotkey", ui->hotkey->getKeySequence().toString());
    AutostartController::shared()->setEnabled(ui->startAtLogin->isChecked());
    settings.setValue("autoUpdateCheck", ui->autoUpdateCheck->isChecked());
    settings.setValue("audioCues", ui->audioCues->isChecked());
    settings.setValue("onboardingVersion", 1);

    trayIcon->hotkey->setShortcut(ui->hotkey->getKeySequence(), true);

    trayIcon->show();
}
