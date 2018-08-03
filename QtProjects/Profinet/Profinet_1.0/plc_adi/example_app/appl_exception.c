//////////////////////////////////////////////////////////////////////////
// HMS Technology Center Ravensburg GmbH
//////////////////////////////////////////////////////////////////////////
/**
  definition of functions to read exception informations

  @file appl_exception.c
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
#include "appl_comm.h"
#include "appl_exception.h"

//////////////////////////////////////////////////////////////////////////
// static constants, types, macros, variables
//////////////////////////////////////////////////////////////////////////

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

#define EXCEPTION_NUM 14
//////////////////////////////////////////////////////////////////////////
/**
  small helper function to get the interpreted exception information

  @param bException
    exception code

  @return 
    interpreted exception string
*/
//////////////////////////////////////////////////////////////////////////
const char * GetExceptionString ( UINT8 bException )
{
  static const char* ExceptionString[ EXCEPTION_NUM ] = { "No exception",
                                                         "Application timeout",
                                                         "Invalid device address",
                                                         "Invalid communication settings",
                                                         "Major unrecoverable app event",
                                                         "Wait for reset",
                                                         "Invalid process data config",
                                                         "Invalid application response",
                                                         "Nonvolatile memory checksum error",
                                                         "ASM communication error",
                                                         "Insufficient application implementation",
                                                         "Missing serial number",
                                                         "Corrupt file system",
                                                         "unknown exception"  };

  bException = (bException < EXCEPTION_NUM) ? bException : EXCEPTION_NUM - 1;
  return ExceptionString[ bException ];
}

//////////////////////////////////////////////////////////////////////////
/**
  function to show the exception ( readen directly out of the anybus module )

  @param hEvent
    Event to wait for
  @param dwTimeout
    timeout !!
*/
//////////////////////////////////////////////////////////////////////////
void ExceptionShow ( HANDLE hEvent, UINT32 dwTimeout )
{
  UINT8 bException;

  if ( GetAttribute ( ABP_OBJ_NUM_ANB, 
                      1, 
                      ABP_ANB_IA_EXCEPTION, 
                      hEvent, 
                      sizeof (bException),
                      &bException, 
                      NULL, 
                      dwTimeout))
  {
    ABCC_PORT_DebugPrint(( "Exception Code:    %X %s\n", 
                            bException, 
                            GetExceptionString(bException)));
  }
}

//////////////////////////////////////////////////////////////////////////
/**
  function to show the exception info ( readen directly out of the anybus module )

  @param hEvent
    Event to wait for
  @param dwTimeout
    timeout !!
*/
//////////////////////////////////////////////////////////////////////////
void ExceptionInfoShow ( HANDLE hEvent, UINT32 dwTimeout )
{
  UINT8 bExceptionInfo;

  if ( GetAttribute ( ABP_OBJ_NUM_NW, 
                      1, 
                      ABP_NW_IA_EXCEPTION_INFO, 
                      hEvent, 
                      sizeof (bExceptionInfo),
                      &bExceptionInfo, 
                      NULL, 
                      dwTimeout))
  {
    ABCC_PORT_DebugPrint(( "Exception Info Code:    %X\n", bExceptionInfo));
  }
}

//////////////////////////////////////////////////////////////////////////
/**
  function to show the LED info ( readen directly out of the anybus module )

  @param hEvent
    Event to wait for
  @param dwTimeout
    timeout !!

  @return 
    If the function succeeds it returns VCI_OK, otherwise a value other 
    than VCI_OK.

  @note 
    Access:    public 
*/
//////////////////////////////////////////////////////////////////////////
void LEDInfoShow( HANDLE hEvent, UINT32 dwTimeout )
{
  UINT8 bStatus;
  UINT8 bValue[4];

  if ( GetAttribute ( ABP_OBJ_NUM_ANB, 
                      1, 
                      ABP_ANB_IA_LED_STATUS, 
                      hEvent, 
                      sizeof (bStatus),
                      &bStatus, 
                      NULL, 
                      dwTimeout))
  {
    ABCC_PORT_DebugPrint(( "LED Status    :    %X\n", bStatus));
  }

  if ( GetAttribute ( ABP_OBJ_NUM_ANB, 
                      1, 
                      ABP_ANB_IA_LED_COLOURS, 
                      hEvent, 
                      sizeof (bValue),
                      (UINT8 *) &bValue, 
                      NULL, 
                      dwTimeout))
  {
    ABCC_PORT_DebugPrint(( "LED Colour    :    %X %X %X %X\n", bValue[0], bValue[1], bValue[2], bValue[3]));
  }
}

