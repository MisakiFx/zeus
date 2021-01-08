#ifndef CAUSECHECKHISTORY_H
#define CAUSECHECKHISTORY_H

#include <QDialog>

namespace Ui {
class CauseCheckHistory;
}

class CauseCheckHistory : public QDialog
{
    Q_OBJECT

public:
    explicit CauseCheckHistory(QWidget *parent = nullptr, qint64 causeId = 0);
    ~CauseCheckHistory();

private:
    Ui::CauseCheckHistory *ui;
};

#endif // CAUSECHECKHISTORY_H
