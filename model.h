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
    CauseInfo(QString causeName, int causeType, int score, qint64 teacherId, QStringList causeTime, QStringList causeWeek, QString classroom, QStringList className, qint64 id = 0);
    qint64 				id;
    QString 			causeName;
    int 				causeType;
    int 				score;
    qint64 				teacherId;
    QStringList 		causeTime;
    QStringList 		causeWeek;
    QString 			classroom;
    QStringList 		className;
};

struct LeaveAsk
{
    LeaveAsk();
    LeaveAsk(qint64 teacherId, qint64 causeId, QString teachTime, qint64 stuId, qint64 id = 0, QString message = "", int status = 0);
    qint64 	id;
    qint64 	stuId;
    qint64 	causeId;
    qint64 	teacherId;
    QString teachTime;
    QString message;
    int 	status;
};

struct CauseCheckModel
{
    CauseCheckModel();
    CauseCheckModel(qint64 causeId, QString teachTime, qint64 stuId, int isLeave, qint64 id = 0);
    qint64 	id;
    qint64 	causeId;
    QString teachTime;
    qint64 	stuId;
    int 	isLeave;
};

struct StuCauseRelModel
{
    StuCauseRelModel();
    StuCauseRelModel(qint64 stuId, qint64 causeId);
    StuCauseRelModel(qint64 stuId, qint64 causeId, int score, QString evalute, qint64 id = 0);
    qint64 id;
    qint64 stuId;
    qint64 causeId;
    int score;
    QString evalute;
};

#endif // MODEL_H
