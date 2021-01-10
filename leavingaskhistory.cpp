#include <QSqlQueryModel>
#include "leavingaskhistory.h"
#include "ui_leavingaskhistory.h"

LeavingAskHistory::LeavingAskHistory(qint64 studentId, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LeavingAskHistory)
{
    ui->setupUi(this);
    QSqlQueryModel* model = new QSqlQueryModel(this);
    model->setQuery(QString("SELECT leaveAsk.cause_name AS '课程名称', userInfo.name as '教师名称', leaveAsk.teach_time AS '上课时间', leaveAsk.message AS '请假说明',\
                            CASE leaveAsk.`status`\
                            WHEN 1 THEN '申请中'\
                            WHEN 2 THEN '已通过'\
                            WHEN 3 THEN '已拒绝'\
                            END AS '审批状态'\
                            FROM zeus_user_info userInfo\
                            INNER JOIN zeus_leave_ask leaveAsk ON userInfo.id = leaveAsk.teacher_id\
                            WHERE leaveAsk.stu_id = %1 ORDER BY leaveAsk.teach_time").arg(studentId));
    ui->tableViewLeaveAskHistory->setModel(model);
    QHeaderView* headerView = ui->tableViewLeaveAskHistory->horizontalHeader();
    headerView->setSectionResizeMode(QHeaderView::Stretch);
}

LeavingAskHistory::~LeavingAskHistory()
{
    delete ui;
}
