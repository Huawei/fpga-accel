
Compiling the Host Program
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

[切换到中文版](./SDAccel_based_SDK_Configuration_and_Compilation_cn.md)


Compile the host program. Create a user directory by referring to the examples mmult_hls, vadd_cl, or vadd_rtl, and modify and compile Makefile according to the user-developed host code and compilation options.

```
  cd FX300/sdaccel/software/app/sdaccel_app/usr_host
  make
```

After the compilation is complete, the executable host file is generated in this directory.

#### Note:

  *usr_host* indicates the user directory, which is created by users. Source files, Makefile, or scripts compiled by users must be stored in this directory.
  You can also copy and rename the example directory and use it as the *usr_host* directory.



