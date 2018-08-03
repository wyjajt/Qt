//////////////////////////////////////////////////////////////////////////
// HMS Technology Center Ravensburg GmbH
//////////////////////////////////////////////////////////////////////////
/**
  definition of some functions executed on the setup of the hardware !

  @file abcc_setup.h

*/
//////////////////////////////////////////////////////////////////////////
// (C) 2002-2016 HMS, all rights reserved
//////////////////////////////////////////////////////////////////////////
#ifndef _ABCC_SETUP_H_
#define _ABCC_SETUP_H_

//////////////////////////////////////////////////////////////////////////
// include files
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// constants and macros
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// data types
//////////////////////////////////////////////////////////////////////////

void ABCC_SetupInit( void );

BOOL ABCC_GatherInfo    ( HANDLE hEvent, UINT32 dwTimeout);
BOOL ABCC_MapPD         ( HANDLE hEvent, UINT32 dwTimeout);
BOOL ABCC_FinalizeSetup ( HANDLE hEvent, UINT32 dwTimeout);


#endif // _ABCC_SETUP_H_
