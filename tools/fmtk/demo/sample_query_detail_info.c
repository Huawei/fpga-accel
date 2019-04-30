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

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include "fmtkapi.h"

#define NULL_FOR_NA(x)  ((strlen((char*)x)>0)?((char*)x):("n/a"))

typedef struct { int tid; int slot; FmSession* SessionPtr; }  thr_data_type;


/*******************************************************************************
Function     : GetOpticalInfo
Description  : Get the optical module information
Input        : FmSession* SessionHdl, unsigned slot
Output       : None
Return       : None
*******************************************************************************/
void GetOpticalInfo(FmSession* SessionHdl, unsigned slot)
{
    FM_OPTICAL_MODULE_DATA OpticalModule = {0};
    fmerrno ret = FM_API_ERR_CODE_EINVAL;
    int i = 0;

    memset(&OpticalModule, 0, sizeof(OpticalModule));
    
    ret = FmAPI_GetOpticalModules(SessionHdl, slot, &OpticalModule);
    if ( FM_API_SUCCESS != ret )
    {
        printf("Get optical info failed, err code: %d\n", ret);
        return;
    }

    for (i = 0; i < OpticalModule.fcCount; i++)
    {   
        printf( "      |---- MODULE[%u]_State\t\t\t\t: %s\n", i+1, OpticalModule.state[i]);

        if ( 0 == strncmp( OpticalModule.state[i], "invalid", 7) )
        {
            continue;
        }

        printf("      |---- MODULE[%u]_Manufacturer\t\t\t: %s\n", i+1, NULL_FOR_NA(OpticalModule.manufacturer[i]));
        printf("      |---- MODULE[%u]_Production_Date\t\t\t: %s\n", i+1, NULL_FOR_NA(OpticalModule.productionDate[i]));
        printf("      |---- MODULE[%u]_Part_Number\t\t\t: %s\n", i+1, NULL_FOR_NA(OpticalModule.partNumber[i]));
        printf("      |---- MODULE[%u]_Serial_Number\t\t\t: %s\n", i+1, NULL_FOR_NA(OpticalModule.serialNumber[i]));
        printf("      |---- MODULE[%u]_Temperature\t\t\t: %s\n", i+1, NULL_FOR_NA(OpticalModule.temperature[i]));
        printf("      |---- MODULE[%u]_Voltage\t\t\t\t: %s\n", i+1, NULL_FOR_NA(OpticalModule.voltage[i]));
        printf("      |---- MODULE[%u]_Type\t\t\t\t: %s \n", i+1, NULL_FOR_NA(OpticalModule.model[i]));
        printf("      |---- MODULE[%u]_Wave_Length\t\t\t: %s\n", i+1, NULL_FOR_NA(OpticalModule.waveLength[i]));
        printf("      |---- MODULE[%u]_Mode\t\t\t\t: %s\n", i+1, NULL_FOR_NA(OpticalModule.fcMode[i]));
        printf("      |---- MODULE[%u]_Channel_Count\t\t\t: %u\n", i+1, OpticalModule.channelCount[i]);
        
    }

    return;
}

/*******************************************************************************
Function     : GetPowerInfo
Description  : Get the power of the fpga
Input        : FmSession* SessionHdl, unsigned slot
Output       : None
Return       : None
*******************************************************************************/
void GetPowerInfo(FmSession* SessionHdl, unsigned slot)
{
    FM_DEVICE_POWER_DATA PowerData;
    fmerrno ret = FM_API_ERR_CODE_EINVAL;

    memset(&PowerData, 0, sizeof(PowerData));
    
    ret = FmAPI_GetDevicePowers(SessionHdl, slot, &PowerData);
    if ( FM_API_SUCCESS != ret )
    {
        printf("Get powers failed, err code: %d\n", ret);
        return;
    }

    printf("      |---- Power                                       : %.01f W\n", PowerData.curPower);

    return;
}

