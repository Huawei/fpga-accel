#!/bin/bash
#
#-------------------------------------------------------------------------------
#      Copyright 2018 Huawei Technologies Co., Ltd. All Rights Reserved.
# 
#      This program is free software; you can redistribute it and/or modify
#      it under the terms of the Huawei Software License (the "License").
#      A copy of the License is located in the "LICENSE" file accompanying 
#      this file.
# 
#      This program is distributed in the hope that it will be useful,
#      but WITHOUT ANY WARRANTY; without even the implied warranty of
#      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#      Huawei Software License for more details. 
#-------------------------------------------------------------------------------

# Print animation '.' for processing
function dots_process () {
    seconds=1
    while :
    do
        sleep $seconds
        echo -n '.'
        if [ -f $WORK_DIR/.find_tmp ] ; then
            exit 0
        fi
    done
}

# Print animation '-, \, |, /' for processing
function rotate_process () {
    INTERVAL=0.5
    RCOUNT="0"
    while : ; do
        ((RCOUNT = RCOUNT + 1))
        case $RCOUNT in
            1)  echo -e '-\b\c'
                sleep $INTERVAL
                ;;
            2)  echo -e '\\\b\c'
                sleep $INTERVAL
                ;;
            3)  echo -e '|\b\c'
                sleep $INTERVAL
                ;;
            4)  echo -e '/\b\c'
                sleep $INTERVAL
                ;;
            *) RCOUNT=0
                ;;
        esac
    done
}

function stop_process () {
    if [ ! -f $WORK_DIR/.find_tmp ] ; then
        touch -f $WORK_DIR/.find_tmp
    fi
}

# Check and split software dir
function check_soft_dir () {
    if [ $# -lt 1 ] ; then
        echo ""
    elif [ -d $1 ] ; then
        echo $1
    else
        soft_dir_tmp=(${1//:/ })
        if [ ${#soft_dir_tmp[@]} -lt 1 ] ; then
            echo ""
        else
            echo ${soft_dir_tmp[@]}
        fi
    fi
}

# Check whether more than one software detected
# If more than one software detected, return first result.
function check_more_soft () {
    if [ $# -lt 1 ] ; then
        echo ""
    elif [ ! -f $1 ]; then
        echo $1 | awk {'print $1'}
    else
        echo $1
    fi
}

# Show usage info
function usage () {
    echo  -e "\e[0;35m Usage: source setup.sh [software directory] / -c / --clean / -u / -l / -xr \e[0m"
    echo  -e "\e[0;35m Setup hardware develop eviranment. \e[0m"
    echo  -e "\e[0;35m example: source setup.sh \e[0m"
    echo  -e "\e[0;35m example: source setup.sh /opt \e[0m"
    echo  -e "\e[0;35m \e[0m"
    echo  -e "\e[0;35m Parameter: \e[0m"
    echo  -e "\e[0;35m         -h / --help          Print usage info \e[0m"
    echo  -e "\e[0;35m         -u / --usercfg       Specify usercfg file(Using setup.cfg as default) \e[0m"
    echo  -e "\e[0;35m         -s / --su            Specify permitted user(Using root as default) \e[0m"
    echo  -e "\e[0;35m         -l / --license       Specify user license cfg for software \e[0m"
    echo  -e "\e[0;35m         -xr/ --xilinxreq     Specify xilinx vivado version \e[0m"
    echo  -e "\e[0;35m         -x / --usesdx        Using SDAccel develop mode \e[0m"
    echo  -e "\e[0;35m         -v / --verbose       Enable Verbose mode(More information will be print) \e[0m"
    echo  -e "\e[0;35m         [software directory] Software install directory(If not define using /software as default) \e[0m"
}

# Touch empty file to stop subprocess
trap "stop_process" 1 2 3 15

# Set env varible
export WORK_DIR=$(cd $(dirname ${BASH_SOURCE[0]});pwd )

default_cfg="$WORK_DIR/setup.cfg"
setup_cfg=$default_cfg

default_soft_dir="/software"
soft_dir=$default_soft_dir
soft_dir_userdef=0

vivado_ver_req="2017.2"

script_name=${BASH_SOURCE[0]}

# Judge which method that scripts run
if [ $script_name == $0 ] ; then
    # using 'sh ./setup.sh' or './setup.sh' 
    script_exec=1
else
    # using 'source ./setup.sh' or '. ./setup.sh' 
    script_exec=0
fi

quit_script=0
user_permit=""
usercfg_file=""
userlic_cfg=""
xilinxuser_req=""
fpga_dev_mode=0
verbose_mode=0

function info_show() {
    if [ $verbose_mode -ne 0 ] ; then
        echo "$1"
    fi
}

function info_show_n() {
    if [ $verbose_mode -ne 0 ] ; then
        echo -n "$1"
    fi
}

function info_show_e() {
    if [ $verbose_mode -ne 0 ] ; then
        echo -e "$1"
    fi
}

# Check whether or not user specified the software installation directory
if [ $# -lt 1 ] ; then
    soft_dir=$default_soft_dir
else

    while [ "$1" != "" ]; do
        case $1 in
            -h | -H | -help | --help ) 
                usage
                quit_script=1
                break
                ;;
            -u | -usercfg | --usercfg )
                shift
                usercfg_file=$1
                ;;
            -s | -su | --su )
                shift
                user_permit=$1
                ;;
            -l | -license | --license )
                shift
                userlic_cfg=$1
                ;;
            -xr | -xilinxreq | --xilinxreq )
                shift
                xilinxuser_req=$1
                ;;
            -x | -usesdx | --usesdx )
                fpga_dev_mode=1
                ;;
            -v | -verbose | --verbose )
                verbose_mode=1
                ;;
            *)
                soft_dir=`check_soft_dir $1`
                if [ "x$soft_dir" == "x" ] ; then
                    echo -e "\e[0;33m Info: Software directory does not ever exist, using $default_soft_dir as default. \e[0m"
                else
                    soft_dir_userdef=1
                fi
                ;;
        esac
        shift
    done
