#include "evalute.h"
#include "ui_evalute.h"
#include "zeusdao.h"
#include "macro.h"
#include <QMessageBox>

Evalute::Evalute(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Evalute)
{
    ui->setupUi(this);
}

Evalute::Evalute(const CauseInfo& causeInfo, const UserInfo& userInfo, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Evalute),
    causeInfo(causeInfo),
    userInfo(userInfo)
{
    ui->setupUi(this);
    StuCauseRelModel stuCuaseRel(userInfo.id, causeInfo.id);
    ErrorClass err = ZeusDao::QueryStuCauseRelByCauseIdAndStuId(stuCuaseRel);
    if(err.GetCode() != ERRCODE_SUCCESS)
    {
        QMessageBox::warning(this, "警告", err.GetMsg());
        return;
    }
    if (stuCuaseRel.evalute.compare("") != 0)
    {
        if (stuCuaseRel.evalute.compare("A") == 0)
        {
            ui->radioButtonA->setChecked(true);
        } else if(stuCuaseRel.evalute.compare("B") == 0)
        {
            ui->radioButtonB->setChecked(true);
        } else if(stuCuaseRel.evalute.compare("C") == 0)
        {
            ui->radioButtonC->setChecked(true);
        } else if(stuCuaseRel.evalute.compare("D") == 0)
        {
            ui->radioButtonD->setChecked(true);
        }
    }
}

Evalute::~Evalute()
{
    delete ui;
}

void Evalute::on_pushButtonSubmit_released()
{
    QMessageBox::StandardButton button = QMessageBox::question(this, "警告", "确定提交?");
    if(button == QMessageBox::No)
    {
        return;
    }
    StuCauseRelModel rel(userInfo.id, causeInfo.id);
    if(ui->radioButtonA->isChecked())
    {
        rel.evalute = "A";
    } else if(ui->radioButtonB->isChecked())
    {
        rel.evalute = "B";
    }else if(ui->radioButtonC->isChecked())
    {
        rel.evalute = "C";
    }else if(ui->radioButtonD->isChecked())
    {
        rel.evalute = "D";
    } else {
        QMessageBox::warning(this, "警告", "选项不能为空");
        return;
    }
    ErrorClass err = ZeusDao::UpdateStuCauseRelEvalute(rel);
    if(err.GetCode() != ERRCODE_SUCCESS)
    {
        QMessageBox::warning(this, "警告", err.GetMsg());
        return;
    }
    QMessageBox::information(this, "成功", "操作成功");
    close();
    return;
}
