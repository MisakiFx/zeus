#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QListWidget>
#include "model.h"
#include "loginwindow.h"
#include "errorclass.h"

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

    void on_pushButtonSubbmit_released();

    void on_comboCreateCauseType_currentIndexChanged(int index);

    void on_pushButtonGrade_released();

    void on_pushButtonEvalute_released();

    void on_pushButtonAbandonChoose_released();

    void on_actionChooseClass_triggered();

    void on_pushButtonChooseCause_released();

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

    void InitAccount(qint64 accountId);
    void Login();
    void DealLoginSlog(qint64 accountId);
    void InitMainWindow();
    ErrorClass FlushMyCauseComboBox();
    ErrorClass FlushChooseCauseComboBox();
    ErrorClass FlushCauseInfo();
    void InitCreateCause();
    void DrawWeekStatusComb();
    void DrawTimeStatusComb();
    void DrawClassStatusComb();
    void ClearAllCreateCauseInput();
    void ClearMyCauseInfo();
};

#endif // MAINWINDOW_H
