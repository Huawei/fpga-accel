# FPGA 加速卡开发套件特性概述

[Switch to the English version](./release_note.md)

## FX300
* 基于VU5P的FPGA加速卡

* 每个FPGA加速卡可以使用的主要接口如下：
  - `1个pcie gen3 x8 `接口    
  - `2个ddr4` RDIMM接口
  - `2个100Gbps` 以太网接口 

* PCIE支持的特性：
    - `2个PF`(physical function)
    - `PF0`供用户使用，Bar0空间大小为32MB，Bar1空间大小为64KB。
    - `PF1`管理侧使用，Bar0空间大小为32MB，Bar1空间大小为128KB。

* 用户逻辑和静态逻辑之间的接口特性:
    - 用户逻辑和静态逻辑之间的数据通道是`512 bit`位宽的AXI4-MM接口
    - 用户逻辑和静态逻辑之间的控制通道是`32 bit`位宽的AXI4-Lite接口


* DDR接口划分：
  - 2个DDR控制器放置在用户逻辑部分
  - 支持用户最多使用`2个DDR控制器`


### Release 1.1.1

- 版本号01210127
- 合入IIC 时钟延展特性

### Release 1.1.0

- 版本号01210126
- 合入hot_reset

### Release 1.0.0

- 版本号01210125
- 支持SDx 2017.4.op开发套件
- 支持1:N多卡场景
- 支持多shell版本共存场景
- 文档优化

---
# FX300 FPGA实例特性详述

# 目录

## 1. [工程构建](#工程构建)
## 2. [应用测试](#应用测试)
## 4. [工具环境](#工具环境)
## 5. [license要求](#license要求)
## 6. [即将支持特性](#即将支持特性)
***
<a name="工程构建"></a>
# 工程构建

## 概述
执行工程构建之前必须要`确认SDx_2017.4.op工具及license安装完成`；工程构建旨在用户通过最小的改动实现符合时序要求的工程设计。

## 特性列表

* 支持sdaccel高级语言设计

* 支持用户使用`opencL、c和c++`编码代码

* 支持`一键式`创建用户目录

* SHELL的SHA256校验

---

<a name="应用测试"></a>

# 应用测试

## 概述

用户可以参考已有的app 工程子目录进行应用设计，对工程进行特性或功能测试。

---

<a name="工具环境"></a>

# 工具环境

* 支持的工具和环境如下：
  - Linux `centos 7.3`  
  - Xilinx `SDx 2017.4.op` 

---

<a name="license要求"></a>

# license要求
* 需要的license如下      
  - SysGen  
  - PartialReconfiguration  
  - Simulationt  
  - Analyzer  
  - HLS  
  - ap_opencl  
  - XCVU5P  
  - xcvu5p_bitgen   

---
<a name="F100"></a>

## FX600

* 基于VU9P的FPGA加速卡的开发套件

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

### Release 1.1.2

- 去除FX600 sdaccel套件

### Release 1.1.1

- 版本号01210165
- 合入IIC 时钟延展特性

### Release 1.1.0

- 版本号01210163
- 合入hot_reset

### Release 1.0.0

- 版本号01210162
- 支持SDx 2017.4.op开发套件
- 支持1:N多卡场景
- 支持多shell版本共存场景
- 文档优化

---
# FX600 FPGA实例特性详述

# 目录

## 1. [工程构建](#工程构建)
## 2. [应用测试](#应用测试)
## 4. [工具环境](#工具环境)
## 5. [license要求](#license要求)
## 6. [即将支持特性](#即将支持特性)
***
<a name="工程构建"></a>
# 工程构建

## 概述
执行工程构建之前必须要`确认SDx_2017.4.op工具及license安装完成`；工程构建旨在用户通过最小的改动实现符合时序要求的工程设计。

## 特性列表

* 支持sdaccel高级语言设计

* 支持用户使用`opencL、c和c++`编码代码

* 支持`一键式`创建用户目录

* SHELL的SHA256校验

---

<a name="应用测试"></a>

# 应用测试

## 概述

用户可以参考已有的app 工程子目录进行应用设计，对工程进行特性或功能测试。

---

<a name="工具环境"></a>

# 工具环境

* 支持的工具和环境如下：
  - Linux `centos 7.3`  
  - Xilinx `SDx 2017.4.op` 

---

<a name="license要求"></a>

# license要求
* 需要的license如下      
  - SysGen  
  - PartialReconfiguration  
  - Simulationt  
  - Analyzer  
  - HLS  
  - ap_opencl  
  - XCVU9P  
  - xcvu9p_bitgen   

