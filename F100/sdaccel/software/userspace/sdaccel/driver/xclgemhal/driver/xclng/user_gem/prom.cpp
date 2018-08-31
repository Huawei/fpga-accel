/**
 * Copyright (C) 2015-2017 Xilinx, Inc
 * In-System Programming of BPI PROM using PCIe
 * Based on XAPP518 (v1.3) April 23, 2014
 * Author: Sonal Santan
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

#include <iostream>
#include <string>
#include <list>
#include <fstream>
#include <cassert>
#include <thread>
#include <cstring>

#include <sys/ioctl.h>

#include "shim.h"
#include "driver/xclng/include/mgmt-ioctl.h"

#ifdef WINDOWS
#define __func__ __FUNCTION__
#endif

namespace xocl {

/*
 * freezeAXIGate
 */
int XOCLShim::freezeAXIGate() {
    if (mLogStream.is_open()) {
        mLogStream << __func__ << ", " << std::this_thread::get_id() << std::endl;
    }
    unsigned char buf = 0x0;
    return pcieBarWrite(SHIM_MGMT_BAR, AXI_GATE_OFFSET, &buf, 1);
}

/*
 * freeAXIGate
 */
int XOCLShim::freeAXIGate() {
    if (mLogStream.is_open()) {
        mLogStream << __func__ << ", " << std::this_thread::get_id() << std::endl;
    }
    // First pulse the OCL RESET. This is important for PR with multiple
    // clocks as it resets the edge triggered clock converter FIFO
#ifndef _WINDOWS
    const timespec interval = {0, 500};
#endif
    unsigned char buf = 0x2;
    if (pcieBarWrite(SHIM_MGMT_BAR, AXI_GATE_OFFSET, &buf, 1))
        return -1;
    buf = 0x0;
#ifndef _WINDOWS
    // TODO: Windows build support
    //  nanosleep is defined in unistd.h
    nanosleep(&interval, 0);
#endif
    if (pcieBarWrite(SHIM_MGMT_BAR, AXI_GATE_OFFSET, &buf, 1))
        return -1;
    buf = 0x2;
#ifndef _WINDOWS
    // TODO: Windows build support
    //  nanosleep is defined in unistd.h
    nanosleep(&interval, 0);
#endif
    if (pcieBarWrite(SHIM_MGMT_BAR, AXI_GATE_OFFSET, &buf, 1))
        return -1;
    buf = 0x3;
#ifndef _WINDOWS
    // TODO: Windows build support
    //    nanosleep is defined in unistd.h
    nanosleep(&interval, 0);
#endif
    return pcieBarWrite(SHIM_MGMT_BAR, AXI_GATE_OFFSET, &buf, 1);
}

/*
 * prepare_microblaze
 */