//////////////////////////////////////////////////////////////////////////
/**
  function to show the Error Counter 
  ( readen directly out of the anybus module )

  @param hEvent
  Event to wait for
  @param dwTimeout
  timeout !!

  @return 
    If the function succeeds it returns VCI_OK, otherwise a value other 
    than VCI_OK.

  @note 
    Access:    public 
*/
//////////////////////////////////////////////////////////////////////////
void ErrorCntShow ( HANDLE hEvent, UINT32 dwTimeout )
{
  UINT16 wValue[4];

  if ( GetAttribute ( ABP_OBJ_NUM_ANB, 
                      1, 
                      ABP_ANB_IA_ERROR_CNTRS, 
                      hEvent, 
                      sizeof (wValue),
                      (UINT8 *) &wValue, 
                      NULL, 
                      dwTimeout))
  {
    ABCC_PORT_DebugPrint(( "Error Cnt:\n  DC %X\n  DR %X\n  SE %X\n  FE %X\n", wValue[0], wValue[1], wValue[2], wValue[3]));
  }
}

//////////////////////////////////////////////////////////////////////////
/**
  function to show and reset a fatal error condition

  @param hEvent
    Event to wait for
  @param dwTimeout
    timeout !!

  @return 
    If the function succeeds it returns VCI_OK, otherwise a value other 
    than VCI_OK.

  @note 
    Access:    public 
*/
//////////////////////////////////////////////////////////////////////////
void FatalEventShow ( HANDLE hEvent, UINT32 dwTimeout )
{
  UINT8 baFatal[0x100];
  UINT16 wReaden;

  ABCC_PORT_DebugPrint(( "Fatal Event:       \n"));

  if ( GetAttribute ( ABP_OBJ_NUM_ANB, 
                      1, 
                      ABP_ANB_IA_FATAL_EVENT, 
                      hEvent, 
                      sizeof (baFatal),
                      baFatal, 
                      &wReaden, 
                      dwTimeout))
  {
    const int iCols = 8;
    int iPos = 0;
    int iPosX = 0;
    int iPosY = 0;
    char szText[0x20] = "";
    char szVal [0x20] = "";

    while ( iPos < 42)
    {
      if ( iPosX < 8)
      {
        sprintf( szVal, " %02x", baFatal[iPos]);
        strcat ( szText, szVal );
        iPosX ++;
      }
      else
      {
        ABCC_PORT_DebugPrint(("  %s\n", szText ));
        szText[0] = 0;
        iPosY ++;
        iPosX = 0;
      }

      iPos = (iPosY *iCols) +iPosX;
    }

    ABCC_PORT_DebugPrint(("  %s\n", szText ));
  }
}

//////////////////////////////////////////////////////////////////////////
/**
  function to check if a information is available in the fatal event attribute

  @param hEvent
    event handle 
  @param pfFatal
    ptr on a BOOL to show if a Fatal Events is stored
  @param dwTimeout
    time to wait for the event !

  @return 
    If the function succeeds it returns TRUE, otherwise FALSE
*/
//////////////////////////////////////////////////////////////////////////
BOOL FatalEventCheck ( HANDLE hEvent, BOOL * pfFatal, UINT32 dwTimeout )
{
  BOOL  fRet = FALSE;
  UINT8 baFatal[0x100];
  UINT16 wReaden;

  if ( pfFatal )
  {
    fRet = GetAttribute ( ABP_OBJ_NUM_ANB, 
                          1, 
                          ABP_ANB_IA_FATAL_EVENT, 
                          hEvent, 
                          sizeof (baFatal),
                          baFatal, 
                          &wReaden, 
                          dwTimeout);
    if ( TRUE == fRet ) // no Error or timeout occured !
    {
      UINT8 baFatalTemp[0x100];
      memset ( baFatalTemp, 0, wReaden);

      if ( 0 != memcmp ( baFatal, baFatalTemp, wReaden))
      {
        *pfFatal = TRUE;
      }
      else
      {
        *pfFatal = FALSE;
      }
    }
  }

  return fRet;
}

//////////////////////////////////////////////////////////////////////////
/**
  function to reset a fatal event

  @param hEvent
    event handle
  @param dwTimeout
    time to wait for the event !
*/
//////////////////////////////////////////////////////////////////////////
void FatalEventReset ( HANDLE hEvent, UINT32 dwTimeout )
{
  ABCC_PORT_DebugPrint(( "Fatal Event Reset \n"));
  if ( SetAttribute ( ABP_OBJ_NUM_ANB, 
                      1, 
                      ABP_ANB_IA_FATAL_EVENT, 
                      hEvent, 
                      0,
                      NULL, 
                      NULL, 
                      dwTimeout))
  {
    ABCC_PORT_DebugPrint(( "  OK \n"));
  }
  else
  {
    ABCC_PORT_DebugPrint(( "  failed \n"));
  }
}

//////////////////////////////////////////////////////////////////////////
/**
  function to collect some collection readen from the anybus module and to
  print them out

  @param hEvent
    Event to wait for
  @param dwTimeout
    timeout !!
*/
//////////////////////////////////////////////////////////////////////////
void ShowInfos ( HANDLE hEvent, UINT32 dwTimeout )
{
  ExceptionShow    ( hEvent, dwTimeout);
  ExceptionInfoShow( hEvent, dwTimeout);
  LEDInfoShow      ( hEvent, dwTimeout);
  ErrorCntShow     ( hEvent, dwTimeout);
  FatalEventShow   ( hEvent, dwTimeout);
}