/*******************************************************************************
Function     : DisplayOneArg
Description  : Display the value of one sensor
Input        : unsigned SensorCount, char Value[SENSOR_NUM_MAX][FMTK_MAX_LABEL_LEN]
Output       : None
Return       : None
*******************************************************************************/
void DisplayOneArg( unsigned SensorCount, char Value[SENSOR_NUM_MAX][FMTK_MAX_LABEL_LEN] )
{
    int i = 0;

    if ( NULL == Value )
    {
        return;
    }

    for ( i = 1; i <= SensorCount; i++ )
    {  
       printf("      |---- %s\n",  Value[i]);
    }

    return;
}

/*******************************************************************************
Function     : GetElabelInfo
Description  : Get the electronic label information
Input        : FmSession* SessionHdl, unsigned slot
Output       : None
Return       : None
*******************************************************************************/
void GetElabelInfo(FmSession* SessionHdl, unsigned slot)
{
    FM_DETAILED_ID_DATA DetailedInfo;
    fmerrno ret = FM_API_ERR_CODE_EINVAL;

    memset(&DetailedInfo, 0, sizeof(DetailedInfo));
    
    ret = FmAPI_GetDetailedInfos(SessionHdl, slot, &DetailedInfo);
    if ( FM_API_SUCCESS != ret )
    {
        printf("Get electronic label info failed, err code: %d\n", ret);
        return;
    }

    printf("      |---- BoardName                                   : %s\n", NULL_FOR_NA(DetailedInfo.boardName));
    printf("      |---- ProductName                                 : %s\n", NULL_FOR_NA(DetailedInfo.productName));
    printf("      |---- ModelNumber                                 : %s\n", NULL_FOR_NA(DetailedInfo.modelNumber));
    printf("      |---- Serial                                      : %s\n", NULL_FOR_NA(DetailedInfo.serial));
    printf("      |---- Manufacturer                                : %s\n", NULL_FOR_NA(DetailedInfo.manufacturer));
    printf("      |---- FruFileID                                   : %s\n", NULL_FOR_NA(DetailedInfo.fruFileID));
    printf("      |---- Mcu_Health_Status                           : %s\n", NULL_FOR_NA(DetailedInfo.healthState));
    printf("      |---- Board_ID                                    : %s\n", NULL_FOR_NA(DetailedInfo.boardId));
    printf("      |---- PCB_ID                                      : %s\n", NULL_FOR_NA(DetailedInfo.pcbId));
    printf("      |---- BOM_ID                                      : %s\n", NULL_FOR_NA(DetailedInfo.bomId));   
    printf("      |---- Flash_Manufacturer                          : %s\n", NULL_FOR_NA(DetailedInfo.flashManufacturer));
    printf("      |---- McuFirmwareVer                              : %s\n", NULL_FOR_NA(DetailedInfo.mcuFirmwareVer));   


    DisplayOneArg(DetailedInfo.temperatureSensorNum, DetailedInfo.TSensorVal);
    DisplayOneArg(DetailedInfo.voltageSensorNum ,DetailedInfo.VSensorVal);

    return;
}

