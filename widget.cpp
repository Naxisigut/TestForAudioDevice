#include "widget.h"
#include "ui_widget.h"

enum Mode_ENUM {AR720_SwitchToBT =1,
                AR720_SwitchToUSB,
               }Mode;
enum Channel_ENUM{N76E885_SwitchToBT=1,
                  N76E885_SwitchToUSB,}Channel;
enum SerialPort_ENUM{AR720_SerialPort =1,
                     USBTo485_Converter_SerialPort}SerialPort_Index;

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->processLabel->setStyleSheet("QLabel{background:#FFFFFF;}");
    QFont font ( "Microsoft YaHei", 14, 75); //第一个属性是字体（微软雅黑），第二个是大小，第三个是加粗（权重是75）
    ui->isPassLabel->setStyleSheet("QLabel{background:#FFFFFF;}");
    ui->isPassLabel->setFont(font);
    ui->isPassLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->isFoundLabel->setStyleSheet("QLabel{background:#FF0000;}");
    ui->isFoundLabel->setText("没有发现设备");

//    QString SettingPath = qApp->applicationDirPath().append("/testItemSetting.ini");
//    qDebug() << "ini Path:" <<SettingPath;
    QSettings testItemSetting(qApp->applicationDirPath().append("/testItemSetting.ini"), QSettings::IniFormat);
