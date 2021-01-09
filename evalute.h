#ifndef EVALUTE_H
#define EVALUTE_H

#include <QDialog>
#include "model.h"

namespace Ui {
class Evalute;
}

class Evalute : public QDialog
{
    Q_OBJECT

public:
    explicit Evalute(QWidget *parent = nullptr);
    Evalute(const CauseInfo& causeInfo, const UserInfo& userInfo, QWidget *parent);
    ~Evalute();

private slots:
    void on_pushButtonSubmit_released();

private:
    Ui::Evalute *ui;
    CauseInfo causeInfo;
    UserInfo userInfo;
};

#endif // EVALUTE_H
