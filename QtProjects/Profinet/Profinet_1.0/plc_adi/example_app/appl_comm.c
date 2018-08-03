//////////////////////////////////////////////////////////////////////////
// HMS Technology Center Ravensburg GmbH
//////////////////////////////////////////////////////////////////////////
/**
  implementation of the function of the exception state machine 

  @file 
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
#include "abcc.h"

#include "appl_comm.h"

#include "appl_abcc_handler.h" // unexpected error !

//////////////////////////////////////////////////////////////////////////
// static constants, types, macros, variables
//////////////////////////////////////////////////////////////////////////
#define CMD_TABLE_ENTRIES   0x100
#define CMD_TEMPSTRING_SIZE 0x100

typedef enum appl_AbccCmdState
{
  STATE_FREE,
  STATE_WAIT,
  STATE_REC,
} appl_AbccCmdStateType;

#define AT_UD 0  // undefined
#define AT_RD 1  // read
#define AT_WR 2  // write
#define AT_RW 3  // read write

typedef struct {
  UINT8  bAccess;        // undefined = 0, read 1, write 2, read write 3))
  UINT8  bState;
  HANDLE hEvent;
  ABP_MsgType* psMsg;
} ABCC_STATE ;

static ABCC_STATE sCmdTable[CMD_TABLE_ENTRIES] = { 0 };


//////////////////////////////////////////////////////////////////////////
// global variables
//////////////////////////////////////////////////////////////////////////

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
  initialize the command table 
*/
//////////////////////////////////////////////////////////////////////////
void CmdInitArray ()
{
  UINT16 wEntry = 0;

  for ( wEntry = 0; wEntry < CMD_TABLE_ENTRIES; wEntry ++)
  {
    sCmdTable[wEntry].bAccess = AT_UD;
    sCmdTable[wEntry].bState  = STATE_FREE;
    sCmdTable[wEntry].hEvent  = INVALID_HANDLE_VALUE;
    sCmdTable[wEntry].psMsg   = NULL;
  }
}

//////////////////////////////////////////////////////////////////////////
/**
  Response handler to the command set get attribute of the
  Anybus object. 

  @param psMsg
    Pointer to the received response message

  @note 
    This is function is running in the context of the receive thread. 
*/
//////////////////////////////////////////////////////////////////////////
static void MsgHandler( ABP_MsgType* psMsg )
{
  ABCC_STATE * pStore;

  if ( psMsg )
  {
    pStore = &sCmdTable[psMsg->sHeader.bSourceId];

    if ( psMsg->sHeader.bCmd & ABP_MSG_HEADER_E_BIT)
    {
      APPL_UnexpectedError();

      pStore->bState = STATE_FREE;

      if ( pStore->hEvent != INVALID_HANDLE_VALUE)
      {
        SetEvent( pStore->hEvent );
      }
    }
    else
    {
      if ( pStore->bState == STATE_WAIT )
      {
        if (( pStore->bAccess & AT_RD) == AT_RD)
        {
          ABCC_TakeMsgBufferOwnership( psMsg );
          pStore->psMsg = psMsg;
          pStore->bState = STATE_REC;

          if ( pStore->hEvent != INVALID_HANDLE_VALUE)
          {
            SetEvent( pStore->hEvent );
          }
        }
        else if (( pStore->bAccess & AT_WR) == AT_WR)
        {
          pStore->bState = STATE_FREE;

          if ( pStore->hEvent != INVALID_HANDLE_VALUE)
          {
            SetEvent( pStore->hEvent );
          }
        }
      }
    }
  }
}

