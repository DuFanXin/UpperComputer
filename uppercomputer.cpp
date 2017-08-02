#include "uppercomputer.h"
#include "ui_uppercomputer.h"

UpperComputer::UpperComputer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::UpperComputer)
{
    ui->setupUi(this);
    connect(this,SIGNAL(carLeave(int)),this,SLOT(handleCarLeave(int)));
    connect(this,SIGNAL(carDistri(QString)),this,SLOT(distributeCar(QString)));
    connect(this,SIGNAL(carIn(int)),this,SLOT(handleCarIn(int)));
    //connect(ui->pushButton_Distribute,SIGNAL(clicked()),this,SLOT(test()));
    connect(this,SIGNAL(readyToInit()),this,SLOT(sysInit()));
    connect(ui->action_signUp,SIGNAL(triggered()),this,SLOT(signUp()));
    connect(ui->action_recharge,SIGNAL(triggered()),this,SLOT(recharge()));
    //connect(ui->pushButton_Distribute,SIGNAL(clicked()),this,SLOT(update()));
    parks[1].statu = "Free";
    parks[1].carNum = "NONE";
    parks[1].inTime = "NONE";
    parks[2].statu = "Waiting";
    parks[2].carNum = "E1234";
    parks[2].inTime = "NONE";
    m_Hum=100;
    m_Temp=50;
    m_MoneyPerSec=1;
    //m_Path = QDir::currentPath();
    m_Alarm = false;
    m_Alarm_ison = false;
    m_Movie_Alarm = new QMovie(":src/FireAlarm.gif");
    m_Movie_Call = new QMovie(":src/call.gif");
    m_signup = new SignUp();
    m_recharge = new Recharge();
    emit readyToInit();
    qDebug()<<"system in";
}

UpperComputer::~UpperComputer()
{
    delete ui;
    delete m_sql;
    delete m_signup;
    delete m_Movie_Alarm;
    delete m_Movie_Call;
    if(myCom->isOpen()){
        myCom->close();
        delete myCom;
        qDebug()<<"port is closed";
    }
    qDebug()<<"system out";
    this->close();
}

//���䳵λ
bool UpperComputer::distributeCar(QString carNum)
{
    //���䳵λ
    int distruNum=-1;
    for(int i=0;i<maxn;i++){
        if(parks[i].statu == "Free"){
            distruNum  = i;
            break;
        }
    }
    if(distruNum==-1){
        qDebug()<<"no more room";
        ui->textBrowser_Information->append(carNum+" no more room ");
        return false;
    }
    else{
        qDebug()<<carNum+" is distributed to park "+ QString::number( distruNum);
        ui->textBrowser_Information->append(carNum+" is distributed to park "+ QString::number( distruNum+1));
    }
    parks[distruNum].statu = "Waiting";
    parks[distruNum].carNum = carNum;
    return true;
}

//���������복λ
bool UpperComputer::handleCarIn(int parkNum)
{
    //�����Ǵ�0��ʼ��
    parkNum--;
    if(isDebug)
        qDebug()<<"handleCarIn";
    //��¼�ó�����ʱ��
    QDateTime inTime = QDateTime::currentDateTime();
    QString inTimeStr = inTime.toString("yyyy-MM-dd hh:mm:ss");
    qDebug()<<parks[parkNum].carNum+" in park"+QString::number(parkNum)+" at "+inTimeStr;
    ui->textBrowser_Information->append(parks[parkNum].carNum+" in park"+QString::number(parkNum)+" at "+inTimeStr);


    //������ʾ���������ʱ��
    parks[parkNum].statu = "Occupied";
    parks[parkNum].inTime = inTimeStr;
    m_sql->InsertTimetoSql( parks[parkNum].carNum,parkNum,inTime,"IN");        //���ò���ʱ�亯��
    return true;
}

