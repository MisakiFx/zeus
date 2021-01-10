#include <QVector>
#include <QMessageBox>
#include "leaveaskcontrol.h"
#include "ui_leaveaskcontrol.h"
#include "errorclass.h"
#include "zeusdao.h"
#include "macro.h"

LeaveAskControl::LeaveAskControl(qint64 teacherId, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LeaveAskControl)
{
    ui->setupUi(this);

    //初始化数据
    QVector<LeaveAsk> leaveAsks;
    ErrorClass err = ZeusDao::QueryAllLeaveAskByTeacherId(teacherId, leaveAsks);
    if (err.GetCode() != ERRCODE_SUCCESS)
    {
        QMessageBox::warning(this, "警告", err.GetMsg());
        return;
    }

    ui->tableWidgetLeaveAskControl->setRowCount(leaveAsks.size());
    ui->tableWidgetLeaveAskControl->setColumnCount(6);

    //设置表头
    QStringList header;
    header << "课程名称" << "学生姓名" << "学生学号" << "上课时间" << "请假说明" << "审批状态";
    ui->tableWidgetLeaveAskControl->setHorizontalHeaderLabels(header);

    //初始化表格信息
    for (int i = 0; i < leaveAsks.size(); i++)
    {
        //课程名称
        ui->tableWidgetLeaveAskControl->setColumnWidth(0, 150);
        QTableWidgetItem* id = new QTableWidgetItem(leaveAsks[i].causeName);
        id->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        id->setFlags(id->flags() & (~Qt::ItemIsEditable));
        ui->tableWidgetLeaveAskControl->setItem(i, 0, id);

        //学生姓名
        ui->tableWidgetLeaveAskControl->setColumnWidth(1, 150);
        QTableWidgetItem* stuName = new QTableWidgetItem(leaveAsks[i].stuName);
        stuName->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        stuName->setFlags(stuName->flags() & (~Qt::ItemIsEditable));
        ui->tableWidgetLeaveAskControl->setItem(i, 1, stuName);

        //学生学号
        ui->tableWidgetLeaveAskControl->setColumnWidth(2, 150);
        QString stuIdStr = QString::number(leaveAsks[i].stuId);
        QTableWidgetItem* stuId = new QTableWidgetItem(stuIdStr);
        stuId->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        stuId->setFlags(stuId->flags() & (~Qt::ItemIsEditable));
        ui->tableWidgetLeaveAskControl->setItem(i, 2, stuId);

        //上课时间
        ui->tableWidgetLeaveAskControl->setColumnWidth(3, 150);
        QTableWidgetItem* teachTime = new QTableWidgetItem(leaveAsks[i].teachTime);
        teachTime->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        teachTime->setFlags(teachTime->flags() & (~Qt::ItemIsEditable));
        ui->tableWidgetLeaveAskControl->setItem(i, 3, teachTime);

        //请假信息
        ui->tableWidgetLeaveAskControl->setColumnWidth(4, 150);
        QTableWidgetItem* message = new QTableWidgetItem(leaveAsks[i].message);
        message->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        message->setFlags(message->flags() & (~Qt::ItemIsEditable));
        ui->tableWidgetLeaveAskControl->setItem(i, 4, message);

        //审批状态
        if (leaveAsks[i].status == LEAVING_ASK_STAUTS_ASKING)
        {
            //设置代理
            QComboBox* comboxStatus = new QComboBox(this);
            ui->tableWidgetLeaveAskControl->setCellWidget(i, 5, comboxStatus);
            comboxStatus->addItem("审批中");
            comboxStatus->addItem("已通过");
            comboxStatus->addItem("已驳回");
            comboxStatus->setCurrentIndex(0);
            idComboxMap[leaveAsks[i].id] = comboxStatus;
        }else
        {
            ui->tableWidgetLeaveAskControl->setColumnWidth(5, 150);
            QString statusStr;
            if (leaveAsks[i].status == LEAVING_ASK_STAUTS_ASKING)
            {
                statusStr = "审批中";
            } else if(leaveAsks[i].status == LEAVING_ASK_STAUTS_PASS)
            {
                statusStr = "已通过";
            } else if(leaveAsks[i].status == LEAVING_ASK_STAUTS_REFUSE)
            {
                statusStr = "已驳回";
            }
            QTableWidgetItem* status = new QTableWidgetItem(statusStr);
            status->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            status->setFlags(status->flags() & (~Qt::ItemIsEditable));
            ui->tableWidgetLeaveAskControl->setItem(i, 5, status);
        }
    }
}

LeaveAskControl::~LeaveAskControl()
{
    delete ui;
}
