#ifndef PLAYTESTAUDIOTHREAD_H
#define PLAYTESTAUDIOTHREAD_H
#include <QThread>
#include <QApplication>
#include <QFile>
#include <QAudioDeviceInfo>

class playTestAudioThread : public QThread
{
public:
    explicit playTestAudioThread(QObject *parent = nullptr);
    ~playTestAudioThread();
    QString testAudioPath;
    QList<QAudioDeviceInfo> outputDeviceList;

protected:
    void run();

signals:
    void isDone();

};

#endif // PLAYTESTAUDIOTHREAD_H