//�������뿪
bool UpperComputer::handleCarLeave(int parkNum)
{
    //�����0��ʼ
    parkNum--;

    if(isDebug)
        qDebug()<<"handleCarLeave";

    //��¼��ǰʱ��
    QDateTime outTime = QDateTime::currentDateTime();
    QString outTimeStr = outTime.toString("yyyy-MM-dd hh:mm:ss");
    qDebug()<<parks[parkNum].carNum+" out park"+QString::number(parkNum)+" at "+outTimeStr;
    ui->textBrowser_Information->append(parks[parkNum].carNum+" out park"+QString::number(parkNum)+" at "+outTimeStr);

    //��ѯ�ó�����ʱ��
    QDateTime inTime;
    m_sql->GetTimefromSql(parks[parkNum].carNum,parkNum,inTime);

    //��ͣ����¼�в��복���뿪ʱ��
    m_sql->InsertTimetoSql(parks[parkNum].carNum,parkNum,outTime,"OUT");

    //��ѯ���
    int money;
    m_sql->GetMoneyformSql(parks[parkNum].carNum,money);

    //����ͣ��ʱ��
    int timelong = inTime.secsTo(outTime);
    money -= m_MoneyPerSec*timelong;

    //�ڽ�����ʾ��Ϣ
    ui->textBrowser_Information->append(parks[parkNum].carNum+
                                        " charge "+
                                        QString::number(m_MoneyPerSec*timelong)+
                                        "$ for parking "+
                                        QString::number(timelong)+" seconds ");
    ui->textBrowser_Information->append(parks[parkNum].carNum+
                                        " remain "+
                                        QString::number(money)+"$");

    //���ø�������
    m_sql->UpdateMoneytoSql(parks[parkNum].carNum,money);

    parks[parkNum].statu = "Free";
    parks[parkNum].carNum = "NONE";
    parks[parkNum].inTime = "NONE";
    return true;
}

//����ϵͳ
bool UpperComputer::test()
{
    //m_label_time[0]->setText("fdsfdsfsd");
    return true;
}

//ϵͳ��ʼ��
bool UpperComputer::sysInit()
{
    //��ʼ�����ݿ�
    m_sql = new QtSqlite();
    ui->textBrowser_Information->setText("system initializing......");
    m_sql->OutputInformation(isDebug,inDetail);
    if(m_sql->initDataBase()){
        ui->textBrowser_Information->append("succeed in database initialization");
    }
    else{
        ui->textBrowser_Information->append("fail to initialize database");
    }

    //��ʼ����ȡ��ʱ����ʱ���
    timerdly = TIMER_INTERVAL;

    //��ʼ���������ͼ�ʱ����ʱ���
    obotimerdly = OBO_TIMER_INTERVAL;

    //���ö�ȡ��ʱ��
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(readMyCom()));
    ui->textBrowser_Information->append("succeed in time initialization");

    /*QPixmap picfree(":src/open.png");
    QPixmap picstop(":src/stop1.png");
    QPixmap pictemp(":src/wenduji.jpg");
    QPixmap picwait(":src/wait.GIF");
    ui->label_park1_statu_pic->setPixmap(picstop);
    ui->label_park2_statu_pic->setPixmap(picstop);
    ui->label_park3_statu_pic->setPixmap(picfree);
    ui->label_park4_statu_pic->setPixmap(picwait);
    ui->label_park5_statu_pic->setPixmap(picstop);
    ui->label_park6_statu_pic->setPixmap(picwait);*/

    //��ͼƬlabel���Ա������
    m_label_pic[0] = ui->label_park1_statu_pic;
    m_label_pic[1] = ui->label_park2_statu_pic;
    m_label_pic[2] = ui->label_park3_statu_pic;
    m_label_pic[3] = ui->label_park4_statu_pic;
    m_label_pic[4] = ui->label_park5_statu_pic;
    m_label_pic[5] = ui->label_park6_statu_pic;

    //����ʾʱ���label���Ա������
    m_label_time[0]=ui->label_park1_inTime_text;
    m_label_time[1]=ui->label_park2_inTime_text;
    m_label_time[2]=ui->label_park3_inTime_text;
    m_label_time[3]=ui->label_park4_inTime_text;
    m_label_time[4]=ui->label_park5_inTime_text;
    m_label_time[5]=ui->label_park6_inTime_text;

    //����ʾ���ŵ�label���Ա������
    m_label_carNum[0]=ui->label_park1_carNum_text;
    m_label_carNum[1]=ui->label_park2_carNum_text;
    m_label_carNum[2]=ui->label_park3_carNum_text;
    m_label_carNum[3]=ui->label_park4_carNum_text;
    m_label_carNum[4]=ui->label_park5_carNum_text;
    m_label_carNum[5]=ui->label_park6_carNum_text;

    //����ʾ���ŵ�label���Ա������
    m_label_statu[0]=ui->label_park1_statu_text;
    m_label_statu[1]=ui->label_park2_statu_text;
    m_label_statu[2]=ui->label_park3_statu_text;
    m_label_statu[3]=ui->label_park4_statu_text;
    m_label_statu[4]=ui->label_park5_statu_text;
    m_label_statu[5]=ui->label_park6_statu_text;

    //����ʾ���е�label���Ա������
    m_label_call[0]=ui->label_park1_call;
    m_label_call[1]=ui->label_park2_call;
    m_label_call[2]=ui->label_park3_call;
    m_label_call[3]=ui->label_park4_call;
    m_label_call[4]=ui->label_park5_call;
    m_label_call[5]=ui->label_park6_call;


    //showCall(6,true);
    QString portName = "COM7";   //��ȡ������
    myCom = new Win_QextSerialPort(portName, QextSerialBase::Polling);
    //����QextSerialBase::QueryModeӦ��ʹ��QextSerialBase::Polling

    if(myCom->open(QIODevice::ReadWrite)){
        ui->textBrowser_Information->append("open port");
        qDebug()<<"open port successfully";
    }
    else{
        //QMessageBox::critical(this, tr("��ʧ��"), tr("δ�ܴ򿪴��� ") + portName + tr("\n�ô����豸�����ڻ��ѱ�ռ��"), QMessageBox::Ok);
        qDebug()<<"fail to open port, the port is not exit or is being used";
        ui->textBrowser_Information->append("fail to open port, the port is not exit or is being used");
        return false;
    }

    //���ò�����
    myCom->setBaudRate((BaudRateType)15);

    //��������λ
    myCom->setDataBits((DataBitsType)3);

    //����У��
    myCom->setParity((ParityType)0);

    //����ֹͣλ
    myCom->setStopBits((StopBitsType)0);

    //������ȡ��ʱ��
    timer->start(timerdly);
    //��������������
    myCom->setFlowControl(FLOW_OFF);
    //������ʱ
    myCom->setTimeout(TIME_OUT);

    ui->textBrowser_Information->append("complete initialzing,system in......");
    return true;
}

