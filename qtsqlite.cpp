#include "qtsqlite.h"
//#include "ui_qtsqlite.h"

QtSqlite::QtSqlite(QWidget *parent) :
    QMainWindow(parent)
{
    //connect(this,SIGNAL(readyForInitDataBase()),this,SLOT(initDataBase()));
    //emit readyForInitDataBase();
}

QtSqlite::~QtSqlite()
{
    //delete ui;
    if(database.isOpen()){
        database.close();
        qDebug()<<"database is closed";
    }
}

//��ʼ�����ݿ�
bool QtSqlite::initDataBase()
{
    bool result=true;
    database = QSqlDatabase::addDatabase("QSQLITE");        //�������ݿ�
    database.setDatabaseName("database.db");        //���ݿ�����
    database.setUserName("root");       //�û���
    database.setPassword("123456");     //����

    //�����ݿ�
    if(!database.open())        //δ�ɹ���
    {
        qDebug()<<database.lastError();     //��ӡ������Ϣ
        qFatal("failed to connect.") ;
        return result = false;
    }
    else{
        qDebug()<<"success to connect";     //��ӡ�ɹ���Ϣ
        sql_query = QSqlQuery::QSqlQuery(database);     //�����ݿ�
    }

    //��ͣ�������ڣ�������������ڣ��򲻴���
    bool isTableExist =
            sql_query.exec(QString("select count(*) from %1").arg("CarParks"));     //���Ա��Ƿ����
    if(!isTableExist)       //���������
    {
        qDebug() << "CarParks table does not exist";     //��ӡ�����ڱ����Ϣ
        QString create_sql = "create table CarParks (CarNum varchar(30), ParkNum int, Time DATETIME primary key, Direction varchar(5))";
        sql_query.prepare(create_sql);      //������
        if(!sql_query.exec())
        {
            if(m_isDebug)
                qDebug()<<sql_query.lastError();        //�������ɹ�����ӡ��Ϣ
            result = false;
        }
        else
        {
            if(m_inDetail)
                qDebug()<<"CarParks table created!";     //�����ɹ�
            QString insert_sql = QString("insert into CarParks values (\"EA12345\",3,\"2017-7-20 16 01 33\",\"OUT\")");
            qDebug()<<insert_sql;       //��ӡ������Ϣ
            sql_query.prepare(insert_sql);      //��������
            if(!sql_query.exec())       //�������ݳ���
            {
                if(m_isDebug)
                    qDebug()<<sql_query.lastError();        //��ӡ������Ϣ
                result = false;
            }
            else        //�������ݳɹ�
            {
                if(m_inDetail)
                    qDebug()<<"CarParks table test OK!";       //��ӡ����ɹ���Ϣ
            }
        }
    }
    else        //�����
    {
        if(m_inDetail)
            qDebug() << "CarParks table exists";     //��ӡ��Ϣ
    }

    //������������Ӧ�����ڣ�������������ڣ��򲻴���
    isTableExist =
            sql_query.exec(QString("select count(*) from %1").arg("CarToMoney"));     //���Ա��Ƿ����
    if(!isTableExist)       //���������
    {
        if(m_inDetail)
            qDebug() << "CarToPark table does not exist";     //��ӡ�����ڱ����Ϣ
        QString create_sql = "create table CarToMoney (CarNum varchar(30) primary key, Money int)";
        sql_query.prepare(create_sql);      //������
        if(!sql_query.exec())
        {
            if(m_isDebug)
                qDebug()<<sql_query.lastError();        //�������ɹ�����ӡ��Ϣ
            result = false;
        }
        else
        {
            if(m_inDetail)
                qDebug()<<"CarToMoney table created!";     //�����ɹ�
            QString insert_sql = QString("insert into CarToMoney values (\"A8888\",100)");
            qDebug()<<insert_sql;       //��ӡ������Ϣ
            sql_query.prepare(insert_sql);      //��������
            if(!sql_query.exec())       //�������ݳ���
            {
                if(m_isDebug)
                    qDebug()<<sql_query.lastError();        //��ӡ������Ϣ
                result = false;
            }
            else        //�������ݳɹ�
            {
                if(m_inDetail)
                    qDebug()<<"CarToMoney table test OK!";       //��ӡ����ɹ���Ϣ
            }
        }
    }
    else        //�����
    {
        if(m_inDetail)
            qDebug() << "CarToMoney table exists";     //��ӡ��Ϣ
    }
    return result;
}

