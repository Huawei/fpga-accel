基于SDAccel的开发使用指南
=======================

[Switch to the English version](./README.md)

#### 说明:

当前版本仅限root账号进行相关操作，其他账号不能保证功能正确。

目录
-------------------------

[环境依赖](#sec-1)

[下载开发套件](#sec-2)

[基于SDAccel的硬件开发流程](#sec-3)

[基于SDAccel的软件开发流程](#sec-4)

[基于SDAccel的Example使用](#sec-5)

<a name="sec_1"></a>
## 1 环境依赖

本开发套件对用户服务器的依赖如下：

| 类型        | 说明                                       |
| --------- | ---------------------------------------- |
| 操作系统      | Linux centos 7.3                         |
| 内存        | >=20GByte                                |
| 硬盘        | 安装SDx工具前不小于100GByte                      |
| SDx工具安装路径 | 必须安装在`/software`下                        |
| license特性 | （1）XCVU9P与XCVU5P、（2）PartialReconfiguration、（3）Simulation 1735 Version 2、（4）xcvu9p_bitgen与xcvu5p_bitgen、（5）ap_opencl |
| SDx工具安装指导 | `./docs/SDx_tool_installation_guide_cn.docx` |


<a name="sec_2"></a>
## 2 下载开发套件

用户下载开发套件到用户服务器，下载路径如下：

```bash
https://github.com/Huawei/fpga-accel.git
```

<a name="sec_3"></a>
## 3 基于SDAccel的硬件开发流程

### 3.1 下载网表文件

用户在开始硬件开发流程前，需要下载相关网表文件到用户服务器，具体下载路径如下：

```bash
https://huawei-fpga.obs.cn-north-1.myhwclouds.com
```

#### 说明:

下载的网表文件及版本信息可参考F100文件夹下的release_note.txt文件，用户根据需求进行下载。

所下载网表文件需存放在以下路径：

```bash
F100/sdaccel/hardware/sdaccel_design/lib/platform/xilinx_huawei-vu5p_2ddr-dynamic_5_1
```

### 3.2 基于SDAccel的硬件开发流程

基于SDAccel的硬件开发详细步骤，请参见[基于SDAccel的硬件开发流程](./docs/Implementation_Process_of_SDAccel_based_Hardware_Development_cn.md)。

<a name="sec_4"></a>
## 4 基于SDAccel的软件开发流程

如果用户已完成硬件开发，则可以按照本章内容完成基于SDAccel的软件开发。

### 4.1 编写及编译用户应用

编写及编译用户应用步骤请参见[基于SDAccel的软件开发流程](./docs/SDAccel_based_SDK_Configuration_and_Compilation_cn.md)。

### 4.2 下载Shell版本到用户服务器

下载路径如下：

```bash
https://huawei-fpga.obs.cn-north-1.myhwclouds.com
```
#### 说明:

下载的shell文件及版本信息可参考Shared-development-tx/F100/ocl_bin/文件夹下的文件，用户根据需求进行下载。

该shell版本匹配有相应的sha256校验文件，名称与shell版本一致，用户可对其进行相应校验以检验正确性。


### 4.3 加载Shell版本到FPGA卡

使用开发套件中的脚本`fmtk`将下载的shell版本加载到FPGA卡中，该脚本路径如下：

```bash
F100/tools
```

#### 说明:

脚本`fmtk`使用方式可通过命令`./fmtk -h`进行查看；
加载完成后需要对服务器进行下上电处理，待服务器上电后再进行后续操作。

### 4.4 运行用户应用

执行run.sh完成硬件的动态部分加载及测试环节，命令如下：

```bash
sh run.sh mmult F100/sdaccel/hardware/sdaccel_design/user/usr_prj/prj/bin/xxx.xclbin 0
```

#### 说明:

上面run.sh脚本末尾的xxx.xclbin文件为用户硬件开发流程环节生成的xclbin文件；

上面run.sh脚本末尾的0表示slot号，该号代表用户所使用环境的FPGA卡号。比如用户服务器带4张FPGA加速卡时，则slot号为0、1、2、3；

run.sh具体使用请执行sh run.sh -h查看。

<a name="sec_5"></a>
## 5 基于SDAccel的Example使用

在通用型服务器架构下，华为FPGA云加速服务提供了三种Example。Example1是一个矩阵乘法实例，基于c实现逻辑算法；Example2是一个矢量相加的实例，采用opencl c实现逻辑算法；Example3是一个矢量相加的实例，采用rtl来实现其逻辑功能。

基于SDAccel的Example详细说明请见[使用基于SDAccel的Example](./docs/Using_an_SDAccel_based_Example_cn.md)。