# Huawei Cloud FPGA - Frequently Asked Questions

[概况 FAQs](#概况) 

[应用市场 FAQs](#应用市场)

[开发语言 FAQs](#开发语言)

[FPGA  FAQs](#fpga)

[FPGA Shell FAQs](#fpga shell) 

[疑难解答](#疑难解答)

[高危操作](#高危操作)


## 概况

**Q: 什么是HDK?**

HDK是Hardware Development Kit的缩写，即硬件开发套件。HDK主要包括RTL（Verilog/VHDL）设计、验证到构建的全部设计文件以及脚本。

**Q: 什么是SDK?**

SDK是Software Development Kit的缩写，即软件开发套件。SDK主要包含运行FPGA实例所需要的驱动、工具、运行环境以及应用程序。


## 应用市场
**Q: Huawei Cloud FPGA Marketplace是什么？**

FPGA开发人员可以使用Marketplace将他们的AEI文件共享或出售给其他Huawei Cloud FPGA用户。 


## 开发语言
**Q: Huawei Cloud FPGA支持哪些开发语言?**

目前主要采用C语言开发。


## FPGA
**Q: Huawei Cloud FPGA使用的FPGA型号是什么?**

目前Huawei Cloud FPGA使用的FGPA是Xilinx公司的Ultra Scale+系列xcvu5p-flva2104-2-i板卡。


## FPGA Shell
**Q: 什么是Shell？**

Shell是HDK提供的静态逻辑部分，包括PCIe、DDR4等外围接口设计。

**Q: Shell的接口协议是axi4吗?**

不完全是。目前Shell接口使用的协议有axi4，axi4-s，axi4-l三种协议，主要和不同接口处理的数据类型不同有关。

## 疑难解答

**Q：使用Vivado软件工具后产生的license缓存文件有安全风险，应该如何处理？**

Xilinx的FPGA设计工具Vivado，使用时会检查license，并在
`/tmp/FLEXnet/` 
`/usr/local/share/macrovision/storage/`
`/usr/local/share/macrovision/storage/FLEXnet`
三个目录下产生缓存文件。由于这些缓存文件具有全局可写权限，所以其数据可以被系统中的任何用户修改和破坏，可能对系统造成危害。

**处理方式**：这些文件仅在Vivado软件检查license时使用，并且每次使用都会重新生成，所以建议用户每次使用Vivado工具后，手动或编写脚本删除这3个目录下的所有文件，删除这些文件不会对下一次使用Vivado软件造成影响。

**Q：如何设计出高性能高质量的代码？**

通用型架构FPGA开发请参考https://www.xilinx.com/support/documentation/sw_manuals/xilinx2017_4/ug1207-sdaccel-optimization-guide.pdf


## 高危操作

**Q：在OCL SDK操作中执行xbsk test过程中按ctrl+c按键，再次执行xbsk test时会失败**

禁止执行此操作。如果不小心已执行了，则需要重新安装xdma驱动才能恢复。

**Q：在OCL SDK操作中执行example的过程中按ctrl+c按键，再次执行example时会失败**

禁止执行此操作。如果不小心已执行了，则用FpgaCmdEntry工具手动执行加载一次才能恢复。


