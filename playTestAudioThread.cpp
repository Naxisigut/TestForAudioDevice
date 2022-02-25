#include "playTestAudioThread.h"

playTestAudioThread::playTestAudioThread(QObject *parent) : QThread(parent)
{
        testAudioPath = qApp->applicationDirPath().append("/1KHz.pcm");

}

void playTestAudioThread::run()
{
    QFile testAudioFile(testAudioPath);
    testAudioFile.open(QIODevice::ReadOnly);

    QAudioDeviceInfo info = outputDeviceList.at(ui->outputListCombox->currentIndex());
//    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
//    if (info.isFormatSupported(format) != true){
//        qDebug() << "Raw audio format not supported by backend, cannot play audio.";
//    }
//    else{
    QAudioOutput audioOutput(info, format, this);
    audioOutput.start(&testAudioFile);
    audioOutput.setVolume(volume);
//    qDebug()<< output.state();
//    qDebug() << output.error();
//    QTest::qWait(duration);
    QEventLoop *loop = new QEventLoop;
    connect(&testSoundDuration, &QTimer::timeout,
            [=]()
            {
               loop->exit();
//               qDebug() <<"exit";
               testSoundDuration.stop();
            }
            );
    testSoundDuration.start(duration);
    loop->exec();
    audioOutput.stop();
//    qDebug()<<"stop";
    testAudioFile.close();
}
