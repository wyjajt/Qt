//////////////////////////////////////////////////////////////////////////
// HMS Technology Center Ravensburg GmbH
//////////////////////////////////////////////////////////////////////////
/**
  implementation of the direct hardware access

  @file abcc_driver.c
*/
//////////////////////////////////////////////////////////////////////////
// (C) 2002-2015  HMS, all rights reserved
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// compiler directives
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// include files
//////////////////////////////////////////////////////////////////////////
#include <Windows.h>
#include <process.h>
#include <stdio.h>

#include "abcc_drv_cfg.h"

#include "vciIdType.h"
#include "abcc.h"

#include "vciIDLc.h"

//////////////////////////////////////////////////////////////////////////
// static constants, types, macros, variables
//////////////////////////////////////////////////////////////////////////
static unsigned eventThreadId;
static HANDLE   hEventThread     = INVALID_HANDLE_VALUE;
static LONG     lEventThreadFlag = 0;  // quit flag for the event receive thread
static HANDLE   g_hDevice = NULL;

unsigned __stdcall ISR( void *pMyID );

//////////////////////////////////////////////////////////////////////////
// global variables
//////////////////////////////////////////////////////////////////////////

/*
** Registered handler functions
*/

ABCC_ErrorCodeType ( *ABCC_RunDriver )( void );
void ( *ABCC_TriggerWrPdUpdate )( void );

//////////////////////////////////////////////////////////////////////////
// static function prototypes
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// global functions
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static functions
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/**
  function called if a a event occured

  @param hDevice
    handle of calling device
  @param pContext
    ptr on Context
  @param wFuncIndex
    function index
  @param pdwRet
    parameter to return a result to the calling function
  @param pArg1
    argument 1 ( depends on function index )
  @param pArg2
    argument 1 ( depends on function index )

  @return 
    If the function succeeds it returns S_OK, otherwise a value other 
    than S_OK

*/
//////////////////////////////////////////////////////////////////////////
HRESULT __cdecl Callback ( HANDLE   hDevice,
                           void   * pContext, 
                           UINT16   wFuncIndex, 
                           UINT32 * pdwRet,
                           void   * pArg1,
                           void   * pArg2)
{
//    printf( "wFuncIndex:%d\n", wFuncIndex);
    UINT16 dwRet = 0;

    switch (wFuncIndex)
    {
    case VCIIDLC_CBIDX_TEST                   : //0
        break;
    case VCIIDLC_CBIDX_NEWREADPD              : //1
        ABCC_CbfNewReadPd(pArg1);
        break;
    case VCIIDLC_CBIDX_RECEIVEMSG             : //2
        ABCC_CbfReceiveMsg( (ABP_MsgType *) pArg1);
//        ABCC_CbfNewReadPd(pArg1);
//        dwRet = ABCC_CbfUpdateWriteProcessData(pArg1);
        break;
    case VCIIDLC_CBIDX_UPDATEWRITEPROCESSDATA : //3
        dwRet = ABCC_CbfUpdateWriteProcessData(pArg1);
        break;
    case VCIIDLC_CBIDX_EVENT                  : //4
        ABCC_CbfEvent( (UINT16) pArg1);
        break;
    case VCIIDLC_CBIDX_AnbStatusChanged       : //5
        ABCC_CbfAnbStateChanged((ABP_AnbStateType)pArg1);
        break;
    }

    if ( pdwRet )
    {
        *pdwRet = dwRet;
    }

    return S_OK;
}

//////////////////////////////////////////////////////////////////////////
/**
  called if an Error occurred

  @param pContext
    ptr on Context
  @param wSeverity
    error severity
  @param wErrorCode
    error code
  @param lAddInfo
    additional info

*/
//////////////////////////////////////////////////////////////////////////
void __cdecl ErrorCallback ( void * pContext, 
                             VCIIDLC_SeverityType wSeverity, 
                             VCIIDLC_SeverityType wErrorCode, 
                             UINT32             lAddInfo )
{
  static UINT16 iErrcount = 0;
  printf("ERROR: Severity:%d Error code:%d, AddInfo:%d\n",wSeverity, wErrorCode, lAddInfo);  
}

