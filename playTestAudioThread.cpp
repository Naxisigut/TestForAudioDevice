#include "playTestAudioThread.h"

playTestAudioThread::playTestAudioThread(PlayThreadPara para, QObject *parent) : QThread(parent)
{
    testAudioPath = qApp->applicationDirPath().append("/1KHz.pcm");
    parameter = para;
//    deviceInfo = info;
    // Set up the format, eg.
    format.setSampleRate(44100);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);
}

playTestAudioThread::~playTestAudioThread()
{
    delete loop;
}

void playTestAudioThread::run()
{
    QFile testAudioFile(testAudioPath);
    testAudioFile.open(QIODevice::ReadOnly);

    QAudioOutput audioOutput(parameter.info, format);
    audioOutput.start(&testAudioFile);
    audioOutput.setVolume(parameter.volume);
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

    testSoundDuration.start(parameter.duration);
    loop->exec();

//    qDebug()<<"stop";

}
