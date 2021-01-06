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
