#include "recharge.h"
#include "ui_recharge.h"

Recharge::Recharge(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Recharge)
{
    ui->setupUi(this);
    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(dataToUpClass()));
    connect(ui->buttonBox,SIGNAL(rejected()),this,SLOT(close()));
}

Recharge::~Recharge()
{
    delete ui;
}

bool Recharge::dataToUpClass()
{
    emit readytoRacharge(ui->lineEdit_carNum->text(),ui->lineEdit_money->text().toInt());
    this->close();
    return true;
}

bool Recharge::dataClear()
{
    ui->lineEdit_carNum->clear();
    ui->lineEdit_money->clear();
    return true;
}
