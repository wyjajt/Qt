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
#define REG_ID_Read_JudgeLight   0  //D1 , 判断是否有光, 1有效
#define REG_ID_Read_InPlace_1    1  //D2 , 转台就位(细扫), 1有效
#define REG_ID_Read_Pos_X        2  //D3 , int
#define REG_ID_Read_Pos_Y        3  //D4 , int
#define REG_ID_Read_MoveOver_1   4  //D5 , 细扫运动完成, 1有效
#define REG_ID_Read_ReDetect     5  //D6 , 请求复检
#define REG_ID_Read_InPlace_2    6  //D7 , 转台就位(粗扫), 1有效
#define REG_ID_Read_HeartBeat    7  //D8 , 读心跳
#define REG_ID_Read_MoveOver_2   8  //D25, 粗扫运动完成, 1有效

#define REG_ID_Write_JudgeLight  9  //D10, 光通量判断结果, 1有光, 2无光
#define REG_ID_Write_InPlace_1  10  //D11, 细扫就位, 1有效
#define REG_ID_Write_Pos_X      11  //D12, int
#define REG_ID_Write_Pos_Y      12  //D13, int
#define REG_ID_Write_Pos_OK_1   13  //D14, 细扫最大值, 可以读取位置, 1有效
#define REG_ID_Write_DetResult  14  //D15, 复检结果, 1 OK, 2 NG
#define REG_ID_Write_Pos_OK_2   15  //D16, 粗扫最大值, 可以读取位置, 1有效
#define REG_ID_Write_DetectNG   16  //D18, 粗扫结束, 没扫到光, 1有效
#define REG_ID_Write_HeartBeat  17  //D19, 写心跳
#define REG_ID_Write_InPlace_2  18  //D21, 粗扫就位, 1有效


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
    State_Null = 0,         //等待检测
    State_Light_Judge,      //判断是否有光
    State_Rough_Detect,     //粗扫到位
    State_Rough_MoveOver,   //粗扫运动完成
    State_Fine_Detect,      //细扫到位
    State_Fine_MoveOver,    //细扫运动完成
    State_ReDetect,     	//请求复检
    State_DetectOver    	//检测结束
};

enum ActionInfo
{
    Action_Null = 0,        //等待检测
    Action_Light_Judge,     //判断是否有光
    Action_Rough_Detect,    //粗扫到位
    Action_Rough_MoveOver,  //粗扫运动完成
    Action_Fine_Detect,     //细扫到位
    Action_Fine_MoveOver,   //细扫运动完成
    Action_ReDetect,        //请求复检
    Action_DetectOver       //检测结束
};

enum TakePhoto_Type
{
    Type_Start = 0,         //到位后开始拍照
    Type_Continue,          //循环拍照
    Type_Stop,              //运动完成则相机不再拍照, 等待之前的线程退出
    Type_OnlyOnce,          //复检时只拍一次
    Type_ReDtect,          //复检时只拍一次

};

#endif // DEFINES_H
