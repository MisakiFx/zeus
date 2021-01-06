#ifndef MODEL_H
#define MODEL_H


#include <QString>

struct UserInfo
{
    UserInfo(QString name, int accountType, QString password, QString phoneNumber, qint64 classId = 0, qint64 grandId = 0, qint64 id = 0);
    UserInfo();

    qint64 	id;
    QString name;
    int 	accountType;
    QString password;
    QString phoneNumber;
    qint64 	classId;
    qint64 	grandId;
};

#endif // MODEL_H
