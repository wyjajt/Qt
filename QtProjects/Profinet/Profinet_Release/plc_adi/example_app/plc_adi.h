#ifndef PLC_ADI_H
#define PLC_ADI_H
/*******************************************************************************
********************************************************************************
**                                                                            **
** ABCC Starter Kit version 2.01.01 (2015-12-14)                              **
**                                                                            **
** Delivered with:                                                            **
**    ABCC Driver 4.01.01 (2015-12-14)                                        **
**    ABP         7.16.01 (2015-10-14)                                        **
**                                                                            */
/*
** This is a workaround for the TP, due to an error caused by Windows.
** This code can be removed when the next TP is released.
*/
//#pragma comment( lib, "comctl32.lib" )
//#pragma comment( linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"" )


/*******************************************************************************
********************************************************************************
** COPYRIGHT NOTIFICATION (c) 2013 HMS Industrial Networks AB                 **
**                                                                            **
** This code is the property of HMS Industrial Networks AB.                   **
** The source code may not be reproduced, distributed, or used without        **
** permission. When used together with a product from HMS, permission is      **
** granted to modify, reproduce and distribute the code in binary form        **
** without any restrictions.                                                  **
**                                                                            **
** THE CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND. HMS DOES NOT    **
** WARRANT THAT THE FUNCTIONS OF THE CODE WILL MEET YOUR REQUIREMENTS, OR     **
** THAT THE OPERATION OF THE CODE WILL BE UNINTERRUPTED OR ERROR-FREE, OR     **
** THAT DEFECTS IN IT CAN BE CORRECTED.                                       **
********************************************************************************
********************************************************************************
** File Description:
** Source file containing the main loop, a basic user interface,
** as well as basic timer and serial port handling.
********************************************************************************
********************************************************************************
*/

#include <QObject>
#include <thread>
#include "windows.h"
#include "stdio.h"
#include "conio.h"

extern "C"
{
#include "abcc.h"
#include "ad_obj.h"
#include "abcc_td.h"
#include "appl_abcc_handler.h"

#include "appl_adi_config.h"
#include "appl_adimap.h"
};

#include "readdata.h"

#define MAX_HANDLED_CHAR 0x10

#define LOOP_RUN   0
#define LOOP_QUIT  1
#define LOOP_RESET 2

//解决中文显示乱码问题
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif


class PLC_Adi : public QObject
{
    Q_OBJECT

public:
    explicit PLC_Adi();
    ~PLC_Adi();
    int     initDevice();

signals:
    void    sendActionInfo( int action );               //发送动作指令
    void    sendDeviceInfoToUI( QString info );         //控制台信息显示到界面

public slots:
    void    rcv_startProcess( int flag );               //流程开始, 判断动作信息
    void    rcv_stopProcess();                          //流程结束, 停止接收动作信
    void    rcv_quitProcess();                          //流程结束, 退出监听线程

    void    rcvJudgeResult( bool result );              //判断当前位置是否有光
    void    rcvResultPosition( bool res, int x, int y );//接收粗扫或细扫最大光通量位置
    void    rcvReDetectResult( bool result );           //接收复检结果

private:
    void    judgeInPlaceType();             //判断到位类型是粗扫还是细扫
    void    judgeMoveOverState();           //判断运动完成状态
    void    enterFineDetect();              //进入细扫状态
    bool    compareRegisterValue( int reg_id, int reg_value );  //比较寄存器结果
    void    writeRegisterValue( int reg_id, int reg_value );    //写寄存器值
    void    refreshReadData( int action );  //更新ReadData结构体
    void    resetRegisters();               //寄存器清零

    void    thd_actionFunction();           //流程判断线程
    void    startThread();                  //回调函数线程
    UINT8   RunUi( void );                  //控制台界面循环

public:
    ReadData        data;

private:
    UINT8           bLoopState;
    APPL_AbccHandlerStatusType eAbccHandlerStatus;
    //    const UINT16    iSleepTimeMS;
    BOOL            fTest;
    BOOL            fShowDialog;
    char*           pszHardwareSerial;

    std::thread     thread_loop;    //控制台循环线程
    std::thread     thread_action;  //寄存器监听线程

    XLogPtr logger;             //日志

    bool    init_state;
    bool    is_process_quit;    //1则退出监听线程
    bool    is_process_start;   //0为暂停, 1为开始

    int     process_state;      //流程循环状态

    bool    is_pos_writen;      //false时写入坐标, 然后复位
    int     position_x;
    int     position_y;

    bool    is_result_writen;   //false时写入结果, 然后复位
    int     detectResult;       //true为OK

    int     process_times;     //检测轮次
};


#endif // PLC_ADI_H
