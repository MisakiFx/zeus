#include <QMessageBox>
#include <QDebug>
#include <QSqlError>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "errorclass.h"
#include "zeusdao.h"
#include "macro.h"
#include "causecheck.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //登录页面逻辑
    //Login();

    //连接数据库
    ErrorClass err = ZeusDao::InitDao();
    if (err.GetCode() != ERRCODE_SUCCESS)
    {
        QMessageBox::warning(this, "数据库连接失败", QString("错误码:%1,错误信息:%2").arg(err.GetCode()).arg(err.GetMsg()));
        this->close();
        return;
    }
    
    //初始化查询模型
    queryModel = new QSqlQueryModel(this);

    //普通测试
    this->show();
    InitAccount(2);

    //初始化界面
    InitMenu();
    on_actionUserInfo_triggered();

    //设置信号
    connect(&login, &LoginWindow::loginSuccess, this, &MainWindow::DealLoginSlog);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//用户信息页面
void MainWindow::on_actionUserInfo_triggered()
{
    //切换分页
    ui->stackedWidget->setCurrentIndex(0);

    QString classOrGrandName = "";
    if(userInfo.accountType == ACCOUNT_TYPE_STUDENT)
    {
        ErrorClass err = ZeusDao::QueryClassNameById(userInfo.classId, classOrGrandName);
        if(err.GetCode() != ERRCODE_SUCCESS)
        {
            QMessageBox::warning(this, "初始化用户信息失败", QString("错误码:%1\n错误信息:%2").arg(err.GetCode()).arg(err.GetMsg()));
        }
    }else if(userInfo.accountType == ACCOUNT_TYPE_TEACHER)
    {
        ErrorClass err = ZeusDao::QueryGrandNameById(userInfo.grandId, classOrGrandName);
        if(err.GetCode() != ERRCODE_SUCCESS)
        {
            QMessageBox::warning(this, "初始化用户信息失败", QString("错误码:%1\n错误信息:%2").arg(err.GetCode()).arg(err.GetMsg()));
        }
    }

    //设置内容
    ui->labelMyClassContent->setText(classOrGrandName);
    ui->labelIdContent->setText(QString::number(userInfo.id));
    ui->labelPhoneContent->setText(userInfo.phoneNumber);
    ui->labelAccountNameContent->setText(userInfo.name);
    switch (userInfo.accountType)
    {
    case ACCOUNT_TYPE_STUDENT:
        ui->labelIdTitle->setText("学号:");
        ui->labelMyClassTitle->setText("班级:");
        ui->labelAccountTypeContent->setText("学生");
        break;
    case ACCOUNT_TYPE_TEACHER:
        ui->labelAccountTypeContent->setText("教师");
        break;
    case ACCOUNT_TYPE_ADMIN:
        ui->labelAccountTypeContent->setText("管理员");
        ui->labelMyClassTitle->setVisible(false);
        ui->labelMyClassContent->setVisible(false);
        break;
    default:
        break;
    }
}

//我的课程详情页
void MainWindow::on_actionMyClass_triggered()
{
    //切换分页
    ui->stackedWidget->setCurrentIndex(1);
    if(userInfo.accountType == ACCOUNT_TYPE_STUDENT)
    {
        ui->pushButtonGrade->setVisible(false);
        ui->pushButtonCauseCheck->setVisible(false);
    } else
    {
        ui->pushButtonEvalute->setVisible(false);
        ui->pushButtonAbandonChoose->setVisible(false);
    }
    //初始化下拉列表
    ErrorClass err = FlushMyClassComboBox();
    if(err.GetCode() != ERRCODE_SUCCESS)
    {
        QMessageBox::warning(this, "警告", err.GetMsg());
        return;
    }

    return;
}

//退出按钮
void MainWindow::on_actionExit_triggered()
{
    this->hide();
    login.show();

}

//登录成功初始化用户信息
void MainWindow::InitAccount(qint64 accountId)
{
    userInfo.id = accountId;
    ErrorClass err = ZeusDao::QueryUserInfoById(userInfo);
    if(err.GetCode() != ERRCODE_SUCCESS || userInfo.name.compare("") == 0)
    {
        QString msg = err.GetMsg();
        int code = err.GetCode();
        if(code == 0)
        {
            code = ERRCODE_UNKNOW_ERROR;
        }
        if(msg.compare(ERRMSG_DATABASE_EMPEY_ERR) == 0 || msg.compare("") == 0)
        {
            msg = ERRMSG_UNKNOW_MSG;
        }
        QMessageBox::warning(this, "错误", QString("错误码:%1\n错误:%2").arg(code).arg(msg));
    }
}

