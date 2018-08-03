///////////////////////////////////////////////////////////////////////////////
// IXXAT Automation GmbH
///////////////////////////////////////////////////////////////////////////////
/**
  header to define all exported function of the vciIDLc.dll

  @file 
*/
#ifndef __VCIIDLC_H__
#define __VCIIDLC_H__

//////////////////////////////////////////////////////////////////////////
// include files
#include "windows.h"

#include "vciIdType.h"
#include "vciIDL_def.h"
#include "vciIDLc_def.h"

#include <pshpack4.h>


//////////////////////////////////////////////////////////////////////////
// constants and macros


//////////////////////////////////////////////////////////////////////////
// data types


typedef long (__cdecl * vciIDLC_t_Callback )  ( HANDLE hDevice,
                                                void * pContext, 
                                                UINT16 wFuncIndex, 
                                                UINT32 * pdwRet,
                                                void   * pArg1,
                                                void   * pArg2);

typedef void (__cdecl * vciIDLC_t_Error)      ( void * pContext, 
                                                VCIIDLC_SeverityType wSeverity, 
                                                VCIIDLC_SeverityType wErrorCode, 
                                                UINT32             lAddInfo );
#define MAX_DBGTEXT 0x100
typedef void (__cdecl * vciIDLC_t_Info )      ( void * pContext, 
                                                char   szText[MAX_DBGTEXT] );



//////////////////////////////////////////////////////////////////////////
// functions



