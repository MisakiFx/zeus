#include "reginstwiget.h"
#include "ui_reginstwiget.h"

reginstWiget::reginstWiget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::reginstWiget)
{
    ui->setupUi(this);
}

reginstWiget::~reginstWiget()
{
    delete ui;
}
