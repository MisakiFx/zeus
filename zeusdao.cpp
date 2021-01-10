#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QCryptographicHash>
#include "zeusdao.h"
#include "macro.h"
#include "toolclass.h"
ZeusDao::ZeusDao()
{
}

ZeusDao::~ZeusDao()
{

}
//初始化数据库
ErrorClass ZeusDao::InitDao()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("127.0.0.1");
    db.setUserName("root");
    db.setPassword("1");
    db.setDatabaseName("test");

    if( !db.open() )
    {
        return ErrorClass(ERRCODE_SERVICE_ERROR, db.lastError().text());
    }
    qDebug() << "数据库连接成功";
    return ErrorClass();
}

//验证密码
ErrorClass ZeusDao::IsPasswordCorrect(qint64 account, QString password)
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query;
    QString sql = QString("SELECT password FROM zeus_user_info WHERE id = %1").arg(account);
    if (!query.exec(sql))
    {
        return ErrorClass(ERRCODE_SERVICE_ERROR, db.lastError().text());
    }
    if(!query.first())
    {
        return ErrorClass(ERRCODE_INPUT_ERROR, "账户不存在");
    }
    QString passwordInDB = query.value(0).toString();

    //密码进行md5加密
    QString passwordMd5 = QCryptographicHash::hash(password.toLocal8Bit(), QCryptographicHash::Md5).toHex().toUpper();

    if(passwordMd5 != passwordInDB)
    {
        return ErrorClass(ERRCODE_INPUT_ERROR, "账户与密码不符");
    }
    return ErrorClass();
}

//根据班级名称获取班级id
ErrorClass ZeusDao::QureyClassIdByName(QString name, qint64& id)
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query;
    QString sql = QString("SELECT id FROM zeus_class_info WHERE name = \"%1\"").arg(name);
    if (!query.exec(sql))
    {
        return ErrorClass(ERRCODE_SERVICE_ERROR, db.lastError().text());
    }
    if(!query.first())
    {
        return ErrorClass(ERRCODE_INPUT_ERROR, "未找到对应班级，请重新打开页面重试");
    }
    id = query.value(0).toLongLong();
    return ErrorClass();
}

//根据年级组名称获取年级组id
ErrorClass ZeusDao::QueryGradeIdByName(QString name, qint64 &id)
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query;
    QString sql = QString("SELECT id FROM zeus_grand_info WHERE name = \"%1\"").arg(name);
    if (!query.exec(sql))
    {
        return ErrorClass(ERRCODE_SERVICE_ERROR, db.lastError().text());
    }
    if(!query.first())
    {
        return ErrorClass(ERRCODE_INPUT_ERROR, "未找到对应年级组，请重新打开页面重试");
    }
    id = query.value(0).toLongLong();
    return ErrorClass();
}

//插入人员信息
ErrorClass ZeusDao::InsertNewUser(UserInfo &info)
{
    QSqlDatabase db = QSqlDatabase::database();

    //密码进行md5加密
    QString passwordMd5 = QCryptographicHash::hash(info.password.toLocal8Bit(), QCryptographicHash::Md5).toHex().toUpper();
    qint64 id = ToolCLass::IDGenerate();
    QSqlQuery query;
    QString sql = QString("INSERT INTO zeus_user_info value(%1, \"%2\", %3, \"%4\", \"%5\", %6, %7)")
                    .arg(id).arg(info.name).arg(info.accountType).arg(passwordMd5)
                    .arg(info.phoneNumber).arg(info.classId).arg(info.grandId);
    if(!query.exec(sql))
    {
        return ErrorClass(ERRCODE_INPUT_ERROR, "电话号码已注册");
    }
    info.id = id;

    return ErrorClass();
}

//根据用户ID查询用户信息
ErrorClass ZeusDao::QueryUserInfoById(UserInfo &info)
{
    QSqlDatabase db = QSqlDatabase::database();

    QSqlQuery query;
    QString sql = QString("SELECT * FROM zeus_user_info WHERE id = %1").arg(info.id);
    if(!query.exec(sql))
    {
        return ErrorClass(ERRCODE_SERVICE_ERROR, db.lastError().text());
    }
    if(!query.first())
    {
        return ErrorClass(ERRCODE_INPUT_ERROR, "未找到对应用户信息");
    }
    info.name = query.value(1).toString();
    info.accountType = query.value(2).toInt();
    info.password = query.value(3).toString();
    info.phoneNumber = query.value(4).toString();
    info.classId = query.value(5).toInt();
    info.grandId = query.value(6).toInt();
    return ErrorClass();
}


