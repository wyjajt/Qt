//////////////////////////////////////////////////////////////////////////
// HMS Technology Center Ravensburg GmbH
//////////////////////////////////////////////////////////////////////////
/**
  implementation

  @file abcc_setup.c
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
#include "ad_map.h"

//////////////////////////////////////////////////////////////////////////
// static constants, types, macros, variables
//////////////////////////////////////////////////////////////////////////

/*
** Invalid ADI index.
*/
#define AD_INVALID_ADI_INDEX           ( 0xffff )


//////////////////////////////////////////////////////////////////////////
// global variables
//////////////////////////////////////////////////////////////////////////

/*******************************************************************************
** Private Globals
********************************************************************************
*/

/*------------------------------------------------------------------------------
** wModuleType       - ABCC module type (read out during SETUP state)
** wNetworkType      - ABCC network type (read out during SETUP state)
** eNetFormat        - net format ( little or big endian )
** eParameterSupport - Parameter support (read out during SETUP state)
**------------------------------------------------------------------------------
*/
static UINT16               wModuleType;
static UINT16               wNetworkType;
static NetFormatType        eNetFormat;
static ParameterSupportType eParameterSupport;
static UINT8                baFirmwareVersion[4];
static char                 szNetworkString[32];

//共享寄存器初始化结果
UINT16      registerSize_Read  = -1;
UINT16      registerSize_Write = -1;

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
  initialize the global variables
*/
//////////////////////////////////////////////////////////////////////////
void ABCC_SetupInit( void )
{
  wModuleType       = 0xFFFF;
  wNetworkType      = 0xFFFF;
  eNetFormat        = NET_UNKNOWN;
  eParameterSupport = PARAMETER_UNKNOWN;
}


//////////////////////////////////////////////////////////////////////////
/**
  Retrieves the network format.
  This function will return a valid value after ABCC_CbfAdiMappingReq has been
  called by the driver. If called earlier the function will return NET_UNKNOWN.

  @return 
    Network format type (NET_LITTLEENDIAN, NET_BIGENDIAN).
*/
//////////////////////////////////////////////////////////////////////////
NetFormatType ABCC_NetFormatType( void )
{
  return eNetFormat;
}

//////////////////////////////////////////////////////////////////////////
/**
  Retrieves the network type.
  This function will return a valid value after ABCC_CbfAdiMappingReq has been
  called by the driver. If called earlier the function will return 0xFFFF which
  indicates that the network is unknown. The different network types could
  be found in abp.h.

  @return 
    Network type (0xFFFF if the network is unknown)
*/
//////////////////////////////////////////////////////////////////////////
UINT16 ABCC_NetworkType( void )
{
  return wNetworkType;
}


