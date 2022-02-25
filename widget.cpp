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

    playTestAudioThread play;

    foreach(const QAudioDeviceInfo &deviceInfo, QAudioDeviceInfo::availableDevices(QAudio::AudioOutput))
    {
        outputDeviceList.append(deviceInfo);
        ui->outputListCombox->addItem(deviceInfo.deviceName());
    }

    // Set up the format, eg.
    format.setSampleRate(44100);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    recordedAudioPath = qApp->applicationDirPath().append("/temp.wav");

    din = (fftw_complex*)fftw_malloc(sizeof (fftw_complex)* N);
    out = (fftw_complex*)fftw_malloc(sizeof (fftw_complex)* N);
    fftwPlan = fftw_plan_dft_1d(N, din, out, FFTW_FORWARD, FFTW_ESTIMATE);
    powOut = (double*)malloc(sizeof(double)*N);
    fOut = (double*)malloc(sizeof(double)*N);
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

double Widget::FindMaxInArray(double arr[],int cnt)
{
    double max = arr[0];
    for (int i=1;i<cnt;i++)
    {
        max = (max<arr[i]?arr[i]:max);
//        qDebug()<<i<<max;
    };
    return max;
}

void Widget::getPowArrayAtFrequency(double f, double arr[], int cnt, double sourcePowArr[])
{
    double accurateIndex = f/((double)11025/N);
//    qDebug()<<accurateIndex;
    int middleIndex = (int)(accurateIndex*10)%10 < 5?(int)accurateIndex:(int)accurateIndex+1;//根据第一位小数四舍五入
    for(int i=0; i<cnt; i++)
    {
        arr[i] = sourcePowArr[middleIndex-cnt/2+i];
    }
    //    qDebug()<<middleIndex;
}

double Widget::THDCalculate(double f, double sourcePowArr[])
{
    double tempPowArrayAtFrequency[5];
    double fundamentalWavePow, sumOfHarmonicWavePow=0;
    for(int i=1; i <= 11025/(2*(int)f); i++)//11025折半为最大的计算频率
    {
        getPowArrayAtFrequency(i*f, tempPowArrayAtFrequency, 5, sourcePowArr);
        if(i==1){
            fundamentalWavePow = FindMaxInArray(tempPowArrayAtFrequency, 5);
        }else{
            sumOfHarmonicWavePow += FindMaxInArray(tempPowArrayAtFrequency, 5);
        }
    }
    double THD = qSqrt(sumOfHarmonicWavePow/fundamentalWavePow);
    return THD;
}

void Widget::TestFuncBase()
{
    /****************功能：新建/清空tempFile******************/
    //若temp.wav已存在，录制前清空其之前的数据
    //若不存在，会在程序所在目录下自动新建一个temp.wav
    QFile tempFile(recordedAudioPath);
    bool QFileOpenOK = tempFile.open(QIODevice::ReadWrite |QIODevice::Truncate);
    if(QFileOpenOK != true)//如果tempFile打开失败，弹窗提示
    {
    qDebug()<< "QFileOpenFail";
    QMessageBox::information(this, "提示", "临时文件打开失败",QMessageBox::Ok);
    return;
    }
    tempFile.close();

    /****************功能：播放测试音频**************/


    /****************功能：开始录音，写入tempFile**************/
//    QAudioEncoderSettings audioSettings;
//    audioSettings.setSampleRate(48000);
//    audioSettings.setChannelCount(2);

    audioRecorder->setAudioInput(ui->audioListCombox->currentText());
    audioRecorder->setOutputLocation(QUrl::fromLocalFile(recordedAudioPath));
//    audioRecorder->setAudioSettings(audioSettings);
    audioRecorder->record();
    QTest::qWait(10000);//延时10s，限定录制时间
    qDebug() << audioRecorder->duration();
    audioRecorder->stop();
    qDebug()<<"录音结束";

    /****************功能：分析tempFile**************/
    AudioFile<double> sourceFile;
    sourceFile.load(recordedAudioPath.toStdString());
//    qDebug() << "采样率：" << sourceFile.getSampleRate() << endl;
//    qDebug() << "位深度/采样深度："<< sourceFile.getBitDepth() << endl;
//    qDebug() << "声道数：" << sourceFile.getNumChannels() << endl;
//    qDebug() << "单声道采样总数：" << sourceFile.getNumSamplesPerChannel() << endl;
//    qDebug() << "音频长度：" << sourceFile.getLengthInSeconds() << "s" << endl;
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

    /****************功能：输出源音频数据至文件**************/

    QString outputPath = qApp->applicationDirPath().append("/AmpOutput.txt");
    QFile outputFile(outputPath);
    outputFile.open(QIODevice::WriteOnly |QIODevice::Truncate);
    QTextStream outStream(&outputFile);
    outStream.setCodec("UTF-8");
    outStream << "Amplitude" << endl;
    for(int i=0; i<N; i++)
    {
        fOut[i] = (double)i*11025/N;//计算频率
        powOut[i] = qPow(out[i][0],2)+qPow(out[i][1],2);//计算功率
        outStream << fOut[i] << "Hz " << powOut[i] << endl;
    }

    /****************功能：计算THD**************/
    double THD = THDCalculate(1000, powOut);
    qDebug() << "THD" << THD;

//    double tempPowArrayAtFrequency[5];
//    double fundamentalWavePow, sumOfHarmonicWavePow=0;
//    for(int i=1;i<=11025/2000;i++)//11025折半为最大的计算频率
//    {
//        getPowArrayAtFrequency(i*1000, tempPowArrayAtFrequency, 5, powOut);
//        if(i==1){
//            fundamentalWavePow = FindMaxInArray(tempPowArrayAtFrequency, 5);
//        }else{
//            sumOfHarmonicWavePow += FindMaxInArray(tempPowArrayAtFrequency, 5);
//        }
//    }
//    double THDAt1KHz = qSqrt(sumOfHarmonicWavePow/fundamentalWavePow);
//    qDebug() << "THDFun1"<< THDAt1KHz;
}