//根据班级ID查询班级名称
ErrorClass ZeusDao::QueryClassNameById(qint64 id, QString &name)
{
    QSqlDatabase db = QSqlDatabase::database();

    QSqlQuery query;
    QString sql = QString("SELECT name FROM zeus_class_info WHERE id = %1").arg(id);
    if(!query.exec(sql))
    {
        return ErrorClass(ERRCODE_SERVICE_ERROR, db.lastError().isValid() ? db.lastError().text() : ERRMSG_UNKNOW_MSG);
    }
    if(!query.first())
    {
        return ErrorClass(ERRCODE_INPUT_ERROR, "未找到对应班级信息");
    }
    name = query.value(0).toString();
    return ErrorClass();
}

//根据年级组id查询年级组名称
ErrorClass ZeusDao::QueryGrandNameById(qint64 id, QString &name)
{
    QSqlDatabase db = QSqlDatabase::database();

    QSqlQuery query;
    QString sql = QString("SELECT name FROM zeus_grand_info WHERE id = %1").arg(id);
    if(!query.exec(sql))
    {
        return ErrorClass(ERRCODE_SERVICE_ERROR, db.lastError().isValid() ? db.lastError().text() : ERRMSG_UNKNOW_MSG);
    }
    if(!query.first())
    {
        return ErrorClass(ERRCODE_INPUT_ERROR, "未找到对应年级组信息");
    }
    name = query.value(0).toString();
    return ErrorClass();
}

//根据班级名称获取班级具体信息
ErrorClass ZeusDao::QueryCauseInfoByName(CauseInfo& causeInfo)
{
    QSqlDatabase db = QSqlDatabase::database();

    QSqlQuery query;
    QString sql = QString("SELECT * FROM zeus_cause_info WHERE cause_name = \"%1\"").arg(causeInfo.causeName);
    if (!query.exec(sql))
    {
        return ErrorClass(ERRCODE_SERVICE_ERROR, db.lastError().isValid() ? db.lastError().text() : ERRMSG_UNKNOW_MSG);
    }
    if(!query.first())
    {
        return ErrorClass(ERRCODE_INPUT_ERROR, "未找到对应课程组信息");
    }

    causeInfo.id = query.value(0).toLongLong();
    causeInfo.causeType = query.value(2).toInt();
    causeInfo.score = query.value(3).toInt();
    causeInfo.teacherId = query.value(4).toLongLong();
    causeInfo.causeTime = query.value(5).toString().split(",");
    causeInfo.causeWeek = query.value(6).toString().split(",");
    causeInfo.classroom = query.value(7).toString();
    return ErrorClass();
}

//根据课程id获取所有选课学生信息
ErrorClass ZeusDao::QueryStudentInfoByCauseId(qint64 causeId, QVector<UserInfo>& studentInfo)
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query;
    QString sql = QString("SELECT * From zeus_user_info WHERE id IN (SELECT stu_id FROM zeus_stu_cause_rel WHERE cause_id = %1)").arg(causeId);
    if (!query.exec(sql))
    {
        return ErrorClass(ERRCODE_SERVICE_ERROR, db.lastError().isValid() ? db.lastError().text() : ERRMSG_UNKNOW_MSG);
    }
    for (; query.next(); )
    {
        studentInfo.push_back(UserInfo(query.value(1).toString(), query.value(2).toInt(), query.value(3).toString(),
                                       query.value(4).toString(), query.value(5).toLongLong(), query.value(6).toLongLong(), query.value(0).toLongLong()));
    }
    return ErrorClass();
}

//根据id批量获取班级名称
ErrorClass ZeusDao::QueryClassNameByIdBatch(QVector<qint64> classId, QHash<qint64, QString>& className)
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query;
    QString idsString;
    for (int i = 0; i < classId.size(); i++)
    {
        if(i == 0)
        {
            idsString += QString::number(classId[i]);
        } else
        {
            idsString += ",";
            idsString += QString::number(classId[i]);
        }
    }
    QString sql = QString("SELECT * FROM zeus_class_info WHERE id IN (%1)").arg(idsString);
    if (!query.exec(sql))
    {
        return ErrorClass(ERRCODE_SERVICE_ERROR, db.lastError().isValid() ? db.lastError().text() : ERRMSG_UNKNOW_MSG);
    }
    for (; query.next(); )
    {
        className.insert(query.value(0).toLongLong(), query.value(1).toString());
    }
    return ErrorClass();
}

