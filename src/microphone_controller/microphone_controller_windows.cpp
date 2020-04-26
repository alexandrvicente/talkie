#include "microphone_controller.h"

#include <QDebug>
#include <mmdeviceapi.h>
#include <Functiondiscoverykeys_devpkey.h>
#include <propvarutil.h>
#include <endpointvolume.h>

MicrophoneController* MicrophoneController::instance = new MicrophoneController();

MicrophoneController::MicrophoneController(QObject *parent)
    : QObject(parent)
{
    HRESULT hresult = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hresult))
    {
        qDebug() << "COM initialization error: " << hresult;
    }
}

MicrophoneController* MicrophoneController::shared()
{
    return instance;
}

void setMute(bool mute)
{
    HRESULT hresult = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hresult))
    {
        qDebug() << "COM initialization error: " << hresult;
        return;
    }

    IMMDeviceEnumerator *deviceEnumerator = NULL;

    hresult = CoCreateInstance(
        __uuidof(MMDeviceEnumerator), 
        NULL, 
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&deviceEnumerator) 
    );
    if (FAILED(hresult))
    {
        qDebug() << "Device enumerator initialization error: " << hresult;
        return;
    }

    IMMDeviceCollection *deviceCollection = NULL;

    hresult = deviceEnumerator->EnumAudioEndpoints(
        eCapture,
        DEVICE_STATE_ACTIVE,
        &deviceCollection
    );
    if (FAILED(hresult))
    {
        qDebug() << "Device collection initialization error: " << hresult;
        deviceEnumerator->Release();
        return;
    }

    UINT deviceCount = 0;

    hresult = deviceCollection->GetCount(&deviceCount);
    if (FAILED(hresult))
    {
        qDebug() << "Device collection count error: " << hresult;
    }

    IMMDevice *device = NULL;

    for(UINT i = 0; i < deviceCount; i++)
    {
        if(device != NULL){
            device->Release();
        }
        device = NULL;

        hresult = deviceCollection->Item(i, &device);
        if (FAILED(hresult))
        {
            qDebug() << "Error collecting device: " << i;
            continue;
        }

        IAudioEndpointVolume *volume = NULL;

        hresult = device->Activate(
            __uuidof(IAudioEndpointVolume), 
            CLSCTX_INPROC_SERVER, 
            NULL,
            reinterpret_cast<void**>(&volume)
        );
        if (FAILED(hresult))
        {
            qDebug() << "Error initializing volume for device: " << i;
            continue;
        }

        hresult = volume->SetMute(mute, NULL);
        volume->Release();

        if (FAILED(hresult))
        {
            qDebug() << "Error muting device: " << i;
            continue;
        }
    }
    deviceEnumerator->Release();
    deviceCollection->Release();
}

void MicrophoneController::mute()
{
    setMute(true);
}

void MicrophoneController::unmute()
{
    setMute(false);
}

MicrophoneController::~MicrophoneController()
{
    CoUninitialize();
}
