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

/*********************************************************************************

    description:


        SSP implementation of functions:

        *   RebootManager_Allocate
        *   RebootManager_Free
        *   RebootManager_CloneString
        *   RebootManager_MergeString

  ------------------------------------------------------------------------------

    revision:

        09/08/2011    initial revision.

**********************************************************************************/
#include "ansc_platform.h"
#include "rm_util.h"
#include "cosa_apis_reboot.h"
#include "cosa_apis_configuration.h"

extern ANSC_HANDLE              bus_handle;
extern char                     g_Subsystem[32];
extern RmObjectConfiguration    rmConfig;
extern char                     *pComponentName;
extern int			BackEndInitDone;

void 
CcspRebootManagerSysReadySignalCB(
        void*	user_data)
{
	CcspTraceDebug(("RebootManager just received SysReadySignal....\n"));

	Configuration_Parameters_Init();
	BackEndInitDone = 1;

	CcspRmHal_PostRebootProcedures();
}


void * RebootManager_Allocate
(
    size_t size
)
{
    //return AnscAllocateMemoryCountSize(pComponentName, size);
    return AnscAllocateMemory(size);  // Has to use this since common data library uses it.
    //return malloc(size);
}

void RebootManager_Free
(
    void *p
)
{
    //return AnscFreeMemoryCountSize(pComponentName, p);
    return AnscFreeMemory(p); // Has to use this since common data library uses it.
    //if(p) return free(p);
}

char * RebootManager_CloneString
    (
    const char * src
    )
{
    if(src == NULL) return NULL;
    size_t len = strlen(src) + 1;
    if(len <= 1) return NULL;
    char * dest = RebootManager_Allocate(len);
    strncpy(dest, src, len);
    dest[len - 1] = 0;
    return dest;
}

char * RebootManager_MergeString
    (
    const char * src1,
    const char * src2
    )
{
    size_t len1 = 0, len2 = 0;
    if(src1 != NULL) len1 = strlen(src1);
    if(src2 != NULL) len2 = strlen(src2);
    size_t len = len1 + len2 + 1;
    if(len <= 1) return NULL;
    char * dest = RebootManager_Allocate(len);
    strncpy(dest, src1, len1);
    dest[len1] = 0;
    strcat(dest, src2);
    dest[len - 1] = 0;
    return dest;
}
