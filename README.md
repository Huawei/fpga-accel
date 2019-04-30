
# Description

[切换到中文版](./README_CN.md)

---

## FX300

* VU5P-based FPGA accelerator card development suite

* Interfaces available for each FPGA accelerator card are as follows:
  - `One PCIe 3.0 x8` interface    
  - `Two DDR4` RDIMM interfaces
  - `Two 100 Gbit/s` Ethernet interfaces 

* PCIe features are as follows:
    - `Two physical functions (PFs)`
    - `PF0` is for users. The size of Bar0 is 32 MB, and the size of Bar1 is 64 KB.
    - `PF1` is for management. The size of Bar0 space is 32 MB, and the size of Bar1 is 128 KB.

* Interface features between user logic and static logic are as follows:
    - The data channel from user logic to static logic uses the AXI4-MM interface with a bit width of `512 bits`.
    - The control channel from user logic to static logic uses the AXI4-Lite interface with a bit width of `32 bits`.


* The DDR interface partition is as follows:
  - Two DDR controllers are placed in the user logic partition.
  - A maximum of two DDR controllers can be used.

* Based on the SDx 2017.4.op software platform and support for the development and design of the `C/C++/OpenCL` language.

* For details, see [README](./FX300/sdaccel/README.md).

## FX600

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

* Provides FLASH CTRL, HWICAP CTRL, and IIC MASTER components and example projects.




