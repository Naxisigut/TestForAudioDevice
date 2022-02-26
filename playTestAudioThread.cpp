#include "playTestAudioThread.h"

playTestAudioThread::playTestAudioThread(QAudioDeviceInfo info, qreal vol, int duration, QObject *parent) : QThread(parent)
{
    testAudioPath = qApp->applicationDirPath().append("/1KHz.pcm");
    deviceInfo = info;
    // Set up the format, eg.
    format.setSampleRate(44100);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);
    volume = vol;
    time = duration;


}

playTestAudioThread::~playTestAudioThread()
{
    delete loop;
}

void playTestAudioThread::run()
{
    QFile testAudioFile(testAudioPath);
    testAudioFile.open(QIODevice::ReadOnly);

    QAudioOutput audioOutput(deviceInfo, format);
    audioOutput.start(&testAudioFile);
    audioOutput.setVolume(volume);
    QTimer testSoundDuration;
//    qDebug()<< output.state();
//    qDebug() << output.error();
//    QTest::qWait(duration);
    loop = new QEventLoop;
    connect(&testSoundDuration, &QTimer::timeout,
            [&]()
            {
               loop->exit();
//               qDebug() <<"exit";
               testSoundDuration.stop();
               audioOutput.stop();
               testAudioFile.close();
               emit isDone();
            }
            );

    testSoundDuration.start(time);
    loop->exec();

//    qDebug()<<"stop";

}
