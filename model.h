#ifndef MODEL_H
#define MODEL_H


#include <QString>
#include <QStringList>

struct UserInfo
{
    UserInfo(QString name, int accountType, QString password, QString phoneNumber, qint64 classId = 0, qint64 grandId = 0, qint64 id = 0);
    UserInfo(qint64 id);
    UserInfo();
    qint64 	id;
    QString name;
    int 	accountType;
    QString password;
    QString phoneNumber;
    qint64 	classId;
    qint64 	grandId;
};

struct CauseInfo
{
    CauseInfo();
    CauseInfo(QString causeName);
    qint64 				id;
    QString 			causeName;
    int 				causeType;
    int 				score;
    qint64 				teacherId;
    QStringList 		causeTime;
    QStringList 		causeWeek;
    QString 			classroom;
};

#endif // MODEL_H
