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
#include <QSettings>
#include <fftw3.h>
#include <AudioFile.h>
#include <playTestAudioThread.h>
#include <testmodulepara.h>

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
    QStandardItemModel *tableModel;

    /**************************************************************************************/
    /************************************串口与模式切换**************************************/
    /**************************************************************************************/
    QSerialPort             *portAR720;
    QSerialPort             *portConverter;
    QList<QSerialPortInfo>  portInfoList;
    bool                    openSerialPort(int SerialPortIndex);
    char                    convertCharToHex(char ch);
    void                    QStringToHex(const QString &str, QByteArray &bytedata);
    void                    receiveAR720Info();
    void                    receiveConverterInfo();
    void                    switchMode(int Mode);
    void                    switchChannel(int Channel);
//    QByteArray QstringToHex(const QString);

    /**************************************************************************************/
    /************************************音频播放与录制**************************************/
    /**************************************************************************************/
    QString                  recordedAudioPath;
    QAudioRecorder           *audioRecorder;
    QAudioFormat             format;
    QList<QAudioDeviceInfo>  outputDeviceList;
    QList<QAudioDeviceInfo>  inputDeviceList;
    playTestAudioThread      *playThread;
    int                      searchDevice(QString str, QList<QAudioDeviceInfo> &list);
    void                     playTestSound(QString deviceName, qreal volume, int duration);
    bool                     startRecord(QString deviceName, int duration, int testItemIndex);

    /**************************************************************************************/
    /***********************************测试数据分析与输出************************************/
    /**************************************************************************************/
    fftw_complex             *din,*out;
    fftw_plan                fftwPlan;
    double                   *powOut,*fOut;
    double                   FindMaxInArray(double arr[],int cnt);
    void                     getPowArrayAtFrequency(double f, double arr[], int cnt, double sourceArr[]);
    double                   THDCalculate(double f, double sourcePow[]);

    /**************************************************************************************/
    /***************************************测试模块****************************************/
    /**************************************************************************************/
    TestModulePara  Para_1,Para_2,Para_3,Para_4;
    void            TestFunc(TestModulePara para);

protected:
    bool eventFilter(QObject *obj, QEvent *e);

private:
    QByteArray BT_CMD = "cmd_switch_to_bt\n";
    QByteArray USB_CMD = "cmd_switch_to_usb\n";
    QByteArray Master_CMD = "close_master_mic\n";
    QByteArray Deputy_CMD = "close_deputy_mic\n";
    QString ToBTChannel_CMD = "AA 01 01 01 02 01 00 00";
    QString BTReply_CMD = "AA 01 01 01 02 01 B5 4D";
    QString ToUSBChannel_CMD = "AA 01 01 01 02 02 00 00";
    QString USBReply_CMD = "AA 01 01 01 02 02 F5 4C";

private slots:
    void on_startButton_clicked();

    void on_stopButton_clicked();

    void on_closeButton_clicked();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
