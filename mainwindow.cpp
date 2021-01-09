#include <QMessageBox>
#include <QDebug>
#include <QSqlError>
#include <QListWidget>
#include <QLineEdit>
#include <QCheckBox>
#include "evalute.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "errorclass.h"
#include "zeusdao.h"
#include "macro.h"
#include "causecheck.h"
#include "causecheckhistory.h"
#include "causegrade.h"

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
    InitAccount(202116097525445712);
    InitCreateCause();

    //初始化界面
    InitMainWindow();
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

//创建课程页
void MainWindow::on_actionCreateClass_triggered()
{
    ui->stackedWidget->setCurrentIndex(2);
    ClearAllCreateCauseInput();
}

//我的课程详情页
void MainWindow::on_actionMyCause_triggered()
{
    //切换分页
    ui->stackedWidget->setCurrentIndex(1);
    if(userInfo.accountType == ACCOUNT_TYPE_STUDENT)
    {
        ui->pushButtonGrade->setVisible(false);
        ui->pushButtonCauseCheck->setVisible(false);
        ui->pushButtonCauseCheckHistory->setVisible(false);
        ui->pushButtonEvalute->setVisible(true);
        ui->pushButtonAbandonChoose->setVisible(true);
        ui->pushButtonChooseCause->setVisible(false);
        ui->labelMyCauseTitle->setText("我的课程");
    } else
    {
        ui->pushButtonGrade->setVisible(true);
        ui->pushButtonCauseCheck->setVisible(true);
        ui->pushButtonCauseCheckHistory->setVisible(true);
        ui->pushButtonEvalute->setVisible(false);
        ui->pushButtonAbandonChoose->setVisible(false);
        ui->pushButtonChooseCause->setVisible(false);
    }
    ClearMyCauseInfo();
    //初始化下拉列表
    ErrorClass err = FlushMyCauseComboBox();
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
    InitMainWindow();
    on_actionUserInfo_triggered();
}

void MainWindow::InitMainWindow()
{
    switch (userInfo.accountType) {
    case ACCOUNT_TYPE_STUDENT:
        ui->actionCreateClass->setVisible(false);
        ui->actionLaeveControl->setVisible(false);
        ui->actionChooseClass->setVisible(true);
        ui->actionLeaveAsk->setVisible(true);
        ui->menuAdmin->menuAction()->setVisible(false);
        break;
    case ACCOUNT_TYPE_TEACHER:
        ui->actionChooseClass->setVisible(false);
        ui->actionLeaveAsk->setVisible(false);
        ui->actionCreateClass->setVisible(true);
        ui->actionLaeveControl->setVisible(true);
        ui->menuAdmin->menuAction()->setVisible(false);
        break;
    case ACCOUNT_TYPE_ADMIN:
        ui->menuClass->menuAction()->setVisible(false);
        ui->menuLeave->menuAction()->setVisible(false);
        ui->menuAdmin->menuAction()->setVisible(true);
        break;
    default:
        break;
    }
}

//初始化我的课程下拉列表
ErrorClass MainWindow::FlushMyCauseComboBox()
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
    ui->comboMyCause->setModel(queryModel);
    if(queryModel->lastError().isValid())
    {
        return ErrorClass(ERRCODE_SERVICE_ERROR, queryModel->lastError().text());
    }
    ui->comboMyCause->setCurrentIndex(0);
    return ErrorClass();
}

//初始化选择课程下拉列表
ErrorClass MainWindow::FlushChooseCauseComboBox()
{
    queryModel->setQuery(QString("SELECT cause_name FROM zeus_cause_info WHERE cause_type = %1").arg(CAUSE_TYPE_OPTIONAL));
    ui->comboMyCause->setModel(queryModel);
    if(queryModel->lastError().isValid())
    {
        return ErrorClass(ERRCODE_SERVICE_ERROR, queryModel->lastError().text());
    }
    ui->comboMyCause->setCurrentIndex(0);
    return ErrorClass();
}

