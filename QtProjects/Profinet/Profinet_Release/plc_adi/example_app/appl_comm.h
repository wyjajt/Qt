//////////////////////////////////////////////////////////////////////////
// HMS Technology Center Ravensburg GmbH
//////////////////////////////////////////////////////////////////////////
/**
  definition of some communication functions

  @file appl_comm.h

*/
//////////////////////////////////////////////////////////////////////////
// (C) 2002-2016 HMS, all rights reserved
//////////////////////////////////////////////////////////////////////////
#ifndef _APPL_COMM_H_
#define _APPL_COMM_H_

//////////////////////////////////////////////////////////////////////////
// include files
//////////////////////////////////////////////////////////////////////////
#include "abcc.h"

//////////////////////////////////////////////////////////////////////////
// constants and macros
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// data types
//////////////////////////////////////////////////////////////////////////

void CmdInitArray ( );

BOOL SetAttribute ( UINT8    bObject,
                    UINT16   wInstance,
                    UINT8    bAttribute,
                    HANDLE   hEvent,
                    UINT16   wSize,
                    UINT8  * pbData,
                    UINT16 * pwWritten,
                    UINT32   dwTimeout);

BOOL GetAttribute ( UINT8    bObject,
                    UINT16   wInstance,
                    UINT8    bAttribute,
                    HANDLE   hEvent,
                    UINT16   wSize,
                    UINT8  * pbData,
                    UINT16 * pwReaden,
                    UINT32   dwTimeout);

BOOL MapADIRead   ( HANDLE   hEvent,
                    UINT16   wSize,
                    UINT8  * pbData,
                    UINT16 * pwWritten,
                    UINT32   dwTimeout );

BOOL MapADIWrite  ( HANDLE   hEvent,
                    UINT16   wSize,
                    UINT8  * pbData,
                    UINT16 * pwWritten,
                    UINT32   dwTimeout);

#endif // _APPL_COMM_H_