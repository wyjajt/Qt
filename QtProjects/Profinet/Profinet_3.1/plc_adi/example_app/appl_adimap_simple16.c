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

UINT32 Read_JudgeLight;         //D1 , 判断是否有光, 1有效
UINT32 Read_InPlace_1;          //D2 , 转台就位(细扫), 1有效
UINT32 Read_Pos_X;              //D3 , int
UINT32 Read_Pos_Y;              //D4 , int
UINT32 Read_MoveOver_1;         //D5 , 细扫运动完成, 1有效
UINT32 Read_ReDetect;           //D6 , 请求复检
UINT32 Read_InPlace_2;          //D7 , 转台就位(粗扫), 1有效
UINT32 Read_HeartBeat;          //D8 , 读心跳
UINT32 Read_MoveOver_2;         //D25, 粗扫运动完成, 1有效

UINT32 Write_JudgeLight;        //D10, 光通量判断结果, 1有光, 2无光
UINT32 Write_InPlace_1;         //D11, 细扫就位, 1有效
UINT32 Write_Pos_X;             //D12, int
UINT32 Write_Pos_Y;             //D13, int
UINT32 Write_Pos_OK_1;          //D14, 细扫最大值, 可以读取位置, 1有效
UINT32 Write_DetResult;         //D15, 复检结果, 1 OK, 2 NG
UINT32 Write_Pos_OK_2;          //D16, 粗扫最大值, 可以读取位置, 1有效
UINT32 Write_DetectNG;          //D18, 粗扫结束, 没扫到光, 1有效
UINT32 Write_HeartBeat;         //D19, 写心跳
UINT32 Write_InPlace_2;         //D21, 粗扫就位, 1有效

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
    /*NO.00*/{ 0x01, "Read_JudgeLight" , ABP_UINT32, 1, APPL_READ_MAP_WRITE_ACCESS_DESC,{ { &Read_JudgeLight , &appl_sUint32Prop } } },
    /*NO.01*/{ 0x02, "Read_InPlace_1"  , ABP_UINT32, 1, APPL_READ_MAP_WRITE_ACCESS_DESC,{ { &Read_InPlace_1  , &appl_sUint32Prop } } },
    /*NO.02*/{ 0x03, "Read_Pos_X"      , ABP_UINT32, 1, APPL_READ_MAP_WRITE_ACCESS_DESC,{ { &Read_Pos_X      , &appl_sUint32Prop } } },
    /*NO.03*/{ 0x04, "Read_Pos_Y"      , ABP_UINT32, 1, APPL_READ_MAP_WRITE_ACCESS_DESC,{ { &Read_Pos_Y      , &appl_sUint32Prop } } },
    /*NO.04*/{ 0x05, "Read_MoveOver_1" , ABP_UINT32, 1, APPL_READ_MAP_WRITE_ACCESS_DESC,{ { &Read_MoveOver_1 , &appl_sUint32Prop } } },
    /*NO.05*/{ 0x06, "Read_ReDetect"   , ABP_UINT32, 1, APPL_READ_MAP_WRITE_ACCESS_DESC,{ { &Read_ReDetect   , &appl_sUint32Prop } } },
    /*NO.06*/{ 0x07, "Read_InPlace_2"  , ABP_UINT32, 1, APPL_READ_MAP_WRITE_ACCESS_DESC,{ { &Read_InPlace_2  , &appl_sUint32Prop } } },
    /*NO.07*/{ 0x08, "Read_HeartBeat"  , ABP_UINT32, 1, APPL_READ_MAP_WRITE_ACCESS_DESC,{ { &Read_HeartBeat  , &appl_sUint32Prop } } },
    /*NO.08*/{ 0x19, "Read_MoveOver_2" , ABP_UINT32, 1, APPL_READ_MAP_WRITE_ACCESS_DESC,{ { &Read_MoveOver_2 , &appl_sUint32Prop } } },

    /*NO.09*/{ 0x0A, "Write_JudgeLight", ABP_UINT32, 1, APPL_WRITE_MAP_READ_ACCESS_DESC,{ { &Write_JudgeLight, &appl_sUint32Prop } } },
    /*NO.10*/{ 0x0B, "Write_InPlace_1" , ABP_UINT32, 1, APPL_WRITE_MAP_READ_ACCESS_DESC,{ { &Write_InPlace_1 , &appl_sUint32Prop } } },
    /*NO.11*/{ 0x0C, "Write_Pos_X"     , ABP_UINT32, 1, APPL_WRITE_MAP_READ_ACCESS_DESC,{ { &Write_Pos_X     , &appl_sUint32Prop } } },
    /*NO.12*/{ 0x0D, "Write_Pos_Y"     , ABP_UINT32, 1, APPL_WRITE_MAP_READ_ACCESS_DESC,{ { &Write_Pos_Y     , &appl_sUint32Prop } } },
    /*NO.13*/{ 0x0E, "Write_Pos_OK_1"  , ABP_UINT32, 1, APPL_WRITE_MAP_READ_ACCESS_DESC,{ { &Write_Pos_OK_1  , &appl_sUint32Prop } } },
    /*NO.14*/{ 0x0F, "Write_DetResult" , ABP_UINT32, 1, APPL_WRITE_MAP_READ_ACCESS_DESC,{ { &Write_DetResult , &appl_sUint32Prop } } },
    /*NO.15*/{ 0x10, "Write_Pos_OK_2"  , ABP_UINT32, 1, APPL_WRITE_MAP_READ_ACCESS_DESC,{ { &Write_Pos_OK_2  , &appl_sUint32Prop } } },
    /*NO.16*/{ 0x12, "Write_DetectNG"  , ABP_UINT32, 1, APPL_WRITE_MAP_READ_ACCESS_DESC,{ { &Write_DetectNG  , &appl_sUint32Prop } } },
    /*NO.17*/{ 0x13, "Write_HeartBeat" , ABP_UINT32, 1, APPL_WRITE_MAP_READ_ACCESS_DESC,{ { &Write_HeartBeat , &appl_sUint32Prop } } },
    /*NO.18*/{ 0x15, "Write_InPlace_2" , ABP_UINT32, 1, APPL_WRITE_MAP_READ_ACCESS_DESC,{ { &Write_InPlace_2 , &appl_sUint32Prop } } }
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
    {  1,  PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
    {  2,  PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
    {  3,  PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
    {  4,  PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
    {  5,  PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
    {  6,  PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
    {  7,  PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
    {  8,  PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
    { 25,  PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },

    { 10, PD_WRITE,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
    { 11, PD_WRITE,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
    { 12, PD_WRITE,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
    { 13, PD_WRITE,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
    { 14, PD_WRITE,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
    { 15, PD_WRITE,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
    { 16, PD_WRITE,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
    { 18, PD_WRITE,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
    { 19, PD_WRITE,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
    { 21, PD_WRITE,  AD_DEFAULT_MAP_ALL_ELEM, 0 },

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
