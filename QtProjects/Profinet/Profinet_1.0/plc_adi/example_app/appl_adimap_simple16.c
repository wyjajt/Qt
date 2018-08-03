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
int    heartBeat_Read  = 0;
int    heartBeat_Write = 0;

UINT32 Read_InPlace;            //D1, 1开始, 0结束
UINT32 Read_Pos_X;              //D2, int
UINT32 Read_Pos_Y;              //D3, int
UINT32 Read_DetectOver;         //D5, 运动完成
UINT32 Read_ReDetect;           //D6, 再检测
UINT32 Read_HeartBeat;          //D8, 读心跳

UINT32 Write_InPlace;           //D11, 1
UINT32 Write_Pos_X;             //D12, int
UINT32 Write_Pos_Y;             //D14, int
UINT32 Write_DetResult_OK;      //D16, 1,产品合格
UINT32 Write_DetResult_NG;      //D17, 1,产品NG
UINT32 Write_ReDetectResult;    //D18, 0,1
UINT32 Write_HeartBeat;         //D20, 写心跳
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
    /*NO.00*/{ 0x1, "Read_InPlace",        ABP_UINT32, 1, APPL_READ_MAP_WRITE_ACCESS_DESC,{ { &Read_InPlace,   &appl_sUint32Prop } } },
    /*NO.01*/{ 0x2, "Read_Pos_X",          ABP_UINT32, 1, APPL_READ_MAP_WRITE_ACCESS_DESC,{ { &Read_Pos_X,     &appl_sUint32Prop } } },
    /*NO.02*/{ 0x3, "Read_Pos_Y",          ABP_UINT32, 1, APPL_READ_MAP_WRITE_ACCESS_DESC,{ { &Read_Pos_Y,     &appl_sUint32Prop } } },
    /*NO.03*/{ 0x5, "Read_DetectOver",     ABP_UINT32, 1, APPL_READ_MAP_WRITE_ACCESS_DESC,{ { &Read_DetectOver,&appl_sUint32Prop } } },
    /*NO.04*/{ 0x6, "Read_ReDetect",       ABP_UINT32, 1, APPL_READ_MAP_WRITE_ACCESS_DESC,{ { &Read_ReDetect,  &appl_sUint32Prop } } },
    /*NO.05*/{ 0x8, "Read_HeartBeat",      ABP_UINT32, 1, APPL_READ_MAP_WRITE_ACCESS_DESC,{ { &Read_HeartBeat, &appl_sUint32Prop } } },
    
    /*NO.06*/{ 0xB, "Write_InPlace"       ,ABP_UINT32, 1, APPL_WRITE_MAP_READ_ACCESS_DESC,{ { &Write_InPlace,  &appl_sUint32Prop } } },
    /*NO.07*/{ 0xC, "Write_Pos_X"         ,ABP_UINT32, 1, APPL_WRITE_MAP_READ_ACCESS_DESC,{ { &Write_Pos_X,    &appl_sUint32Prop } } },
    /*NO.08*/{ 0xE, "Write_Pos_Y"         ,ABP_UINT32, 1, APPL_WRITE_MAP_READ_ACCESS_DESC,{ { &Write_Pos_Y  ,  &appl_sUint32Prop } } },
    /*NO.09*/{ 0x10,"Write_DetResult_OK"  ,ABP_UINT32, 1, APPL_WRITE_MAP_READ_ACCESS_DESC,{ { &Write_DetResult_OK         , &appl_sUint32Prop } } },
    /*NO.10*/{ 0x11,"Write_DetResult_NG"  ,ABP_UINT32, 1, APPL_WRITE_MAP_READ_ACCESS_DESC,{ { &Write_DetResult_NG         , &appl_sUint32Prop } } },
    /*NO.11*/{ 0x12,"Write_ReDetectResult",ABP_UINT32, 1, APPL_WRITE_MAP_READ_ACCESS_DESC,{ { &Write_ReDetectResult, &appl_sUint32Prop } } },
    /*NO.12*/{ 0x14,"Write_HeartBeat"     ,ABP_UINT32, 1, APPL_WRITE_MAP_READ_ACCESS_DESC,{ { &Write_HeartBeat     , &appl_sUint32Prop } } }
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
    { 5,  PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
    { 6,  PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
    { 8,  PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },

    { 11, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
    { 12, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
    { 14, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
    { 16, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
    { 17, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
    { 18, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
    { 20, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },

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
