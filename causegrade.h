#ifndef CAUSEGRADE_H
#define CAUSEGRADE_H

#include <QDialog>
#include "model.h"
#include <QHash>
#include <QLineEdit>

namespace Ui {
class CauseGrade;
}

class CauseGrade : public QDialog
{
    Q_OBJECT

public:
    explicit CauseGrade(QWidget *parent = nullptr);
    CauseGrade(const CauseInfo& causeInfo, QWidget *parent);
    ~CauseGrade();

private slots:
    void on_pushButtonSubmit_released();

private:
    Ui::CauseGrade *ui;

    CauseInfo causeInfo;
    QHash<int, QLineEdit*> rowLineEditMap;
};

#endif // CAUSEGRADE_H