ErrorClass ZeusDao::QueryLeaveAskByTeacherId(qint64 teacherId, QVector<LeaveAsk> &leaveAsks)
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query;
    QString sql = QString("SELECT * FROM zeus_leave_ask WHERE teacher_id = %1 AND status = %2;").arg(teacherId).arg(LEAVING_ASK_STAUTS_ASKING);
    if (!query.exec(sql))
    {
        return ErrorClass(ERRCODE_SERVICE_ERROR, db.lastError().isValid() ? db.lastError().text() : ERRMSG_UNKNOW_MSG);
    }
    for (; query.next(); )
    {
        leaveAsks.push_back(LeaveAsk(query.value(0).toLongLong(), LEAVING_ASK_STAUTS_ASKING));
    }

    return ErrorClass();
}

//根据学生id批量查询学生是否请假
ErrorClass ZeusDao::QueryLeaveAskStatus(QVector<LeaveAsk>& leaveAskInfo)
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query;
    for (int i = 0; i < leaveAskInfo.size(); i++)
    {
        QString sqlString = QString("SELECT status FROM zeus_leave_ask WHERE teacher_id = %1 AND cause_id = %2 AND teach_time = \"%3\" AND stu_id = %4")
                        .arg(leaveAskInfo[i].teacherId)
                        .arg(leaveAskInfo[i].causeId)
                        .arg(leaveAskInfo[i].teachTime)
                        .arg(leaveAskInfo[i].stuId);
        if (!query.exec(sqlString))
        {
            return ErrorClass(ERRCODE_SERVICE_ERROR, db.lastError().isValid() ? db.lastError().text() : ERRMSG_UNKNOW_MSG);
        }
        if (!query.first())
        {
            leaveAskInfo[i].status = 0;
        }else
        {
            leaveAskInfo[i].status = query.value(0).toInt();
        }
    }
    return ErrorClass();
}

//批量插入考勤表
ErrorClass ZeusDao::InsertCauseCheckBatch(QVector<CauseCheckModel>& causeCheck)
{
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.transaction())
    {
        return ErrorClass(ERRCODE_SERVICE_ERROR, db.lastError().isValid() ? db.lastError().text() : ERRMSG_UNKNOW_MSG);
    }
    QSqlQuery query;
    for (int i = 0; i < causeCheck.size(); i++)
    {
        qint64 id = ToolCLass::IDGenerate();
        QSqlQuery query;
        QString sql = QString("INSERT INTO zeus_cause_check_info value(%1, %2, '%3', %4, %5)")
                        .arg(id)
                        .arg(causeCheck[i].causeId)
                        .arg(causeCheck[i].teachTime)
                        .arg(causeCheck[i].stuId)
                        .arg(causeCheck[i].isLeave);
        if (!query.exec(sql))
        {
            db.rollback();
            return ErrorClass(ERRCODE_SERVICE_ERROR, db.lastError().isValid() ? db.lastError().text() : ERRMSG_UNKNOW_MSG);
        }
        causeCheck[i].id = id;
    }
    db.commit();
    return ErrorClass();
}

//查询所有班级名称
ErrorClass ZeusDao::QueryAllClassName(QStringList &names)
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query;
    QString sql = "SELECT name FROM zeus_class_info ORDER BY name";
    if (!query.exec(sql))
    {
        return ErrorClass(ERRCODE_SERVICE_ERROR, db.lastError().isValid() ? db.lastError().text() : ERRMSG_UNKNOW_MSG);
    }
    for (; query.next(); )
    {
        names.push_back(query.value(0).toString());
    }
    return ErrorClass();
}

//根据班级名称批次查询在班级里的学号
ErrorClass ZeusDao::QueryStuIdByClassNameBatch(QStringList className, QVector<qint64> &stuIds)
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query;
    QString classNamesStr;
    for (int i = 0; i < className.size(); i++)
    {
        if(i == 0)
        {
            classNamesStr += "'";
            classNamesStr += className[i];
            classNamesStr += "'";
        } else
        {
            classNamesStr += ", '";
            classNamesStr += className[i];
            classNamesStr += "'";
        }
    }
    QString sql = QString("SELECT userInfo.id FROM zeus_class_info classInfo INNER JOIN zeus_user_info userInfo ON classInfo.id = userInfo.class_id WHERE classInfo.name IN (%1)").arg(classNamesStr);
    if (!query.exec(sql))
    {
        return ErrorClass(ERRCODE_SERVICE_ERROR, db.lastError().isValid() ? db.lastError().text() : ERRMSG_UNKNOW_MSG);
    }
    for (; query.next(); )
    {
        stuIds.push_back(query.value(0).toLongLong());
    }
    return ErrorClass();
}

