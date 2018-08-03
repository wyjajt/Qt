/*******************************************************************************
********************************************************************************
**                                                                            **
** ABCC Starter Kit version 2.01.01 (2015-12-14)                              **
**                                                                            **
** Delivered with:                                                            **
**    ABCC Driver 4.01.01 (2015-12-14)                                        **
**    ABP         7.16.01 (2015-10-14)                                        **
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

#include "appl_adi_config.h"

#if ( APPL_ACTIVE_ADI_SETUP == APPL_ADI_SETUP_SIMPLE_16 )

/*******************************************************************************
** Constants
********************************************************************************
*/

/*******************************************************************************
** Typedefs
********************************************************************************
*/

/*******************************************************************************
** Private Globals
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Data holder for the ADI instances
**------------------------------------------------------------------------------
*/

UINT32 Read_InPlace;            //D1,  动作, 0默认, 1开始, 2复检
UINT32 Read_Pos_X;              //D2,  int
UINT32 Read_Pos_Y;              //D3,  int
UINT32 Read_HeartBeat;          //D4,  读心跳
UINT32 Read_MoveOver;			//D5,  运动完成, 0默认, 1生效
UINT32 Read_JudgeLight;			//D6,  判断是否有光, 0默认, 1生效
UINT32 Read_ClearData;			//D9,  清零, 0默认, 1生效

UINT32 Write_InPlace;           //D10, 就位, 0默认, 1开始
UINT32 Write_DetectResult;      //D11, 第一次结果, 0默认, 1有效
UINT32 Write_Pos_X;             //D12, int
UINT32 Write_Pos_Y;             //D13, int
UINT32 Write_ReDetectResult;    //D14, 复检结果, 0默认, 1合格, 2不合格
UINT32 Write_HeartBeat;         //D15, 写心跳
UINT32 Write_JudgeLight;        //D16, 判断结果, 0默认, 1有光, 2 无光
UINT32 Write_StopMove;          //D17, 无光时停止运动, 0默认, 1有效

/*------------------------------------------------------------------------------
** Min, max and default value for appl_aiUint16
**------------------------------------------------------------------------------
*/
static AD_UINT32Type appl_sUint32Prop = { { 0, 0xFFFFFFFF, 0 } };
/*******************************************************************************
** Public Globals
********************************************************************************
*/

/*------------------------------------------------------------------------------
** 32 16-bit values as an array
**------------------------------------------------------------------------------
*/

/*-------------------------------------------------------------------------------------------------------------
** 1. iInstance | 2. pabName | 3. bDataType | 4. bNumOfElements | 5. bDesc | 6. pxValuePtr | 7. pxValuePropPtr
**--------------------------------------------------------------------------------------------------------------
*/
const AD_AdiEntryType APPL_asAdiEntryList[] =
{
    /*NO.00*/{ 0x1, "Read_InPlace"        ,ABP_UINT32, 1, APPL_READ_MAP_WRITE_ACCESS_DESC,{ { &Read_InPlace		   , &appl_sUint32Prop } } },
    /*NO.01*/{ 0x2, "Read_Pos_X"          ,ABP_UINT32, 1, APPL_READ_MAP_WRITE_ACCESS_DESC,{ { &Read_Pos_X		   , &appl_sUint32Prop } } },
    /*NO.02*/{ 0x3, "Read_Pos_Y"          ,ABP_UINT32, 1, APPL_READ_MAP_WRITE_ACCESS_DESC,{ { &Read_Pos_Y		   , &appl_sUint32Prop } } },
    /*NO.03*/{ 0x4, "Read_HeartBeat"      ,ABP_UINT32, 1, APPL_READ_MAP_WRITE_ACCESS_DESC,{ { &Read_HeartBeat	   , &appl_sUint32Prop } } },
    /*NO.04*/{ 0x5, "Read_MoveOver"       ,ABP_UINT32, 1, APPL_READ_MAP_WRITE_ACCESS_DESC,{ { &Read_MoveOver	   , &appl_sUint32Prop } } },
    /*NO.05*/{ 0x9, "Read_ClearData"      ,ABP_UINT32, 1, APPL_READ_MAP_WRITE_ACCESS_DESC,{ { &Read_ClearData	   , &appl_sUint32Prop } } },

    /*NO.06*/{ 0xA, "Write_InPlace"       ,ABP_UINT32, 1, APPL_WRITE_MAP_READ_ACCESS_DESC,{ { &Write_InPlace       , &appl_sUint32Prop } } },
    /*NO.07*/{ 0xB, "Write_DetectResult"  ,ABP_UINT32, 1, APPL_WRITE_MAP_READ_ACCESS_DESC,{ { &Write_DetectResult  , &appl_sUint32Prop } } },
    /*NO.08*/{ 0xC, "Write_Pos_X"         ,ABP_UINT32, 1, APPL_WRITE_MAP_READ_ACCESS_DESC,{ { &Write_Pos_X         , &appl_sUint32Prop } } },
    /*NO.09*/{ 0xD, "Write_Pos_Y"         ,ABP_UINT32, 1, APPL_WRITE_MAP_READ_ACCESS_DESC,{ { &Write_Pos_Y         , &appl_sUint32Prop } } },
    /*NO.10*/{ 0xE, "Write_ReDetectResult",ABP_UINT32, 1, APPL_WRITE_MAP_READ_ACCESS_DESC,{ { &Write_ReDetectResult, &appl_sUint32Prop } } },
    /*NO.11*/{ 0xF, "Write_HeartBeat"     ,ABP_UINT32, 1, APPL_WRITE_MAP_READ_ACCESS_DESC,{ { &Write_HeartBeat     , &appl_sUint32Prop } } }
};

EXTFUNC AD_AdiEntryType Get_APPL_asAdiEntryList( int i )
{
    return APPL_asAdiEntryList[i];
}

EXTFUNC void Set_APPL_asAdiEntryList(int i, int plValue )
{
    *(APPL_asAdiEntryList[i].uData.sUINT32.plValuePtr) = plValue;

}

/*------------------------------------------------------------------------------
** Map all adi:s in both directions
**------------------------------------------------------------------------------
** 1. AD instance | 2. Direction | 3. Num elements | 4. Start index |
**------------------------------------------------------------------------------
*/
const AD_DefaultMapType APPL_asAdObjDefaultMap[] =
{
    { 1,  PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
    { 2,  PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
    { 3,  PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
    { 4,  PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
    { 5,  PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
    { 9,  PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },

    { 10, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
    { 11, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
    { 12, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
    { 13, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
    { 14, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
    { 15, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },

	{ AD_DEFAULT_MAP_END_ENTRY }
};

/*******************************************************************************
** Private Services
********************************************************************************
*/

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
