#include <QMessageBox>
#include <QLineEdit>
#include <QDebug>
#include <QVector>
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

CauseGrade::CauseGrade(const CauseInfo& causeInfo, QWidget *parent):
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

    //初始化班级信息
    QVector<qint64> classIds;
    QHash<qint64, QString> classIdNameMap;
    if(studentInfo.size() > 0)
    {
        for(int i = 0; i < studentInfo.size(); i++)
        {
            classIds.push_back(studentInfo[i].classId);
        }
        err = ZeusDao::QueryClassNameByIdBatch(classIds, classIdNameMap);
        if(err.GetCode() != ERRCODE_SUCCESS)
        {
            QMessageBox::warning(this, "警告", err.GetMsg());
            return;
        }
    }

    //查询学生成绩
    QHash<qint64, StuCauseRelModel> stuIdCauseMap;
    err = ZeusDao::QueryStuCauseRelByCauseId(causeInfo.id, stuIdCauseMap);
    if(err.GetCode() != ERRCODE_SUCCESS)
    {
        QMessageBox::warning(this, "警告", err.GetMsg());
        return;
    }

    //设置表格行列
    ui->tableWidgetCauseGrand->setRowCount(studentInfo.size());
    ui->tableWidgetCauseGrand->setColumnCount(4);

    //设置表头
    QStringList header;
    header << "学号" << "姓名" << "班级" << "成绩";
    ui->tableWidgetCauseGrand->setHorizontalHeaderLabels(header);

    //初始化表格信息
    for (int i = 0; i < studentInfo.size(); i++)
    {
        //id
        ui->tableWidgetCauseGrand->setColumnWidth(0, 160);
        QString idStr = QString::number(studentInfo[i].id);
        QTableWidgetItem* id = new QTableWidgetItem(idStr);
        id->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        id->setFlags(id->flags() & (~Qt::ItemIsEditable));
        ui->tableWidgetCauseGrand->setItem(i, 0, id);

        //姓名
        ui->tableWidgetCauseGrand->setColumnWidth(1, 150);
        QTableWidgetItem* name = new QTableWidgetItem(studentInfo[i].name);
        name->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        name->setFlags(name->flags() & (~Qt::ItemIsEditable));
        ui->tableWidgetCauseGrand->setItem(i, 1, name);

        //班级
        QTableWidgetItem* className = new QTableWidgetItem(classIdNameMap[studentInfo[i].classId]);
        className->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        className->setFlags(className->flags() & (~Qt::ItemIsEditable));
        ui->tableWidgetCauseGrand->setItem(i, 2, className);

        //设置代理
        QLineEdit * m_edit = new QLineEdit(this);
        ui->tableWidgetCauseGrand->setCellWidget(i, 3, m_edit); //(0,0)项的item设置代理
        QRegExp rx("^(\\d{1,2}|100)$");
        m_edit->setValidator(new QRegExpValidator(rx,m_edit));
        m_edit->setText(QString::number(stuIdCauseMap[studentInfo[i].id].score));
        rowLineEditMap[i] = m_edit;
    }

}

CauseGrade::~CauseGrade()
{
    QHash<int, QLineEdit*>::const_iterator j = rowLineEditMap.constBegin();
    while (j != rowLineEditMap.constEnd())
    {
        delete j.value();
        ++j;
    }
    rowLineEditMap.clear();
    delete ui;
}


//成绩修改提交按钮
void CauseGrade::on_pushButtonSubmit_released()
{
    QMessageBox::StandardButton button = QMessageBox::question(this, "警告", "确定修改?");
    if(button == QMessageBox::No)
    {
        return;
    }
    QVector<StuCauseRelModel> rels;
    for (int i = 0; i < ui->tableWidgetCauseGrand->rowCount(); i++)
    {
        rels.push_back(StuCauseRelModel(ui->tableWidgetCauseGrand->item(i, 0)->text().toLongLong(),
                                        causeInfo.id,
                                        rowLineEditMap[i]->text().toInt(),
                                        ""));
    }
    ErrorClass err = ZeusDao::UpdateStuCauseRelScoreBatch(rels);
    if(err.GetCode() != ERRCODE_SUCCESS)
    {
        QMessageBox::warning(this, "警告", err.GetMsg());
        return;
    }
    QMessageBox::information(this, "成功", "操作成功");
    close();
}
