#ifndef ONBOARDING_WINDOW_H
#define ONBOARDING_WINDOW_H

#include <QWizard>
#include <QBitmap>
#include <QPainter>
#include <QPushButton>
#include "tray_icon.h"
#include "../autostart_controller/autostart_controller.h"

namespace Ui {
class OnboardingWindow;
}

class OnboardingWindow : public QWizard
{
    Q_OBJECT

public:
    explicit OnboardingWindow(QWidget *parent = nullptr, TrayIcon *trayIcon = nullptr);
    ~OnboardingWindow();
    void handleCompletion();

private:
    Ui::OnboardingWindow *ui;
    TrayIcon *trayIcon;
};

#endif // ONBOARDING_WINDOW_H