//登录
void MainWindow::Login()
{
    this->hide();
    login.show();
}

//登录成功信号处理
void MainWindow::DealLoginSlog(qint64 accountId)
{
    //关闭登录窗口打开主窗口
    login.hide();
    this->show();

    //初始化用户数据
    InitAccount(accountId);
    on_actionUserInfo_triggered();
}

void MainWindow::InitMenu()
{
    switch (userInfo.accountType) {
    case ACCOUNT_TYPE_STUDENT:
        ui->actionCreateClass->setVisible(false);
        ui->actionLaeveControl->setVisible(false);
        ui->menuAdmin->menuAction()->setVisible(false);
        break;
    case ACCOUNT_TYPE_TEACHER:
        ui->actionChooseClass->setVisible(false);
        ui->actionLeaveAsk->setVisible(false);
        ui->menuAdmin->menuAction()->setVisible(false);
        break;
    case ACCOUNT_TYPE_ADMIN:
        ui->menuClass->menuAction()->setVisible(false);
        ui->menuLeave->menuAction()->setVisible(false);
        break;
    default:
        break;
    }
}

//初始化下拉列表
ErrorClass MainWindow::FlushMyClassComboBox()
{
    if(userInfo.accountType == ACCOUNT_TYPE_STUDENT)
    {
        queryModel->setQuery(QString("SELECT cause_name From zeus_cause_info cause_info \
                                     INNER JOIN zeus_stu_cause_rel cause_rel ON cause_info.id = cause_rel.cause_id \
                                     WHERE cause_rel.stu_id = %1").arg(userInfo.id));
    } else
    {
        queryModel->setQuery(QString("SELECT cause_name From zeus_cause_info WHERE teacher_id = %1").arg(userInfo.id));
    }
    ui->comboMyClass->setModel(queryModel);
    if(queryModel->lastError().isValid())
    {
        return ErrorClass(ERRCODE_SERVICE_ERROR, queryModel->lastError().text());
    }
    ui->comboMyClass->setCurrentIndex(0);
    return ErrorClass();
}

//初始化课程信息
ErrorClass MainWindow::FlushMyClassInfo()
{
    QString className = ui->comboMyClass->currentText();
    CauseInfo causeInfo(className);
    ErrorClass err = ZeusDao::QueryCauseInfoByName(causeInfo);
    if(err.GetCode() != ERRCODE_SUCCESS)
    {
        return err;
    }

    UserInfo teacher(causeInfo.teacherId);
    err = ZeusDao::QueryUserInfoById(teacher);
    if(err.GetCode() != ERRCODE_SUCCESS)
    {
        return err;
    }

    QString causeTime;
    for(int i = 0; i < causeInfo.causeTime.size(); i++)
    {
        if(i == 0)
        {
            causeTime += causeInfo.causeTime[i];
        } else
        {
            causeTime += QString(",%1").arg(causeInfo.causeTime[i]);
        }
    }
    QString causeWeek;
    for (int i = 0; i < causeInfo.causeWeek.size(); i++)
    {
        if(i == 0)
        {
            causeWeek += causeInfo.causeWeek[i];
        } else
        {
            causeWeek += QString(",%1").arg(causeInfo.causeWeek[i]);
        }
    }

    ui->labelCauseNameContent->setText(causeInfo.causeName);
    ui->labelCauseTypeContent->setText(causeInfo.causeType == CAUSE_TYPE_REQUIRED ? "必修课" : "选修课");
    ui->labelCauseScoreContent->setNum(causeInfo.score);
    ui->labelCauseTeacherContent->setText(teacher.name);
    ui->labelCauseTimeContent->setText(causeTime);
    ui->labelCauseWeekContent->setText(causeWeek);
    ui->labelCauseClassroomContent->setText(causeInfo.classroom);
    return ErrorClass();
}

//我的课程页课程切换
void MainWindow::on_comboMyClass_currentIndexChanged(const QString &arg1)
{
    if(arg1.compare("") == 0)
    {
        return;
    }
    ErrorClass err = FlushMyClassInfo();
    if(err.GetCode() != ERRCODE_SUCCESS)
    {
        QMessageBox::warning(this, "警告", err.GetMsg());
        return;
    }
}

void MainWindow::on_pushButtonCauseCheck_released()
{
    CauseCheck checkDialog(this);
    checkDialog.exec();
}
