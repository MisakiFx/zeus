#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>
#include "model.h"
#include "loginwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QDialog
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    LoginWindow login; //登录窗口
    UserInfo userInfo; //用户基本信息

    void InitAccount(qint64 accountId);
    void Login();
    void DealLoginSlog(qint64 accountId);
};
#endif // MAINWINDOW_H