//插入新课程
ErrorClass ZeusDao::CreateNewClassInfo(CauseInfo &causeInfo)
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query;

    //插入课程信息
    qint64 causeId = ToolCLass::IDGenerate();
    QString causeTimeStr;
    QString causeWeekStr;
    QVector<qint64> stuIds;
    for(int i = 0; i < causeInfo.causeTime.size(); i++)
    {
        if (i == 0)
        {
            causeTimeStr += causeInfo.causeTime[i];
        } else
        {
            causeTimeStr += ",";
            causeTimeStr += causeInfo.causeTime[i];
        }
    }
    for (int i = 0; i < causeInfo.causeWeek.size(); i++)
    {
        if(i == 0)
        {
            causeWeekStr += causeInfo.causeWeek[i];
        }else
        {
            causeWeekStr += ",";
            causeWeekStr += causeInfo.causeWeek[i];
        }
    }
    if(causeInfo.causeType == CAUSE_TYPE_REQUIRED)
    {
        QueryStuIdByClassNameBatch(causeInfo.className, stuIds);
    }
    QString sql = QString("INSERT INTO zeus_cause_info VALUES(%1, '%2', %3, %4, %5, '%6', '%7', '%8')")
                          .arg(causeId)
                          .arg(causeInfo.causeName)
                          .arg(causeInfo.causeType)
                          .arg(causeInfo.score)
                          .arg(causeInfo.teacherId)
                          .arg(causeTimeStr)
                          .arg(causeWeekStr)
                          .arg(causeInfo.classroom);
    if (!db.transaction())
    {
        return ErrorClass(ERRCODE_SERVICE_ERROR, db.lastError().isValid() ? db.lastError().text() : ERRMSG_UNKNOW_MSG);
    }
    if (!query.exec(sql))
    {
        db.rollback();
        return ErrorClass(ERRCODE_SERVICE_ERROR, db.lastError().isValid() ? db.lastError().text() : ERRMSG_UNKNOW_MSG);
    }

    //如果是必修课，插入上课学生
    for (int i = 0; i < stuIds.size(); i++)
    {
        qint64 id = ToolCLass::IDGenerate();
        QString sql = QString("INSERT INTO zeus_stu_cause_rel(id, stu_id, cause_id) VALUES(%1, %2, %3)")
                .arg(id)
                .arg(stuIds[i])
                .arg(causeId);
        if (!query.exec(sql))
        {
            db.rollback();
            return ErrorClass(ERRCODE_SERVICE_ERROR, db.lastError().isValid() ? db.lastError().text() : ERRMSG_UNKNOW_MSG);
        }
    }
    db.commit();
    causeInfo.id = causeId;
    return ErrorClass();
}

//根据课程id查询关联表信息
ErrorClass ZeusDao::QueryStuCauseRelByCauseId(qint64 causeId, QHash<qint64, StuCauseRelModel>& stuIdCauseRelMap)
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query;
    QString sql = QString("SELECT * FROM zeus_stu_cause_rel WHERE cause_id = %1").arg(causeId);
    if (!query.exec(sql))
    {
        db.rollback();
        return ErrorClass(ERRCODE_SERVICE_ERROR, db.lastError().isValid() ? db.lastError().text() : ERRMSG_UNKNOW_MSG);
    }
    for(; query.next(); )
    {
        qint64 stuId = query.value(1).toLongLong();
        stuIdCauseRelMap.insert(stuId, StuCauseRelModel(
                                    stuId,
                                    query.value(2).toLongLong(),
                                    query.value(3).toInt(),
                                    query.value(4).toString(),
                                    query.value(0).toLongLong()));
    }

    return ErrorClass();
}

