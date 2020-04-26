#include "onboarding_window_hotkey_page.h"

OnboardingWindowHotkeyPage::OnboardingWindowHotkeyPage(QWidget *parent)
    : QWizardPage(parent)
{
}

void OnboardingWindowHotkeyPage::setHotkeyInput(HotkeyInput *hotkeyInput)
{
    registerField("hotkey*", hotkeyInput);
}
