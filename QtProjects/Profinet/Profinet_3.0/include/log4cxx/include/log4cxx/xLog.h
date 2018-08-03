#ifndef XLOG_H
#define XLOG_H
/*********************************************************************************
  *Copyright(C):旭鑫智能南研所
  *Version:     1.0.2
  *Date:        2017.9.29
  *Description:  log4cxx日志宏封装，将log4cxx的主要配置功能及日志打印功能封装为宏，内部实现为c++匿名函数
  *Others:       需要c++11及以上，需要log4cxx0.10.0
  *Function List:
     1.stringTowstring()    窄字符转宽字符宏
     2.SetRootLogLevel()    设置根日志等级
     3.InitModuleLog()      初始化模块日志（配合ReleaseModuleLog，InitModuleLog和ReleaseModuleLog必须要配对使用）
     4.ReleaseModuleLog()   释放模块日志（配合InitModuleLog，InitModuleLog和ReleaseModuleLog必须要配对使用）
     5.InitClassLog()       初始化子类日志，继承模块日志中的配置
     6.Log_Trace(logger, message, ... ) 打印跟踪消息
     7.Log_Debug( logger, message, ...) 打印调试消息
     8.Log_Info( logger, message, ... ) 打印一般消息
     9.Log_Warn( logger, message, ... ) 打印警告消息
     10.Log_Error( logger, message, ...) 打印错误消息
     11.Log_Fatal( logger, message, ...)打印严重错误消息
  *History:
     1.Date:    2017.10.9
       Author:  cxw
       Modification: 修复了class日志继承混乱的bug，修复了由日志未释放引起的应用程序退出异常的bug，增加string转wstring宏
     2.Date:    2017.10.9
       Author:  Ma
       Modification: 在class日志继承时，父类日志不存在时将日志指针置空，并在打印时进行判断
     3.Date:    2017.10.10
       Author:  cxw
       Modification: 修复了模块日志在类库中多次new delete崩溃的bug
     4.Date:    2018.05.08
       Author:  shawn
       Modification: 修复了日志内容长度大于1024个字符时，会出现缓冲区溢出崩溃的BUG
**********************************************************************************/

#include <log4cxx/helpers/pool.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/dailyrollingfileappender.h>
#include <log4cxx/consoleappender.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/logmanager.h>
#include <cstdarg>
#include <cstdio>
#include <log4cxx/logger.h>
#include <log4cxx/logstring.h>
#include <log4cxx/log4cxx.h>
#include <stdio.h>
#include <io.h>
#include <direct.h>
#include <string>

using namespace log4cxx;

#define _MSGBUF_MAX     1024                //日志最大长度字符数
#define XLogPtr         log4cxx::LoggerPtr  //日志变量名宏

#if defined(__linux__) || defined(__linux)
#define Log_Layout  "%d{yyMMdd-HH:mm:ss,SSS}:%p-%c (Thread id: %t) [%M:%L] %m%n"
#define HourPattern "'.'yyyyMMdd-HH"		// logPattern = 1
#define AMPMPattern "'.'yyyyMMdd-a"		// logPattern = 2
#define DatePattern "'.'yyyyMMdd"		// logPattern = 3
#define Encoding    "UTF-8"
#define consoleAppenderName "consoleAppender"
#else

#if _MSC_VER
#define snprintf _snprintf
#define swprintf swprintf_s
#endif

#define Log_Layout  L"%d{yyMMdd-HH:mm:ss,SSS}:%5p-%c (Thread id: %t) [%M:%L] %m%n"
#define HourPattern L"'.'yyyyMMdd-HH"		// logPattern = 1 日志分卷
#define AMPMPattern L"'.'yyyyMMdd-a"		// logPattern = 2 日志分卷
#define DatePattern L"'.'yyyyMMdd"		// logPattern = 3 日志分卷
#define Encoding    L"UTF-8"
#define consoleAppenderName L"consoleAppender"
#endif


//日志输出文件分卷枚举
enum PatternType
{
    PatternHour = 1,            //每小时分一卷
    PatternAMPM = 2,            //上午下午分卷
    PatternDate = 3,            //每天分卷
    PatternNull = PatternAMPM   //默认上午下午分卷
};

