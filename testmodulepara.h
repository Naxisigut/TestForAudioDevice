#ifndef TESTMODULEPARA_H
#define TESTMODULEPARA_H

#include <QString>
#include <QSettings>
#include <enum_for_all.h>

class TestModulePara
{
public:
    TestModulePara();
    ~TestModulePara();

    QString  name;
    int      testItemIndex;
    int      modeCmd;
    int      channelCmd;
    QString  outputDevice;
    QString  inputDevice;
    double   playVolume;
    int      playDuration;
    int      recordDuration;
    bool     isTestNeeded;
    bool     isBTWaitNeeded;
    double   THDThreshold;

    void     readINI(QString iniPath, int paraIndex);
    void     clear();

};

#endif // TESTMODULEPARA_H