//////////////////////////////////////////////////////////////////////////
/**
  This function initialize the VCI for the calling thread.

  @return
    VCI_OK on success, otherwise an Error code
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C HRESULT WINAPI vciIDLcInitialize ( void );

//////////////////////////////////////////////////////////////////////////
/**
  This function is called to format the specified error code into a human
  readable text string.

  @param hrError
    error code to format
  @param pszText
    Points to the location where this function stores a human readable string 
    with error information. This buffer must be at least 'dwSize'bytes in size.
  @param dwSize
    size of the memory behind pszText
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C void    WINAPI vciIDLcFormatError ( IN  HRESULT hrError,
                                             OUT PCHAR   pszText,
                                             IN  UINT32  dwSize );

//////////////////////////////////////////////////////////////////////////
/**
  This function shows a message box according to the specified error code.

  @param hwndParent
    Handle to the owner window of the message box to be showed. If this 
    parameter is NULL, the message box has no owner window. 
  @param pszCaption
    Pointer to a null-terminated string that contains the title for the 
    message box. If this parameter is NULL, a default title is used.
  @param hrError
    Error code or -1 to retrieve the error code by a call to GetLastError. 
    If this parameter is set to 0, the function does not display a message box.
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C void    WINAPI vciIDLcDisplayError ( IN HWND    hwndParent,
                                              IN PCHAR   pszCaption,
                                              IN HRESULT hrError );

//////////////////////////////////////////////////////////////////////////
/**
  This function obtains the version of the currently running VCI.

  @param pdwMajorVersion
    Pointer to an variable where the function stores the major VCI version 
    number.
  @param pdwMinorVersion
    Pointer to an variable where the function stores the minor VCI version 
    number.

  @return
    VCI_OK on success, otherwise an Error code.
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C HRESULT WINAPI vciIDLcGetVersion ( OUT PUINT32 pdwMajorVersion,
                                            OUT PUINT32 pdwMinorVersion );

//////////////////////////////////////////////////////////////////////////
/**
  This function opens a VCI device enumerator.

  @param phEnum
    Points to a variable where the function stores a handle to the opened device 
    enumerator. If the function fails this variable is set to NULL.

  @return 
    If the function succeeds it returns VCI_OK, otherwise a value other 
    than VCI_OK.

  @note 
    Access:    public 
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C HRESULT WINAPI vciIDLcEnumDeviceOpen ( PHANDLE phEnum);

//////////////////////////////////////////////////////////////////////////
/**
  This function close the specified VCI device enumerator.

  @param hEnum
    Handle to the VCI device enumerator to close.

  @return 
    If the function succeeds it returns VCI_OK, otherwise a value other 
    than VCI_OK.

  @note 
    Access:    public 
    The handle specified by the parameter <hEnum> is not longer valid
    after the function returns and must not be used any longer.
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C HRESULT WINAPI vciIDLcEnumDeviceClose ( HANDLE hEnum);

//////////////////////////////////////////////////////////////////////////
/**
  This function retrieves information about the next available VCI device.

  @param hEnum
    Handle to the VCI device enumerator.
  @param pInfo
    Points to a VCIDEVICEINFO structure where the function stores information 
    about the next VCI device.

  @return 
    If the function succeeds it returns VCI_OK, otherwise a value other 
    than VCI_OK.
    E_HANDLE
    Invalid handle.

    E_INVALIDARG
    One or more arguments are invalid.

    E_NO_MORE_ITEMS
    No more entries are available from an enumeration operation.

  @note 
    Access:    public 
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C HRESULT WINAPI vciIDLcEnumDeviceNext ( HANDLE hEnum,
                                                PVCIIDL_DEVINFO pInfo);

//////////////////////////////////////////////////////////////////////////
/**
  This function resets the specified VCI device enumerator.

  @param hEnum
    Handle to the VCI device enumerator.

  @return 
    If the function succeeds it returns VCI_OK, otherwise a value other 
    than VCI_OK.

  @note 
    Access:    public 
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C HRESULT WINAPI vciIDLcEnumDeviceReset  ( HANDLE hEnum );

//////////////////////////////////////////////////////////////////////////
/**
  This function opens a device specified by the unique VCI object ID.

  @param rVciid
    Reference to the unique VCI object identifier of the device to open.
  @param phDevice
    Points to a variable where the function stores the handle to the opened device 
    object. If the function fails, this variable is set to NULL.

  @return 
    If the function succeeds it returns VCI_OK, otherwise a value other 
    than VCI_OK.

  @note 
    Access:    public 
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C HRESULT WINAPI vciIDLcDeviceOpen ( PVCIID pVciid, PHANDLE phDevice);

//////////////////////////////////////////////////////////////////////////
/**
  This function shows a dialog box which allows the user to select a VCI device 
  and opens the selected device.

  @param hwndParent
    Handle to the window that owns the dialog box.
  @param phDevice
    Points to a variable where the function stores the handle to the opened 
    device object. If the function fails, this variable is set to NULL.

  @return 
    If the function succeeds it returns VCI_OK, otherwise a value other 
    than VCI_OK.

  @note 
    Access:    public 
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C HRESULT WINAPI vciIDLcDeviceOpenDlg ( HWND hwndParent, PHANDLE phDevice);

//////////////////////////////////////////////////////////////////////////
/**
  This function close the specified VCI device.

  @param hDevice
    Handle to the VCI device to close.

  @return 
    If the function succeeds it returns VCI_OK, otherwise a value other 
    than VCI_OK.

  @note 
    Access:    public 
    The handle specified by the parameter <hDevice> is not longer valid
    after the function returns and must not be used any longer.
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C HRESULT WINAPI vciIDLcDeviceClose ( HANDLE hDevice );


//////////////////////////////////////////////////////////////////////////
/**
  This function retrieve information about the specified VCI device.

  @param hDevice
    Handle to the VCI device.
  @param pInfo
    Points to a VCIDEVICEINFO structure where the function stores information 
    about the device.

  @return 
    If the function succeeds it returns VCI_OK, otherwise a value other 
    than VCI_OK.

  @note 
    Access:    public 
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C HRESULT WINAPI vciIDLcDeviceGetInfo ( HANDLE hDevice,
                                               PVCIIDL_DEVINFO pInfo);

//////////////////////////////////////////////////////////////////////////
/**
  function to start the firmware on the device

  @param hDevice
    device handle
  @param pFirmware
    ptr on a hex array
  @param dwFirmwareSize  
    size in bytes of the hex array


  @return 
    If the function succeeds it returns VCI_OK, otherwise a value other 
    than VCI_OK.

  @note 
    Access:    public 
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C HRESULT WINAPI vciIDLcStart ( HANDLE hDevice, 
                                       PVOID  pFirmware, 
                                       UINT32 dwFirmwareSize,
                                       UINT16 wIntEnableMask,
                                       UINT16 wIntHandleInISR,
                                       UINT16 wUsrMaxNumApplCmds,
                                       UINT16 wUsrMaxNumAbccCmds,
                                       UINT16 wUsrMaxMsgSize);

//////////////////////////////////////////////////////////////////////////
/**
  function to stop the firmware on the device

  @param hDevice
    device handle

  @return 
    If the function succeeds it returns VCI_OK, otherwise a value other 
    than VCI_OK.

  @note 
    Access:    public 
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C HRESULT WINAPI vciIDLcStop ( HANDLE hDevice );


//////////////////////////////////////////////////////////////////////////
/**
  function to register a callback function 

  @param hDevice
    device handle 
  @param pContext
    only a pointer, which is submitted on a callback call
  @param pCallback
    ptr on callback function 

  @return 
    If the function succeeds it returns VCI_OK, otherwise a value other 
    than VCI_OK.

  @note 
    Access:    public 
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C HRESULT WINAPI vciIDLcRegisterDevCallback ( HANDLE hDevice,
                                                     void * pContext,
                                                     vciIDLC_t_Callback pCallback);

//////////////////////////////////////////////////////////////////////////
/**
  function to register a callback function 

  @param hDevice
    device handle 
  @param pContext
    only a pointer, which is submitted on a callback call
  @param pCallback
    ptr on callback function 

  @return 
    If the function succeeds it returns VCI_OK, otherwise a value other 
    than VCI_OK.

  @note 
    Access:    public 
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C HRESULT WINAPI vciIDLcRegisterCallbacks ( void * pErrContext,
                                                   vciIDLC_t_Error pErrorHdlr,
                                                   vciIDLC_t_Info  pInfo );

//////////////////////////////////////////////////////////////////////////
/**
  This function will initiate the driver, enable interrupt, and set the operation
  mode. 
  When this function has been called the timer system could be started.
   ( see ABCC_RunTimerSystem() )

  @param hDevice
    device handle
  @param lMaxStartupTimeMs
    Max startup time for ABCC

  @return 
    If the function succeeds it returns VCI_OK, otherwise a value other 
    than VCI_OK.

  @note 
    This function will NOT release the reset of the ABCC.
    To release reset, ABCC_HwReleaseReset() has to be called by the application.
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C VCIIDLC_StatusType WINAPI vciIDLcStartDriver ( HANDLE hDevice,
                                                        UINT32 lMaxStartupTimeMs );

//////////////////////////////////////////////////////////////////////////
/**
  Stops the driver and puts it into SHUTDOWN state. The ABCC will be reset.

  @param hDevice
    device handle
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C void WINAPI vciIDLcShutdownDriver ( HANDLE hDevice );

//////////////////////////////////////////////////////////////////////////
/**
  This function must be polled after the ABCC_StartDriver() until it returns
  the value TRUE.  This indicates that the ABCC is ready for communication and
  the ABCC setup sequence is started.

  @param hDevice
    device handle

  @return 
    VCIIDLC_CommunicationStateType
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C VCIIDLC_CommunicationStateType WINAPI vciIDLcIsReadyForCommunication ( HANDLE hDevice );


//////////////////////////////////////////////////////////////////////////
/**
  This function should be called from inside the ABCC interrupt routine to 
  acknowledge and handle received ABCC events ( Triggered by IRQ_n on the
  abcc application interface )
  The user configuration defines, ABCC_USER_INT_ENABLE_MASK and ABCC_USER_HANDLE_IN_ABCC_ISR_MASK,
  allows configuration of which events to handle by the ISR and which events to
  pass on to the application ( ABCC_CbfEvent() ).

  @param hDevice
    device handle

*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C void WINAPI vciIDLcISR ( HANDLE hDevice );

//////////////////////////////////////////////////////////////////////////
/**
  ABCC hardware reset.

  @param hDevice
    device handle

  @return 
    If the function succeeds it returns VCI_OK, otherwise a value other 
    than VCI_OK.

  @note 
    This function will only set reset pin to low. It the responsibility of
    the caller to make sure that the reset time is long enough.
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C void WINAPI vciIDLcHwReset ( HANDLE hDevice );

//////////////////////////////////////////////////////////////////////////
/**
  Releases the ABCC reset.

  @param hDevice
    device handle

*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C void WINAPI vciIDLcHwReleaseReset ( HANDLE hDevice );

//////////////////////////////////////////////////////////////////////////
/**
  This function drives the ABCC driver sending and receiving mechanism.
  The driver must be ready for communication before this function is called 
  (ABCC_isReadyForCommunication() must be TRUE). 
  This function could be called cyclically or be based on events from the ABCC. 
  If all events are handled in the interrupt context then there is no need to call 
  this function.

  @param hDevice
    device handle
  @param wUsrIntEnaMask

  @return 
    TRUE              - Normal operation
    FALSE             - If the driver is stopped or not started for some 
                        reason.
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C VCIIDLC_ErrorCodeType WINAPI vciIDLcRunDriver ( HANDLE hDevice, 
                                                         UINT16 wUsrIntEnaMask);

//////////////////////////////////////////////////////////////////////////
/**
  Sends a command message to the ABCC.
  The function ABCC_MsgAlloc() must be used to allocate the message buffer.
  It is OK to re-use a previously received response buffer. If ABCC_MSG_Q_FULL
  is returned then the message buffer will still owned by the caller and needs
  to be freed by the ABCC_MsgFree() function or be re-sent later.
  The driver will use the sourceId to map the response to the correct response 
   handler. ABCC_GetNewSourceId() could be used to provide an new source id.

   Example where ABCC_CbfMessageReceived() function is used as response handler:
     eResp = ABCC_SendCmdMsg( psMsg, ABCC_CbfMessageReceived );

  @param hDevice
    device handle
  @param psMsgCmd
    Pointer to the command message.
  @param pnMsgHandler
    Pointer to the function to handle the response message.

  @return 
    If the function succeeds it returns VCI_OK, otherwise a value other 
    than VCI_OK.
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C VCIIDLC_ErrorCodeType WINAPI vciIDLcCmdSendMsg ( HANDLE                  hDevice,
                                                          VCIIDLC_MsgType       * psMsgCmd,
                                                          vciIDLc_t_MsgHandler    pnMsgHdlr,
                                                          vciIDLc_t_MsgHandlerExt pnMsgHdlrExt);


//////////////////////////////////////////////////////////////////////////
/**
  Sends a command message to the ABCC.
  The function ABCC_MsgAlloc() must be used to allocate the message buffer.
  It is OK to re-use a previously received response buffer. If ABCC_MSG_Q_FULL
  is returned then the message buffer will still owned by the caller and needs
  to be freed by the ABCC_MsgFree() function or be re-sent later.
  The driver will use the sourceId to map the response to the correct response 
   handler. ABCC_GetNewSourceId() could be used to provide an new source id.

   Example where ABCC_CbfMessageReceived() function is used as response handler:
     eResp = ABCC_SendCmdMsg( psMsg, ABCC_CbfMessageReceived );

  @param hDevice
    device handle
  @param psMsgCmd
    Pointer to the command message.
  @param hEvent
    event handle 

  @return 
    If the function succeeds it returns VCI_OK, otherwise a value other 
    than VCI_OK.
*/
//////////////////////////////////////////////////////////////////////////
// EXTERN_C VCIIDLC_ErrorCodeType WINAPI vciIDLcCmdSendMsgEvt ( HANDLE                  hDevice,
//                                                              VCIIDLC_MsgType       * psMsgCmd,
//                                                              HANDLE                  hEvent);

