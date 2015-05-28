/* Copyright 2015 Palmer Dabbelt <palmer@dabbelt.com> */

#include <stdio.h>
#include <stdint.h>

#ifdef HAVE_SEEK
#include <seek.hpp>
#endif

#ifndef WARMUP_COUNT
#define WARMUP_COUNT 200
#endif

int main(int argc __attribute__((unused)), 
         const char **argv __attribute__((unused)))
{
#ifdef HAVE_SEEK
    auto ffplay = popen("ffplay -i - -f rawvideo -video_size 208x156 -pixel_format gray16le", "w");
    setbuf(ffplay, NULL);

    LibSeek::Imager interface;
    interface.init();

    LibSeek::Frame frame;
    interface.frame_init(frame);

    for (auto i = 0*WARMUP_COUNT; i < WARMUP_COUNT; ++i) {
        interface.frame_acquire(frame);
        fwrite((uint8_t*)frame.data(),
               sizeof(uint16_t) * frame.width() * frame.height(),
               1,
               ffplay);
        fprintf(stderr, "%d\n", i);
        
    }
    pclose(ffplay);

    auto out = fopen("seek-calibrate.raw", "w");
    fwrite((uint8_t*)frame.data(),
           sizeof(uint16_t) * frame.width() * frame.height(),
           1,
           out);
    fclose(out);
#endif

    return 0;
}
