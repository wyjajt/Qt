//////////////////////////////////////////////////////////////////////////
// HMS Technology Center Ravensburg GmbH
//////////////////////////////////////////////////////////////////////////
/**
  implementaton of the ABCC handler main function to be called from the 
  main loop, including a main state machine for handling startup and restart 
  of the ABCC.

  based on the Startkit Version 2 .01.01 (2015-12-14)
  Delivered with:
     ABCC Driver 4.01.01 (2015-12-14)
     ABP         7.16.01 (2015-10-14)

  @file appl_abcc_handler.c

*/
//////////////////////////////////////////////////////////////////////////
// (C) 2002-2016  HMS, all rights reserved
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// compiler directives
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// include files
//////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include "abcc_td.h"
#include "appl_state.h"

#include "appl_userinit.h"
#include "abcc_setup.h"
#include "appl_exception.h"

#include "abp.h"
#include "abcc.h"
#include "ad_obj.h"
#include "abcc_port.h"

#include "ad_obj.h"         /* Application data object:   254                 */
#include "app_obj.h"        /* Application object:        255                 */
#include "appl_abcc_handler.h"
#include "abcc_port.h"
#include "etn_obj.h"
#include "sync_obj.h"

#include "eip.h"
#include "prt.h"
#include "epl.h"
#include "dpv1.h"
#include "ect.h"
#include "dev.h"
#include "mod.h"
#include "cop.h"

#include "abcc_obj_cfg.h"
#include "appl_adi_config.h"

#include "ad_map.h"
#include "appl_comm.h"

//////////////////////////////////////////////////////////////////////////
// static constants, types, macros, variables
//////////////////////////////////////////////////////////////////////////

//#define RESTART_AUTOMATIC 1
//#define STOP_ON_FATAL     1


/*------------------------------------------------------------------------------
** Maximum start up time when the module is upgrading its firmware
**------------------------------------------------------------------------------
*/
#define APPL_FW_UPGRADE_STARTUP_TIME_MS     ( 2 * 60 * (UINT32)1000 )


//////////////////////////////////////////////////////////////////////////
// global variables
//////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------------------
** Set to TRUE when an unexpected error occur. The main state machine will
** return APPL_MODULE_UNEXPECTED_ERROR when this flag is set.
**------------------------------------------------------------------------------
*/
static BOOL appl_fUnexpectedError = FALSE;

/*------------------------------------------------------------------------------
** Event flags used by application to invoke the corresponding
** ABCC_Trigger<event_action> function from the desired context. The flag will
** be set to TRUE in ABCC_CbfEvent().
**------------------------------------------------------------------------------
*/
static volatile BOOL appl_fMsgReceivedEvent  = FALSE;
static volatile BOOL appl_fRdPdReceivedEvent = FALSE;
static volatile BOOL appl_fTransmitMsgEvent  = FALSE;
static volatile BOOL appl_fAbccStatusEvent   = FALSE;
static volatile BOOL APPL_DEV_RESET          = FALSE;

#define GTIMEOUT 5000 // 5 seconds timeout

HANDLE g_hEvent = INVALID_HANDLE_VALUE;

//////////////////////////////////////////////////////////////////////////
// static function prototypes
//////////////////////////////////////////////////////////////////////////
void ABCC_UserInitReq( void );

//////////////////////////////////////////////////////////////////////////
// global functions
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static functions
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
/**
  Handle events indicated in ABCC_CbfEvent()callback function. 

  @return 
    If the function succeeds it returns VCI_OK, otherwise a value other 
    than VCI_OK.

  @note 
    Note that these events could be handled from any chosen context but in
    in this application it is done from main loop context.
*/
//////////////////////////////////////////////////////////////////////////
static void HandleEvents ()
{
  if( appl_fRdPdReceivedEvent )
  {
      appl_fRdPdReceivedEvent = FALSE;
      ABCC_TriggerRdPdUpdate();
  }

  if( appl_fMsgReceivedEvent )
  {
      appl_fMsgReceivedEvent = FALSE;
      ABCC_TriggerReceiveMessage();
  }

  if( appl_fTransmitMsgEvent )
  {
      appl_fTransmitMsgEvent = FALSE;
      ABCC_TriggerTransmitMessage();
  }

  if( appl_fAbccStatusEvent )
  {
      appl_fAbccStatusEvent = FALSE;
      ABCC_TriggerAnbStatusUpdate();
  }
  /*
  ** End event handling.
  */

#if SYNC_OBJ_ENABLE
  if( SYNC_GetMode() == SYNC_MODE_NONSYNCHRONOUS )
  {
      ABCC_TriggerWrPdUpdate();
  }
#else
  /*
  ** Always update write process data
  */
  ABCC_TriggerWrPdUpdate();
#endif
}

