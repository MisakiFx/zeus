#include "causecheck.h"
#include "ui_causecheck.h"

CauseCheck::CauseCheck(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CauseCheck)
{
    ui->setupUi(this);
}

CauseCheck::~CauseCheck()
{
    delete ui;
}
