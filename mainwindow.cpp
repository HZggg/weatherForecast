#include "mainwindow.h"
#include "ui_mainwindow.h"

#define DISTANCE 10
#define TEXT_OFFSET_X 12//文本偏移
#define TEXT_OFFSET_Y 10//文本偏移

int mHightTemp[6];

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);



    //设置界面无边框
    setWindowFlag(Qt::FramelessWindowHint);
    //固定界面大小
    setFixedSize(800,450);


    //构建右键退出菜单
    mExitMenu = new QMenu(this);

    mExitAction = new QAction();
    mExitAction->setText("退出");
    mExitAction->setIcon(QIcon(":/weatherImages/close.png"));

    mExitMenu->addAction(mExitAction);
    //关联退出信号
    connect(mExitAction,&QAction::triggered,this,[=](){
        qApp->exit(0);
    });


    //关联请求服务器数据，HTTP请求
    mNetAccessManager = new QNetworkAccessManager(this);
    //请求完成即finished之后，就会自动onReplied函数
    connect(mNetAccessManager,&QNetworkAccessManager::finished,this,&MainWindow::onReplied);

    //直接获取北京天气数据信息
//    getWeatherInfo("101010100");
//    getWeatherInfo("101280101");
      getWeatherInfo("广州");


    // UI初始化
    //将控件放到数组里面，方便使用循环进行处理
    //星期和日期
    mWeekList << ui->lblWeek0 << ui->lblWeek1 << ui->lblWeek2 << ui->lblWeek3 << ui->lblWeek4 << ui->lblWeek5;
    mDateList << ui->lblDate0 << ui->lblDate1 << ui->lblDate2 << ui->lblDate3 << ui->lblDate4 << ui->lblDate5;
    //天气和天气图标
    mTypeList << ui->lblType0 << ui->lblType1 << ui->lblType2 << ui->lblType3 << ui->lblType4 << ui->lblType5;
    mTypeIconList << ui->lblTypeIcon0 << ui->lblTypeIcon1 << ui->lblTypeIcon2 << ui->lblTypeIcon3 << ui->lblTypeIcon4 << ui->lblTypeIcon5;
    //天气污染指数，优……
    mAqiList << ui->lblQuality0 << ui->lblQuality1 << ui->lblQuality2 << ui->lblQuality3 << ui->lblQuality4 << ui->lblQuality5;
    //风向，风力
    mFxList << ui->lblFx0 << ui->lblFx1 << ui->lblFx2 << ui->lblFx3 << ui->lblFx4 << ui->lblFx5;
    mFlList << ui->lblFl0 << ui->lblFl1 << ui->lblFl2 << ui->lblFl3 << ui->lblFl4 << ui->lblFl5;

    //图标

    mTypeIconMap.insert("晴", ":/weatherImages/type/Qing.png");
    mTypeIconMap.insert("多云", ":/weatherImages/type/DuoYun.png");
    mTypeIconMap.insert("阴", ":/weatherImages/type/Yin.png");
    mTypeIconMap.insert("雨", ":/weatherImages/type/Yu.png");
    mTypeIconMap.insert("雪", ":/weatherImages/type/Xue.png");
    mTypeIconMap.insert("沙尘暴", ":/weatherImages/type/ShaChenBao.png");

    mTypeIconMap.insert("雷阵雨", ":/weatherImages/type/LeiZhenYu.png");
    mTypeIconMap.insert("大雨", ":/weatherImages/type/DaYu.png");
    mTypeIconMap.insert("小雨", ":/weatherImages/type/XiaoYu.png");
    mTypeIconMap.insert("中雨", ":/weatherImages/type/ZhongYu.png");
    mTypeIconMap.insert("阵雨", ":/weatherImages/type/ZhenYu.png");
    mTypeIconMap.insert("暴雨", ":/weatherImages/type/BaoYu.png");
    mTypeIconMap.insert("大暴雨", ":/weatherImages/type/DaBaoYu.png");
    mTypeIconMap.insert("大到暴雨",":/weatherImages/type/DaDaoBaoYu.png");
    mTypeIconMap.insert("暴雨到大暴雨",":/weatherImages/type/BaoYuDaoDaBaoYu.png");
    mTypeIconMap.insert("大暴雨到大暴雨",":/weatherImages/type/DaBaoYuDaoDaBaoYu.png");

    mTypeIconMap.insert("暴雪",":/weatherImages/type/BaoXue.png");
    mTypeIconMap.insert("大到暴雪",":/weatherImages/type/DaDaoBaoXue.png");
    mTypeIconMap.insert("大雪", ":/weatherImages/type/DaXue.png");
    mTypeIconMap.insert("小雪", ":/weatherImages/type/XiaoXue.png");
    mTypeIconMap.insert("中雪", ":/weatherImages/type/ZhongXue.png");


    mTypeIconMap.insert("雨夹雪", ":/weatherImages/type/YuJiaXue.png");
    mTypeIconMap.insert("霾", ":/weatherImages/type/Mai.png");
    mTypeIconMap.insert("扬沙", ":/weatherImages/type/YangSha.png");
    mTypeIconMap.insert("沙尘暴", ":/weatherImages/type/ShaChenBao.png");
    mTypeIconMap.insert("特大暴雨", ":/weatherImages/type/TeDaBaoYu.png");
    mTypeIconMap.insert("乌", ":/weatherImages/type/Wu.png");
    mTypeIconMap.insert("小到中雨", ":/weatherImages/type/XiaoDaoZhongYu.png");
    mTypeIconMap.insert("小到中雪", ":/weatherImages/type/XiaoDaoZhongXue.png");
    mTypeIconMap.insert("雨夹雪", ":/weatherImages/type/YuJiaXue.png");
    mTypeIconMap.insert("阵雪", ":/weatherImages/type/ZhenXue.png");

    //温度曲线
    //步骤1 安装事件过滤器
    ui->lblHightCurve->installEventFilter(this);
    ui->lblLowCurve->installEventFilter(this);
}

