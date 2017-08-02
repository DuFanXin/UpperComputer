#ifndef UPPERCOMPUTER_H
#define UPPERCOMPUTER_H

#include <QMainWindow>
#include "qtsqlite.h"
#include "win_qextserialport.h"
#include <QLabel>
#include <QMessageBox>
#include <QPainter>
#include <QPen>
#include <string>
#include <QMovie>
#include <signup.h>
#include <recharge.h>
using std::string;

namespace Ui {
    class UpperComputer;
}

const int maxn=6;       //��λ����
const bool isDebug = false;     //true���������Ϣ��false�����������Ϣ
const bool inDetail = true;        //true�����ϸ��Ϣ��false�������ϸ��Ϣ

//��ʱ��TIME_OUT�Ǵ��ڶ�д����ʱ
#define TIME_OUT 10

//��ȡ��ʱ����ʱ���,200ms����ȡ��ʱ�������Ƕ�ȡ���ڻ������ʱ
#define TIMER_INTERVAL 200

//�������Ͷ�ʱ����ʱ���,200ms
#define OBO_TIMER_INTERVAL 200

//�����ļ�ʱ������ļ���С����TIP_FILE_SIZE�ֽ�����ʾ�ļ������Ƿ������
#define TIP_FILE_SIZE 5000

//�����ļ���󳤶�������MAX_FILE_SIZE�ֽ���
#define MAX_FILE_SIZE 10000

struct park{
    QString carNum;
    int parkNum;
    QString inTime;
    QString statu;      //Free��Waiting��Occupied��0��1��2,

    park(QString carnum="EA12345",int parknum=0,QString intime="2017-07-18 19:36:56",QString sta="Occupied"):
        carNum(carnum),
        parkNum(parknum),
        inTime(intime),
        statu(sta){}
};

class UpperComputer : public QMainWindow
{
    Q_OBJECT

public:
    explicit UpperComputer(QWidget *parent = 0);
    ~UpperComputer();
    void paintEvent(QPaintEvent *event);

signals:
    void carLeave(int parkNum);
    void carDistri(QString carNum);
    void carIn(int parkNum);
    void readyToInit();
    void callForDatajudge(QString str);


private slots:
    bool handleCarLeave(int parkNum);
    bool distributeCar(QString carNum);
    bool handleCarIn(int parkNum);
    bool test();
    bool sysInit();
    bool readMyCom();
    bool sendMsg(QString qstr);
    bool paintHum();
    bool paintAlarm();
    bool dataJudge(QByteArray str);
    bool paintTemp();
    bool paintPark();
    bool signUp();
    bool recharge();
    bool handelSignup(QString carNum, int money);
    bool handelRecharge(QString carNum, int money);
    bool showCall(int parkNum,bool isCall);
    bool checkData(QByteArray qb);

private:
    Ui::UpperComputer *ui;
    QtSqlite *m_sql;
    park parks[maxn];
    QLabel *m_label_pic[maxn];
    QLabel *m_label_time[maxn];
    QLabel *m_label_carNum[maxn];
    QLabel *m_label_statu[maxn];
    QLabel *m_label_call[maxn];
    Win_QextSerialPort *myCom;
    QTimer *timer;
    QString write2fileName;    //д��ȡ�Ĵ������ݵ����ļ�
    QMovie *m_Movie_Alarm;
    QMovie *m_Movie_Call;
    SignUp *m_signup;
    Recharge *m_recharge;

    int m_MoneyPerSec;
    unsigned int timerdly;
    unsigned int obotimerdly;
    int m_Hum;        //ʪ��
    int m_Temp;     //�¶�
    bool m_Alarm;
    bool m_Alarm_ison;
};

#endif // UPPERCOMPUTER_H