//��ȡ��������
bool UpperComputer::readMyCom()
{
    QByteArray temp = myCom->readAll();

    if(!temp.isEmpty()){
        //sprintf(ctime, "%dhhgh%djfhhf%d", H, M, S);

        if(write2fileName.isEmpty()){
            qDebug()<< QString(temp);
            if(checkData(temp))
                dataJudge(temp);
        }
        else{
            QFile file(write2fileName);
            //�����ʧ���������ʾ���˳�����
            if(!file.open(QFile::Append | QIODevice::Text)){
                //QMessageBox::warning(this, tr("д���ļ�"), tr("���ļ� %1 ʧ��, �޷�д��\n%2").arg(write2fileName).arg(file.errorString()), QMessageBox::Ok);
                qDebug()<<"���ļ�ʧ��";
                return false;
            }
            QTextStream out(&file);
            out<<temp<<endl;
            file.close();
            //ui->textBrowser->append(tr("����:�����Ѿ�д���ļ� %1").arg(write2fileName));
        }

        //ui->recvbyteslcdNumber->display(ui->recvbyteslcdNumber->value() + temp.size());
        // ui->statusBar->showMessage(tr("�ɹ���ȡ%1�ֽ�����").arg(temp.size()));
    }
    return true;
}

//��ʪ��
bool UpperComputer::paintHum()
{
    QPainter painter(this);
    QString text,color="red";
    QFont font;
    QPoint Position;
    //int Hum=65;
    int PosX=0,PosY=0,Width=0,Height=0;
    text.setNum(m_Hum);
    /* ���û�����ɫ��https://www.w3.org/TR/SVG/types.html#ColorKeywords*/

    //�õ��߶�GoupBox������
    Position = ui->groupBox_Hum->pos();
    PosX = Position.x();
    PosY = Position.y();
    Width = ui->groupBox_Hum->width();
    Height = ui->groupBox_Hum->height();
    painter.save(); //��������ϵ״̬��
    painter.translate(PosX+Width/2,PosY+20+Height/2);
    painter.rotate(42);
    //painter.setPen(QPen(QBrush(),20));
    QPen pen;
    pen.setWidth(2);
    pen.setCapStyle(Qt::RoundCap);
    pen.setColor(Qt::red);
    //painter.setPen(QPen(Qt::red,15));
    painter.setPen(pen);
    for (int i = 0; i < m_Hum ; ++i) {
        //painter.setPen(QColor("limegreen"));
        painter.drawLine(0, 60, 0, 80);
        painter.rotate(2.8);
    }
    QPoint minu[3]={
        QPoint(26,0),
        QPoint(-25,0),
        QPoint(0,70)
    };

    //painter.drawLine(0, 0, 0, 80);
    pen.setColor(Qt::darkGray);
    painter.setPen(pen);
    for (int i = m_Hum; i < 100 ; ++i) {
        painter.drawLine(0, 60, 0, 80);
        painter.rotate(2.8);
    }
    painter.setBrush(QBrush(QColor("palegreen"),Qt::SolidPattern));//������Ȧʵ��Բ��
    painter.drawEllipse(-45,-45,90,90);
    painter.setBrush(QBrush(QColor(color),Qt::SolidPattern));//���м�ʵ��Բ�����ɱ�ɫ
    painter.drawEllipse(-35,-35,70,70);
    painter.rotate(-2.8*(100-m_Hum));
    painter.setBrush(QBrush(QColor(color)));//���м�ʵ��Բ�����ɱ�ɫ
    painter.drawConvexPolygon(minu,3);
    painter.setBrush(QBrush(QColor("lavenderblush"),Qt::SolidPattern));//�м����ף���ʾ�ı�
    painter.drawEllipse(-25,-25,50,50);
    painter.restore(); //�ָ���ǰ������ϵ״̬
    // �������壺΢���źڡ����С50��б��
    painter.setPen(QColor(color));
    font.setFamily("Microsoft YaHei");
    font.setPointSize(15);
    font.setItalic(true);
    painter.setFont(font);
    // �����ı�
    painter.drawText(PosX,PosY+20,Width,Height,Qt::AlignCenter, text);//QPoint(PosX+Width/4,PosY+Height*3/4)
    //m_Hum--;
    return true;
}

