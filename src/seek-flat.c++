/* Copyright 2015 Palmer Dabbelt <palmer@dabbelt.com> */

#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <assert.h>

#include "generate-calibration.h++"
#include "util.h++"

#ifndef CALIBRATE_MAX
#define CALIBRATE_MAX 0x8200
#define CALIBRATE_MIN 0x7e00
#endif

#define SIZE (208 * 156)

#include "hwacha-flat.h++"

int main(int argc __attribute__((unused)), 
         const char **argv __attribute__((unused)))
{
    auto cold = generate_calibration::cold;
    auto hot  = generate_calibration::hot;

#ifdef HAVE_FFPLAY
    auto ffplay = popen("ffplay -i - -f rawvideo -video_size 208x156 -pixel_format gray -loglevel quiet", "w");
    setbuf(ffplay, NULL);
#endif

    setbuf(stdout, NULL);
    setbuf(stdin, NULL);

    uint16_t raw[SIZE];
    uint8_t flat[SIZE];

    while (true) {
        freadall(&raw[0],
                 SIZE * sizeof(*raw),
                 stdin);

        flatten(flat, raw, hot, cold);

#ifdef HAVE_FFPLAY
        fwriteall(&flat[0],
                  SIZE * sizeof(*flat),
                  ffplay);
#endif

        fwriteall(&flat[0],
                  SIZE * sizeof(*flat),
                  stdout);
    }

#ifdef HAVE_FFPLAY
    pclose(ffplay);
#endif

    return 0;
}
