#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    audioRecorder = new QAudioRecorder;
    ui->audioListCombox->addItems(audioRecorder->audioInputs());
    ui->audioListCombox->installEventFilter(this);
}

Widget::~Widget()
{
    delete ui;
}

bool Widget::eventFilter(QObject *obj, QEvent *e)
{
    /**********功能：点击audioListCombox时更新设备列表**********/
    if((obj == ui->audioListCombox) && (e->type() == QEvent::MouseButtonPress))
    {
        ui->audioListCombox->clear();
        QList<QAudioDeviceInfo> list =QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
        foreach(QAudioDeviceInfo info, list)
        {
            ui->audioListCombox->addItem(info.deviceName());
        }
//        qDebug() << "press";
        return QWidget::eventFilter(obj, e);//执行完上面的代码后，继续执行原有的功能
    }else{
        return QWidget::eventFilter(obj, e);
    }
}

void Widget::on_startButton_clicked()
{
    /****************功能：新建/清空tempFile******************/
    //在程序运行目录下新建temp文件
    tempPath = qApp->applicationDirPath();//获取程序所在路径
    tempPath += "/temp.wav";//创建temp文件的路径

    //若temp.wav已存在，录制前清空其之前的数据
    //若不存在，会在程序所在目录下自动新建一个temp.wav
    QFile tempFile(tempPath);
    bool QFileOpenOK = tempFile.open(QIODevice::ReadWrite |QIODevice::Truncate);
    if(QFileOpenOK != true)//如果tempFile打开失败，弹窗提示
    {
    qDebug()<< "QFileOpenFail";
    QMessageBox::information(this, "提示", "临时文件打开失败",QMessageBox::Ok);
    return;
    }
    tempFile.close();

    /****************功能：开始录音，写入tempFile**************/
    audioRecorder->setAudioInput(ui->audioListCombox->currentText());
    audioRecorder->setOutputLocation(QUrl::fromLocalFile(tempPath));
    audioRecorder->record();
//    qDebug() << audioRecorder->status();
//    qDebug() << audioRecorder->state();
//    qDebug() << audioRecorder->error();
}

void Widget::on_stopButton_clicked()
{
    audioRecorder->stop();
}
