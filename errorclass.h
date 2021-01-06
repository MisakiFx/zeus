#ifndef ERRORCLASS_H
#define ERRORCLASS_H
#include <QString>

class ErrorClass
{
public:
    ErrorClass(int code = 0, QString msg = QString(""));
    int GetCode();
    QString GetMsg();
private:
    int code;
    QString msg;
};

#endif // ERRORCLASS_H
