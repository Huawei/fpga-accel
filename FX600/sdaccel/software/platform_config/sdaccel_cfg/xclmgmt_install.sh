#!/bin/bash
#
#-------------------------------------------------------------------------------
#Copyright (c) 2018 Huawei Technologies Co., Ltd. All Rights Reserved.
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

if [[ "$0" =~ ^\/.* ]]; then
	script=$0
else
	script=$(pwd)/$0
fi
script=$(readlink -f $script)
script_path=${script%/*}

realpath=$(readlink -f $script_path)

echo $realpath
FPGADESIGN_PATH=${realpath}/../../..
XCLMGMT_DRV_PATH=${FPGADESIGN_PATH}/software/kernel_drivers/xclmgmt/driver/xclng/mgmt

if [ -z $XILINX_SDX ]
then 
    echo -e "Xilinx SDx is not found! Please check it first!\n"
    exit
fi

#check XOCL driver is installed or not
DRIVER_MGMT=xclmgmt

echo -e "\nCompile and installing...\n"

cd ${XCLMGMT_DRV_PATH}
make clean

echo -e "\nXclmgmt driver is compling..."

XCLMGMT_COMLOG=`make |egrep -w 'err|ERR|error|ERROR'`
if [[ -n "$XCLMGMT_COMLOG" ]]; then
    echo "ERROR: XCLMGMT driver compiled error, please check first!"
    exit 1
fi

MODULE_INFO=`lsmod | grep $DRIVER_MGMT 2>&1`
if [[ -z "$MODULE_INFO" ]]; then
    echo "Warning: $DRIVER_MGMT driver is not exist, will install first."    
    insmod xclmgmt.ko health_check=0 skip_load_dsabin=1   
    echo "$DRIVER_MGMT driver is installed now!" 
else
    echo ""
    echo "$DRIVER_MGMT driver is exist!"
    echo "Note: If necessary, please re-insmod $DRIVER driver manually!"
    echo ""
fi

make clean

cd ${realpath}

echo "*************************************************"
echo "Compile and install XCLMGMT driver completed!"
echo "*************************************************"
