
# FPGA Accelerator Card Development Suite Feature Overview

[切换到中文版](./release_note_cn.md)

## FX300
* VU5P-based FPGA accelerator card

* Interfaces available for each FPGA accelerator card are as follows:
  - `One PCIe 3.0 x8` interface    
  - `Two DDR4` RDIMM interfaces
  - `Two 100 Gbit/s` Ethernet interfaces 

* PCIe features are as follows:
    - `Two physical functions (PFs)`
    - `PF0` is used by users. The size of Bar0 is 32 MB, and the size of Bar1 is 64 KB.
    - `PF1` is for management. The size of Bar0 space is 32 MB, and the size of Bar1 is 128 KB.

* Interface features between user logic and static logic are as follows:
    - The data channel from user logic to static logic uses the AXI4-MM interface with a bit width of `512 bits`.
    - The control channel from user logic to static logic uses the AXI4-Lite interface with a bit width of `32 bits`.


* The DDR interface partition is as follows:
  - Two DDR controllers are placed in the user logic partition.
  - A maximum of two DDR controllers can be used.


### Release 1.1.1

- Version 01210127
- Added the IIC clock extension feature.

### Release 1.1.0

- Version 01210126
- Added hot_reset.

### Release 1.0.0

- Version 01210125
- Supported the SDx 2017.4.op development suite.
- Supported the 1:N multi-card scenario.
- Supported coexistence of multiple shell versions.
- Optimized the document.

---
# FX300 FPGA Example Feature Description

# Contents

## 1 [Project Building](#Project Building)
## 2 [Application Tests](#Application Tests)
## 4 [Tools and Environment](#Tools and Environment)
## 5 [License Requirements](#License Requirements)
## 6 [Features To Be Supported](#Features To Be Supported)
***
<a name="Project Building"></a>
# Project Building

## Overview
Before building a project, `ensure that SDx_2017.4.op tool and license are installed`. The project building aims to design a project that meets the timing requirements through the minimum modification.

## Features

* SDAccel advanced language design

* `OpenCL, C, and C++` coding

* `One-click` user directory creation

SHA256 verification of * SHELL

---

<a name="Application Tests"></a>

# Application Tests

## Overview

You can use the existing app project subdirectory to design applications and test features or functions of the project.

---

<a name="Tools and Environment"></a>

# Tools and Environment

* The supported tools and environment are as follows:
  - Linux `centos 7.3`  
  - Xilinx `SDx 2017.4.op` 

---

<a name="License Requirements"></a>

# License Requirements
* The required licenses are as follows:      
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

* VU9P-based FPGA accelerator card development suite

* Interfaces available for each FPGA accelerator card are as follows:
  - `One PCIe 3.0 x16` interface    
  - `Four DDR4` RDIMM interfaces
  - `Two 100 Gbit/s` Ethernet interfaces 

* PCIe features are as follows:
    - `Two physical functions (PFs)`
    - `PF0` is for users. The size of Bar0 is 32 MB, and the size of Bar1 is 64 KB.
    - `PF1` is for management. The size of Bar0 space is 32 MB, and the size of Bar1 is 128 KB.

* Interface features between user logic and static logic are as follows:
    - The data channel from user logic to static logic uses the AXI4-MM interface with a bit width of `512 bits`.
    - The control channel from user logic to static logic uses the AXI4-Lite interface with a bit width of `32 bits`.


* The DDR interface partition is as follows:
  - Four DDR controllers are placed in the user logic partition.
  - A maximum of four DDR controllers can be used.

### Release 1.1.2

- Removed the FX600 SDAccel suite.

### Release 1.1.1

- Version 01210165
- Added the IIC clock extension feature.

### Release 1.1.0

- Version 01210163
- Added hot_reset.

### Release 1.0.0

- Version 01210162
- Supported the SDx 2017.4.op development suite.
- Supported the 1:N multi-card scenario.
- Supported coexistence of multiple shell versions.
- Optimized the document.

---
# FX600 FPGA Example Feature Description

# Contents

## 1 [Project Building](#Project Building)
## 2 [Application Tests](#Application Tests)
## 4 [Tools and Environment](#Tools and Environment)
## 5 [License Requirements](#License Requirements)
## 6 [Features To Be Supported](#Features To Be Supported)
***
<a name="Project Building"></a>
# Project Building

## Overview
Before building a project, `ensure that SDx_2017.4.op tool and license are installed`. The project building aims to design a project that meets the timing requirements through the minimum modification.

## Features

* SDAccel advanced language design

* `OpenCL, C, and C++` coding

* `One-click` user directory creation

SHA256 verification of * SHELL

---

<a name="Application Tests"></a>

# Application Tests

## Overview

You can use the existing app project subdirectory to design applications and test features or functions of the project.

---

<a name="Tools and Environment"></a>

# Tools and Environment

* The supported tools and environment are as follows:
  - Linux `centos 7.3`  
  - Xilinx `SDx 2017.4.op` 

---

<a name="License Requirements"></a>

# License Requirements
* The required licenses are as follows:      
  - SysGen  
  - PartialReconfiguration  
  - Simulationt  
  - Analyzer  
  - HLS  
  - ap_opencl  
  - XCVU9P  
  - xcvu9p_bitgen   




