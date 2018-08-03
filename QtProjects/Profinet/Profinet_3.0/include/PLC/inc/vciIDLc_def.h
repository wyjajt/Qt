///////////////////////////////////////////////////////////////////////////////
// IXXAT Automation GmbH
///////////////////////////////////////////////////////////////////////////////
/**
  definition of structures to comunicate with the vciIDLc.dll

  @file 
*/
#ifndef __VCIIDLCDEF_H__
#define __VCIIDLCDEF_H__

//////////////////////////////////////////////////////////////////////////
// include files
#include "windows.h"

#include "vciIdType.h"
#include "vciIDL_def.h"

#include <pshpack4.h>


//////////////////////////////////////////////////////////////////////////
// constants and macros
#define VCIIDLC_PROCESS_DATA_SIZE              ( 1536 )

#define VCIIDLC_CBIDX_TEST                   0
#define VCIIDLC_CBIDX_NEWREADPD              1
#define VCIIDLC_CBIDX_RECEIVEMSG             2
#define VCIIDLC_CBIDX_UPDATEWRITEPROCESSDATA 3
#define VCIIDLC_CBIDX_EVENT                  4
#define VCIIDLC_CBIDX_AnbStatusChanged       5

//////////////////////////////////////////////////////////////////////////
// data types

/*------------------------------------------------------------------------------
**
** ABP_MsgHeaderType
**
** Structure describing a message header.
**
**------------------------------------------------------------------------------
*/

typedef struct _VCIIDLC_MsgHeaderType
{
   UINT16   iDataSize;
   UINT16   iReserved;
   UINT8    bSourceId;
   UINT8    bDestObj;
   UINT16   iInstance;
   UINT8    bCmd;
   UINT8    bReserved;
   UINT8    bCmdExt0;
   UINT8    bCmdExt1;
} VCIIDLC_MsgHeaderType;

typedef struct _VCIIDLC_MsgHeaderType16
{
  UINT16   iDataSize;
  UINT16   iReserved;
  UINT16   iSourceIdDestObj;
  UINT16   iInstance;
  UINT16   iCmdReserved;
  UINT16   iCmdExt0CmdExt1;
} VCIIDLC_MsgHeaderType16;

/*------------------------------------------------------------------------------
**
** VCIIDLC_MsgType
**
** Structure describing a message.
**
**------------------------------------------------------------------------------
*/
typedef struct _VCIIDLC_MsgType
{
  /*
  ** The message header part.
  */
  union {
    VCIIDLC_MsgHeaderType   sHeader;
    VCIIDLC_MsgHeaderType16 sHeader16;
  };

  /*
  ** The message data.
  */
  union {
    UINT8    abData[ 1 ];
    UINT16   aiData[ 1 ];
  };

} VCIIDLC_MsgType;




/*------------------------------------------------------------------------------
** Data format type.
**------------------------------------------------------------------------------
*/
typedef enum _VCIIDLC_DataFormatType
{
   VCIIDLC_DF_LSB_FIRST = 0,
   VCIIDLC_DF_MSB_FIRST = 1
} VCIIDLC_DataFormatType;


/*------------------------------------------------------------------------------
** Driver severity codes indicated by VCIIDLC_CbfDriverError.
**------------------------------------------------------------------------------
*/
typedef enum _VCIIDLC_SeverityType
{
   /* 
   ** Information about an event that has occurred (e.g., serial message lost).
   */
   VCIIDLC_SEV_INFORMATION = 0,
   
   /*
   ** An error of minor importance has occurred. The system can recover from
   ** this error.
   */
   VCIIDLC_SEV_WARNING,
   
   /*
   ** A fatal event has occurred, the system cannot recover (e.g., driver is
   ** out of timers).
   */
   VCIIDLC_SEV_FATAL,
   
   /*
   ** Force the compiler to use a 16-bit variable as enumeration.
   */
   VCIIDLC_SEV_SET_ENUM_ANSI_SIZE = 0x7FFF

} VCIIDLC_SeverityType;


/*------------------------------------------------------------------------------
**  Driver error codes indicated by VCIIDLC_CbfDriverError.
**------------------------------------------------------------------------------
*/
typedef enum _VCIIDLC_ErrorCodeType
{
  VCIIDLC_EC_NO_ERROR,
  VCIIDLC_EC_INTERNAL_ERROR,
  VCIIDLC_EC_LINK_CMD_QUEUE_FULL,
  VCIIDLC_EC_LINK_RESP_QUEUE_FULL,
  VCIIDLC_EC_OUT_OF_MSG_BUFFERS,
  VCIIDLC_EC_TRYING_TO_FREE_NULL_POINTER,
  VCIIDLC_EC_INCORRECT_OPERATING_MODE,
  VCIIDLC_EC_INCORRECT_STATE,
  VCIIDLC_EC_RESP_MSG_E_BIT_SET,
  VCIIDLC_EC_WRPD_SIZE_ERR,
  VCIIDLC_EC_RDPD_SIZE_ERR,
  VCIIDLC_EC_RDMSG_SIZE_ERR,
  VCIIDLC_EC_INVALID_RESP_SOURCE_ID,
  VCIIDLC_EC_MODULE_NOT_DECTECTED,
  VCIIDLC_EC_PARAMETER_NOT_VALID,
  VCIIDLC_EC_MODULE_ID_NOT_SUPPORTED,
  VCIIDLC_EC_DEFAULT_MAP_ERR,
  VCIIDLC_EC_ERROR_IN_READ_MAP_CONFIG,
  VCIIDLC_EC_ERROR_IN_WRITE_MAP_CONFIG,
  VCIIDLC_EC_INTSTATUS_NOT_SUPPORTED_BY_DRV_IMPL,
  VCIIDLC_EC_MODCAP_NOT_SUPPORTED_BY_DRV_IMPL,
  VCIIDLC_EC_SPI_OP_NOT_ALLOWED_DURING_SPI_TRANSACTION,
  VCIIDLC_EC_WRMSG_SIZE_ERR,
  VCIIDLC_EC_MSG_BUFFER_CORRUPTED,
  VCIIDLC_EC_MSG_BUFFER_ALREADY_FREED,
  VCIIDLC_EC_NO_RESOURCES,
  VCIIDLC_EC_HW_INIT_FAILED,
  VCIIDLC_EC_SET_ENUM_ANSI_SIZE       = 0x7FFF
} VCIIDLC_ErrorCodeType;

