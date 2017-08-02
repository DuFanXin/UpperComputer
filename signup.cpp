#include "signup.h"
#include "ui_signup.h"

SignUp::SignUp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SignUp)
{
    ui->setupUi(this);
    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(dataToUpClass()));
    connect(ui->buttonBox,SIGNAL(rejected()),this,SLOT(close()));
}

SignUp::~SignUp()
{
    delete ui;
}

bool SignUp::dataToUpClass()
{
    emit readytoSignup(ui->lineEdit_carNum->text(),ui->lineEdit_money->text().toInt());
    this->close();
    return true;
}

bool SignUp::dataClear()
{
    ui->lineEdit_carNum->clear();
    ui->lineEdit_money->clear();
    return true;
}
