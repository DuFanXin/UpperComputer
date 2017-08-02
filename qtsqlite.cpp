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

//初始化数据库
bool QtSqlite::initDataBase()
{
    bool result=true;
    database = QSqlDatabase::addDatabase("QSQLITE");        //加载数据库
    database.setDatabaseName("database.db");        //数据库名称
    database.setUserName("root");       //用户名
    database.setPassword("123456");     //密码

    //打开数据库
    if(!database.open())        //未成功打开
    {
        qDebug()<<database.lastError();     //打印错误信息
        qFatal("failed to connect.") ;
        return result = false;
    }
    else{
        qDebug()<<"success to connect";     //打印成功信息
        sql_query = QSqlQuery::QSqlQuery(database);     //绑定数据库
    }

    //若停车表不存在，创建表；若表存在，则不创建
    bool isTableExist =
            sql_query.exec(QString("select count(*) from %1").arg("CarParks"));     //测试表是否存在
    if(!isTableExist)       //如果表不存在
    {
        qDebug() << "CarParks table does not exist";     //打印不存在表的信息
        QString create_sql = "create table CarParks (CarNum varchar(30), ParkNum int, Time DATETIME primary key, Direction varchar(5))";
        sql_query.prepare(create_sql);      //创建表
        if(!sql_query.exec())
        {
            if(m_isDebug)
                qDebug()<<sql_query.lastError();        //创建不成功，打印信息
            result = false;
        }
        else
        {
            if(m_inDetail)
                qDebug()<<"CarParks table created!";     //创建成功
            QString insert_sql = QString("insert into CarParks values (\"EA12345\",3,\"2017-7-20 16 01 33\",\"OUT\")");
            qDebug()<<insert_sql;       //打印调试信息
            sql_query.prepare(insert_sql);      //插入数据
            if(!sql_query.exec())       //插入数据出错
            {
                if(m_isDebug)
                    qDebug()<<sql_query.lastError();        //打印错误信息
                result = false;
            }
            else        //插入数据成功
            {
                if(m_inDetail)
                    qDebug()<<"CarParks table test OK!";       //打印插入成功信息
            }
        }
    }
    else        //表存在
    {
        if(m_inDetail)
            qDebug() << "CarParks table exists";     //打印信息
    }

    //若车号与余额对应表不存在，创建表；若表存在，则不创建
    isTableExist =
            sql_query.exec(QString("select count(*) from %1").arg("CarToMoney"));     //测试表是否存在
    if(!isTableExist)       //如果表不存在
    {
        if(m_inDetail)
            qDebug() << "CarToPark table does not exist";     //打印不存在表的信息
        QString create_sql = "create table CarToMoney (CarNum varchar(30) primary key, Money int)";
        sql_query.prepare(create_sql);      //创建表
        if(!sql_query.exec())
        {
            if(m_isDebug)
                qDebug()<<sql_query.lastError();        //创建不成功，打印信息
            result = false;
        }
        else
        {
            if(m_inDetail)
                qDebug()<<"CarToMoney table created!";     //创建成功
            QString insert_sql = QString("insert into CarToMoney values (\"A8888\",100)");
            qDebug()<<insert_sql;       //打印调试信息
            sql_query.prepare(insert_sql);      //插入数据
            if(!sql_query.exec())       //插入数据出错
            {
                if(m_isDebug)
                    qDebug()<<sql_query.lastError();        //打印错误信息
                result = false;
            }
            else        //插入数据成功
            {
                if(m_inDetail)
                    qDebug()<<"CarToMoney table test OK!";       //打印插入成功信息
            }
        }
    }
    else        //表存在
    {
        if(m_inDetail)
            qDebug() << "CarToMoney table exists";     //打印信息
    }
    return result;
}

//向数据库插入时间
bool QtSqlite::InsertTimetoSql(QString t_carNum,int t_parkNum,QDateTime t_time,QString t_direction)
{
    //向数据库写进入时间
    QString inTimeStr = t_time.toString("yyyy-MM-dd hh mm ss");     //转换时间类型
    QString insert_sql =        //插入数据SQL语句
            QString("insert into CarParks values (\"%1\",%2,\"%3\",\"%4\")").arg(t_carNum).arg(t_parkNum).arg(inTimeStr).arg(t_direction);
    if(m_isDebug) qDebug()<<insert_sql;       //打印SQL语句，方便调试
    sql_query.prepare(insert_sql);      //插入数据
    bool result = sql_query.exec();     //获取操作结果
    if(!result)       //插入数据出错
    {
        qDebug()<<sql_query.lastError();        //打印错误信息
    }
    else        //插入数据成功
    {
        //打印成功信息
       if(m_inDetail) qDebug()<<QString("insert (%1,%2,\%3,\%4) into DataBase").arg(t_carNum).arg(t_parkNum).arg(inTimeStr).arg(t_direction);
    }
    return result;      //将操作结果返回给调用函数
}

