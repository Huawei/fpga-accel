使用基于SDAccel的Example
=======================

[Switch to the English version](./Using_an_SDAccel_based_Example.md)

目录
-------------------------

[总体操作流程](#sec-1)

[下载开发套件](#sec-2)

[配置硬件开发环境](#sec-4)

[编译example](#sec-5)

[仿真example](#sec-6)

[加载Shell版本到FPGA卡](#sec-3)

[加载动态及测试](#sec-7)

<a id="sec-1" name="sec-1"></a>
总体操作流程
------------

![](media/SDAccel_example.jpg)


<a id="sec-2" name="sec-2"></a>
下载开发套件
------------

对于https连接执行`git clone https://github.com/Huawei/fpga-accel.git`命令将开发套件下载到您的本地服务器。

对于ssh连接执行`git clone git@github.com/Huawei/fpga-accel.git`命令将开发套件下载到您的本地服务器。

用户在开始硬件开发流程前，需要下载相关网表文件到用户服务器，具体下载路径如下（本描述前提为用户基于linux系统环境进行下载，以初始版本`01210162`为例进行说明，用户如果需要下载其他版本时则直接替换掉下面命令中的`01210162`字段即可）：

```bash
curl -k -s -O --retry 3 https://huawei-fpga.obs.cn-north-1.myhwclouds.com/FX600/hardware/sdaccel_design/dsa_v01210162/huawei_vu9p_dynamic_f100_vc_5_1.dsa

curl -k -s -O --retry 3 https://huawei-fpga.obs.cn-north-1.myhwclouds.com/FX600/hardware/sdaccel_design/dsa_v01210162/huawei_vu9p_dynamic_f100_vc_5_1.spfm

curl -k -s -O --retry 3 https://huawei-fpga.obs.cn-north-1.myhwclouds.com/FX600/hardware/sdaccel_design/dsa_v01210162/huawei_vu9p_dynamic_f100_vc_5_1.xpfm
```

#### 说明:

用户下载的网表文件及版本信息可参考`release_note.md`文件，用户根据自己的需求进行选择下载。

所下载网表文件包含`huawei_vu9p_dynamic_f100_vc_5_1.dsa、huawei_vu9p_dynamic_f100_vc_5_1.spfm、huawei_vu9p_dynamic_f100_vc_5_1.xpfm`三个，下载后存放在以下路径下：

```bash
FX600/sdaccel/hardware/sdaccel_design/lib/platform/xilinx_huawei-vu5p_2ddr-dynamic_5_1
```

<a id="sec-4" name="sec-4"></a>
配置硬件开发环境
------------

用户进入开发套件`fpga-accel/FX600`目录下，在进行SDAccel开发前，用户需要运行开发套件下的`setup.sh`脚本完成硬件开发环境的配置，命令如下：

```bash
cd FX600/sdaccel/ 
export HW_FPGA_DIR=$(pwd)
source $HW_FPGA_DIR/setup.sh
```


<a id="sec-5" name="sec-5"></a>
编译example
------------

本开发套件中提供三个example供用户进行参考设计，具体如下：

`vadd_cl`：这是一个opencl 矢量相加的实例，基于opencl c实现kernel；

`mmult_hls`：这是一个基于opencl矩阵乘法实例，基于c实现kernel；

`vadd_rtl`：这是一个opencl 矢量相加的实例，基于rtl实现kernel。

此文档基于`mmult_hls`作为示例进行说明,另外两个example可参考此文档进行相关操作。

```bash
cd $HW_FPGA_DIR/hardware/sdaccel_design/examples/mmult_hls/scripts
sh compile.sh compile_mode
```

##### 说明:
   以下步骤的命令都可以通过–h参数获取帮助信息。  
   *compile_mode*为三种编译模式，对应可选择：  
   cpu_em ------------ cpu仿真模式，编译结束生成bin_mmult_cpu_emu.xclbin  
   hw_em ------------- 硬件仿真模式，编译结束生成bin_mmult_hw_emu.xclbin   
   hw ------------------ 硬件编译模式，编译结束生成bin_mmult_hw.xclbin   
   用户根据自身需求选择对应编译模式即可。 


<a id="sec-6" name="sec-6"></a>
仿真example
------------


##### 提示:

不需要仿真测试时，请跳过本章节。

进行mmult的cpu_em仿真编译：

```bash
cd $HW_FPGA_DIR/hardware/sdaccel_design/examples/mmult_hls/scripts
sh run.sh emu ../prj/bin/mmult ../prj/bin/bin_mmult_cpu_emu.xclbin
```

进行mmult的hw_em仿真编译：

```bash
cd $HW_FPGA_DIR/hardware/sdaccel_design/examples/mmult_hls/scripts
sh run.sh emu ../prj/bin/mmult ../prj/bin/bin_mmult_hw_emu.xclbin
```

##### 说明:

   *host*为编译生成的主机程序（硬件编译模式不支持仿真）：  
   mmult_hls示例 -------------------------- 对应主机程序为mmult  
   vadd_cl示例 ----------------------------- 对应主机程序为vadd  
   vadd_rtl示例 ----------------------------- 对应主机程序为host

   *xclbin*为编译生成的xclbin文件,不同的编译模式对应不同的xclbin文件：  
   *compile_mode*为cpu_em ----------- 对应选择bin_mmult_cpu_emu.xclbin  
   *compile_mode*为hw_em ------------ 对应选择bin_mmult_hw_emu.xclbin  

<a id="sec-3" name="sec-3"></a>
加载Shell版本到FPGA卡
------------

下载Shell部分bin文件命令如下（本描述前提为用户基于linux系统环境进行下载，以初始版本`01210162`为例进行说明，用户如果需要下载其他版本时则直接替换掉下面命令中的`01210162`字段即可）：

```bash
curl -k -s -O --retry 3 https://huawei-fpga.obs.cn-north-1.myhwclouds.com/FX600/hardware/sdaccel_design/bin_v01210162/hcf_sdaccel_x_vu9p_1xpr_shell.bin
```

使用工具`fmtk`将下载的shell bin加载到FPGA卡中，工具`fmtk`下载路径`http://support.huawei.com/enterprise/zh/software/23249385-ESW2000049499`。使用方式可参考《FX600开发者指南》文档中相关章节。

#### 说明:

加载完成后需要对服务器进行下上电处理，待服务器上电后再进行后续操作。

<a id="sec-7" name="sec-7"></a>
加载动态及测试
------------

执行run.sh完成硬件的动态部分加载及测试环节，具体步骤如下：

```bash
make
sh run.sh mmult $HW_FPGA_DIR/hardware/sdaccel_design/examples/mmult_hls/prj/bin/bin_mmult_hw.xclbin 0
```

##### 说明:

上面run.sh脚本末尾的0表示slot号，该号代表用户所使用环境的FPGA卡号。比如用户服务器带4张FPGA加速卡时，则slot号为0、1、2、3。
bin_mmult_hw.xclbin文件为mmult_hls example生成的xclbin文件。
run.sh具体使用请执行sh run.sh -h查看。