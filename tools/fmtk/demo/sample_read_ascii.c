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
Function     : DemoReadAscii
Description  : Reads a character string from the eeprom
Input        : None
Output       : None
Return       : None
*******************************************************************************/
void DemoReadAscii()
{
    fmerrno Result = FM_API_ERR_CODE_EINVAL;
    FmSession* SessionHdl = NULL;
    unsigned Addr = 0x20;
    unsigned Length = 12;
    char AsciiData[225];
    memset ( AsciiData, 0, sizeof ( AsciiData ) );
 
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

    printf ( "Start to read ascii about slot 0.\n" );

    /* The address range is 0x20-0xff; The read length ranges is 1-224. */
    /* You can also use another slot to perform this operation */
    Result = FmAPI_ReadDataFromEeprom ( SessionHdl, 0, Addr, Length, AsciiData );
    if ( FM_API_SUCCESS != Result )
    {
         printf( "FPGA[0]API_ReadDataFromEeprom failed. err code: %d\n", Result );
    }
    else
    {
        printf ( "FPGA[0] Succeeded in reading the ascii!\n" );
        printf ( "The data is %s\n",AsciiData );
    }

    /* Other API call can be added here */
    
    /* stop sesssions; make sure all device mutex lock released */
    FmAPI_StopSession ( SessionHdl );

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
    DemoReadAscii();
    return 0;
}

