基于SDAccel的开发使用指南
=======================

#### 说明:

**当前版本仅限root账号进行相关操作，其他账号不能保证功能正确。**

目录
-------------------------

[环境依赖](#sec-1)

[下载开发套件](#sec-2)

[基于SDAccel的硬件开发流程](#sec-3)

[基于SDAccel的软件开发流程](#sec-4)

[基于SDAccel的Example使用](#sec-5)

[多ddr 使用说明](#sec-6)

<a name="sec-1"></a>
## 1 环境依赖

本开发套件对用户服务器的依赖如下：

| 类型        | 说明                                       |
| --------- | ---------------------------------------- |
| 操作系统      | Linux centos 7.3                         |
| 内存        | >=20GByte                                |
| 硬盘        | 安装SDx工具前>=100GByte                       |
| SDx工具安装路径 | 必须安装在文件夹/software下                       |
| license特性 | （1）XCVU5P、（2）PartialReconfiguration、（3）Simulation 1735 Version 2、（4）xcvu5p_bitgen、（5）ap_opencl |
| SDx工具版本   | 2017.4.op                                |

#### 说明:

SDx工具安装指导请参考文档[SDx_tool_installation_guide_cn](./docs/SDx_tool_installation_guide_cn.docx)。


<a name="sec-2"></a>
## 2 下载开发套件

用户下载开发套件到用户服务器，下载路径如下：

对于https连接执行`git clone https://github.com/Huawei/fpga-accel.git`命令将开发套件下载到您的本地服务器。

对于ssh连接执行`git clone git@github.com/Huawei/fpga-accel.git`命令将开发套件下载到您的本地服务器。

<a name="sec-3"></a>
## 3 基于SDAccel的硬件开发流程

### 3.1 下载网表文件

用户在开始硬件开发流程前，需要下载相关网表文件到用户服务器，具体下载路径如下（本描述前提为用户基于linux系统环境进行下载，以初始版本`01210125`为例进行说明，用户如果需要下载其他版本时则直接替换掉下面命令中的`01210125`字段即可）：

```bash
curl -k -s -O --retry 3 https://huawei-fpga.obs.cn-north-1.myhwclouds.com/FX300/hardware/sdaccel_design/dsa_v01210125/xilinx_huawei-vu5p_2ddr-dynamic_5_1.dsa

curl -k -s -O --retry 3 https://huawei-fpga.obs.cn-north-1.myhwclouds.com/FX300/hardware/sdaccel_design/dsa_v01210125/xilinx_huawei-vu5p_2ddr-dynamic_5_1.spfm

curl -k -s -O --retry 3 https://huawei-fpga.obs.cn-north-1.myhwclouds.com/FX300/hardware/sdaccel_design/dsa_v01210125/xilinx_huawei-vu5p_2ddr-dynamic_5_1.xpfm
```

#### 说明:

用户下载的网表文件及版本信息可参考[release_note.md](../../release_note.md)文件FX300章节，用户根据自己的需求进行选择下载。

所下载网表文件包含`xilinx_huawei-vu5p_2ddr-dynamic_5_1.dsa、xilinx_huawei-vu5p_2ddr-dynamic_5_1.spfm、xilinx_huawei-vu5p_2ddr-dynamic_5_1.xpfm`三个，下载后存放在以下路径下：

```bash
FX300/sdaccel/hardware/sdaccel_design/lib/platform/xilinx_huawei-vu5p_2ddr-dynamic_5_1
```

### 3.2 基于SDAccel的硬件开发流程

基于SDAccel的硬件开发详细步骤，请参见[基于SDAccel的硬件开发流程](./docs/Implementation_Process_of_SDAccel_based_Hardware_Development_cn.md)。

<a name="sec-4"></a>
## 4 基于SDAccel的软件开发流程

如果用户已完成硬件开发，则可以按照本章内容完成基于SDAccel的软件开发。

### 4.1 编写及编译用户应用

编写及编译用户应用步骤请参见[基于SDAccel的软件开发流程](./docs/SDAccel_based_SDK_Configuration_and_Compilation_cn.md)。

### 4.2 下载Shell版本到用户服务器

下载路径如下（本描述前提为用户基于linux系统环境进行下载，以初始版本`01210125`为例进行说明，用户如果需要下载其他版本时则直接替换掉下面命令中的`01210125`字段即可）：

```bash
curl -k -s -O --retry 3 https://huawei-fpga.obs.cn-north-1.myhwclouds.com/FX300/hardware/sdaccel_design/bin_v01210125/hcf_sdaccel_x_vu5p_1xpr_shell.bin
```

### 4.3 加载Shell版本到FPGA卡

使用工具`fmtk`将下载的shell bin加载到FPGA卡中，工具`fmtk`请登录`http://support.huawei.com/enterprise/zh/index.html`，在搜索框中输入关键字`fmtk`获取软件包。

#### 说明:

加载完成后需要对服务器进行下上电处理，待服务器上电后再进行后续操作。

### 4.4 运行用户应用

执行run.sh完成硬件的动态部分加载及测试环节，命令如下：

```bash
sh run.sh xxx FX300/sdaccel/hardware/sdaccel_design/user/usr_prj/prj/bin/xxx.xclbin 0
```

#### 说明:

上面run.sh脚本后的xxx表示用户生成可执行文件的名称；

上面run.sh脚本末尾的xxx.xclbin文件为用户硬件开发流程环节生成的xclbin文件；

上面run.sh脚本末尾的0表示slot号，该号代表用户所使用环境的FPGA卡号。比如用户服务器带4张FPGA加速卡时，则slot号为0、1、2、3；

run.sh具体使用请执行sh run.sh -h查看。

<a name="sec-5"></a>
## 5 基于SDAccel的Example使用

在通用型服务器架构下，华为FPGA云加速服务提供了三种Example。Example1是一个矩阵乘法实例，基于c实现逻辑算法；Example2是一个矢量相加的实例，采用opencl c实现逻辑算法；Example3是一个矢量相加的实例，采用rtl来实现其逻辑功能。

基于SDAccel的Example详细说明请见[使用基于SDAccel的Example](./docs/Using_an_SDAccel_based_Example_cn.md)。

<a name="sec-6"></a>
## 6  多ddr 使用说明

用例开发是默认只使用ddr0空间，如果用户有用多个DDR的需求，需要用户修改example。具体的修改方法参见[多DDR使用开发指导](./docs/Using Multiple DDR Banks.md)。