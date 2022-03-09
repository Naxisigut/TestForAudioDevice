#ifndef TESTMODULEPARA_H
#define TESTMODULEPARA_H

#include <QString>


class TestModulePara
{
public:
    TestModulePara();
    ~TestModulePara();

    QString name;
    int testItemIndex;
    int modeCmd;
    int channelCmd;
    QString outputDevice;
    QString inputDevice;
    double playVolume;
    int playDuration;
    int recordDuration;
    bool isTestNeeded;
    bool isBTWaitNeeded;
    double THDThreshold;

};

#endif // TESTMODULEPARA_H