//////////////////////////////////////////////////////////////////////////
/**
  called if an info callback occured

  @param pContext
    ptr on Context
  @param pszText
    information string

*/
//////////////////////////////////////////////////////////////////////////
void __cdecl InfoCallback ( void * pContext, char * pszText )
{
  static UINT16 iErrcount = 0;
  printf("Info: %s \n",pszText);  
}

//////////////////////////////////////////////////////////////////////////
/**
  interrupt thread 

  @param hDevice
    hardware device handle !

  @return 
    If the function succeeds it returns S_OK, otherwise a value other 
    than S_OK.
*/
//////////////////////////////////////////////////////////////////////////
unsigned __stdcall ISR( HANDLE hDevice )
{
  while (lEventThreadFlag == 0)
  {
    vciIDLcISR(hDevice);
  }

  _endthread();
  return 0;
}

//////////////////////////////////////////////////////////////////////////
/**
  function to start the communication thread

  @param hDevice
    device handle
*/
//////////////////////////////////////////////////////////////////////////
void ABCC_StartThread ( HANDLE hDevice )
{
  HRESULT hResult = E_FAIL;

  if (hDevice)
  {
    InterlockedExchange(&lEventThreadFlag, 0);
    hEventThread = (HANDLE)_beginthreadex(NULL, 0, &ISR, hDevice, 0, &eventThreadId);
  }
}

//////////////////////////////////////////////////////////////////////////
/**
  function to stop the communication thread

  @param hDevice
    device handle
*/
//////////////////////////////////////////////////////////////////////////
void ABCC_StopThread ( void )
{
  if ( hEventThread != INVALID_HANDLE_VALUE)
  {
    InterlockedExchange(&lEventThreadFlag, 1);
    WaitForSingleObject( hEventThread, INFINITE );
    CloseHandle( hEventThread );
    hEventThread = INVALID_HANDLE_VALUE;
  }
}

//////////////////////////////////////////////////////////////////////////
/**
  This function indicates that new process data from the application is
  available and will be sent to the ABCC.
*/
//////////////////////////////////////////////////////////////////////////
void LOCAL_TriggerWrPdUpdate ( void )
{
  vciIDLcTriggerWrPdUpdate(g_hDevice);
}

//////////////////////////////////////////////////////////////////////////
/**
  This function drives the ABCC driver sending and receiving mechanism.
  The driver must be ready for communication before this function is called
  (ABCC_isReadyForCommunication() must be TRUE). This function could be called
  cyclically or be based on events from the ABCC. If all events are handled in
  the interrupt context then there is no need to call this function.

  @return 
    ABCC_ErrorCodeType

  @note 
    CHANGED
*/
//////////////////////////////////////////////////////////////////////////
ABCC_ErrorCodeType LOCAL_RunDriver ( void )
{
  UINT16 wUsrIntEnaMask = 0;
  BOOL   fPollWrPd      = FALSE;

  #ifdef ABCC_USER_POLL_WRPD
    fPollWrPd = TRUE;
  #endif

  #ifdef ABCC_USER_INT_ENABLED
    wUsrIntEnaMask = ABCC_USER_INT_ENABLE_MASK;
  #endif

  return vciIDLcRunDriver( g_hDevice, wUsrIntEnaMask );
}

