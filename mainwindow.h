#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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

private:
    Ui::MainWindow *ui;
    LoginWindow login; //登录窗口
    UserInfo userInfo; //用户基本信息
    QSqlQueryModel* queryModel;

    void InitAccount(qint64 accountId);
    void Login();
    void DealLoginSlog(qint64 accountId);
    void InitMenu();
    ErrorClass FlushMyClassComboBox();
    ErrorClass FlushMyClassInfo();
};

#endif // MAINWINDOW_H
