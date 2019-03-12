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
#include <stdlib.h>
#include <string.h>
#include "fmtkapi.h"

/*******************************************************************************
Function     : DemoResetFpga
Description  : Reset the FPGA chip.
Input        : None
Output       : None
Return       : None
*******************************************************************************/
void DemoResetFpga()
{
    fmerrno Result = FM_API_ERR_CODE_EINVAL;
    FmSession* SessionHdl = NULL;
    unsigned ResetMode = 0;
    unsigned char ResetResult = 0;
 
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

    printf ( "Start to reset the FPGA from the slot 0 mode 0.\n" );

    /* The ResetMode range is 0-2. */
    /* You can also use another slot to perform this operation */
    Result = FmAPI_ResetFpga ( SessionHdl, 0, ResetMode, &ResetResult );
    if ( FM_API_SUCCESS != Result )
    {
         printf ( "FPGA[0]API ResetFpga failed. err code: %d\n", Result );
    }
    else
    {
        switch ( ResetResult )
        {
            case 0:
                printf ( "FPGA[0]MCU did not receive a reset command\n" );
                break;
            case 1:
                printf ( "Resetting fpga from fpga[0] done successful!\n" );
                break;
            case 2:
                printf ( "Resetting fpga from fpga[0] done,but mcu is busy, please try again later.\n" );
                break;
            default:
                printf ( "FPGA[0] Recieving message unkown\n" );
                break;
        }
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
    DemoResetFpga();
    return 0;
}