//////////////////////////////////////////////////////////////////////////
/**
  This function will open the access to the hardware 

  @param fSelectDlg
    TRUE if a dialog should be used to select a hardware !
  @param pszHardwareSerial
    if fSelectDlg is False this value defines the hardware serial number
    to select 
  @param pDevice
    ptr to return the device handler

  @return 
    If the function succeeds it returns VCI_OK, otherwise a value other 
    than VCI_OK.

  @note 
    Access:    public 
    This function is called by the application before the driver is accessed.
*/
//////////////////////////////////////////////////////////////////////////
HRESULT ABCC_AccessDriver( BOOL     fSelectDlg,
                            char   * pszHardwareSerial, 
                            HANDLE * pDevice )
{
    HRESULT hResult = E_FAIL;
    HANDLE  hEnum   = NULL;
    HANDLE  hDevice = NULL;

    if( pDevice)
    {
        if( fSelectDlg )
        {
            //
            // selection through dialog
            //
            hResult = vciIDLcDeviceOpenDlg(  NULL, &hDevice );
        }
        else
        {
            //
            // Open without dialog
            //
            BOOL           fFound   = FALSE;
            VCIIDL_DEVINFO sDevInfo = { 0 };
            //
            // selection through enumeration
            //
            hResult = vciIDLcEnumDeviceOpen( &hEnum );

            if ( S_OK == hResult )
            {
                while ( S_OK == vciIDLcEnumDeviceNext (hEnum, &sDevInfo))
                {
                  if ( pszHardwareSerial )
                  {
                    if ( 0 == strcmp ( pszHardwareSerial, sDevInfo.UniqueHardwareId.AsChar ))
                    {
                      fFound = TRUE;
                      break;
                    }
                  }
                  else
                  {
                    // select specific destination hardware
                    fFound = TRUE;
                  }
                }

                vciIDLcEnumDeviceClose(hEnum);
            }

            if( fFound)
            {
                hResult = vciIDLcDeviceOpen ( &sDevInfo.VciObjectId, &hDevice);
            }
            else
            {
                hResult = E_INVALIDARG;
            }
        }

        //板卡打开成功
        if( S_OK == hResult )
        {
            hResult = vciIDLcRegisterDevCallback( hDevice,
                                                  NULL,
                                                  Callback);

            hResult = vciIDLcRegisterCallbacks( NULL,
                                                ErrorCallback,
                                                InfoCallback);


            hResult = vciIDLcStart( hDevice,
                                    NULL,
                                    0,
                                    ABCC_CFG_INT_ENABLE_MASK_PAR,
                                    ABCC_CFG_INT_ENABLE_MASK_PAR,
                                    ABCC_CFG_MAX_NUM_APPL_CMDS,
                                    ABCC_CFG_MAX_NUM_ABCC_CMDS,
                                    ABCC_CFG_MAX_MSG_SIZE);

            if( S_OK == hResult )
            {
                *pDevice  = hDevice;
                g_hDevice = hDevice;

                ABCC_TriggerWrPdUpdate = &LOCAL_TriggerWrPdUpdate;
                ABCC_RunDriver         = &LOCAL_RunDriver;
            }
            else
            {
                HRESULT hr = vciIDLcDeviceClose (hDevice);
            }
        }
    }

    return hResult;
}


//////////////////////////////////////////////////////////////////////////
/**
  This function will initiate the hardware required to communicate with the
  ABCC. 

  @param fSelectDlg
    TRUE if a dialog should be used to select a hardware !
  @param pszHardwareSerial
    if fSelectDlg is False this value defines the hardware serial number
    to select 

  @return 
    ABCC_ErrorCodeType
*/
//////////////////////////////////////////////////////////////////////////
ABCC_ErrorCodeType ABCC_HwInit( BOOL fSelectDlg, char * pszHardwareSerial )
{
    HRESULT hResult = ABCC_AccessDriver( fSelectDlg,
                                        pszHardwareSerial,
                                        &g_hDevice );

    if( S_OK == hResult )
    {
        return ABCC_EC_NO_ERROR;
    }

    return ABCC_EC_HW_INIT_FAILED;
}

//////////////////////////////////////////////////////////////////////////
/**
  This function will release the hardware 

  @return 
    ABCC_ErrorCodeType
*/
//////////////////////////////////////////////////////////////////////////
ABCC_ErrorCodeType ABCC_HwRelease( void )
{
  HRESULT hResult = vciIDLcStop (g_hDevice);

  if ( S_OK == hResult)
  {
    hResult = vciIDLcDeviceClose (g_hDevice);
  }

  if ( S_OK == hResult )
  {
    return ABCC_EC_NO_ERROR;
  }

  return ABCC_EC_INTERNAL_ERROR;
}

//////////////////////////////////////////////////////////////////////////
/**
  This function returns the size of an ABP data type.

  @param bDataType
    Data type number.

  @return 
    Data type size in bytes.
    CHANGED
*/
//////////////////////////////////////////////////////////////////////////
UINT8 ABCC_GetDataTypeSize( UINT8 bDataType )
{
  UINT8 bSize = 0;

  switch( bDataType )
  {
    case ABP_BOOL:
    case ABP_CHAR:
    case ABP_UINT8:
    case ABP_ENUM:
    case ABP_SINT8:
    case ABP_BITS8:
    case ABP_OCTET:
      bSize = 1;
      break;
    case ABP_UINT16:
    case ABP_SINT16:
    case ABP_BITS16:
      bSize = 2;
      break;
    case ABP_UINT32:
    case ABP_SINT32:
    case ABP_FLOAT:
    case ABP_BITS32:
      bSize = 4;
      break;
    case ABP_UINT64:
    case ABP_SINT64:
      bSize = 8;
      break;
    default:
      //while(1); /* Trap */
      break;
  }

  return( bSize );
}

