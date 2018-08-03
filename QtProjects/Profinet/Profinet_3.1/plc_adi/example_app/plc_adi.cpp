#include "plc_adi.h"
#include <QDebug>
/*------------------------------------------------------------------------------
** RunUi()
** This function handles the user interface.
** It is called every 5ms.
** Returns TRUE is the user have pressed the Q key.
**------------------------------------------------------------------------------
** Inputs:
**    -
**
** Outputs:
**    Returns:    - TRUE if user have pressed the "Q" key.
**
** Usage:
**    fQuit = RunUi();
**------------------------------------------------------------------------------
*/
const UINT16 iSleepTimeMS = 1;

static auto delay_plc = AccurateDelay_ms();

extern "C"
{
 extern AD_AdiEntryType Get_APPL_asAdiEntryList( int i );

 extern void Set_APPL_asAdiEntryList( int i, int plValue );
}

UINT8 PLC_Adi::RunUi( void )
{
    static char   cNewInput;
    static char   abUserInput[ MAX_HANDLED_CHAR + 1];
    static UINT8  bInputIndex = 0;

    BOOL8         fKbInput = FALSE;
    BOOL8         fRun     = TRUE;
    UINT8         bRet     = LOOP_RUN;

    if(_kbhit())
    {
        cNewInput = (char)_getch();
        fKbInput = TRUE;

        if( ( cNewInput == 'q' ) || ( cNewInput == 'Q' ) )
        {
            /*
            ** Q is for quit.
            */
            fRun = FALSE;
            bRet = LOOP_QUIT;
        }
        else if( ( cNewInput == 'r' ) || ( cNewInput == 'R' ) )
        {
            /*
            ** Q is for quit.
            */
            fRun = FALSE;
            bRet = LOOP_RESET;
        }
        else
        {
            _putch (cNewInput);

            if ( bInputIndex < MAX_HANDLED_CHAR )
            {
                abUserInput[bInputIndex++] = cNewInput;
                abUserInput[bInputIndex]   = 0;
            }
            else
            {
                bInputIndex = 0;
                abUserInput[bInputIndex]   = 0;
                fKbInput    = FALSE;
            }
        }
    }

    if( TRUE == fRun)
    {
        #if( APPL_ACTIVE_ADI_SETUP == APPL_ADI_SETUP_SPEED )

        if( fKbInput )
        {
            if( TRUE == APPL_HandleUserInput ( bInputIndex, abUserInput ))
            {
                bInputIndex = 0;
                abUserInput[bInputIndex] = 0;
            }
        }

        if( TRUE == APPL_UpdateADIs() )
        {
            APPL_ShowADIs ();
        }

        #endif
    }

    fKbInput = FALSE;

    return bRet;
} /* End of RunUi() */


///////////////////////////////////////////////////////////////////////////////
/**
  main function called by the Operating system on the application startup

  @param argc 
    number of commandline arguments
  @param argv[] 
    commandline arguments

  @return 
    1 on success
*/
PLC_Adi::PLC_Adi() /*main(int argc, char* argv[])*/
{
    bLoopState = LOOP_RUN;
    eAbccHandlerStatus = APPL_MODULE_NO_ERROR;
    fTest       = FALSE;
//    fShowDialog = TRUE;
    fShowDialog = FALSE;
    pszHardwareSerial = NULL;

    process_times = 0;
    position_x    = 0;
    position_y    = 0;

    init_state       = false;
    is_process_quit  = false;
    is_process_start = false;
    is_pos_writen    = true;
    is_result_writen = true;
    detectResult     = 0;

    process_state    = State_Null;

    auto initLog = InitClassLog() ;
    initLog( logger, LOG_NAME_ROOT, LOG_NAME_PLC );
/*
// check the command line
//
//    if( argc == 2 )
//    {
//        if( strstr(argv[1], "-t") )
//        {
//            if( strlen( argv[1] ) > 2 )
//            {
//                pszHardwareSerial = &argv[1][2];
//            }

//            fTest       = TRUE;
//            fShowDialog = FALSE;
//        }
//    }
*/


//    return ( 0 );
}

