#ifndef CAUSEGRADE_H
#define CAUSEGRADE_H

#include <QDialog>
#include <model.h>

namespace Ui {
class CauseGrade;
}

class CauseGrade : public QDialog
{
    Q_OBJECT

public:
    explicit CauseGrade(QWidget *parent = nullptr);
    CauseGrade(CauseInfo& causeInfo, QWidget *parent);
    ~CauseGrade();

private:
    Ui::CauseGrade *ui;

    CauseInfo causeInfo;
};

#endif // CAUSEGRADE_H
