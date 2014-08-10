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

    module:

        CcspRmHal.c

    -------------------------------------------------------------------

    copyright:

        Cisco Systems, Inc.
        All Rights Reserved.

    -------------------------------------------------------------------

    description:

        stub out for rdk-b

    -------------------------------------------------------------------

    author: WSN


    -------------------------------------------------------------------

    revision:

        07/07/2014    initial revision.

**************************************************************************/

#include "ansc_platform.h"
#include "rm_util.h"
#include "cosa_apis_reboot.h"
#include "cosa_apis_configuration.h"

int CcspRmHal_PostRebootProcedures()
{
	CcspTraceDebug(("Post Reboot procedures are called here ....\n"));
	/*
	 * TO BE FILLED IN BY PRODUCT INSTATIATION: 
	 
	   Requirement CCSP_RM_5 (Post Reboot procedures)
	   The reboot Manager provides a framework for handling any post reboot 
	   procedures required by specific product instantiations. The actual 
	   post reboot procedures are provided by the product instantiation. 
	   The reboot manager calls into a component specific HAL layer after 
	   a successful reboot indicated by the “system ready” signal delivered 
	   by Component Registrar.  
	 *
     *
    system("reboot");
	 */
	return 0;
}


