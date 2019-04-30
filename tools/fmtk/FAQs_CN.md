# Frequently Asked Questions
[Switch to the English version](./FAQs.md)

**Q1: 什么是FX300?**

FX300 FPGA加速卡（以下简称FX300）是一张半高半长单槽位PCIe卡，基于Xilinx的Virtex UltraScale+ XCVU5P FPGA芯片，支持PCIe Gen3.0 x16接口，支持2个100GE的QSFP28光口。FX300作为标准PCIe卡，可以用于华为公司的服务器上，也可以用于业界其他供应商的标准服务器。

**Q2:什么是FX600？**

FX600 FPGA加速卡（以下简称FX600）是一张全高3/4长双槽位PCIe卡，基于Xilinx的Virtex UltraScale+ XCVU9P FPGA芯片，支持PCIe Gen3.0 x16接口，支持2个100GE的QSFP28光口。FX600作为标准PCIe卡，可以用于华为公司的服务器上，也可以用于业界其他供应商的标准服务器。

**Q3: 什么是Shell？**

Shell是HDK提供的静态逻辑部分，包括PCIe、DDR4等外围接口设计。

**Q4: 什么是固件生效？**

固件生效是指golden和shell升级后，通过调用相关的API接口完成升级后固件的生效。

**Q5: 什么是固件重加载？**

固件重加载是指用户通过调用相关API完成从指定FLASH区域加载固件。

**Q6：详细信息查询能获取哪些信息？**

详细信息可以提供电子标签信息、光模块信息、DIMM信息、功率、电压、温度等信息。

**Q7：健康状态查询针对哪些场景？**

健康状态查询只针对FX300/FX600加载了华为提供的OCL逻辑场景。