fi

fiscfg=$(readlink -f ~/.fiscfg)
# If using source xxxxx, error will not cause quit of shell.
if [ $quit_script == 1 -a $script_exec == 0 ] ; then
    return
elif [ $quit_script == 1 ] ; then
    exit
fi

# Load user cfg file
if [ "x$usercfg_file" != "x" ] ; then
    setup_cfg=$userlic_cfg
fi

source $setup_cfg
# If using source xxxxx, error will not cause quit of shell.
if [ $quit_script == 1 -a $script_exec == 0 ] ; then
    return
elif [ $quit_script == 1 ] ; then
    exit
fi

# Load soft_dir
if [ $soft_dir_userdef -eq 1 ] ; then
    # Do nothing
    soft_dir=$soft_dir
elif [ "x$SOFT_INSTALL_DIR" != "x" ] ; then
    if [ -d $SOFT_INSTALL_DIR ] ; then
        soft_dir=$SOFT_INSTALL_DIR
    fi
fi

# Check whether software directory exists
if [ ! -d $soft_dir ] ; then
    echo -e "\e[0;34m Error: No software directory $soft_dir detected, please check the directory. \e[0m"
    quit_script=1
fi

# Load Xilin User Version Req
if [ "x$xilinxuser_req" != "x" ] ; then
    vivado_ver_req=$xilinxuser_req
elif [ "x$VIVADO_VER_REQ" != "x" ] ; then
    vivado_ver_req=$VIVADO_VER_REQ
fi

# If using source xxxxx, error will not cause quit of shell.
if [ $quit_script == 1 -a $script_exec == 0 ] ; then
    return
elif [ $quit_script == 1 ] ; then
    exit
fi

# Show Fpga Develop Mode
if [ $fpga_dev_mode == 1 -o $FPGA_DEVELOP_MODE == "vivado" ] ; then
    dev_mode_name="Vivado"
elif [ $fpga_dev_mode == 1 -o $FPGA_DEVELOP_MODE == "sdx" ] ; then
    dev_mode_name="SDAccel"
else
    echo "Error: Fpga develop mode is '$FPGA_DEVELOP_MODE' , The correct configuration is vivado or sdx"
    return
fi

echo
echo "---------------------------------------------------"
echo
echo "Fpga develop mode is $dev_mode_name."

# Get software info
echo
echo "---------------------------------------------------"
echo

echo -n "Checking software information."

if [ -f $WORK_DIR/.find_tmp ] ; then
    rm -f $WORK_DIR/.find_tmp
fi

# Using subprocess to aviod usless info
(dots_process &)