//�����棬ϵͳ����
void UpperComputer::paintEvent(QPaintEvent *event)
{
    //��ʪ��
    paintHum();

    //������
    paintAlarm();

    //��ʪ��
    paintTemp();

    //��������λ���
    paintPark();
}

//���ݴ���
bool UpperComputer::dataJudge(QByteArray str)
{
    //string str = Qstr.toUtf8().data();
    if(inDetail)
        qDebug()<<"In datajudge";
    switch(str[4]){

    //��λ״̬
    case '1':
        if(inDetail)
            qDebug()<<"park statu";
        if(str[9]=='1'&&parks[str[8]-'0'-1].statu=="Waiting")
            handleCarIn(str[8]-'0');
        if(str[9]=='0'&&parks[str[8]-'0'-1].statu=="Occupied")
            handleCarLeave(str[8]-'0');
        break;

    //��λ����
    case '2':
        if(inDetail)
            qDebug()<<"distri car";
        distributeCar(str.mid(5,5));
        break;

    //��ʪ�ȼ�������Ϣ
    case '3':
        if(inDetail)
            qDebug()<<"TempandHum ";
        m_Hum=(str[8]-'0')*10+(str[9]-'0');
        m_Temp=(str[5]-'0')*10+(str[6]-'0');
        //if(inDetail)
            qDebug()<<"Hum is "<<m_Hum,qDebug()<<" Temp is "<<m_Temp;
        if(str[7]=='0'){//����״̬
            m_Alarm  =false;
            if(inDetail)
                qDebug()<<"Fire Ok";
        }
        if(str[7]=='1'){//�¶Ⱦ���
            m_Alarm = true;
            if(inDetail)
                qDebug()<<"Fire Alarm";
        }
        break;

    //���������
    case '4'://���������
        if(inDetail)
            qDebug()<<"help";
        if(str[9]=='0')
            showCall(str[8]-'0',true);
        else
            showCall(str[8]-'0',false);
        break;
    default:
        if(inDetail)
            qDebug()<<"default";

    }
    this->update();
    return true;
}

