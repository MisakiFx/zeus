#ifndef LEAVEASKCONTROL_H
#define LEAVEASKCONTROL_H

#include <QDialog>
#include <QComboBox>

namespace Ui {
class LeaveAskControl;
}

class LeaveAskControl : public QDialog
{
    Q_OBJECT

public:
    explicit LeaveAskControl(qint64 teacherId, QWidget *parent = nullptr);
    ~LeaveAskControl();

private:
    Ui::LeaveAskControl *ui;
    QHash<qint64, QComboBox*> idComboxMap;
};

#endif // LEAVEASKCONTROL_H