//////////////////////////////////////////////////////////////////////////
/**
  Runs the state machine controlling the ABCC module 

  @return 
    APPL_AbccHandlerStatusType
*/
//驱动执行过程
APPL_AbccHandlerStatusType APPL_HandleAbcc( void )
{
    static APPL_AbccHandlerStatusType eModuleStatus = APPL_MODULE_NO_ERROR;
    UINT32 lStartupTimeMs;
    ABCC_CommunicationStateType eAbccComState;

    UINT32 appl_eAbccHandlerState = GetAppState( );
    UINT32 appl_eAnbState;
    BOOL   fFatal = FALSE;

    if( INVALID_HANDLE_VALUE == g_hEvent )
    {
        g_hEvent = CreateEvent ( NULL, FALSE, FALSE, NULL );
    }

    switch( appl_eAbccHandlerState )
    {
    case APPL_INIT:
    {
        printf ( "APPL_INIT ... \n" );
        eModuleStatus = APPL_MODULE_NO_ERROR;

        appl_fMsgReceivedEvent  = FALSE;
        appl_fRdPdReceivedEvent = FALSE;
        appl_fTransmitMsgEvent  = FALSE;
        appl_fAbccStatusEvent   = FALSE;

        if( !ABCC_ModuleDetect() )
        {
            eModuleStatus = APPL_MODULE_NOT_DETECTED;
        }

        if( eModuleStatus == APPL_MODULE_NO_ERROR )
        {
            /*
            ** Init application data object
            */
            if( AD_Init( APPL_asAdiEntryList,
                         APPL_GetNumAdi(),
                         APPL_asAdObjDefaultMap ) == AD_NO_ERROR )
            {
                CmdInitArray ();
            }
            else
            {
                eModuleStatus = APPL_MODULE_UNEXPECTED_ERROR ;
            }

        #if APP_OBJ_ENABLE
            if( APP_GetCandidateFwAvailable() == TRUE )
            {
                lStartupTimeMs = APPL_FW_UPGRADE_STARTUP_TIME_MS;
            }
            else
        #endif
            {
                /*
                ** Default time will be used
                */
                lStartupTimeMs = 0;
            }

            if( ABCC_StartDriver( lStartupTimeMs ) == ABCC_EC_NO_ERROR )
            {
                ABCC_HWReleaseReset();
                SetAppState(APPL_WAITCOM );
            }
            else
            {
                eModuleStatus = APPL_MODULE_NOT_ANSWERING;
            }
        }

        if( eModuleStatus != APPL_MODULE_NO_ERROR )
        {
            SetAppState( APPL_HALT );
        }

        break;
    }

    //设置通信协议和寄存器
    case APPL_WAITCOM:
    {
        //
        // With this function the setup communication is executed !!
        // This function returns TRUE if the ANB Ready Interrupt was received
        eAbccComState = ABCC_isReadyForCommunication();

        if( eAbccComState == ABCC_READY_FOR_COMMUNICATION )
        {
            printf( "Start Setup communication\n" );

            // design guide page 60
            if( FatalEventCheck( g_hEvent, &fFatal, GTIMEOUT))
            {
                if( fFatal )
                {
                    SetAppState( APPL_FATAL );
                }
                else
                {
                    // 1. Gather information about the Anybus Module (Optional)
                    ABCC_SetupInit();
                    if( FALSE == ABCC_GatherInfo ( g_hEvent, GTIMEOUT ))
                    {
                        printf( "Restart Application ( the module doesn't react !) \n");
                        APPL_RestartAbcc ();
                    }
                    else
                    {
                         // 2. Network Configuration (Optional)
                         ABCC_UserInitReq();
                         UpdateSwitchValues( g_hEvent, GTIMEOUT);

                         // 3. Process Data Mapping (Optional)
                         ABCC_MapPD( g_hEvent, GTIMEOUT );

                         // 4. Finalize the Setup
                         ABCC_FinalizeSetup( g_hEvent, GTIMEOUT );

                         SetAppState(APPL_RUN);
                    }
                }
            }
            else
            {
                 // timeout !!
                 APPL_RestartAbcc();
            }
        }
        else if( eAbccComState == ABCC_COMMUNICATION_ERROR )
        {
            SetAppState( APPL_HALT );
            eModuleStatus = APPL_MODULE_NOT_ANSWERING;
        }
        break;
    }

    case APPL_FATAL:
    {
        ShowInfos( g_hEvent, GTIMEOUT );
        FatalEventReset( g_hEvent, GTIMEOUT );
        ShowInfos( g_hEvent, GTIMEOUT );

    #ifdef STOP_ON_FATAL
        SetAppState( APPL_HALT );
    #else
        SetAppState( APPL_WAITCOM );
    #endif
        break;
    }

    case APPL_RUN:
    {
        appl_eAnbState = GetAnbState();

        if( appl_eAnbState == ABP_ANB_STATE_EXCEPTION )
        {
            ExceptionShow     ( g_hEvent, GTIMEOUT );
            ExceptionInfoShow ( g_hEvent, GTIMEOUT );
        }
        else
        {
            //
            // At the start the anybus module is now in the Anybus module NW_INIT
            // The callback function 'ABCC_CbfReceiveMsg' is called
            // After this the anybus module is in the WAIT_PROCESS state.
            HandleEvents();   // which are not handled automatically
            ABCC_RunDriver(); // polling

            // test to restart automatically !!
        #ifdef RESTART_AUTOMATIC
            static BOOL fFirst = TRUE;
            static UINT32 dwTimerTick = 0;
            static UINT32 dwLoop = 0;

            if(fFirst)
            {
                dwTimerTick = GetTickCount();
                fFirst = FALSE;
            }
            else
            {
                if( GetTickCount()- dwTimerTick > 1000)
                {
                    fFirst = TRUE;
                    printf ( "------- %8i ------- \n", ++dwLoop);

                    SetAppState( APPL_ABCCRESET );
                }
            }
        #endif
        }

        break;
    }

    case APPL_SHUTDOWN:
        ABCC_HWReset();
        SetAnbState(ABP_ANB_STATE_SETUP);

        SetAppState(APPL_HALT );
        eModuleStatus = APPL_MODULE_SHUTDOWN;
        break;

    case APPL_ABCCRESET:
        ABCC_HWReset();
        SetAnbState(ABP_ANB_STATE_SETUP);

        SetAppState( APPL_INIT  );
        eModuleStatus = APPL_MODULE_NO_ERROR;
        break;

    case APPL_DEVRESET:
        ABCC_HWReset();
        SetAnbState(ABP_ANB_STATE_SETUP);

        SetAppState(APPL_HALT );
        eModuleStatus = APPL_MODULE_RESET;
        break;

    case APPL_HALT:
        break;

    default:
        break;
    }

    if( appl_fUnexpectedError )
    {
        return( APPL_MODULE_UNEXPECTED_ERROR );
    }

    return( eModuleStatus );
}