//��ʾ��������
bool UpperComputer::paintAlarm()
{
    if(m_Alarm){        //��Ҫ��������
        if(!m_Alarm_ison){      //����δ��
        ui->label_Warning->setMovie(m_Movie_Alarm);
        m_Movie_Alarm->start();
        m_Alarm_ison = true;
        }
        //�����Ѿ����ˣ�����Ҫ�ڴ�����
    }
    else
    {       //����Ҫ����
        if(m_Alarm_ison){        //��������
            m_Movie_Alarm->stop();
            m_Alarm_ison = false;       //�ص�����
            ui->label_Warning->clear();
        }
        //QPainter painter(this);
        QString text ="  safe";
        QFont font;
        QPalette pa;
        pa.setColor(QPalette::WindowText,Qt::darkGreen);
        // �������壺΢���źڡ����С50��б��
        font.setFamily("Microsoft YaHei");
        font.setPointSize(60);
        ui->label_Warning->setFont(font);
        ui->label_Warning->setPalette(pa);
        ui->label_Warning->setText(text);
    }
    return true;
}

//���¶�
bool UpperComputer::paintTemp()
{
    QPainter painter(this);
    QPoint Position;
    QPen pen;
    QFont  font;
    font.setFamily("Microsoft YaHei");
    font.setPointSize(7);
    //font.setItalic(true);
    int PosX=0,PosY=0,Width=0,Height=0;
    /* ���û�����ɫ��https://www.w3.org/TR/SVG/types.html#ColorKeywords*/

    //�õ��߶�GoupBox������
    Position = ui->groupBox_Temp->pos();
    PosX = Position.x();
    PosY = Position.y() + 18;
    Width = ui->groupBox_Temp->width();
    Height = ui->groupBox_Temp->height();
    pen.setWidth(2);
    pen.setCapStyle(Qt::RoundCap);
    pen.setColor(Qt::red);
    painter.save(); //��������ϵ״̬��
    painter.translate(PosX+Width/2,PosY+Height-20);//+Width/2,+20+Height/2
    painter.rotate(90);
    pen.setWidth(2);
    pen.setCapStyle(Qt::RoundCap);
    pen.setColor(Qt::red);
    int base = -30;
    painter.drawLine(base, 10, base, 30);
    painter.drawLine(base, -10, base, -30);

    for (int i = 0; i < 5; ++i) {
        //painter.setFont(font);
        for(int j=1;j<5;j++){
            painter.drawLine(base-4*j, 10, base-4*j, 20);
            painter.drawLine(base-4*j, -10, base-4*j, -20);
        }
        base = base  - 4*5;
        painter.drawLine(base, 10, base, 25);
        painter.drawLine(base, -10, base, -25);
        for(int j=1;j<5;j++){
            painter.drawLine(base-4*j, 10, base-4*j, 20);
            painter.drawLine(base-4*j, -10, base-4*j, -20);
        }
        base = base  - 4*5;
        painter.drawLine(base, 10, base, 30);
        painter.drawLine(base, -10, base, -30);
    }
    painter.setBrush(QBrush(Qt::red,Qt::SolidPattern));//������Ȧʵ��Բ��
    painter.drawEllipse(-28,-28,56,56);
    painter.fillRect(7,-7,-4*m_Temp-38,14,Qt::red);
    painter.setBrush(QBrush(Qt::white,Qt::SolidPattern));//������Ȧʵ��Բ��
    painter.drawEllipse(-18,-18,36,36);
    painter.restore();
    painter.setFont(font);
    for(int i=1;i<6;i++){
        painter.drawText(36+PosX+Width/2,40*i+PosY+8,15,10,Qt::AlignCenter, QString::number((60-10*i)*1.8+32));//QPoint(PosX+Width/4,PosY+Height*3/4)
        painter.drawText(36+PosX+Width/2-80,40*i+PosY+8,10,10,Qt::AlignCenter, QString::number(60-10*i));//QPoint(PosX+Width/4,PosY+Height*3/4)
    }
    painter.drawText(36+PosX+Width/2-10,PosY+25,20,20,Qt::AlignCenter, "F");//QPoint(PosX+Width/4,PosY+Height*3/4)
    painter.drawText(36+PosX+Width/2-80,PosY+25,20,20,Qt::AlignCenter, "C");//QPoint(PosX+Width/4,PosY+Height*3/4)
    return true;
}

