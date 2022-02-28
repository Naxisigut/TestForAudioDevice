#ifndef PLAYTESTAUDIOTHREAD_H
#define PLAYTESTAUDIOTHREAD_H
#include <QThread>
#include <QApplication>
#include <QFile>
#include <QAudioDeviceInfo>
#include <QAudioOutput>
#include <QAudioFormat>
#include <QTimer>
#include <playthreadpara.h>

class playTestAudioThread : public QThread
{
   Q_OBJECT

public:
    explicit playTestAudioThread(PlayThreadPara para, QObject *parent = nullptr);
    ~playTestAudioThread();
    QString testAudioPath;
    PlayThreadPara parameter;
    QAudioFormat format;
//    QAudioDeviceInfo deviceInfo;
//    qreal volume;
//    int time;
    QEventLoop *loop;


protected:
    void run();

signals:
    void isDone();

};

#endif // PLAYTESTAUDIOTHREAD_H
