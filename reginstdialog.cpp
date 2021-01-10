#include "reginstdialog.h"
#include "ui_reginstdialog.h"
#include "zeusdao.h"
#include "macro.h"
#include <QListView>
#include <QSqlQueryModel>
#include <QMessageBox>
#include <QDebug>
#include <QSqlError>
ReginstDialog::ReginstDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReginstDialog)
{
    ui->setupUi(this);
    InitComboBox();
    InitLineEdit();
}

ReginstDialog::~ReginstDialog()
{
    delete ui;
}

//初始化下拉框
void ReginstDialog::InitComboBox()
{
    //设置最大显示数
    ui->comboBoxClass->setMaxVisibleItems(10);

    //初始化查询模型
    queryModel = new QSqlQueryModel(this);
}

//初始化文本框
void ReginstDialog::InitLineEdit()
{
    QRegExp regExp("^[0-9]*$");
    QRegExpValidator *pattern= new QRegExpValidator(regExp, this);//创建了一个表达式
    ui->lineEditPhone->setValidator(pattern);
}

//提交按钮槽函数
void ReginstDialog::on_pushButtonSubmit_pressed()
{
    QMessageBox::StandardButton button = QMessageBox::question(this, "警告", "确定提交?");
    if(button == QMessageBox::No)
    {
        return;
    }
    QString name = ui->lineEditName->text();
    QString phoneNumber = ui->lineEditPhone->text();
    QString className = ui->comboBoxClass->currentText();
    QString passwordFirst = ui->lineEditPasswordFirst->text();
    QString passwordSecond = ui->lineEditPasswordSecond->text();
    //基础校验
    if(name == "")
    {
        QMessageBox::warning(this, "警告", "姓名不能为空");
        return;
    }
    if(phoneNumber == "")
    {
        QMessageBox::warning(this, "警告", "电话不能为空");
        return;
    }
    if(className == "")
    {
        QMessageBox::warning(this, "警告", "班级未选择");
        return;
    }
    if(passwordFirst == "" || passwordSecond == "")
    {
        QMessageBox::warning(this, "警告", "密码不能为空");
        return;
    }
    if(passwordFirst != passwordSecond)
    {
        QMessageBox::warning(this, "警告", "两次密码输入不一致");
        return;
    }

    //查询班级代号
    qint64 classId = -1;
    ErrorClass err = ZeusDao::QureyClassIdByName(ui->comboBoxClass->currentText(), classId);
    if(err.GetCode() != 0 || classId == -1)
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
        QMessageBox::warning(this, "警告", QString("错误码:%1\n错误:%2").arg(code).arg(msg));
        return;
    }

    //写库
    UserInfo userInfo(name, ACCOUNT_TYPE_STUDENT, passwordFirst, phoneNumber, classId);
    err = ZeusDao::InsertNewUser(userInfo);
    if(err.GetCode() != ERRCODE_SUCCESS || userInfo.id == 0)
    {
        QMessageBox::warning(this, "注册失败", QString("错误码:%1\n错误:%2").arg(err.GetCode() != 0 ? err.GetCode() : ERRCODE_UNKNOW_ERROR)
                             .arg(err.GetMsg().compare(ERRMSG_DATABASE_EMPEY_ERR) != 0 ? err.GetMsg() : ERRMSG_UNKNOW_MSG));
        return;
    }

    QMessageBox::information(this, "成功", QString("注册成功\n请牢记学号:%1").arg(userInfo.id));
    this->hide();

    return;
}

//刷新下拉框数据
bool ReginstDialog::FlushComboBoxData()
{
    queryModel->setQuery("SELECT name FROM zeus_class_info ORDER BY name;");
    if(queryModel->lastError().isValid())
    {
        return false;
    }
    ui->comboBoxClass->setModel(queryModel);
    ui->comboBoxClass->setCurrentIndex(0);
    return true;
}

//清空所有输入
void ReginstDialog::ClearAllLineEdit()
{
    ui->lineEditName->setText("");
    ui->lineEditPhone->setText("");
    ui->lineEditPasswordFirst->setText("");
    ui->lineEditPasswordSecond->setText("");
}
