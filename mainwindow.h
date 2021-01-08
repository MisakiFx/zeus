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

    void on_actionMyClass_triggered();

    void on_comboMyClass_currentIndexChanged(const QString &arg1);

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
    void InitMenu();
    ErrorClass FlushMyClassComboBox();
    ErrorClass FlushMyClassInfo();
    void InitCreateCause();
    void DrawWeekStatusComb();
    void DrawTimeStatusComb();
    void DrawClassStatusComb();
    void ClearAllCreateCauseInput();
};

#endif // MAINWINDOW_H
