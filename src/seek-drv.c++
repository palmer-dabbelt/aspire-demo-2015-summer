/* Copyright 2015 Palmer Dabbelt <palmer@dabbelt.com> */

#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <stdlib.h>

#ifdef HAVE_SEEK
#include <seek.hpp>
#endif

int main(int argc __attribute__((unused)), 
         const char **argv __attribute__((unused)))
{
#ifdef HAVE_SEEK
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
#endif

    return 0;
}
