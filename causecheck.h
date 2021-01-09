#ifndef CAUSECHECK_H
#define CAUSECHECK_H

#include <QDialog>
#include "model.h"

namespace Ui {
class CauseCheck;
}

class CauseCheck : public QDialog
{
    Q_OBJECT

public:
    explicit CauseCheck(QWidget *parent = nullptr);
    explicit CauseCheck(const CauseInfo& causeInfo, QWidget *parent);
    ~CauseCheck();

private slots:
    void on_pushButtonSubmmit_released();

private:
    Ui::CauseCheck *ui;

    CauseInfo causeInfo;
};

#endif // CAUSECHECK_H