/*------------------------------------------------------------------------------
** ABCC_CommunicationStateType:
**
** ABCC_NOT_READY_FOR_COMMUNICATION: Nothing is wrong but it
**                                   is not yet possible to
**                                   communicate with the ABCC.
** ABCC_READY_FOR_COMMUNICATION:     Possible to communicate with ABCC
** ABCC_COMMUNICATION_ERROR:         ABCC module is not responding
**
**------------------------------------------------------------------------------
*/
typedef enum _VCIIDLC_CommunicationStateType
{
  VCIIDLC_NOT_READY_FOR_COMMUNICATION = 0,
  VCIIDLC_READY_FOR_COMMUNICATION = 1,
  VCIIDLC_COMMUNICATION_ERROR = 2
} VCIIDLC_CommunicationStateType;


/*------------------------------------------------------------------------------
** Type for command message sending status.
**  VCIIDLC_MSG_SENT: The message is already delivered to the ABCC.
**  VCIIDLC_IN_Q: The message is not yet sent but will be sent as soon as possible
**  VCIIDLC_MSG_Q_FULL: The message is not handled at all due to lack of resources.
**  VCIIDLC_MSG_SIZE_ERROR: The message has an unsupported size.
**  VCIIDLC_MSG_ERROR: Internal error.
**------------------------------------------------------------------------------
*/
typedef enum _VCIIDLC_MsgStatusType
{
   VCIIDLC_MSG_SENT = 0,   
   VCIIDLC_MSG_IN_Q,
   VCIIDLC_MSG_Q_FULL,
   VCIIDLC_MSG_SIZE_ERR,
   VCIIDLC_MSG_ERROR
} VCIIDLC_MsgStatusType;


/*------------------------------------------------------------------------------
** VCIIDLC_API_StatusType:
** Defines the current state of the driver.
**------------------------------------------------------------------------------
*/
typedef enum _VCIIDLC_StatusType
{
   VCIIDLC_OK,
   VCIIDLC_NOK
} VCIIDLC_StatusType;


/*------------------------------------------------------------------------------
**
** Application status data type.
**
**------------------------------------------------------------------------------
*/
typedef enum _VCIIDLC_AppStatusType
{
  VCIIDLC_APPSTAT_NO_ERROR           = 0x0000,
  VCIIDLC_APPSTAT_NOT_SYNCED         = 0x0001,
  VCIIDLC_APPSTAT_SYNC_CFG_ERR       = 0x0002,
  VCIIDLC_APPSTAT_READ_PD_CFG_ERR    = 0x0003,
  VCIIDLC_APPSTAT_WRITE_PD_CFG_ERR   = 0x0004,
  VCIIDLC_APPSTAT_SYNC_LOSS          = 0x0005,
  VCIIDLC_APPSTAT_PD_DATA_LOSS       = 0x0006,
  VCIIDLC_APPSTAT_OUTPUT_ERR         = 0x0007
} VCIIDLC_AppStatusType;

/*------------------------------------------------------------------------------
** Used for storing the data format of the field bus.
** NET_UNKNOWN means that the Anybus-CC has not yet responded to our command to
** read the fieldbus data format.
**------------------------------------------------------------------------------
*/
typedef enum _VCIIDLC_NetFormatType
{
   VCIIDLC_NET_LITTLEENDIAN,
   VCIIDLC_NET_BIGENDIAN,
   VCIIDLC_NET_UNKNOWN
} VCIIDLC_NetFormatType;

/*------------------------------------------------------------------------------
** Type for indicate if parameter support is available or not.
**------------------------------------------------------------------------------
*/
typedef enum _VCIIDLC_ParameterSupportType
{
   VCIIDLC_NOT_PARAMETER_SUPPORT,
   VCIIDLC_PARAMETER_SUPPORT,
   VCIIDLC_PARAMETER_UNKNOWN
} VCIIDLC_ParameterSupportType;

typedef enum _VCIIDLC_MemBufferStatusType
{
  VCIIDLC_MEM_BUFSTAT_FREE = 0,
  VCIIDLC_MEM_BUFSTAT_ALLOCATED = 1,
  //  ABCC_MEM_BUFSTAT_SENT = 2,
  VCIIDLC_MEM_BUFSTAT_OWNED = 3,
  VCIIDLC_MEM_BUFSTAT_UNKNOWN = 0x7FFF
} VCIIDLC_MemBufferStatusType;


typedef void (__cdecl * vciIDLc_t_MsgHandler)           ( VCIIDLC_MsgType* psMsg );
typedef void (__cdecl * vciIDLc_t_MsgHandlerExt)        ( HANDLE hDevice, VCIIDLC_MsgType* psMsg );



#include <poppack.h>

#endif  //__VCIABCCNPL_H__