//////////////////////////////////////////////////////////////////////////
/**
  function to post a command ( set, get attribute ) to the anybus module

  @param eService
    defines the access functionality
  @param bObject
    Object ( 01 == Anybus, 02 == Diagnostic, 03 == network )
  @param wInstance
    instance within object
  @param bAttribute
    attribute within instance
  @param hEvent
    event to signal
  @param pbSrcId
    ptr to store the unique src id 
  @param wSize
    size of the data to read
  @param pbData
    memory ptr to store the readen data

  @return 
    TRUE on success, otherwise FALSE
*/
//////////////////////////////////////////////////////////////////////////
BOOL PostCommand ( ABP_MsgCmdType eService,
                   UINT8          bObject,
                   UINT16         wInstance,
                   UINT8          bAttribute,
                   HANDLE         hEvent,
                   UINT8        * pbSrcId,
                   UINT16         wSize,
                   UINT8        * pbData,
                   UINT16       * pwWritten )
{
  BOOL  fRet  = FALSE;
  BOOL  fSend = FALSE;

  volatile UINT8         bSrcId = 0;
  volatile ABCC_STATE  * pEntry = NULL; 
  char szTemp[CMD_TEMPSTRING_SIZE];

  // get the unique source id
  bSrcId = ABCC_GetNewSourceId();

  if ( pbSrcId )
  {
    * pbSrcId = bSrcId;
  }

  // get a entry to the command table 
  pEntry = &sCmdTable[bSrcId];

  if ( pEntry->bState == STATE_FREE )
  {
    ABP_MsgType* psMsg = ABCC_GetCmdMsgBuffer();

    if ( psMsg)
    {
      switch ( eService )
      {
        case ABP_CMD_GET_ATTR:
          pEntry->bAccess = AT_RD;
          pEntry->hEvent  = hEvent;
          fSend= TRUE;
          break;
        case ABP_CMD_SET_ATTR:
          pEntry->bAccess = AT_WR;
          pEntry->hEvent  = hEvent;
          fSend= TRUE;
          break;
        case ABP_NW_CMD_MAP_ADI_WRITE_EXT_AREA:
          pEntry->bAccess = AT_RW;
          pEntry->hEvent  = hEvent;
          fSend= TRUE;
          break;
        case ABP_NW_CMD_MAP_ADI_READ_EXT_AREA:
          pEntry->bAccess = AT_RW;
          pEntry->hEvent  = hEvent;
          fSend= TRUE;
          break;
      }

      if ( fSend )
      {
        ABCC_SetMsgHeader ( psMsg, 
                            bObject, 
                            wInstance,
                            bAttribute, 
                            eService,
                            wSize,
                            bSrcId );
        if ( wSize )
        {
          memcpy ( psMsg->abData, pbData, wSize );
        }

        pEntry->bState = STATE_WAIT;

        if( ABCC_SendCmdMsg( psMsg, MsgHandler ) == ABCC_EC_NO_ERROR )
        {
          fRet = TRUE;
      
          if ( pwWritten )
          {
            *pwWritten = wSize;
          }
        }
        else
        {
          sprintf_s ( szTemp, CMD_TEMPSTRING_SIZE, "--- ABCC_SendCmdMsg returns error \n");

          APPL_UnexpectedError();
        }
      }
      else
      {
        sprintf_s ( szTemp, CMD_TEMPSTRING_SIZE, "--- eService %i \n", eService );
      }

    }
    else
    {
      sprintf_s ( szTemp, CMD_TEMPSTRING_SIZE, "--- ABCC_GetCmdMsgBuffer returns NULL \n" );
    }
  }
  else
  {
    sprintf_s ( szTemp, CMD_TEMPSTRING_SIZE, "--- State %i, Access %i \n", 
                        pEntry->bState, pEntry->bAccess);
  }

/*
  if ( FALSE == fRet )
  {
    printf ( szTemp );
  }
*/
  return fRet;
}

//////////////////////////////////////////////////////////////////////////
/**
  function to wait on an event generated by the executed command handler

  @param hEvent
    event to wait for
  @param dwTimeout
    time to wait

  @return 
    TRUE, if the events was raised, otherwise FALSE !

  @note 
    Access:    public 
*/
//////////////////////////////////////////////////////////////////////////
BOOL WaitOnResponse ( HANDLE hEvent, UINT32 dwTimeout )
{  
  BOOL   fRet  = FALSE;
  UINT32 dwRet = 0xFFFFFFFF;

  dwRet = WaitForSingleObject( hEvent, dwTimeout );

  switch (dwRet)
  {
    default:
    case WAIT_OBJECT_0:
      fRet = TRUE;
      break;
    case WAIT_ABANDONED_0:
      printf ( " WAIT_ABANDONED_0\n");
      break;
    case WAIT_TIMEOUT:
      printf ( " WAIT_TIMEOUT\n");
      break;
    case WAIT_FAILED:
      printf ( " WAIT_FAILED\n");
      break;
  }

  return fRet;
}

