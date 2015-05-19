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
		$(CC) -std=c99 -o main main.c ${OPENCL_FLAG} -lm

clean:
		rm -f main