PLC_Adi::~PLC_Adi()
{

}

int PLC_Adi::initDevice()
{
    thread_loop = std::thread( &PLC_Adi::startThread, this );
    thread_loop.detach();

    int result = 0;
    TIME_TYPE start, end;
    TIME_NOW( start );
    for(;;)
    {
        if( init_state )
        {
            result = -1;
            Log_Debug( logger, "init success ... ");
            break;
        }
        else
        {
            Delay_ms(200);
        }

        TIME_NOW( end );
        if( DIFF_MS( start, end ) > PROFINET_TIMEOUT )
        {
            result = -2;
            Log_Error( logger, "init failed !!! ");
            break;
        }
    }

    if( result = -1 )
    {
        thread_action = std::thread( &PLC_Adi::thd_actionFunction, this );
        thread_action.detach();
    }

    return result;
}

void PLC_Adi::startThread()
{
    // initialize the hardware
    if( ABCC_HwInit( fShowDialog, pszHardwareSerial ) == ABCC_EC_NO_ERROR )
    {
        init_state = true;

//        long time_start, time_end, time_diff;
//        TIME_NOW(time_start);

//        bool result = false;    //寄存器初始化完成状态

        while( LOOP_QUIT != bLoopState )
        {
            bLoopState = LOOP_RUN;

            eAbccHandlerStatus = APPL_HandleAbcc();

            switch( eAbccHandlerStatus )
            {
            case APPL_MODULE_NO_ERROR:

                // if a test is actually running and the INpact configuration was
                // successful quit the application !
                if(( fTest ) && ( IsResetRequestAllowed (ABP_RESET_POWER_ON) ))
                {
                    printf( "LOOP_QUIT \n" );
                    emit sendDeviceInfoToUI( "LOOP_QUIT \n" );
                    bLoopState = LOOP_QUIT;
                }

                //确认寄存器初始化是否正确
//                if( !result /*&& GetAppState() == APPL_RUN*/ )
//                {
//                    printf( "init register completed ...\n" );
//                    emit sendDeviceInfoToUI( QString( "SET IA_READ_PD_SIZE: %1 \n"
//                                             "SET IA_WRITE_PD_SIZE: %2 \n")
//                                             .arg( QString::number( registerSize_Read ) )
//                                             .arg( QString::number( registerSize_Write ) ) );

//                    result = true;
//                }

                break;
            case APPL_MODULE_RESET:
                APPL_RestartAbcc();
                break;
            default:
                bLoopState = LOOP_QUIT;
                break;
            }

            if( bLoopState == LOOP_RUN )
            {
                bLoopState = RunUi();
            }

            switch( bLoopState )
            {
                case LOOP_RESET:
                    APPL_RestartAbcc();
                    bLoopState = LOOP_RUN;
                    break;
                case LOOP_RUN:
                case LOOP_QUIT:
                default:
                    break;
            }

            if( bLoopState == LOOP_RUN )
            {
//                TIME_NOW(time_end);
//                time_diff = time_end - time_start;
//                if( time_diff > 0 )
//                {
//                    if( time_diff > (UINT16)0xFFFF )
//                    {
//                        time_diff = (UINT16)0xFFFF;
//                    }
//                }

//                ABCC_RunTimerSystem( (UINT16)time_diff );
//                time_start = time_end;

                Sleep( iSleepTimeMS );
                ABCC_RunTimerSystem( iSleepTimeMS );
            }
        }

        printf( "break while loop \n" );
        emit sendDeviceInfoToUI( "break while loop \n" );
        ABCC_ShutdownDriver();
        ABCC_HwRelease();
    }
}

