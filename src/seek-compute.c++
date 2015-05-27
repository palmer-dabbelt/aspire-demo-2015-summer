/* Copyright 2015 Palmer Dabbelt <palmer@dabbelt.com> */

#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <assert.h>

#include "colorspace.h++"

static pixel_rgb32 rainbow(uint8_t input);

int main(int argc __attribute__((unused)), 
         const char **argv __attribute__((unused)))
{
    auto ffplay = popen("ffplay -i - -f rawvideo -video_size 208x156 -pixel_format bgra -loglevel quiet", "w");
    setbuf(ffplay, NULL);

    setbuf(stdin, NULL);

    auto ir = std::vector<uint8_t>(208 * 156);
    auto color = std::vector<pixel_rgb32>(208 * 156);

    while (true) {
        fread(ir.data(),
              sizeof(*(ir.data())) * ir.size(),
              1,
              stdin);

        assert(ir.size() == color.size());
        for (size_t i = 0; i < ir.size(); ++i) {
            color[i] = rainbow(ir[i]);
        }

        fwrite(color.data(),
               sizeof(*(color.data())) * color.size(),
               1,
               ffplay);
    }
    pclose(ffplay);

    return 0;
}

pixel_rgb32 rainbow(uint8_t input)
{
    pixel_rgb32 lo(0, 0, 255);
    pixel_rgb32 hi(255, 0, 0);

    double lambda = input / 255.0;

    return ((lambda) * hi) + ((1 - lambda) * lo);
}