# Autoscan can be bypassed by user to avoid searching the directory.
# It is recommonded that user should enable 'SOFT_AUTO_SCAN' to find the software install infoamtion if not too many softwares installed
if [ "x$SOFT_AUTO_SCAN" == "xyes" ] ; then
    if [ "x$vivado_ver_req" == "x2017.4.op" ] ; then
        find $soft_dir/ -type f -name "vsim" -o -type f -name "sdx" -o -type f -name "vcs" -o -type l -name "verdi" > $WORK_DIR/.tmp
    else
        find $soft_dir/ -type f -name "vsim" -o -type f -name "vivado" -o -type f -name "vcs" -o -type l -name "verdi" > $WORK_DIR/.tmp
    fi
else
    # Redirect all software install directory into '.tmp' file
    echo "$VIVADO_INS_DIR" >  $WORK_DIR/.tmp
    echo "$VERDI_INS_DIR"  >> $WORK_DIR/.tmp
    echo "$VCS_INS_DIR"    >> $WORK_DIR/.tmp
    echo "$QUESTA_INS_DIR" >> $WORK_DIR/.tmp
fi

mv -f $WORK_DIR/.tmp $WORK_DIR/.find_tmp
sleep 1
echo
echo
echo "---------------------------------------------------"
echo

echo -n "Checking software license."
# Get license info
lic_info=(${LM_LICENSE_FILE//:/ })
echo "."
echo
echo "---------------------------------------------------"
echo

# Get vivado directory
echo "Checking vivado/sdx information..."
echo

echo "---------------------------------------------------"
echo

info_show_n "Checking whether vivado env has been set : "
if [ $fpga_dev_mode -eq 1 -o $FPGA_DEVELOP_MODE != "vivado" ] ; then
    vivado_info=`which vivado 2> /dev/null | grep SDx`
    vivado_name="vivado_sdx"
else
    vivado_info=`which vivado 2> /dev/null | grep -v SDx`
    vivado_name="vivado"
fi

if [ "x$vivado_info" == "x" ] ; then
    if [ $fpga_dev_mode -eq 0 -a $FPGA_DEVELOP_MODE == "vivado" ] ; then
        vivado_info=`cat $WORK_DIR/.find_tmp | grep vivado | grep $vivado_ver_req | grep -v unwrapped | sort -r`
    else
        if [ "x$vivado_ver_req" == "x2017.1" ] ; then
            vivado_info=`cat $WORK_DIR/.find_tmp | grep vivado | grep $vivado_ver_req | grep -v unwrapped | grep SDx | sort -r`
        else
            vivado_info=`cat $WORK_DIR/.find_tmp | grep sdx | grep $vivado_ver_req | grep -v unwrapped | grep SDx | sort -r`        
        fi
    fi
    info_show_e "\e[0;33m have not been set \e[0m"
    info_show_e "\e[0;32mStart setup vivado env \e[0m"

    # Check vivado info and add vivado to path
    info_show_n "Checking whether vivado install correctly : "
    if [ "x$vivado_info" != "x" ] ; then
        info_show_e "\e[0;32m ok \e[0m"
        # Check whether more than one vivado software detected
        vivado_info=`check_more_soft $vivado_info`
        vivado_dir=${vivado_info%%/bin*}
        if [ $fpga_dev_mode -eq 1 -o $FPGA_DEVELOP_MODE != "vivado" ] ; then
            vivado_dir=${vivado_dir%%/Vivado}
        fi
        info_show_n "Setup vivado env : "
        # Config vivado env
        source $vivado_dir/settings64.sh $vivado_dir > /dev/null
        info_show "vivado installed in $vivado_dir"
        info_show_e "\e[0;32mFinish setup vivado env \e[0m"
        echo "Setup $vivado_name env successful"
    else
        info_show_e "\e[0;34m fail \e[0m"
        echo "Error: No $vivado_name elf file find, please make sure the $vivado_name has install correctly!"
        echo "Setup $vivado_name env fail"
        quit_script=1
    fi
else
    info_show_e "\e[0;32m have been set  \e[0m"
    info_show "Vivado env has been set correctly, skip"
    echo "Setup $vivado_name env successful"
fi

# If using source xxxxx, error will not cause quit of shell.
if [ $quit_script == 1 -a $script_exec == 0 ] ; then
    return
elif [ $quit_script == 1 ] ; then
    exit
fi

# Check vivado version
vivado_ver=`vivado -version | grep Vivado | awk {'print $2'}`
if [ $fpga_dev_mode -eq 1 -o $FPGA_DEVELOP_MODE != "vivado" ] ; then
    if [ "x$vivado_ver_req" == "x2017.1" ] ; then
        vivado_ver_req="$vivado_ver_req"_sdx
    else
        vivado_ver_req="$vivado_ver_req"
    fi
fi
if [ x$vivado_ver != "xv$vivado_ver_req" ] ; then
    echo "Error: Vivado version not matched, only support vivado$vivado_ver_req!"
    quit_script=1
fi

# If using source xxxxx, error will not cause quit of shell.
if [ $quit_script == 1 -a $script_exec == 0 ] ; then
    return
elif [ $quit_script == 1 ] ; then
    exit
fi

rm -f $WORK_DIR/.find_tmp

###################################################################################################
#check gcc and sudo 
###################################################################################################
#check gcc
echo "Check the gcc/sudo information..."
gcc_info=`which gcc 2>/dev/null`
if [ -z "$gcc_info" ]; then
    echo "ERROR: gcc has not been installed."
    quit_script=1
fi
#check sudo
sudo_info=`which sudo 2>/dev/null`
if [ -z "$sudo_info" ]; then
    echo "ERROR: sudo is not in path or not installed. Driver installations will fail "
    echo "To install drivers please add sudo to your path or install sudo by running \"yum install sudo\" as root "
    quit_script=1
fi

echo
echo "---------------------------------------------------"
echo

if [ $quit_script == 1 -a $script_exec == 0 ] ; then
     return
elif [ $quit_script == 1 ] ; then
     exit
fi
###################################################################################################
#check the fpga_tool
###################################################################################################
bonding_log=/var/log/fpga/install_driver.log
mkdir -p ${bonding_log%/*}
###################################################################################################
#check the driver
###################################################################################################

if [ "$dev_mode_name" == "Vivado" ];then
    #***************************************************
    # if the device is exist
    #***************************************************
    echo "Check the driver status..."
    declare -a bdf_list=(`lspci |grep "19e5:d503" |awk '{print $1}'`)
    if [ ${#bdf_list[*]} -eq 0 ];then
        echo "Warning: The device does not exist."
    else
        bound="Kernel driver in use: igb_uio"
        #***************************************************
        # if the vf_driver is bonding
        #***************************************************
        if [ "`lspci -s ${bdf_list[0]} -vvv|grep "$bound"`" ];then
            echo "Driver has bound to device."
        else
            echo "Installing and binding driver to device. Please wait 3~5 minutes. "
            sh  $WORK_DIR/software/platform_config/dpdk_cfg/vivado_env_cfg.sh > $bonding_log 2>&1 
            dpdk_bind=`dpdk_nic_bind.py --status | grep d503`
            if [ -n "`echo $dpdk_bind | grep "drv=igb_uio"`" ];then
                echo "Driver installation and binding success!"
            else
                echo "ERROR: Driver installation or binding failed! Please check $bonding_log!"
                quit_script=1
            fi
                
        fi
    fi
elif [  "$dev_mode_name" == "SDAccel" ];then

    echo "Check the driver status..."

    sh $WORK_DIR/software/platform_config/sdaccel_cfg/sdaccel_env_cfg.sh > $bonding_log  2>&1 
    if [ $? == 0 ];then
        echo "SDAccel configuration successful!"
    else
        echo "ERROR: SDAccel configuration failed! please check $bonding_log!"
        quit_script=1
    fi
	
	sh $WORK_DIR/software/platform_config/sdaccel_cfg/xclmgmt_install.sh > $bonding_log  2>&1 
    if [ $? == 0 ];then
        echo "SDAccel xclmgmt install successful!"
    else
        echo "ERROR: SDAccel xclmgmt install failed! please check $bonding_log!"
        quit_script=1
    fi
fi

echo
echo "---------------------------------------------------"
echo

if [ $quit_script == 1 -a $script_exec == 0 ] ; then
     return
elif [ $quit_script == 1 ] ; then
     exit
fi

####################################################################################################
#end new develop
####################################################################################################

echo "+-----------+--------------------------------------------------------------------------------+"
echo "|   tool    |   install home directory                                                       |"
echo "+-----------+--------------------------------------------------------------------------------+"
printf "|   vivado  | %78s |\n" $vivado_dir
echo "+-----------+--------------------------------------------------------------------------------+"