//////////////////////////////////////////////////////////////////////////
/**
  Application has encountered an unexpected error.
  The abcc main state machine will halt and indicate error.
*/
//////////////////////////////////////////////////////////////////////////
void APPL_UnexpectedError( void )
{
  appl_fUnexpectedError = TRUE;
}

//////////////////////////////////////////////////////////////////////////
/**
  This function will force the ABCC handler to restart the ABCC module
*/
//////////////////////////////////////////////////////////////////////////
void APPL_RestartAbcc( void )
{
  SetAppState( APPL_ABCCRESET );
}

//////////////////////////////////////////////////////////////////////////
/**
  This function will force the ABCC handler to shutdown the ABCC module
*/
//////////////////////////////////////////////////////////////////////////
void APPL_Shutdown( void )
{
  SetAppState( APPL_SHUTDOWN );
}
//////////////////////////////////////////////////////////////////////////
/**
  This function will force the ABCC handler to reset the ABCC
  */
//////////////////////////////////////////////////////////////////////////
void APPL_Reset( void )
{
  UINT32 dwExpState = 0;

  if ( ABP_ANB_STATE_EXCEPTION == GetAnbState())
  {
    APPL_DEV_RESET = TRUE;
    //
    // ShowInfos !!
    // 
    ExceptionShow ( g_hEvent, GTIMEOUT);
    ExceptionInfoShow ( g_hEvent, GTIMEOUT);
  }

  SetAppState( APPL_DEVRESET );
}