//初始化课程信息
ErrorClass MainWindow::FlushCauseInfo()
{
    QString causeName = ui->comboMyCause->currentText();
    CauseInfo causeInfo(causeName);
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
void MainWindow::on_comboMyCause_currentIndexChanged(const QString &arg1)
{
    if(arg1.compare("") == 0)
    {
        return;
    }
    ErrorClass err = FlushCauseInfo();
    if(err.GetCode() != ERRCODE_SUCCESS)
    {
        QMessageBox::warning(this, "警告", err.GetMsg());
        return;
    }
    if (userInfo.accountType == ACCOUNT_TYPE_STUDENT)
    {
        if (ui->labelCauseTypeContent->text().compare("必修课") == 0)
        {
            ui->pushButtonAbandonChoose->setVisible(false);
        } else
        {
            ui->pushButtonAbandonChoose->setVisible(true);
        }
    }
}

//我的课程页考勤管理功能
void MainWindow::on_pushButtonCauseCheck_released()
{
    QString causeName = ui->comboMyCause->currentText();
    CauseInfo causeInfo(causeName);
    ErrorClass err = ZeusDao::QueryCauseInfoByName(causeInfo);
    if(err.GetCode() != ERRCODE_SUCCESS)
    {
        QMessageBox::warning(this, "警告", err.GetMsg());
        return;
    }
    CauseCheck checkDialog(causeInfo, this);
    checkDialog.exec();
}

//查看课程缺勤历史记录
void MainWindow::on_pushButtonCauseCheckHistory_released()
{
    QString causeName = ui->comboMyCause->currentText();
    CauseInfo causeInfo(causeName);
    ErrorClass err = ZeusDao::QueryCauseInfoByName(causeInfo);
    if(err.GetCode() != ERRCODE_SUCCESS)
    {
        QMessageBox::warning(this, "警告", err.GetMsg());
        return;
    }
    CauseCheckHistory causeCheckHistory(this, causeInfo.id);
    causeCheckHistory.exec();
}

//更新周多选框状态
void MainWindow::WeekStateChanged(int state)
{
    bSelectedWeek = true;
    QString strSelectedData("");
    strSelectedTextWeek.clear();
    int nCount = listWidgetWeek->count();
    for (int i = 0; i < nCount; ++i)
    {
        QListWidgetItem *pItem = listWidgetWeek->item(i);
        QWidget *pWidget = listWidgetWeek->itemWidget(pItem);
        QCheckBox *pCheckBox = (QCheckBox *)pWidget;
        if (pCheckBox->isChecked())
        {
            QString strText = pCheckBox->text();
            strSelectedData.append(strText).append(",");
        }
    }
    if (strSelectedData.endsWith(","))
        strSelectedData.remove(strSelectedData.count() - 1, 1);
    if (!strSelectedData.isEmpty())
    {
        strSelectedTextWeek = strSelectedData;
        lineEditWeek->setText(strSelectedData);
        lineEditWeek->setToolTip(strSelectedData);
    }
    else
    {
        lineEditWeek->clear();
    }
    bSelectedWeek = false;
}

//更新时间多选框状态
void MainWindow::TimeStateChanged(int state)
{
    bSelectedTime = true;
    QString strSelectedData("");
    strSelectedTextTime.clear();
    int nCount = listWidgetTime->count();
    for (int i = 0; i < nCount; ++i)
    {
        QListWidgetItem *pItem = listWidgetTime->item(i);
        QWidget *pWidget = listWidgetTime->itemWidget(pItem);
        QCheckBox *pCheckBox = (QCheckBox *)pWidget;
        if (pCheckBox->isChecked())
        {
            QString strText = pCheckBox->text();
            strSelectedData.append(strText).append(",");
        }
    }
    if (strSelectedData.endsWith(","))
        strSelectedData.remove(strSelectedData.count() - 1, 1);
    if (!strSelectedData.isEmpty())
    {
        strSelectedTextTime = strSelectedData;
        lineEditTime->setText(strSelectedData);
        lineEditTime->setToolTip(strSelectedData);
    }
    else
    {
        lineEditTime->clear();
    }
    bSelectedTime = false;
}

//更新班级多选框状态
void MainWindow::ClassStateChanged(int state)
{
    bSelectedClass = true;
    QString strSelectedData("");
    strSelectedTextClass.clear();
    int nCount = listWidgetClass->count();
    for (int i = 0; i < nCount; ++i)
    {
        QListWidgetItem *pItem = listWidgetClass->item(i);
        QWidget *pWidget = listWidgetClass->itemWidget(pItem);
        QCheckBox *pCheckBox = (QCheckBox *)pWidget;
        if (pCheckBox->isChecked())
        {
            QString strText = pCheckBox->text();
            strSelectedData.append(strText).append(",");
        }
    }
    if (strSelectedData.endsWith(","))
        strSelectedData.remove(strSelectedData.count() - 1, 1);
    if (!strSelectedData.isEmpty())
    {
        strSelectedTextClass = strSelectedData;
        lineEditClass->setText(strSelectedData);
        lineEditClass->setToolTip(strSelectedData);
    }
    else
    {
        lineEditClass->clear();
    }
    bSelectedClass = false;
}

//更新周多选框文案
void MainWindow::WeekTextChanged(const QString &text)
{
    if (!bSelectedWeek)
        lineEditWeek->setText(strSelectedTextWeek);
}

//更新时间多选框文案
void MainWindow::TimeTextChanged(const QString &text)
{
    if (!bSelectedTime)
        lineEditTime->setText(strSelectedTextTime);
}

//更新班级多选框文案
void MainWindow::ClassTextChanged(const QString &text)
{
    if (!bSelectedClass)
        lineEditClass->setText(strSelectedTextClass);
}

//绘制上课周多选框
void MainWindow::DrawWeekStatusComb()
{
    listWidgetWeek = new QListWidget(this);
    lineEditWeek = new QLineEdit(this);
    for (int i = 1; i <= 20; ++i)
    {
        QListWidgetItem *pItem = new QListWidgetItem(listWidgetWeek);
        listWidgetWeek->addItem(pItem);
        pItem->setData(Qt::UserRole, i);
        QCheckBox *pCheckBox = new QCheckBox(this);
        pCheckBox->setText(QStringLiteral("%1").arg(i));
        listWidgetWeek->addItem(pItem);
        listWidgetWeek->setItemWidget(pItem, pCheckBox);
        connect(pCheckBox, SIGNAL(stateChanged(int)), this, SLOT(WeekStateChanged(int)));
    }
    ui->comboCreateCauseWeek->setModel(listWidgetWeek->model());
    ui->comboCreateCauseWeek->setView(listWidgetWeek);
    ui->comboCreateCauseWeek->setLineEdit(lineEditWeek);
    lineEditWeek->setReadOnly(true);                                                                                                                    //ui.comboBox->setEditable(true);
    connect(lineEditWeek, SIGNAL(textChanged(const QString &)), this, SLOT(WeekTextChanged(const QString &)));
}

//绘制上课时间多选框
void MainWindow::DrawTimeStatusComb()
{
    listWidgetTime = new QListWidget(this);
    lineEditTime = new QLineEdit(this);
    for (int i = 1; i <= 5; ++i)
    {
        for (int j = 1; j <= 7; j+=2)
        {
            QListWidgetItem *pItem = new QListWidgetItem(listWidgetTime);
            listWidgetTime->addItem(pItem);
            pItem->setData(Qt::UserRole, i);
            QCheckBox *pCheckBox = new QCheckBox(this);
            pCheckBox->setText(QStringLiteral("%1_%2").arg(i).arg(QString::number(j) + QString::number(j + 1)));
            listWidgetTime->addItem(pItem);
            listWidgetTime->setItemWidget(pItem, pCheckBox);
            connect(pCheckBox, SIGNAL(stateChanged(int)), this, SLOT(TimeStateChanged(int)));
        }
    }
    ui->comboCreateCauseTime->setModel(listWidgetTime->model());
    ui->comboCreateCauseTime->setView(listWidgetTime);
    ui->comboCreateCauseTime->setLineEdit(lineEditTime);
    lineEditTime->setReadOnly(true);                                                                                                                    //ui.comboBox->setEditable(true);
    connect(lineEditTime, SIGNAL(textChanged(const QString &)), this, SLOT(TimeTextChanged(const QString &)));
}

//绘制上课班级多选框
void MainWindow::DrawClassStatusComb()
{
    listWidgetClass = new QListWidget(this);
    lineEditClass = new QLineEdit(this);
    QStringList names;
    ErrorClass err = ZeusDao::QueryAllClassName(names);
    if(err.GetCode() != ERRCODE_SUCCESS)
    {
        QMessageBox::warning(this, "警告", err.GetMsg());
        return;
    }
    for (int i = 0; i < names.size(); ++i)
    {
        QListWidgetItem *pItem = new QListWidgetItem(listWidgetClass);
        listWidgetClass->addItem(pItem);
        pItem->setData(Qt::UserRole, i);
        QCheckBox *pCheckBox = new QCheckBox(this);
        pCheckBox->setText(QStringLiteral("%1").arg(names[i]));
        listWidgetClass->addItem(pItem);
        listWidgetClass->setItemWidget(pItem, pCheckBox);
        connect(pCheckBox, SIGNAL(stateChanged(int)), this, SLOT(ClassStateChanged(int)));
    }
    ui->comboCreateCauseClass->setModel(listWidgetClass->model());
    ui->comboCreateCauseClass->setView(listWidgetClass);
    ui->comboCreateCauseClass->setLineEdit(lineEditClass);
    lineEditClass->setReadOnly(true);                                                                                                                    //ui.comboBox->setEditable(true);
    connect(lineEditClass, SIGNAL(textChanged(const QString &)), this, SLOT(ClassTextChanged(const QString &)));
}

//清空所有复选框
void MainWindow::ClearAllCreateCauseInput()
{
    //时间
    int nCount = listWidgetTime->count();
    for (int i = 0; i < nCount; ++i)
    {
        QListWidgetItem *pItem = listWidgetTime->item(i);
        QWidget *pWidget = listWidgetTime->itemWidget(pItem);
        QCheckBox *pCheckBox = (QCheckBox *)pWidget;
        if (pCheckBox->isChecked())
        {
            pCheckBox->setCheckState(Qt::Unchecked);
        }
    }
    //周次
    nCount = listWidgetWeek->count();
    for (int i = 0; i < nCount; ++i)
    {
        QListWidgetItem *pItem = listWidgetWeek->item(i);
        QWidget *pWidget = listWidgetWeek->itemWidget(pItem);
        QCheckBox *pCheckBox = (QCheckBox *)pWidget;
        if (pCheckBox->isChecked())
        {
            pCheckBox->setCheckState(Qt::Unchecked);
        }
    }
    //课程
    nCount = listWidgetClass->count();
    for (int i = 0; i < nCount; ++i)
    {
        QListWidgetItem *pItem = listWidgetClass->item(i);
        QWidget *pWidget = listWidgetClass->itemWidget(pItem);
        QCheckBox *pCheckBox = (QCheckBox *)pWidget;
        if (pCheckBox->isChecked())
        {
            pCheckBox->setCheckState(Qt::Unchecked);
        }
    }

    ui->lineEditCreateCauseNameContent->clear();
    ui->lineEditCreateCauseScoreContent->clear();
    ui->lineEditCreateCauseClassroomContent->clear();
    ui->comboCreateCauseType->setCurrentIndex(0);
}

//创建课程提交按钮
void MainWindow::on_pushButtonSubbmit_released()
{
    //入参校验
    if(ui->lineEditCreateCauseNameContent->text().compare("") == 0)
    {
        QMessageBox::warning(this, "警告", "课程名称不能为空");
        return;
    }
    if(ui->comboCreateCauseType->currentText().compare("") == 0)
    {
        QMessageBox::warning(this, "警告", "课程类型不能为空");
        return;
    }
    if(ui->lineEditCreateCauseScoreContent->text().compare("") == 0)
    {
        QMessageBox::warning(this, "警告", "课程学分不能为空");
        return;
    }
    if(ui->lineEditCreateCauseClassroomContent->text().compare("") == 0)
    {
        QMessageBox::warning(this, "警告", "课程教室不能为空");
        return;
    }
    if(ui->comboCreateCauseTime->currentText().compare("") == 0)
    {
        QMessageBox::warning(this, "警告", "上课时间不能为空");
        return;
    }
    if(ui->comboCreateCauseWeek->currentText().compare("") == 0)
    {
        QMessageBox::warning(this, "警告", "上课周次不能为空");
        return;
    }
    if(ui->comboCreateCauseType->currentText().compare("必修课") == 0 &&
            ui->comboCreateCauseClass->currentText().compare("") == 0)
    {
        QMessageBox::warning(this, "警告", "必修课上课班级不能为空");
        return;
    }
    //创建课程
    CauseInfo causeInfo(ui->lineEditCreateCauseNameContent->text(),
                       ui->comboCreateCauseType->currentText().compare("必修课") == 0 ? CAUSE_TYPE_REQUIRED : CAUSE_TYPE_OPTIONAL,
                       ui->lineEditCreateCauseScoreContent->text().toInt(),
                       userInfo.id,
                       ui->comboCreateCauseTime->currentText().split(","),
                       ui->comboCreateCauseWeek->currentText().split(","),
                       ui->lineEditCreateCauseClassroomContent->text(),
                       ui->comboCreateCauseClass->currentText().split(","));
    ErrorClass err = ZeusDao::CreateNewClassInfo(causeInfo);
    if (err.GetCode() != ERRCODE_SUCCESS)
    {
        QMessageBox::warning(this, "警告", err.GetMsg());
        return;
    }
    QMessageBox::information(this, "成功", "课程新建成功");
    ClearAllCreateCauseInput();
    return;
}

//初始化创建课程页面
void MainWindow::InitCreateCause()
{
    DrawWeekStatusComb();
    DrawTimeStatusComb();
    DrawClassStatusComb();
    ui->comboCreateCauseType->addItem("必修课");
    ui->comboCreateCauseType->addItem("选修课");
    QRegExp regExp("^[0-9]*$");
    QRegExpValidator *pattern= new QRegExpValidator(regExp, this);//创建了一个表达式
    ui->lineEditCreateCauseScoreContent->setValidator(pattern);
}

//创建课程页切换课程类型逻辑
void MainWindow::on_comboCreateCauseType_currentIndexChanged(int index)
{
    if(ui->comboCreateCauseType->currentText() == "必修课")
    {
        ui->layoutCreateCauseClass->setVisible(true);
    } else
    {
        ui->layoutCreateCauseClass->setVisible(false);
    }
}

//成绩管理页面
void MainWindow::on_pushButtonGrade_released()
{
    QString causeName = ui->comboMyCause->currentText();
    CauseInfo causeInfo(causeName);
    ErrorClass err = ZeusDao::QueryCauseInfoByName(causeInfo);
    if(err.GetCode() != ERRCODE_SUCCESS)
    {
        QMessageBox::warning(this, "警告", err.GetMsg());
        return;
    }
    CauseGrade gradeDialog(causeInfo, this);
    gradeDialog.exec();
}

//评教页面
void MainWindow::on_pushButtonEvalute_released()
{
    QString causeName = ui->comboMyCause->currentText();
    CauseInfo causeInfo(causeName);
    ErrorClass err = ZeusDao::QueryCauseInfoByName(causeInfo);
    if(err.GetCode() != ERRCODE_SUCCESS)
    {
        QMessageBox::warning(this, "警告", err.GetMsg());
        return;
    }
    Evalute evalute(causeInfo, userInfo, this);
    evalute.exec();
}

//退选功能
void MainWindow::on_pushButtonAbandonChoose_released()
{
    QString causeName = ui->comboMyCause->currentText();
    CauseInfo causeInfo(causeName);
    ErrorClass err = ZeusDao::QueryCauseInfoByName(causeInfo);
    if(err.GetCode() != ERRCODE_SUCCESS)
    {
        QMessageBox::warning(this, "警告", err.GetMsg());
        return;
    }
    if (causeInfo.causeType == CAUSE_TYPE_REQUIRED)
    {
        QMessageBox::warning(this, "警告", "必修课不能退选");
        return;
    }
    StuCauseRelModel rel(userInfo.id, causeInfo.id);
    err = ZeusDao::DeleteStuCauseRel(rel);
    if(err.GetCode() != ERRCODE_SUCCESS)
    {
        QMessageBox::warning(this, "警告", err.GetMsg());
        return;
    }
    QMessageBox::information(this, "成功", "退选成功");
    FlushMyCauseComboBox();
    ClearMyCauseInfo();
    ui->comboMyCause->setCurrentIndex(0);
}

//清空我的课程页面
void MainWindow::ClearMyCauseInfo()
{
    ui->labelCauseNameContent->clear();
    ui->labelCauseTypeContent->clear();
    ui->labelCauseScoreContent->clear();
    ui->labelCauseTeacherContent->clear();
    ui->labelCauseTimeContent->clear();
    ui->labelCauseWeekContent->clear();
    ui->labelCauseClassroomContent->clear();
}

//学生选课界面
void MainWindow::on_actionChooseClass_triggered()
{
    //切换分页
    ui->stackedWidget->setCurrentIndex(1);
    ui->pushButtonGrade->setVisible(false);
    ui->pushButtonCauseCheck->setVisible(false);
    ui->pushButtonCauseCheckHistory->setVisible(false);
    ui->pushButtonAbandonChoose->setVisible(false);
    ui->pushButtonEvalute->setVisible(false);
    ui->pushButtonChooseCause->setVisible(true);
    ui->labelMyCauseTitle->setText("选择课程");
    ClearMyCauseInfo();
    //初始化下拉列表
    ErrorClass err = FlushChooseCauseComboBox();
    if(err.GetCode() != ERRCODE_SUCCESS)
    {
        QMessageBox::warning(this, "警告", err.GetMsg());
        return;
    }

    return;
}

//选择课程按钮
void MainWindow::on_pushButtonChooseCause_released()
{
    QString causeName = ui->comboMyCause->currentText();
    CauseInfo causeInfo(causeName);
    ErrorClass err = ZeusDao::QueryCauseInfoByName(causeInfo);
    if(err.GetCode() != ERRCODE_SUCCESS)
    {
        QMessageBox::warning(this, "警告", err.GetMsg());
        return;
    }
    StuCauseRelModel rel(userInfo.id, causeInfo.id);
    err = ZeusDao::InsertStuCauseRel(rel);
    if(err.GetCode() != ERRCODE_SUCCESS)
    {
        QMessageBox::warning(this, "警告", "课程已经选过,不能重复选择");
        return;
    }
    QMessageBox::information(this, "成功", "选课成功");
}
