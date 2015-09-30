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

        *   ssp_create
        *   ssp_engage
        *   ssp_cancel
        *   ssp_CcdIfGetComponentName
        *   ssp_CcdIfGetComponentVersion
        *   ssp_CcdIfGetComponentAuthor
        *   ssp_CcdIfGetComponentHealth
        *   ssp_CcdIfGetComponentState
        *   ssp_CcdIfGetLoggingEnabled
        *   ssp_CcdIfSetLoggingEnabled
        *   ssp_CcdIfGetLoggingLevel
        *   ssp_CcdIfSetLoggingLevel
        *   ssp_CcdIfGetMemMaxUsage
        *   ssp_CcdIfGetMemMinUsage
        *   ssp_CcdIfGetMemConsumed

  ------------------------------------------------------------------------------

    revision:

        09/08/2011    initial revision.

**********************************************************************************/
#if defined(_COSA_DRG_CNS_) || defined(_COSA_DRG_TPG_)
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>   
#include <unistd.h>
#include <termios.h> /* for tcdrain, optional */
#include <stdlib.h>
#include <arpa/inet.h>
#include <sysexits.h>

#define SECTORSZ  512 /* bootloader defined */

/*#ifndef _IOW
# define _IOW(x,y,z)    (((x)<<8)|y) // This definition actually does not work. It has to be defined in previous header files.
#endif*/

#define kMagic_Bootldr                    150
#define kDld_Ioctl_Magic        kMagic_Bootldr

typedef struct {
        char    *device;                /* NULL terminated device name where
                                         * the image resides */
        size_t  imageSize;              /* Size of the image resident on the
                                         * drive */
} tDld_CodeDiskUpdate;

#define kDld_Ioctl_CodeDiskUpdateGW     _IOW(kDld_Ioctl_Magic, 17, \
                                                tDld_CodeDiskUpdate*)

#endif

#include "ssp_global.h"
#include "plugin_main.h"
#include "cosa_apis_configuration.h"
#include "cosa_apis_reboot.h"
#include "ccsp_cr_definitions.h"
#include "rm_util.h"

PDSLH_CPE_CONTROLLER_OBJECT     pDslhCpeController      = NULL;
PCOMPONENT_COMMON_RM           g_pComponent_COMMON_Rm   = NULL;
PCCSP_CCD_INTERFACE             pRmCcdIf                = (PCCSP_CCD_INTERFACE        )NULL;
PDSLH_LCB_INTERFACE             pDslhLcbIf              = (PDSLH_LCB_INTERFACE        )NULL;

#define  CCSP_DATAMODEL_XML_FILE           "RebootManager.xml"

extern char *pComponentName;
char *pComponentDataPath = CCSP_COMPONENT_PATH_RM;
extern char                                g_Subsystem[32];
extern  ANSC_HANDLE                        bus_handle;
extern  ULONG                              g_ulAllocatedSizePeak;
extern RmObjectConfiguration rmConfig;
extern int     BackEndInitDone;
BOOL bOngingCriticalEvent = FALSE;
BOOL bRebootEnabled = FALSE;
BOOL bRebootEnabledtemp = FALSE;
char sRebootStatus[REBOOT_STATUS_STRING_MAX_SIZE] = "No Reboot Request Received";
char paramToWatchFor[256] = { '\0' };