//重写父类的虚函数,父类中默认的实现 是忽略右键菜单事件,重写之后，就可以处理右键菜单
//鼠标右键点击事件重写
void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    //右键弹出退出选项，QCursor::pos()在鼠标当前位置显示一个弹出菜单
    mExitMenu->exec(QCursor::pos());
}


//界面随鼠标移动
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    mOffset = event->globalPos() - this->pos();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos() - mOffset);
}


//根据城市名字获取城市编码
void MainWindow::getWeatherInfo(QString city_name)
{
    QString cityCode = getCityCode(city_name);

    if(cityCode.isEmpty())
    {
       QMessageBox::warning(this,"错误","请检查是否输入正确的城市",QMessageBox::Ok);
       return;
    }
    QUrl url("http://t.weather.itboy.net/api/weather/city/" + cityCode);
    mNetAccessManager->get(QNetworkRequest(url)); //get请求完成就会是finished，上面的槽函数onReplied就会被调用
}

QString MainWindow::getCityCode(QString cityName)
    {
        // 初始化map
        if (mCityMap.isEmpty())
        {
            initCityCode();
        }
         // 根据城市名字遍历map，获取城市编码
        QMap<QString, QString>::iterator it = mCityMap.find(cityName);
        // 特殊：输入北京/北京市都可以找到
        if (it == mCityMap.end())
        {
            it = mCityMap.find(cityName + "市");
        }
        if (it == mCityMap.end())
        {
            it = mCityMap.find(cityName + "区");
        }
        if (it == mCityMap.end())
        {
            it = mCityMap.find(cityName + "县");
        }
         // 如果遍历到有这个城市名字，就返回编码
        if (it != mCityMap.end())
        {
            return it.value();
        }
        return ""; // 没有就返回空
    }