//////////////////////////////////////////////////////////////////////////
/**
  Sends a response message to the ABCC.

  @param psMsgResp
    Pointer to the message.

  @return 
    ABCC_ErrorCodeType

  @note 
    The received command buffer can be reused as a response buffer. If a
    new buffer is used then the function ABCC_GetCmdMsgBuffer() must be used to
    allocate the buffer.

@note CHANGED 
*/
//////////////////////////////////////////////////////////////////////////
ABCC_ErrorCodeType ABCC_SendRespMsg( ABP_MsgType* psMsgResp )
{
  ABCC_ErrorCodeType eRet;
  eRet = vciIDLcSendRespMsg( g_hDevice, (VCIIDLC_MsgType *) psMsgResp );

  if ( eRet != ABCC_EC_NO_ERROR)
  {
    printf ( "ABCC_SendRespMsg Error\n");
  }

  return eRet;
}

//////////////////////////////////////////////////////////////////////////
/**
  Sends a command message to the ABCC.

  @param psMsgCmd
    Pointer to the command message.
  @param pnMsgHandler
    Pointer to the function to handle the response message.

  @return 
    ABCC_ErrorCodeType

  @note 
    The function ABCC_GetCmdMsgBuffer() must be used to allocate the message
    buffer. It is OK to re-use a previously received response buffer
    The driver will use the sourceId to map the response to the correct response
    handler. ABCC_GetNewSourceId() could be used to provide an new source id.
    Example where ABCC_CbfMessageReceived() function is used as response handler

@note CHANGED 
*/
//////////////////////////////////////////////////////////////////////////
ABCC_ErrorCodeType ABCC_SendCmdMsg ( ABP_MsgType* psMsgCmd,
                                     ABCC_MsgHandlerFuncType pnMsgHandler )
{
  ABCC_ErrorCodeType eRet;
  eRet = vciIDLcCmdSendMsg( g_hDevice,
                            (VCIIDLC_MsgType *) psMsgCmd, 
                            (vciIDLc_t_MsgHandler) pnMsgHandler,
                            NULL );

  if ( eRet != ABCC_EC_NO_ERROR)
  {
    printf ( "ABCC_SendCmdMsg Error\n");
  }

  return eRet;
}





//////////////////////////////////////////////////////////////////////////
/**
  This function sets the input arguments to the ABCC message header correctly.
  The data must be copied to message data buffer separately.

  @param psMsg
    Pointer to message buffer.
  @param bObject
    Object number
  @param iInstance
    Instance number
  @param bAttribute
    Attribute number
  @param eService
    Message command
  @param iDataSize
    Size of the message data in bytes
  @param bSourceId
    source identifier

  @return 
    If the function succeeds it returns VCI_OK, otherwise a value other 
    than VCI_OK.

  @note 
    Access:    public 
*/
//////////////////////////////////////////////////////////////////////////
void ABCC_SetMsgHeader( ABP_MsgType* psMsg,
                        UINT8 bObject,
                        UINT16 iInstance,
                        UINT8 bAttribute,
                        ABP_MsgCmdType eService,
                        UINT16 iDataSize,
                        UINT8 bSourceId )
{
  vciIDLcSetMsgHeader( ( VCIIDLC_MsgType *) psMsg, 
                          bObject, 
                          iInstance, 
                          bAttribute, 
                          eService , 
                          iDataSize, 
                          bSourceId );
}


//////////////////////////////////////////////////////////////////////////
/**
  This function fills an ABCC message with parameters to get an attribute.

  @param psMsg
    Pointer to message buffer
  @param bObject
    Object number
  @param iInstance
    Instance number
  @param bAttribute
    Attribute number
  @param bSourceId
    source identifier
*/
//////////////////////////////////////////////////////////////////////////
void ABCC_GetAttribute( ABP_MsgType* psMsg,
                        UINT8 bObject,
                        UINT16 iInstance,
                        UINT8 bAttribute,
                        UINT8 bSourceId )
{
  vciIDLcSetMsgHeader( (VCIIDLC_MsgType *) psMsg, 
                       bObject, 
                       iInstance, 
                       bAttribute, 
                       ABP_CMD_GET_ATTR, 
                       0, 
                       bSourceId );
}


