//////////////////////////////////////////////////////////////////////////
// HMS Technology Center Ravensburg GmbH
//////////////////////////////////////////////////////////////////////////
/**
  implementation of the function of the user init state machine 

  @file appl_userinit.c
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
#include "appl_userinit.h"
#include "appl_state.h"

#include "appl_abcc_handler.h" // unexpected error !
//#include "abcc_setup.h"

#include "appl_comm.h"

//////////////////////////////////////////////////////////////////////////
// static constants, types, macros, variables
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// global variables
//////////////////////////////////////////////////////////////////////////
//static BOOL appl_fUserInitWaitForResp = FALSE;

/*------------------------------------------------------------------------------
** Network IP address.
**------------------------------------------------------------------------------
*/
static UINT8 appl_abNwIpAddress[ 4 ] = { 192, 168, 0, 0 };
static UINT8 appl_bLastSetNwIpAddress;

/*------------------------------------------------------------------------------
** Network Node address.
**------------------------------------------------------------------------------
*/
static UINT8 appl_bNwNodeAddress;
static UINT8 appl_bLastSetNwNodeAddress;

/*------------------------------------------------------------------------------
** Common address variables (used for both node address and IP address depending
** on network type).
**------------------------------------------------------------------------------
*/
static BOOL appl_fSetAddr = FALSE;

/*------------------------------------------------------------------------------
** Network baud rate.
**------------------------------------------------------------------------------
*/
static UINT8 appl_bNwBaudRate;
static UINT8 appl_bLastSetNwBaudRate;
static BOOL  appl_fSetBaudRate = FALSE;

/*------------------------------------------------------------------------------
** Flags to keep track of if the network type supports node ID/IP address
** and/or baud rate to be set.
**------------------------------------------------------------------------------
*/
static BOOL appl_fNwSupportsNodeId;
static BOOL appl_fNwSupportsBaudRate;


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
  Use this function to set a value to NC instance 1 (Switch 1) or 2 (Switch 2) 

  @param iInstance
    NC instance to set (ABP_NC_INST_NUM_SW1 or ABP_NC_INST_NUM_SW2).
  @param bValue
    Value to be set.
  @param pnMsgHandler
    Pointer to the function to handle the response message.

  @return 
    TRUE  - Set command was successfully sent.
    FALSE - Failed to send command.
*/
//////////////////////////////////////////////////////////////////////////
static BOOL UserInitNodeID ( HANDLE hEvent, UINT8 bNwNodeAddress, UINT32 dwTimeout )
{
//  return SetSw1Sw2NcInstance ( ABP_NC_INST_NUM_SW1, appl_bNwNodeAddress, hEvent);
  return SetAttribute( ABP_OBJ_NUM_NC, 
                       ABP_NC_INST_NUM_SW1, 
                       ABP_NC_VAR_IA_VALUE,
                       hEvent,
                       sizeof ( bNwNodeAddress),
                       &bNwNodeAddress,
                       NULL,
                       dwTimeout );
}

//////////////////////////////////////////////////////////////////////////
/**
  Use this function to set a value to NC instance 1 (Switch 1) or 2 (Switch 2) 

  @param iInstance
    NC instance to set (ABP_NC_INST_NUM_SW1 or ABP_NC_INST_NUM_SW2).
  @param bValue
    Value to be set.
  @param pnMsgHandler
    Pointer to the function to handle the response message.

  @return 
    TRUE  - Set command was successfully sent.
    FALSE - Failed to send command.
*/
//////////////////////////////////////////////////////////////////////////
static BOOL UserInitBaudrate ( HANDLE hEvent, UINT8 bBaudrate, UINT32 dwTimeout )
{
//  return SetSw1Sw2NcInstance ( ABP_NC_INST_NUM_SW2, appl_bNwBaudRate, hEvent);
  return SetAttribute( ABP_OBJ_NUM_NC, 
                       ABP_NC_INST_NUM_SW2, 
                       ABP_NC_VAR_IA_VALUE,
                       hEvent,
                       sizeof ( bBaudrate),
                       &bBaudrate,
                       NULL,
                       dwTimeout );
}