int XOCLShim::prepare_microblaze(unsigned startAddress, unsigned endAddress) {
    int status = 0;
    //Send the "hi" msb address command
    std::cout << "sending hi cmd" << std::endl;
    unsigned addrHi = startAddress;
    addrHi = (addrHi >>= 24) & 0xF;
    unsigned endAddressHi = (endAddress >>24)& 0x3;
    unsigned cmdHi = START_ADDR_HI_CMD;
    unsigned versionCmd = VERSION_CMD;
    cmdHi |= addrHi;

    // Drain mailbox
    pcieBarRead(SHIM_MGMT_BAR, BPI_FLASH_OFFSET+0x10, &status, 4);
    while(!(status&0x1)){ //0: fifo is not empty
        if(pcieBarRead(SHIM_MGMT_BAR, BPI_FLASH_OFFSET+0x8, &status, 4)) {
            return -1;
        }
        pcieBarRead(SHIM_MGMT_BAR, BPI_FLASH_OFFSET+0x10, &status, 4);
    }
    std::cout << "INFO: Finished draining mailbox\n";
    // Check for Ready
    if (pcieBarWrite(SHIM_MGMT_BAR, BPI_FLASH_OFFSET, &cmdHi, 4)) {
        return -1;
    }
    if (waitAndFinish_microblaze(READY_STAT, 0xff)) {
        return -1;
    }
    std::cout << "INFO: Finished waiting for READY_STAT\n";

    if (pcieBarWrite(SHIM_MGMT_BAR, BPI_FLASH_OFFSET, &cmdHi, 4)) {
        return -1;
    }
    std::cout << "done sending hi cmd " << std::hex << cmdHi << std::dec<< std::endl;
    //End sending "hi" msb address command
    startAddress &= 0x00ffffff; // truncate to 24 bits
    startAddress >>= 8; // Pick the middle 16 bits
    endAddress &= 0x00ffffff; // truncate to 24 bits

    if (waitForReady_microblaze(READY_STAT)) {
        return -1;
    }

    std::cout << "INFO: Sending the address range\n";
    // Send start and end address
    unsigned command = START_ADDR_CMD;
    command |= startAddress;
    if (pcieBarWrite(SHIM_MGMT_BAR, BPI_FLASH_OFFSET, &command, 4)) {
        return -1;
    }

    command = END_ADDR_CMD;
    command |= endAddress;
    if (pcieBarWrite(SHIM_MGMT_BAR, BPI_FLASH_OFFSET, &command, 4)) {
        return -1;
    }

    command = END_ADDR_HI_CMD;
    command |= endAddressHi;
    if (pcieBarWrite(SHIM_MGMT_BAR, BPI_FLASH_OFFSET, &command, 4)) {
        return -1;
    }

    //  if (waitForReady(READY_STAT)) {
    //    return -1;
    //  }

    std::cout << "INFO: Sending unlock command\n";
    // Send unlock command
    command = UNLOCK_CMD;
    if (pcieBarWrite(SHIM_MGMT_BAR, BPI_FLASH_OFFSET, &command, 4)) {
        return -1;
    }
    if (waitForReady_microblaze(READY_STAT)) {
        return -1;
    }

    // Send erase command
    std::cout << "INFO: Sending erase command\n";
    command = ERASE_CMD;
    if (pcieBarWrite(SHIM_MGMT_BAR, BPI_FLASH_OFFSET, &command, 4)) {
        return -1;
    }
    // now hanging here
    if (waitForReady_microblaze(ERASE_STAT)) {
        return -1;
    }

    if (waitForReady_microblaze(READY_STAT)) {
        return -1;
    }

    // Send program command
    std::cout << "INFO: Erasing the address range\n";
    command = PROGRAM_CMD;
    if (pcieBarWrite(SHIM_MGMT_BAR, BPI_FLASH_OFFSET, &command, 4)) {
        return -1;
    }

    if (waitForReady_microblaze(PROGRAM_STAT)) {
        return -1;
    }

    return 0;
}

/*
 * prepare
 */
