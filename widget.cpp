#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    audioRecorder = new QAudioRecorder;
    ui->audioListCombox->addItems(audioRecorder->audioInputs());

}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_startButton_clicked()
{

//    qDebug() <<audioInputsList;
//qDebug() << audioRecorder->audioInputs();
}

void Widget::on_stopButton_clicked()
{

}

void Widget::on_audioListCombox_highlighted(const QString &arg1)
{
    audioInputsList.clear();
    QList<QAudioDeviceInfo> list =QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    foreach(QAudioDeviceInfo info, list)
    {
        audioInputsList << info.deviceName();
    }
    ui->audioListCombox->clear();
    ui->audioListCombox->addItems(audioInputsList);
}
