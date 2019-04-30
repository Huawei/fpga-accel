# 说明

[Switch to the English version](./README.md)

---

## FX300

* 基于VU5P的FPGA加速卡的开发套件

* 每个FPGA加速卡可以使用的主要接口如下：
  - `1个pcie gen3 x8 `接口    
  - `2个ddr4` RDIMM接口
  - `2个100Gbps` 以太网接口 

* PCIE支持的特性：
    - `2个PF`(physical function)
    - `PF0`供用户使用，Bar0空间大小为32MB，Bar1空间大小为64KB
    - `PF1`管理侧使用，Bar0空间大小为32MB，Bar1空间大小为128KB

* 用户逻辑和静态逻辑之间的接口特性:
    - 用户逻辑和静态逻辑之间的数据通道是`512 bit`位宽的AXI4-MM接口
    - 用户逻辑和静态逻辑之间的控制通道是`32 bit`位宽的AXI4-Lite接口


* DDR接口划分：
  - 2个DDR控制器放置在用户逻辑部分
  - 支持用户最多使用`2个DDR控制器`

* 基于SDx 2017.4.op软件平台，支持`c/c++/opencl`语言开发设计

* 详细使用方法见：[README](./FX300/sdaccel/README_CN.md)

## FX600

* 每个FPGA加速卡可以使用的主要接口如下：
  - `1个pcie gen3 x 16 `接口    
  - `4个ddr4` RDIMM接口
  - `2个100Gbps` 以太网接口 

* PCIE支持的特性：
    - `2个PF`(physical function)
    - `PF0`供用户使用，Bar0空间大小为32MB，Bar1空间大小为64KB
    - `PF1`管理侧使用，Bar0空间大小为32MB，Bar1空间大小为128KB

* 用户逻辑和静态逻辑之间的接口特性:
    - 用户逻辑和静态逻辑之间的数据通道是`512 bit`位宽的AXI4-MM接口
    - 用户逻辑和静态逻辑之间的控制通道是`32 bit`位宽的AXI4-Lite接口

* DDR接口划分：
  - 4个DDR控制器放置在用户逻辑部分
  - 支持用户最多使用`4个DDR控制器`

* 提供FLASH CTRL、HWICAP CTRL、IIC MASTER组件与样例工程