/*******************************************************************************
Function     : GetDimmInfo
Description  : Get the dimm information
Input        : FmSession* SessionHdl, unsigned slot
Output       : None
Return       : None
*******************************************************************************/
void GetDimmInfo(FmSession* SessionHdl, unsigned slot)
{
    FM_DDR_DATA DDRData = { 0 };
    fmerrno ret = FM_API_ERR_CODE_EINVAL;
    int i = 0;

    memset(&DDRData, 0, sizeof(DDRData));

    ret = FmAPI_GetDDRDatas(SessionHdl, slot, &DDRData);
    if ( FM_API_SUCCESS != ret )
    {
        printf("Get dimm info failed, err code: %d\n", ret);
        return;
    }  

    for (i = 0; i < DDRData.spdCount; i++)
    {
        printf("      |---- DIMM[%u]_State\t\t\t\t: %s\n", i+1, NULL_FOR_NA(DDRData.spdMode[i]));
        if ( 0 == strncmp(DDRData.spdMode[i], "invalid", 7) )
        {
            continue;
        }
        printf("      |---- DIMM[%u]_Manufacturer\t\t\t: %s \n", i+1, NULL_FOR_NA(DDRData.sensorName[i]) );        
        printf("      |---- DIMM[%u]_Serial_Number\t\t\t: %s \n", i+1,  NULL_FOR_NA(DDRData.serialNum[i]));      
        printf("      |---- DIMM[%u]_Part_Number\t\t\t\t: %s \n", i+1,  NULL_FOR_NA(DDRData.partNum[i]) );
        printf("      |---- DIMM[%u]_Volume\t\t\t\t: %u Mb \n", i+1, DDRData.size[i] );    
        printf("      |---- DIMM[%u]_Frequency\t\t\t\t: %u MHz \n", i+1, DDRData.frequencyHz[i]); 
        printf("      |---- DIMM[%u]_Rank\t\t\t\t: %d  \n", i+1, DDRData.rank[i] ); 
        printf("      |---- DIMM[%u]_Type\t\t\t\t: %s\n", i+1, NULL_FOR_NA(DDRData.spdType[i]));
    }

    return;
}

/*******************************************************************************
Function     : GetComponentInfo
Description  : Get the component information
Input        : FmSession* SessionHdl, unsigned slot
Output       : None
Return       : None
*******************************************************************************/
void GetComponentInfo(FmSession* SessionHdl, unsigned slot)
{
    FM_COMPONENT_DATA strComponent = { 0 };
    fmerrno ret = FM_API_ERR_CODE_EINVAL;
    int i = 0;

    ret = FmAPI_GetComponentVers(SessionHdl, slot, &strComponent);
    if ( FM_API_SUCCESS != ret )
    {
        printf("Get M-brick info failed, err code: %d\n", ret);
        return;
    }  

    printf("   M-BRICK INFO\n");

    for (i = 0; i < strComponent.componentCount; i++)
    {
       
        (strComponent.componentVersion[i] == 0)? \
        printf("      |---- M-brick[%s] Version\t\t\t: %s\n", strComponent.componentName[i], "n/a"):
        printf("      |---- M-brick[%s] Version\t\t\t: %x\n", strComponent.componentName[i], strComponent.componentVersion[i]);
    }
    
    return;
}

/*******************************************************************************
Function     : DemoAcquireDetailInfo
Description  : Acquire detail info of slot 0
Input        : None
Output       : None
Return       : None
*******************************************************************************/
void DemoAcquireDetailInfo()
{
    fmerrno ret = FM_API_ERR_CODE_EINVAL;
    FmSession* SessionHdl = NULL;
    unsigned Num = 0;
 
    ret  = FmAPI_Initialize(SELECTED_API_VERSION, SELECTED_API_SUB_VER);
    if (FM_API_SUCCESS != ret)
    {
        printf("Initialization failed.\n");
        return;
    }

    printf("FMTK api Initialized.\n");

    FmAPI_StartSession(&SessionHdl);
    
    Num = SessionHdl->PfNum;

    if ( 0 == Num )
    {
        printf("No fpga devices!\n");
        FmAPI_StopSession(SessionHdl);
        FmAPI_Unload();
        return;
    }
    else
    {
        printf("%u fpga devices founded!\n", Num);
    }

    /* You can also use another slot to perform this operation */
    printf("The demo will query detail info of slot 0\n");
    GetElabelInfo(SessionHdl, 0);
    GetPowerInfo(SessionHdl, 0);
    GetOpticalInfo(SessionHdl, 0);
    GetDimmInfo(SessionHdl, 0);
    GetComponentInfo(SessionHdl, 0);

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
Input        : 
Output       : None
Return       : 0:sucess other:fail
*******************************************************************************/
int main()
{   
    DemoAcquireDetailInfo();
    
    return 0;
}


