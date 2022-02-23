#ifndef WAVIODEVICE_H
#define WAVIODEVICE_H

#include <AudioFile.h>
#include <QIODevice>
#include <QAudioFormat>

class WavIODevice : public QIODevice
{
public:
    WavIODevice();
    ~WavIODevice();

    bool isSequential() const{return false;}
    bool open(QString fileName, OpenMode mode, QAudioFormat format = QAudioFormat());
    void close();
    bool canReadLine()const{return false;}
    qint64 pos() const;
    qint64 size() const;


};

#endif // WAVIODEVICE_H
