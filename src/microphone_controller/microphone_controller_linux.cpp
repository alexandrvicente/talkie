#include "microphone_controller.h"

MicrophoneController* MicrophoneController::instance = new MicrophoneController();

MicrophoneController::MicrophoneController(QObject *parent)
    : QObject(parent)
{
}

MicrophoneController* MicrophoneController::shared()
{
    return instance;
}

void MicrophoneController::mute()
{
    system("amixer set Capture nocap > /dev/null");
}


void MicrophoneController::unmute()
{
    system("amixer set Capture cap > /dev/null");
}

MicrophoneController::~MicrophoneController()
{
}