ANSC_STATUS
ssp_create
    (
    )
{
    /* Create component common data model object */

    g_pComponent_COMMON_Rm = (PCOMPONENT_COMMON_RM)AnscAllocateMemory(sizeof(COMPONENT_COMMON_RM));

    if ( !g_pComponent_COMMON_Rm )
    {
        return ANSC_STATUS_RESOURCES;
    }

    ComponentCommonDmInit(g_pComponent_COMMON_Rm);

    g_pComponent_COMMON_Rm->Name     = AnscCloneString(CCSP_COMPONENT_NAME_RM);
    g_pComponent_COMMON_Rm->Version  = 1;
    g_pComponent_COMMON_Rm->Author   = AnscCloneString("Qi Zeng");

    /* Create ComponentCommonDatamodel interface*/
    if ( !pRmCcdIf )
    {
        pRmCcdIf = (PCCSP_CCD_INTERFACE)AnscAllocateMemory(sizeof(CCSP_CCD_INTERFACE));

        if ( !pRmCcdIf )
        {
            return ANSC_STATUS_RESOURCES;
        }
        else
        {
            AnscCopyString(pRmCcdIf->Name, CCSP_CCD_INTERFACE_NAME);

            pRmCcdIf->InterfaceId              = CCSP_CCD_INTERFACE_ID;
            pRmCcdIf->hOwnerContext            = NULL;
            pRmCcdIf->Size                     = sizeof(CCSP_CCD_INTERFACE);

            pRmCcdIf->GetComponentName         = ssp_CcdIfGetComponentName;
            pRmCcdIf->GetComponentVersion      = ssp_CcdIfGetComponentVersion;
            pRmCcdIf->GetComponentAuthor       = ssp_CcdIfGetComponentAuthor;
            pRmCcdIf->GetComponentHealth       = ssp_CcdIfGetComponentHealth;
            pRmCcdIf->GetComponentState        = ssp_CcdIfGetComponentState;
            pRmCcdIf->GetLoggingEnabled        = ssp_CcdIfGetLoggingEnabled;
            pRmCcdIf->SetLoggingEnabled        = ssp_CcdIfSetLoggingEnabled;
            pRmCcdIf->GetLoggingLevel          = ssp_CcdIfGetLoggingLevel;
            pRmCcdIf->SetLoggingLevel          = ssp_CcdIfSetLoggingLevel;
            pRmCcdIf->GetMemMaxUsage           = ssp_CcdIfGetMemMaxUsage;
            pRmCcdIf->GetMemMinUsage           = ssp_CcdIfGetMemMinUsage;
            pRmCcdIf->GetMemConsumed           = ssp_CcdIfGetMemConsumed;
            pRmCcdIf->ApplyChanges             = ssp_CcdIfApplyChanges;
        }
    }

    /* Create ComponentCommonDatamodel interface*/
    if ( !pDslhLcbIf )
    {
        pDslhLcbIf = (PDSLH_LCB_INTERFACE)AnscAllocateMemory(sizeof(DSLH_LCB_INTERFACE));

        if ( !pDslhLcbIf )
        {
            return ANSC_STATUS_RESOURCES;
        }
        else
        {
            AnscCopyString(pDslhLcbIf->Name, CCSP_LIBCBK_INTERFACE_NAME);

            pDslhLcbIf->InterfaceId              = CCSP_LIBCBK_INTERFACE_ID;
            pDslhLcbIf->hOwnerContext            = NULL;
            pDslhLcbIf->Size                     = sizeof(DSLH_LCB_INTERFACE);

            pDslhLcbIf->InitLibrary              = COSA_Init;
        }
    }

    pDslhCpeController = DslhCreateCpeController(NULL, NULL, NULL);

    if ( !pDslhCpeController )
    {
        CcspTraceError(("CANNOT Create pDslhCpeController... Exit!\n"));

        return ANSC_STATUS_RESOURCES;
    }

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
ssp_engage
    (
    )
{
    ANSC_STATUS                     returnStatus                = ANSC_STATUS_SUCCESS;
    PCCC_MBI_INTERFACE              pRmMbiIf                   = (PCCC_MBI_INTERFACE)MsgHelper_CreateCcdMbiIf((void*)bus_handle, g_Subsystem);
    char                            CrName[256] = {0};

    g_pComponent_COMMON_Rm->Health = CCSP_COMMON_COMPONENT_HEALTH_Yellow;

    /* data model configuration */
    pDslhCpeController->AddInterface((ANSC_HANDLE)pDslhCpeController, (ANSC_HANDLE)pDslhLcbIf);
    pDslhCpeController->AddInterface((ANSC_HANDLE)pDslhCpeController, (ANSC_HANDLE)pRmMbiIf);
    pDslhCpeController->AddInterface((ANSC_HANDLE)pDslhCpeController, (ANSC_HANDLE)pRmCcdIf);
    pDslhCpeController->SetDbusHandle((ANSC_HANDLE)pDslhCpeController, (ANSC_HANDLE)bus_handle);
    pDslhCpeController->Engage((ANSC_HANDLE)pDslhCpeController);

    if ( g_Subsystem[0] != 0 )
    {
        _ansc_sprintf(CrName, "%s%s", g_Subsystem, CCSP_DBUS_INTERFACE_CR);
    }
    else
    {
        _ansc_sprintf(CrName, "%s", CCSP_DBUS_INTERFACE_CR);
    }

    returnStatus =
        pDslhCpeController->RegisterCcspDataModel
            (
                (ANSC_HANDLE)pDslhCpeController,
                CrName,              /* CCSP CR ID */
                CCSP_DATAMODEL_XML_FILE,             /* Data Model XML file. Can be empty if only base data model supported. */
                CCSP_COMPONENT_NAME_RM,            /* Component Name    */
                CCSP_COMPONENT_VERSION_RM,         /* Component Version */
                CCSP_COMPONENT_PATH_RM,            /* Component Path    */
                g_Subsystem /* Component Prefix  */
            );

    if ( returnStatus == ANSC_STATUS_SUCCESS )
    {
        /* System is fully initialized */
        g_pComponent_COMMON_Rm->Health = CCSP_COMMON_COMPONENT_HEALTH_Green;
    }

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
ssp_cancel
    (
    )
{
    int                             nRet  = 0;
    char                            CrName[256];
    char                            CpName[256];

    if( g_pComponent_COMMON_Rm == NULL)
    {
        return ANSC_STATUS_SUCCESS;
    }

    if ( g_Subsystem[0] != 0 )
    {
        _ansc_sprintf(CrName, "%s%s", g_Subsystem, CCSP_DBUS_INTERFACE_CR);
        _ansc_sprintf(CpName, "%s%s", g_Subsystem, CCSP_COMPONENT_NAME_RM);
    }
    else
    {
        _ansc_sprintf(CrName, "%s", CCSP_DBUS_INTERFACE_CR);
        _ansc_sprintf(CpName, "%s", CCSP_COMPONENT_NAME_RM);
    }
    
    /* unregister component */
    nRet = CcspBaseIf_unregisterComponent(bus_handle, CrName, CpName );  
    CcspTraceDebug(("unregisterComponent returns %d\n", nRet));

    pDslhCpeController->Cancel((ANSC_HANDLE)pDslhCpeController);
    AnscFreeMemory(pDslhCpeController);

    if ( pRmCcdIf ) AnscFreeMemory(pRmCcdIf);
    if ( g_pComponent_COMMON_Rm ) AnscFreeMemory(g_pComponent_COMMON_Rm);

    g_pComponent_COMMON_Rm = NULL;
    pRmCcdIf                = NULL;
    pDslhCpeController       = NULL;

    return ANSC_STATUS_SUCCESS;
}

int monitorRebootRequest()
{
    while(!BackEndInitDone)
    {
        AnscSleep(50);
		CcspTraceDebug(("Initiating backend....\n"));
#if defined(_COSA_VEN501_)
        /* 
         * XXX: This is a hacking in ven501.
         *    I think used the global symbol form components is bad idea, 
         *    especially we have a lot of same symbols in the each components.   
         */
        BackEndInitDone = 1;
#endif
    }

    while(!bRebootEnabled)
    {
        AnscSleep(5000);
		CcspTraceDebug(("No Component Has Requested System Reboot yet.\n"));
    }

    return (GracefullyRebootSystem());
}

int GracefullyRebootSystem()
{   
    //Hard coded for now. TO DO: Read subsystem list form a platform profile
    char *pSubSysPrefixList[] = {"eRT.", "eMG.", END_OF_LIST};

    //char sObjName[29] = "Device.DeviceInfo.";
    char sObjName[29] = "RebootService.BlockingEvent.";
    componentStruct_t ** ppComponents = NULL;
    parameterValStruct_t **ppParamVal = NULL;
    int ret   = 0;
    int size  = 0;
    int size2 = 0;
    int i = 0;
    int j = 0;
    int k = 0;
    int loop_cnt = 0;
    int compnts_cnt = 0;
    char *pAllCompnts[128] = {NULL};
    char *pAllBusPaths[128] = {NULL};
    char * pFullNames[128][1] = {{NULL}};    
    parameterValStruct_t strValStruct[128][1];
    char *pPartialName  = NULL;
    char * pFaultParameter  = NULL;
    char cr_id[256];
    _ansc_sprintf(cr_id, "%s%s", g_Subsystem, CCSP_DBUS_INTERFACE_CR);
    parameterAttributeStruct_t attriStruct;
    attriStruct.parameterName = NULL;
    attriStruct.notificationChanged = 1;
    attriStruct.accessControlChanged = 0;



    AnscCopyString(sRebootStatus, "Checking for Ongoing Critical Event(s)...");
    CcspTraceDebug(("Reboot Status: %s\n", sRebootStatus));

    for(i=0; FALSE == AnscEqualString(pSubSysPrefixList[i], END_OF_LIST, TRUE);i++)
    {
        ret = CcspBaseIf_discComponentSupportingNamespace 
            (
                bus_handle,
                cr_id,
                sObjName,
                pSubSysPrefixList[i],
                &ppComponents,
                &size
            );

        CcspTraceDebug(("Reboot Manager: Called CcspBaseIf_discComponentSupportingNamespace with subsys: %s!!!\n", pSubSysPrefixList[i]));        

        if ( CCSP_SUCCESS != ret) 
        {
            CcspTraceError(("Reboot Manager: CcspBaseIf_discComponentSupportingNamespace failed!!!\n"));
            size = 0;
        } 
            CcspTraceDebug(("There are %d components might be processing critical events!!!\n", size));

        for(j=0;j<size;j++)
        {
            pAllCompnts[compnts_cnt] = RebootManager_CloneString(ppComponents[j]->componentName);
            pAllBusPaths[compnts_cnt] = RebootManager_CloneString(ppComponents[j]->dbusPath);
            pPartialName = RebootManager_MergeString(sObjName,pAllCompnts[compnts_cnt]);
            pFullNames[compnts_cnt][0]= RebootManager_MergeString(pPartialName,".OngoingCriticalEvent");
            strValStruct[compnts_cnt][0].parameterName  = RebootManager_MergeString(pPartialName, ".AllowCriticalEvents");
            strValStruct[compnts_cnt][0].type  = ccsp_boolean;

            CcspTraceDebug(("Reboot Manager found blocking component: %s\n", pAllCompnts[compnts_cnt]));
            CcspTraceDebug(("Bus Path for blocking component: %s\n", pAllBusPaths[compnts_cnt]));
            CcspTraceDebug(("Namespace for OngoingCriticalEvent: %s\n", pFullNames[compnts_cnt][0]));
            CcspTraceDebug(("Namespace for AllowCriticalEvents: %s\n", strValStruct[compnts_cnt][0].parameterName));

            if(pPartialName) RebootManager_Free(pPartialName);
            compnts_cnt++;
        }

        free_componentStruct_t(bus_handle, size, ppComponents); 
        size = 0;
    }

    for(i=0; i<compnts_cnt; i++)
    {
        CcspTraceDebug(("Reboot Manager is in loop number: %d\n", i));

        //Disables the component from accepting new critical events by setting 
        //RebootService.BlockingEvent.<Component Name>.AllowCriticalEvents parameter to false
        if(strValStruct[i][0].parameterValue) RebootManager_Free(strValStruct[i][0].parameterValue);
        strValStruct[i][0].parameterValue = AnscCloneString("false");

        ret = CCSP_FAILURE;
        for(j=0; j < 10; j++)
        {
        //Per Jian Wu: The parameter RM try to write will only write by RM, so we 
        //probably don’t need a separate write ID for RM. RM will try to write it 
        //anyway until it success.
            ret = CcspBaseIf_setParameterValues(
                    bus_handle,
                    pAllCompnts[i],
                    pAllBusPaths[i],
                    0, 
                    0xFFFFFFFF,
                    strValStruct[i],
                    1,//set strValStruct[i][0]
                    1,//commit = TRUE
                    &pFaultParameter
            );
        
        CcspTraceDebug(("Disable AllowCriticalEvents returned: %d\n", ret));

            if (pFaultParameter)AnscFreeMemory(pFaultParameter);

            if (ret == CCSP_SUCCESS)
                break;

            sleep(1);
        }
        
        if (j == 10) 
            continue;  //skip non-responding component

        //Checks if the component is processing an ongoing critical event -  
        //RebootService.BlockingEvent. <Component Name>.OngoingCriticalEvent = TRUE
        ret = CCSP_FAILURE;
        for(j=0; j < 10; j++)
        {
            ret = CcspBaseIf_getParameterValues(
                bus_handle,
                pAllCompnts[i],
                pAllBusPaths[i],
                pFullNames[i],
                1, //get value of pFullNames[i][0]
                &size2,
                &ppParamVal
            );

            if (ret == CCSP_SUCCESS)
                break;

            free_parameterValStruct_t (bus_handle, size2, ppParamVal);    
            sleep(1);
        }

        if (j == 10) 
        {
            continue;  //skip non-responding component
        }

        bOngingCriticalEvent = AnscEqualString(ppParamVal[0]->parameterValue,"true", TRUE);

        free_parameterValStruct_t (bus_handle, size2, ppParamVal);    

        if (bOngingCriticalEvent == FALSE)
        {
            continue;  //no OngoingCriticalEvent, iterate to next component
        }

        //_ansc_sprintf(paramToWatchFor, "%s",  pFullNames[i]);
        //
        AnscCopyString(paramToWatchFor, pFullNames[i]);

        //Turns on notification on the parameter RebootService.BlockingEvent.<Component Name>.OngoingCriticalEvent. 
        //This allows the Reboot Manager to be notified whenever the critical event is over.
        ret = CcspBaseIf_Register_Event(
                bus_handle, 
                pAllCompnts[i], 
                "parameterValueChangeSignal"
            );      
        
        if ( CCSP_SUCCESS != ret) 
        {
            printf("Reboot Manager: CcspBaseIf_Register_Event failed!!!\n");
        } 

        CcspBaseIf_SetCallback2
            (
                bus_handle,
                "parameterValueChangeSignal",
                CcspRebootManagerValueChangedCB,
                NULL
            );

        attriStruct.parameterName = pFullNames[i];
        attriStruct.notification = 2;  //use active notification (2) instead of passive notification (1)
        ret = CcspBaseIf_setParameterAttributes(
            bus_handle,
            pAllCompnts[i],
            pAllBusPaths[i],
            0,
            &attriStruct,
            1
        );
        
        if ( CCSP_SUCCESS != ret) 
        {
            CcspTraceDebug(("CcspBaseIf_setParameterAttributes (turn notification on) failed!!!\n"));
        } 


        //Re-Enable component(s) to accept new critical events by 
        //setting RebootService.BlockingEvent.<Component Name>.AllowCriticalEvents 
        //parameter back to true. 
        for(j=0;j<=i;j++)
        {
            if(strValStruct[j][0].parameterValue) RebootManager_Free(strValStruct[j][0].parameterValue);
            strValStruct[j][0].parameterValue = AnscCloneString("true");

            ret = CcspBaseIf_setParameterValues(
                    bus_handle,
                    pAllCompnts[j],
                    pAllBusPaths[j],
                    0, 
                    0xFFFFFFFF,
                    strValStruct[j],
                    1,//set strValStruct[i][0]
                    1,//commit = TRUE
                    &pFaultParameter
            );

            if (pFaultParameter)AnscFreeMemory(pFaultParameter);

        }

        _ansc_sprintf(sRebootStatus, "%s%s%s", "Waiting for ", pAllCompnts[i], "to finish processing critical event(s)");
        CcspTraceDebug(("Reboot Status: %s\n", sRebootStatus));

        k = 0; //setup wait timer
        loop_cnt = (rmConfig.ulULongParaValue[RM_CONFIGURATION_CriticalWaitTimeId] * 1000)/50;

        //Wait until the notification is received or the timer expires 
        //CcspRebootManagerValueChangedCB will set  bOngingCriticalEvent 
        //to false when value change notification received
        while(bOngingCriticalEvent && k < loop_cnt )
        {
            AnscSleep(50);
            k++;
        }

        AnscCopyString(sRebootStatus, "Continue to Checking for more Ongoing Critical Event(s)...");
        CcspTraceDebug(("Reboot Status: %s\n", sRebootStatus));

        //Turn off notification and move onto next component
        ret = CcspBaseIf_UnRegister_Event(
                bus_handle, 
                pAllCompnts[i], 
                "parameterValueChangeSignal"
            );      

        if ( CCSP_SUCCESS != ret) 
        {
            CcspTraceDebug(("CcspBaseIf_UnRegister_Event failed!!!\n"));
        } 

        attriStruct.notification = 0;  
        ret = CcspBaseIf_setParameterAttributes(
            bus_handle,
            pAllCompnts[i],
            pAllBusPaths[i],
            0,
            &attriStruct,
            1
        );
        
        if ( CCSP_SUCCESS != ret) 
        {
            CcspTraceDebug(("CcspBaseIf_setParameterAttributes (turn notification off) failed!!!\n"));
        } 
        
        //loop though all components again until no OngoingCriticalEvent(s) found
        i = -1;

    }

    //Clear to reboot. Free all pointers that are no longer needed
    for(i=0; i<compnts_cnt; i++)
    {
        if(pAllCompnts[i]) RebootManager_Free(pAllCompnts[i]);
        if(pAllBusPaths[i]) RebootManager_Free(pAllBusPaths[i]);
        if(pFullNames[i][0]) RebootManager_Free(pFullNames[i][0]);
        if(strValStruct[i][0].parameterName) RebootManager_Free(strValStruct[i][0].parameterName);
        if(strValStruct[i][0].parameterValue) RebootManager_Free(strValStruct[i][0].parameterValue);
    }

    AnscCopyString(sRebootStatus, "Now It is Safe to Reset System. Sending SystemRebootSignal to All Components...");
    CcspTraceDebug(("Reboot Status: %s\n", sRebootStatus));

    //Inform other components that system is going down...
    if ( CcspBaseIf_SendsystemRebootSignal(bus_handle) != CCSP_SUCCESS )
    {
        CcspTraceError(("CcspBaseIf_SendsystemRebootSignal failed!!!\n"));
        return ANSC_STATUS_FAILURE;
    }

    //Allows all components to persist any necessary state and gracefully shut down  
    sleep(rmConfig.ulULongParaValue[RM_CONFIGURATION_RebootSignalTimeId]);
    
    AnscCopyString(sRebootStatus, "Rebooting now...");
    CcspTraceDebug(("Reboot Status: %s\n", sRebootStatus));

    ResetSystem();
    return ANSC_STATUS_SUCCESS;//should never get to here

}

    /* 
     * Product Team can customize this routine to reset more complicated  
     * hardware platform.
     */
void ResetSystem()
{
#if defined(_COSA_DRG_CNS_)
    /* 
     * reboot the box 
     * TODO: The "reboot" shell command is currently broken.  Use it when fixed.
     */
    CcspTraceDebug(("Rebooting...\n"));
    fflush(NULL);
    tcdrain(STDOUT_FILENO);
    tcdrain(STDERR_FILENO);
    
    /* and for good measure */
    sleep(1);

    /* now make it so */
    system("echo reset > /proc/watchdog");
#elif defined(_COSA_E4200_)
    CcspTraceDebug(("Rebooting...\n"));
    Utopia_Reboot();
#elif defined(_COSA_VEN501_)
    /* XXX: Now used system is daring */
    CcspTraceDebug(("Rebooting...\n"));
    printf("Rebooting...\n");
    system("reboot");
#else
    int i;

    for (i=0; i< 20; i++)
    {   //not going actually reset a PC simulation or a TPG board
        CcspTraceDebug(("Reset system here....\n"));
        printf("Reset system here.... %d\n", __LINE__);
    }
    system("reboot");
#endif
}

void
CcspRebootManagerValueChangedCB
    (
        parameterSigStruct_t*       val,
        int                         size,
        void*                       user_data
    )
{
    if (size == 1 && 
        AnscEqualString(val->parameterName, paramToWatchFor, TRUE) &&
        AnscEqualString(val->newValue, "false", TRUE)
        )
    {
        bOngingCriticalEvent = FALSE;
    }
}

char*
ssp_CcdIfGetComponentName
    (
        ANSC_HANDLE                     hThisObject
    )
{
    return g_pComponent_COMMON_Rm->Name;
}


ULONG
ssp_CcdIfGetComponentVersion
    (
        ANSC_HANDLE                     hThisObject
    )
{
    return g_pComponent_COMMON_Rm->Version;
}


char*
ssp_CcdIfGetComponentAuthor
    (
        ANSC_HANDLE                     hThisObject
    )
{
    return g_pComponent_COMMON_Rm->Author;
}


ULONG
ssp_CcdIfGetComponentHealth
    (
        ANSC_HANDLE                     hThisObject
    )
{
    return g_pComponent_COMMON_Rm->Health;
}


ULONG
ssp_CcdIfGetComponentState
    (
        ANSC_HANDLE                     hThisObject
    )
{
    return g_pComponent_COMMON_Rm->State;
}



BOOL
ssp_CcdIfGetLoggingEnabled
    (
        ANSC_HANDLE                     hThisObject
    )
{
    return g_pComponent_COMMON_Rm->LogEnable;
}

ANSC_STATUS
ssp_CcdIfSetLoggingEnabled
    (
        ANSC_HANDLE                     hThisObject,
        BOOL                            bEnabled
    )
{
    if(g_pComponent_COMMON_Rm->LogEnable == bEnabled) return ANSC_STATUS_SUCCESS;
    g_pComponent_COMMON_Rm->LogEnable = bEnabled;

    if (!bEnabled)
        AnscSetTraceLevel(CCSP_TRACE_INVALID_LEVEL);
    else
        AnscSetTraceLevel(g_pComponent_COMMON_Rm->LogLevel);

    return ANSC_STATUS_SUCCESS;
}


ULONG
ssp_CcdIfGetLoggingLevel
    (
        ANSC_HANDLE                     hThisObject
    )
{
    return g_pComponent_COMMON_Rm->LogLevel;
}


ANSC_STATUS
ssp_CcdIfSetLoggingLevel
    (
        ANSC_HANDLE                     hThisObject,
        ULONG                           LogLevel
    )
{
    if(g_pComponent_COMMON_Rm->LogLevel == LogLevel) return ANSC_STATUS_SUCCESS;
    g_pComponent_COMMON_Rm->LogLevel = LogLevel;

    if (g_pComponent_COMMON_Rm->LogEnable)
        AnscSetTraceLevel(LogLevel);        

    return ANSC_STATUS_SUCCESS;
}


ULONG
ssp_CcdIfGetMemMaxUsage
    (
        ANSC_HANDLE                     hThisObject
    )
{
    return g_ulAllocatedSizePeak;
}


ULONG
ssp_CcdIfGetMemMinUsage
    (
        ANSC_HANDLE                     hThisObject
    )
{
    return g_pComponent_COMMON_Rm->MemMinUsage;
}


ULONG
ssp_CcdIfGetMemConsumed
    (
        ANSC_HANDLE                     hThisObject
    )
{
    LONG             size = 0;

    size = AnscGetComponentMemorySize(CCSP_COMPONENT_NAME_RM);
    if (size == -1 )
        size = 0;

    return size;
}


ANSC_STATUS
ssp_CcdIfApplyChanges
    (
        ANSC_HANDLE                     hThisObject
    )
{
    ANSC_STATUS                         returnStatus    = ANSC_STATUS_SUCCESS;

    /* Assume the parameter settings are committed immediately. */    
    /*AnscSetTraceLevel((INT)g_pComponent_COMMON_Rm->LogLevel); */
    return returnStatus;
}
