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

PLC_Adi::PLC_Adi() /*main(int argc, char* argv[])*/
{
    bLoopState = LOOP_RUN;
    eAbccHandlerStatus = APPL_MODULE_NO_ERROR;
    fTest       = FALSE;
//    fShowDialog = TRUE;
    fShowDialog = FALSE;    //只加载一个设备，不显示窗口
    pszHardwareSerial = NULL;

    process_times = 0;
    position_x    = 0;
    position_y    = 0;

    is_detect_over_reset_rcved = false;
    init_state       = false;
    is_process_quit  = false;
    is_process_start = false;
    is_pos_writen    = true;
    is_result_writen = true;
    detectResult     = 0;

    process_state    = State_Null;

    auto initLog = InitClassLog() ;
    initLog( logger, LOG_NAME_ROOT, LOG_NAME_PLC );
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
                    bLoopState = LOOP_QUIT;
                }
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
        ABCC_ShutdownDriver();
        ABCC_HwRelease();
    }
}

void PLC_Adi::thd_actionFunction()
{
    Log_Debug( logger, "enter thd_actionFunction ... ");

    for(;;)
    {
        //退出流程
        if( is_process_quit )
        {
            is_process_quit = false;
            Log_Debug( logger, "exit thd_actionFunction ... ");
            break;
        }
        //流程未开始
        if( !is_process_start )
        {
            Delay_ms(50);
            continue;
        }

        //切换状态--到位
        if( process_state == State_Null )
        {
            if( !compareRegisterValue( REG_ID_SUB_ACTION, 1 ) )
            {
                delay_plc(1);
                continue;
            }

            //接收就位
            writeRegisterValue( REG_ID_SUB_IN_PLACE, 1 );

            //切换状态--InPlace
            Log_Debug( logger, "enter Inplace ====>> %d", process_times++ );
            process_state = State_InPlace;
            refreshReadData( Action_InPlace );
            emit sendActionInfo( Action_InPlace );
        }
        else if( process_state == State_InPlace )   //当前状态--InPlace
        {
            if( compareRegisterValue( REG_ID_SUB_MOVE_OVER, 1 ) )   //切换状态--获取粗检坐标
            {
                Log_Debug( logger, "enter MoveOver ====>> %d", process_times );
                process_state = State_MoveOver;
                emit sendActionInfo( Action_MoveOver );
            }
            else                                                    //刷新坐标
            {
                refreshReadData( Action_InPlace );
                emit sendActionInfo( Action_InPlace );
            }
        }
        else if( process_state == State_MoveOver )  //当前状态--GetPosition
        {
            //收到UI传来的坐标
            if( !is_pos_writen )
            {
                Log_Debug( logger, "write ResultPosition " );
                is_pos_writen = true;
                writeRegisterValue( REG_ID_SUB_POS_W_X, position_x );
                writeRegisterValue( REG_ID_SUB_POS_W_Y, position_y );
                writeRegisterValue( REG_ID_SUB_DET_RESULT, 1 );
            }

            if( compareRegisterValue( REG_ID_SUB_ACTION, 2 ) )
            {
                //切换状态--请求复检
                Log_Debug( logger, "enter ReDetect ====>> %d", process_times );
                process_state = State_ReDetect;
                emit sendActionInfo( Action_ReDetect );
            }
        }
        else if( process_state == State_ReDetect )  //当前状态--ReDetect
        {
            //收到UI传来的复检结果
            if( !is_result_writen )
            {
                is_result_writen = true;
                writeRegisterValue( REG_ID_SUB_REDET_RESULT, detectResult );

                //切换状态--检测结束
                Log_Debug( logger, "enter DetectOver ====>> %d", process_times );
                process_state = State_DetectOver;
            }
        }
        else                                        //当前状态--DetectOver
        {
            if( compareRegisterValue( REG_ID_SUB_CLEAR_DATA, 1 ) )
            {
                resetRegisters();
                //切换状态--等待检测
                process_state = State_Null;
            }
        }

        delay_plc(1);
    }
}

void PLC_Adi::resetRegisters()
{
    for( int i = REG_ID_SUB_IN_PLACE; i <= REG_ID_SUB_HEART_BEAT_W; i++ )
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
    data.pos_x_1    = *( APPL_asAdiEntryList[REG_ID_SUB_POS_R_X].uData.sUINT32.plValuePtr );
    data.pos_y_1    = *( APPL_asAdiEntryList[REG_ID_SUB_POS_R_Y].uData.sUINT32.plValuePtr );
    Log_Debug( logger, "PLC get position   : pos_x: %d ; pos_y: %d", data.pos_x_1, data.pos_y_1  );
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

void PLC_Adi::rcvResultPosition( int x, int y )
{
    Log_Debug( logger, "rcvResultPosition: (%d, %d)", x, y );
    position_x = x;
    position_y = y;

    is_pos_writen = false;      //false时写入坐标, 然后复位
}

void PLC_Adi::rcvReDetectResult( bool result )
{
    Log_Debug( logger, "rcvReDetectResult: %d ", (int)result );
    if( result )
    {
        detectResult = 1;   //合格
    }
    else
    {
        detectResult = 2;
    }


    is_result_writen = false;   //false时写入结果, 然后复位
}