void MainWindow::initCityCode()
{
    // 读取文件
    QFile file(":/citycode.json");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // 处理文件打开失败的情况
        return;
    }
    QByteArray json = file.readAll();
    file.close();
     // 解析json，并且写入到map，这个文件是json数组
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    if (error.error != QJsonParseError::NoError || !doc.isArray())
    {
        // 处理JSON解析失败的情况
        return;
    }
     // 然后获取json数据当中的城市名字和对应的编码
    QJsonArray cityArr = doc.array();
    for (int i = 0; i < cityArr.size(); i++)
    {
        QString city = cityArr[i].toObject().value("city_name").toString();
        QString code = cityArr[i].toObject().value("city_code").toString();
         // 然后写入到map
        // 注意当输入是省份时，是没有城市编码的，不可以查整个省份的天气，只能是具体的城市
        if (!code.isEmpty())
        {
            mCityMap.insert(city, code);
        }
    }
}
//搜索按钮点击
void MainWindow::on_btnSearch_clicked()
{
    QString cityname = ui->Cityline->text();
    getWeatherInfo(cityname);
}

//输入框回车
void MainWindow::on_Cityline_returnPressed()
{
    QString cityname = ui->Cityline->text();
    getWeatherInfo(cityname);
}


//请求服务器数据
void MainWindow::onReplied(QNetworkReply *reply)
{
    qDebug() << "onReplied successfully!" ;

    //响应的信息
    // 响应的状态码为200，表示请求成功
    int status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    qDebug() << "operation: " << reply->operation();    //请求方式
    qDebug() << "status_code: " << status_code;         //状态码
    qDebug() << "url: " << reply->url();                //url
    qDebug() << "raw Header: " << reply->rawHeaderList(); //响应header

    if(reply->error() != QNetworkReply::NoError || status_code != 200)
    {
        //errorString()函数返回一个QString类型的错误字符串，而toLatin1()将该QString转换为使用Latin-1编码的QByteArray类型。
        //最后，data()函数返回QByteArray的数据指针
        qDebug() << reply->errorString().toLatin1().data();
        QMessageBox::warning(this,"提示","请求数据失败！",QMessageBox::Ok);
    }else{
        //获取响应信息
        QByteArray reply_data = reply->readAll();
        QByteArray byteArray = QString(reply_data).toUtf8();
        qDebug() << "read All: " << byteArray.data();

        //获取到json数据后进行解析
        parseJson(byteArray);
    }

    //不然会造成内存泄漏
    reply->deleteLater();
}

//解析数据
void MainWindow::parseJson(QByteArray &byteArray)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(byteArray,&error); //如果报错就会把错误信息写到error里
    //报错直接退出
    if(error.error != QJsonParseError::NoError)
    {
        return;
    }

    QJsonObject rootObj = doc.object();