//日志等级枚举
enum LogLevel
{
    Trace   = 0,    //过程追踪级
    Debug   = 1,    //调试级
    Info    = 2,    //正常消息级
    Warn    = 3,    //警告级
    Error   = 4,    //错误级
    Fatal   = 5     //致命错误级
};

#define stringTowstring() [](std::string &str) -> std::wstring                                  \
{                                                                                               \
    int datalen = str.size() + 1 ;                                                              \
                                                                                                \
    wchar_t *wdata = new wchar_t[ datalen ];                                                    \
                                                                                                \
    wmemset( wdata, 0, datalen );                                                               \
                                                                                                \
    swprintf( wdata, datalen, L"%hs", str.data() );                                             \
                                                                                                \
    std::wstring wstr = std::wstring( wdata );                                                  \
                                                                                                \
    delete [] wdata;                                                                            \
                                                                                                \
    return wstr;                                                                                \
}

//功能：设置Root日志等级, 一般只在主程序中设置, 只使用一次即可, 模块日志不要设置等级
//参数：level 日志等级
#define SetRootLogLevel() []( LogLevel level )                                                  \
{                                                                                               \
    log4cxx::LevelPtr ptr;                                                                      \
    switch( level )                                                                             \
    {                                                                                           \
    case Trace:                                                                                 \
        ptr = Level::getTrace();                                                                \
        break ;                                                                                 \
    case Debug:                                                                                 \
        ptr = Level::getDebug();                                                                \
        break ;                                                                                 \
    case Info:                                                                                  \
        ptr = Level::getInfo();                                                                 \
        break ;                                                                                 \
    case Warn:                                                                                  \
        ptr = Level::getWarn();                                                                 \
        break ;                                                                                 \
    case Error:                                                                                 \
        ptr = Level::getError();                                                                \
        break ;                                                                                 \
    case Fatal:                                                                                 \
        ptr = Level::getFatal();                                                                \
        break ;                                                                                 \
    default:                                                                                    \
        ptr = Level::getDebug();                                                                \
        break ;                                                                                 \
    }                                                                                           \
    log4cxx::LoggerPtr logger = Logger::getRootLogger();                                        \
    logger->setLevel( ptr );                                                                    \
};

//功能：初始化模块日志, 每个模块使用一次，生成一个单独的日志文件
//参数： logPtr    日志指针引用，由外部定义
//      logName     创建的日志名称
//      logPattern  日志分卷方式
//      logPath     日志输出路径 默认路径为当前的 ./debugMessage
#define InitModuleLog() []( XLogPtr     &logPtr,                                                \
                            std::string logName,                                                \
                            PatternType logPattern,                                             \
                            std::string logPath )                                               \
{                                                                                               \
    auto fun = stringTowstring();                                                               \
                                                                                                \
    if( std::string( "" ) == logPath  )                                                         \
    {                                                                                           \
        logPath = std::string( "./DebugMessage" );                                              \
    }                                                                                           \
    if( 0!= _access( logPath.data() , 0 ) )                                                     \
    {                                                                                           \
        _mkdir( logPath.data() );                                                               \
    }                                                                                           \
                                                                                                \
    std::wstring wlogName = fun(logName);                                                       \
    std::wstring wlogPath = fun(logPath);                                                       \
                                                                                                \
                                                                                                \
    logPtr = Logger::getLogger( wlogName );                                                     \
                                                                                                \
    LogString           logPathName         =  wlogPath + L"/" +  wlogName + L".log";           \
    LogString           rollAppendername    =  wlogName  + L"rollingfileAppender";              \
    PatternLayoutPtr    layout              = new PatternLayout();                              \
    LogString           conversionPattern   = LogString( Log_Layout );                          \
                                                                                                \
    layout->setConversionPattern( conversionPattern );                                          \
                                                                                                \
    helpers::Pool p;                                                                            \
    log4cxx::DailyRollingFileAppenderPtr  rollingfileAppender = new DailyRollingFileAppender(); \
                                                                                                \
    rollingfileAppender->setFile( logPathName );                                                \
    rollingfileAppender->setAppend(true);                                                       \
                                                                                                \
    switch( logPattern )                                                                        \
    {                                                                                           \
    case PatternHour:                                                                           \
        rollingfileAppender->setDatePattern( HourPattern );                                     \
        break ;                                                                                 \
    case PatternAMPM :                                                                          \
        rollingfileAppender->setDatePattern( AMPMPattern );                                     \
        break ;                                                                                 \
    case PatternDate :                                                                          \
        rollingfileAppender->setDatePattern( DatePattern );                                     \
        break ;                                                                                 \
    default:                                                                                    \
        rollingfileAppender->setDatePattern( AMPMPattern );                                     \
        break ;                                                                                 \
    }                                                                                           \
                                                                                                \
    rollingfileAppender->setLayout( LayoutPtr( layout ) );                                      \
    rollingfileAppender->activateOptions( p );                                                  \
    rollingfileAppender->setEncoding( Encoding );                                               \
    rollingfileAppender->setName( rollAppendername );                                           \
                                                                                                \
    logPtr->addAppender( rollingfileAppender  );                                                \
    logPtr->setAdditivity( false );                                                             \
                                                                                                \
    Log_Fatal(logPtr, "\n\n"                                                                    \
                      "                                 "                                       \
                      "**********  "                                                            \
                      "Start  %s  system!!   "                                                  \
                      "**********  "                                                            \
                      "\n\n"                                                                    \
                      "****************************************"                                \
                      " Building Time: %s-%s "                                                  \
                      "****************************************"                                \
                      "\n\n",                                                                   \
                      logName.data(), __DATE__, __TIME__ );                                     \
}

