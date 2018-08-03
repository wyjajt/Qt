///////////////////////////////////////////////////////////////////////////////
// IXXAT Automation GmbH
///////////////////////////////////////////////////////////////////////////////
/**
  definition of structures to comunicate with the vciIDL.dll

  @file vciIDL_def.h
*/
#ifndef __VCIIDL_DEF_H__
#define __VCIIDL_DEF_H__

//////////////////////////////////////////////////////////////////////////
// include files
#include <pshpack4.h>

//////////////////////////////////////////////////////////////////////////
// constants and macros
#define VCIIDL_TXFIFOEVENT           0
#define VCIIDL_RXFIFOEVENT           1
#define VCIIDL_RXRWWBEVENT           2
#define VCIIDL_RXRWWBMODSTATEVENT    3
#define VCIIDL_RXIRQEVENT            4

//////////////////////////////////////////////////////////////////////////
// data types

/*****************************************************************************
 * VCI device information
 ****************************************************************************/

typedef struct _VCIIDL_DEVINFO
{
  VCIID  VciObjectId;          // unique VCI object identifier
  GUID   DeviceClass;          // device class identifier

  UINT8  DriverMajorVersion;   // major driver version number
  UINT8  DriverMinorVersion;   // minor driver version number
  UINT16 DriverBuildVersion;   // build driver version number

  UINT8  HardwareBranchVersion;// hardware branch version number
  UINT8  HardwareMajorVersion; // hardware major version number
  UINT8  HardwareMinorVersion; // hardware minor version number
  UINT8  HardwareBuildVersion; // hardware build version number

  union _UniqueHardwareId      // unique hardware identifier
  {
    CHAR AsChar[16];
    GUID AsGuid;
  } UniqueHardwareId;

  CHAR   Description[128];     // device description
  CHAR   Manufacturer[126];    // device manufacturer
  UINT16 DriverReleaseVersion; // minor driver version number
} VCIIDL_DEVINFO, *PVCIIDL_DEVINFO;


#include <poppack.h>
#endif //__VCIIDL_DEF_H__
