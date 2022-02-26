#ifndef PLAYTESTAUDIOTHREAD_H
#define PLAYTESTAUDIOTHREAD_H
#include <QThread>
#include <QApplication>
#include <QFile>
#include <QAudioDeviceInfo>
#include <QAudioOutput>
#include <QAudioFormat>
#include <QTimer>

class playTestAudioThread : public QThread
{
   Q_OBJECT

public:
    explicit playTestAudioThread(QAudioDeviceInfo info, qreal vol, int duration, QObject *parent = nullptr);
    ~playTestAudioThread();
    QString testAudioPath;
    QAudioDeviceInfo deviceInfo;
    QAudioFormat format;
    qreal volume;
    int time;
    QEventLoop *loop;


protected:
    void run();

signals:
    void isDone();

};

#endif // PLAYTESTAUDIOTHREAD_H