//////////////////////////////////////////////////////////////////////////
/**
  Set HW switch1 value to the application. The value is used to generate the
  the node address or the IP address (192.168.0.X) depending on network.

  @param bSwitchValue
    Switch 1 value

  @note 
    NOTE: For networks that uses an IP address and if this function is called
    with address 0 no address will be set to the ABCC.
*/
//////////////////////////////////////////////////////////////////////////
void APPL_SetAddress( UINT8 bSwitchValue )
{
  setAddress ( bSwitchValue );

  /*
  ** Indicate to application object that the address is set by HW switches
  */
  #if APP_OBJ_ENABLE
    APP_HwConfAddress( TRUE );
  #endif
}

//////////////////////////////////////////////////////////////////////////
/**
  Set HW switch2 value to the application. The value is used to generate the
  baud rate for networks that implements network configuration object instance 2.

  @param bSwitchValue
    Switch 2 value
*/
//////////////////////////////////////////////////////////////////////////
void APPL_SetBaudrate( UINT8 bSwitchValue )
{
  setBaudrate(bSwitchValue);
}

//////////////////////////////////////////////////////////////////////////
/**
  This function need to be implemented by the application. The function is
  called when the driver is about to start the automatic process data mapping.
  If no automatic configuration is desired then the pointers are set to NULL.
  Otherwise the pointers are set to point at the structures containing mapping
  information. The mapping structures are defined in abcc_ad_if.h.
  This function will be invoked in same context as the read message handling.
  (See comment for callback section above)

  @param ppsAdiEntry
    Pointer to the requested configuration structure pointer.
  @param ppsDefaultMap
    Pointer to default mapping table.

  @return 
    If the function succeeds it returns VCI_OK, otherwise a value other 
    than VCI_OK.

  @note 
    Access:    public 
*/
//////////////////////////////////////////////////////////////////////////
UINT16  ABCC_CbfAdiMappingReq( const AD_AdiEntryType**  const ppsAdiEntry,
                               const AD_DefaultMapType** const ppsDefaultMap )
{
  return AD_AdiMappingReq( ppsAdiEntry, ppsDefaultMap );
}

//////////////////////////////////////////////////////////////////////////
/**
  This function needs to be implemented by the application. The function
  updates the current write process data. The data must be copied into the
  buffer before returning from the function.
  The data will only be sent to the ABCC if the return value is TRUE.
  Regarding callback context, see comment for callback section above.

  @param pxWritePd
    Pointer to the process data to be sent.

  @return 
  TRUE  - If the process data has been changed since last call.
  FALSE - Process data not changed.
*/
//////////////////////////////////////////////////////////////////////////
BOOL ABCC_CbfUpdateWriteProcessData( void* pxWritePd )
{
    /*
    ** AD_UpdatePdWriteData is a general function that updates all ADI:s according
    ** to current map.
    ** If the ADI mapping is fixed there is potential for doing that in a more
    ** optimized way, for example by using memcpy.
    */

//    printf( "\n WriteValue: 0x0C:%d\n", *(APPL_asAdiEntryList[6].uData.sUINT32.plValuePtr));
//    printf( "\n WriteValue: 0x0D:%d\n", *(APPL_asAdiEntryList[7].uData.sUINT32.plValuePtr));

    return( AD_UpdatePdWriteData( pxWritePd ) );
}

