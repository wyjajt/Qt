#ifndef XMODBUS_H
#define XMODBUS_H

/*********************************************************************************
  *Copyright(C): nys
  *Version:      1.0.1
  *Date:         2018.07.31
  *Description:  XModbus, a C++ Class which can establish and manager a modbus connection,
  *              making it convenient for other Class to read and write register values.
  *
  *Others:       need c++11 and above
  *Function List:
     1.
     2.
     3.
     4.
     5.
     6.
     7.
     8.
     9.
  *History:
     1.Date:
       Author:
       Modification:
     2.Date:
       Author:
       Modification:

**********************************************************************************/

#include <QString>
#include "xLog.h"
#include "xBaseDefine.h"
#include "modbus-rtu.h"

//解决中文显示乱码问题
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif


#define PROTOCOL_TCP        "TcpModbus"
#define PROTOCOL_SERIAL     "SerialModbus"


struct ConnectParameter
{
    QString     protocol;       //communication type --- tcp/serial
    QString     portName;       //transform to char*
    int         baudRate;
    QString     parity;         //transform to char  --- 'N'/'O'/'E'
    int         dataBit;        //int                --- 5/6/7/8
    float       stopBit;        //float              --- 1.0/1.5/2.0
    QString     ipAddress;      //transform to char*
    int         portNumber;
    int         startRegisterID;
    int         registerNumber;
    int         slaveAddress;
};

enum DataBit{
    DataBit_5 = 5,
    DataBit_6,
    DataBit_7,
    DataBit_8
};

class XModbus
{
public:
    XModbus( QString logName );
    ~XModbus();

    void    setParameters( ConnectParameter para );
    bool    connectToModbus();
    void    disconnectFromSlave();
    bool    readRegisterValues( int address, int number );                      //读取一个或多个寄存器值
    bool    writeRegisterValues( int address, int number, ushort *data );       //写一个或多个寄存器值

private:
    void    initDataBuffer();           //读所有寄存器的值, 初始化buffer
    bool    reconnectToModbus();        //modbus重连
    bool    readAllowedLengthOfValues( int address, int number );               //读最多125个寄存器
    bool    writeAllowedLengthOfValues( int address, int number, ushort *dest );//写最多125个寄存器

public:
    ushort      *dataBuffer;
    bool        isConnected;

private:
    ConnectParameter    modbusParameter;//modbus连接参数

    modbus_t    *modbusClient;          //modbus主机类
    XLogPtr     logger;                 //日志变量名


};

#endif // XMODBUS_H
