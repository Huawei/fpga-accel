/**
 * Copyright (C) 2016-2017 Xilinx, Inc
 * Author(s): Hem C Neema
 * PCIe HAL Driver layered on top of XOCL GEM kernel driver
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You may
 * not use this file except in compliance with the License. A copy of the
 * License is located at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include <errno.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <cstring>
#include <unistd.h>
#include <sys/file.h>

#include "driver/include/xclbin.h"
#include "scan.h"
#include "xbsak.h"

