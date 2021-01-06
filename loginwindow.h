#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include "reginstdialog.h"

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

    void ClearAllText();

private slots:
    void on_pushButtonLogin_pressed();

    void on_pushButtonReginst_pressed();

signals:
    void loginSuccess(qint64 account);

private:
    Ui::LoginWindow *ui;
    ReginstDialog* reginst;

    void InitLineEdit();

};

#endif // LOGINWINDOW_H
