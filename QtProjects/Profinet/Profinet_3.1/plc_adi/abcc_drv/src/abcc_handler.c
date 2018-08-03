/*******************************************************************************
********************************************************************************
**                                                                            **
** ABCC Driver version 4.01.01 (2015-12-14)                                   **
**                                                                            **
** Delivered with:                                                            **
**    ABP         7.16.01 (2015-10-14)                                        **
**                                                                            */
/*******************************************************************************
********************************************************************************
** COPYRIGHT NOTIFICATION (c) 2013 HMS Industrial Networks AB                 **
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
** File Description:
** Implementation of operation mode independent parts of the abcc handler.
********************************************************************************
********************************************************************************
*/

#include "abcc.h"
#include "abcc_port.h"



void ABCC_GetMsgString( ABP_MsgType* psMsg, char* pcString, UINT16 iNumChar, UINT16 iOctetOffset )
{
   ABCC_PORT_StrCpyToNative( pcString,
                             ABCC_GetMsgDataPtr( psMsg ),
                             iOctetOffset,
                             iNumChar );
}

void ABCC_SetMsgString( ABP_MsgType* psMsg, const char* pcString, UINT16 iNumChar, UINT16 iOctetOffset )
{
   ABCC_PORT_StrCpyToPacked( ABCC_GetMsgDataPtr( psMsg ),
                             iOctetOffset,
                             pcString,
                             iNumChar );
}

void ABCC_GetMsgData8( ABP_MsgType* psMsg, UINT8* pbData, UINT16 iOctetOffset )
{
#ifdef ABCC_SYS_16_BIT_CHAR
   *pbData = 0;
#endif
   ABCC_PORT_Copy8( pbData, 0, ABCC_GetMsgDataPtr( psMsg ), iOctetOffset );
}

void ABCC_SetMsgData8( ABP_MsgType* psMsg, UINT8 bData, UINT16 iOctetOffset )
{
   ABCC_PORT_Copy8( ABCC_GetMsgDataPtr( psMsg ), iOctetOffset, &bData, 0 );
}

void ABCC_GetMsgData16( ABP_MsgType* psMsg, UINT16* piData, UINT16 iOctetOffset )
{
   ABCC_PORT_Copy16( piData, 0, ABCC_GetMsgDataPtr( psMsg ), iOctetOffset );
   *piData = iLeTOi( *piData );
}

void ABCC_SetMsgData16( ABP_MsgType* psMsg, UINT16 iData, UINT16 iOctetOffset )
{
   iData = iTOiLe( iData );
   ABCC_PORT_Copy16( ABCC_GetMsgDataPtr( psMsg ), iOctetOffset, &iData, 0 );
}

void ABCC_GetMsgData32( ABP_MsgType* psMsg, UINT32* plData, UINT16 iOctetOffset )
{
   ABCC_PORT_Copy32( plData, 0, ABCC_GetMsgDataPtr( psMsg ), iOctetOffset );
   *plData = lLeTOl( *plData );
}

void ABCC_SetMsgData32( ABP_MsgType* psMsg, UINT32 lData, UINT16 iOctetOffset )
{
   lData = lTOlLe( lData );
   ABCC_PORT_Copy32( ABCC_GetMsgDataPtr( psMsg ), iOctetOffset, &lData, 0 );
}

#if( ABCC_CFG_64BIT_ADI_SUPPORT )
void ABCC_GetMsgData64( ABP_MsgType* psMsg, UINT64* plData, UINT16 iOctetOffset )
{
   ABCC_PORT_Copy64( plData, 0, ABCC_GetMsgDataPtr( psMsg ), iOctetOffset );
   *plData = lLeTOl64( *plData );
}

void ABCC_SetMsgData64( ABP_MsgType* psMsg, UINT64 lData, UINT16 iOctetOffset )
{
   lData = lTOlLe64( lData );
   ABCC_PORT_Copy64( ABCC_GetMsgDataPtr( psMsg ), iOctetOffset, &lData, 0 );
}
#endif
