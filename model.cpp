#include "model.h"

UserInfo::UserInfo(QString name, int accountType, QString password, QString phoneNumber, qint64 classId, qint64 grandId, qint64 id)
    : id(id)
    , name(name)
    , accountType(accountType)
    , password(password)
    , phoneNumber(phoneNumber)
    , classId(classId)
    , grandId(grandId)
{

}

UserInfo::UserInfo(qint64 id)
    : id(id)
{

}

UserInfo::UserInfo()
{

}

CauseInfo::CauseInfo()
{

}

CauseInfo::CauseInfo(QString causeName)
    : causeName(causeName)
{

}

CauseInfo::CauseInfo(QString causeName, int causeType, int score, qint64 teacherId, QStringList causeTime, QStringList causeWeek, QString classroom, QStringList className, qint64 id)
    : id(id)
    , causeName(causeName)
    , causeType(causeType)
    , score(score)
    , teacherId(teacherId)
    , causeTime(causeTime)
    , causeWeek(causeWeek)
    , classroom(classroom)
    , className(className)
{

}

LeaveAsk::LeaveAsk(qint64 teacherId, qint64 causeId, QString teachTime, qint64 stuId, qint64 id, QString message, int status)
    : id(id)
    , stuId(stuId)
    , causeId(causeId)
    , teacherId(teacherId)
    , teachTime(teachTime)
    , message(message)
    , status(status)
{

}

CauseCheckModel::CauseCheckModel()
{

}

CauseCheckModel::CauseCheckModel(qint64 causeId, QString teachTime, qint64 stuId, int isLeave, qint64 id)
    : id(id)
    , causeId(causeId)
    , teachTime(teachTime)
    , stuId(stuId)
    , isLeave(isLeave)
{

}

StuCauseRelModel::StuCauseRelModel()
{

}

StuCauseRelModel::StuCauseRelModel(qint64 stuId, qint64 causeId)
    : stuId(stuId)
    , causeId(causeId)
{

}
