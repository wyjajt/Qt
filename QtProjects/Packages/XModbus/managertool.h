#ifndef MANAGERTOOL_H
#define MANAGERTOOL_H

#include <QMainWindow>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include "xmodbus.h"


#define  LOG_NAME   "TestModbus"


namespace Ui {
class ManagerTool;
}

class ManagerTool : public QMainWindow
{
    Q_OBJECT

public:
    explicit ManagerTool(QWidget *parent = 0);
    ~ManagerTool();

private:
    void    initParameterAndUi();   //初始化界面配置参数
    void    getParameterFromUi();   //获取界面配置的连接参数
    void    initSignalConnection(); //初始化信号槽连接

public Q_SLOTS:


private Q_SLOTS:
    void    connectToModbus();      //连接modbus从机
    void    disConnectFromModbus(); //断开modbus连接
    void    readDataBuffer();       //读缓冲区数据
    void    readRegisterValues();   //读寄存器
    void    writeSingleValueToRegisters();      //向一个或多个寄存器写相同值
    void    writeMultipleValuesToRegisters();   //向一个或多个寄存器写相同值

private:
    Ui::ManagerTool     *ui;
    XLogPtr             logger;     //日志记录对象
    XModbus             *xModbus;
    ConnectParameter    parameter;

};

#endif // MANAGERTOOL_H
