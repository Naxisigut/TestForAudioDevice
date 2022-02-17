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

    din = (fftw_complex*)fftw_malloc(sizeof (fftw_complex)* N);
    out = (fftw_complex*)fftw_malloc(sizeof (fftw_complex)* N);
    fftwPlan = fftw_plan_dft_1d(N, din, out, FFTW_FORWARD, FFTW_ESTIMATE);
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
//    QAudioEncoderSettings audioSettings;
//    audioSettings.setSampleRate(48000);
//    audioSettings.setChannelCount(2);

    audioRecorder->setAudioInput(ui->audioListCombox->currentText());
    audioRecorder->setOutputLocation(QUrl::fromLocalFile(tempPath));
//    audioRecorder->setAudioSettings(audioSettings);
    audioRecorder->record();
    QTest::qWait(10000);//延时10s，限定录制时间
    qDebug() << audioRecorder->duration();
    audioRecorder->stop();
    qDebug()<<"录音结束";

    /****************功能：分析tempFile**************/
    sourceFile.load(tempPath.toStdString());
    qDebug() << "采样率：" << sourceFile.getSampleRate() << endl;
    qDebug() << "位深度/采样深度："<< sourceFile.getBitDepth() << endl;
    qDebug() << "声道数：" << sourceFile.getNumChannels() << endl;
    qDebug() << "单声道采样总数：" << sourceFile.getNumSamplesPerChannel() << endl;
    qDebug() << "音频长度：" << sourceFile.getLengthInSeconds() << "s" << endl;
    if ((din == NULL)||(out == NULL)){
        qDebug() << "Error:insufficient available memory" << endl;
    }
    else{
        for (int i=0; i<N; i++)
        {
            din[i][0] = sourceFile.samples[0][i];//将samples的channel0的数据赋给din
            din[i][1] = 0;
        }
    }
    fftw_execute(fftwPlan);

//    QFile outputFileDin("../TestForAR720/outputDin.txt");
//    outputFileDin.open(QIODevice::WriteOnly |QIODevice::Truncate);
//    QTextStream outDinStream(&outputFileDin);
//    outDinStream.setCodec("UTF-8");
//    outDinStream << "Amplitude" << endl;
//    for(int i=0; i<N; i++)
//    {
//        outDinStream << i << " " << din[i][0] << endl;
//    }

    outputPath = qApp->applicationDirPath();
    outputPath += "/output.txt";
    qDebug() << outputPath;
    QFile outputFile(outputPath);
    outputFile.open(QIODevice::WriteOnly |QIODevice::Truncate);
    QTextStream outStream(&outputFile);
    outStream.setCodec("UTF-8");
    outStream << "Amplitude" << endl;
    for(int i=1; i<N/2; i++)
    {
        outStream << i << " " << qSqrt(qPow(out[i][0],2)+qPow(out[i][1],2)) << endl;
    }

}

void Widget::on_stopButton_clicked()
{
    audioRecorder->stop();
}
