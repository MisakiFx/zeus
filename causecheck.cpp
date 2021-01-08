#include <QTableWidgetItem>
#include <QMessageBox>
#include <QDebug>
#include "causecheck.h"
#include "ui_causecheck.h"
#include "zeusdao.h"
#include "macro.h"
#include "model.h"

CauseCheck::CauseCheck(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CauseCheck)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
}

CauseCheck::CauseCheck(CauseInfo& causeInfo, QWidget *parent):
    QDialog(parent),
    ui(new Ui::CauseCheck),
    causeInfo(causeInfo)
{
    ui->setupUi(this);

    //初始化下拉框
    for(int i = 0; i < causeInfo.causeWeek.size(); i++)
    {
        ui->comboBoxCauseWeek->addItem(causeInfo.causeWeek[i]);
    }
    for(int i = 0; i < causeInfo.causeTime.size(); i++)
    {
        ui->comboBoxCauseTime->addItem(causeInfo.causeTime[i]);
    }

    //初始化学生列表
    QVector<UserInfo> studentInfo;
    ErrorClass err = ZeusDao::QueryStudentInfoByCauseId(causeInfo.id, studentInfo);
    if(err.GetCode() != ERRCODE_SUCCESS)
    {
        QMessageBox::warning(this, "警告", err.GetMsg());
        return;
    }

    //初始化班级信息
    QVector<qint64> classIds;
    for(int i = 0; i < studentInfo.size(); i++)
    {
        classIds.push_back(studentInfo[i].classId);
    }
    QHash<qint64, QString> classIdNameMap;
    err = ZeusDao::QueryClassNameByIdBatch(classIds, classIdNameMap);
    if(err.GetCode() != ERRCODE_SUCCESS)
    {
        QMessageBox::warning(this, "警告", err.GetMsg());
        return;
    }

    //设置表格行列
    ui->tableWidgetStuTable->setRowCount(studentInfo.size());
    ui->tableWidgetStuTable->setColumnCount(4);

    //设置表头
    QStringList header;
    header << "缺勤" << "学号" << "姓名" << "班级";
    ui->tableWidgetStuTable->setHorizontalHeaderLabels(header);

    //初始化表格信息
    for (int i = 0; i < studentInfo.size(); i++)
    {
        //设置多选框
        ui->tableWidgetStuTable->setColumnWidth(0, 40);
        QTableWidgetItem* check = new QTableWidgetItem;
        check->setCheckState(Qt::Unchecked);
        check->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidgetStuTable->setItem(i, 0, check);

        //id
        ui->tableWidgetStuTable->setColumnWidth(1, 160);
        QString idStr = QString::number(studentInfo[i].id);
        QTableWidgetItem* id = new QTableWidgetItem(idStr);
        id->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidgetStuTable->setItem(i, 1, id);

        //姓名
        ui->tableWidgetStuTable->setColumnWidth(2, 150);
        QTableWidgetItem* name = new QTableWidgetItem(studentInfo[i].name);
        name->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidgetStuTable->setItem(i, 2, name);

        //班级
        QTableWidgetItem* className = new QTableWidgetItem(classIdNameMap[studentInfo[i].classId]);
        className->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidgetStuTable->setItem(i, 3, className);
    }
}

CauseCheck::~CauseCheck()
{
    delete ui;
    ui->tableWidgetStuTable->clearContents();
}

//提交缺勤学生按钮
void CauseCheck::on_pushButtonSubmmit_released()
{
    QVector<LeaveAsk> leaveAsks;
    for (int i = 0; i < ui->tableWidgetStuTable->rowCount(); i++)
    {
        if(ui->tableWidgetStuTable->item(i, 0)->checkState() == Qt::Checked)
        {
            leaveAsks.push_back(LeaveAsk(causeInfo.teacherId, causeInfo.id,
                                         ui->comboBoxCauseWeek->currentText() + "_" + ui->comboBoxCauseTime->currentText(),
                                         ui->tableWidgetStuTable->item(i, 1)->text().toLongLong()));
        }
    }
    //判断每个学生是否有请假
    ErrorClass err = ZeusDao::QueryLeaveAskStatus(leaveAsks);
    if(err.GetCode() != ERRCODE_SUCCESS)
    {
        QMessageBox::warning(this, "警告", err.GetMsg());
        return;
    }

    QVector<CauseCheckModel> causeChecks;
    for (int i = 0; i < leaveAsks.size(); i++)
    {
        causeChecks.push_back(CauseCheckModel(leaveAsks[i].causeId, leaveAsks[i].teachTime, leaveAsks[i].stuId, leaveAsks[i].status == LEAVING_ASK_STAUTS_PASS ? 1 : 2));
    }
    err = ZeusDao::InsertCauseCheckBatch(causeChecks);
    if(err.GetCode() != ERRCODE_SUCCESS)
    {
        QMessageBox::warning(this, "警告", err.GetMsg());
        return;
    }
    QMessageBox::information(this, "成功", "操作成功");
    close();
    return;
}
