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
** File Description:
** This is the public header file for the EtherCat object.
********************************************************************************
********************************************************************************
** Services:
**
** Public Services:
**    ECT_ProcessCmdMsg()     - Processes commands sent to the ECT object.
********************************************************************************
********************************************************************************
*/
#ifndef ECT_H
#define ECT_H

/*******************************************************************************
** Typedefs
********************************************************************************
*/

/*******************************************************************************
** Private Globals
********************************************************************************
*/

/*******************************************************************************
** Public Services
********************************************************************************
*/

/*------------------------------------------------------------------------------
** void ECT_ProcessCmdMsg()
** Processes commands sent to the ECT object.
**------------------------------------------------------------------------------
** Arguments:
**       psNewMessage      - Pointer to a ABP_MsgType message.
**
** Returns:
**       None.
**
** Usage:
**       ECT_ProcessCmdMsg( &sMessage );
**------------------------------------------------------------------------------
*/
void ECT_ProcessCmdMsg( ABP_MsgType* psNewMessage );

#endif  /* inclusion lock */
