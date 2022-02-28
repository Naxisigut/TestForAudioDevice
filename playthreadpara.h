#ifndef PLAYTHREADPARA_H
#define PLAYTHREADPARA_H

#include <QAudioDeviceInfo>

class PlayThreadPara
{
public:
    PlayThreadPara();
    ~PlayThreadPara();
    QAudioDeviceInfo info;//DeviceToPlay
    double volume;
    int duration;//PlayDuration, ms, less than 10000

};

#endif // PLAYTHREADPARA_H
