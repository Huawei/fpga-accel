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

#define SYS_ALARM_FX300_DEFAULT_VALUE        0xf0000
#define SYS_ALARM_FX600_DEFAULT_VALUE        0xfffffd
#define FX300_ALARM_FLAG                 1
#define FX600_ALARM_FLAG                 2

/*******************************************************************************
Function     : DispAlarmPerType
Description  : Print the alarm information about a single register
Input        : name               Register
               AlarmSupportFlag   Indicates whether the alarm function is supported
               AlarmValue         Register value
Output       : None
Return       : None
*******************************************************************************/
void DispAlarmPerType ( const char* name, unsigned int AlarmSupportFlag, unsigned int AlarmValue)
{

    unsigned int strLen = 0;

    if ( NULL == name )
    {
        printf ( "The input pointer is null.\n" );
        return;
    }

    strLen = strlen ( name );

    /* The default value of alarm register reg_sys_err is not 0 */
    if ( 0 == strncmp ( name, "reg_sys_err", strLen ))
    {

        /* Check the alarm value of d518 */
        if ( FX300_ALARM_FLAG == AlarmSupportFlag )
        {
            if ( SYS_ALARM_FX300_DEFAULT_VALUE == AlarmValue )            
            {
                printf ( "|%-56s|      OK      |\n", name );
            }
            else
            {
                printf ( "|%-56s| ERR:0x%06x |\n", name, AlarmValue );
            }
        }

        /* Check the alarm value of d516 */
        if ( FX600_ALARM_FLAG == AlarmSupportFlag )
        {
            if ( SYS_ALARM_FX600_DEFAULT_VALUE == AlarmValue )            
            {
                printf ( "|%-56s|      OK      |\n", name );
            }
            else
            {
                printf ( "|%-56s| ERR:0x%06x |\n", name, AlarmValue );
            }
        }
        return;
    }

    /* The default value of other alarm registers is 0 */
    else 
    {
        /* If the alarm value is not 0,an alarm is generated */
        if ( AlarmValue )
        {
            printf ( "|%-56s| ERR:0x%06x |\n", name, AlarmValue );
        }
        else
        {
            printf ( "|%-56s|      OK      |\n", name );
        }
        return;
    }
}

/*******************************************************************************
Function     : DispAlarmInformation
Description  : Print alarm information
Input        : FpgaSlot          Logical Slot No.
               AlarmInformation  alarm information
Output       : None
Return       : None
*******************************************************************************/
void DispAlarmInformation ( unsigned FpgaSlot, FM_DEVICE_ALARM_DATA* AlarmInfor )
{

    if ( NULL == AlarmInfor )
    {
        printf ( "The input pointer is null.\n" );
        return;
    }

    switch ( AlarmInfor->AlarmSupportFlag )
    {
        case 0:
            printf ( "FPGA:%u doesn't support alarm information query.\n", FpgaSlot );
            break;
        case FX300_ALARM_FLAG:
            printf ( "\n====================== Alarm State of FPGA slot %u =======================\n", FpgaSlot );
            printf ( "|        Alarm Type                                      |    status    |\n" );
            printf ( "--------------------------------------------------------------------------\n" );
            DispAlarmPerType ( "reg_sys_err", FX300_ALARM_FLAG, AlarmInfor->systemAlarmReturn );
            printf ( "--------------------------------------------------------------------------\n" );
            DispAlarmPerType ( "reg_ddr0_ecc_status", FX300_ALARM_FLAG, AlarmInfor->DDR0ECCReturn );
            printf ( "--------------------------------------------------------------------------\n");
            DispAlarmPerType ( "reg_ddr1_ecc_status", FX300_ALARM_FLAG, AlarmInfor->DDR1ECCReturn );
            printf ( "--------------------------------------------------------------------------\n");    
            DispAlarmPerType ( "firewall_ctrl_status", FX300_ALARM_FLAG, AlarmInfor->mgmtCtrlFirewallReturn );
            printf ( "--------------------------------------------------------------------------\n");
            DispAlarmPerType ( "firewall_usr_ctrl_status", FX300_ALARM_FLAG, AlarmInfor->userCtrlFirewallReturn );
            printf ( "--------------------------------------------------------------------------\n");
            DispAlarmPerType ( "firewall_usr_data_status", FX300_ALARM_FLAG, AlarmInfor->userDatafirewallReturn );
            printf ( "================================== End ===================================\n\n" );
            break;
        case FX600_ALARM_FLAG:
            printf ( "\n====================== Alarm State of FPGA slot %u =======================\n", FpgaSlot );
            printf ( "|        Alarm Type                                      |    status    |\n" );
            printf ( "--------------------------------------------------------------------------\n" );
            DispAlarmPerType ( "reg_sys_err", FX600_ALARM_FLAG, AlarmInfor->systemAlarmReturn );
            printf ( "================================== End ===================================\n\n" );
            break;
        default:
            printf ( "FPGA[%u] an unknown error occurred.\n", FpgaSlot );
            break;
    }
    return;
}

/*******************************************************************************
Function     : DemoInquireAlarm
Description  : Query the alarm information
Input        : None
Output       : None
Return       : None
*******************************************************************************/
void DemoInquireAlarm()
{
    fmerrno Result = FM_API_ERR_CODE_EINVAL;
    FmSession* SessionHdl = NULL;
    FM_DEVICE_ALARM_DATA AlarmInfor = {0};
 
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
        FmAPI_StopSession ( SessionHdl );
        FmAPI_Unload();
        return;
    }
    else
    {
        printf ("%u fpga devices founded!\n", ( SessionHdl->PfNum ));
    }

    /* You can also use another slot to perform this operation */
    printf ( "Start to query the alarm information about slot 0.\n" );

    Result = FmAPI_GetDeviceAlarm ( SessionHdl, 0, &AlarmInfor );
    if( FM_API_ERR_NOT_SUPPORT == Result )
    {
        printf( "FPGA:0 doesn't support alarm information query.\n" );
    }

    if( FM_API_FPGA_STATUS_ERR == Result )
    {
        printf( "Fpga 0 status error!\n" );
    }

    if ( FM_API_SUCCESS != Result )
    {
         printf( "FPGA 0 API Inquire Alarm failed. err code: %d\n", Result );
    }
    else
    {
        DispAlarmInformation( 0, &AlarmInfor );
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
    DemoInquireAlarm();
    return 0;
}