#if( ABCC_CFG_REMAP_SUPPORT_ENABLED )
  //////////////////////////////////////////////////////////////////////////
  /**
    This callback is invoked when REMAP response is successfully sent to the
    ABCC.
  */
  //////////////////////////////////////////////////////////////////////////
  void ABCC_CbfRemapDone( void )
  {
    AD_RemapDone();
  }
#endif

//////////////////////////////////////////////////////////////////////////
/**
  This function needs to be implemented by the application. The function is
  called when new process data has been received. The process data needs to
  be copied to the application ADI:s before returning from the function. N

  @param pxReadPd
    Pointer to the received process data.

  @note
    Note that the data may not be changed since last time.
    Regarding callback context, see comment for callback section above.
*/
//////////////////////////////////////////////////////////////////////////
void ABCC_CbfNewReadPd( void* pxReadPd )
{
    /*
    ** AD_UpdatePdReadData is a general function that updates all ADI:s according
    ** to current map.
    ** If the ADI mapping is fixed there is potential for doing that in a more
    ** optimized way, for example by using memcpy.
    */
    AD_UpdatePdReadData( pxReadPd );

    //处理软件需要读的数据
//    printf( "\n ReadValue: 0x01:%d\n", *(APPL_asAdiEntryList[0].uData.sUINT32.plValuePtr));
//    printf( "\n ReadValue: 0x02:%d\n", *(APPL_asAdiEntryList[1].uData.sUINT32.plValuePtr));
//    printf( "\n ReadValue: 0x03:%d\n", *(APPL_asAdiEntryList[2].uData.sUINT32.plValuePtr));
//    printf( "\n ReadValue: 0x05:%d\n", *(APPL_asAdiEntryList[3].uData.sUINT32.plValuePtr));
//    printf( "\n ReadValue: 0x06:%d\n", *(APPL_asAdiEntryList[4].uData.sUINT32.plValuePtr));
//    printf( "\n ReadValue: 0x08:%d\n", *(APPL_asAdiEntryList[5].uData.sUINT32.plValuePtr));

//    printf( "\n ReadValue: 0x0B:%d\n", *(APPL_asAdiEntryList[6].uData.sUINT32.plValuePtr));
//    printf( "\n ReadValue: 0x0c:%d\n", *(APPL_asAdiEntryList[7].uData.sUINT32.plValuePtr));
//    printf( "\n ReadValue: 0x0e:%d\n", *(APPL_asAdiEntryList[8].uData.sUINT32.plValuePtr));
//    printf( "\n ReadValue: 0x10:%d\n", *(APPL_asAdiEntryList[9].uData.sUINT32.plValuePtr));
//    printf( "\n ReadValue: 0x11:%d\n", *(APPL_asAdiEntryList[10].uData.sUINT32.plValuePtr));
//    printf( "\n ReadValue: 0x12:%d\n", *(APPL_asAdiEntryList[11].uData.sUINT32.plValuePtr));
//    printf( "\n ReadValue: 0x14:%d\n", *(APPL_asAdiEntryList[12].uData.sUINT32.plValuePtr));

}

//////////////////////////////////////////////////////////////////////////
/**
  This function needs to be implemented by the application.
  The context of the call is depending on where the error has occured.

  @param eSeverity
    Severity of the event (see ABCC_SeverityType).
  @param iErrorCode
    Error code.
  @param lAddInfo
    Depending on error different additional information can be added.
*/
//////////////////////////////////////////////////////////////////////////
void ABCC_CbfDriverError( ABCC_SeverityType  eSeverity, 
                          ABCC_ErrorCodeType iErrorCode, 
                          UINT32             lAddInfo )
{
  (void)eSeverity;
  (void)iErrorCode;
  (void)lAddInfo;
  ABCC_PORT_DebugPrint( ( "ERROR: Severity:%d Error code:%d, AddInfo:%d\n",
                          eSeverity, iErrorCode, lAddInfo ) );
}

