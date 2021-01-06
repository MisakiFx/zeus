#include "errorclass.h"

ErrorClass::ErrorClass(int code, QString msg)
    :code(code),
     msg(msg)
{

}

int ErrorClass::GetCode()
{
    return code;
}

QString ErrorClass::GetMsg()
{
    return msg;
}

