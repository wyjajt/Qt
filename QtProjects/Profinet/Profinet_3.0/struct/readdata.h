#ifndef READDATA_H
#define READDATA_H
#include "include/defines.h"
#include <QStringList>

#include "CameraApi.h"

//坐标结构体
class ReadData
{
public:
    int actionInfo;
    int pos_x_1;
    int pos_y_1;
    int pos_x_2;
    int pos_y_2;
    int detect_result;

public:
    //默认构造
    ReadData();

    //转换为QString
    QString toQString();
    void    fromQString( QString str );


};
Q_DECLARE_METATYPE(ReadData);


#endif // READDATA_H