//////////////////////////////////////////////////////////////////////////
/**
  function to read a response of a command identified by the unique source id

  @param hDevice
    device handle
  @param bSourceId
    unique id
  @param ppsMsgCmd
    ptr to store the command message ptr

  @return 
    If the function succeeds it returns VCI_OK, otherwise a value other 
    than VCI_OK.

  @note 
    Access:    public 
*/
//////////////////////////////////////////////////////////////////////////
// EXTERN_C VCIIDLC_ErrorCodeType WINAPI vciIDLcCmdReadResponse ( HANDLE              hDevice,
//                                                                UINT8               bSourceId,
//                                                                VCIIDLC_MsgType **  ppsMsgCmd);

//////////////////////////////////////////////////////////////////////////
/**
  function to release a response of a command identified by the unique source id

  @param hDevice
    device handle
  @param bSourceId
    unique id
  @param ppsMsgCmd
    ptr to store the command message ptr

  @return 
    If the function succeeds it returns VCI_OK, otherwise a value other 
    than VCI_OK.

  @note 
    Access:    public 
*/
//////////////////////////////////////////////////////////////////////////
// EXTERN_C VCIIDLC_ErrorCodeType WINAPI vciIDLcCmdReleaseResponse ( HANDLE              hDevice, 
//                                                                   UINT8               bSourceId,
//                                                                   VCIIDLC_MsgType * * ppsMsgCmd);

