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

#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "fmtkapi.h"

typedef struct { int tid ;int slot ; FmSession* SessionPtr; }  thr_data_type;
const char* filepath = "IT21MPFA_MCU_V031.bin";

/*******************************************************************************
Function     : DemoMcuProgressRoutine
Description  : Entry for obtaining the progress
Input        : void *arg
Output       : None
Return       : None
*******************************************************************************/
void* DemoMcuProgressRoutine(void *arg)
{
    char progress = 0;
    char old_progress = 100;
    thr_data_type* DataPtr = (thr_data_type * )arg;
    

    do{
        FmAPI_GetUpgradeProgress(DataPtr->SessionPtr, DataPtr->slot, &progress);

        sleep(1);
        if (old_progress == progress)
            continue;

        if ( 0 <= progress && 100 >= progress )
        {
            fprintf(stdout, "Thread[%d], FPGA Slot[%d] upgrade progress is %d %%\n",DataPtr->tid, DataPtr->slot, progress);
        }

        old_progress = progress;
    }while( progress < 100 );
    pthread_exit(NULL);
}

/*******************************************************************************
Function     : DemoMcuUpgradeRoutine
Description  : Entry of the mcu upgrade thread
Input        : void *arg
Output       : None
Return       : None
*******************************************************************************/
void* DemoMcuUpgradeRoutine(void *arg)
{
    thr_data_type* DataPtr = (thr_data_type * )arg;
    FmAPI_UpgradeMCUFirmware(DataPtr->SessionPtr, DataPtr->slot, filepath);
    pthread_exit(NULL);
}

/*******************************************************************************
Function     : DemoMcuUpgradeFirmware
Description  : Create task based on the number of cards
Input        : None
Output       : None
Return       : None
*******************************************************************************/
void DemoMcuUpgradeFirmware()
{
    fmerrno Result = FM_API_ERR_CODE_EINVAL;
    FmSession* SessionHdl = NULL;
    unsigned int Ret = 0;
    struct stat FileState = {0};  
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

    /* You can also use another slot to perform this operation */
    printf ( "Start to upgrade MCU about slot 0.\n" );        

    Ret = stat ( filepath, &FileState );
    if ( 0 != Ret )
    {
        printf ( "The file path does not exist.\n" );
        return;
    }

    if ( S_ISDIR( FileState.st_mode ) )
    {
        printf ( "The file does not exist.\n" );
        return;        
    }

    pthread_t* thr = (pthread_t*) calloc(2, sizeof(*thr));


    thr_data_type* thr_data = (thr_data_type *)calloc( sizeof(*thr_data), 1);

    memset(thr_data, 0, sizeof(* thr_data));
  
    thr_data[0].tid = 0;
    thr_data[0].slot = SessionHdl->Slot[0];
    thr_data[0].SessionPtr = SessionHdl;
    pthread_create(&thr[0], NULL, DemoMcuUpgradeRoutine, (void * )&thr_data[0]);
    pthread_create(&thr[1], NULL, DemoMcuProgressRoutine,  (void * )&thr_data[0]);


    for ( i = 0; i < 2 ; i ++)
    {
        pthread_join(thr[i], NULL);
    }

    free (thr);
    thr = NULL;
    
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
    DemoMcuUpgradeFirmware();
    return 0;
}
