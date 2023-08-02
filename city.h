#ifndef CITY_H
#define CITY_H

#include <QMap>
#include <QFile>
#include <QString>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>

class city
{
public:
    city() {}

    QString getCityCode(QString cityName);


private:
    static QMap<QString,QString> mCityMap;

    // 初始化城市数据,读取解析一个json文件
      static void initCityCode()
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
};

QMap<QString,QString> city::mCityMap = {};
#endif // CITY_H
