#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QListWidget>
#include <QComboBox>
#include <QSqlTableModel>
#include "model.h"
#include "loginwindow.h"
#include "errorclass.h"
#include "readonlydelegate.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionUserInfo_triggered();

    void on_actionExit_triggered();

    void on_actionMyCause_triggered();

    void on_comboMyCause_currentIndexChanged(const QString &arg1);

    void on_pushButtonCauseCheck_released();

    void on_pushButtonCauseCheckHistory_released();

    void on_actionCreateClass_triggered();

    void WeekStateChanged(int state);

    void WeekTextChanged(const QString &text);

    void TimeStateChanged(int state);

    void TimeTextChanged(const QString &text);

    void ClassStateChanged(int state);

    void ClassTextChanged(const QString &text);

    void on_pushButtonCreateCauseSubmit_released();

    void on_comboCreateCauseType_currentIndexChanged(int index);

    void on_pushButtonGrade_released();

    void on_pushButtonEvalute_released();

    void on_pushButtonAbandonChoose_released();

    void on_actionChooseClass_triggered();

    void on_pushButtonChooseCause_released();

    void on_actionLeaveAsk_triggered();

    void on_pushButtonLeaveAskSubmit_released();

    void on_comboBoxLeaveAskMyCause_currentIndexChanged(int index);

    void on_pushButton_released();

    void on_actionLaeveControl_triggered();

    void on_pushButton_2_released();

    void on_actionStatusControl_triggered();

    void on_pushButtonDataControlSubmit_released();

    void on_actionAllClass_triggered();

    void on_actionAllGrand_triggered();

    void on_actionAllStudent_triggered();

    void on_actionAllCause_triggered();

    void on_actionAddTeacher_triggered();

    void on_pushButtonSubmit_released();

    void on_actionAddClass_triggered();

    void on_pushButtonAddClassGradeSubmit_released();

private:
    Ui::MainWindow *ui;
    LoginWindow login; //登录窗口
    UserInfo userInfo; //用户基本信息
    QSqlQueryModel* queryModel;

    QLineEdit *lineEditWeek;
    QListWidget *listWidgetWeek;
    bool bSelectedWeek;
    QString strSelectedTextWeek;

    QLineEdit *lineEditTime;
    QListWidget *listWidgetTime;
    bool bSelectedTime;
    QString strSelectedTextTime;

    QLineEdit *lineEditClass;
    QListWidget *listWidgetClass;
    bool bSelectedClass;
    QString strSelectedTextClass;

    QHash<qint64, QComboBox*> idComboxMap;

    QSqlTableModel* tableModel;

    ReadOnlyDelegate* readOnlyDelegate;

    bool isChooseTime;
    bool isEvaluteTime;

    bool isChooseCause;

    void InitAccount(qint64 accountId);
    void Login();
    void DealLoginSlog(qint64 accountId);
    void InitMainWindow();
    ErrorClass FlushMyCauseComboBox();
    ErrorClass FlushChooseCauseComboBox();
    ErrorClass FlushCauseInfo();
    ErrorClass FlushLeaveAskMyCauseCombo();
    ErrorClass InitLeaveAskControl();
    void InitCreateCause();
    void InitLeaveAsk();
    void DrawWeekStatusComb();
    void DrawTimeStatusComb();
    void DrawCauseClassComb();
    void ClearAllCreateCauseInput();
    void ClearMyCauseInfo();
    void ClearLeaveAskWindow();
    void InitReginstTeacher();
};

#endif // MAINWINDOW_H