//////////////////////////////////////////////////////////////////////////
/**
  Use this function to set the value stored in appl_abNwIpAddress to the IP
  address NC instance (instance 3).

  @param pnMsgHandler
    Pointer to the function to handle the response message.

  @return 
    TRUE  - Set command was successfully sent.
    FALSE - Failed to send command.
*/
//////////////////////////////////////////////////////////////////////////
static BOOL UserInitNodeIPAddr ( HANDLE hEvent, UINT32 dwTimeout )
{
  return SetAttribute( ABP_OBJ_NUM_NC, 
                       3, 
                       ABP_NC_VAR_IA_VALUE,
                       hEvent,
                       4,
                       appl_abNwIpAddress,
                       NULL,
                       dwTimeout);
}

//////////////////////////////////////////////////////////////////////////
/**
  Checks if the IP address/node address and/or baud rate has changes
  and sets the updated values to the corresponding network configuration
  instances if it has.
*/
//////////////////////////////////////////////////////////////////////////
void UpdateSwitchValues( HANDLE hEvent, UINT32 dwTimeout )
{
   /*
   ** Check if the IP address/node address has been changed and take appropriate
   ** action depending on network type.
   */
   if( ( appl_bLastSetNwIpAddress != appl_abNwIpAddress[ 3 ] ) &&
       ( appl_abNwIpAddress[ 3 ] != 0 ) &&
       ( !appl_fNwSupportsNodeId ) &&
       ( appl_fSetAddr ) )
   {
      if( UserInitNodeIPAddr( hEvent, dwTimeout ) )
      {
         appl_bLastSetNwIpAddress = appl_abNwIpAddress[ 3 ];
      }
   }
   else if( ( appl_bLastSetNwNodeAddress != appl_bNwNodeAddress ) &&
            ( appl_fNwSupportsNodeId ) &&
            ( appl_fSetAddr ) )
   {
      if( UserInitNodeID ( hEvent, appl_bNwNodeAddress, dwTimeout ))
      {
         appl_bLastSetNwNodeAddress = appl_bNwNodeAddress;
      }
   }

   /*
   ** Check if baud rate has changed and if a baud rate can be set for the
   ** current network type.
   */
   if( ( appl_bLastSetNwBaudRate != appl_bNwBaudRate ) &&
       ( appl_fNwSupportsBaudRate ) &&
       ( appl_fSetBaudRate ) )
   {
      if ( UserInitBaudrate ( hEvent, appl_bNwBaudRate, dwTimeout ))
      {
         appl_bLastSetNwBaudRate = appl_bNwBaudRate;
      }
   }
}

//////////////////////////////////////////////////////////////////////////
/**
  set the Network Support Node Id flag

  @param fVal
    value
*/
//////////////////////////////////////////////////////////////////////////
void setNwSupportsNodeId ( BOOL fVal )
{
  appl_fNwSupportsNodeId = fVal;
}

//////////////////////////////////////////////////////////////////////////
/**
  set the baudrate flag

  @param fVal
    value
*/
//////////////////////////////////////////////////////////////////////////
void setNwSupportsBaudRate ( BOOL fVal )
{
  appl_fNwSupportsBaudRate = fVal;
}

//////////////////////////////////////////////////////////////////////////
/**
  set the network address 

  @param bSwitchValue
    value
*/
//////////////////////////////////////////////////////////////////////////
void setAddress ( UINT8 bSwitchValue )
{
  /*
  ** HW switch 1 will the last octet in the IP address
  ** for applicable networks ( 192.168.0.X )
  */
  appl_abNwIpAddress[ 3 ] = bSwitchValue;
  appl_fSetAddr = TRUE;

  /*
  ** Switch 1 is node address for applicable networks
  */
  appl_bNwNodeAddress = bSwitchValue;
}

//////////////////////////////////////////////////////////////////////////
/**
  set the baudrate

  @param bSwitchValue
    baudrate swich value
*/
//////////////////////////////////////////////////////////////////////////
void setBaudrate( UINT8 bSwitchValue )
{
  appl_bNwBaudRate  = bSwitchValue;
  appl_fSetBaudRate = TRUE;
}