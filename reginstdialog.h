#ifndef REGINSTDIALOG_H
#define REGINSTDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>
namespace Ui {
class ReginstDialog;
}

class ReginstDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReginstDialog(QWidget *parent = nullptr);
    ~ReginstDialog();

    bool FlushComboBoxData();
    void ClearAllLineEdit();

private slots:
    void on_pushButtonSubmit_pressed();

private:
    Ui::ReginstDialog *ui;
    QSqlQueryModel* queryModel;
    void InitComboBox();
    void InitLineEdit();
};

#endif // REGINSTDIALOG_H
