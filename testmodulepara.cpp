#include "testmodulepara.h"

TestModulePara::TestModulePara()
{

}

TestModulePara::~TestModulePara()
{}

void TestModulePara::readINI(QString iniPath, int paraIndex)
{
    QSettings testItemSetting(iniPath, QSettings::IniFormat);
    QString path = "Item";
    QString tempPath;

    //name
    tempPath =path;
    tempPath.append(QString::number(paraIndex+1).append("/name"));
    name           = testItemSetting.value(tempPath).toString();

    //testItemIndex
    tempPath =path;
    tempPath.append(QString::number(paraIndex+1).append("/testItemIndex"));
    testItemIndex  = testItemSetting.value(tempPath).toInt();

    //outputDevice
    tempPath =path;
    tempPath.append(QString::number(paraIndex+1).append("/outputDevice"));
    outputDevice   = testItemSetting.value(tempPath).toString();

    //inputDevice
    tempPath =path;
    tempPath.append(QString::number(paraIndex+1).append("/inputDevice"));
    inputDevice    = testItemSetting.value(tempPath).toString();

    //playVolume
    tempPath =path;
    tempPath.append(QString::number(paraIndex+1).append("/playVolume"));
    playVolume     = testItemSetting.value(tempPath).toDouble();

    //playDuration
    tempPath =path;
    tempPath.append(QString::number(paraIndex+1).append("/playDuration"));
    playDuration   = testItemSetting.value(tempPath).toInt();

    //recordDuration
    tempPath =path;
    tempPath.append(QString::number(paraIndex+1).append("/recordDuration"));
    recordDuration = testItemSetting.value(tempPath).toInt();

    //THDThreshold
    tempPath =path;
    tempPath.append(QString::number(paraIndex+1).append("/THDThreshold"));
    THDThreshold   = testItemSetting.value(tempPath).toDouble();

    //modeCmd
    tempPath =path;
    tempPath.append(QString::number(paraIndex+1).append("/modeCmd"));
    if(testItemSetting.value(tempPath).toString().contains("BT"))
    {modeCmd       = AR720_SwitchToBT;}else
    {modeCmd       = AR720_SwitchToUSB;}

    //channelCmd
    tempPath =path;
    tempPath.append(QString::number(paraIndex+1).append("/channelCmd"));
    if(testItemSetting.value(tempPath).toString().contains("BT"))
    {channelCmd    = N76E885_SwitchToBT;}else
    {channelCmd    = N76E885_SwitchToUSB;}

    //isTestNeeded
    tempPath =path;
    tempPath.append(QString::number(paraIndex+1).append("/isTestNeeded"));
    isTestNeeded   = testItemSetting.value(tempPath).toInt();

    //isBTWaitNeeded
    tempPath =path;
    tempPath.append(QString::number(paraIndex+1).append("/isBTWaitNeeded"));
    isBTWaitNeeded = testItemSetting.value(tempPath).toInt();
}

void TestModulePara::clear()
{
    name           = "NULL";
    testItemIndex  = 0;
    outputDevice   = "NULL";
    inputDevice    = "NULL";
    playVolume     = 0;
    playDuration   = 0;
    recordDuration = 0;
    THDThreshold   = 0;
    modeCmd       = 0;
    channelCmd    = 0;
    isTestNeeded   = 1;
    isBTWaitNeeded = 0;
};
