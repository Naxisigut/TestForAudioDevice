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
//    ui->inputListCombox->addItems(audioRecorder->audioInputs());
    ui->inputListCombox->installEventFilter(this);
    ui->outputListCombox->installEventFilter(this);

    tableModel = new QStandardItemModel;
    ui->tableView->setModel(tableModel);
    tableModel->setColumnCount(4);
    tableModel->setHeaderData(0,Qt::Horizontal, "序号");
    tableModel->setHeaderData(1,Qt::Horizontal, "测试项");
    tableModel->setHeaderData(2,Qt::Horizontal, "THD");
    tableModel->setHeaderData(3,Qt::Horizontal, "结果");
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            tableModel->setItem(i,j,new QStandardItem);
            tableModel->item(i, j)->setTextAlignment(Qt::AlignHCenter);
        }
    }
    for(int i=0; i<4; i++){
        tableModel->item(i, 0)->setText(QString::number(i+1));
    }
    for(int i=0; i<4; i++){
        tableModel->item(i,1)->setTextAlignment(Qt::AlignLeft);
    }
    tableModel->item(0, 1)->setText("声卡输出 ->720 USB Mic");
    tableModel->item(1, 1)->setText("720 USB Speaker ->声卡输入");
    tableModel->item(2, 1)->setText("声卡输出 ->蓝牙模块 ->720 Speaker ->声卡输入");
    ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignHCenter);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableView->setColumnWidth(0,30);
    ui->tableView->setColumnWidth(1,300);
    ui->tableView->setColumnWidth(2,50);
    ui->tableView->setColumnWidth(3,50);
    ui->tableView->setRowHeight(0,20);
    ui->tableView->setRowHeight(1,20);
    ui->tableView->setRowHeight(2,20);
    ui->tableView->setRowHeight(3,20);
    ui->tableView->verticalHeader()->setVisible(false);


    foreach(const QAudioDeviceInfo &deviceInfo, QAudioDeviceInfo::availableDevices(QAudio::AudioOutput))
    {
        outputDeviceList.append(deviceInfo);
        ui->outputListCombox->addItem(deviceInfo.deviceName());
    }
    foreach(const QAudioDeviceInfo &deviceInfo, QAudioDeviceInfo::availableDevices(QAudio::AudioInput))
    {
        inputDeviceList.append(deviceInfo);
        ui->inputListCombox->addItem(deviceInfo.deviceName());
    }
    foreach(const QSerialPortInfo &portInfo, QSerialPortInfo::availablePorts())
    {
        portInfoList.append(portInfo);
        ui->portListCombox->addItem(portInfo.portName());
    }

    // Set up the playTestAudio format, eg.
    format.setSampleRate(44100);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    recordedAudioPath = qApp->applicationDirPath().append("/temp.wav");

    audioRecorder = new QAudioRecorder;

    din = (fftw_complex*)fftw_malloc(sizeof (fftw_complex)* N);
    out = (fftw_complex*)fftw_malloc(sizeof (fftw_complex)* N);
    fftwPlan = fftw_plan_dft_1d(N, din, out, FFTW_FORWARD, FFTW_ESTIMATE);
    powOut = (double*)malloc(sizeof(double)*N);
    fOut = (double*)malloc(sizeof(double)*N);

    portAR720 = NULL;
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
}

bool Widget::eventFilter(QObject *obj, QEvent *e)
{
    /**********功能：点击audioListCombox时更新设备列表**********/
    if((obj == ui->inputListCombox) && (e->type() == QEvent::MouseButtonPress))
    {
        ui->inputListCombox->clear();
        QList<QAudioDeviceInfo> list =QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
        foreach(QAudioDeviceInfo info, list)
        {
            ui->inputListCombox->addItem(info.deviceName());
        }
//        qDebug() << "press";
        return QWidget::eventFilter(obj, e);//执行完上面的代码后，继续执行原有的功能
    }else if((obj == ui->outputListCombox) && (e->type() == QEvent::MouseButtonPress))
    {
        ui->outputListCombox->clear();
        QList<QAudioDeviceInfo> list =QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
        foreach(QAudioDeviceInfo info, list)
        {
            ui->outputListCombox->addItem(info.deviceName());
        }
//        qDebug() << "press";
        return QWidget::eventFilter(obj, e);//执行完上面的代码后，继续执行原有的功能
    }else{
        return QWidget::eventFilter(obj, e);
    }
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
                qDebug() << portAR720->portName();
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
                qDebug() << portConverter->portName();
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
        QMessageBox::information(this, "提示", "没有发现串口",QMessageBox::Ok);
        return 0;
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
            QMessageBox::information(this, "提示", "打开converter串口失败",QMessageBox::Ok);
            return 0;
        }
        break;
    default:
        break;
    }
    return 0;
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
    qDebug() << "read:" <<info;
    ui->textEdit->append(info);

}

void Widget::receiveConverterInfo()
{
    QByteArray info = portConverter->readAll();
    qDebug()<< "read:" <<info;
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
//            qDebug()<<"包含"<<i;
            return i;
        }
//        else {
//            qDebug()<<"不包含i"<<i;
//        }
    }
    return -1;//搜索不到设备时，返回-1
}