//功能： 释放模块日志
//参数： logPtr    日志指针引用，由外部定义
#define ReleaseModuleLog() []( XLogPtr  &logPtr)                                                \
{                                                                                               \
    if( nullptr != logPtr )                                                                     \
    {                                                                                           \
        logPtr->removeAllAppenders();                                                           \
    }                                                                                           \
}

//功能：初始化子类的日志 , 不生成日志文件 只更新日志名，输出位置为上个InitModuleLog中设置的位置
//参数： logPtr         日志指针引用，由外部定义
//      logParentName  父类（模块）日志名，如果日志名不存在会创建失败
//      logSonName     创建的日志名称
#define InitClassLog() []( XLogPtr     &logPtr,                                                 \
                           std::string logParentName,                                           \
                           std::string logSonName )                                             \
{                                                                                               \
    auto fun = stringTowstring();                                                               \
                                                                                                \
    std::wstring wlogParentName = fun( logParentName );                                         \
                                                                                                \
    std::wstring wlogSonName    = fun( logSonName );                                            \
                                                                                                \
    log4cxx::LoggerPtr  logParent = log4cxx::LogManager::exists( wlogParentName );              \
                                                                                                \
    if( logParent != nullptr )                                                                  \
    {                                                                                           \
        logPtr = logParent->getLogger( wlogParentName + std::wstring(L".") + wlogSonName );     \
        logPtr->setAdditivity( true );                                                          \
        return;                                                                                 \
    }                                                                                           \
                                                                                                \
    logPtr = nullptr;                                                                           \
}

