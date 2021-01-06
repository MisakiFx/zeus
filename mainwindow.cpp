#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "errorclass.h"
#include "zeusdao.h"
#include "macro.h"
#include <QMessageBox>
#include <QDebug>

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
        break;
    default:
        break;
    }
}

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
    ui->labelClassContent->setText(classOrGrandName);
    ui->labelIdContent->setText(QString::number(userInfo.id));
    ui->labelPhoneContent->setText(userInfo.phoneNumber);

    switch (userInfo.accountType)
    {
    case ACCOUNT_TYPE_STUDENT:
        ui->labelIdTitle->setText("学号:");
        ui->labelClassTitle->setText("班级:");
        ui->labelAccountTypeContent->setText("学生");
        break;
    case ACCOUNT_TYPE_TEACHER:
        ui->labelAccountTypeContent->setText("教师");
        break;
    case ACCOUNT_TYPE_ADMIN:
        ui->labelAccountTypeContent->setText("管理员");
        ui->labelClassTitle->setVisible(false);
        ui->labelClassContent->setVisible(false);
        break;
    default:
        break;
    }
}

//退出登录按钮
void MainWindow::on_actionExit_triggered()
{
    this->hide();
    login.show();

}

void MainWindow::on_actionMyClass_triggered()
{
    //切换分页
    ui->stackedWidget->setCurrentIndex(1);

    //初始化下拉列表
}