int XOCLShim::prepare(unsigned startAddress, unsigned endAddress) {
    //Send the "hi" msb address command
    std::cout << "sending hi cmd" << std::endl;
    unsigned addrHi = startAddress;
    addrHi = (addrHi >>= 24) & 0xF;
    unsigned endAddressHi = (endAddress >>24)& 0x3;
    unsigned cmdHi = START_ADDR_HI_CMD;
    cmdHi |= addrHi;
    if (pcieBarWrite(SHIM_MGMT_BAR, BPI_FLASH_OFFSET, &cmdHi, 4)) {
        return -1;
    }
    std::cout << "done sending hi cmd " << std::hex << cmdHi << std::dec<< std::endl;
    //End sending "hi" msb address command
    startAddress &= 0x00ffffff; // truncate to 24 bits
    startAddress >>= 8; // Pick the middle 16 bits
    endAddress &= 0x00ffffff; // truncate to 24 bits

    if (waitForReady(READY_STAT)) {
        return -1;
    }

    std::cout << "INFO: Sending the address range\n";
    // Send start and end address
    unsigned command = START_ADDR_CMD;
    command |= startAddress;
    if (pcieBarWrite(SHIM_MGMT_BAR, BPI_FLASH_OFFSET, &command, 4)) {
        return -1;
    }

    command = END_ADDR_CMD;
    command |= endAddress;
    if (pcieBarWrite(SHIM_MGMT_BAR, BPI_FLASH_OFFSET, &command, 4)) {
        return -1;
    }

    command = END_ADDR_HI_CMD;
    command |= endAddressHi;
    if (pcieBarWrite(SHIM_MGMT_BAR, BPI_FLASH_OFFSET, &command, 4)) {
        return -1;
    }

    //  if (waitForReady(READY_STAT)) {
    //    return -1;
    //  }

    std::cout << "INFO: Sending unlock command\n";
    // Send unlock command
    command = UNLOCK_CMD;
    if (pcieBarWrite(SHIM_MGMT_BAR, BPI_FLASH_OFFSET, &command, 4)) {
        return -1;
    }
    if (waitForReady(READY_STAT)) {
        return -1;
    }

    // Send erase command
    std::cout << "INFO: Sending erase command\n";
    command = ERASE_CMD;
    if (pcieBarWrite(SHIM_MGMT_BAR, BPI_FLASH_OFFSET, &command, 4)) {
        return -1;
    }
    // now hanging here
    if (waitForReady(ERASE_STAT)) {
        return -1;
    }

    if (waitForReady(READY_STAT)) {
        return -1;
    }

    // Send program command
    std::cout << "INFO: Erasing the address range\n";
    command = PROGRAM_CMD;
    if (pcieBarWrite(SHIM_MGMT_BAR, BPI_FLASH_OFFSET, &command, 4)) {
        return -1;
    }

    if (waitForReady(PROGRAM_STAT)) {
        return -1;
    }

    return 0;
}

/*
 * program_microblaze
 */
int XOCLShim::program_microblaze(std::ifstream& mcsStream, const ELARecord& record) {
    int status = 0;
    if (mLogStream.is_open()) {
        mLogStream << __func__ << ", " << std::this_thread::get_id() << std::endl;
    }
#ifndef _WINDOWS
    // TODO: Windows build support
    //  timespec
    const timespec req = {0, 2000};
#endif

    std::cout << "Programming block (" << std::hex << record.mStartAddress << ", " << record.mEndAddress << std::dec << ")" << std::endl;
    assert(mcsStream.tellg() < record.mDataPos);
    mcsStream.seekg(record.mDataPos, std::ifstream::beg);
    unsigned char buffer[64];
    int bufferIndex = 0;
    for (unsigned index = record.mDataCount; index > 0;) {
        std::string line;
        std::getline(mcsStream, line);
        //std::cout << "current line:" <<line << std::endl;
        const unsigned dataLen = std::stoi(line.substr(1, 2), 0 , 16);
        index -= dataLen;
        const unsigned recordType = std::stoi(line.substr(7, 2), 0 , 16);
        if (recordType != 0x00) {
            continue;
        }
        const std::string data = line.substr(9, dataLen * 2);
        // Write in byte swapped order
        for (unsigned i = 0; i < data.length(); i += 2) {
            assert(bufferIndex <= 64);
            unsigned value = std::stoi(data.substr(i, 2), 0, 16);
            buffer[bufferIndex++] = (unsigned char)value;
            if (bufferIndex == 64) {
                break;
            }
        }

        assert((bufferIndex % 4) == 0);
        assert(bufferIndex <= 64);
        if (bufferIndex == 64) {
            if (waitForReady_microblaze(PROGRAM_STAT, false)) {
                return -1;
            }
            for (int i=0; i< 16; i++)
            {
                pcieBarRead(SHIM_MGMT_BAR, BPI_FLASH_OFFSET+0x10, &status, 4);
                while((status&2)==2){ //2: fifo is full
                    pcieBarRead(SHIM_MGMT_BAR, BPI_FLASH_OFFSET+0x10, &status, 4);
                }
                if (pcieBarWrite(SHIM_MGMT_BAR, BPI_FLASH_OFFSET, buffer+4*i, 4)) {
                    return -1;
                }
            }
            if (waitForReady_microblaze(PROGRAM_STAT, false)) {
                return -1;
            }
#ifndef _WINDOWS
            // TODO: Windows build support
            //   nanosleep is defined in unistd.h
            nanosleep(&req, 0);
#endif
            bufferIndex = 0;
        }
    }
    if (bufferIndex) {
        if (waitForReady_microblaze(PROGRAM_STAT, false)) {
            return -1;
        }
        
        for (int i=0; i<bufferIndex/4 ; i++)
        {
            pcieBarRead(SHIM_MGMT_BAR, BPI_FLASH_OFFSET+0x10, &status, 4);
            while((status&2)==2){ //2: fifo is full
                pcieBarRead(SHIM_MGMT_BAR, BPI_FLASH_OFFSET+0x10, &status, 4);
            }
            if (pcieBarWrite(SHIM_MGMT_BAR, BPI_FLASH_OFFSET, buffer+4*i, 4)) {
                return -1;
            }
        }

#ifndef _WINDOWS
        // TODO: Windows build support
        //   nanosleep is defined in unistd.h
        nanosleep(&req, 0);
#endif
    }
    return 0;
}