//////////////////////////////////////////////////////////////////////////
/**
  This function fills an ABCC message with parameters in order to set an
  attribute.

  @param psMsg
    Pointer to message buffer.
  @param bObject
    Object number.
  @param iInstance
    Instance number.
  @param bAttribute
    Attribute number.
  @param bVal
    Value to set
  @param bSourceId
    Source identifier
*/
//////////////////////////////////////////////////////////////////////////
void ABCC_SetByteAttribute( ABP_MsgType* psMsg,
                            UINT8  bObject,
                            UINT16 iInstance,
                            UINT8  bAttribute,
                            UINT8  bVal,
                            UINT8  bSourceId )
{
  vciIDLcSetMsgHeader( (VCIIDLC_MsgType *) psMsg, 
                       bObject, 
                       iInstance, 
                       bAttribute, 
                       ABP_CMD_SET_ATTR, 
                       1, 
                       bSourceId );
  psMsg->abData[0] = bVal;  
}

//////////////////////////////////////////////////////////////////////////
/**
  This function verifies an ABCC response message.

  @param psMsg
    Pointer to message buffer.

  @return 
    ABCC_ErrorCodeType

@note CHANGED 
*/
//////////////////////////////////////////////////////////////////////////
ABCC_ErrorCodeType ABCC_VerifyMessage( const ABP_MsgType* psMsg )
{
  return vciIDLcVerifyMessage( (VCIIDLC_MsgType *) psMsg );
}

//////////////////////////////////////////////////////////////////////////
/**
  Reads the module ID.
*/
//////////////////////////////////////////////////////////////////////////
UINT8 ABCC_ReadModuleId( void )
{
  return vciIDLcReadModuleId(g_hDevice);
}


//////////////////////////////////////////////////////////////////////////
/**
  This function returns a new source id that could used when sending a command
  message. It is guaranteed be unique if this function is used every time a new
   command is sent. The alternative would be that the user uses fixed source
   id:s.

  @return 
    New SourceId
*/
//////////////////////////////////////////////////////////////////////////
UINT8 ABCC_GetNewSourceId(  )
{
  return vciIDLcGetNewSourceId( g_hDevice );
}

//////////////////////////////////////////////////////////////////////////
/**
  Reads the current Anybus state.

  @return 
    Anybus state
 
*/
//////////////////////////////////////////////////////////////////////////
UINT8 ABCC_AnbState( void)
{
  return vciIDLcAnbState( g_hDevice );
}

//////////////////////////////////////////////////////////////////////////
/**
  Stops the driver and puts it into SHUTDOWN state. The ABCC will be reset.
*/
//////////////////////////////////////////////////////////////////////////
void ABCC_ShutdownDriver( void )
{
  ABCC_StopThread ();
  vciIDLcShutdownDriver( g_hDevice );
}

//////////////////////////////////////////////////////////////////////////
/**
  This function shall be polled after ABCC_StartDriver() has been executed
  until ABCC_READY_FOR_COMMUNICATION is returned. This indicates that the ABCC
  is ready for communication and the ABCC setup sequence is started.

  @return 
    ABCC_CommunicationStateType

  @note CHANGED
*/
//////////////////////////////////////////////////////////////////////////
ABCC_CommunicationStateType ABCC_isReadyForCommunication( void )
{
  return (ABCC_CommunicationStateType) vciIDLcIsReadyForCommunication( g_hDevice );
}

//////////////////////////////////////////////////////////////////////////
/**
  ABCC hardware reset.

  @note 
    This function will only set reset pin to low. It the responsibility of
    the caller to make sure that the reset time is long enough.
*/
//////////////////////////////////////////////////////////////////////////
void ABCC_HWReset( void )
{
  vciIDLcHwReset( g_hDevice );
  ABCC_StopThread();
}

//////////////////////////////////////////////////////////////////////////
/**
  Releases the ABCC reset.
*/
//////////////////////////////////////////////////////////////////////////
void ABCC_HWReleaseReset( void )
{
  ABCC_StartThread( g_hDevice );
  vciIDLcHwReleaseReset( g_hDevice );
}

