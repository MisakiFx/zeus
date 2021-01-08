#include <QMessageBox>
#include "causegrade.h"
#include "ui_causegrade.h"
#include "errorclass.h"
#include "zeusdao.h"
#include "macro.h"

CauseGrade::CauseGrade(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CauseGrade)
{
    ui->setupUi(this);
}

CauseGrade::CauseGrade(CauseInfo& causeInfo, QWidget *parent):
    QDialog(parent),
    ui(new Ui::CauseGrade),
    causeInfo(causeInfo)
{
    ui->setupUi(this);

    //初始化数据
    QVector<UserInfo> studentInfo;
    ErrorClass err = ZeusDao::QueryStudentInfoByCauseId(causeInfo.id, studentInfo);
    if(err.GetCode() != ERRCODE_SUCCESS)
    {
        QMessageBox::warning(this, "警告", err.GetMsg());
        return;
    }

}

CauseGrade::~CauseGrade()
{
    delete ui;
}