/*
 * program
 */
int XOCLShim::program(std::ifstream& mcsStream, const ELARecord& record) {
    if (mLogStream.is_open()) {
        mLogStream << __func__ << ", " << std::this_thread::get_id() << std::endl;
    }
#ifndef _WINDOWS
    // TODO: Windows build support
    //  timespec
    const timespec req = {0, 2000};
#endif

    std::cout << "Programming block (" << std::hex << record.mStartAddress << ", " << record.mEndAddress << std::dec << ")" << std::endl;
    assert(mcsStream.tellg() < record.mDataPos);
    mcsStream.seekg(record.mDataPos, std::ifstream::beg);
    unsigned char buffer[64];
    int bufferIndex = 0;
    for (unsigned index = record.mDataCount; index > 0;) {
        std::string line;
        std::getline(mcsStream, line);
        const unsigned dataLen = std::stoi(line.substr(1, 2), 0 , 16);
        index -= dataLen;
        const unsigned recordType = std::stoi(line.substr(7, 2), 0 , 16);
        if (recordType != 0x00) {
            continue;
        }
        const std::string data = line.substr(9, dataLen * 2);
        // Write in byte swapped order
        for (unsigned i = 0; i < data.length(); i += 2) {
            if ((bufferIndex % 4) == 0) {
                bufferIndex += 4;
            }
            assert(bufferIndex <= 64);
            unsigned value = std::stoi(data.substr(i, 2), 0, 16);
            buffer[--bufferIndex] = (unsigned char)value;
            if ((bufferIndex % 4) == 0) {
                bufferIndex += 4;
            }
            if (bufferIndex == 64) {
                break;
            }
        }

        assert((bufferIndex % 4) == 0);
        assert(bufferIndex <= 64);
        if (bufferIndex == 64) {
            if (waitForReady(PROGRAM_STAT, false)) {
                return -1;
            }
            if (pcieBarWrite(SHIM_MGMT_BAR, BPI_FLASH_OFFSET, buffer, 64)) {
                return -1;
            }
            if (waitForReady(PROGRAM_STAT, false)) {
                return -1;
            }
#ifndef _WINDOWS
            // TODO: Windows build support
            //   nanosleep is defined in unistd.h
            nanosleep(&req, 0);
#endif
            bufferIndex = 0;
        }
    }
    if (bufferIndex) {
        if (waitForReady(PROGRAM_STAT, false)) {
            return -1;
        }
        if (pcieBarWrite(SHIM_MGMT_BAR, BPI_FLASH_OFFSET, buffer, bufferIndex)) {
            return -1;
        }
        if (waitForReady(PROGRAM_STAT, false)) {
            return -1;
        }
#ifndef _WINDOWS
        // TODO: Windows build support
        //   nanosleep is defined in unistd.h
        nanosleep(&req, 0);
#endif
    }
    return 0;
}

/*
 * program
 */
