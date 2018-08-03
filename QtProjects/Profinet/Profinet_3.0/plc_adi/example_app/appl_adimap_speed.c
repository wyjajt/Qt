/*******************************************************************************
********************************************************************************
**                                                                            **
** ABCC Starter Kit version 1.01.01 (2015-07-07)                              **
**                                                                            **
** Delivered with:                                                            **
**    ABCC Driver 3.01.01 (2015-07-07)                                        **
**    ABP         7.12.01 (2015-06-25)                                        **
**                                                                            */
/*******************************************************************************
********************************************************************************
** COPYRIGHT NOTIFICATION (c) 2015 HMS Industrial Networks AB                 **
**                                                                            **
** This code is the property of HMS Industrial Networks AB.                   **
** The source code may not be reproduced, distributed, or used without        **
** permission. When used together with a product from HMS, permission is      **
** granted to modify, reproduce and distribute the code in binary form        **
** without any restrictions.                                                  **
**                                                                            **
** THE CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND. HMS DOES NOT    **
** WARRANT THAT THE FUNCTIONS OF THE CODE WILL MEET YOUR REQUIREMENTS, OR     **
** THAT THE OPERATION OF THE CODE WILL BE UNINTERRUPTED OR ERROR-FREE, OR     **
** THAT DEFECTS IN IT CAN BE CORRECTED.                                       **
********************************************************************************
********************************************************************************
** Example of an ADI setup with an array of 16 bit values
**
** In abcc_drv_cfg.h make sure that the following definitions are set to:
** ABCC_CFG_STRUCT_DATA_TYPE     ( FALSE )
** ABCC_CFG_ADI_GET_SET_CALLBACK ( FALSE )
********************************************************************************
********************************************************************************
*/
 #include <stdio.h>
#include "abcc.h"

#include "appl_adi_config.h"
#include "appl_adimap.h"


#if ( APPL_ACTIVE_ADI_SETUP == APPL_ADI_SETUP_SPEED )

#if( !ABCC_CFG_STRUCT_DATA_TYPE || !ABCC_CFG_ADI_GET_SET_CALLBACK  )
  #error ABCC_CFG_ADI_GET_SET_CALLBACK must be set to TRUE and ABCC_CFG_STRUCT_DATA_TYPE set to TRUE in order to run this example
#endif

/*******************************************************************************
** Constants
********************************************************************************
*/
#define ADI_MINIMUM_SPEED        -500
#define ADI_MAXIMUM_SPEED         500
#define ADI_MINIMUM_TEMPERATURE   -40
#define ADI_MAXIMUM_TEMPERATURE   125

/*******************************************************************************
** Typedefs
********************************************************************************
*/

/*******************************************************************************
** Private Globals
********************************************************************************
*/
//extern IDL_CTRL_HDL hCtrl;

/*------------------------------------------------------------------------------
** Data holder for the ADI instances
**------------------------------------------------------------------------------
*/
UINT16   iADI_Speed;
INT16    iADI_Temperature;
UINT16   iADI_RefSpeed;
BOOL8    fADI_DirectionIsForward;
UINT8    bADI_TripCurrent;

/*------------------------------------------------------------------------------
** Min, max and default value for appl_aiUint16
**------------------------------------------------------------------------------
*/
static AD_SINT16Type  sADI_SpeedProps              = {{ ADI_MINIMUM_SPEED, ADI_MAXIMUM_SPEED, 10 }};
static AD_SINT16Type  sADI_TemperatureProps        = {{ ADI_MINIMUM_TEMPERATURE, ADI_MAXIMUM_TEMPERATURE, 25 }};
static AD_UINT16Type  sADI_RefSpeedProps           = {{ 0, 500, 10 }};
static AD_BOOL8Type   sADI_DirectionIsForwardProps = {{ FALSE, TRUE, TRUE }};
static AD_UINT8Type   sADI_TripCurrentProps        = {{ 0, 6, 4 }};

/*******************************************************************************
** Public Globals
********************************************************************************
*/

/*------------------------------------------------------------------------------
** ADI
**------------------------------------------------------------------------------
*/
const AD_AdiEntryType APPL_asAdiEntryList[] =
{
  /* Index: 0 */{ 1, "Speed",               ABP_SINT16, 1, APPL_WRITE_MAP_READ_ACCESS_DESC,   { { &iADI_Speed,              &sADI_SpeedProps } },              NULL, NULL, NULL },
  /* Index: 1 */{ 2, "Ref speed",           ABP_UINT16, 1, APPL_READ_MAP_WRITE_ACCESS_DESC,   { { &iADI_RefSpeed,           &sADI_RefSpeedProps } },           NULL, NULL, NULL },
  /* Index: 2 */{ 3, "Direction = Forward", ABP_BOOL,   1, APPL_READ_MAP_WRITE_ACCESS_DESC,   { { &fADI_DirectionIsForward, &sADI_DirectionIsForwardProps } }, NULL, NULL, NULL },
  /* Index: 3 */{ 4, "Temperature",         ABP_SINT16, 1, APPL_WRITE_MAP_READ_ACCESS_DESC,   { { &iADI_Temperature,        &sADI_TemperatureProps        } }, NULL, NULL, NULL },
  /* Index: 4 */{ 5, "Trip Current",        ABP_UINT8,  1, APPL_READ_MAP_WRITE_ACCESS_DESC,   { { &bADI_TripCurrent,        &sADI_TripCurrentProps        } }, NULL, NULL, NULL }
};

