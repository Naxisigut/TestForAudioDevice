#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QAudioRecorder>
#include <QtDebug>
#include <QAudioDeviceInfo>
#include <QAudioOutput>
//#include <QAudio>
#include <QFile>
#include <QMessageBox>
#include <QUrl>
#include <QTest>
#include <QtMath>
#include <QMediaPlayer>
#include <fftw3.h>
#include <AudioFile.h>

//#define N 131072
#define N 8192

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    QString testAudioPath;
    QAudioFormat format;
    QList<QAudioDeviceInfo> outputDeviceList;
    fftw_complex *din,*out;
    fftw_plan fftwPlan;
    QAudioRecorder *audioRecorder;
    double *powOut,*fOut;
//    QMediaPlayer *audioPlayer;
    double FindMaxInArray(double arr[],int cnt);
    void getPowArrayAtFrequency(double f, double arr[], int cnt, double sourceArr[]);
    double THDCalculate(double f, double sourcePow[]);
    void TestFuncBase();
    void TestFunc1st();
    void TestFunc2nd();
    void playTestSound(qreal volume, int duration);

protected:
    bool eventFilter(QObject *obj, QEvent *e);

private slots:
    void on_startButton_clicked();

    void on_stopButton_clicked();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