//////////////////////////////////////////////////////////////////////////
/**
  This function will call ABCC_SYS_GetOpmode() to read the operating mode from
  HW. If the operation is known and fixed or in any other way decided by the
  application this function could be ignored.

  @return 
    The ABCC40 4 bit operating mode according to abp.h

  @note 
    Access:    public 
*/
//////////////////////////////////////////////////////////////////////////
UINT8 ABCC_GetOpmode( void )
{
  return vciIDLcGetOpmode( g_hDevice );
}


//////////////////////////////////////////////////////////////////////////
/**
  Sets the new process data sizes.

  @param iReadPdSize
    Size of the read process data (in bytes), used from this point on.
  @param iWritePdSize
    Size of the write process data (in bytes), used from this point on.

  @note 
    Access:    public 
*/
//////////////////////////////////////////////////////////////////////////
void ABCC_SetPdSize ( const UINT16 iReadPdSize, 
                      const UINT16 iWritePdSize )
{
  vciIDLcSetPdSize ( g_hDevice, iReadPdSize, iWritePdSize );
}


//////////////////////////////////////////////////////////////////////////
/**
  This function will initiate the driver, enable interrupt, and set the
  operation mode. If a firmware update is pending a delay, iMaxStartupTime, can
  be defined describing how long the driver is to wait for the startup
  interrupt. iMaxStartupTime set to Zero (0) makes the driver use the
  ABCC_CFG_STARTUP_TIME_MS time.
  When this function has been called the timer system could be started,
  see ABCC_RunTimerSystem().

  @param lMaxStartupTimeMs
    Max startup time for ABCC

  @return 
    ABCC_ErrorCodeType

  @note 
    This function will NOT release the reset of the ABCC.
    To release reset, ABCC_HwReleaseReset() has to be called by the application.
    CHANGED
*/
//////////////////////////////////////////////////////////////////////////
ABCC_ErrorCodeType ABCC_StartDriver( UINT32 lMaxStartupTimeMs )
{
  return vciIDLcStartDriver( g_hDevice, lMaxStartupTimeMs);
}

//////////////////////////////////////////////////////////////////////////
/**
  This function is responsible for handling all timers for the ABCC-driver. 
  It is recommended to call this function on a regular basis from a timer
  interrupt. Without this function no timeout and watchdog functionality will
  work. This function can be called after ABCC_StartDriver() has been called.

  @param iDeltaTimeMs
    Milliseconds since last call.

  @note 
    Access:    public 
*/
//////////////////////////////////////////////////////////////////////////
void ABCC_RunTimerSystem( const INT16 iDeltaTimeMs )
{
//    void* pArg1;
//    ABCC_CbfNewReadPd(pArg1);
//    ABCC_CbfUpdateWriteProcessData(pArg1);
}



//////////////////////////////////////////////////////////////////////////
/**
  Check if current anybus status has changed.
  If the status  is changed  ABCC_CbfAnbStatusChanged() will be invoked.
*/
//////////////////////////////////////////////////////////////////////////
void ABCC_TriggerAnbStatusUpdate ( void )
{
  vciIDLcTriggerAnbStatusUpdate(g_hDevice);
}


//////////////////////////////////////////////////////////////////////////
/**
  Checks if there are any messages to send.
*/
//////////////////////////////////////////////////////////////////////////
void ABCC_TriggerTransmitMessage( void )
{
  vciIDLcTriggerTransmitMessage( g_hDevice );
}

//////////////////////////////////////////////////////////////////////////
/**
  This function is called to trigger a message receive read. If a read message
  is available then the corresponding message handler will be called.
*/
//////////////////////////////////////////////////////////////////////////
void ABCC_TriggerReceiveMessage ( void )
{
  vciIDLcTriggerReceiveMessage ( g_hDevice );
}

//////////////////////////////////////////////////////////////////////////
/**
  This function is called to trigger a RdPd read. If the read process data is
  available then a call to the function ABCC_CbfNewReadPd() will be triggered.

  @param 

  @return 
    If the function succeeds it returns VCI_OK, otherwise a value other 
    than VCI_OK.

  @note 
    Access:    public 
*/
//////////////////////////////////////////////////////////////////////////
void ABCC_TriggerRdPdUpdate( void )
{
  vciIDLcTriggerRdPdUpdate (g_hDevice);
}

