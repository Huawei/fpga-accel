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
XOCLDRV_PATH=${FPGADESIGN_PATH}/software/kernel_drivers/xocl/driver/xclng/drm/xocl
USERHAL_PATH=${FPGADESIGN_PATH}/software/userspace/sdaccel/driver/xclgemhal/driver/xclng/user_gem
USERHALLIB_PATH=${USERHAL_PATH}/../../../../../lib/runtime/platforms/hal/driver
echo "dma drv  path is ${DMADRV_PATH}"
if [ -z $XILINX_SDX ]
then 
    echo -e "Xilinx SDx is not found! Please check it first!\n"
    exit
fi

#check XOCL driver is installed or not
DRIVER=xocl

echo -e "\nCompile and installing...\n"

cd ${XOCLDRV_PATH}
make clean

echo -e "\nXocl driver is compling..."
XOCL_COMLOG=`make |egrep -w 'err|ERR|error|ERROR'`
if [[ -n "$XOCL_COMLOG" ]]; then
    echo "ERROR: XOCL driver compiled error, please check first!"
    exit 1
fi

MODULE_INFO=`lsmod | grep $DRIVER 2>&1`
if [[ -z "$MODULE_INFO" ]]; then
    echo "Warning: $DRIVER driver is not exist, will install first."
    modprobe drm
    insmod xocl.ko
    echo "$DRIVER driver is installed now!"
else
    echo ""
    echo "The exsisted $DRIVER driver will be uninstalled and the new will be re-install !"
    rmmod xocl 
    modprobe drm
    insmod xocl.ko
    echo "Note: If necessary, please re-insmod $DRIVER driver manually!"
    echo ""
fi

make clean

rm -rf ${USERHALLIB_PATH}/*
cd ${USERHAL_PATH} 
make clean

echo -e "\nHAL is compling..."
HAL_COMLOG=`make |egrep -w 'err|ERR|error|ERROR'`
if [[ -n "$HAL_COMLOG" ]]; then
    echo "ERROR: hal compiled error, please check first!"
    exit 1
fi
make clean

cd ${realpath}

echo "*************************************************"
echo "Compile and install driver and HAL lib completed!"
echo "*************************************************"