void Widget::TestFunc1st()
{
    /****************功能：新建/清空tempFile******************/
    //若temp.wav已存在，录制前清空其之前的数据
    //若不存在，会在程序所在目录下自动新建一个temp.wav
    QFile tempFile(recordedAudioPath);
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
    audioRecorder->setOutputLocation(QUrl::fromLocalFile(recordedAudioPath));
    audioRecorder->record();
    QTest::qWait(10000);//延时10s，限定录制时间
    audioRecorder->stop();
    qDebug()<<"录音结束";

    /****************功能：分析tempFile**************/
    AudioFile<double> sourceFile;
    sourceFile.load(recordedAudioPath.toStdString());
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

    /****************功能：输出源音频数据至文件**************/
    QString outputPath = qApp->applicationDirPath().append("/AmpOutput.txt");
    QFile outputFile(outputPath);
    outputFile.open(QIODevice::WriteOnly |QIODevice::Truncate);
    QTextStream outStream(&outputFile);
    outStream.setCodec("UTF-8");
    outStream << "Amplitude" << endl;
    for(int i=0; i<N; i++)
    {
        fOut[i] = (double)i*11025/N;//计算频率
        powOut[i] = qPow(out[i][0],2)+qPow(out[i][1],2);//计算功率
        outStream << fOut[i] << "Hz " << powOut[i] << endl;
    }

    /****************功能：计算THD**************/
    double THD = THDCalculate(1000, powOut);
    qDebug() << "THD" << THD;
}

void Widget::TestFunc2nd()
{
//   audioPlayer->setMedia(QUrl::fromLocalFile(TestAudio));
//   audioPlayer->setVolume(30);
//   audioPlayer->play();

    playTestSound(1, 5000);
}

void Widget::playTestSound(qreal volume, int duration)
{
    QFile testAudioFile(testAudioPath);
    testAudioFile.open(QIODevice::ReadOnly);

    QAudioDeviceInfo info = outputDeviceList.at(ui->outputListCombox->currentIndex());
//    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
//    if (info.isFormatSupported(format) != true){
//        qDebug() << "Raw audio format not supported by backend, cannot play audio.";
//    }
//    else{
    QAudioOutput audioOutput(info, format, this);
    audioOutput.start(&testAudioFile);
    audioOutput.setVolume(volume);
//    qDebug()<< output.state();
//    qDebug() << output.error();
//    QTest::qWait(duration);
    QEventLoop *loop = new QEventLoop;
    connect(&testSoundDuration, &QTimer::timeout,
            [=]()
            {
               loop->exit();
//               qDebug() <<"exit";
               testSoundDuration.stop();
            }
            );
    testSoundDuration.start(duration);
    loop->exec();
    audioOutput.stop();
//    qDebug()<<"stop";
    testAudioFile.close();
}

void Widget::startRecord()
{
    /****************功能：新建/清空tempFile******************/
    //若temp.wav已存在，录制前清空其之前的数据
    //若不存在，会在程序所在目录下自动新建一个temp.wav
    QFile tempFile(recordedAudioPath);
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
    audioRecorder->setOutputLocation(QUrl::fromLocalFile(recordedAudioPath));
    audioRecorder->record();
    QTest::qWait(10000);//延时10s，限定录制时间
    audioRecorder->stop();
//    qDebug()<<"录音结束";
}

void Widget::on_startButton_clicked()
{
    TestFunc1st();
}


void Widget::on_stopButton_clicked()
{
    TestFunc2nd();
}
