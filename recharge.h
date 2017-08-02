#ifndef RECHARGE_H
#define RECHARGE_H

#include <QDialog>

namespace Ui {
    class Recharge;
}

class Recharge : public QDialog
{
    Q_OBJECT

public:
    explicit Recharge(QWidget *parent = 0);
    ~Recharge();
    bool dataClear();

signals:
    void readytoRacharge(QString carnum,int money);

private slots:
    bool dataToUpClass();

private:
    Ui::Recharge *ui;
};

#endif // RECHARGE_H