//    QSettings testItemSetting(SettingPath, QSettings::IniFormat);
    Para_1.name           = testItemSetting.value("Item1/name").toString();
    Para_1.testItemIndex  = testItemSetting.value("Item1/testItemIndex").toInt();
    Para_1.outputDevice   = testItemSetting.value("Item1/outputDevice").toString();
    Para_1.inputDevice    = testItemSetting.value("Item1/inputDevice").toString();
    Para_1.playVolume     = testItemSetting.value("Item1/playVolume").toDouble();
    Para_1.playDuration   = testItemSetting.value("Item1/playDuration").toInt();
    Para_1.recordDuration = testItemSetting.value("Item1/recordDuration").toInt();
    Para_1.THDThreshold   = testItemSetting.value("Item1/THDThreshold").toDouble();
    if(testItemSetting.value("Item1/modeCmd").toString().contains("BT"))
    {Para_1.modeCmd       = AR720_SwitchToBT;}else
    {Para_1.modeCmd       = AR720_SwitchToUSB;}
    if(testItemSetting.value("Item1/channelCmd").toString().contains("BT"))
    {Para_1.channelCmd    = N76E885_SwitchToBT;}else
    {Para_1.channelCmd    = N76E885_SwitchToUSB;}
    Para_1.isTestNeeded   = testItemSetting.value("Item1/isTestNeeded").toInt();
    Para_1.isBTWaitNeeded = testItemSetting.value("Item1/isBTWaitNeeded").toInt();

    Para_2.name           = testItemSetting.value("Item2/name").toString();
    Para_2.testItemIndex  = testItemSetting.value("Item2/testItemIndex").toInt();
    Para_2.outputDevice   = testItemSetting.value("Item2/outputDevice").toString();
    Para_2.inputDevice    = testItemSetting.value("Item2/inputDevice").toString();
    Para_2.playVolume     = testItemSetting.value("Item2/playVolume").toDouble();
    Para_2.playDuration   = testItemSetting.value("Item2/playDuration").toInt();
    Para_2.recordDuration = testItemSetting.value("Item2/recordDuration").toInt();
    Para_2.THDThreshold   = testItemSetting.value("Item2/THDThreshold").toDouble();
    if(testItemSetting.value("Item2/modeCmd").toString().contains("BT"))
    {Para_2.modeCmd       = AR720_SwitchToBT;}else
    {Para_2.modeCmd       = AR720_SwitchToUSB;}
    if(testItemSetting.value("Item2/channelCmd").toString().contains("BT"))
    {Para_2.channelCmd    = N76E885_SwitchToBT;}else
    {Para_2.channelCmd    = N76E885_SwitchToUSB;}
    Para_2.isTestNeeded   = testItemSetting.value("Item2/isTestNeeded").toInt();
    Para_2.isBTWaitNeeded = testItemSetting.value("Item2/isBTWaitNeeded").toInt();

    Para_3.name           = testItemSetting.value("Item3/name").toString();
    Para_3.testItemIndex  = testItemSetting.value("Item3/testItemIndex").toInt();
    Para_3.outputDevice   = testItemSetting.value("Item3/outputDevice").toString();
    Para_3.inputDevice    = testItemSetting.value("Item3/inputDevice").toString();
    Para_3.playVolume     = testItemSetting.value("Item3/playVolume").toDouble();
    Para_3.playDuration   = testItemSetting.value("Item3/playDuration").toInt();
    Para_3.recordDuration = testItemSetting.value("Item3/recordDuration").toInt();
    Para_3.THDThreshold   = testItemSetting.value("Item3/THDThreshold").toDouble();
    if(testItemSetting.value("Item3/modeCmd").toString().contains("BT"))
    {Para_3.modeCmd       = AR720_SwitchToBT;}else
    {Para_3.modeCmd       = AR720_SwitchToUSB;}
    if(testItemSetting.value("Item3/channelCmd").toString().contains("BT"))
    {Para_3.channelCmd    = N76E885_SwitchToBT;}else
    {Para_3.channelCmd    = N76E885_SwitchToUSB;}
    Para_3.isTestNeeded   = testItemSetting.value("Item3/isTestNeeded").toInt();
    Para_3.isBTWaitNeeded = testItemSetting.value("Item3/isBTWaitNeeded").toInt();

    testParaList.append(Para_1);
    testParaList.append(Para_2);
    testParaList.append(Para_3);

    tableModel = new QStandardItemModel;
    ui->tableView->setModel(tableModel);
    tableModel->setColumnCount(4);
    tableModel->setHeaderData(0,Qt::Horizontal, "序号");
    tableModel->setHeaderData(1,Qt::Horizontal, "测试项");
    tableModel->setHeaderData(2,Qt::Horizontal, "THD");
    tableModel->setHeaderData(3,Qt::Horizontal, "结果");
    for(int i=0; i<testParaList.count(); i++)
    {
        for(int j=0; j<4; j++)
        {
            tableModel->setItem(i,j,new QStandardItem);
            tableModel->item(i, j)->setTextAlignment(Qt::AlignHCenter);
        }
    }
    for(int i=0; i<testParaList.count(); i++)
    {
        tableModel->item(i, 0)->setText(QString::number(i+1));//第一列序号
    }
    for(int i=0; i<testParaList.count(); i++)
    {
        tableModel->item(i,1)->setTextAlignment(Qt::AlignLeft);//第二列左对齐
        tableModel->item(i,1)->setText(testParaList.at(i).name);//第二列测试项名称
    }
    ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignHCenter);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableView->verticalHeader()->setVisible(false);//设置垂直表头不可见
    ui->tableView->setColumnWidth(0,30);
    ui->tableView->setColumnWidth(1,300);
    ui->tableView->setColumnWidth(2,50);
    ui->tableView->setColumnWidth(3,53);
    for(int i=0; i<testParaList.count(); i++)
    {
    ui->tableView->setRowHeight(i,20);//设置行高
    }

    deviceDetecTimer = new QTimer;
    deviceDetecTimer->start(2000);
    connect(deviceDetecTimer, &QTimer::timeout, this, &Widget::deviceDectFunc);
    connect(this, &Widget::deviceIsFound, this, &Widget::whenDeviceIsFound);

    // Set up the playTestAudio format
    format.setSampleRate(44100);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    recordedAudioPath = qApp->applicationDirPath().append("/temp");
    audioRecorder     = new QAudioRecorder;

    din       = (fftw_complex*)fftw_malloc(sizeof (fftw_complex)* N);
    out       = (fftw_complex*)fftw_malloc(sizeof (fftw_complex)* N);
    fftwPlan  = fftw_plan_dft_1d(N, din, out, FFTW_FORWARD, FFTW_ESTIMATE);
    powOut    = (double*)malloc(sizeof(double)*N);
    fOut      = (double*)malloc(sizeof(double)*N);

    portAR720     = NULL;
    portConverter = NULL;

}

