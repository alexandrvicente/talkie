QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/ui/about_window.cpp \
    src/ui/hotkey_input.cpp \
    src/main.cpp \
    src/ui/onboarding_window.cpp \
    src/ui/onboarding_window_hotkey_page.cpp \
    src/ui/preferences_window.cpp \
    src/ui/tray_icon.cpp \
    src/ui/update_window.cpp \
    src/updater/updater.cpp

mac: SOURCES += \
    src/autostart_controller/autostart_controller_macos.cpp \
    src/microphone_controller/microphone_controller_macos.cpp

linux: SOURCES += \
    src/autostart_controller/autostart_controller_linux.cpp \
    src/microphone_controller/microphone_controller_linux.cpp

windows: SOURCES += \
    src/autostart_controller/autostart_controller_windows.cpp \
    src/microphone_controller/microphone_controller_windows.cpp

mac: INCLUDEPATH += /usr/local/include

HEADERS += \
    src/autostart_controller/autostart_controller.h \
    src/ui/about_window.h \
    src/ui/hotkey_input.h \
    src/microphone_controller/microphone_controller.h \
    src/ui/onboarding_window.h \
    src/ui/onboarding_window_hotkey_page.h \
    src/ui/preferences_window.h \
    src/ui/tray_icon.h \
    src/ui/update_window.h \
    src/updater/updater.h

FORMS += \
    src/ui/about_window.ui \
    src/ui/onboarding_window.ui \
    src/ui/preferences_window.ui \
    src/ui/update_window.ui

TRANSLATIONS += \
    i18n/Talkie_en_US.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += resources/resources.qrc

mac: LIBS += \
    -framework \
    CoreServices \
    -framework \
    CoreAudio \
     /usr/local/lib/libsodium.a

windows: LIBS += \
    -lole32 \
    -loleaut32 \
    -lpropsys \
    -ladvapi32 \
    -llibssl \
    -llibsodium

linux: LIBS += -lsodium


mac {
    QMAKE_INFO_PLIST = misc/mac/Info.plist
    ICON = resources/talkie.icns
}

windows: RC_ICONS = resources/talkie.ico

unix {
    isEmpty(PREFIX) {
        PREFIX = /usr/local
    }

    target.path = $$PREFIX/bin

    shortcutfiles.files = resources/talkie.desktop
    shortcutfiles.path = $$PREFIX/share/applications/
    data.files += resources/talkie.png
    data.path = $$PREFIX/share/pixmaps/

    INSTALLS += shortcutfiles
    INSTALLS += data
}

INSTALLS += target


include(vendor/vendor.pri)
