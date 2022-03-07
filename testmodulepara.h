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
    bool isBTWaitNeeded;

};

#endif // TESTMODULEPARA_H