//批量修改学生成绩
ErrorClass ZeusDao::UpdateStuCauseRelScoreBatch(QVector<StuCauseRelModel> rels)
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query;
    if (!db.transaction())
    {
        return ErrorClass(ERRCODE_SERVICE_ERROR, db.lastError().isValid() ? db.lastError().text() : ERRMSG_UNKNOW_MSG);
    }
    for (int i = 0; i < rels.size(); i++)
    {
        QString sql = QString("UPDATE zeus_stu_cause_rel SET score = %1 WHERE cause_id = %2 AND stu_id = %3")
                .arg(rels[i].score)
                .arg(rels[i].causeId)
                .arg(rels[i].stuId);
        if (!query.exec(sql))
        {
            db.rollback();
            return ErrorClass(ERRCODE_SERVICE_ERROR, db.lastError().isValid() ? db.lastError().text() : ERRMSG_UNKNOW_MSG);
        }
    }
    db.commit();
    return ErrorClass();
}

//根据课程id和学生id查询课程和学生关联表信息
ErrorClass ZeusDao::QueryStuCauseRelByCauseIdAndStuId(StuCauseRelModel &stuCauseRel)
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query;
    QString sql = QString("SELECT * FROM zeus_stu_cause_rel WHERE cause_id = %1 AND stu_id = %2")
            .arg(stuCauseRel.causeId)
            .arg(stuCauseRel.stuId);
    if (!query.exec(sql))
    {
        return ErrorClass(ERRCODE_SERVICE_ERROR, db.lastError().isValid() ? db.lastError().text() : ERRMSG_UNKNOW_MSG);
    }
    if (!query.first())
    {
        return ErrorClass(ERRCODE_INPUT_ERROR, "该学生没有选择该课程");
    }
    stuCauseRel.id = query.value(0).toLongLong();
    stuCauseRel.score = query.value(3).toInt();
    stuCauseRel.evalute = query.value(4).toString();
    return ErrorClass();
}

//根据学号和课程编号修改学生评教信息
ErrorClass ZeusDao::UpdateStuCauseRelEvalute(const StuCauseRelModel &rel)
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query;
    QString sql = QString("UPDATE zeus_stu_cause_rel SET evalute = '%1' WHERE cause_id = %2 AND stu_id = %3")
            .arg(rel.evalute)
            .arg(rel.causeId)
            .arg(rel.stuId);
    if (!query.exec(sql))
    {
        return ErrorClass(ERRCODE_SERVICE_ERROR, db.lastError().isValid() ? db.lastError().text() : ERRMSG_UNKNOW_MSG);
    }
    return ErrorClass();
}

ErrorClass ZeusDao::DeleteStuCauseRel(const StuCauseRelModel& rel)
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query;
    QString sql = QString("DELETE FROM zeus_stu_cause_rel WHERE cause_id = %1 AND stu_id = %2")
            .arg(rel.causeId)
            .arg(rel.stuId);
    if (!query.exec(sql))
    {
        return ErrorClass(ERRCODE_SERVICE_ERROR, db.lastError().isValid() ? db.lastError().text() : ERRMSG_UNKNOW_MSG);
    }
    return ErrorClass();
}

//插入管理表信息
ErrorClass ZeusDao::InsertStuCauseRel(const StuCauseRelModel& rel)
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query;
    qint64 id = ToolCLass::IDGenerate();
    QString sql = QString("INSERT INTO zeus_stu_cause_rel(id, stu_id, cause_id) VALUES(%1, %2, %3)")
            .arg(id)
            .arg(rel.stuId)
            .arg(rel.causeId);
    if (!query.exec(sql))
    {
        db.rollback();
        return ErrorClass(ERRCODE_SERVICE_ERROR, db.lastError().isValid() ? db.lastError().text() : ERRMSG_UNKNOW_MSG);
    }
    return ErrorClass();
}

//新增请假申请
ErrorClass ZeusDao::InsertLeaveAsk(const LeaveAsk& leaveAskInfo)
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query;
    qint64 id = ToolCLass::IDGenerate();
    QString sql = QString("INSERT INTO zeus_leave_ask VALUES(%1, %2, %3, '%4', %5, '%6', '%7', %8)")
            .arg(id)
            .arg(leaveAskInfo.stuId)
            .arg(leaveAskInfo.causeId)
            .arg(leaveAskInfo.causeName)
            .arg(leaveAskInfo.teacherId)
            .arg(leaveAskInfo.teachTime)
            .arg(leaveAskInfo.message)
            .arg(leaveAskInfo.status);
    if (!query.exec(sql))
    {
        db.rollback();
        return ErrorClass(ERRCODE_SERVICE_ERROR, db.lastError().isValid() ? db.lastError().text() : "该课程同一时间下已经请过假");
    }
    return ErrorClass();
}