Widget::~Widget()
{
    delete ui;
    if (portAR720 != NULL)
    {portAR720->close();}
    delete portAR720;
    if (portConverter != NULL)
    {portConverter->close();}
    delete portConverter;
    delete audioRecorder;
}

void Widget::deviceDectFunc()
{
    foreach(const QAudioDeviceInfo &deviceInfo, QAudioDeviceInfo::availableDevices(QAudio::AudioOutput))
    {
        outputDeviceList.append(deviceInfo);
        if(deviceInfo.deviceName().contains("JVC") == true)
           emit deviceIsFound();
    }
    foreach(const QAudioDeviceInfo &deviceInfo, QAudioDeviceInfo::availableDevices(QAudio::AudioInput))
    {
        inputDeviceList.append(deviceInfo);
    }
    foreach(const QSerialPortInfo &portInfo, QSerialPortInfo::availablePorts())
    {
        portInfoList.append(portInfo);
    }

}

void Widget::whenDeviceIsFound()
{
    ui->isFoundLabel->setStyleSheet("QLabel{background:#FFFF00;}");
    ui->isFoundLabel->setText("设备已连接");
}

void Widget::delay_MSec(unsigned int msec)
{
    QEventLoop loop;//定义一个新的事件循环
    QTimer::singleShot(msec, &loop, SLOT(quit()));//创建单次定时器，槽函数为事件循环的退出函数
    loop.exec();//事件循环开始执行，程序会卡在这里，直到定时时间到，本循环被退出
}

bool Widget::eventFilter(QObject *obj, QEvent *e)
{
    return QWidget::eventFilter(obj, e);
}


/**************************************************************************************/
/************************************串口与模式切换**************************************/
/**************************************************************************************/

bool Widget::openSerialPort(int SerialPortIndex)
{
    bool isFound = false;

    switch (SerialPortIndex) {
    case AR720_SerialPort:
        foreach(const QSerialPortInfo &portInfo, portInfoList)
        {
            if(portInfo.vendorIdentifier() == 0x2207 && portInfo.productIdentifier() == 0x0111)
            {
                portAR720 = new QSerialPort(portInfo);
                connect(portAR720,&QIODevice::readyRead,this, &Widget::receiveAR720Info);
                portAR720->setBaudRate(QSerialPort::Baud115200,QSerialPort::AllDirections);//设置波特率和读写方向
                portAR720->setDataBits(QSerialPort::Data8);		//数据位为8位
                portAR720->setFlowControl(QSerialPort::NoFlowControl);//无流控制
                portAR720->setParity(QSerialPort::NoParity);	//无校验位
                portAR720->setStopBits(QSerialPort::OneStop); //一位停止位
//                qDebug() << portAR720->portName();
                if(portAR720->isOpen() == true){portAR720->close();}
                isFound = true;
            }
        }
        break;
    case USBTo485_Converter_SerialPort:
        foreach(const QSerialPortInfo &portInfo, portInfoList)
        {
            if(portInfo.vendorIdentifier() == 0x0403 && portInfo.productIdentifier() == 0x6001)
            {
                portConverter = new QSerialPort(portInfo);
                connect(portConverter,&QIODevice::readyRead,this, &Widget::receiveConverterInfo);
                portConverter->setBaudRate(QSerialPort::Baud9600,QSerialPort::AllDirections);//设置波特率和读写方向
                portConverter->setDataBits(QSerialPort::Data8);		//数据位为8位
                portConverter->setFlowControl(QSerialPort::NoFlowControl);//无流控制
                portConverter->setParity(QSerialPort::NoParity);	//无校验位
                portConverter->setStopBits(QSerialPort::OneStop); //一位停止位
//                qDebug() << portConverter->portName();
                if(portConverter->isOpen() == true){portConverter->close();}
                isFound = true;
            }
        }
        break;
    default:
        break;
    }

    if(isFound == false)
    {
        switch (SerialPortIndex)
        {
        case AR720_SerialPort:
            QMessageBox::information(this, "提示", "没有发现AR720串口",QMessageBox::Ok);
            return 0;
        case USBTo485_Converter_SerialPort:
            QMessageBox::information(this, "提示", "没有发现Converter串口",QMessageBox::Ok);
            return 0;
        }

    }

    switch (SerialPortIndex) {
    case AR720_SerialPort:
        if(portAR720->open(QIODevice::ReadWrite) == true)
        {return 1;}
        else
        {
            QMessageBox::information(this, "提示", "打开720串口失败",QMessageBox::Ok);
            return 0;
        }
        break;
    case USBTo485_Converter_SerialPort:
        if(portConverter->open(QIODevice::ReadWrite) == true)
        {return 1;}
        else
        {
            QMessageBox::information(this, "提示", "打开Converter串口失败",QMessageBox::Ok);
            return 0;
        }
        break;
    default:
        break;
    }
    return 1;
}

