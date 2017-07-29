#ifndef _FDD_DRIVER_HEADER_FILE_
#define _FDD_DRIVER_HEADER_FILE_


/**********************************************************************************************************
 *                                                INCLUDE FILES                                           *
 **********************************************************************************************************/
#include "windef.h"


/**********************************************************************************************************
 *                                               DIRVER FUNCTIONS                                         *
 **********************************************************************************************************/
bool FddReadSector (WORD SectorNumber, BYTE NumbersOfSectors, BYTE *pData);
bool FddWriteSector(WORD SectorNumber, BYTE NumbersOfSectors, BYTE *pData);


#endif /* #ifndef _FDD_DRIVER_HEADER_FILE_ */