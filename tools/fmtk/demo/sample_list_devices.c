/*-
 *   BSD LICENSE
 *
 *   Copyright(c)  2019 Huawei Technologies Co., Ltd. All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *     * Neither the name of Huawei Technologies Co., Ltd  nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include<stdio.h>
#include "fmtkapi.h"

/*******************************************************************************
Function     : DemoDeviceList
Description  : List all the device
Input        : None
Output       : None
Return       : None
*******************************************************************************/
void DemoDeviceList()
{
    fmerrno Result = FM_API_ERR_CODE_EINVAL;
    FmSession* SessionHdl = NULL;
    unsigned Count = 0;
    FM_IDENTIFIERS_DATA ids = {0};
    FM_FIRMWARES_DATA fmw_ids = {0};
    int i = 0;
 
    Result  = FmAPI_Initialize(SELECTED_API_VERSION, SELECTED_API_SUB_VER);
    if (FM_API_SUCCESS != Result)
    {
        printf("Initialization failed.\n");
        return;
    }

    printf("FMTK api Initialized.\n");

    FmAPI_StartSession(&SessionHdl);

    if ( 0 == ( SessionHdl->PfNum ) )
    {
        printf ( "No fpga devices!\n" );
        FmAPI_StopSession ( SessionHdl );
        FmAPI_Unload();
        return;
    }
    else
    {
        printf ("%u fpga devices founded!\n", ( SessionHdl->PfNum ));
    }

    printf ( "Start to query the basic information about all FPGA in the current environment.\n" );
    
    Count = SessionHdl->PfNum;
    
    for ( i = 0; i < Count; i ++)
    {
        Result = FmAPI_GetDeviceIdentifiers(SessionHdl, SessionHdl->Slot[i], &ids);
        fprintf(stdout, "*************** identifier INFO ***************\n");  
        fprintf(stdout, "FPGA:               (%d) \n",      SessionHdl->Slot[i]); 
        fprintf(stdout, "pciBusId:           (%s) \n",      ids.pciBusId);
        fprintf(stdout, "pciVenderId:        (0x%x) \n",       ids.pciVenderId);
        fprintf(stdout, "pciSubVenderId:     (0x%x) \n",    ids.pciSubVenderId);
        fprintf(stdout, "pciDeviceId:        (0x%x) \n",    ids.pciDeviceId);
        fprintf(stdout, "pciSubDeviceId:     (0x%x) \n", ids.pciSubDeviceId);

        Result = FmAPI_GetDeviceFirmwares(SessionHdl, SessionHdl->Slot[i], &fmw_ids);
        fprintf(stdout, "\n*************** firmwares INFO ***************\n");
        fprintf(stdout, "libraryDataVer:     (0x%x) \n",    fmw_ids.version);
        fprintf(stdout, "firmType            (0x%x) (1 = GOLDEN, 2 = SHELL)\n", fmw_ids.curfirmType);
        fprintf(stdout, "curfirmVer:         (0x%x) \n",    fmw_ids.curfirmVer);
        fprintf(stdout, "releaseDate:        (0x%x) \n",    fmw_ids.releaseDate);
        fprintf(stdout, "firmStatus:         (%d)(1 = DISABLED, 2 = ENABLED) \n",      fmw_ids.firmStatus);
        fprintf(stdout, "\n\n");    

    }

    /* Other API call can be added here */
    
    /* stop sesssions; make sure all device mutex lock released */
    FmAPI_StopSession(SessionHdl);

    /* check sessions and release all resources */
    FmAPI_Unload();
    
    
    return;    
}

/*******************************************************************************
Function     : main
Description  : Main function entry
Input        : None
Output       : None
Return       : 0:sucess other:fail
*******************************************************************************/
int main()
{
    DemoDeviceList();
    return 0;
}