//��ʾ����ͣ������״̬
bool UpperComputer::paintPark()
{
    QPixmap picfree(":src/open.png");
    QPixmap picstop(":src/stop1.png");
    QPixmap picwait(":src/wait.png");
    for(int i=0;i<6;i++)
    {
        m_label_time[i]->setText(parks[i].inTime);
        m_label_carNum[i]->setText(parks[i].carNum);
        m_label_statu[i]->setText(parks[i].statu);
        if(parks[i].statu == "Free"){
            m_label_pic[i]->setPixmap(picfree);
        }
        if(parks[i].statu == "Waiting"){
            m_label_pic[i]->setPixmap(picwait);
        }
        if(parks[i].statu == "Occupied"){
            m_label_pic[i]->setPixmap(picstop);
        }
    }
    return true;
}

//���ʹ�����Ϣ
bool UpperComputer::sendMsg(QString qstr)
{
    myCom->write(qstr.toAscii());
    return true;
}

//���û�ע��
bool UpperComputer::signUp()
{
    m_signup->dataClear();
    m_signup->setWindowIcon(QIcon(":src/person.png"));
    m_signup->show();
    qDebug()<<"sign up";
    connect(m_signup,SIGNAL(readytoSignup(QString,int)),this,SLOT(handelSignup(QString,int)));
    //delete signup;
    return true;
}

//�������û����Ϻ������ݿ�ע��
bool UpperComputer::handelSignup(QString carNum, int money)
{
    if(m_sql->SignUptoSql(carNum,money)){
        if(inDetail)
            qDebug()<<"successfully signup "+carNum+" with "+QString::number(money);
        ui->textBrowser_Information->append("successfully signup "+carNum+" with "+QString::number(money));
        return true;
    }
    else{
        if(inDetail)
            qDebug()<<"fail to signup "+carNum+" with "+QString::number(money);
        ui->textBrowser_Information->append("fail to signup "+carNum+" with "+QString::number(money));
        ui->textBrowser_Information->append("Mabey the money is 0 or the car has already exited");
        return false;
    }
}

//�û���ֵ
bool UpperComputer::recharge()
{
    m_recharge->dataClear();
    m_recharge->setWindowIcon(QIcon(":src/money.jpg"));
    m_recharge->show();
    qDebug()<<"recharge";
    connect(m_recharge,SIGNAL(readytoRacharge(QString,int)),this,SLOT(handelRecharge(QString,int)));
    return true;
}

//�����û���ֵ���������ݿ����
bool UpperComputer::handelRecharge(QString carNum, int money)
{
    //��ѯ���
    int t_money;
    bool result;
    result = m_sql->GetMoneyformSql(carNum,t_money);

    money += t_money;

    //���ø�������
    result &= m_sql->UpdateMoneytoSql(carNum,money);
    if(result)
        ui->textBrowser_Information->append("successfully recharge "+carNum+" with total "+QString::number(money));
    return result;
}

//��ʾ����
bool UpperComputer::showCall(int parkNum, bool isCall)
{
    parkNum--;
    if(isCall){
        m_label_call[parkNum]->setMovie(m_Movie_Call);
        m_Movie_Call->start();
        if(inDetail)
            qDebug()<<"park"+QString::number(parkNum+1)+" calling!";
        ui->textBrowser_Information->append("park"+QString::number(parkNum+1)+" calling!");
    }
    else{
        m_label_call[parkNum]->clear();
        if(inDetail)
            qDebug()<<"park"+QString::number(parkNum+1)+" cancel the call";
        ui->textBrowser_Information->append("park"+QString::number(parkNum+1)+" cancel the call");
    }
    return true;
}

//���У���
bool UpperComputer::checkData(QByteArray qb)
{
    QByteArray T;
    T[0]=qb[1];
    T[1]=qb[2];
    T[2]=qb[3];
    T[3]=qb[4];
    T[4]=qb[5];
    T[5]=qb[6];
    T[6]=qb[7];
    T[7]=qb[8];
    T[8]=qb[9];
    int a=0,b=0,c,i;
    for(i=0;i<9;i+=2)
        a+=T[i];
    for(i=1;i<9;i+=2)
        b+=(3*T[i]);
    c=a+b;

    T[9]=(c/1000)%10+'0';

    T[10]=(c/100)%10+'0';
    T[11]=(c/10)%10+'0';
    T[12]=c%10+'0';

    qDebug()<<"checkdata "<<QString(T);
    if(T[9]==qb[10]&&T[10]==qb[11]&&T[11]==qb[12]&&T[12]==qb[13]){
        if(isDebug)
            qDebug()<<"data check ok";
        return true;
    }
    else{
        if(isDebug)
            qDebug()<<"data check wrong";
        return false;
    }
}
