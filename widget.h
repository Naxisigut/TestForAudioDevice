#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QAudioRecorder>
#include <QTimer>
#include <QtDebug>
#include <QAudioDeviceInfo>
#include <QAudio>
#include <QFile>
#include <QMessageBox>
#include <QUrl>
#include <QTest>
#include <QtMath>
#include <fftw3.h>
#include <AudioFile.h>

#define N 131072
//#define N 8192

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    QAudioRecorder *audioRecorder;
    QString tempPath;
    QString outputPath;
    AudioFile<double> sourceFile;
    fftw_complex *din,*out;
    fftw_plan fftwPlan;

protected:
    bool eventFilter(QObject *obj, QEvent *e);

private slots:
    void on_startButton_clicked();

    void on_stopButton_clicked();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