//////////////////////////////////////////////////////////////////////////
/**
  function to read the response of a query ( PostCommand ) to the Anybus 
  module 

  @param bSrcId
    unique id, which identifies the query
  @param wLength
    size of the data to read
  @param pbData
    memory ptr to store the readen data
  @param pwReaden
    ptr on a WORD variable to store the number of readen data

  @return 
    TRUE on success, otherwise FALSE
*/
//////////////////////////////////////////////////////////////////////////
BOOL GetResponseData ( UINT8 bSrcId, UINT16 wLength, UINT8 * pbData, UINT16 * pwReaden )
{
  BOOL fRet = FALSE;
  ABCC_STATE  * pEntry = &sCmdTable[bSrcId];
  ABP_MsgType * pMsg   = NULL;

  if ( STATE_REC == pEntry->bState)
  {
    pMsg    = pEntry->psMsg;
    wLength = ( wLength > pMsg->sHeader.iDataSize) ? pMsg->sHeader.iDataSize : wLength;

    if ( wLength )
    {
      memcpy ( pbData, pMsg->abData, wLength );
    }

    if ( pwReaden )
    {
      *pwReaden = wLength;
    }

    ABCC_ReturnMsgBuffer( pMsg );
    pEntry->bState = STATE_FREE;
    fRet = TRUE;
  }

  return fRet;
}

//////////////////////////////////////////////////////////////////////////
/**
  function to read the response of a query ( PostCommand ) to the Anybus 
  module 

  @param bSrcId
    unique id, which identifies the query
  @param pMsg
    ptr to store the message contents !

  @return 
    TRUE on success, otherwise FALSE
*/
//////////////////////////////////////////////////////////////////////////
BOOL GetResponseMsg ( UINT8 bSrcId, ABP_MsgType * pMsg )
{
  BOOL fRet = FALSE;
  ABCC_STATE  * pEntry = &sCmdTable[bSrcId];

  if ( pMsg )
  {
    if ( STATE_REC == pEntry->bState)
    {
      memcpy ( pMsg, pEntry->psMsg, sizeof ( ABP_MsgType));
      ABCC_ReturnMsgBuffer( pEntry->psMsg );
      pEntry->bState = STATE_FREE;
      fRet = TRUE;
    }
  }

  return fRet;
}



//////////////////////////////////////////////////////////////////////////
/**
  function to set the content of an Anybus attribute

  @param bObject
    Object ( 01 == Anybus, 02 == Diagnostic, 03 == network )
  @param wInstance
    instance within object
  @param bAttribute
    attribute within instance
  @param hEvent
    event to signal
  @param wWrite
    size of the data to write
  @param pbData
    memory ptr where the data to write is stored
  @param pwWritten
    ptr on a WORD variable to store the number of written data
  @param dwTimeout
    time to wait for a response

  @return 
    TRUE on success, otherwise FALSE
*/
//////////////////////////////////////////////////////////////////////////
BOOL SetAttribute ( UINT8    bObject,
                    UINT16   wInstance,
                    UINT8    bAttribute,
                    HANDLE   hEvent,
                    UINT16   wWrite,
                    UINT8  * pbData,
                    UINT16 * pwWritten,
                    UINT32   dwTimeout)
{
  BOOL fRet = FALSE;
  UINT8 bSrcId = 0;

  if ( PostCommand ( ABP_CMD_SET_ATTR, 
                     bObject, 
                     wInstance, 
                     bAttribute, 
                     hEvent, 
                     &bSrcId, 
                     wWrite, 
                     pbData,
                     pwWritten ))
  {
    if ( hEvent != INVALID_HANDLE_VALUE)
    {
      if ( WaitOnResponse( hEvent, dwTimeout ))
      {
        fRet = TRUE;
      }
    }
    else
    {
      fRet = TRUE;
    }
  }

  return fRet;
}

