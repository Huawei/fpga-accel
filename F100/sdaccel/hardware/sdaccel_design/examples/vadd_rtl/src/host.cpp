/**********
Copyright (c) 2018, Xilinx, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**********/
#include "xcl2.hpp"
#include <vector>

#define DATA_SIZE 256

int main(int argc, char** argv)
{
    int size = DATA_SIZE;
    //Allocate Memory in Host Memory
    size_t vector_size_bytes = sizeof(int) * size;
    std::vector<int,aligned_allocator<int>> source_input1    (size);
    std::vector<int,aligned_allocator<int>> source_input2    (size);
    std::vector<int,aligned_allocator<int>> source_hw_results(size);
    std::vector<int,aligned_allocator<int>> source_sw_results(size);

    // Create the test data and Software Result 
    for(int i = 0 ; i < size ; i++){
        source_input1[i] = i;
        source_input2[i] = i;
        source_sw_results[i] = source_input1[i] + source_input2[i];
        source_hw_results[i] = 0;
    }

//OPENCL HOST CODE AREA START
    //Create Program and Kernel
    std::vector<cl::Device> devices = xcl::get_xil_devices();
    cl::Device device = devices[0];

    cl::Context context(device);
    cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE);
    std::string device_name = device.getInfo<CL_DEVICE_NAME>(); 

    std::string binaryFile = xcl::find_binary_file(device_name,"vadd");
    cl::Program::Binaries bins = xcl::import_binary_file(binaryFile);
    devices.resize(1);
    cl::Program program(context, devices, bins);
    cl::Kernel krnl_vadd(program,"krnl_vadd_rtl");

    //Allocate Buffer in Global Memory
    std::vector<cl::Memory> inBufVec, outBufVec;
    cl::Buffer buffer_r1(context,CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, 
            vector_size_bytes, source_input1.data());
    cl::Buffer buffer_r2(context,CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, 
            vector_size_bytes, source_input2.data());
    cl::Buffer buffer_w (context,CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, 
            vector_size_bytes, source_hw_results.data());
    inBufVec.push_back(buffer_r1);
    inBufVec.push_back(buffer_r2);
    outBufVec.push_back(buffer_w);

    //Copy input data to device global memory
    q.enqueueMigrateMemObjects(inBufVec,0/* 0 means from host*/);

    //Set the Kernel Arguments
    krnl_vadd.setArg(0,buffer_r1);
    krnl_vadd.setArg(1,buffer_r2);
    krnl_vadd.setArg(2,buffer_w);
    krnl_vadd.setArg(3,size);

    //Launch the Kernel
    q.enqueueTask(krnl_vadd);

    //Copy Result from Device Global Memory to Host Local Memory
    q.enqueueMigrateMemObjects(outBufVec,CL_MIGRATE_MEM_OBJECT_HOST);
    q.finish();

//OPENCL HOST CODE AREA END
    
    // Compare the results of the Device to the simulation
    int match = 0;
    for (int i = 0 ; i < size ; i++){
        if (source_hw_results[i] != source_sw_results[i]){
            std::cout << "Error: Result mismatch" << std::endl;
            std::cout << "i = " << i << " Software result = " << source_sw_results[i]
                << " Device result = " << source_hw_results[i] << std::endl;
            match = 1;
            break;
        }
    }

    std::cout << "TEST " << (match ? "FAILED" : "PASSED") << std::endl; 
    return (match ? EXIT_FAILURE :  EXIT_SUCCESS);
}
