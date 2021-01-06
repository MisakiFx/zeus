#include "toolclass.h"
#include <QDateTime>
#include <QRandomGenerator>

ToolCLass::ToolCLass()
{

}
ToolCLass::~ToolCLass()
{

}

qint64 ToolCLass::IDGenerate()
{
    QDateTime currentDateTime =QDateTime::currentDateTime();
    QString currentYear = currentDateTime.toString("yyyy");
    uint timeStamp = currentDateTime.toTime_t();
    QString timeStampString = QString::number(timeStamp);
    int randomNum = QRandomGenerator::global()->bounded(1000, 9999);
    QString randowNumString = QString::number(randomNum);
    QString idString = currentYear + timeStampString + randowNumString;
    return idString.toLongLong();
}