//////////////////////////////////////////////////////////////////////////
/**
  A message has been received from the ABCC. This is the receive function for
  all received commands from the ABCC. 

  @param psReceivedMsg
    Pointer to received message.

  @note 
    It could also be used as a response handler if passed on as an argument 
    to the ABCC_SendCmdMsg() function.
    Regarding callback context, see comment for callback section above
*/
//////////////////////////////////////////////////////////////////////////
void ABCC_CbfReceiveMsg( ABP_MsgType* psReceivedMsg )
{
   switch(  ABCC_GetMsgDestObj( psReceivedMsg ) )
   {
#if EPL_OBJ_ENABLE
   case ABP_OBJ_NUM_EPL:

      EPL_ProcessCmdMsg( psReceivedMsg );
      break;
#endif
#if EIP_OBJ_ENABLE
   case ABP_OBJ_NUM_EIP:

      EIP_ProcessCmdMsg( psReceivedMsg );
      break;
#endif
#if PRT_OBJ_ENABLE
   case ABP_OBJ_NUM_PNIO:

      PRT_ProcessCmdMsg( psReceivedMsg );
      break;
#endif
#if DPV1_OBJ_ENABLE
   case ABP_OBJ_NUM_DPV1:

      DPV1_ProcessCmdMsg( psReceivedMsg );
      break;
#endif
#if DEV_OBJ_ENABLE
   case ABP_OBJ_NUM_DEV:

      DEV_ProcessCmdMsg( psReceivedMsg );
      break;
#endif
#if MOD_OBJ_ENABLE
   case ABP_OBJ_NUM_MOD:

      MOD_ProcessCmdMsg( psReceivedMsg );
      break;
#endif
#if COP_OBJ_ENABLE
   case ABP_OBJ_NUM_COP:

      COP_ProcessCmdMsg( psReceivedMsg );
      break;
#endif
#if ETN_OBJ_ENABLE
   case ABP_OBJ_NUM_ETN:

      ETN_ProcessCmdMsg( psReceivedMsg );
      break;
#endif
#if ECT_OBJ_ENABLE
   case ABP_OBJ_NUM_ECT:

      ECT_ProcessCmdMsg( psReceivedMsg );
      break;
#endif
   case ABP_OBJ_NUM_APPD:

      AD_ProcObjectRequest( psReceivedMsg );
      break;

#if APP_OBJ_ENABLE
   case ABP_OBJ_NUM_APP:

      APP_ProcessCmdMsg( psReceivedMsg );
      break;
#endif
#if SYNC_OBJ_ENABLE
   case ABP_OBJ_NUM_SYNC:

      SYNC_ProcessCmdMsg( psReceivedMsg );
      break;
#endif

   default:

      /*
      ** We have received a command to an unsupported object.
      */
      ABP_SetMsgErrorResponse( psReceivedMsg, 1, ABP_ERR_UNSUP_OBJ );
      ABCC_SendRespMsg( psReceivedMsg );
      break;
   }
}

#if ABCC_CFG_SYNC_ENABLE
//////////////////////////////////////////////////////////////////////////
/**
  If sync is supported this function will be invoked at the sync event.
  The function is executed in interrupt context. If the separate sync pin in
  the ABCC interface is used this function shall be called from the interrupt
  handler. If the ABCC interrupt is used the driver will call this function.
*/
//////////////////////////////////////////////////////////////////////////
void ABCC_CbfSyncIsr( void )
{
   /*
   ** Call application specific handling of sync event
   */
   APPL_SyncIsr();
}
#endif

