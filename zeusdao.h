#ifndef ZEUSDAO_H
#define ZEUSDAO_H

#include <QObject>
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
    static ErrorClass QueryGrandNameById(qint64 id, QString& name);
};

#endif // ZEUSDAO_H
