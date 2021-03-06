#include <QSqlDatabase>
#include <QDebug>
#include <QMessageBox>
#include <QSqlError>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "loginwindow.h"
#include "zeusdao.h"
#include "errorclass.h"
#include "macro.h"

MainWindow::MainWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //登录页面逻辑
    Login();

    //连接数据库
    ErrorClass err = ZeusDao::InitDao();
    if (err.GetCode() != ERRCODE_SUCCESS)
    {
        QMessageBox::warning(this, "数据库连接失败", QString("错误码:%1,错误信息:%2").arg(err.GetCode()).arg(err.GetMsg()));
        this->close();
        return;
    }

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
    qDebug() << userInfo.id << ";" << userInfo.name << "," << userInfo.accountType << ";" << userInfo.phoneNumber << ";" << userInfo.classId << ";" << userInfo.grandId;
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
}