//    qDebug() << rootObj.value("message").toString();

    //1.解析城市跟日期
    mToday.city = rootObj.value("cityInfo").toObject().value("city").toString();
    mToday.date = rootObj.value("date").toString();

    //2.解析昨天yesterday数据
    QJsonObject dataObj = rootObj.value("data").toObject();

    QJsonObject yesterdayObj = dataObj.value("yesterday").toObject();
    mDay[0].week = yesterdayObj.value("week").toString();
    mDay[0].date = yesterdayObj.value("ymd").toString();
    //天气类型
    mDay[0].type = yesterdayObj.value("type").toString();

    //空气指数
    mDay[0].aqi =yesterdayObj.value("aqi").toDouble();

    //温度，要分割取数据,"high":"高温 32℃"
    QString highS;
    highS = yesterdayObj.value("high").toString().split(" ").at(1); //高温 32℃ -> 32°C ，按空格切割，取第二个数据
    mDay[0].high = highS.left(highS.length() - 1).toInt(); //长度减一，从左边开始计算取，32°c -> 32 ,在转换为int
    QString lowS;
    lowS = yesterdayObj.value("low").toString().split(" ").at(1);
    mDay[0].low = lowS.left(lowS.length() - 1).toInt();

    //风向，风力
    mDay[0].fx = yesterdayObj.value("fx").toString();
    mDay[0].fl = yesterdayObj.value("fl").toString();

    //3.解析forecast中5天的数据 ,forecast当中包括15天的数据，数组形式
    QJsonArray forecastArr = dataObj.value("forecast").toArray();
    for(int i=0 ; i<5 ; i++)
    {
        QJsonObject forecastObj = forecastArr[i].toObject();

        mDay[i+1].week = forecastObj.value("week").toString();
        mDay[i+1].date = forecastObj.value("ymd").toString();
        mDay[i+1].type = forecastObj.value("type").toString();
        //空气指数
        mDay[i+1].aqi =forecastObj.value("aqi").toDouble();

        //温度，要分割取数据,"high":"高温 32℃"
        QString highS;
        highS = forecastObj.value("high").toString().split(" ").at(1); //高温 32℃ -> 32°C ，按空格切割，取第二个数据
        mDay[i+1].high = highS.left(highS.length() - 1).toInt(); //长度减一，从左边开始计算取，32°c -> 32 ,在转换为int
        QString lowS;
        lowS = forecastObj.value("low").toString().split(" ").at(1);
        mDay[i+1].low = lowS.left(lowS.length() - 1).toInt();

        //风向，风力
        mDay[i+1].fx = forecastObj.value("fx").toString();
        mDay[i+1].fl = forecastObj.value("fl").toString();
    }

    //4.解析今天的数据
//    mToday.wendu =dataObj.value("wendu").toInt(); //不知道为啥这样直接写会显示为0,获取到数据了，但toInt()会不行
    QString wendu1 =dataObj.value("wendu").toString();
    mToday.wendu = wendu1.toInt();
//    qDebug() << wendu1;

    mToday.ganmao =dataObj.value("ganmao").toString();

    mToday.shidu = dataObj.value("shidu").toString();
    mToday.pm25 =dataObj.value("pm25").toInt();
    mToday.quality =dataObj.value("quality").toString();

    //还有一些今天的天气数据在forecast里面
    mToday.type = mDay[1].type;
    mToday.high = mDay[1].high;
    mToday.low = mDay[1].low;

    mToday.fx = mDay[1].fx;
    mToday.fl = mDay[1].fl;

    //更新UI
    updateUI();
}