//////////////////////////////////////////////////////////////////////////
/**
  function to get the content of an Anybus attribute

  @param bObject
    Object ( 01 == Anybus, 02 == Diagnostic, 03 == network )
  @param wInstance
    instance within object
  @param bAttribute
    attribute within instance
  @param hEvent
    event to signal
  @param wSize
    size of the data to read
  @param pbData
    memory ptr to store the readen data
  @param pwReaden
    ptr on a WORD variable to store the number of readen data
  @param dwTimeout
    time to wait for a response

  @return 
    TRUE on success, otherwise FALSE
*/
//////////////////////////////////////////////////////////////////////////
BOOL GetAttribute ( UINT8    bObject,
                    UINT16   wInstance,
                    UINT8    bAttribute,
                    HANDLE   hEvent,
                    UINT16   wSize,
                    UINT8  * pbData,
                    UINT16 * pwReaden,
                    UINT32   dwTimeout)
{
  BOOL fRet = FALSE;
  UINT8 bSrcId = 0;
  char  szTemp[CMD_TEMPSTRING_SIZE] = ""; 

  sprintf_s ( szTemp, CMD_TEMPSTRING_SIZE, "PostCommand");
  if ( PostCommand ( ABP_CMD_GET_ATTR, 
                     bObject, 
                     wInstance, 
                     bAttribute, 
                     hEvent, 
                     &bSrcId, 
                     0, 
                     NULL,
                     NULL ))
  {
    if ( hEvent != INVALID_HANDLE_VALUE)
    {
      sprintf_s ( szTemp, CMD_TEMPSTRING_SIZE, "WaitOnResponse");
      if ( WaitOnResponse( hEvent, dwTimeout ))
      {
        sprintf_s ( szTemp, CMD_TEMPSTRING_SIZE, "GetResponseData");
        if ( GetResponseData( bSrcId, wSize, pbData, pwReaden) )
        {
          fRet = TRUE;
        }
      }
    }
    else
    {
      fRet = TRUE;
    }
  }
   
/*
  if ( FALSE == fRet)
  {
    printf ( "GetAttribute failed ( %s ) \n", szTemp);
  }
*/
  return fRet;
}

//////////////////////////////////////////////////////////////////////////
/**
  function to map a write ADI entry

  @param hEvent
    event to signal
  @param wWrite
    size of the ADI Info
  @param pbWriteData
    memory ptr where the data to write is stored
  @param pwWritten
    ptr on a WORD variable to store the number of written data
  @param wRead
    size of the ADI Info response
  @param pbReadData
    memory ptr where the ADI Infos response is stored
  @param pwWritten
    ptr on a WORD variable to store the number of written data
  @param dwTimeout
    time to wait for a response

  @return 
    TRUE on success, otherwise FALSE
*/
//////////////////////////////////////////////////////////////////////////
BOOL MapADIWrite ( HANDLE           hEvent,
                   UINT16           wWrite,
                   UINT8          * pbWriteData,
                   UINT16         * pwWritten,
                   UINT32           dwTimeout)
{
  BOOL fRet = FALSE;
  UINT8 bSrcId = 0;
  ABP_MsgType sResponse;

  if ( PostCommand ( ABP_NW_CMD_MAP_ADI_WRITE_EXT_AREA, 
                     ABP_OBJ_NUM_NW, 
                     1,  // Instance 1
                     1,  // number of ADIs to map
                     hEvent, 
                     &bSrcId,
                     wWrite, 
                     pbWriteData,
                     pwWritten ))
  {
    if ( hEvent != INVALID_HANDLE_VALUE)
    {
      if ( WaitOnResponse( hEvent, dwTimeout ))
      {
        if ( GetResponseMsg( bSrcId, &sResponse) )
        {
          fRet = TRUE;
        }
      }
    }
    else
    {
      fRet = TRUE;
    }
  }

  return fRet;
}

//////////////////////////////////////////////////////////////////////////
/**
  function to map a read ADI entry

  @param hEvent
    event to signal
  @param wWrite
    size of the ADI Info
  @param pbWriteData
    memory ptr where the data to write is stored
  @param pwWritten
    ptr on a WORD variable to store the number of written data
  @param wRead
    size of the ADI Info response
  @param pbReadData
    memory ptr where the ADI Infos response is stored
  @param pwWritten
    ptr on a WORD variable to store the number of written data
  @param dwTimeout
    time to wait for a response

  @return 
    TRUE on success, otherwise FALSE
*/
//////////////////////////////////////////////////////////////////////////
BOOL MapADIRead  ( HANDLE           hEvent,
                   UINT16           wWrite,
                   UINT8          * pbWriteData,
                   UINT16         * pwWritten,
                   UINT32           dwTimeout)
{
  BOOL fRet = FALSE;
  UINT8 bSrcId = 0;
  ABP_MsgType sResponse;

  if ( PostCommand ( ABP_NW_CMD_MAP_ADI_READ_EXT_AREA, 
                     ABP_OBJ_NUM_NW, 
                     1,  // Instance 1
                     1,  // number of ADIs to map
                     hEvent, 
                     &bSrcId,
                     wWrite, 
                     pbWriteData,
                     pwWritten ))
  {
    if ( hEvent != INVALID_HANDLE_VALUE)
    {
      if ( WaitOnResponse( hEvent, dwTimeout ))
      {
        if ( GetResponseMsg( bSrcId, &sResponse) )
        {
          fRet = TRUE;
        }
      }
    }
    else
    {
      fRet = TRUE;
    }
  }

  return fRet;
}