void PLC_Adi::thd_actionFunction()
{
    Log_Debug( logger, "enter thd_actionFunction ... ");
    emit sendDeviceInfoToUI( "enter thd_actionFunction ..." );

    for(;;)
    {
        //退出流程
        if( is_process_quit )
        {
            is_process_quit = false;
            Log_Debug( logger, "exit thd_actionFunction ... ");
            emit sendDeviceInfoToUI( "exit thd_actionFunction ..." );
            break;
        }
        //流程未开始
        if( !is_process_start )
        {
            Delay_ms(50);
            continue;
        }

        //切换状态--判断光通量
        if( process_state == State_Null )
        {
            if( !compareRegisterValue( REG_ID_Read_JudgeLight, 1 ) )
            {
                delay_plc(1);
                continue;
            }

            Log_Debug( logger, "enter JudgeLight ====>> %d", process_times++ );
            emit sendDeviceInfoToUI( "enter JudgeLight ..." );
            process_state = State_Light_Judge;
            emit sendActionInfo( Action_Light_Judge );
        }

        //切换状态--转台到位
        if( process_state == State_Light_Judge )
        {
            judgeInPlaceType();     //判断到位类型
        }

        //切换状态--运动完成
        if( process_state == State_Rough_Detect || process_state == State_Fine_Detect )
        {
            judgeMoveOverState();
        }

        //切换状态--转台就位(细扫)
        if(  process_state == State_Rough_MoveOver
          && compareRegisterValue( REG_ID_Read_InPlace_1, 1 ) )
        {
            enterFineDetect();
        }

        //切换状态--请求复检
        if(  process_state == State_Fine_MoveOver
          && compareRegisterValue( REG_ID_Read_ReDetect, 1 ) )
        {
            Log_Debug( logger, "enter ReDetect ====>> %d", process_times );
            emit sendDeviceInfoToUI( "enter ReDetect ..." );

            process_state = State_ReDetect;
            emit sendActionInfo( Action_ReDetect );
        }

        //切换状态--检测结束, 清零
        if( process_state == State_DetectOver)
        {
            delay_plc(5000);
            resetRegisters();
            //切换状态--等待检测
            process_state = State_Null;
        }

        delay_plc(1);
    }
}

void PLC_Adi::resetRegisters()
{
    for( int i = REG_ID_Write_JudgeLight; i <= REG_ID_Write_InPlace_2; i++ )
    {
        writeRegisterValue( i, 0 );
    }
}

bool PLC_Adi::compareRegisterValue( int reg_id, int reg_value )
{
    return ( *(APPL_asAdiEntryList[reg_id].uData.sUINT32.plValuePtr) == reg_value );
}

void PLC_Adi::writeRegisterValue( int reg_id, int reg_value )
{
    *(APPL_asAdiEntryList[reg_id].uData.sUINT32.plValuePtr) = reg_value;
}

void PLC_Adi::refreshReadData( int action )
{
    data.actionInfo = action;
    data.pos_x_1    = *( APPL_asAdiEntryList[REG_ID_Read_Pos_X].uData.sUINT32.plValuePtr );
    data.pos_y_1    = *( APPL_asAdiEntryList[REG_ID_Read_Pos_Y].uData.sUINT32.plValuePtr );
//    Log_Debug( logger, "PLC get position   : pos_x: %d ; pos_y: %d", data.pos_x_1, data.pos_y_1  );
}

void PLC_Adi::rcv_startProcess( int flag )
{
    if( flag == 0 )
    {
        is_process_start = true;
    }
    else
    {
        is_process_quit = false;
        thread_action = std::thread( &PLC_Adi::thd_actionFunction, this );
        thread_action.detach();
    }
}

void PLC_Adi::rcv_stopProcess()
{
    is_process_start = false;
}

void PLC_Adi::rcv_quitProcess()
{
    is_process_quit = true;
}

