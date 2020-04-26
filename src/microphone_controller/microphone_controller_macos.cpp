#include "microphone_controller.h"

#include <QProcess>
#include <QStringList>
#include <QStringBuilder>
#include <QSettings>
#include <QDebug>
#include <CoreServices/CoreServices.h>
#include <CoreAudio/CoreAudio.h>

QList<AudioDeviceID> getInputDeviceIds();

MicrophoneController* MicrophoneController::instance = new MicrophoneController();

MicrophoneController::MicrophoneController(QObject *parent)
    : QObject(parent)
{
}

MicrophoneController* MicrophoneController::shared()
{
    return instance;
}

void setMuted(bool mute)
{
    AudioObjectPropertyAddress propertyAddress = {
        kAudioDevicePropertyMute,
        kAudioObjectPropertyScopeInput,
        kAudioObjectPropertyElementMaster
    };
    UInt32 size = sizeof(UInt32);
    UInt32 muteVal = (UInt32) mute;
    foreach( AudioDeviceID id, getInputDeviceIds() ){
        OSStatus status = AudioObjectSetPropertyData(id, &propertyAddress, 0, NULL, size, &muteVal);
        if(kAudioHardwareNoError != status){
            fprintf(stderr, "AudioObjectSetPropertyData failed: %i\n", status);
        }
    }
}

void MicrophoneController::mute(){
    setMuted(true);
}


void MicrophoneController::unmute(){
    setMuted(false);
}

// Modified from https://stackoverflow.com/a/4577271
QList<AudioDeviceID> getInputDeviceIds()
{
    QList<AudioDeviceID> list;

    AudioObjectPropertyAddress propertyAddress = {
        kAudioHardwarePropertyDevices,
        kAudioObjectPropertyScopeGlobal,
        kAudioObjectPropertyElementMaster
    };

    UInt32 dataSize = 0;
    OSStatus status = AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, &propertyAddress, 0, NULL, &dataSize);
    if(kAudioHardwareNoError != status) {
        fprintf(stderr, "AudioObjectGetPropertyDataSize (kAudioHardwarePropertyDevices) failed: %i\n", status);
        return list;
    }

    UInt32 deviceCount = static_cast<UInt32>(dataSize / sizeof(AudioDeviceID));

    AudioDeviceID *audioDevices = static_cast<AudioDeviceID *>(malloc(dataSize));
    if(NULL == audioDevices) {
        fputs("Unable to allocate memory", stderr);
        return list;
    }

    status = AudioObjectGetPropertyData(kAudioObjectSystemObject, &propertyAddress, 0, NULL, &dataSize, audioDevices);
    if(kAudioHardwareNoError != status) {
        fprintf(stderr, "AudioObjectGetPropertyData (kAudioHardwarePropertyDevices) failed: %i\n", status);
        free(audioDevices), audioDevices = NULL;
        return list;
    }

    // Iterate through all the devices and determine which are input-capable
    propertyAddress.mScope = kAudioDevicePropertyScopeInput;

    for(UInt32 i = 0; i < deviceCount; i++){
        // Determine if the device is an input device (it is an input device if it has input channels)
        dataSize = 0;
        propertyAddress.mSelector = kAudioDevicePropertyStreamConfiguration;
        status = AudioObjectGetPropertyDataSize(audioDevices[i], &propertyAddress, 0, NULL, &dataSize);
        if(kAudioHardwareNoError != status) {
            fprintf(stderr, "AudioObjectGetPropertyDataSize (kAudioDevicePropertyStreamConfiguration) failed: %i\n", status);
            continue;
        }

        AudioBufferList *bufferList = static_cast<AudioBufferList *>(malloc(dataSize));
        if(NULL == bufferList) {
            fputs("Unable to allocate memory", stderr);
            break;
        }

        status = AudioObjectGetPropertyData(audioDevices[i], &propertyAddress, 0, NULL, &dataSize, bufferList);
        if(kAudioHardwareNoError != status || 0 == bufferList->mNumberBuffers) {
            if(kAudioHardwareNoError != status)
                fprintf(stderr, "AudioObjectGetPropertyData (kAudioDevicePropertyStreamConfiguration) failed: %i\n", status);
            free(bufferList), bufferList = NULL;
            continue;
        }

        free(bufferList), bufferList = NULL;

        list.append(audioDevices[i]);
    }

    free(audioDevices);

    return list;
}

MicrophoneController::~MicrophoneController()
{
}
