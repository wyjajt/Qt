//////////////////////////////////////////////////////////////////////////
// HMS Technology Center Ravensburg GmbH
//////////////////////////////////////////////////////////////////////////
/**
  @todo

  @file

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
#include "ad_map.h"
#include "ad_obj.h"

//////////////////////////////////////////////////////////////////////////
// static constants, types, macros, variables
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// global variables
//////////////////////////////////////////////////////////////////////////

/*
** Invalid ADI index.
*/
#define AD_INVALID_ADI_INDEX                 ( 0xffff )

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
  simple helper function to look for the instance in a ADI entry

  @param iInstance
    instance to look for
  @param iNumAdi
    number of ADIs
  @param psAdiEntry
    a list of ADI entries

  @return 
    If the function succeeds it returns the index of the ADI entry, 
    otherwise AD_INVALID_ADI_INDEX
*/
//////////////////////////////////////////////////////////////////////////
static UINT16 GetAdiIndex( UINT16 iInstance, 
                           int    iNumAdi,
                           AD_AdiEntryType* psAdiEntry)
{
  UINT16 i;
  UINT16  iIndex;

  iIndex = AD_INVALID_ADI_INDEX;

  for( i = 0; i < iNumAdi; i++ )
  {
    if( psAdiEntry[ i ].iInstance == iInstance )
    {
      iIndex = i;
      break;
    }
  }

  return( iIndex );
}

//////////////////////////////////////////////////////////////////////////
/**
  Calculates map size of of an ADI mapping item, in bits.

  @param psAdiEntry
    Pointer to ADI entry.
  @param bNumElem
    Number of elements
  @param bElemStartIndex
     First element index.

  @return 
    If the function succeeds it returns the size on Bits, otherwise 0.
*/
//////////////////////////////////////////////////////////////////////////
static UINT16 GetAdiMapSizeInBits( const AD_AdiEntryType* psAdiEntry, 
                                   UINT8 bNumElem, 
                                   UINT8 bElemStartIndex )
{
  UINT16 iSize = 0;
#if( ABCC_CFG_STRUCT_DATA_TYPE )
  UINT16 i;
  if( psAdiEntry->psStruct == NULL )
  {
    iSize = ABCC_GetDataTypeSizeInBits( psAdiEntry->bDataType ) * bNumElem ;
  }
  else
  {
    iSize = 0;
    for( i = bElemStartIndex; i < ( bNumElem + bElemStartIndex ) ; i++ )
    {
      iSize += ABCC_GetDataTypeSizeInBits( psAdiEntry->psStruct[ i ].bDataType );
    }
  }
#else
  (void)bElemStartIndex;
  iSize = ABCC_cGetDataTypeSizeInBits( psAdiEntry->bDataType ) * bNumElem ;
#endif

  return iSize;
}

//////////////////////////////////////////////////////////////////////////
/**
  function to set the message data according to the definition

  @param iAdi
    ADI number
  @param bAdiTotNumElem
    number of elements of this ADI
  @param bElemStartIndex
    Index of element
  @param bNumElem
    number of elements
  @param bDataType
    data type of this element
  @param pwData
    ptr on the message data section to fill
  @param pwWritten
    number of bytes written in the data section
*/
//////////////////////////////////////////////////////////////////////////
static void FillMapExtCommand( UINT16 iAdi, 
                               UINT8 bAdiTotNumElem, 
                               UINT8 bElemStartIndex, 
                               UINT8 bNumElem, 
                               UINT8 bDataType,
                               UINT16 * pwData, 
                               UINT16 * pwWritten )
{
  if (( pwData ) && (pwWritten))
  {
    pwData[ 0 ] = iTOiLe( iAdi );                         /* ADI Instance number. */
    ABCC_SetLowAddrOct ( pwData[ 1 ], bAdiTotNumElem);    /* Total number of elements in ADI. */
    ABCC_SetHighAddrOct( pwData[ 1 ], bElemStartIndex );
    ABCC_SetLowAddrOct ( pwData[ 2 ], bNumElem);
    ABCC_SetHighAddrOct( pwData[ 2 ], 1 );                /* Number of type descriptors. */
    ABCC_SetLowAddrOct ( pwData[ 3 ], bDataType );        /* ADI element data type. */
    *pwWritten = iTOiLe( 7 );  /* The number of used octets in aiData. (The bytes written below). */
  }
}

