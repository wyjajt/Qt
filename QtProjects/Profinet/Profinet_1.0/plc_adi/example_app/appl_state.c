//////////////////////////////////////////////////////////////////////////
// HMS Technology Center Ravensburg GmbH
//////////////////////////////////////////////////////////////////////////
/**
  @todo

  @file appl_state.c
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
#include "abcc.h"
#include "abcc_td.h"
#include "appl_state.h"



//////////////////////////////////////////////////////////////////////////
// static constants, types, macros, variables
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// global variables
//////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------------------
** Current anybus state
**------------------------------------------------------------------------------
*/
static UINT32 appl_eAnbState = ABP_ANB_STATE_SETUP;

/*------------------------------------------------------------------------------
** Current state of the ABCC
**------------------------------------------------------------------------------
*/
static UINT32 appl_eAbccHandlerState = APPL_INIT;


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
  simple helper function to set the application state 

  @param dwAppState
    application state

  @return 
    If the function succeeds it returns VCI_OK, otherwise a value other 
    than VCI_OK.
*/
//////////////////////////////////////////////////////////////////////////
void SetAppState ( UINT32 dwState )
{
    static const char* AppStateString[ 12 ] = { "APPL_INIT",
                                                "APPL_WAITCOM",
                                                "APPL_RUN",
                                                "APPL_FATAL",
                                                "APPL_SHUTDOWN",
                                                "APPL_ABCCRESET",
                                                "APPL_DEVRESET",
                                                "APPL_HALT" };

    UINT8 appl_bAnbState = 0;

    if( appl_eAbccHandlerState != dwState )
    {
        appl_eAbccHandlerState = (appl_AbccHandlerStateType) dwState;
        ABCC_PORT_DebugPrint( ( "\n\nAPP_STATUS: %s \n" , AppStateString[ dwState ] ) );
    }
}

//////////////////////////////////////////////////////////////////////////
/**
  function to get the actual application state 

  @return 
    application state
*/
//////////////////////////////////////////////////////////////////////////
UINT32 GetAppState ( )
{
  return appl_eAbccHandlerState;
}

//////////////////////////////////////////////////////////////////////////
/**
  function to set the anybus module state ( interrupt !) 

  @param dwState
    actual anybus module state

  @return 
    If the function succeeds it returns VCI_OK, otherwise a value other 
    than VCI_OK.
*/
//////////////////////////////////////////////////////////////////////////
void SetAnbState ( UINT32 dwState )
{
  static const char* AnbStateString[ 8 ] = { "ABP_ANB_STATE_SETUP",
                                             "ABP_ANB_STATE_NW_INIT",
                                             "ABP_ANB_STATE_WAIT_PROCESS",
                                             "ABP_ANB_STATE_IDLE",
                                             "ABP_ANB_STATE_PROCESS_ACTIVE",
                                             "ABP_ANB_STATE_ERROR",
                                             "",
                                             "ABP_ANB_STATE_EXCEPTION" };

  appl_eAnbState = (ABP_AnbStateType) dwState;
  ABCC_PORT_DebugPrint( ( "ANB_STATUS: %s \n" , AnbStateString[ appl_eAnbState ] ) );
}

//////////////////////////////////////////////////////////////////////////
/**
  function to get the last stored anybus state of the anybus module

  @return 
    anybus module state
*/
//////////////////////////////////////////////////////////////////////////
UINT32 GetAnbState ( )
{
  return appl_eAnbState;
}

