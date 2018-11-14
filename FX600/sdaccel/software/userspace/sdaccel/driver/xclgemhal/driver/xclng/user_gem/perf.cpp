/**
 * Copyright (C) 2016-2017 Xilinx, Inc
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

/*
 * Copyright (C) 2015 Xilinx, Inc
 * Author: Paul Schumacher
 * Performance Monitoring using PCIe for XDMA HAL Driver
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

#include "shim.h"
//#include "datamover.h"
#include "driver/xclng/include/mgmt-reg.h"
#include "driver/xclng/include/mgmt-ioctl.h"
#include "driver/xclng/include/xocl_ioctl.h"
#include "driver/include/xclperf.h"
#include "perfmon_parameters.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <cstdio>
#include <cstring>
#include <cassert>
#include <algorithm>
#include <thread>
#include <vector>
#include <time.h>
#include <string.h>

#ifndef _WINDOWS
// TODO: Windows build support
//    unistd.h is linux only header file
//    it is included for read, write, close, lseek64
#include <unistd.h>
#endif

#ifdef _WINDOWS
#define __func__ __FUNCTION__
#endif

#define FAST_OFFLOAD_MAJOR 2
#define FAST_OFFLOAD_MINOR 2

namespace xocl {

  static int unmgdPread(int fd, void *buffer, size_t size, uint64_t addr)
  {
    drm_xocl_pread_unmgd unmgd = {0, 0, addr, size, reinterpret_cast<uint64_t>(buffer)};
    return ioctl(fd, DRM_IOCTL_XOCL_PREAD_UNMGD, &unmgd);
  }
  // Memory alignment for DDR and AXI-MM trace access
  template <typename T> class AlignedAllocator {
      void *mBuffer;
      size_t mCount;
  public:
      T *getBuffer() {
          return (T *)mBuffer;
      }

      size_t size() const {
          return mCount * sizeof(T);
      }

      AlignedAllocator(size_t alignment, size_t count) : mBuffer(0), mCount(count) {
          if (posix_memalign(&mBuffer, alignment, count * sizeof(T))) {
              mBuffer = 0;
          }
      }
      ~AlignedAllocator() {
          if (mBuffer)
      	  {
              free(mBuffer);
              mBuffer = 0;
       	  }
      }
  };

  // ****************
  // Helper functions
  // ****************

  bool XOCLShim::isDSAVersion(unsigned majorVersion, unsigned minorVersion, bool onlyThisVersion) {
    unsigned checkVersion = (majorVersion << 4) + (minorVersion);
    if (onlyThisVersion)
      return (mDeviceInfo.mDeviceVersion == checkVersion);
    return (mDeviceInfo.mDeviceVersion >= checkVersion);
  }

  unsigned XOCLShim::getBankCount() {
    return mDeviceInfo.mDDRBankCount;
  }
  
   // Convert binary string to decimal
  uint32_t XOCLShim::bin2dec(std::string str, int start, int number) {
    return bin2dec(str.c_str(), start, number);
  }

  // Convert binary char * to decimal
  uint32_t XOCLShim::bin2dec(const char* ptr, int start, int number) {
    const char* temp_ptr = ptr + start;
    uint32_t value = 0;
    int i = 0;

    do {
      if (*temp_ptr != '0' && *temp_ptr!= '1')
        return value;
      value <<= 1;
      if(*temp_ptr=='1')
        value += 1;
      i++;
      temp_ptr++;
    } while (i < number);

    return value;
  }

  // Convert decimal to binary string
  // NOTE: length of string is always sizeof(uint32_t) * 8
  std::string XOCLShim::dec2bin(uint32_t n) {
    char result[(sizeof(uint32_t) * 8) + 1];
    unsigned index = sizeof(uint32_t) * 8;
    result[index] = '\0';

    do {
      result[ --index ] = '0' + (n & 1);
    } while (n >>= 1);

    for (int i=index-1; i >= 0; --i)
      result[i] = '0';

    return std::string( result );
  }

  // Convert decimal to binary string of length bits
  std::string XOCLShim::dec2bin(uint32_t n, unsigned bits) {
    char result[bits + 1];
    unsigned index = bits;
    result[index] = '\0';

    do result[ --index ] = '0' + (n & 1);
    while (n >>= 1);

    for (int i=index-1; i >= 0; --i)
      result[i] = '0';

    return std::string( result );
  }

} // namespace xocl_gem


// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