int XOCLShim::program(std::ifstream& mcsStream) {
    int status = 0;
    int rxthresh = 256;
	int iRet1;
	int iRet2;
    bool use_mailbox = 0;
    for (ELARecordList::iterator i = mRecordList.begin(), e = mRecordList.end(); i != e; ++i) {
        i->mStartAddress <<= 16;
        i->mEndAddress += i->mStartAddress;
        // Convert from 2 bytes address to 4 bytes address
        i->mStartAddress /= 2;
        i->mEndAddress /= 2;
    }
    std::cout << "INFO: Start address 0x" << std::hex << mRecordList.front().mStartAddress << std::dec << "\n";
    std::cout << "INFO: End address 0x" << std::hex << mRecordList.back().mEndAddress << std::dec << "\n";

    // Check for existance of Mailbox IP
    iRet1 = pcieBarWrite(SHIM_MGMT_BAR, BPI_FLASH_OFFSET+0x1C, &rxthresh, 4);
    iRet2 = pcieBarRead(SHIM_MGMT_BAR, BPI_FLASH_OFFSET+0x1C, &status, 4);
    if((iRet1 == 0) && (iRet2 == 0)
		&& (status==rxthresh)) {
        use_mailbox = 1;
        std::cout << "INFO: Using Mailbox and Microblaze for flash programming\n";
    } else {
        std::cout << "INFO: Using custom flash programmer for flash programming\n";
    }

    if(use_mailbox) {
        if (prepare_microblaze(mRecordList.front().mStartAddress, mRecordList.back().mEndAddress)) {
            std::cout << "ERROR: Could not unlock or erase the blocks\n";
            return -1;
        }
    } else {
        if (prepare(mRecordList.front().mStartAddress, mRecordList.back().mEndAddress)) {
            std::cout << "ERROR: Could not unlock or erase the blocks\n";
            return -1;
        }
    }
#ifndef _WINDOWS
    // TODO: Windows build support
    //  timespec
    const timespec req = {0, 1000};
#endif
    int beatCount = 0;
    for (ELARecordList::iterator i = mRecordList.begin(), e = mRecordList.end(); i != e; ++i)
    {
        beatCount++;
        if(beatCount%10==0) {
            std::cout << "." << std::flush;
        }

        if(use_mailbox) {
            if (program_microblaze(mcsStream, *i)) {
                std::cout << "ERROR: Could not program the block\n";
                return -1;
            }
        } else {
            if (program(mcsStream, *i)) {
                std::cout << "ERROR: Could not program the block\n";
                return -1;
            }
        }
#ifndef _WINDOWS
        // TODO: Windows build support
        //   nanosleep is defined in unistd.h
        nanosleep(&req, 0);
#endif
    }
    std::cout << std::endl;
    // Now keep writing 0xff till the hardware says ready
    if(use_mailbox) {
        if (waitAndFinish_microblaze(READY_STAT, 0xff)) {
            return -1;
        }
    } else {
        if (waitAndFinish(READY_STAT, 0xff)) {
            return -1;
        }
    }
    return 0;
}

int XOCLShim::waitForReady_microblaze(unsigned code, bool verbose) {
    unsigned status = ~code;
    long long delay = 0;
#ifndef _WINDOWS
    // TODO: Windows build support
    //  timespec
    const timespec req = {0, 5000};
#endif
    if (verbose) {
        std::cout << "INFO: Waiting for hardware\n";
    }
    if(code==ERASE_STAT) {
        std::cout << "INFO: Waiting for erase...  Will take a couple minutes...\n";
    }
    while ((status != code) && (delay < 300000000000)) {
#ifndef _WINDOWS
        // TODO: Windows build support
        //    nanosleep is defined in unistd.h
        nanosleep(&req, 0);
#endif

        pcieBarRead(SHIM_MGMT_BAR, BPI_FLASH_OFFSET+0x10, &status, 4);
        if(!(status&0x1)){ //0: fifo is not empty

            if(pcieBarRead(SHIM_MGMT_BAR, BPI_FLASH_OFFSET+0x8, &status, 4)) {
                return -1;
            }
        }
        delay += 5000;
        if(code==ERASE_STAT) {
            if((delay % 5000000) == 0) {
                std::cout << ".";
            }
        }
    }
    return (status == code) ? 0 : -1;
}

