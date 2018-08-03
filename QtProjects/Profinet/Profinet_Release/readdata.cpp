#include "readdata.h"

ReadData::ReadData()
{
    actionInfo    = -1;
    pos_x_1       = 0;
    pos_y_1       = 0;
    pos_x_2       = 0;
    pos_y_2       = 0;
    detect_result = 0;
}

QString ReadData::toQString()
{
    QStringList data;
    data.clear();

    data << QString::number( actionInfo )
         << QString::number( pos_x_1 )
         << QString::number( pos_y_1 )
         << QString::number( pos_x_2 )
         << QString::number( pos_y_2 )
         << QString::number( detect_result );

    return data.join("*$#*");
}

void ReadData::fromQString(QString str)
{
    QStringList list = str.split("*$#*");

    actionInfo    = list.at(0).toInt();
    pos_x_1       = list.at(1).toInt();
    pos_y_1       = list.at(2).toInt();
    pos_x_2       = list.at(3).toInt();
    pos_y_2       = list.at(4).toInt();
    detect_result = list.at(5).toInt();
}