//////////////////////////////////////////////////////////////////////////
/**
  @todo: description  

  @param hEvent
  @param dwTimeout

  @return 
    If the function succeeds it returns VCI_OK, otherwise a value other 
    than VCI_OK.

  @note 
    Access:    public 
*/
//////////////////////////////////////////////////////////////////////////
BOOL ABCC_GatherInfo ( HANDLE hEvent, UINT32 dwTimeout )
{
  BOOL fRet = FALSE;

  ABCC_PORT_DebugPrint(( "RSP NW_IA_DATA_FORMAT:\n" ));
  fRet = GetAttribute ( ABP_OBJ_NUM_NW, 
                        1, 
                        ABP_NW_IA_DATA_FORMAT , 
                        hEvent, 
                        sizeof (UINT8),
                        (UINT8 *) &eNetFormat, 
                        NULL, 
                        dwTimeout);
  if ( fRet )
  {
    ABCC_PORT_DebugPrint(( "    Data format: 0x%02x\n", eNetFormat ));

    ABCC_PORT_DebugPrint(( "RSP NW_IA__PARAM_SUPPORT:\n" ));
    fRet = GetAttribute ( ABP_OBJ_NUM_NW, 
                          1, 
                          ABP_NW_IA_PARAM_SUPPORT, 
                          hEvent, 
                          sizeof (UINT8), 
                          (UINT8 *) &eParameterSupport, 
                          NULL, 
                          dwTimeout);
  }

  if ( fRet )
  {
    ABCC_PORT_DebugPrint(( "    Parameter data support: 0x%02x\n", eParameterSupport ));

    ABCC_PORT_DebugPrint(( "RSP ANB_IA_MODULE_TYPE:\n" ));
    fRet = GetAttribute ( ABP_OBJ_NUM_ANB, 
                          1, 
                          ABP_ANB_IA_MODULE_TYPE, 
                          hEvent, 
                          sizeof (wModuleType), 
                          (UINT8 *) &wModuleType, 
                          NULL, 
                          dwTimeout);
  }

  if ( fRet )
  {
    ABCC_PORT_DebugPrint(( "    Module type: 0x%04x\n", wModuleType ));

    ABCC_PORT_DebugPrint(( "RSP NW_IA_NW_TYPE:\n" ));
    fRet = GetAttribute ( ABP_OBJ_NUM_NW, 
                          1, 
                          ABP_NW_IA_NW_TYPE, 
                          hEvent, 
                          sizeof (wNetworkType), 
                          (UINT8 *) &wNetworkType, 
                          NULL, 
                          dwTimeout);
  }


  // neu !!
  if ( fRet )
  {
    ABCC_PORT_DebugPrint(( "    Network type: 0x%04x\n", wNetworkType ));

    ABCC_PORT_DebugPrint(( "RSP ABP_ANB_IA_FW_VERSION:\n" ));
    fRet = GetAttribute ( ABP_OBJ_NUM_ANB, 
                          1, 
                          ABP_ANB_IA_FW_VERSION, 
                          hEvent, 
                          sizeof (baFirmwareVersion), 
                          (UINT8 *) &baFirmwareVersion[0], 
                          NULL, 
                          dwTimeout);
  }

  if ( fRet )
  {
    ABCC_PORT_DebugPrint(( "    Firmware version: %02d.%02d.%02d\n", 
                          baFirmwareVersion[0], baFirmwareVersion[1], baFirmwareVersion[2]));

    ABCC_PORT_DebugPrint(( "RSP ABP_NW_IA_NW_TYPE_STR:\n" ));
    fRet = GetAttribute ( ABP_OBJ_NUM_NW, 
                          1, 
                          ABP_NW_IA_NW_TYPE_STR, 
                          hEvent, 
                          sizeof (szNetworkString), 
                          (UINT8 *) &szNetworkString, 
                          NULL, 
                          dwTimeout);
  }

  if ( fRet )
  {
    szNetworkString[31] = 0;
    ABCC_PORT_DebugPrint(( "    Network type string: %s\n", szNetworkString ));
  }
  else
  {
    ABCC_PORT_DebugPrint(( "    failed \n" ));
  }

  return fRet;
}