/*------------------------------------------------------------------------------
** Map all adi:s in both directions
**------------------------------------------------------------------------------
** 1. AD instance | 2. Direction | 3. Num elements | 4. Start index |
**------------------------------------------------------------------------------
*/
const AD_DefaultMapType APPL_asAdObjDefaultMap[] =
{
    { 1, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 }, // PLC input
    { 2, PD_READ , AD_DEFAULT_MAP_ALL_ELEM, 0 }, // PLC output
    { 3, PD_READ , AD_DEFAULT_MAP_ALL_ELEM, 0 }, // PLC output
    { 4, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 }, // PLC input
    { 5, PD_READ , AD_DEFAULT_MAP_ALL_ELEM, 0 }, // PLC output
    { AD_DEFAULT_MAP_END_ENTRY }
};

/*******************************************************************************
** Private Services
********************************************************************************
*/

/*------------------------------------------------------------------------------
** called if a key was entered
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    TRUE if a CR was detected, otherwise FALSE
**------------------------------------------------------------------------------
*/
BOOL8 APPL_HandleUserInput ( UINT8 bStrLen, char * abUserInput )
{
  BOOL8 fRet = FALSE ;
  INT32 lNewTemperature = 0;

  char  cLast = 0;

  if (bStrLen)
  {
    cLast = abUserInput[bStrLen - 1];
    if ((cLast == '\r') || (cLast == '\n'))
    {
      sscanf(abUserInput, "%d", &lNewTemperature);

      if (lNewTemperature > ADI_MAXIMUM_TEMPERATURE)
      {
        iADI_Temperature = ADI_MAXIMUM_TEMPERATURE;
      }
      else if (lNewTemperature < ADI_MINIMUM_TEMPERATURE)
      {
        iADI_Temperature = ADI_MINIMUM_TEMPERATURE;
      }
      else
      {
        iADI_Temperature = (INT16)lNewTemperature;
      }

      printf("\n ------ Temperature: %4d ( min: %4d, max: %4d ) ------ \n"  , 
              iADI_Temperature,
              ADI_MINIMUM_TEMPERATURE,
              ADI_MAXIMUM_TEMPERATURE);
      fRet = TRUE ;
    }
  }

  return fRet;
}

/*------------------------------------------------------------------------------
** function to show the ADI contents
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    TRUE
**------------------------------------------------------------------------------
*/
BOOL8 APPL_ShowADIs ( )
{
  BOOL8 fRet = FALSE ;

  static INT8   iAngle = 0;
  const  char   abRotor[] = "|/-\\";

  printf("\rMotor: %c"          , abRotor[iAngle]);
  printf("\tSpeed  : %4hd rpm, ", iADI_Speed);
  printf("\tPoles  : %4d, "     , bADI_TripCurrent);
  printf("\tTemperature: %4d "  , iADI_Temperature);

  if (fADI_DirectionIsForward)
  {
    iAngle = (iAngle + 1) & 0x3;
  }
  else
  {
    iAngle = (iAngle - 1) & 0x3;
  }

  fRet = TRUE ;

  return fRet;
}

/*------------------------------------------------------------------------------
** function to trigger the update of the ADIs
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    TRUE if the update was successful, otherwise FALSE
**------------------------------------------------------------------------------
*/
BOOL APPL_UpdateADIs ( )
{
  BOOL8 fRet = FALSE ;

  if ((ABCC_AnbState() == ABP_ANB_STATE_PROCESS_ACTIVE))
  {
    if (fADI_DirectionIsForward)
    {
      if (iADI_Speed != iADI_RefSpeed)
      {
        /*
        ** Speed has changed.
        */
        iADI_Speed = iADI_RefSpeed;
        ABCC_TriggerWrPdUpdate();
        fRet = TRUE ;
      }
    }
    else
    {
      if (iADI_Speed != -iADI_RefSpeed)
      {
        /*
        ** Speed has changed.
        */
        iADI_Speed = -iADI_RefSpeed;
        ABCC_TriggerWrPdUpdate();
        fRet = TRUE ;
      }
    }
  }

  return fRet;
}


/*******************************************************************************
** Public Services
********************************************************************************
*/

UINT16 APPL_GetNumAdi( void )
{
   return( sizeof( APPL_asAdiEntryList ) / sizeof( AD_AdiEntryType ) );
}

/*******************************************************************************
** Tasks
********************************************************************************
*/

#endif
