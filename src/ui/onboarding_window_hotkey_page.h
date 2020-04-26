#ifndef ONBOARDINGWINDOWHOTKEYPAGE_H
#define ONBOARDINGWINDOWHOTKEYPAGE_H

#include <QWizardPage>
#include "hotkey_input.h"

class OnboardingWindowHotkeyPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit OnboardingWindowHotkeyPage(QWidget *parent = nullptr);
    void setHotkeyInput(HotkeyInput *hotkeyInput);
};

#endif // ONBOARDINGWINDOWHOTKEYPAGE_H
