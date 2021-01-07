#ifndef CAUSECHECK_H
#define CAUSECHECK_H

#include <QDialog>

namespace Ui {
class CauseCheck;
}

class CauseCheck : public QDialog
{
    Q_OBJECT

public:
    explicit CauseCheck(QWidget *parent = nullptr);
    ~CauseCheck();

private:
    Ui::CauseCheck *ui;
};

#endif // CAUSECHECK_H