//////////////////////////////////////////////////////////////////////////
/**
  Retrieves the number of entries left in the command queue.

  @return 
    Number queue entries left in command queue.

  @note 
    When sending a message the returned status must always be checked to 
    verify that the message has in fact been sent.
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C UINT16 WINAPI vciIDLcGetCmdQueueSize ( HANDLE hDevice );

//////////////////////////////////////////////////////////////////////////
/**
  Sends a response message to the ABCC.

  @param hDevice
    device handle
  @param psMsgResp
    Pointer to the command message.

  @return 
    If the function succeeds it returns VCI_OK, otherwise a value other 
    than VCI_OK.

  @note 
    The received command buffer can be reused as a response buffer. If a 
    new buffer is used then the function ABCC_MsgAlloc() must be used to allocate 
    the buffer. If ABCC_MSG_Q_FULL is returned then the message buffer will still 
    owned by the caller and needs to be freed by the ABCC_MsgFree() function or 
    be re-sent later.
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C VCIIDLC_ErrorCodeType WINAPI vciIDLcSendRespMsg ( HANDLE hDevice, 
                                                           VCIIDLC_MsgType* psMsgResp );



//////////////////////////////////////////////////////////////////////////
/**
  Sets the current application status.

  @param hDevice
    device handle
  @param eAppStatus
    Current application status

  @return 
    If the function succeeds it returns VCI_OK, otherwise a value other 
    than VCI_OK.

  @note 
    This information is only supported in SPI and parallel operation mode.
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C void WINAPI vciIDLcSetAppStatus ( HANDLE hDevice, 
                                           VCIIDLC_AppStatusType eAppStatus  );

//////////////////////////////////////////////////////////////////////////
/**
  Allocates a message buffer. This function MUST be used when allocating
  message buffers. The size of the buffer  is controlled by ABCC_CFG_MAX_MSG_SIZE.

  @return 
    Pointer to the message buffer. NULL is returned if no resource is available.
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C VCIIDLC_MsgType* WINAPI vciIDLcGetCmdMsgBuffer ( HANDLE hDevice );

//////////////////////////////////////////////////////////////////////////
/**
  Takes the ownership of the message buffer. The driver will not free this
  buffer when returning from e.g. a response callback. It is the user's
  responsibility to free this buffer when it is not needed anymore by using
  ABCC_ReturnMsgBuffer().

  @param psMsg
    Pointer to the message buffer to take ownership of

  @return 
    ABCC_StatusType

  @note 
    Access:    public 
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C void WINAPI vciIDLcTakeMsgBufferOwnership( HANDLE hDevice, 
                                                    VCIIDLC_MsgType* psMsg );

//////////////////////////////////////////////////////////////////////////
/**
  Returns the message buffer to the driver to for other messages.

  @param hDevice
    device handle
  @param psMsg
    Pointer to the message buffer that was freed. 
    The buffer pointer will be set to NULL.

  @return 
    If the function succeeds it returns VCI_OK, otherwise a value other 
    than VCI_OK.
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C VCIIDLC_ErrorCodeType WINAPI vciIDLcReturnMsgBuffer ( HANDLE hDevice, 
                                                               VCIIDLC_MsgType* psMsg );

//////////////////////////////////////////////////////////////////////////
/**
  Reads the module ID.

  @param hDevice
    device handle

  @return 
    ModuleId
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C UINT8 WINAPI vciIDLcReadModuleId ( HANDLE hDevice );


//////////////////////////////////////////////////////////////////////////
/**
  Detects if a module is present.

  @param hDevice
    device handle

  @return 
    TRUE              - Module detected.
    FALSE             - No module detected
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C BOOL WINAPI vciIDLcModuleDetect ( HANDLE hDevice );

//////////////////////////////////////////////////////////////////////////
/**
  Reads the module capability. This function is only supported by the ABCC40 
  parallel operation mode.

  @param hDevice
    device handle

  @return 
    Module capability.
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C UINT16 WINAPI vciIDLcModCap ( HANDLE hDevice );

//////////////////////////////////////////////////////////////////////////
/**
  Reads the LED status. Only supported in SPI and parallel operation mode.

  @param hDevice
    device handle

  @return 
    LED status according to the software design guide.
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C UINT16 WINAPI vciIDLcLedStatus ( HANDLE hDevice );

//////////////////////////////////////////////////////////////////////////
/**
  Reads the current Anybus state.

  @param hDevice
    device handle

  @return 
    Anybus state.
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C UINT8 WINAPI vciIDLcAnbState ( HANDLE hDevice );

//////////////////////////////////////////////////////////////////////////
/**
  Returns the current status of the supervision bit.

  @param hDevice
    device handle

  @return 
    TRUE           - Is supervised by another network device.
    FALSE          - Not supervised.

*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C BOOL WINAPI vciIDLcIsSupervised ( HANDLE hDevice );

//////////////////////////////////////////////////////////////////////////
/**
  This function will read the operation mode from HW. 
  If the operation is known and fixed or in any other way decided by the
  application this function could be ignored.

  @param hDevice
    device handle

  @return 
    The ABCC40 4 bit operation mode according to abp.h
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C UINT8 WINAPI vciIDLcGetOpmode ( HANDLE hDevice );

//////////////////////////////////////////////////////////////////////////
/**
  This function is called to trigger a RdPd read. If the read process data is 
  available then a call to the function ABCC_CbfNewReadPd() will be triggered.

  @param hDevice
    device handle
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C void WINAPI vciIDLcTriggerRdPdUpdate ( HANDLE hDevice );

//////////////////////////////////////////////////////////////////////////
/**
  function to start the setup state machine

  @param hDevice
    device handle

  @return 
    TRUE on success, otherwise FALSE
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C BOOL WINAPI vciIDLcTriggerSetupSM ( HANDLE hDevice );

//////////////////////////////////////////////////////////////////////////
/**
  This function is called to trigger a message receive read. If a read message 
  is available then the corresponding message handler will be called.

  @param hDevice
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C  void WINAPI vciIDLcTriggerReceiveMessage ( HANDLE hDevice );

//////////////////////////////////////////////////////////////////////////
/**
  This function indicates that new process data from the application is
  available and will be sent to the ABCC.

  @param hDevice
    device handle
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C void WINAPI vciIDLcTriggerWrPdUpdate ( HANDLE hDevice );


//////////////////////////////////////////////////////////////////////////
/**
  Check if current anybus status has changed.
  If the status  is changed  ABCC_CbfAnbStatusChanged() will be invoked.

  @param hDevice
    device handle
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C void WINAPI vciIDLcTriggerAnbStatusUpdate ( HANDLE hDevice );

//////////////////////////////////////////////////////////////////////////
/**
  Checks if there are any messages to send.

  @param hDevice
    device handle
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C void WINAPI vciIDLcTriggerTransmitMessage ( HANDLE hDevice );


/*******************************************************************************
** Message support functions
********************************************************************************
*/


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
EXTERN_C void WINAPI vciIDLcSetMsgHeader ( VCIIDLC_MsgType* psMsg,
                                           UINT8  bObject,
                                           UINT16 iInstance,
                                           UINT8  bAttribute,
                                           UINT8  bCmd,
                                           UINT16 iDataSize,
                                           UINT8  bSourceId );