//更新UI
void MainWindow::updateUI()
{
    //设置日期 城市
    //注意返回来的值为“20230728”这种格式，要进行转换2023/07/28
    ui->lblDate->setText(QDateTime::fromString(mToday.date,"yyyyMMdd").toString("yyyy/MM/dd") + " " + mDay[1].week);
    ui->lblCity->setText(mToday.city);

    //更新今天
    ui->lblTypeIcon->setPixmap(mTypeIconMap[mToday.type]);
    ui->lblTemperture->setText(QString::number(mToday.wendu));
    ui->lblTypeChange->setText(mToday.type);
    ui->lblLowHight->setText(QString::number(mToday.low) + "°" +"~" + QString::number(mToday.high) + "°");
    ui->lblGanmao->setText("感冒指数：" + mToday.ganmao);
    ui->lblWinFx->setText(mToday.fx);
    ui->lblWinFI->setText(mToday.fl);
    ui->lblPM25->setText(QString::number(mToday.pm25));
    ui->lblShiDu->setText(mToday.shidu);
    ui->lblQuality->setText(mToday.quality);


    //更新6天
    for(int i=0 ; i<6 ; i++)
    {
        //更新星期
        mWeekList[i]->setText("周" + mDay[i].week.right(1)); //数据是”星期六“,取右边第一位
        ui->lblWeek0->setText("昨天");
        ui->lblWeek1->setText("今天");
        ui->lblWeek2->setText("明天");

        //更新日期 ,数据是”2023-07-28“
        QStringList ymdList = mDay[i].date.split("-");
        mDateList[i]->setText(ymdList[1] + "/" + ymdList[2]);

        //更新天气类型
        mTypeList[i]->setText(mDay[i].type);
        mTypeIconList[i]->setPixmap(mTypeIconMap[mDay[i].type]);

        //更新空气质量
        if (mDay[i].aqi >= 0 && mDay[i].aqi <= 50)
        {
            mAqiList[i]->setText("优");
            mAqiList[i]->setStyleSheet("background-color: rgb(121, 184, 0);");
        }
        else if (mDay[i].aqi > 50 && mDay[i].aqi <= 100)
        {
            mAqiList[i]->setText("良");
            mAqiList[i]->setStyleSheet("background-color: rgb(255, 187, 23);");
        }
        else if (mDay[i].aqi > 100 && mDay[i].aqi <= 150)
        {
            mAqiList[i]->setText("轻度");
            mAqiList[i]->setStyleSheet("background-color: rgb(255, 87, 97);");
        }
        else if (mDay[i].aqi > 150 && mDay[i].aqi <= 200)
        {
            mAqiList[i]->setText("中度");
            mAqiList[i]->setStyleSheet("background-color: rgb(235, 17, 27);");
        }
        else if (mDay[i].aqi > 200 && mDay[i].aqi <= 250)
        {
            mAqiList[i]->setText("重度");
            mAqiList[i]->setStyleSheet("background-color: rgb(170, 0, 0);");
        }
        else
        {
            mAqiList[i]->setText("严重");
            mAqiList[i]->setStyleSheet("background-color: rgb(110, 0, 0);");
        }


        //更新风
        mFxList[i]->setText(mDay[i].fx);
        mFlList[i]->setText(mDay[i].fl);
    }

    //更新温度曲线
    ui->lblHightCurve->update();
    ui->lblLowCurve->update();
}

//注意这里要在更新UI函数这里调用Upadate函数，不然就会一直显示都是温度曲线0
//因为eventFilter会最先调用，请求服务器那些语句都还没执行，所以不会有数据
//在更新UI函数中，在调用一次eventFilter函数就会有数据，此时请求数据的那些函数已经被执行
//步骤2重写事件
bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{

    //处理绘图事件（QEvent::Paint）。
    //在这个事件处理函数中，首先通过检查事件的类型（event->type() == QEvent::Paint）来确定是否是绘图事件
    if(event->type() == QEvent::Paint)
    {
        //"watched" 是否等于 "ui->lblHight"，来判断是否是标签控件 "ui->lblHight" 的绘图事件
        if(watched == ui->lblHightCurve)
        {
            paintHight();
        }

        if(watched == ui->lblLowCurve)
        {
            paintLow();
        }
    }

    return QWidget::eventFilter(watched,event);
}


void MainWindow::paintHight()
{

    //在lblHighCurve上绘图
    QPainter painter(ui->lblHightCurve);
    //设置 QPainter::Antialiasing 标志来启用抗锯齿功能,产生更平滑的图形效果
    painter.setRenderHint(QPainter::Antialiasing,true);

    //1.计算温度点的坐标（X,Y）
    //X
    int pointX[6] = {0};
    for(int i = 0; i < 6 ;i++)
    {   //6个点，平均分成5等分
        //ui->lblHight->pos().x()用于获取lblHightCurve相对于其父级或包含的窗口的左边的x坐标，就是label的左边边边的位置
        //为了更好看，设置一个边距DISTANCE 10
        pointX[i] = mWeekList[i]->pos().x()+(mWeekList[i]->width()/2);
    }

    //Y
    int tempertureSum = 0;
    int tempertureAvg = 0;
    int yCenter = ui->lblHightCurve->height() / 2; //中心轴，平均温度绘制在这里
    int moveDistance = ui->lblHightCurve->height() / 20 ; //偏移量，根据实际温度与平均温度的差值*偏移量 来计算距离中心轴的距离
    for(int i = 0; i < 6 ;i++)
    {
        tempertureSum += mDay[i].high;
    }
    tempertureAvg = tempertureSum / 6;

    int pointY[6] = {0};
    for(int i = 0; i < 6 ;i++)
    {
        //y轴方向为向下
        pointY[i] = yCenter - (mDay[i].high - tempertureAvg)*moveDistance;
    }

    //2.设置画笔,字体
    QPen pen = painter.pen();
    pen.setWidth(2);
    pen.setColor(QColor(250,170,0));
    painter.setPen(pen);

    painter.setBrush(QColor(250,170,0));
    painter.setFont(QFont("Microsoft YaHei", 10));


    //3.画温度点，设置文本
    for(int i=0 ; i<6 ; i++)
    {
        painter.drawEllipse(QPoint(pointX[i],pointY[i]),2,2);
        painter.drawText(QPoint(pointX[i] - TEXT_OFFSET_X ,pointY[i] - TEXT_OFFSET_Y),QString::number(mDay[i].high) + "°");
    }

    //4、连线
    for(int i = 0 ; i < 5 ; i++)
    {
        //第一天是虚线
        if(i==0)
        {
            pen.setStyle(Qt::DotLine);
            painter.setPen(pen);
        }
        else
        {
            pen.setStyle(Qt::SolidLine);
            painter.setPen(pen);
        }
        painter.drawLine(QPoint(pointX[i],pointY[i]),QPoint(pointX[i+1],pointY[i+1]));
    }

}

