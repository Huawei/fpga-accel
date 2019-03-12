#
#   BSD LICENSE
#
#   Copyright(c)  2019 Huawei Technologies Co., Ltd. All rights reserved.
#   All rights reserved.
#
#   Redistribution and use in source and binary forms, with or without
#   modification, are permitted provided that the following conditions
#   are met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in
#       the documentation and/or other materials provided with the
#       distribution.
#     * Neither the name of Huawei Technologies Co., Ltd  nor the names of its
#       contributors may be used to endorse or promote products derived
#       from this software without specific prior written permission.
#
#   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
#   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
#   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
#   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
#   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
#   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
#   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
#   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
 
.PHONY: all

GCC=gcc -pthread -lrt

CFLAG += -Wall
ifeq ($(DEBUG), 1)
    CFLAG += -g
endif

AR_LIB=libfpgadrv.a
DYN_LIB=libfpgadrv.so

AR_PATH := /usr/lib*
DYN_PATH := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

INCLUDE_PATH += ../include

DEMOS_SRC=$(wildcard *.c)
DEMOS_OBJ=$(patsubst %.c, %.o, $(DEMOS_SRC))
DEMOS=$(patsubst %.o, %, $(DEMOS_OBJ))
DEMOS_DYN=$(patsubst %.o, %_dyn, $(DEMOS_OBJ))

all: clean $(DEMOS_OBJ) $(DEMOS) $(DEMOS_DYN) delete_object

$(DEMOS_OBJ):%.o:%.c
	@mkdir -p ../dist
	$(GCC) $(CFLAG) -c $< -I$(INCLUDE_PATH) -o $@ 
	@echo "finish compiling $(DEMOS)"

$(DEMOS):%:%.o
	$(GCC) $< $(AR_PATH)/$(AR_LIB) -o ../dist/$@ 
	@echo "finish linking $(DEMOS)"

$(DEMOS_DYN):%_dyn:%.o
	$(GCC) $< -L $(DYN_PATH) -lfpgadrv -o ../dist/$@
	@echo "finish linking $(DEMOS_DYN)"

delete_object:
	rm -rf *.o

clean:
	rm -rf *.o
	rm -f $(DEMOS)
	rm -f $(DEMOS_DYN)
