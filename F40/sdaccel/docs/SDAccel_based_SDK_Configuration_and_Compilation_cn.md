编译host主机程序
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

[Switch to the English version](./SDAccel_based_SDK_Configuration_and_Compilation.md)

编译host主机程序。用户参考已提供的mmult_hls或vadd_cl或vadd_rtl示例建立用户目录，按照用户编写的host代码及编译选项修改Makefile并进行编译。

```
  cd F40/sdaccel/software/app/sdaccel_app/usr_host
  make
```

编译成功后，在该目录下会生成host可执行文件。

#### 说明:

  `usr_host` 为用户目录，由用户创建。用户编写的源文件、Makefile或脚本等必须放在该目录下。
  用户也可以拷贝示例目录进行修改后作为`usr_host`目录。