void MainWindow::paintLow()
{
    //在lblHighCurve上绘图
    QPainter painter(ui->lblLowCurve);
    //设置 QPainter::Antialiasing 标志来启用抗锯齿功能,产生更平滑的图形效果
    painter.setRenderHint(QPainter::Antialiasing,true);

    //1.计算温度点的坐标（X,Y）
    //X
    int pointX[6] = {0};
    for(int i = 0; i < 6 ;i++)
    {   //6个点，平均分成5等分
        //ui->lblHight->pos().x()用于获取lblHightCurve相对于其父级或包含的窗口的左边的x坐标，就是label的左边边边的位置
        //为了更好看，设置一个边距DISTANCE 10
        pointX[i] = mWeekList[i]->pos().x()+(mWeekList[i]->width()/2);
    }

    //Y
    int tempertureSum = 0;
    int tempertureAvg = 0;
    int yCenter = ui->lblLowCurve->height() / 2; //中心轴，平均温度绘制在这里
    int moveDistance = ui->lblLowCurve->height() / 20 ; //偏移量，根据实际温度与平均温度的差值*偏移量 来计算距离中心轴的距离
    for(int i = 0; i < 6 ;i++)
    {
        tempertureSum += mDay[i].low;
    }
    tempertureAvg = tempertureSum / 6;

    int pointY[6] = {0};
    for(int i = 0; i < 6 ;i++)
    {
        //y轴方向为向下
        pointY[i] = yCenter - (mDay[i].low - tempertureAvg)*moveDistance;
    }

    //2.设置画笔,字体
    QPen pen = painter.pen();
    pen.setWidth(2);
    pen.setColor(QColor(0,255,255));
    painter.setPen(pen);

    painter.setBrush(QColor(0,255,255));
    painter.setFont(QFont("Microsoft YaHei", 10));


    //3.画温度点，设置文本
    for(int i=0 ; i<6 ; i++)
    {
        painter.drawEllipse(QPoint(pointX[i],pointY[i]),2,2);
        painter.drawText(QPoint(pointX[i] - TEXT_OFFSET_X ,pointY[i] - TEXT_OFFSET_Y),QString::number(mDay[i].low) + "°");
    }

    //4、连线
    for(int i = 0 ; i < 5 ; i++)
    {
        //第一天是虚线
        if(i==0)
        {
            pen.setStyle(Qt::DotLine);
            painter.setPen(pen);
        }
        else
        {
            pen.setStyle(Qt::SolidLine);
            painter.setPen(pen);
        }
        painter.drawLine(QPoint(pointX[i],pointY[i]),QPoint(pointX[i+1],pointY[i+1]));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
