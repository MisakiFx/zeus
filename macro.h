#ifndef MACRO_H
#define MACRO_H


#define ERRMSG_UNKNOW_MSG "未知错误"
#define ERRMSG_DATABASE_EMPEY_ERR " "

const int ERRCODE_SUCCESS = 0;
const int ERRCODE_SERVICE_ERROR = -1;
const int ERRCODE_INPUT_ERROR = -2;
const int ERRCODE_UNKNOW_ERROR = -99;

const int ACCOUNT_TYPE_STUDENT = 1;
const int ACCOUNT_TYPE_TEACHER = 2;
const int ACCOUNT_TYPE_ADMIN = 3;

const int CAUSE_TYPE_REQUIRED = 1;
const int CAUSE_TYPE_OPTIONAL = 1;

const int LEAVING_ASK_STAUTS_ASKING = 1;
const int LEAVING_ASK_STAUTS_PASS = 2;
const int LEAVING_ASK_STAUTS_REFUSE = 3;

#endif // MACRO_H