void Widget::playTestSound(QString deviceName, qreal volume, int duration)
{
    PlayThreadPara playPara;
    int deviceIndex = searchDevice(deviceName, outputDeviceList);
    if (deviceIndex != -1){
        playPara.info = outputDeviceList.at(deviceIndex);
    }else {
        QMessageBox::information(this, "提示", "没有发现播放设备",QMessageBox::Ok);
        return;
    }
    playPara.volume = volume;
    playPara.duration = duration;
    playThread = new playTestAudioThread(playPara, this);
    playThread->start();
}

void Widget::startRecord(QString deviceName, int duration, int testItemIndex)
{
    /****************功能：新建/清空tempFile******************/
    //若temp.wav已存在，录制前清空其之前的数据
    //若不存在，会在程序所在目录下自动新建一个temp.wav
    QFile tempFile(recordedAudioPath.append(testItemIndex));
    bool QFileOpenOK = tempFile.open(QIODevice::ReadWrite |QIODevice::Truncate);
    if(QFileOpenOK != true)//如果tempFile打开失败，弹窗提示
    {
    QMessageBox::information(this, "提示", "临时文件打开失败",QMessageBox::Ok);
    return;
    }
    tempFile.close();

    /****************功能：开始录音，写入tempFile**************/
    int deviceIndex = searchDevice(deviceName, inputDeviceList);
    if (deviceIndex != -1){
        QAudioDeviceInfo info = inputDeviceList.at(deviceIndex);
    }else {
        QMessageBox::information(this, "提示", "没有发现录音设备",QMessageBox::Ok);
        return;
    }
    audioRecorder->setAudioInput(inputDeviceList.at(deviceIndex).deviceName());
    audioRecorder->setOutputLocation(QUrl::fromLocalFile(recordedAudioPath));
    audioRecorder->record();
    QTest::qWait(duration);//延时，单位为ms, 限定录制时间
    audioRecorder->stop();
    //    qDebug()<<"录音结束";
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
    //用FindMaxInArray以避免不够精确导致的频点错误。在中心频点前后共5个频点中的最大值，是计算时用到的真正的数据。

    double THD = qSqrt(sumOfHarmonicWavePow/fundamentalWavePow);
    return THD;
}


/**************************************************************************************/
/***************************************测试模块****************************************/
/**************************************************************************************/

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

    audioRecorder->setAudioInput(ui->inputListCombox->currentText());
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

void Widget::TestFunc(int testItemIndex)
{
    /****************Step1：开启串口，发送指令**************/
//    openSerialPort(AR720_SerialPort);
//    switchMode(AR720_SwitchToUSB);
//    openSerialPort(USBTo485_Converter_SerialPort);
//    switchChannel(N76E885_SwitchToUSB);

    /****************Step2：播放音频，开始录音**************/
    //ooooo寻找声卡
    playTestSound("Realtek", 1, 5000);
    startRecord("HUAWEI", 10000, testItemIndex);

    /****************Step3：分析文件，判断是否通过**************/
    AudioFile<double> sourceFile;
    sourceFile.load(recordedAudioPath.toStdString());
    if ((din == NULL)||(out == NULL)){
        QMessageBox::information(this, "提示", "FFT内存错误",QMessageBox::Ok);;
    }
    else{
        for (int i=0; i<N; i++)
        {
            din[i][0] = sourceFile.samples[0][i];//将samples的channel0的数据赋给din
            din[i][1] = 0;
        }
    }
    fftw_execute(fftwPlan);

    /****************Step4：输出源音频数据至文件**************/

//    QString outputPath = qApp->applicationDirPath().append("/AmpOutput").append(testItemIndex).append(".txt");
//    QFile outputFile(outputPath);
//    outputFile.open(QIODevice::WriteOnly |QIODevice::Truncate);
//    QTextStream outStream(&outputFile);
//    outStream.setCodec("UTF-8");
//    outStream << "Amplitude" << endl;
    for(int i=0; i<N; i++)
    {
//        fOut[i] = (double)i*11025/N;//计算频率
        powOut[i] = qPow(out[i][0],2)+qPow(out[i][1],2);//计算功率
//        outStream << fOut[i] << "Hz " << powOut[i] << endl;
    }

    /****************Step5：计算THD并判断**************/
    double THD = THDCalculate(1000, powOut);
//    qDebug() << "THD" << THD;
    tableModel->item(testItemIndex-1, 3)->setData(THD);

    /****************Step6：开启下一步测试**************/
}

void Widget::TestFunc2nd()
{
//   audioPlayer->setMedia(QUrl::fromLocalFile(TestAudio));
//   audioPlayer->setVolume(30);
//   audioPlayer->play();

//    playTestSound(1, 5000);
}



/**************************************************************************************/
/*****************************************按键******************************************/
/**************************************************************************************/

void Widget::on_startButton_clicked()
{
    //更新设备列表

   TestFunc(1);
}

void Widget::on_stopButton_clicked()
  {
if(portConverter == NULL){openSerialPort(USBTo485_Converter_SerialPort);}
switchChannel(N76E885_SwitchToUSB);

}




