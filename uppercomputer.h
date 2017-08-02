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

const int maxn=6;       //车位个数
const bool isDebug = false;     //true输出调试信息，false不输出调试信息
const bool inDetail = true;        //true输出详细信息，false不输出详细信息

//延时，TIME_OUT是串口读写的延时
#define TIME_OUT 10

//读取定时器计时间隔,200ms，读取定时器是我们读取串口缓存的延时
#define TIMER_INTERVAL 200

//连续发送定时器计时间隔,200ms
#define OBO_TIMER_INTERVAL 200

//载入文件时，如果文件大小超过TIP_FILE_SIZE字节则提示文件过大是否继续打开
#define TIP_FILE_SIZE 5000

//载入文件最大长度限制在MAX_FILE_SIZE字节内
#define MAX_FILE_SIZE 10000

struct park{
    QString carNum;
    int parkNum;
    QString inTime;
    QString statu;      //Free、Waiting、Occupied，0、1、2,

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
    QString write2fileName;    //写读取的串口数据到该文件
    QMovie *m_Movie_Alarm;
    QMovie *m_Movie_Call;
    SignUp *m_signup;
    Recharge *m_recharge;

    int m_MoneyPerSec;
    unsigned int timerdly;
    unsigned int obotimerdly;
    int m_Hum;        //湿度
    int m_Temp;     //温度
    bool m_Alarm;
    bool m_Alarm_ison;
};

#endif // UPPERCOMPUTER_H
