#include "loginwindow.h"
#include "ui_loginwindow.h"
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlQuery>
#include <QMessageBox>
#include <QRegExpValidator>
#include "reginstdialog.h"
#include "errorclass.h"
#include "macro.h"
#include "zeusdao.h"

LoginWindow::LoginWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LoginWindow),
    reginst(nullptr)
{
    ui->setupUi(this);
    InitLineEdit();
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

//登录按钮槽函数
void LoginWindow::on_pushButtonLogin_pressed()
{
    QString account = ui->lineEditAccount->text();
    QString password = ui->lineEditPassword->text();
    if(account == "" || password == "")
    {
        QMessageBox::warning(this, "警告", "账号或密码不能为空");
        return;
    }
    bool ok;
    qint64 accountInt64 = account.toLongLong(&ok, 10);
    if(!ok)
    {
        QMessageBox::warning(this, "警告", "学号只能是数字，输入不合法");
        return;
    }
    ErrorClass err = ZeusDao::IsPasswordCorrect(accountInt64, password);
    if(err.GetCode() != ERRCODE_SUCCESS)
    {
        QMessageBox::warning(this, "警告", QString("错误码:%1\n错误:%2").arg(err.GetCode()).arg(err.GetMsg()));
        return;
    }
    qDebug() << "登录成功";
    emit loginSuccess(accountInt64);
    return;
}

//注册按钮槽函数
void LoginWindow::on_pushButtonReginst_pressed()
{
    if (reginst == nullptr)
    {
        reginst = new ReginstDialog(this);
    }
    reginst->ClearAllLineEdit();
    bool isSuccess = reginst->FlushComboBoxData();
    if(!isSuccess)
    {
        QMessageBox::warning(this, "警告", "数据初始化失败，请重试");
        return;
    }
    reginst->show();
}

//初始化文本框
void LoginWindow::InitLineEdit()
{
    QRegExp regExp("^[0-9]*$");
    QRegExpValidator *pattern= new QRegExpValidator(regExp, this);//创建了一个表达式
    ui->lineEditAccount->setValidator(pattern);
}

//清空所有文本框
void LoginWindow::ClearAllText()
{
    ui->lineEditAccount->setText("");
    ui->lineEditPassword->setText("");
}
