#ifndef QTSQLITE_H
#define QTSQLITE_H

#include <QMainWindow>
#include <QtSql>
#include <QDebug>

/*namespace Ui {
    class QtSqlite;
}*/

class QtSqlite : public QMainWindow
{
    Q_OBJECT

public:
    explicit QtSqlite(QWidget *parent = 0);
    ~QtSqlite();

    bool InsertTimetoSql(QString t_carNum,int t_parkNum,QDateTime t_time,QString t_direction);     //向数据库写入时间
    bool GetTimefromSql(QString t_carNum,int t_parkNum,QDateTime &t_time);       //从数据库读取时间
    bool GetMoneyformSql(QString t_carNum,int &t_money);      //从数据库读取余额
    bool UpdateMoneytoSql(QString t_carNum,int t_money);     //更新余额
    bool OutputInformation(bool t_isdebug,bool t_indetail);     //是否输出各类信息
    bool initDataBase();
    bool SignUptoSql(QString t_carNum,int t_money);

signals:
    void readyForInitDataBase();


private:
    QSqlDatabase database;      //数据库
    QSqlQuery sql_query;        //数据库连接
    bool m_isDebug;     //true输出调试信息，false不输出调试信息
    bool m_inDetail;        //true输出详细信息，false不输出详细信息
};

#endif // QTSQLITE_H
