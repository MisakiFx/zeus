#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "model.h"
#include "loginwindow.h"

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

private:
    Ui::MainWindow *ui;
    LoginWindow login; //登录窗口
    UserInfo userInfo; //用户基本信息

    void InitAccount(qint64 accountId);
    void Login();
    void DealLoginSlog(qint64 accountId);
    void InitMenu();
};

#endif // MAINWINDOW_H
