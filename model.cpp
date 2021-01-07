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

CauseInfo::CauseInfo(QString causeName)
    : causeName(causeName)
{

}
