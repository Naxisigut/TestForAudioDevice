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
#include <QTimer>
#include <QStandardItemModel>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <fftw3.h>
#include <AudioFile.h>
#include <playTestAudioThread.h>

//#define N 131072
#define N 32768



QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    QString recordedAudioPath;
    QAudioFormat format;
    QList<QAudioDeviceInfo> outputDeviceList;
    QList<QAudioDeviceInfo> inputDeviceList;
    QList<QSerialPortInfo> portInfoList;
    fftw_complex *din,*out;
    fftw_plan fftwPlan;
    QAudioRecorder *audioRecorder;
    double *powOut,*fOut;
    playTestAudioThread *playThread;
    QStandardItemModel *tableModel;
    QSerialPort *portAR720;
    const char *BT_CMD = "cmd_switch_to_bt\n";
    const char *USB_CMD = "cmd_switch_to_usb\n";
    const char *Master_CMD = "close_master_mic\n";
    const char *Deputy_CMD = "close_deputy_mic\n";

    double  FindMaxInArray(double arr[],int cnt);
    void    getPowArrayAtFrequency(double f, double arr[], int cnt, double sourceArr[]);
    double  THDCalculate(double f, double sourcePow[]);
    void    TestFuncBase();
    void    TestFunc1st();
    void    TestFunc2nd();
    void    playTestSound(QString deviceName, qreal volume, int duration);
    void    startRecord(QString deviceName, int duration);
    int     searchDevice(QString str, QList<QAudioDeviceInfo> &list);
    void    receiveInfo();
    bool    openSerialPort();
    void    switchBT();
    void    switchUSB();



protected:
    bool eventFilter(QObject *obj, QEvent *e);

private slots:
    void on_startButton_clicked();

    void on_stopButton_clicked();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
