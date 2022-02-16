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

protected:
    bool eventFilter(QObject *obj, QEvent *e);

private slots:
    void on_startButton_clicked();

    void on_stopButton_clicked();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
