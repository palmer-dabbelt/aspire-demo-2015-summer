CC=gcc

ifeq "$(PLATFORM)" ""
PLATFORM := $(shell uname)
endif

ifeq "$(PLATFORM)" "Darwin"
OPENCL_FLAG = -framework OpenCL
else
OPENCL_FLAG = -lOpenCL
endif

main: main.c
		gcc -o main main.c ${OPENCL_FLAG}
