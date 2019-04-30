
Vector Addition Example Using OpenCL C for Kernel Code Development
===============================

[切换到中文版](./README_CN.md)

This is an example of OpenCL vector addition, using OpenCL C for kernel code development. 

This example includes:
---------------------
Application layer host code file

- vadd.cpp
- vadd.h


Compilation Script
--------------------------------
* Makefile: A script for compiling host code at the application layer.

**Note:**
--------------------------------
* The xcpp tool can be used only after SDx is installed. 


Compile the host code and then run the **run.sh** command to test the case. The command is as follows:

```
make
sh run.sh vadd ../../../../hardware/sdaccel_design/examples/vadd_cl/prj/bin/bin_vadd_hw.xclbin 0

```

The value 0 at the end of the run.sh script indicates the slot number, which is obtained when the user applies for the environment. For example, if a user applies for a VM with four FPGA accelerator cards, the slot numbers are 0, 1, 2, and 3.