//////////////////////////////////////////////////////////////////////////
/**
  This function verifies an ABCC response message.

  @param psMsg
    Pointer to message buffer.

  @return 
    VCIIDLC_ErrorCodeType
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C VCIIDLC_ErrorCodeType WINAPI vciIDLcVerifyMessage ( const VCIIDLC_MsgType* const psMsg );

//////////////////////////////////////////////////////////////////////////
/**
  This function returns a new source id that could used when sending a command
  message. It is guaranteed be unique if this function is used every time a new
  command is sent. The alternative would be that the user uses fixed source
  id:s.

  @param hDevice
    device handle

  @return 
    new source id

  @note 
    Access:    public 
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C UINT8 WINAPI vciIDLcGetNewSourceId ( HANDLE hDevice );

//////////////////////////////////////////////////////////////////////////
/**
  Sets the new process data sizes.

  @param hDevice
    device handle
  @param iReadPdSize
    Size of the read process data (in bytes), used from this point on.
  @param iWritePdSize
    Size of the write process data (in bytes), used from this point on.

  @note 
    Access:    public 
*/
//////////////////////////////////////////////////////////////////////////
EXTERN_C void WINAPI vciIDLcSetPdSize ( HANDLE hDevice, 
                                        const UINT16 iReadPdSize, 
                                        const UINT16 iWritePdSize );


#include <poppack.h>

#endif  //__VCIIDLC_H__
