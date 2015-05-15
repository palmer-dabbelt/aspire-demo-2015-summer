C_INCLUDE_PATH=/usr/local/cuda/include

CC=gcc

ifeq "$(PLATFORM)" ""
PLATFORM := $(shell uname)
endif

ifeq "$(PLATFORM)" "Darwin"
OPENCL_FLAG = -framework OpenCL
else
OPENCL_FLAG = -lOpenCL -I${C_INCLUDE_PATH}
endif

main: main.c
		gcc -std=c11 -o main main.c ${OPENCL_FLAG}
