
Matrix Multiplication Example Using C for Kernel Code Development
================================

[切换到中文版](./README_CN.md)

This is an example of 16 x 16 OpenCL matrix multiplication, using C for kernel code development. 

This example includes:

Example File
---------------------
Application layer host code file

* test-cl.cpp

Compilation Script
--------------------------------
* Makefile: A script for compiling host code at the application layer.

**Note:**
--------------------------------
* The xcpp tool can be used only after SDx is installed. 

Compile the host code and then run the **run.sh** command to test the case. The command is as follows:

```bash
make
sh run.sh mmult ../../../../hardware/sdaccel_design/examples/mmult_hls/prj/bin/bin_mmult_hw.xclbin 0

```

The value 0 at the end of the run.sh script indicates the slot number, which is obtained when the user applies for the environment. For example, if a user applies for a VM with four FPGA accelerator cards, the slot numbers are 0, 1, 2, and 3.


