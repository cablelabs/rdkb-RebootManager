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

    module: cosa_apis_configuration.c

        For COSA Data Model Library Development

    -------------------------------------------------------------------

    copyright:

        Cisco Systems, Inc.
        All Rights Reserved.

    -------------------------------------------------------------------

    description:

        This file implementes back-end apis for the COSA Data Model Library

    -------------------------------------------------------------------

    environment:

        platform independent

    -------------------------------------------------------------------

    author:

        COSA XML TOOL CODE GENERATOR 1.0

    -------------------------------------------------------------------

    revision:

        11/16/2011    initial revision.

**************************************************************************/
#include "ansc_platform.h"
#include "cosa_apis_reboot.h"
#include "cosa_apis_configuration.h"
#include "ccsp_base_api.h"
#include "slap_definitions.h"
#include "slap_vco_exported_api.h"
#include "ccsp_psm_helper.h"
#include "rm_util.h"

/***********************************************************************
 IMPORTANT NOTE:

 According to TR69 spec:
 On successful receipt of a SetParameterValues RPC, the CPE MUST apply 
 the changes to all of the specified Parameters atomically. That is, either 
 all of the value changes are applied together, or none of the changes are 
 applied at all. In the latter case, the CPE MUST return a fault response 
 indicating the reason for the failure to apply the changes. 
 
 The CPE MUST NOT apply any of the specified changes without applying all 
 of them.

 In order to set parameter values correctly, the back-end is required to
 hold the updated values until "Validate" and "Commit" are called. Only after
 all the "Validate" passed in different objects, the "Commit" will be called.
 Otherwise, "Rollback" will be called instead.

 The sequence in COSA Data Model will be:

 SetParamBoolValue/SetParamIntValue/SetParamUlongValue/SetParamStringValue 
 or SetBulkParamValues
 -- Backup the updated values;

 if( Validate_XXX())
 {
     Commit_XXX();    -- Commit the update all together in the same object
 }
 else
 {
     Rollback_XXX();  -- Remove the update at backup;
 }
 
***********************************************************************/
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
#define RM_CONFIGURATION_PSM_NAME "com.cisco.spvtg.ccsp.rm.Configuration.Value."

RmObjectConfiguration rmConfig = {
    .pULongParaName = {"OngoingCriticalEventWaitTimer", "SystemRebootSignalTimer", "SoftwareWatchdogTimerInterval"},
	.ulULongParaValue = {DEFAUT_CRITICAL_WAIT_TIME,DEFAUT_REBOOT_SIGNAL_TIME,DEFAUT_WATCH_DOG_INTERVAL},
	.eULongParaState = {CAN_NEITHER,CAN_NEITHER,CAN_NEITHER}
};

/* This is an array to which the object name and parameter name can be directly concatenated. */
char                pConfigurationPsmRecordName[200] = RM_CONFIGURATION_PSM_NAME;
size_t              iConfigurationPsmRecordNameHeaderLen;
extern ANSC_HANDLE  bus_handle;
extern char         g_Subsystem[32];



