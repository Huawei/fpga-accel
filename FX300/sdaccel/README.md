
SDAccel-based Development and Usage Guide
=======================

[切换到中文版](./README_CN.md)

#### Note:

** In the current version, only the root account can be used to perform operations. Other accounts cannot ensure that the functions are correct. **

Contents
-------------------------

[Environment Dependencies](#sec-1)

[Downloading the Development Suite](#sec-2)

[SDAccel-based Hardware Development Process](#sec-3)

[SDAccel-based Software Development Process](#sec-4)

[SDAccel-based Example Usage](#sec-5)

[Multi-DDR Usage Description](#sec-6)

<a name="sec-1"></a>
## 1 Environment Dependencies

The dependencies of the development suite on the user server are as follows:

| Item                       | Requirement                              |
| -------------------------- | ---------------------------------------- |
| OS                         | Linux CentOS 7.3                         |
| Memory                     | ≥ 20 GByte                               |
| Storage                    | Before installing the SDx tool ≥100 GByte |
| SDx tool installation path | /software                                |
| License features           | (1) XCVU5P, (2) PartialReconfiguration, (3) Simulation 1735 Version 2, (4) xcvu5p_bitgen, and (5) ap_opencl |
| SDx tool version           | 2017.4.op                                |

#### Note:

For details about how to install the SDx tool, see the [SDx_tool_installation_guide_cn](./docs/SDx_tool_installation_guide_cn.docx).


<a name="sec-2"></a>
## 2 Downloading the Development Suite

Download the development suite to the user server. The download path is as follows:

For HTTPS connections, run the `git clone https://github.com/Huawei/fpga-accel.git` command to download the development suite.

For SSH connections, run the `git clone git@github.com/Huawei/fpga-accel.git` command to download the development suite.

<a name="sec-3"></a>
## 3 SDAccel-based Hardware Development Process

### 3.1 Download Network Table Files

Before starting the hardware development process, download the related network table files to the user server. The download path is as follows: (The following information assumes that the user server OS is Linux ant the initial version is `01210125`. If you need to download another version, replace `01210125` in the following commands with the actual version.

```bash
curl -k -s -O --retry 3 https://huawei-fpga.obs.cn-north-1.myhwclouds.com/FX300/hardware/sdaccel_design/dsa_v01210125/xilinx_huawei-vu5p_2ddr-dynamic_5_1.dsa

curl -k -s -O --retry 3 https://huawei-fpga.obs.cn-north-1.myhwclouds.com/FX300/hardware/sdaccel_design/dsa_v01210125/xilinx_huawei-vu5p_2ddr-dynamic_5_1.spfm

curl -k -s -O --retry 3 https://huawei-fpga.obs.cn-north-1.myhwclouds.com/FX300/hardware/sdaccel_design/dsa_v01210125/xilinx_huawei-vu5p_2ddr-dynamic_5_1.xpfm
```

#### Note:

For details about the downloaded network table files and versions, see the section FX300 in [release_note.md](../../release_note.md). You can download the files as required.

The downloaded network table file package includes three files `xilinx_huawei-vu5p_2ddr-dynamic_5_1.dsa, xilinx_huawei-vu5p_2ddr-dynamic_5_1.spfm, and xilinx_huawei-vu5p_2ddr-dynamic_5_1.xpfm`. The downloaded files are stored in the following directories:

```bash
FX300/sdaccel/hardware/sdaccel_design/lib/platform/xilinx_huawei-vu5p_2ddr-dynamic_5_1
```

### 3.2 SDAccel-based Hardware Development Process

For details, see [SDAccell-based Hardware Development Process](./docs/Implementation_Process_of_SDAccel_based_Hardware_Development.md).

<a name="sec-4"></a>
## 4 SDAccel-based Software Development Process

If you have completed the hardware development, you can start the software development using SDAccel.

### 4.1 Configuring and Compiling User Applications

For details about how to configure and compile user applications, see [SDAccel-based Software Development Process](./docs/SDAccel_based_SDK_Configuration_and_Compilation.md).

### 4.2 Downloading the Shell Version to the User Server

The download path is as follows: (The following information assumes that the user server OS is Linux ant the initial version is `01210125`. If you need to download another version, replace `01210125` in the following command with the actual version.)

```bash
curl -k -s -O --retry 3 https://huawei-fpga.obs.cn-north-1.myhwclouds.com/FX300/hardware/sdaccel_design/bin_v01210125/hcf_sdaccel_x_vu5p_1xpr_shell.bin
```

### 4.3 Loading the Shell Version to the FPGA Card

Use the `fmtk` tool to load the downloaded shell bin to the FPGA card. Log in to the `http://support.huawei.com/enterprise/en/index.html` and enter the keyword `fmtk` in the search box to obtain the software package.

#### Note:

After the tool is loaded, power on the server and perform subsequent operations after the server is powered on.

### 4.4 Running User Applications

Run the run.sh script to perform dynamic loading and test of the hardware. The command is as follows:

```bash
sh run.sh xxx FX300/sdaccel/hardware/sdaccel_design/user/usr_prj/prj/bin/xxx.xclbin 0
```

#### Note:

In the preceding command, xxx next to run.sh indicates the name of the executable file generated by the user.

The xxx.xclbin file at the end of the run.sh script is the xclbin file generated during the hardware development process.

0 at the end of the run.sh script indicates the slot number, which indicates the FPGA card number of the user environment. For example, if the user server has four FPGA accelerator cards, the slot numbers are 0, 1, 2, and 3.

Run sh run.sh -h to query how to use run.sh.

<a name="sec-5"></a>
## 5 SDAccel-based Example Usage

When a general-purpose architecture is used, the Huawei FPGA Accelerated Cloud Server (FACS) services provide three examples. Example 1 is an example of matrix multiplication using C to implement the logic algorithm. Example 2 is an example of vector addition using OpenCL C to implement the logic algorithm. Example 3 is an example of vector addition using RTL to implement the logic function.

For details about SDAccel-based examples, see [Using SDAccel-based Examples](./docs/Using_an_SDAccel_based_Example.md).

<a name="sec-6"></a>
## 6 Multi-DDR Usage Description

In the case development, only the ddr0 space is used by default. If you require multiple DDR banks, you can modify the example. For details, see [Multi-DDR Usage Guide](./docs/Using Multiple DDR Banks.md).