//根据老师工号查询所有与之相关的请假申请
ErrorClass ZeusDao::QueryAllLeaveAskByTeacherId(qint64 teacherId, QVector<LeaveAsk> &leaveAsks)
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query;
    QString sql = QString("SELECT leaveAsk.id, leaveAsk.cause_id, leaveAsk.cause_name, userInfo.name, userInfo.id AS 'stu_id', leaveAsk.teach_time, leaveAsk.message, leaveAsk.status\
                          FROM zeus_user_info userInfo\
                          INNER JOIN zeus_leave_ask leaveAsk ON userInfo.id = leaveAsk.stu_id\
                          WHERE teacher_id = %1 ORDER BY leaveAsk.teach_time").arg(teacherId);
    if (!query.exec(sql))
    {
        db.rollback();
        return ErrorClass(ERRCODE_SERVICE_ERROR, db.lastError().isValid() ? db.lastError().text() : ERRMSG_UNKNOW_MSG);
    }
    for (; query.next(); )
    {
        leaveAsks.push_back(LeaveAsk(query.value(4).toLongLong(),
                                     query.value(3).toString(),
                                     query.value(1).toLongLong(),
                                     query.value(2).toString(),
                                     teacherId,
                                     query.value(5).toString(),
                                     query.value(6).toString(),
                                     query.value(7).toInt(),
                                     query.value(0).toLongLong()));
    }
    return ErrorClass();
}

//更新请假审批的状态
ErrorClass ZeusDao::UpdateLeaveAskStatusBatch(QVector<LeaveAsk> leaveAskInfo)
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query;
    if (!db.transaction())
    {
        return ErrorClass(ERRCODE_SERVICE_ERROR, db.lastError().isValid() ? db.lastError().text() : ERRMSG_UNKNOW_MSG);
    }
    for (int i = 0; i < leaveAskInfo.size(); i++)
    {
        QString sql = QString("UPDATE zeus_leave_ask SET status = %1 WHERE id = %2")
                .arg(leaveAskInfo[i].status)
                .arg(leaveAskInfo[i].id);
        if (!query.exec(sql))
        {
            db.rollback();
            return ErrorClass(ERRCODE_SERVICE_ERROR, db.lastError().isValid() ? db.lastError().text() : ERRMSG_UNKNOW_MSG);
        }
    }
    db.commit();
    return ErrorClass();
}

//新增班级
ErrorClass ZeusDao::InserClassInfo(QString name)
{
    QSqlDatabase db = QSqlDatabase::database();
    qint64 id = ToolCLass::IDGenerate();
    QSqlQuery query;
    QString sql = QString("INSERT INTO zeus_class_info VALUES(%1, '%2')")
            .arg(id)
            .arg(name);
    if(!query.exec(sql))
    {
        return ErrorClass(ERRCODE_INPUT_ERROR, "班级名称已存在");
    }
    return ErrorClass();
}

//新增年级组
ErrorClass ZeusDao::InserGradeInfo(QString name)
{
    QSqlDatabase db = QSqlDatabase::database();
    qint64 id = ToolCLass::IDGenerate();
    QSqlQuery query;
    QString sql = QString("INSERT INTO zeus_grand_info VALUES(%1, '%2')")
            .arg(id)
            .arg(name);
    if(!query.exec(sql))
    {
        return ErrorClass(ERRCODE_INPUT_ERROR, "年级组名称已存在");
    }
    return ErrorClass();
}

//判断是不是选课时间
ErrorClass ZeusDao::QueryIsTime(bool& isChooseCauseTime, bool& isEvaluteTime)
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query;

    QString sql = QString("SELECT * FROM zeus_status_control WHERE id = 1");
    if(!query.exec(sql))
    {
        return ErrorClass(ERRCODE_INPUT_ERROR, db.lastError().isValid() ? db.lastError().text() : ERRMSG_UNKNOW_MSG);
    }
    if(!query.first())
    {
        return ErrorClass(ERRCODE_INPUT_ERROR, db.lastError().isValid() ? db.lastError().text() : ERRMSG_UNKNOW_MSG);
    }
    if (query.value(1).toInt() == 1)
    {
        isChooseCauseTime = true;
        qDebug() << 1;
    } else
    {
        isChooseCauseTime = false;
    }
    if (query.value(2).toInt() == 1)
    {
        isEvaluteTime = true;
        qDebug() << 2;
    }else
    {
        isEvaluteTime = false;
    }
    return ErrorClass();
}