char Widget::convertCharToHex(char ch)
{
    if((ch >= '0') && (ch <= '9'))
        return ch-0x30;
    else if((ch >= 'A') && (ch <= 'F'))
        return ch-'A'+10;
    else if((ch >= 'a') && (ch <= 'f'))
        return ch-'a'+10;
    else return (-1);
}

void Widget::QStringToHex(const QString &str, QByteArray &bytedata)
{
    int hexdata,lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    bytedata.resize(len/2);
    char lstr,hstr;
    for(int i=0; i<len; )
    {
        //char lstr,
        hstr=str[i].toLatin1();
        if(hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        if(i >= len)
            break;
        lstr = str[i].toLatin1();
        hexdata = convertCharToHex(hstr);
        lowhexdata = convertCharToHex(lstr);
        if((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata*16+lowhexdata;
        i++;
        bytedata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    bytedata.resize(hexdatalen);
}

void Widget::receiveAR720Info()
{

    QByteArray info = portAR720->readAll();
//    qDebug() << "read:" <<info;
    ui->textEdit->append(info);

}

void Widget::receiveConverterInfo()
{
    QByteArray info = portConverter->readAll();
//    qDebug()<< "read:" <<info;
    ui->textEdit->append(QString(info.toHex()));
}

void Widget::switchMode(int Mode)
{
    if(portAR720->isOpen() != true)//ooooo考虑发现串口关闭后 应当停止测试
    {
        QMessageBox::information(this, "提示", "720串口关闭",QMessageBox::Ok);
        return;
    }

    switch (Mode) {
    case AR720_SwitchToBT:
        portAR720->write(BT_CMD);
        break;
    case AR720_SwitchToUSB:
        portAR720->write(USB_CMD);
        break;
    }
}

void Widget::switchChannel(int Channel)
{
    if(portConverter->isOpen() != true)//ooooo考虑发现串口关闭后 应当停止测试
    {
        QMessageBox::information(this, "提示", "converter串口关闭",QMessageBox::Ok);
        return;
    }
    QByteArray sendBuf;
    switch (Channel)
    {
    case N76E885_SwitchToBT:
        QStringToHex(ToBTChannel_CMD, sendBuf); //把QString转换为hex
        portConverter->write(sendBuf);
        break;
    case N76E885_SwitchToUSB:
        QStringToHex(ToUSBChannel_CMD, sendBuf); //把QString转换为hex
        portConverter->write(sendBuf);
        break;
    }
}



/**************************************************************************************/
/************************************音频播放与录制**************************************/
/**************************************************************************************/

int Widget::searchDevice(QString str, QList<QAudioDeviceInfo> &list)
{
    for(int i=0; i<list.size(); i++)
    {
        if(list.at(i).deviceName().contains(str) == true)
        {
            return i;
        }
    }
    return -1;//搜索不到设备时，返回-1
}

bool Widget::playTestSound(QString deviceName, qreal volume, int duration)
{
//    qDebug() << "playTestSoundBegin";
    PlayThreadPara playPara;
    int deviceIndex = searchDevice(deviceName, outputDeviceList);
    if (deviceIndex != -1){
        playPara.info = outputDeviceList.at(deviceIndex);
    }else {
        QMessageBox::information(this, "提示", "没有发现播放设备",QMessageBox::Ok);
        return 0;
    }
    playPara.volume = volume;
    playPara.duration = duration;
    playThread = new playTestAudioThread(playPara, this);
    playThread->start();
//    qDebug() << "playTestSoundOver";
    return 1;
}

bool Widget::startRecord(QString deviceName, int duration, int testItemIndex)
{
//    qDebug() << "startRecordBegin";
    /****************功能：新建/清空tempFile******************/
    //若temp.wav已存在，录制前清空其之前的数据
    //若不存在，会在程序所在目录下自动新建一个temp.wav
    QString tempRecordedAudioPath = recordedAudioPath;
    tempRecordedAudioPath.append(QString::number(testItemIndex)).append(".wav");
//    qDebug()<< "audioRecorderPath:" <<tempRecordedAudioPath;
    QFile tempFile(tempRecordedAudioPath);

    tempFile.open(QIODevice::ReadWrite |QIODevice::Truncate);
    tempFile.close();

    /****************功能：开始录音，写入tempFile**************/
    int deviceIndex = searchDevice(deviceName, inputDeviceList);
    if (deviceIndex != -1)
    {
        QAudioDeviceInfo info = inputDeviceList.at(deviceIndex);
    }else
    {
        QMessageBox::information(this, "提示", "没有发现录音设备",QMessageBox::Ok);
        return 0;
    }
    audioRecorder->setAudioInput(inputDeviceList.at(deviceIndex).deviceName());
    audioRecorder->setOutputLocation(QUrl::fromLocalFile(tempRecordedAudioPath));
    audioRecorder->record();
    delay_MSec(duration);//延时，单位为ms, 限定录制时间
    audioRecorder->stop();
    return 1;
}


/**************************************************************************************/
/***********************************测试数据分析与输出************************************/
/**************************************************************************************/

double Widget::FindMaxInArray(double arr[],int cnt)
{
    double max = arr[0];
    for (int i=1;i<cnt;i++)
    {
        max = (max<arr[i]?arr[i]:max);
    }
    return max;
}

void Widget::getPowArrayAtFrequency(double f, double arr[], int cnt, double sourcePowArr[])
{
    //此函数获取频率f处前后cnt个频点的功率

    double accurateIndex = f/((double)11025/N);//根据f计算出对应的精确频点
//    qDebug()<<accurateIndex;
    //根据精确频点找到距离最近的整数频点（四舍五入）
    int middleIndex = (int)(accurateIndex*10)%10 < 5?(int)accurateIndex:(int)accurateIndex+1;

    //返回f处的功率数组
    for(int i=0; i<cnt; i++)
    {
        arr[i] = sourcePowArr[middleIndex-cnt/2+i];
    }
    //    qDebug()<<middleIndex;
}

double Widget::THDCalculate(double f, double sourcePowArr[])
{
    double tempPowArrayAtFrequency[5];
    //fundamentalWavePow为基波功率，sumOfHarmonicWavePow为谐波总功率
    double fundamentalWavePow=0, sumOfHarmonicWavePow=0;
    for(int i=1; i <= 11025/(2*(int)f); i++)//11025折半为最大的计算频率
    {
        getPowArrayAtFrequency(i*f, tempPowArrayAtFrequency, 5, sourcePowArr);
        if(i==1){
            fundamentalWavePow = FindMaxInArray(tempPowArrayAtFrequency, 5);
        }else{
            sumOfHarmonicWavePow += FindMaxInArray(tempPowArrayAtFrequency, 5);
        }
    }
    //用FindMaxInArray以避免不够精确导致的频点错误。在中心频点前后共5个频点中的最大值，是计算时用到的真正的数据。

    double THD = qSqrt(sumOfHarmonicWavePow/fundamentalWavePow);
    return THD;
}


/**************************************************************************************/
/***************************************测试模块****************************************/
/**************************************************************************************/

int Widget::TestFunc(TestModulePara para)
{
    if(para.isTestNeeded == false)return 1;
    /****************Step1：开启串口，发送指令**************/
    if(openSerialPort(AR720_SerialPort)==false)
        return -1;
    switchMode(para.modeCmd);
    if(openSerialPort(USBTo485_Converter_SerialPort)==false)
        return -1;
    switchChannel(para.channelCmd);

    /****************Step2：播放音频，开始录音**************/
//    qDebug()<<"播放音频";
    if(para.isBTWaitNeeded == 1)
    {
         if(QMessageBox::information(this, "提示", "蓝牙是否连接成功？",QMessageBox::Yes|QMessageBox::No) == 0x00010000)
         {
             portAR720->close();
             portConverter->close();
             return -1;
         };
         delay_MSec(2000);
    }
    bool playOK = playTestSound(para.outputDevice, para.playVolume, para.playDuration);
    if(playOK == false)
    {
        QMessageBox::information(this, "提示", "录音失败",QMessageBox::Ok);
        portAR720->close();
        portConverter->close();
        return -1;
    }
    bool recordOK = startRecord(para.inputDevice, para.recordDuration, para.testItemIndex);
    if(recordOK == false)
    {
        QMessageBox::information(this, "提示", "录音失败",QMessageBox::Ok);
        portAR720->close();
        portConverter->close();
        return -1;
    }
    /*********************Step3：分析文件*******************/
//    qDebug()<<"分析文件";
//    qDebug()<<"Get Path";
    QString tempRecordedAudioPath = recordedAudioPath;
    tempRecordedAudioPath.append(QString::number(para.testItemIndex)).append(".wav");
//    qDebug()<<"AudioFilePath:"<<tempRecordedAudioPath;
    AudioFile<double> sourceFile;

    sourceFile.load(tempRecordedAudioPath.toLocal8Bit().constData());
//    qDebug()<<sourceFile.getBitDepth();
//    qDebug()<<sourceFile.getSampleRate();
//    qDebug()<<sourceFile.getLengthInSeconds();
//    qDebug()<<sourceFile.getNumSamplesPerChannel();
//    qDebug()<<sourceFile.getNumChannels();
//    qDebug()<<sourceFile.isMono();
//    qDebug()<<sourceFile.isStereo();

//    qDebug()<<sourceFile.samples.size();
//    qDebug()<<sourceFile.samples.capacity();
//    qDebug()<<sizeof (sourceFile.samples);
//    qDebug()<<sizeof (sourceFile.samples[0][0]);


    if ((din == NULL)||(out == NULL)){
        QMessageBox::information(this, "提示", "FFT内存错误",QMessageBox::Ok);
        portAR720->close();
        portConverter->close();
        return -1;
    }
    else
    {
        for (int i=0; i<N; i++)
        {
            din[i][0] = sourceFile.samples[0][i];
            din[i][1] = 0;
        }
    }
//    qDebug()<<"execute FFT";
    fftw_execute(fftwPlan);

    /****************Step4：计算功率**************/
//    qDebug()<<"Calculate Pow";
    for(int i=0; i<N; i++)
    {
        powOut[i] = qPow(out[i][0],2)+qPow(out[i][1],2);//计算功率
    }

    /****************Step5：计算THD并判断**************/
//    qDebug()<<"Calculate THD";
    double THD = THDCalculate(1000, powOut);
    qDebug() << "THD" << THD;
    qDebug() << "THDThreshold" << para.THDThreshold;
    tableModel->item(para.testItemIndex-1,2)->setText(QString::number(THD*100,'f', 1).append("%"));
    int isPass;
    if(THD <=para.THDThreshold)
    {
        tableModel->item(para.testItemIndex-1,3)->setData(QColor(0,255,0),Qt::BackgroundRole);
        tableModel->item(para.testItemIndex-1,3)->setText("Pass");
        isPass =1;
    }else
    {
        tableModel->item(para.testItemIndex-1,3)->setData(QColor(255,0,0),Qt::BackgroundRole);
        tableModel->item(para.testItemIndex-1,3)->setText("Fail");
        isPass =0;
    }

    /****************Step6：关闭串口**************/

    portAR720->close();
    portConverter->close();

    return isPass;
}

/**************************************************************************************/
/*****************************************按键******************************************/
/**************************************************************************************/

void Widget::on_startButton_clicked()
{
    //界面初始化
    ui->isPassLabel->setStyleSheet("QLabel{background:#FFFFFF;}");
    ui->isPassLabel->setText("");
    for (int i=0; i<testParaList.count(); i++)
    {
        for (int j=2; j<4; j++)
        {
            tableModel->item(i,j)->setText("");
            tableModel->item(i,j)->setData(QColor(255,255,255),Qt::BackgroundRole);
        }
    }

    deviceDetecTimer->stop();//测试时禁用设备检测
    ui->startButton->setEnabled(false);//测试时禁用开始按钮

    //开始测试
    int passFlag = 0;
    foreach(TestModulePara Para, testParaList)
    {
        QString processDisplay = "第";
        processDisplay.append(QString::number(Para.testItemIndex)).append("项正在测试中……");
        ui->processLabel->setText(processDisplay);
        switch (TestFunc(Para))
        {
        case -1:
            ui->processLabel->setText("测试中断");
            ui->isPassLabel->setStyleSheet("QLabel{background:#FF0000;}");
            ui->isPassLabel->setText("Fail");
            ui->startButton->setEnabled(true);
            return;
        case 0:
            break;
        case 1:
            passFlag++;
            break;
        }
    }
    if(passFlag == testParaList.count())
    {
        ui->isPassLabel->setStyleSheet("QLabel{background:#00FF00;}");
        ui->isPassLabel->setText("Pass");
    }else
    {
        ui->isPassLabel->setStyleSheet("QLabel{background:#FF0000;}");
        ui->isPassLabel->setText("Fail");
    }
    ui->processLabel->setText("测试完成");
    ui->startButton->setEnabled(true);
    deviceDetecTimer->start(2000);//测试完成后重启设备检测

//    if(testParaList.last().isBTWaitNeeded ==1)
//    {
//        openSerialPort(AR720_SerialPort);
//        switchMode(AR720_SwitchToUSB);
//        openSerialPort(USBTo485_Converter_SerialPort);
//        switchChannel(N76E885_SwitchToUSB);
//        portAR720->close();
//        portConverter->close();
//        qDebug()<<"关闭蓝牙已发送";
//    }



}





//void Widget::on_pushButton_clicked()
//{
//    tempPara.outputDevice = "C-Media";
//    tempPara.inputDevice = "C-Media";
//    tempPara.playVolume = 1;

//    deviceDetecTimer->stop();
//    ui->startButton->setEnabled(false);
//    ui->isPassLabel->setStyleSheet("QLabel{background:#FFFFFF;}");
//    ui->isPassLabel->setText("");
//}
