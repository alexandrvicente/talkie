#include "autostart_controller.h"
#include <QCoreApplication>
#include <windows.h>
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
    return QCoreApplication::arguments().first();
}

bool AutostartController::enabled()
{
    HKEY hkey;
    long status = RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_READ, &hkey);
    if(status != ERROR_SUCCESS) {
        return false;
    }
    wchar_t buffer[MAX_PATH];
    DWORD bufferSize = sizeof(buffer);
    status = RegQueryValueExW(hkey, L"net.alexandrevicente.Talkie", 0, NULL, (LPBYTE)buffer, &bufferSize);
    if(status != ERROR_SUCCESS) {
        return false;
    }
    status = RegCloseKey(hkey);
    QString registryValue = QString::fromWCharArray(buffer, bufferSize / sizeof(wchar_t));
    return registryValue.compare(getAppPath()) == 0;
}

void AutostartController::setEnabled(bool enabled)
{
    HKEY hkey;
    long status = RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_WRITE, &hkey);
    if(status != ERROR_SUCCESS) {
        return;
    }
    if(enabled){
        wchar_t appPath[MAX_PATH];
        DWORD size = getAppPath().toWCharArray(appPath) * sizeof(wchar_t);
        status = RegSetValueExW(hkey, L"net.alexandrevicente.Talkie", 0, REG_SZ, reinterpret_cast<BYTE*>(appPath), size);
    } else {
        status = RegDeleteValueW(hkey, L"net.alexandrevicente.Talkie");
    }
    status = RegCloseKey(hkey);
}