//////////////////////////////////////////////////////////////////////////
/**
  function to go through the ADI mapping 

  @param bModuleId
    anybus module type
  @param iMappingIndex
    mapping index
  @param pbDirectory
    direction of the mapping entry ( read or write )
  @param piMappedInBits
    bit count of the mapping entry
  @param wLength
    length of the data field, which can be written
  @param pwWritten
    length, which were written
  @param pbData
    data ptr

  @return 
    If the function succeeds it returns TRUE, otherwise FALSE

  @note 
    Access:    public 
*/
//////////////////////////////////////////////////////////////////////////
BOOL GetADMapping ( UINT8    bModuleId, 
                    int      iMappingIndex, 
                    UINT8  * pbDirectory,
                    int    * piMappedInBits,
                    UINT16   wLength, // maximal length
                    UINT16 * pwWritten, 
                    UINT8  * pbData )
{
  BOOL fRet = FALSE ;
  UINT16 * pwData = (UINT16 *) pbData;
  /*
  ** Help variables for ADI mapping service
  */
  AD_AdiEntryType*     psAdiEntry     = NULL;
  AD_DefaultMapType*   psDefaultMap   = NULL;

  static UINT16               abcc_iNumAdi   = 0;

  int iLocalMapIndex  = 0;
  int iLocalSize      = 0;

  if (( pbDirectory ) && ( piMappedInBits) && ( wLength ) && (pbData ))
  {
    fRet = TRUE;
    abcc_iNumAdi = AD_AdiMappingReq ( (const AD_AdiEntryType** )   &psAdiEntry,
                                      (const AD_DefaultMapType** ) &psDefaultMap );

    //
    // execute some tests
    // and initialize iLocalMapIndex
    if( psAdiEntry && psDefaultMap && ( psDefaultMap[ iMappingIndex ].eDir != PD_END_MAP ) )
    {
        if( psDefaultMap[ iMappingIndex ].iInstance != AD_MAP_PAD_ADI )
        {
          iLocalMapIndex = GetAdiIndex( psDefaultMap[ iMappingIndex ].iInstance, 
                                        abcc_iNumAdi,
                                        psAdiEntry );

          if( iLocalMapIndex == AD_INVALID_ADI_INDEX )
          {
            fRet = FALSE;
          }
        }
    }
    else
    {
      fRet = FALSE;
    }

    if ( fRet)
    {
      fRet = FALSE;

      if( bModuleId == ABP_MODULE_ID_ACTIVE_ABCC40 ) /* If an ABCC40 is attached. */
      {
        UINT8 bNumElemToMap;
        UINT8 bElemMapStartIndex;

        /*
        ** Implement mapping according to the extended command for ABCC40.
        */

        if ( psDefaultMap[ iMappingIndex ].iInstance != AD_MAP_PAD_ADI )
        {
          if( psDefaultMap[ iMappingIndex ].bNumElem == AD_DEFAULT_MAP_ALL_ELEM )
          {
              bNumElemToMap = psAdiEntry[ iLocalMapIndex ].bNumOfElements;
              bElemMapStartIndex = 0;
          }
          else
          {
              bNumElemToMap = psDefaultMap[ iMappingIndex ].bNumElem;
              bElemMapStartIndex = psDefaultMap[ iMappingIndex ].bElemStartIndex;
          }

          FillMapExtCommand( psAdiEntry[ iLocalMapIndex ].iInstance ,       // Adi
                              psAdiEntry[ iLocalMapIndex ].bNumOfElements , // Adi total num elements
                              bElemMapStartIndex,                           // Mapping  start index
                              bNumElemToMap,                                // Num elements to map 
                              psAdiEntry[ iLocalMapIndex ].bDataType,
                              pwData,
                              pwWritten );                                  // Data type

          iLocalSize = GetAdiMapSizeInBits( &psAdiEntry[ iLocalMapIndex ],
                                                  bNumElemToMap, bElemMapStartIndex );

    #if( ABCC_CFG_STRUCT_DATA_TYPE )
          if ( psAdiEntry[ iLocalMapIndex ].psStruct != NULL )
          {
            UINT16 iDescOffset;
            iDescOffset = 0;
            ABCC_SetHighAddrOct( pwData[ 2 ], bNumElemToMap );

            while ( iDescOffset < bNumElemToMap  )
            {
              ABCC_SetLowAddrOct(pwData[ ( iDescOffset >> 1) + 3 ], psAdiEntry[ iLocalMapIndex ].psStruct[ iDescOffset + bElemMapStartIndex].bDataType );
              iDescOffset++;
              if( iDescOffset < bNumElemToMap )
              {
                  ABCC_SetHighAddrOct( pwData[ ( iDescOffset >> 1) + 3 ], psAdiEntry[ iLocalMapIndex ].psStruct[ iDescOffset + bElemMapStartIndex].bDataType );
                  iDescOffset++;
              }
            }

            *pwWritten = iTOiLe( 6 + iDescOffset );
          }
    #endif
        }
        else
        {
          FillMapExtCommand( 0 ,                                       // Adi
                              psDefaultMap[ iMappingIndex ].bNumElem , // Adi total num elements 
                              0,                                       // Mapping  start index 
                              psDefaultMap[ iMappingIndex ].bNumElem , // Num elements to map 
                              ABP_PAD1,                                // Data type 
                              pwData,
                              pwWritten );

          iLocalSize = psDefaultMap[ iMappingIndex ].bNumElem;
        }

        *pbDirectory = psDefaultMap[ iMappingIndex ].eDir;

        if ( piMappedInBits )
        {
          *piMappedInBits = iLocalSize;
        }

        fRet = TRUE;
      }
    }
  }

  return fRet;
}


