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

    bool InsertTimetoSql(QString t_carNum,int t_parkNum,QDateTime t_time,QString t_direction);     //�����ݿ�д��ʱ��
    bool GetTimefromSql(QString t_carNum,int t_parkNum,QDateTime &t_time);       //�����ݿ��ȡʱ��
    bool GetMoneyformSql(QString t_carNum,int &t_money);      //�����ݿ��ȡ���
    bool UpdateMoneytoSql(QString t_carNum,int t_money);     //�������
    bool OutputInformation(bool t_isdebug,bool t_indetail);     //�Ƿ����������Ϣ
    bool initDataBase();
    bool SignUptoSql(QString t_carNum,int t_money);

signals:
    void readyForInitDataBase();


private:
    QSqlDatabase database;      //���ݿ�
    QSqlQuery sql_query;        //���ݿ�����
    bool m_isDebug;     //true���������Ϣ��false�����������Ϣ
    bool m_inDetail;        //true�����ϸ��Ϣ��false�������ϸ��Ϣ
};

#endif // QTSQLITE_H
