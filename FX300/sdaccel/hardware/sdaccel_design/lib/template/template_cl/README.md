
# Description

[切换到中文版](./README_CN.md)

This directory stores the `project directory template`, aiming to standardizing and simplifying construction projects. The template can also be used as a reference for users to build their own projects.

# Directory Structure
[template_XX](#template_XX_dir)/  

- prj
  - bin
  - log
- sim
- src
- scripts

# File and Folder Description
- prj

  - prj/bin

  This directory stores the executable files and target files, such as xclbin, generated after compilation.

  - prj/log

  This directory stores logs generated after the execution of target files.
- sim

  User Simulation

- src

  This directory stores host source code and kernel source code. The directory contains basic makefile, which can be modified as required. For details, see src README_CN.md.

- scripts

  This directory stores compilation and execution scripts.

    compile.sh

  This is the compilation script. For details, see `sh compile.sh -h` or `sh compile.sh --help`.

  run.sh

  This is the execution script. For details, see `sh run.sh -h` or `sh run.sh --help`.