void PLC_Adi::rcvResultPosition( bool res, int x, int y )
{
    Log_Debug( logger, "rcvResultPosition: %d ==> (%d, %d)", (int)res, x, y );

    if( process_state == State_Rough_MoveOver )
    {
        if( res )
        {   //粗扫有光
            writeRegisterValue( REG_ID_Write_Pos_X, x );
            writeRegisterValue( REG_ID_Write_Pos_Y, y );

            writeRegisterValue( REG_ID_Write_Pos_OK_2, 1 );
        }
        else
        {   //粗扫没有光，返回NG，流程结束
            writeRegisterValue( REG_ID_Write_DetectNG, 1 );
            process_state = State_DetectOver;
        }

        return;
    }

    //细扫最大值
    writeRegisterValue( REG_ID_Write_Pos_X, x );
    writeRegisterValue( REG_ID_Write_Pos_Y, y );

    writeRegisterValue( REG_ID_Write_Pos_OK_1, 1 );
}

void PLC_Adi::rcvReDetectResult( bool result )
{
    Log_Debug( logger, "rcvReDetectResult: %d ", (int)result );

    detectResult = result ? 1: 2;   //1合格, 2不合格

    writeRegisterValue( REG_ID_Write_DetResult, detectResult );

    //切换状态--检测结束
    Log_Debug( logger, "enter DetectOver ====>> %d", process_times );
    process_state = State_DetectOver;
}

void PLC_Adi::rcvJudgeResult( bool result )
{
    Log_Debug( logger, "rcvJudgeResult: %d ", (int)result );

//    if( result )
//    {
//        //有光
//        writeRegisterValue( REG_ID_Write_JudgeLight, 1 );
//    }
//    else
//    {
        //无光
        writeRegisterValue( REG_ID_Write_JudgeLight, 2 );
//    }
}

//判断到位类型是粗扫还是细扫
void PLC_Adi::judgeInPlaceType()
{
    if(   compareRegisterValue( REG_ID_Read_InPlace_1, 1 )
      && !compareRegisterValue( REG_ID_Read_InPlace_2, 1 ) )
    {   //转台就位(细扫)
        enterFineDetect();
    }

    if(   compareRegisterValue( REG_ID_Read_InPlace_2, 1 )
      && !compareRegisterValue( REG_ID_Read_InPlace_1, 1 ) )
    {   //转台就位(粗扫)
        Log_Debug( logger, "enter Rough_Detect ====>> %d", process_times );
        emit sendDeviceInfoToUI( "enter Rough_Detect ..." );

        //粗扫就位反馈
        writeRegisterValue( REG_ID_Write_InPlace_2, 1);

        process_state = State_Rough_Detect;
        refreshReadData( Action_Rough_Detect );
        emit sendActionInfo( Action_Rough_Detect );
    }
}

//判断运动完成状态
void PLC_Adi::judgeMoveOverState()
{
    //切换状态--粗扫运动完成
    if( process_state == State_Rough_Detect )
    {
        if( compareRegisterValue( REG_ID_Read_MoveOver_2, 1 ) )
        {   //转台就位(粗扫)
            Log_Debug( logger, "enter MoveOver_2 ====>> %d", process_times );

            process_state = State_Rough_MoveOver;
            emit sendActionInfo( Action_Rough_MoveOver );
        }
        else    //刷新坐标
        {
            refreshReadData( Action_Rough_Detect );
        }
    }

    //切换状态--细扫运动完成
    if( process_state == State_Fine_Detect )
    {
        if( compareRegisterValue( REG_ID_Read_MoveOver_1, 1 ) )
        {   //转台就位(细扫)
            Log_Debug( logger, "enter MoveOver_1 ====>> %d", process_times );

            process_state = State_Fine_MoveOver;
            emit sendActionInfo( Action_Fine_MoveOver );
        }
        else                                                    //刷新坐标
        {
            refreshReadData( Action_Fine_Detect );
        }
    }
}

//进入细扫
void PLC_Adi::enterFineDetect()
{
    Log_Debug( logger, "enter Fine_Detect ====>> %d", process_times );
    emit sendDeviceInfoToUI( "enter Fine_Detect ..." );

    //细扫就位反馈
    writeRegisterValue( REG_ID_Write_InPlace_1, 1);

    process_state = State_Fine_Detect;
    refreshReadData( Action_Fine_Detect );
    emit sendActionInfo( Action_Fine_Detect );
}
