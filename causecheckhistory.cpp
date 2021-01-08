#include <QSqlQueryModel>
#include "causecheckhistory.h"
#include "ui_causecheckhistory.h"

CauseCheckHistory::CauseCheckHistory(QWidget *parent, qint64 causeId) :
    QDialog(parent),
    ui(new Ui::CauseCheckHistory)
{
    ui->setupUi(this);
    QSqlQueryModel* model = new QSqlQueryModel(this);
    model->setQuery(QString("SELECT userInfo.id AS '学号', userInfo.name AS '姓名', classInfo.name AS '班级', causeCheckInfo.teach_time AS '上课时间',\
                            CASE causeCheckInfo.is_leave\
                                WHEN 1 THEN '是'\
                                WHEN 2 THEN '否'\
                            END AS '是否请假' \
                            FROM zeus_cause_check_info causeCheckInfo \
                            INNER JOIN zeus_user_info userInfo ON causeCheckInfo.stu_id = userInfo.id\
                            INNER JOIN zeus_class_info classInfo ON userInfo.class_id = classInfo.id\
                            WHERE causeCheckInfo.cause_id = 1").arg(causeId));
    ui->tableViewCauseCheck->setModel(model);
    QHeaderView* headerView = ui->tableViewCauseCheck->horizontalHeader();
    headerView->setSectionResizeMode(QHeaderView::Stretch);
}

CauseCheckHistory::~CauseCheckHistory()
{
    delete ui;
}
