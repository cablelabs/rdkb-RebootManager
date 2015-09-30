/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2015 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

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

    module: cosa_apis_configuration.h

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


#ifndef  _COSA_APIS_CONFIGURATION_H
#define  _COSA_APIS_CONFIGURATION_H

#define RM_CONFIGURATION_OBJECT_NAME "com.cisco.spvtg.ccsp.rm.Configuration."

#define RM_CONFIGURATION_CriticalWaitTimeId      0
#define RM_CONFIGURATION_RebootSignalTimeId		 1
#define RM_CONFIGURATION_WatchdogIntervalId      2

#define RM_CONFIGURATION_NumULongPara    3

/*
Number of secs to wait for an ongoing critical event to complete. 
This is used in addition to turning on notification on the Component 
to notify when the critical event is processed, This allows the Reboot 
Manager to recover from conditions where the component processing 
critical event dies and never sends a notification back
*/
#define DEFAUT_CRITICAL_WAIT_TIME	2

/*
Number of seconds to wait after the Reboot Manger sends out a signal 
on the CCSP Message Bus indicating that the system is going down. 
This allows other CCSP components to persist any state/settings
*/
#define DEFAUT_REBOOT_SIGNAL_TIME	5

/*
Timer interval in secs. The Reboot Manager must respond to the 
signal from kernel space within this time. Otherwise the hardware 
watchdog resets the box
*/
#define DEFAUT_WATCH_DOG_INTERVAL	1

enum param_state_e
{
    CAN_COMMIT,
    CAN_ROLLBACK,
    CAN_NEITHER
}  ;

typedef  struct
_RmObjectConfiguration
{
    char *pULongParaName[RM_CONFIGURATION_NumULongPara]; 
    ULONG ulULongParaValue[RM_CONFIGURATION_NumULongPara];
    enum param_state_e eULongParaState[RM_CONFIGURATION_NumULongPara];
    ULONG ulULongParaValueBackup[RM_CONFIGURATION_NumULongPara];
}
RmObjectConfiguration,  *PRmObjectConfiguration;

int
Configuration_SavePsmValueRecord
(
    int paraId
);

void Configuration_Parameters_Init();

/***********************************************************************

 APIs for Object:

    com.cisco.spvtg.ccsp.rm.Configuration.

    *  Configuration_GetParamBoolValue
    *  Configuration_GetParamIntValue
    *  Configuration_GetParamUlongValue
    *  Configuration_GetParamStringValue
    *  Configuration_SetParamBoolValue
    *  Configuration_SetParamIntValue
    *  Configuration_SetParamUlongValue
    *  Configuration_SetParamStringValue
    *  Configuration_Validate
    *  Configuration_Commit
    *  Configuration_Rollback

***********************************************************************/
BOOL
Configuration_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    );

BOOL
Configuration_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    );

BOOL
Configuration_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      pUlong
    );

ULONG
Configuration_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    );

BOOL
Configuration_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    );

BOOL
Configuration_SetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int                         value
    );

BOOL
Configuration_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValuepUlong
    );

BOOL
Configuration_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       strValue
    );

BOOL
Configuration_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    );

ULONG
Configuration_Commit
    (
        ANSC_HANDLE                 hInsContext
    );

ULONG
Configuration_Rollback
    (
        ANSC_HANDLE                 hInsContext
    );

#endif