//日志输出宏;
//踪迹日志，等级0 最低
//使用方式参照printf
#define Log_Trace(logger, message, ... )                                                        \
{                                                                                               \
    if( nullptr != logger )                                                                     \
    {                                                                                           \
        if (LOG4CXX_UNLIKELY( logger->isTraceEnabled() ) )                                      \
        {                                                                                       \
            char message_data[ _MSGBUF_MAX ] = {0};                                             \
                                                                                                \
            snprintf( message_data, sizeof( message_data ) - 1, message, ##__VA_ARGS__ );       \
                                                                                                \
            ::log4cxx::helpers::MessageBuffer oss_;                                             \
                                                                                                \
            logger->forcedLog(  ::log4cxx::Level::getTrace(),                                   \
                                oss_.str( oss_ << message_data ),                               \
                                LOG4CXX_LOCATION );                                             \
        }                                                                                       \
    }                                                                                           \
}

//调试日志，等级1  默认等级
#define Log_Debug( logger, message, ...)                                                        \
{                                                                                               \
    if( nullptr != logger )                                                                     \
    {                                                                                           \
        if( LOG4CXX_UNLIKELY( logger->isDebugEnabled() ) )                                      \
        {                                                                                       \
            char message_data[ _MSGBUF_MAX ] = {0};                                             \
                                                                                                \
            snprintf( message_data, sizeof( message_data ) - 1, message, ##__VA_ARGS__ );       \
                                                                                                \
            ::log4cxx::helpers::MessageBuffer oss_;                                             \
                                                                                                \
            logger->forcedLog(  ::log4cxx::Level::getDebug(),                                   \
                                oss_.str( oss_ << message_data ),                               \
                                LOG4CXX_LOCATION );                                             \
        }                                                                                       \
    }                                                                                           \
}

//消息日志，等级2
#define Log_Info( logger, message, ... )                                                        \
{                                                                                               \
    if( nullptr != logger )                                                                     \
    {                                                                                           \
        if ( logger->isInfoEnabled() )                                                          \
        {                                                                                       \
            char message_data[ _MSGBUF_MAX ] = {0};                                             \
                                                                                                \
            snprintf( message_data, sizeof( message_data ) - 1, message, ##__VA_ARGS__ );       \
                                                                                                \
            ::log4cxx::helpers::MessageBuffer oss_;                                             \
                                                                                                \
            logger->forcedLog(  ::log4cxx::Level::getInfo(),                                    \
                                oss_.str( oss_ << message_data ),                               \
                                LOG4CXX_LOCATION );                                             \
        }                                                                                       \
    }                                                                                           \
}

//警告日志， 等级3
#define Log_Warn( logger, message, ... )                                                        \
{                                                                                               \
    if( nullptr != logger )                                                                     \
    {                                                                                           \
        if ( logger->isWarnEnabled() )                                                          \
        {                                                                                       \
            char message_data[ _MSGBUF_MAX ] = {0};                                             \
                                                                                                \
            snprintf( message_data, sizeof( message_data ) - 1, message, ##__VA_ARGS__ );       \
                                                                                                \
            ::log4cxx::helpers::MessageBuffer oss_;                                             \
                                                                                                \
            logger->forcedLog(  ::log4cxx::Level::getWarn(),                                    \
                                oss_.str( oss_ << message_data ),                               \
                                LOG4CXX_LOCATION );                                             \
        }                                                                                       \
    }                                                                                           \
}

//错误日志， 等级4
#define Log_Error( logger, message, ...)                                                        \
{                                                                                               \
    if( nullptr != logger )                                                                     \
    {                                                                                           \
        if ( logger->isErrorEnabled() )                                                         \
        {                                                                                       \
            char message_data[ _MSGBUF_MAX ] = {0};                                             \
                                                                                                \
            snprintf( message_data, sizeof( message_data ) - 1, message, ##__VA_ARGS__ );       \
                                                                                                \
            ::log4cxx::helpers::MessageBuffer oss_;                                             \
                                                                                                \
            logger->forcedLog(  ::log4cxx::Level::getError(),                                   \
                                oss_.str( oss_ << message_data ),                               \
                                LOG4CXX_LOCATION);                                              \
        }                                                                                       \
    }                                                                                           \
}

//致命错误日志，等级5 最高等级，任何情况下都会打印
#define Log_Fatal( logger, message, ...)                                                        \
{                                                                                               \
    if( nullptr != logger )                                                                     \
    {                                                                                           \
        if ( logger->isFatalEnabled() )                                                         \
        {                                                                                       \
            char message_data[ _MSGBUF_MAX ] = {0};                                             \
                                                                                                \
            snprintf( message_data, sizeof( message_data ) - 1, message, ##__VA_ARGS__ );       \
                                                                                                \
            ::log4cxx::helpers::MessageBuffer oss_;                                             \
                                                                                                \
            logger->forcedLog(  ::log4cxx::Level::getFatal(),                                   \
                                oss_.str( oss_ << message_data ),                               \
                                LOG4CXX_LOCATION );                                             \
        }                                                                                       \
    }                                                                                           \
}

#endif // XLOG_H
