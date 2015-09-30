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

/*********************************************************************** 
  
    module: plugin_main.c

        Implement COSA Data Model Library Init and Unload apis.
 
    ---------------------------------------------------------------

    copyright:

        Cisco Systems, Inc.
        All Rights Reserved.

    ---------------------------------------------------------------

    author:

        COSA XML TOOL CODE GENERATOR 1.0

    ---------------------------------------------------------------

    revision:

        11/16/2011    initial revision.

**********************************************************************/

#include "ansc_platform.h"
#include "ansc_load_library.h"
#include "cosa_plugin_api.h"
#include "plugin_main.h"
#include "cosa_apis_configuration.h"
#include "cosa_apis_reboot.h"

#define THIS_PLUGIN_VERSION                         1
int     BackEndInitDone = 0;

int ANSC_EXPORT_API
COSA_Init
    (
        ULONG                       uMaxVersionSupported, 
        void*                       hCosaPlugInfo         /* PCOSA_PLUGIN_INFO passed in by the caller */
    )
{
    PCOSA_PLUGIN_INFO               pPlugInfo  = (PCOSA_PLUGIN_INFO)hCosaPlugInfo;

    if ( uMaxVersionSupported < THIS_PLUGIN_VERSION )
    {
      /* this version is not supported */
        return -1;
    }   
    
    pPlugInfo->uPluginVersion       = THIS_PLUGIN_VERSION;

    /* register the back-end apis for the data model */
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Configuration_GetParamBoolValue",  Configuration_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Configuration_GetParamIntValue",  Configuration_GetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Configuration_GetParamUlongValue",  Configuration_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Configuration_GetParamStringValue",  Configuration_GetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Configuration_SetParamBoolValue",  Configuration_SetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Configuration_SetParamIntValue",  Configuration_SetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Configuration_SetParamUlongValue",  Configuration_SetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Configuration_SetParamStringValue",  Configuration_SetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Configuration_Validate",  Configuration_Validate);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Configuration_Commit",  Configuration_Commit);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Configuration_Rollback",  Configuration_Rollback);

	pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Reboot_GetParamBoolValue",  Reboot_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Reboot_GetParamIntValue",  Reboot_GetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Reboot_GetParamUlongValue",  Reboot_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Reboot_GetParamStringValue",  Reboot_GetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Reboot_SetParamBoolValue",  Reboot_SetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Reboot_SetParamIntValue",  Reboot_SetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Reboot_SetParamUlongValue",  Reboot_SetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Reboot_SetParamStringValue",  Reboot_SetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Reboot_Validate",  Reboot_Validate);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Reboot_Commit",  Reboot_Commit);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Reboot_Rollback",  Reboot_Rollback);

	//Reboot manager start before PSM, so e must call Configuration_Parameters_Init() later when SystemReadySignal 
    Configuration_Parameters_Init();
    BackEndInitDone = 1;
    return  0;
}

BOOL ANSC_EXPORT_API
COSA_IsObjSupported
    (
        char*                        pObjName
    )
{
    /* COSA XML file will be generated based on standard TR-xxx data model definition.
     * By default, all the objects are expected to supported in the libraray. 
     * Realistically, we will have certain ones cannot be supported at the early stage of development.
     * We can rule them out by return FALSE even if they're defined in COSA XML file.
     */

#if 0 

    if( AnscEqualString(pObjName, "InternetGatewayDevice.UserInterface.",TRUE))
    {
        /* all the objects/parameters under "UserInterface" will not be populated in Data Model Tree. */
        return FALSE;
    }

#endif

    return TRUE;
}

void ANSC_EXPORT_API
COSA_Unload
    (
        void
    )
{
    /* unload the memory here */
}
