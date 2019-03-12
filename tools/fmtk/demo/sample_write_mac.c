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

/*******************************************************************************
Function     : DemoWriteMac
Description  : Write mac to eeprom
Input        : None
Output       : None
Return       : None
*******************************************************************************/
#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fmtkapi.h"

void DemoWriteMac()
{
    fmerrno Result = FM_API_ERR_CODE_EINVAL;
    FmSession* SessionHdl = NULL;
    unsigned MacIndex = 0;
    char MacData[] = { "11:22:33:44:aa:ff" };
 
    Result  = FmAPI_Initialize ( SELECTED_API_VERSION, SELECTED_API_SUB_VER );
    if ( FM_API_SUCCESS != Result )
    {
        printf ( "Failed to initialize the fmtk.\n" );
        return;
    }

    printf ( "FMTK api Initialized.\n" );

    /* Obtain information about FPGA cards in the current environment */
    FmAPI_StartSession ( &SessionHdl );
    
    if ( 0 == ( SessionHdl->PfNum ) )
    {
        printf ( "No fpga devices!\n" );
        FmAPI_StopSession(SessionHdl);
        FmAPI_Unload();
        return;
    }
    else
    {
        printf ( "%u fpga devices founded!\n", ( SessionHdl->PfNum ));
    }

    printf ( "Start to write the MAC address in the slot 0 index 0.\n" );

    /* The MacIndex range is 0-1. */
    /* You can also use another slot to perform this operation */
    Result = FmAPI_WriteHexMacFromEeprom ( SessionHdl, 0, MacIndex, MacData );
    if ( FM_API_SUCCESS != Result )
    {
         printf ( "FPGA[0]API WriteHexMacFromEeprom failed. err code: %d\n", Result );
    }
    else
    {
        printf ( "FPGA[0] Succeeded in writing the MAC address.\n" );
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
    DemoWriteMac();
    return 0;
}

