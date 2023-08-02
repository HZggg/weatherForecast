#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QMouseEvent>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QPainter>

#include "weatherdata.h"

extern int mHightTemp[6];


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void contextMenuEvent(QContextMenuEvent *event) override; //鼠标右键点击事件

    void mousePressEvent(QMouseEvent *event) override; //窗口左键移动
    void mouseMoveEvent(QMouseEvent *event) override;

//    void getWeatherInfo(QString cityCode); //获取天气数据
    void getWeatherInfo(QString city_name); //获取天气数据

    void parseJson(QByteArray & byteArray); //解析json

    void updateUI(); //更新UI

    //weatherTool的数据，懒得分开了，注意报错什么多重定义的，不要头文件 #include "weathertool.h" 的引入放到这里
    // 在头文件中定义了静态变量，然后在多个源文件中包含了这个头文件，
    // 就会出现同一个变量被多次定义的情况，从而导致链接错误,所以放在.cpp中
    void initCityCode(); // 初始化城市数据,读取解析一个json文件，将数据存到map里面
    QString getCityCode(QString cityName); //在map里面根据城市名字找对应编码


    bool eventFilter(QObject *watched, QEvent *event) override; //重写绘图事件

    //绘制曲线
    void paintHight();
    void paintLow();


private slots:

    void onReplied(QNetworkReply* reply);

    void on_btnSearch_clicked();

    void on_Cityline_returnPressed();

private:
    Ui::MainWindow *ui;

    QMenu* mExitMenu; //右键退出的菜单
    QAction* mExitAction; //退出的行为-菜单项

    QPoint mOffset;  //窗口移动时，鼠标与天气界面窗口左上角的偏移

    QNetworkAccessManager * mNetAccessManager; //http请求


    Today mToday; //json解析到到数据存到这两个类里面
    Day mDay[6]; //6天的数据
    
    
    //界面右边部分label名称，用数组存起来，用于ui更新
    //星期和日期
    QList<QLabel* > mWeekList;
    QList<QLabel* > mDateList;
    //天气和天气图标
    QList<QLabel* > mTypeList;
    QList<QLabel* > mTypeIconList;
    //天气污染指数，优……
    QList<QLabel* > mAqiList;
    //风向，风力
    QList<QLabel* > mFxList;
    QList<QLabel* > mFlList;

    //图标
    QMap<QString,QString> mTypeIconMap;

    //城市编码
    QMap<QString,QString> mCityMap;

};

#endif // MAINWINDOW_H