//�����ݿ����ʱ��
bool QtSqlite::InsertTimetoSql(QString t_carNum,int t_parkNum,QDateTime t_time,QString t_direction)
{
    //�����ݿ�д����ʱ��
    QString inTimeStr = t_time.toString("yyyy-MM-dd hh mm ss");     //ת��ʱ������
    QString insert_sql =        //��������SQL���
            QString("insert into CarParks values (\"%1\",%2,\"%3\",\"%4\")").arg(t_carNum).arg(t_parkNum).arg(inTimeStr).arg(t_direction);
    if(m_isDebug) qDebug()<<insert_sql;       //��ӡSQL��䣬�������
    sql_query.prepare(insert_sql);      //��������
    bool result = sql_query.exec();     //��ȡ�������
    if(!result)       //�������ݳ���
    {
        qDebug()<<sql_query.lastError();        //��ӡ������Ϣ
    }
    else        //�������ݳɹ�
    {
        //��ӡ�ɹ���Ϣ
       if(m_inDetail) qDebug()<<QString("insert (%1,%2,\%3,\%4) into DataBase").arg(t_carNum).arg(t_parkNum).arg(inTimeStr).arg(t_direction);
    }
    return result;      //������������ظ����ú���
}

//�����ݿ��ȡ��λʱ��
bool QtSqlite::GetTimefromSql(QString t_carNum,int t_parkNum,QDateTime &t_time)
{
    QString select_sql =
            QString("select max(time) from CarParks where ParkNum = %1 and CarNum = \"%2\" and Direction = \"IN\"").arg(t_parkNum).arg(t_carNum);
    if(m_isDebug) qDebug()<<select_sql;
    sql_query.prepare(select_sql);      //��ѯ��Ϣ
    bool result = sql_query.exec();
    if(!result)       //��ѯ����
    {
        qDebug()<<sql_query.lastError();        //��ӡ������Ϣ
    }
    else
    {
        if(m_isDebug) qDebug()<<t_carNum+" parktime find successfully!";     //��ӡ�ɹ���Ϣ

        while(sql_query.next())     //�����ѯ���
        {
            t_time = sql_query.value(0).toDateTime();      //ת�����ݽṹ
           if(m_inDetail) qDebug()<<"find the  intime of "+t_carNum+" in "+t_parkNum +" is "+t_time.toString("yyyy-MM-dd hh:mm:ss");
        }
    }
    return result;
}

//�����ݿ��ȡ���
bool QtSqlite::GetMoneyformSql(QString t_carNum, int &t_money)
{
    QString select_sql =    //��ѯ����SQL���
            QString("select money from CarToMoney where CarNum = \"%1\"").arg(t_carNum);
    if(m_isDebug) qDebug()<<select_sql;       //�����ѯSQL��䣬�������
    sql_query.prepare(select_sql);      //��ѯ��Ϣ
    bool result = sql_query.exec();     //��ȡ�������
    if(!result)       //��ѯ����
    {
        qDebug()<<sql_query.lastError();        //��ӡ������Ϣ
    }
    else
    {
        if(m_inDetail) qDebug()<<t_carNum+" money find successfully!";     //��ӡ�ɹ���Ϣ
        while(sql_query.next())     //�����ѯ���
        {
            t_money = sql_query.value(0).toInt();      //ת�����ݽṹ
        }
        if(m_inDetail) qDebug()<<t_carNum+" money is "+QString::number(t_money);
    }
    return result;
}

//�����ݿ�������
bool QtSqlite::UpdateMoneytoSql(QString t_carNum, int t_money)
{
    QString update_sql =        //�������SQL���
            QString("update CarToMoney set Money = \"%1\" where CarNum = \"%2\"").arg(t_money).arg(t_carNum);
    if(m_isDebug) qDebug()<<update_sql;
    sql_query.prepare(update_sql);
    bool result = sql_query.exec();     //��ȡ�������
    if(!result)     //����ʧ��
    {
        qDebug()<<sql_query.lastError();        //��ӡ������Ϣ
    }
    else        //���³ɹ�
    {
        qDebug()<<t_carNum+"`s money is updated as "+QString::number(t_money);        //��ӡ�ɹ���Ϣ
        if(t_money<10)
            //��Ҫ��UI������ʾ
            qDebug()<<t_carNum + " needs recharge!";
    }
    return result;
}

//�Ƿ����������Ϣ
bool QtSqlite::OutputInformation(bool t_isdebug, bool t_indetail)
{
    m_isDebug = t_isdebug;
    m_inDetail = t_indetail;
    return true;
}

//ע�����û�
bool QtSqlite::SignUptoSql(QString t_carNum, int t_money)
{
    QString insert_sql =        //��������SQL���
            QString("insert into CarToMoney values (\"%1\",%2)").arg(t_carNum).arg(t_money);
    if(m_isDebug)
        qDebug()<<insert_sql;       //��ӡSQL��䣬�������
    sql_query.prepare(insert_sql);      //��������
    bool result = sql_query.exec();     //��ȡ�������
    if(!result)       //�������ݳ���
    {
        qDebug()<<sql_query.lastError();        //��ӡ������Ϣ
    }
    else        //�������ݳɹ�
    {
        //��ӡ�ɹ���Ϣ
       if(m_inDetail)
           qDebug()<<QString("insert into CarToMoney values (\"%1\",%2)").arg(t_carNum).arg(t_money);
    }
    return result;      //������������ظ����ú���
}
