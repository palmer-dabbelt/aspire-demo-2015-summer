/* Copyright 2015 Palmer Dabbelt */

#include <stdio.h>
#include <stdlib.h>

#include "util.h++"

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

int main(...) {
    char buffer[BUFFER_SIZE];
    while (true) {
        freadall (buffer, BUFFER_SIZE, stdin );
        fwriteall(buffer, BUFFER_SIZE, stdout);
    }
}
