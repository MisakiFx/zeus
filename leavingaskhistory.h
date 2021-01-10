#ifndef LEAVINGASKHISTORY_H
#define LEAVINGASKHISTORY_H

#include <QDialog>

namespace Ui {
class LeavingAskHistory;
}

class LeavingAskHistory : public QDialog
{
    Q_OBJECT

public:
    LeavingAskHistory(qint64 studentId, QWidget *parent = nullptr);
    ~LeavingAskHistory();

private:
    Ui::LeavingAskHistory *ui;
};

#endif // LEAVINGASKHISTORY_H