//从数据库读取入位时间
bool QtSqlite::GetTimefromSql(QString t_carNum,int t_parkNum,QDateTime &t_time)
{
    QString select_sql =
            QString("select max(time) from CarParks where ParkNum = %1 and CarNum = \"%2\" and Direction = \"IN\"").arg(t_parkNum).arg(t_carNum);
    if(m_isDebug) qDebug()<<select_sql;
    sql_query.prepare(select_sql);      //查询信息
    bool result = sql_query.exec();
    if(!result)       //查询出错
    {
        qDebug()<<sql_query.lastError();        //打印错误信息
    }
    else
    {
        if(m_isDebug) qDebug()<<t_carNum+" parktime find successfully!";     //打印成功信息

        while(sql_query.next())     //输出查询结果
        {
            t_time = sql_query.value(0).toDateTime();      //转换数据结构
           if(m_inDetail) qDebug()<<"find the  intime of "+t_carNum+" in "+t_parkNum +" is "+t_time.toString("yyyy-MM-dd hh:mm:ss");
        }
    }
    return result;
}

//从数据库读取余额
bool QtSqlite::GetMoneyformSql(QString t_carNum, int &t_money)
{
    QString select_sql =    //查询余额的SQL语句
            QString("select money from CarToMoney where CarNum = \"%1\"").arg(t_carNum);
    if(m_isDebug) qDebug()<<select_sql;       //输出查询SQL语句，方便调试
    sql_query.prepare(select_sql);      //查询信息
    bool result = sql_query.exec();     //获取操作结果
    if(!result)       //查询出错
    {
        qDebug()<<sql_query.lastError();        //打印错误信息
    }
    else
    {
        if(m_inDetail) qDebug()<<t_carNum+" money find successfully!";     //打印成功信息
        while(sql_query.next())     //输出查询结果
        {
            t_money = sql_query.value(0).toInt();      //转换数据结构
        }
        if(m_inDetail) qDebug()<<t_carNum+" money is "+QString::number(t_money);
    }
    return result;
}

//在数据库更新余额
bool QtSqlite::UpdateMoneytoSql(QString t_carNum, int t_money)
{
    QString update_sql =        //更新余额SQL语句
            QString("update CarToMoney set Money = \"%1\" where CarNum = \"%2\"").arg(t_money).arg(t_carNum);
    if(m_isDebug) qDebug()<<update_sql;
    sql_query.prepare(update_sql);
    bool result = sql_query.exec();     //获取操作结果
    if(!result)     //更新失败
    {
        qDebug()<<sql_query.lastError();        //打印错误信息
    }
    else        //更新成功
    {
        qDebug()<<t_carNum+"`s money is updated as "+QString::number(t_money);        //打印成功信息
        if(t_money<10)
            //需要向UI界面显示
            qDebug()<<t_carNum + " needs recharge!";
    }
    return result;
}

//是否输出各类信息
bool QtSqlite::OutputInformation(bool t_isdebug, bool t_indetail)
{
    m_isDebug = t_isdebug;
    m_inDetail = t_indetail;
    return true;
}

//注册新用户
bool QtSqlite::SignUptoSql(QString t_carNum, int t_money)
{
    QString insert_sql =        //插入数据SQL语句
            QString("insert into CarToMoney values (\"%1\",%2)").arg(t_carNum).arg(t_money);
    if(m_isDebug)
        qDebug()<<insert_sql;       //打印SQL语句，方便调试
    sql_query.prepare(insert_sql);      //插入数据
    bool result = sql_query.exec();     //获取操作结果
    if(!result)       //插入数据出错
    {
        qDebug()<<sql_query.lastError();        //打印错误信息
    }
    else        //插入数据成功
    {
        //打印成功信息
       if(m_inDetail)
           qDebug()<<QString("insert into CarToMoney values (\"%1\",%2)").arg(t_carNum).arg(t_money);
    }
    return result;      //将操作结果返回给调用函数
}
