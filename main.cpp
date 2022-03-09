#include "widget.h"

#include <QApplication>
#include <QStyleFactory>
#include <QMutex>
#include <QDateTime>

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // 加锁
    static QMutex mutex;
    mutex.lock();

    QByteArray localMsg = msg.toLocal8Bit();

    QString strMsg("");
    switch(type)
    {
    case QtDebugMsg:
        strMsg = QString("Debug:");
        break;
    case QtWarningMsg:
        strMsg = QString("Warning:");
        break;
    case QtCriticalMsg:
        strMsg = QString("Critical:");
        break;
    case QtFatalMsg:
        strMsg = QString("Fatal:");
        break;
    }

    // 设置输出信息格式
    QString strDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
//    QString strMessage = QString("%1 Message:%2 File:%3  Line:%4  Function:%5  DateTime:%6")
//            .arg(strMsg).arg(localMsg.constData()).arg(context.file).arg(context.line).arg(context.function).arg(strDateTime);
    QString strMessage = QString("%1 Message:%2 ")
            .arg(strMsg).arg(localMsg.constData());

    // 输出信息至文件中（读写、追加形式）
    QFile file("log.txt");
    file.open(QIODevice::ReadWrite | QIODevice::Append);
    QTextStream stream(&file);
    stream << strMessage << "\r\n";
    file.flush();
    file.close();

    // 解锁
    mutex.unlock();
}

int main(int argc, char *argv[])
{
//    qInstallMessageHandler(myMessageOutput);//qdebug信息输出至文件

    QApplication a(argc, argv);
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    Widget w;
    w.show();
    return a.exec();
}
