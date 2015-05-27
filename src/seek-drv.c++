/* Copyright 2015 Palmer Dabbelt <palmer@dabbelt.com> */

#include <stdio.h>
#include <seek.hpp>
#include <stdint.h>
#include <vector>

int main(int argc __attribute__((unused)), 
         const char **argv __attribute__((unused)))
{
    setbuf(stdout, NULL);

    LibSeek::Imager interface;
    interface.init();

    LibSeek::Frame frame;
    interface.frame_init(frame);
    if ((frame.width() != 208) || (frame.height() != 156))
        abort();
        
    while (true) {
        interface.frame_acquire(frame);

        fwrite(frame.data(),
               sizeof(*(frame.data())) * frame.width() * frame.height(),
               1,
               stdout);
    }

    return 0;
}
