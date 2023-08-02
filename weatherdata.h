#ifndef WEATHERDATA_H
#define WEATHERDATA_H

#include <QString>

//根据我们之前设计的UI布局的lable名称来获取设计对应的数据名称
class Today
{
public:
    Today()
    {
        city ="广州";
        date ="2023-7-28";

        wendu = 0;
        type ="多云";
        high = 30;
        low = 18;

        ganmao ="感冒指数";

        shidu = "0%";
        pm25 = 0;
        fx ="南风";
        fl ="2级";
        quality ="无数据";
    }

    QString city;
    QString date;

    int wendu;
    QString type;

    int high;
    int low;

    QString ganmao;

    QString shidu;
    int pm25;
    QString fx;
    QString fl;
    QString quality;
};

class Day
{
public:
    Day()
    {
        week ="周五";
        date ="2023-7-29";

        type ="多云";

        aqi = 0; //空气指数，优

        high = 0;
        low = 0;

        fx ="南风";
        fl ="2级";
    }

    QString week;
    QString date;

    QString type;

    int aqi;

    int high;
    int low;

    QString fx;
    QString fl;
};

#endif // WEATHERDATA_H
