#include "ui/tray_icon.h"
#include "ui/onboarding_window.h"

#include <QApplication>
#include <QSettings>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);

    QCoreApplication::setOrganizationName("Alexandre Vicente");
    QCoreApplication::setOrganizationDomain("alexandrevicente.net");
    QCoreApplication::setApplicationName("Talkie");
    QCoreApplication::setApplicationVersion("0.1.0");

    TrayIcon trayIcon;
    OnboardingWindow *onboardingWindow;

    QSettings settings;
    if(settings.value("onboardingVersion", 0).toInt() < 1){
        onboardingWindow = new OnboardingWindow(nullptr, &trayIcon);
        onboardingWindow->setAttribute(Qt::WA_DeleteOnClose);
        onboardingWindow->show();
    } else {
        trayIcon.show();
    }

    int exitCode = a.exec();

    MicrophoneController::shared()->unmute();

    return exitCode;
}