//////////////////////////////////////////////////////////////////////////
/**
  This function is called from ABCC_ISR() when events specified in
  ABCC_CFG_INT_ENABLE_MASK_X have occurred. 
  The function returns a mask of ABCC_ISR_EVENT_X bits with the currently 
  active events that has not already been handled by the ISR itself. 
  What interrupt to be handled by the ISR is defined in the 
  ABCC_CFG_HANDLE_INT_IN_ISR_MASK.
 
  @param iEvents
    Mask according to the ISR event bits ABCC_ISR_EVENT_X

  @note 
    This function is always called from interrupt context.
*/
//////////////////////////////////////////////////////////////////////////
void ABCC_CbfEvent( UINT16 iEvents )
{
   /*
   ** Set flag to indicate that an event has occurred and the corresponding
   ** ABCC_Trigger<event_action> must be called. In the sample code the the
   ** trigger function is called from main loop context.
   */
   if( iEvents & ABCC_ISR_EVENT_RDPD )
   {
      appl_fRdPdReceivedEvent = TRUE;
   }

   if( iEvents & ABCC_ISR_EVENT_RDMSG )
   {
      appl_fMsgReceivedEvent = TRUE;
   }

   if( iEvents & ABCC_ISR_EVENT_WRMSG )
   {
      appl_fTransmitMsgEvent = TRUE;
   }

   if( iEvents & ABCC_ISR_EVENT_STATUS  )
   {
      appl_fAbccStatusEvent = TRUE;
   }
}

//////////////////////////////////////////////////////////////////////////
/**
  This callback is invoked if the anybus changes state
  See ABP_AnbStateType in abp.h for more information.

  @param eNewAnbState
    New anybus state

  @note 
    Regarding callback context, see comment for callback section above.
*/
//////////////////////////////////////////////////////////////////////////
void ABCC_CbfAnbStateChanged( ABP_AnbStateType eNewAnbState )
{
  switch( eNewAnbState )
  {
    case ABP_ANB_STATE_PROCESS_ACTIVE:
      SetAnbState(eNewAnbState);
      ABCC_TriggerWrPdUpdate();
      break;
    case ABP_ANB_STATE_EXCEPTION:
    case ABP_ANB_STATE_ERROR:
    default:
      SetAnbState(eNewAnbState);
      break;
  }
}

//////////////////////////////////////////////////////////////////////////
/**
  This function needs to be implemented by the application. 
  The function is called to trigger a user specific setup during the ABCC setup state. 
  The ABCC driver will remain in ABCC_CFG_INIT state until ABCC_UserInitComplete() 
  is called by the application.  
  If no user specific setup is required, ABCC_UserInitComplete() must be called 
  inside this function otherwise setup complete will never be sent.

  @note 
    This function call will be invoked in same context as the read message handling.
    (See comment for callback section above)
*/
//////////////////////////////////////////////////////////////////////////
void ABCC_UserInitReq( void )
{
   UINT16 iNetworkType;

   iNetworkType = ABCC_NetworkType();

   if( ( iNetworkType == ABP_NW_TYPE_DEV ) ||
       ( iNetworkType == ABP_NW_TYPE_PDPV0 ) ||
       ( iNetworkType == ABP_NW_TYPE_PDPV1 ) ||
       ( iNetworkType == ABP_NW_TYPE_COP ) ||
       ( iNetworkType == ABP_NW_TYPE_CNT ) ||
       ( iNetworkType == ABP_NW_TYPE_CCL ) ||
       ( iNetworkType == ABP_NW_TYPE_CPN ) ||
       ( iNetworkType == ABP_NW_TYPE_ECT ) ||
       ( iNetworkType == ABP_NW_TYPE_EPL ) )
   {

      setNwSupportsNodeId (TRUE);
   }
   else
   {
      setNwSupportsNodeId (FALSE);
   }

   if( ( iNetworkType == ABP_NW_TYPE_DEV ) ||
       ( iNetworkType == ABP_NW_TYPE_COP ) ||
       ( iNetworkType == ABP_NW_TYPE_CCL ) )
   {
      setNwSupportsBaudRate ( TRUE );
   }
   else
   {
      setNwSupportsBaudRate ( FALSE );
   }
}
