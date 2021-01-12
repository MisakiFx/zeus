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
#include "leavingaskhistory.h"
#include "causegrade.h"
#include "leaveaskcontrol.h"

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow),
        isChooseTime(false),
        isEvaluteTime(false)
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


    //初始化查询模型
    queryModel = new QSqlQueryModel(this);
    tableModel = new QSqlTableModel(this);
    readOnlyDelegate = new ReadOnlyDelegate(this);

    //普通测试
    //this->show();
    //InitAccount(3);

    //初始化界面
    InitCreateCause();
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
        ui->labelMyClassTitle->setVisible(true);
        ui->labelMyClassContent->setVisible(true);
        ui->labelIdTitle->setText("学号:");
        ui->labelMyClassTitle->setText("班级:");
        ui->labelAccountTypeContent->setText("学生");
        break;
    case ACCOUNT_TYPE_TEACHER:
        ui->labelMyClassTitle->setVisible(true);
        ui->labelMyClassContent->setVisible(true);
        ui->labelMyClassTitle->setText("年级组:");
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
void MainWindow::on_actionCreateCause_triggered()
{
    ui->stackedWidget->setCurrentIndex(2);
    ClearAllCreateCauseInput();
}

//我的课程详情页
void MainWindow::on_actionMyCause_triggered()
{
    isChooseCause = false;

    //切换分页
    ui->stackedWidget->setCurrentIndex(1);

    //初始化选课和评教状态
    ZeusDao::QueryIsTime(isChooseTime, isEvaluteTime);

    if(userInfo.accountType == ACCOUNT_TYPE_STUDENT)
    {
        ui->pushButtonGrade->setVisible(false);
        ui->pushButtonCauseCheck->setVisible(false);
        ui->pushButtonCauseCheckHistory->setVisible(false);
        if(isEvaluteTime)
        {
            ui->pushButtonEvalute->setVisible(true);
        } else
        {
            ui->pushButtonEvalute->setVisible(false);
        }
        if(isChooseTime)
        {
            ui->pushButtonAbandonChoose->setVisible(true);
        } else
        {
            ui->pushButtonAbandonChoose->setVisible(false);
        }
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
    on_actionUserInfo_triggered();
    InitMainWindow();
}

void MainWindow::InitMainWindow()
{
    //初始化选课和评教状态
    ZeusDao::QueryIsTime(isChooseTime, isEvaluteTime);

    QVector<LeaveAsk> leaveAsks;
    switch (userInfo.accountType) {
    case ACCOUNT_TYPE_STUDENT:
        ui->menuClass->menuAction()->setVisible(true);
        ui->menuLeave->menuAction()->setVisible(true);
        ui->actionCreateCause->setVisible(false);
        ui->actionLaeveControl->setVisible(false);
        ui->actionLeaveAsk->setVisible(true);
        ui->menuAdmin->menuAction()->setVisible(false);
        if (isChooseTime)
        {
            ui->actionChooseClass->setVisible(true);
            QMessageBox::information(this, "提示", "选课时间到，请快去选课");
        } else
        {
            ui->actionChooseClass->setVisible(false);
        }
        if (isEvaluteTime)
        {
            QMessageBox::information(this, "提示", "评教时间到，请快去评教");
        }
        break;
    case ACCOUNT_TYPE_TEACHER:
        ui->menuClass->menuAction()->setVisible(true);
        ui->menuLeave->menuAction()->setVisible(true);
        ZeusDao::QueryLeaveAskByTeacherId(userInfo.id, leaveAsks);
        if (leaveAsks.size() > 0)
        {
            QMessageBox::information(this, "提示", "有未审批的请假申请，请审批");
        }
        ui->actionChooseClass->setVisible(false);
        ui->actionLeaveAsk->setVisible(false);
        ui->actionCreateCause->setVisible(true);
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
    if(causeName.compare("") == 0)
    {
        return ErrorClass();
    }
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
            if (isChooseTime && !isChooseCause)
            {
                ui->pushButtonAbandonChoose->setVisible(true);
            } else
            {
                ui->pushButtonAbandonChoose->setVisible(false);
            }
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
void MainWindow::DrawCauseClassComb()
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
void MainWindow::on_pushButtonCreateCauseSubmit_released()
{
    QMessageBox::StandardButton button = QMessageBox::question(this, "警告", "确定提交?");
    if(button == QMessageBox::No)
    {
        return;
    }
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
    DrawCauseClassComb();
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
    QMessageBox::StandardButton button = QMessageBox::question(this, "警告", "确定退选?");
    if(button == QMessageBox::No)
    {
        return;
    }
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
    isChooseCause = true;

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
    QMessageBox::StandardButton button = QMessageBox::question(this, "警告", "确定选课?");
    if(button == QMessageBox::No)
    {
        return;
    }
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

//清空请假申请页面
void MainWindow::ClearLeaveAskWindow()
{
    ui->comboBoxLeaveAskWeek->setCurrentIndex(0);
    ui->comboBoxLeaveAskTime->setCurrentIndex(0);
    ui->textEditLeaveAskMsg->clear();
}

//刷新请假申请页面的
ErrorClass MainWindow::FlushLeaveAskMyCauseCombo()
{
    queryModel->setQuery(QString("SELECT cause_name From zeus_cause_info cause_info \
                                 INNER JOIN zeus_stu_cause_rel cause_rel ON cause_info.id = cause_rel.cause_id \
                    WHERE cause_rel.stu_id = %1").arg(userInfo.id));
    ui->comboBoxLeaveAskMyCause->setModel(queryModel);
    if(queryModel->lastError().isValid())
    {
        return ErrorClass(ERRCODE_SERVICE_ERROR, queryModel->lastError().text());
    }
    ui->comboBoxLeaveAskMyCause->setCurrentIndex(0);
    return ErrorClass();
}

//初始化请假申请页面
void MainWindow::InitLeaveAsk()
{
    //周单选框
    for (int i = 1; i <= 20; ++i)
    {
        ui->comboBoxLeaveAskWeek->addItem(QString("%1").arg(i));
    }

    //时间单选框
    for (int i = 1; i <= 5; i++)
    {
        for (int j = 1; j <= 7; j += 2)
        {
            ui->comboBoxLeaveAskTime->addItem(QString("%1_%2%3").arg(i).arg(j).arg(j+1));
        }
    }
}

//请假申请页面
void MainWindow::on_actionLeaveAsk_triggered()
{
    //切换分页
    ui->stackedWidget->setCurrentIndex(3);

    //刷新页面
    ClearLeaveAskWindow();

    //刷新我的课程下拉框
    ErrorClass err = FlushLeaveAskMyCauseCombo();
    if(err.GetCode() != ERRCODE_SUCCESS)
    {
        QMessageBox::warning(this, "警告", err.GetMsg());
        return;
    }

    return;
}

//请假提交按钮
void MainWindow::on_pushButtonLeaveAskSubmit_released()
{
    QMessageBox::StandardButton button = QMessageBox::question(this, "警告", "确定提交?");
    if(button == QMessageBox::No)
    {
        return;
    }
    QString causeName = ui->comboBoxLeaveAskMyCause->currentText();
    CauseInfo causeInfo(causeName);
    ErrorClass err = ZeusDao::QueryCauseInfoByName(causeInfo);
    if(err.GetCode() != ERRCODE_SUCCESS)
    {
        QMessageBox::warning(this, "警告", err.GetMsg());
        return;
    }
    QString teachTime = ui->comboBoxLeaveAskWeek->currentText() + "_" + ui->comboBoxLeaveAskTime->currentText();
    LeaveAsk leaveAskInfo(userInfo.id, causeInfo.id, causeInfo.causeName, causeInfo.teacherId, teachTime,
                          ui->textEditLeaveAskMsg->toPlainText(), LEAVING_ASK_STAUTS_ASKING);
    err = ZeusDao::InsertLeaveAsk(leaveAskInfo);
    if(err.GetCode() != ERRCODE_SUCCESS)
    {
        QMessageBox::warning(this, "警告", err.GetMsg());
        return;
    }
    QMessageBox::information(this, "成功", "操作成功");
    on_actionLeaveAsk_triggered();
}

//请假申请页课程下拉框变换
void MainWindow::on_comboBoxLeaveAskMyCause_currentIndexChanged(int index)
{
    QString causeName = ui->comboBoxLeaveAskMyCause->currentText();
    if (causeName.compare("") == 0)
    {
        return;
    }
    CauseInfo causeInfo(causeName);
    ErrorClass err = ZeusDao::QueryCauseInfoByName(causeInfo);
    if(err.GetCode() != ERRCODE_SUCCESS)
    {
        QMessageBox::warning(this, "警告", err.GetMsg());
        return;
    }

    //周单选框
    ui->comboBoxLeaveAskWeek->clear();
    for (int i = 0; i < causeInfo.causeWeek.size(); ++i)
    {
        ui->comboBoxLeaveAskWeek->addItem(QString("%1").arg(causeInfo.causeWeek[i]));
    }
    ui->comboBoxLeaveAskWeek->setCurrentIndex(0);

    //时间单选框
    ui->comboBoxLeaveAskTime->clear();
    for (int i = 0; i < causeInfo.causeTime.size(); i++)
    {
        ui->comboBoxLeaveAskTime->addItem(QString("%1").arg(causeInfo.causeTime[i]));
    }
    ui->comboBoxLeaveAskTime->setCurrentIndex(0);
}

//历史请假记录
void MainWindow::on_pushButton_released()
{
    LeavingAskHistory leaveAskHistory(userInfo.id, this);
    leaveAskHistory.exec();
}

ErrorClass MainWindow::InitLeaveAskControl()
{
    //初始化数据
    QVector<LeaveAsk> leaveAsks;
    ErrorClass err = ZeusDao::QueryAllLeaveAskByTeacherId(userInfo.id, leaveAsks);
    if (err.GetCode() != ERRCODE_SUCCESS)
    {
        return err;
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
    return ErrorClass();
}

//请假审批
void MainWindow::on_actionLaeveControl_triggered()
{
    //清空历史数据
    QHash<qint64, QComboBox*>::const_iterator j = idComboxMap.constBegin();
    while (j != idComboxMap.constEnd())
    {
        delete j.value();
        ++j;
    }
    idComboxMap.clear();
    ui->tableWidgetLeaveAskControl->clear();
    ui->stackedWidget->setCurrentIndex(4);
    ErrorClass err = InitLeaveAskControl();
    if(err.GetCode() != ERRCODE_SUCCESS)
    {
        QMessageBox::warning(this, "警告", err.GetMsg());
        return;
    }
    return;
}

//请假审批提交按钮
void MainWindow::on_pushButton_2_released()
{
    QMessageBox::StandardButton button = QMessageBox::question(this, "警告", "确定提交?");
    if(button == QMessageBox::No)
    {
        return;
    }
    QHash<qint64, QComboBox*>::const_iterator j = idComboxMap.constBegin();
    QVector<LeaveAsk> leaveAsks;
    while (j != idComboxMap.constEnd())
    {
        QComboBox* comboxRes = j.value();
        if (comboxRes->currentText().compare("已通过") == 0)
        {
            leaveAsks.push_back(LeaveAsk(j.key(), LEAVING_ASK_STAUTS_PASS));
        } else if (comboxRes->currentText().compare("已驳回") == 0)
        {
            leaveAsks.push_back(LeaveAsk(j.key(), LEAVING_ASK_STAUTS_REFUSE));
        }
        ++j;
    }
    ErrorClass err = ZeusDao::UpdateLeaveAskStatusBatch(leaveAsks);
    if(err.GetCode() != ERRCODE_SUCCESS)
    {
        QMessageBox::warning(this, "警告", err.GetMsg());
        return;
    }
    QMessageBox::information(this, "成功", "操作成功");
    //刷新页面
    on_actionLaeveControl_triggered();
}

//状态控制界面
void MainWindow::on_actionStatusControl_triggered()
{
    //刷新数据
    ui->stackedWidget->setCurrentIndex(5);
    tableModel->setTable("zeus_status_control");
    ui->tableViewDataControl->setModel(tableModel);
    tableModel->select();

    tableModel->setHeaderData(0, Qt::Horizontal, "编号");
    ui->tableViewDataControl->setColumnWidth(0, 150);
    ui->tableViewDataControl->setItemDelegateForColumn(0, readOnlyDelegate);

    tableModel->setHeaderData(1, Qt::Horizontal, "选课状态(1:开启, 2:关闭)");
    ui->tableViewDataControl->setColumnWidth(1, 300);
    ui->tableViewDataControl->setItemDelegateForColumn(1, nullptr);

    tableModel->setHeaderData(2, Qt::Horizontal, "评教状态(1:开启, 2:关闭)");
    ui->tableViewDataControl->setColumnWidth(2, 300);
    ui->tableViewDataControl->setItemDelegateForColumn(2, nullptr);

    tableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
}

//提交按钮
void MainWindow::on_pushButtonDataControlSubmit_released()
{
    QMessageBox::StandardButton button = QMessageBox::question(this, "警告", "确定提交?");
    if(button == QMessageBox::No)
    {
        return;
    }
    tableModel->database().transaction(); //开始事务操作
    if (tableModel->submitAll()) {
        tableModel->database().commit(); //提交
    } else
    {
        tableModel->database().rollback(); //回滚
        QMessageBox::warning(this, "警告", "班级名称不能相同");
        return;
    }
    QMessageBox::information(this, "成功", "提交成功");
}

//班级信息管理
void MainWindow::on_actionAllClass_triggered()
{
    //刷新数据
    ui->stackedWidget->setCurrentIndex(5);
    tableModel->setTable("zeus_class_info");
    ui->tableViewDataControl->setModel(tableModel);
    tableModel->setFilter("1 = 1 ORDER BY name");
    tableModel->select();

    tableModel->setHeaderData(0, Qt::Horizontal, "班级编号");
    ui->tableViewDataControl->setColumnWidth(0, 150);
    ui->tableViewDataControl->setItemDelegateForColumn(0, readOnlyDelegate);

    tableModel->setHeaderData(1, Qt::Horizontal, "班级名称");
    ui->tableViewDataControl->setColumnWidth(1, 300);
    ui->tableViewDataControl->setItemDelegateForColumn(1, nullptr);

    tableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
}

//年级信息管理
void MainWindow::on_actionAllGrand_triggered()
{
    //刷新数据
    ui->stackedWidget->setCurrentIndex(5);
    tableModel->setTable("zeus_grand_info");
    ui->tableViewDataControl->setModel(tableModel);
    tableModel->setFilter("1 = 1 ORDER BY name");
    tableModel->select();

    tableModel->setHeaderData(0, Qt::Horizontal, "年级组编号");
    ui->tableViewDataControl->setColumnWidth(0, 150);
    ui->tableViewDataControl->setItemDelegateForColumn(0, readOnlyDelegate);

    tableModel->setHeaderData(1, Qt::Horizontal, "年级组名称");
    ui->tableViewDataControl->setColumnWidth(1, 300);
    ui->tableViewDataControl->setItemDelegateForColumn(1, nullptr);

    tableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
}

//用户信息管理
void MainWindow::on_actionAllStudent_triggered()
{
    ui->stackedWidget->setCurrentIndex(5);
    tableModel->setTable("zeus_user_info");
    ui->tableViewDataControl->setModel(tableModel);
    tableModel->setFilter("1 = 1 ORDER BY id");
    tableModel->select();

    tableModel->setHeaderData(0, Qt::Horizontal, "用户编号(学号/工号)");
    ui->tableViewDataControl->setColumnWidth(0, 150);
    ui->tableViewDataControl->setItemDelegateForColumn(0, readOnlyDelegate);

    tableModel->setHeaderData(1, Qt::Horizontal, "用户名称");
    ui->tableViewDataControl->setColumnWidth(1, 150);
    ui->tableViewDataControl->setItemDelegateForColumn(1, nullptr);

    tableModel->setHeaderData(2, Qt::Horizontal, "用户类型(1:学生,2:教师,3:管理员)");
    ui->tableViewDataControl->setColumnWidth(2, 150);
    ui->tableViewDataControl->setItemDelegateForColumn(2, readOnlyDelegate);

    tableModel->setHeaderData(3, Qt::Horizontal, "用户密码md5签名");
    ui->tableViewDataControl->setColumnWidth(3, 150);
    ui->tableViewDataControl->setItemDelegateForColumn(3, readOnlyDelegate);

    tableModel->setHeaderData(4, Qt::Horizontal, "电话");
    ui->tableViewDataControl->setColumnWidth(4, 150);
    ui->tableViewDataControl->setItemDelegateForColumn(4, nullptr);

    tableModel->setHeaderData(5, Qt::Horizontal, "班级id");
    ui->tableViewDataControl->setColumnWidth(5, 150);
    ui->tableViewDataControl->setItemDelegateForColumn(5, nullptr);

    tableModel->setHeaderData(6, Qt::Horizontal, "年级组id");
    ui->tableViewDataControl->setColumnWidth(6, 150);
    ui->tableViewDataControl->setItemDelegateForColumn(6, nullptr);

    tableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
}

//课程信息管理
void MainWindow::on_actionAllCause_triggered()
{
    ui->stackedWidget->setCurrentIndex(5);
    tableModel->setTable("zeus_cause_info");
    ui->tableViewDataControl->setModel(tableModel);
    tableModel->setFilter("1 = 1 ORDER BY cause_name");
    tableModel->select();

    tableModel->setHeaderData(0, Qt::Horizontal, "课程编号");
    ui->tableViewDataControl->setColumnWidth(0, 150);
    ui->tableViewDataControl->setItemDelegateForColumn(0, readOnlyDelegate);

    tableModel->setHeaderData(1, Qt::Horizontal, "课程名称");
    ui->tableViewDataControl->setColumnWidth(1, 150);
    ui->tableViewDataControl->setItemDelegateForColumn(1, readOnlyDelegate);

    tableModel->setHeaderData(2, Qt::Horizontal, "课程类型(1:必修课,2:选修课)");
    ui->tableViewDataControl->setColumnWidth(2, 150);
    ui->tableViewDataControl->setItemDelegateForColumn(2, readOnlyDelegate);

    tableModel->setHeaderData(3, Qt::Horizontal, "课程学分");
    ui->tableViewDataControl->setColumnWidth(3, 150);
    ui->tableViewDataControl->setItemDelegateForColumn(3, nullptr);

    tableModel->setHeaderData(4, Qt::Horizontal, "课程教师工号");
    ui->tableViewDataControl->setColumnWidth(4, 150);
    ui->tableViewDataControl->setItemDelegateForColumn(4, readOnlyDelegate);

    tableModel->setHeaderData(5, Qt::Horizontal, "上课时间");
    ui->tableViewDataControl->setColumnWidth(5, 150);
    ui->tableViewDataControl->setItemDelegateForColumn(5, nullptr);

    tableModel->setHeaderData(6, Qt::Horizontal, "上课周次");
    ui->tableViewDataControl->setColumnWidth(6, 150);
    ui->tableViewDataControl->setItemDelegateForColumn(6, nullptr);

    tableModel->setHeaderData(7, Qt::Horizontal, "上课教室");
    ui->tableViewDataControl->setColumnWidth(7, 150);
    ui->tableViewDataControl->setItemDelegateForColumn(7, nullptr);

    tableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
}

//初始化注册教师界面
void MainWindow::InitReginstTeacher()
{
    ui->lineEditReginstTeacherName->clear();
    ui->lineEditReginstTeacherPhone->clear();
    ui->lineEditReginstTeacherPasswordFirst->clear();
    ui->lineEditReginstTeacherPasswordSecond->clear();

    //刷新下拉框
    queryModel->setQuery("SELECT name FROM zeus_grand_info ORDER BY name;");
    if(queryModel->lastError().isValid())
    {
        QMessageBox::warning(this, "警告", queryModel->lastError().text());
        return;
    }
    ui->comboBoxReginstTeacherGrand->setModel(queryModel);
    ui->comboBoxReginstTeacherGrand->setCurrentIndex(0);

    //设置电话输入框
    QRegExp regExp("^[0-9]*$");
    QRegExpValidator *pattern= new QRegExpValidator(regExp, this);//创建了一个表达式
    ui->lineEditReginstTeacherPhone->setValidator(pattern);
}

//添加教师页面
void MainWindow::on_actionAddTeacher_triggered()
{
    ui->stackedWidget->setCurrentIndex(6);
    InitReginstTeacher();
}

//注册教师提交按钮
void MainWindow::on_pushButtonSubmit_released()
{
    QMessageBox::StandardButton button = QMessageBox::question(this, "警告", "确定提交?");
    if(button == QMessageBox::No)
    {
        return;
    }
    QString name = ui->lineEditReginstTeacherName->text();
    QString phoneNumber = ui->lineEditReginstTeacherPhone->text();
    QString gradeName = ui->comboBoxReginstTeacherGrand->currentText();
    QString passwordFirst = ui->lineEditReginstTeacherPasswordFirst->text();
    QString passwordSecond = ui->lineEditReginstTeacherPasswordSecond->text();
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
    if(gradeName == "")
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
    qint64 gradeId = -1;
    ErrorClass err = ZeusDao::QueryGradeIdByName(gradeName, gradeId);
    if(err.GetCode() != 0 || gradeId == -1)
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
    UserInfo userInfo(name, ACCOUNT_TYPE_TEACHER, passwordFirst, phoneNumber, 0, gradeId);
    err = ZeusDao::InsertNewUser(userInfo);
    if(err.GetCode() != ERRCODE_SUCCESS || userInfo.id == 0)
    {
        QMessageBox::warning(this, "注册失败", QString("错误码:%1\n错误:%2").arg(err.GetCode() != 0 ? err.GetCode() : ERRCODE_UNKNOW_ERROR)
                             .arg(err.GetMsg().compare(ERRMSG_DATABASE_EMPEY_ERR) != 0 ? err.GetMsg() : ERRMSG_UNKNOW_MSG));
        return;
    }

    QMessageBox::information(this, "成功", QString("注册成功\n请牢记工号:%1").arg(userInfo.id));
    on_actionAddTeacher_triggered();
    return;
}

//添加班级/年级组页面
void MainWindow::on_actionAddClass_triggered()
{
    ui->stackedWidget->setCurrentIndex(7);
    ui->comboBoxClassOrGrade->clear();
    ui->comboBoxClassOrGrade->addItem("班级");
    ui->comboBoxClassOrGrade->addItem("年级组");
    ui->comboBoxClassOrGrade->setCurrentIndex(0);
    ui->lineEditClassGradeName->clear();
}

//添加班级/年级组提交按钮
void MainWindow::on_pushButtonAddClassGradeSubmit_released()
{
    QMessageBox::StandardButton button = QMessageBox::question(this, "警告", "确定提交?");
    if(button == QMessageBox::No)
    {
        return;
    }

    //入参校验
    if (ui->comboBoxClassOrGrade->currentText().compare("") == 0)
    {
        QMessageBox::warning(this, "警告", "创建类型不能为空");
        return;
    }
    if (ui->lineEditClassGradeName->text().compare("") == 0)
    {
        QMessageBox::warning(this, "警告", "名称不能为空");
        return;
    }

    if (ui->comboBoxClassOrGrade->currentText().compare("班级") == 0)
    {
        ErrorClass err = ZeusDao::InserClassInfo(ui->lineEditClassGradeName->text());
        if (err.GetCode() != ERRCODE_SUCCESS)
        {
            QMessageBox::warning(this, "警告", err.GetMsg());
            return;
        }
    } else
    {
        ErrorClass err = ZeusDao::InserGradeInfo(ui->lineEditClassGradeName->text());
        if (err.GetCode() != ERRCODE_SUCCESS)
        {
            QMessageBox::warning(this, "警告", err.GetMsg());
            return;
        }
    }
    QMessageBox::information(this, "成功", "提交成功");
    on_actionAddClass_triggered();
}
