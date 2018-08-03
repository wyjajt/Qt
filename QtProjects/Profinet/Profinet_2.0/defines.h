#ifndef DEFINES_H
#define DEFINES_H

#include <Windows.h>
#include <time.h>
#include <QDateTime>

#include <QMessageBox>
#include "xLog.h"

//日志
#define LOG_NAME_ROOT           "UI"
#define LOG_NAME_CAMERA         "Camera"
#define LOG_NAME_PLC            "PLC"

#define TABLE_ROW_COUNT         0
#define TABLE_COL_COUNT         7

#define LOG_PRINT_INTERVAL      1000    //日志打印图片序号的间隔
#define PROFINET_TIMEOUT        6000    //板卡初始化超时时间
#define CAMERA_NO_CONNECT       0

//寄存器ID在数组中的下标
#define REG_ID_SUB_ACTION       0   //动作寄存器(Read)
#define REG_ID_SUB_POS_R_X      1   //x坐标寄存器(Read)
#define REG_ID_SUB_POS_R_Y      2   //y坐标寄存器(Read)
#define REG_ID_SUB_HEART_BEAT_R 3   //读心跳寄存器(Read)
#define REG_ID_SUB_MOVE_OVER    4   //运动结束寄存器(Read)
//#define REG_ID_SUB_JUDGE_LIGHT  5   //判断是否有光寄存器(Read)
#define REG_ID_SUB_CLEAR_DATA   5   //清零寄存器(Read)

#define REG_ID_SUB_IN_PLACE     6   //就位寄存器(Write)
#define REG_ID_SUB_DET_RESULT   7   //检测结果寄存器(Write)
#define REG_ID_SUB_POS_W_X      8   //结果x坐标寄存器(Write)
#define REG_ID_SUB_POS_W_Y      9   //结果y坐标寄存器(Write)
#define REG_ID_SUB_REDET_RESULT 10  //复检结果寄存器(Write)
#define REG_ID_SUB_HEART_BEAT_W 11  //写心跳寄存器(Write)

//延时
#define Delay_ms(x)             Sleep(x);
#define TIME_NOW(t)             {t = clock();}
#define DIFF_MS(start, end)     ((end) - (start))
#define TIME_TYPE               clock_t

//精准延时
#define AccurateDelay_ms()  [](int time)                                   \
{                                                                          \
    if( time > 20 )                                                        \
    {                                                                      \
        Sleep(time);                                                       \
        return;                                                            \
    }                                                                      \
    QDateTime _timer = QDateTime::currentDateTime();                       \
    QDateTime _now_timer;                                                  \
    do                                                                     \
    {                                                                      \
        _now_timer = QDateTime::currentDateTime();                         \
    }                                                                      \
    while ( _timer.msecsTo( _now_timer ) < time );                         \
}

enum Process_State
{
    State_Null = 0,     //等待检测
    State_InPlace,      //到位
    State_MoveOver,     //运动完成
    State_ReDetect,     //请求复检
    State_DetectOver    //检测结束
};

enum ActionInfo
{
    Action_InPlace = 1, //到位
    Action_MoveOver,    //运动完成
    Action_ReDetect,    //请求复检
    Action_DetectOver   //检测结束
};

enum TakePhoto_Type
{
    Type_Start = 0,     //到位后循环拍照
    Type_Stop,          //运动完成则相机不再拍照, 等待之前的线程退出
    Type_OnlyOnce,      //复检时只拍一次
};

#endif // DEFINES_H