//////////////////////////////////////////////////////////////////////////
/**
  function to map the process data

  @param hEvent
    event handle
  @param dwTimeout
    time to wait for the event

  @return 
    If the function succeeds it returns TRUE, otherwise FALSE
*/
//////////////////////////////////////////////////////////////////////////
BOOL ABCC_MapPD (HANDLE hEvent, UINT32 dwTimeout)
{
    BOOL   fRet = FALSE;
    BOOL   fRun = FALSE;
    int    iMappingIndex = 0;
    UINT8  abData[ ABP_MAX_MSG_DATA_BYTES ];
    UINT8  bModuleId = ABCC_ReadModuleId();
    UINT16 wWritten;
    UINT8  bDirection;  //数据方向
    int    iMappedBits;
    int    iPdReadBitSize  = 0;
    int    iPdWriteBitSize = 0;
    UINT16 wRdPDSize       = 0;
    UINT16 wWrPDSize       = 0;

    ABCC_PORT_DebugPrint(( "PD Mapping \n"));

    do{
        fRun = FALSE;

        if( GetADMapping( bModuleId,
                          iMappingIndex,
                          &bDirection,
                          &iMappedBits,
                          ABP_MAX_MSG_DATA_BYTES,
                          &wWritten,
                          abData ))
        {
            ABCC_PORT_DebugPrint(( "ADI ( %d ) got \n",  iMappingIndex));

            if( PD_READ == bDirection )
            {
                iPdReadBitSize += iMappedBits;
                wRdPDSize       = ( iPdReadBitSize + 7 )/8;

                fRet = MapADIRead( hEvent, wWritten, abData, &wWritten, dwTimeout );

                if( fRet )
                {
                    // if the handling was successful execute the next mapping entry
                    iMappingIndex++;
                    fRun = TRUE;

                    ABCC_PORT_DebugPrint(( "    mapped read ADI \n"));
                }
            }
            else if( PD_WRITE == bDirection )
            {
                iPdWriteBitSize += iMappedBits;
                wWrPDSize        = ( iPdWriteBitSize + 7 )/8;

                fRet = MapADIWrite( hEvent, wWritten, abData, &wWritten, dwTimeout );
                if( fRet )
                {
                    // if the handling was successful execute the next mapping entry
                    iMappingIndex++;
                    fRun = TRUE;

                    ABCC_PORT_DebugPrint(( "    mapped write ADI \n"));
                }
            }
        }
    }while( fRun );

    if ( iMappingIndex )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

//////////////////////////////////////////////////////////////////////////
/**
  function called to finalize the setup ( move from the SETUP to the NW_INIT state)

  @param hEvent
    event handle
  @param dwTimeout
    timeout 

  @return 
    If the function succeeds it returns TRUE, otherwise FALSE
*/
//////////////////////////////////////////////////////////////////////////
BOOL ABCC_FinalizeSetup(HANDLE hEvent, UINT32 dwTimeout)
{
    BOOL   fRet           = FALSE;
    BOOL   fSetupComplete = TRUE;
    UINT16 wRdPDSize      = 0;
    UINT16 wWrPDSize      = 0;

    ABCC_PORT_DebugPrint(( "SET IA_READ_PD_SIZE: \n" ));
    fRet = GetAttribute( ABP_OBJ_NUM_NW,
                         1,
                         ABP_NW_IA_READ_PD_SIZE,
                         hEvent,
                         sizeof(wRdPDSize),
                         (UINT8 *) &wRdPDSize,
                         NULL,
                         dwTimeout );
    if( fRet )
    {
        ABCC_PORT_DebugPrint(( "    %d\n", wRdPDSize ));

        ABCC_PORT_DebugPrint(( "SET IA_WRITE_PD_SIZE:\n" ));
        fRet = GetAttribute( ABP_OBJ_NUM_NW,
                             1,
                             ABP_NW_IA_WRITE_PD_SIZE,
                             hEvent,
                             sizeof(wWrPDSize),
                             (UINT8 *) &wWrPDSize,
                             NULL,
                             dwTimeout );
    }

    if( fRet )
    {
        ABCC_PORT_DebugPrint(( "    %d\n", wWrPDSize ));

        ABCC_PORT_DebugPrint(( "SET ANB_IA_SETUP_COMPLETE: \n" ));
        fRet =  SetAttribute( ABP_OBJ_NUM_ANB,
                              1,
                              ABP_ANB_IA_SETUP_COMPLETE,
                              hEvent,
                              1,
                              (UINT8 *)&fSetupComplete,
                              NULL,
                              dwTimeout );
    }

    if( fRet )
    {
        ABCC_PORT_DebugPrint(( "    OK \n" ));

        // important initialization of the communication driver
        // otherwise the process data can not be transferred
        ABCC_SetPdSize( wRdPDSize, wWrPDSize );

        registerSize_Read  = wRdPDSize;
        registerSize_Write = wWrPDSize;
    }

    return fRet;
}