/* 
   Try to initilize configuration parameters to saved values (if any) from PSM first.
   If unable to get values from PSM, We will use hard coded default values.
*/
void Configuration_Parameters_Init()
{
    char   *pValue = NULL;
    int    i;
    size_t len;
    iConfigurationPsmRecordNameHeaderLen = strlen(RM_CONFIGURATION_PSM_NAME);
    //printf("in Configuration_Parameters_Init %d\n", iConfigurationPsmRecordNameHeaderLen);

    for(i = 0; i<RM_CONFIGURATION_NumULongPara; i++)
	{
		//construct full pConfigurationPsmRecordName
        len = strlen(rmConfig.pULongParaName[i]);
        strncpy(&pConfigurationPsmRecordName[iConfigurationPsmRecordNameHeaderLen], rmConfig.pULongParaName[i], len);
        pConfigurationPsmRecordName[iConfigurationPsmRecordNameHeaderLen + len] = 0;

        pValue = NULL;
        int res = PSM_Get_Record_Value2(
            bus_handle,
            g_Subsystem,
            pConfigurationPsmRecordName,
            NULL,
            &pValue);
        if(res == CCSP_SUCCESS)
		{
            //printf("in Configuration_Parameters_Init psm read success 1: %d, %s, %s\n", res, pConfigurationPsmRecordName, pValue);
            rmConfig.ulULongParaValue[i] = _ansc_atoi(pValue);
            if(pValue) RebootManager_Free(pValue);
        }
        else
        {   
            //printf("In Configuration_Parameters_Init psm read failure 1: %d, %s, %s\n", res, pConfigurationPsmRecordName, pValue);
            if(pValue) RebootManager_Free(pValue);
			/*
            if(i == RM_CONFIGURATION_CriticalWaitTimeId)
			{
                rmConfig.ulULongParaValue[RM_CONFIGURATION_CriticalWaitTimeId] = DEFAUT_CRITICAL_WAIT_TIME;
            }
			else if(i == RM_CONFIGURATION_RebootSignalTimeId)
			{
                rmConfig.ulULongParaValue[RM_CONFIGURATION_RebootSignalTimeId] = DEFAUT_REBOOT_SIGNAL_TIME;
            }
			else //(i == RM_CONFIGURATION_WatchdogIntervalId)
			{
                rmConfig.ulULongParaValue[RM_CONFIGURATION_WatchdogIntervalId] = DEFAUT_WATCH_DOG_INTERVAL;
            }
			*/
            //printf("in Configuration_Parameters_Init psm read failure 2: %d, %s, %s\n", res, pConfigurationPsmRecordName, pValue);
        }
        rmConfig.eULongParaState[i] = CAN_NEITHER;
    }

    CcspTraceDebug(("Configuration init ends. \n"));
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Configuration_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Configuration_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Configuration_GetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int*                        pInt
            );

    description:

        This function is called to retrieve integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int*                        pInt
                The buffer of returned integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Configuration_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Configuration_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Configuration_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    /* check the parameter name and return the corresponding value */
    int i;
    for(i = 0; i<RM_CONFIGURATION_NumULongPara; i++){
        if( AnscEqualString(ParamName, rmConfig.pULongParaName[i], TRUE))
        {
            /* collect value */
            *puLong = rmConfig.ulULongParaValue[i];
            return TRUE;
        }
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Configuration_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
Configuration_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Configuration_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Configuration_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    /* check the parameter name and set the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Configuration_SetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int                         iValue
            );

    description:

        This function is called to set integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int                         iValue
                The updated integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Configuration_SetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int                         iValue
    )
{
    /* check the parameter name and set the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Configuration_SetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG                       uValue
            );

    description:

        This function is called to set ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG                       uValue
                The updated ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Configuration_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    )
{
    /* check the parameter name and set the corresponding value */
    int i;
    for(i=0; i <= RM_CONFIGURATION_NumULongPara; i++){
        if( AnscEqualString(ParamName, rmConfig.pULongParaName[i], TRUE))
        {
            /* save update to backup */
            rmConfig.ulULongParaValueBackup[i] = uValue;
            rmConfig.eULongParaState[i] = CAN_COMMIT;
            return TRUE;
        }
    }
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Configuration_SetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pString
            );

    description:

        This function is called to set string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pString
                The updated string value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Configuration_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
    /* check the parameter name and set the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Configuration_Validate
            (
                ANSC_HANDLE                 hInsContext,
                char*                       pReturnParamName,
                ULONG*                      puLength
            );

    description:

        This function is called to finally commit all the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       pReturnParamName,
                The buffer (128 bytes) of parameter name if there's a validation. 

                ULONG*                      puLength
                The output length of the param name. 

    return:     TRUE if there's no validation.

**********************************************************************/
BOOL
Configuration_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    )
{
    return TRUE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Configuration_Commit
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to finally commit all the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
Configuration_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    ULONG  ulValue;
    int    i;

    for(i = 0; i<RM_CONFIGURATION_NumULongPara; i++){
        if(rmConfig.eULongParaState[i] != CAN_COMMIT) {
            rmConfig.eULongParaState[i] = CAN_NEITHER;
            continue;
        }
        ulValue = rmConfig.ulULongParaValue[i];
        rmConfig.ulULongParaValue[i] = rmConfig.ulULongParaValueBackup[i];
        rmConfig.ulULongParaValueBackup[i] = ulValue;
        rmConfig.eULongParaState[i] = CAN_ROLLBACK;

        int res = Configuration_SavePsmValueRecord(i);
        if(res != CCSP_SUCCESS){
			CcspTraceError(("Commit save psm failures 2\n"));
            return -1;
        }
    }
	//do this only after all prameters are committed succesfully (obsolutely no need to rollback)
    for(i = 0; i<RM_CONFIGURATION_NumULongPara; i++){
        rmConfig.eULongParaState[i] = CAN_NEITHER;
    }

    return 0;
}
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Configuration_Rollback
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to roll back the update whenever there's a 
        validation found.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
Configuration_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    int i;

    for(i = 0; i<RM_CONFIGURATION_NumULongPara; i++)
	{

        if(rmConfig.eULongParaState[i] != CAN_ROLLBACK) 
		{
            rmConfig.eULongParaState[i] = CAN_NEITHER;
            continue;
        }

        rmConfig.ulULongParaValue[i] = rmConfig.ulULongParaValueBackup[i];

        int res = Configuration_SavePsmValueRecord(i);

        if(res != CCSP_SUCCESS)
		{
            return -1;
        }

        rmConfig.eULongParaState[i] = CAN_NEITHER;
    }
    return 0;
}

int
Configuration_SavePsmValueRecord
(
    int paraId
)
{
    size_t len;
    len = strlen(rmConfig.pULongParaName[paraId]);
    strncpy(&pConfigurationPsmRecordName[iConfigurationPsmRecordNameHeaderLen], rmConfig.pULongParaName[paraId], len);
    pConfigurationPsmRecordName[iConfigurationPsmRecordNameHeaderLen + len] = 0;
    char val[50] = {0};
    _ansc_itoa(rmConfig.ulULongParaValue[paraId], val, 10);
    int res = PSM_Set_Record_Value2(
        bus_handle,
        g_Subsystem,
        pConfigurationPsmRecordName,
        ccsp_string,
        val);
    if(res != CCSP_SUCCESS){
        //CcspTraceError("PSM write failure %d!=%d: %s----%s.\n", res, CCSP_SUCCESS, pConfigurationPsmRecordName, val); 
        CcspTraceError(("PSM write failure %d!=%d: %s----%s.\n", res, CCSP_SUCCESS, pConfigurationPsmRecordName, val));         
		return -1;
    }
}