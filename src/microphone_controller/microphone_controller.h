#ifndef MICROPHONECONTROLLER_H
#define MICROPHONECONTROLLER_H

#include <QObject>

class MicrophoneController : public QObject
{
    Q_OBJECT
public:
    static MicrophoneController *shared();
    ~MicrophoneController();

    void mute();
    void unmute();
private:
    static MicrophoneController *instance;
    MicrophoneController(QObject *parent = nullptr);
    uint unmutedVolume;
};

#endif // MICROPHONECONTROLLER_H