//////////////////////////////////////////////////////////////////////////
/**
  Detects if a module is present. If the ABCC Module detect pins on the host
  connector is not connected (ABCC_CFG_MOD_DETECT_PINS_CONN shall be defined)
  this interface will always return TRUE.

  @return 
    TRUE  - Module detected.
    FALSE - No module detected
*/
//////////////////////////////////////////////////////////////////////////
BOOL ABCC_ModuleDetect( void )
{
  return vciIDLcModuleDetect( g_hDevice );
}

//////////////////////////////////////////////////////////////////////////
/**
  This function returns the size of an ABP data type in bits.

  @param bDataType
    Data type number.

  @return 
    Data type size in bits.
*/
//////////////////////////////////////////////////////////////////////////
UINT16 ABCC_GetDataTypeSizeInBits( UINT8 bDataType )
{
  UINT16 WSetBitSize;

  if( ABP_Is_PADx( bDataType ) )
  {
    WSetBitSize = bDataType - ABP_PAD0;
  }
  else if ( ABP_Is_BITx( bDataType ) )
  {
    WSetBitSize = ( ( bDataType - ABP_BIT1 ) + 1 );
  }
  else
  {
    WSetBitSize = (UINT16) ABCC_GetDataTypeSize(  bDataType );
    WSetBitSize *= 8;
  }

  return( WSetBitSize );
}


#if( ABCC_CFG_REMAP_SUPPORT_ENABLED )

  //////////////////////////////////////////////////////////////////////////
  /**
    Sends a remap response to the ABCC. When the response is sent the new process
    data sizes will be set and the function ABCC_CbfRemapDone() will be called to
    indicate the change.
  
    @param psMsgResp
      Pointer to the response message.
    @param iNewReadPdSize
      RdPd size when the remap is done.
    @param iNewWritePdSize
      WrPd size when the remap is done.
  
    @return 
      ABCC_ErrorCodeType
  
  */
  //////////////////////////////////////////////////////////////////////////
  ABCC_ErrorCodeType ABCC_SendRemapRespMsg( ABP_MsgType* psMsgResp, 
                                            UINT16       iNewReadPdSize, 
                                            const UINT16 iNewWritePdSize )
  {
    VCIIDLC_ErrorCodeType eResult;

    /*
    ** When ack is sent 
    */
    eResult = vciIDLcSendRespMsg ( g_hDevice, (VCIIDLC_MsgType *) psMsgResp );

    if ( VCIIDLC_EC_NO_ERROR == eResult )
    {
      vciIDLcSetPdSize ( g_hDevice, iNewReadPdSize, iNewWritePdSize );
      ABCC_CbfRemapDone();
    }

    return eResult;
  }
#endif


// proof !!
//////////////////////////////////////////////////////////////////////////
/**
  Retrieves a message buffer. 

  @return 
    Pointer to the message buffer. NULL is returned if no resource is available.

  @note 
    This function MUST be used when allocating message buffers. 
    The size of the buffer is controlled by ABCC_CFG_MAX_MSG_SIZE.
*/
//////////////////////////////////////////////////////////////////////////
ABP_MsgType* ABCC_GetCmdMsgBuffer( void )
{
  return (ABP_MsgType* )vciIDLcGetCmdMsgBuffer ( g_hDevice );
}

//////////////////////////////////////////////////////////////////////////
/**
  Takes the ownership of the message buffer. The driver will not free this
  buffer when returning from e.g. a response callback. It is the user's
  responsibility to free this buffer when it is not needed anymore by using
  ABCC_ReturnMsgBuffer().

  @param psMsg
    Pointer to the message buffer to take ownership of
*/
//////////////////////////////////////////////////////////////////////////
void ABCC_TakeMsgBufferOwnership( ABP_MsgType* psMsg )
{
  vciIDLcTakeMsgBufferOwnership( g_hDevice, (VCIIDLC_MsgType *)psMsg);
}

//////////////////////////////////////////////////////////////////////////
/**
  Returns the message buffer to the driver's message pool. 
  
  @param psMsg
    Pointer to the message buffer that was freed. 

  @note 
    This function may only be used in combination with ABCC_TakeMsgBufferOwnership().
*/
//////////////////////////////////////////////////////////////////////////
ABCC_ErrorCodeType ABCC_ReturnMsgBuffer ( ABP_MsgType* psMsg )
{
  return vciIDLcReturnMsgBuffer( g_hDevice, (VCIIDLC_MsgType *) psMsg );
}

