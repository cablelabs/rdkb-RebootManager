/**********************************************************************
   Copyright [2014] [Cisco Systems, Inc.]
 
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
 
       http://www.apache.org/licenses/LICENSE-2.0
 
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
**********************************************************************/


/**************************************************************************

    module: cosa_apis_reboot.h

        For COSA Data Model Library Development

    -------------------------------------------------------------------

    copyright:

        Cisco Systems, Inc.
        All Rights Reserved.

    -------------------------------------------------------------------

    description:

        This file defines the apis for objects to support Data Model Library.

    -------------------------------------------------------------------


    author:

        COSA XML TOOL CODE GENERATOR 1.0

    -------------------------------------------------------------------

    revision:

        11/16/2011    initial revision.

**************************************************************************/


#ifndef  _COSA_APIS_REBOOT_H
#define  _COSA_APIS_REBOOT_H

#if 0
#include "slap_definitions.h"
#endif


#define REBOOT_STATUS_STRING_MAX_SIZE	128

/***********************************************************************

 APIs for Object:

    com.cisco.spvtg.ccsp.rm.Reboot.

    *  Reboot_GetParamBoolValue
    *  Reboot_GetParamIntValue
    *  Reboot_GetParamUlongValue
    *  Reboot_GetParamStringValue
    *  Reboot_SetParamBoolValue
    *  Reboot_SetParamIntValue
    *  Reboot_SetParamUlongValue
    *  Reboot_SetParamStringValue
    *  Reboot_Validate
    *  Reboot_Commit
    *  Reboot_Rollback

***********************************************************************/
BOOL
Reboot_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    );

BOOL
Reboot_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    );

BOOL
Reboot_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      pUlong
    );

ULONG
Reboot_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    );

BOOL
Reboot_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    );

BOOL
Reboot_SetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int                         value
    );

BOOL
Reboot_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValuepUlong
    );

BOOL
Reboot_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       strValue
    );

BOOL
Reboot_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    );

ULONG
Reboot_Commit
    (
        ANSC_HANDLE                 hInsContext
    );

ULONG
Reboot_Rollback
    (
        ANSC_HANDLE                 hInsContext
    );

#endif
