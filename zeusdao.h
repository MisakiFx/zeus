#ifndef ZEUSDAO_H
#define ZEUSDAO_H

#include <QObject>
#include <QVector>
#include "errorclass.h"
#include "model.h"
class ZeusDao
{
public:
    explicit ZeusDao();
    ~ZeusDao();
    static ErrorClass IsPasswordCorrect(qint64 account, QString password);
    static ErrorClass InitDao();
    static ErrorClass QureyClassIdByName(QString name, qint64& id);
    static ErrorClass InsertNewUser(UserInfo& info);
    static ErrorClass QueryUserInfoById(UserInfo& info);
    static ErrorClass QueryClassNameById(qint64 id, QString& name);
    static ErrorClass QueryClassNameByIdBatch(QVector<qint64> classId, QHash<qint64, QString>& className);
    static ErrorClass QueryAllClassName(QStringList& names);
    static ErrorClass QueryGrandNameById(qint64 id, QString& name);
    static ErrorClass QueryCauseInfoByName(CauseInfo& causeInfo);
    static ErrorClass QueryStudentInfoByCauseId(qint64 causeId, QVector<UserInfo>& studentInfo);
    static ErrorClass QueryLeaveAskStatus(QVector<LeaveAsk>& leaveAskInfo);
    static ErrorClass InsertCauseCheckBatch(QVector<CauseCheckModel>& causeCheck);
    static ErrorClass CreateNewClassInfo(CauseInfo& causeInfo);
    static ErrorClass QueryStuIdByClassNameBatch(QStringList className, QVector<qint64>& stuIds);
    static ErrorClass InsertStuCauseRel(CauseCheckModel rel);
};

#endif // ZEUSDAO_H
