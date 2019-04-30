组件使用说明
=======

[Switch to the English version](./README.md)

目录
-------
[简介](#a)

[文件列表](#b)

[组件集成方法](#c)

<a name="a"></a>

简介
-------
华为向用户提供了多种功能组件（DCP类型），用户可以按照组件集成指导书，快速地将这些组件集成到自己的工程之中，实现逻辑烧写升级、加速卡系统状态读取等多种功能。

<a name="b"></b>

文件列表
--------
当前华为向用户提供以下文件：

| 名称          | 描述                           |
|:--------      |:-----------                       |
| FLASH    |FLASH控制器组件，用于逻辑烧写FLASH     |
| ICAP   |ICAP控制器组件，用于逻辑warmboot升级 |
| MCU_MST |MCU_MST组件，用于读取加速卡系统状态  |
| Example Project |集成多个组件的示例工程，用户可以参考工程中的连接方式 |

<a name="c"></c>

组件集成方法
--------
请参考下列超链接文档：
[FLASH 组件的集成开发指导](./FLASH/README_CN.md)
[ICAP 组件的集成开发指导](./ICAP/README_CN.md)
[MCU_MST组件的集成开发指导](./MCU_MST/README_CN.md)
[Example Project样例工程使用方法](./example_prj/README_CN.md)