/*
 * waitForReady
 */
int XOCLShim::waitForReady(unsigned code, bool verbose) {
    unsigned status = ~code;
    long long delay = 0;
#ifndef _WINDOWS
    // TODO: Windows build support
    //  timespec
    const timespec req = {0, 5000};
#endif
    if (verbose) {
        std::cout << "INFO: Waiting for hardware\n";
    }
    while ((status != code) && (delay < 30000000000)) {
#ifndef _WINDOWS
        // TODO: Windows build support
        //    nanosleep is defined in unistd.h
        nanosleep(&req, 0);
#endif
        if (pcieBarRead(SHIM_MGMT_BAR, BPI_FLASH_OFFSET, &status, 4)) {
            return -1;
        }
        delay += 5000;
    }
    return (status == code) ? 0 : -1;
}

/*
 * waitAndFinish_microblaze
 */
int XOCLShim::waitAndFinish_microblaze(unsigned code, unsigned data, bool verbose) {
    unsigned status = ~code;
    long long delay = 0;
#ifndef _WINDOWS
    // TODO: Windows build support
    //  timespec
    const timespec req = {0, 5000};
#endif
    if (verbose) {
        std::cout << "INFO: Finishing up\n";
    }
    pcieBarRead(SHIM_MGMT_BAR, BPI_FLASH_OFFSET+0x10, &status, 4);
    if(!(status&0x1)){ //0: fifo is not empty

        if(pcieBarRead(SHIM_MGMT_BAR, BPI_FLASH_OFFSET+0x8, &status, 4)) {
            std::cout << "INFO: Failed to read from Mailbox\n";
            return -1;
        }
    }

    while ((status != code) && (delay < 300000000000)) {
#ifndef _WINDOWS
        // TODO: Windows build support
        //    nanosleep is defined in unistd.h
        nanosleep(&req, 0);
#endif
        pcieBarRead(SHIM_MGMT_BAR, BPI_FLASH_OFFSET+0x10, &status, 4);
        if((status&2)==2) {
            return 0;
        }
        if (pcieBarWrite(SHIM_MGMT_BAR, BPI_FLASH_OFFSET, &data, 4)) {
            std::cout << "INFO: Failed to write to Mailbox\n";
            return -1;
        }

        if(!(status&0x1)){ //0: fifo is not empty

            if(pcieBarRead(SHIM_MGMT_BAR, BPI_FLASH_OFFSET+0x8, &status, 4)) {
                std::cout << "INFO: Failed to read from Mailbox\n";
                return -1;
            }
        }

        delay += 5000;
    }
    return (status == code) ? 0 : -1;
}

/*
 * waitAndFinish
 */
int XOCLShim::waitAndFinish(unsigned code, unsigned data, bool verbose) {
    unsigned status = ~code;
    long long delay = 0;
#ifndef _WINDOWS
    // TODO: Windows build support
    //  timespec
    const timespec req = {0, 5000};
#endif
    if (verbose) {
        std::cout << "INFO: Finishing up\n";
    }
    if (pcieBarRead(SHIM_MGMT_BAR, BPI_FLASH_OFFSET, &status, 4)) {
        return -1;
    }
    while ((status != code) && (delay < 30000000000)) {
#ifndef _WINDOWS
        // TODO: Windows build support
        //    nanosleep is defined in unistd.h
        nanosleep(&req, 0);
#endif
        if (pcieBarWrite(SHIM_MGMT_BAR, BPI_FLASH_OFFSET, &data, 4)) {
            return -1;
        }
        if (pcieBarRead(SHIM_MGMT_BAR, BPI_FLASH_OFFSET, &status, 4)) {
            return -1;
        }
        delay += 5000;
    }
    return (status == code) ? 0 : -1;
}

/*
 * xclBootFPGA
 */
int XOCLShim::xclBootFPGA() {
    return ioctl(mMgtHandle, XCLMGMT_IOCREBOOT);
}

} // end namespac xocl

// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
