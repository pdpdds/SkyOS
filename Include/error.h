/******************************************************************************
   error.h
		-system error codes

   modified\ Aug 06 2008
   arthor\ Mike
-------------------------------------------------------------------------------
   Copyright © 2008, BrokenThorn Entertainment Co.  Copyright of, or the use of
   this, or any part, of this software without the consent of BrokenThorn Entertainment,
   Co. is strickly prohibated. All other rights reserved.
******************************************************************************/

//
//  Values are 32 bit values laid out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code

//! facility codes

#define FACILITY_MOS                     8
#define FACILITY_URT                     19
#define FACILITY_UMI                     22
#define FACILITY_SXS                     23
#define FACILITY_STORAGE                 3
#define FACILITY_STATE_MANAGEMENT        34
#define FACILITY_SSPI                    9
#define FACILITY_SCARD                   16
#define FACILITY_SETUPAPI                15
#define FACILITY_SECURITY                9
#define FACILITY_RPC                     1
#define FACILITY_CONTROL                 10
#define FACILITY_NULL                    0
#define FACILITY_METADIRECTORY           35
#define FACILITY_MSMQ                    14
#define FACILITY_MEDIASERVER             13
#define FACILITY_INTERNET                12
#define FACILITY_ITF                     4
#define FACILITY_HTTP                    25
#define FACILITY_DPLAY                   21
#define FACILITY_DISPATCH                2
#define FACILITY_CONFIGURATION           33
#define FACILITY_COMPLUS                 17
#define FACILITY_CERT                    11
#define FACILITY_BACKGROUNDCOPY          32
#define FACILITY_ACS                     20
#define FACILITY_AAF                     18

//! error codes

#define ERROR_SUCCESS                    0L
#define ERROR_INVALID_FUNCTION           1L

