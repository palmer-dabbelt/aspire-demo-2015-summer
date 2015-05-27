/* Copyright 2015 Palmer Dabbelt <palmer@dabbelt.com> */

#include <stdio.h>
#include <seek.hpp>
#include <stdint.h>
#include <vector>
#include <assert.h>

#ifndef CALIBRATE_MAX
#define CALIBRATE_MAX 0x8200
#define CALIBRATE_MIN 0x7e00
#endif

static std::vector<uint16_t> read_calibration_file(const std::string& filename);

int main(int argc __attribute__((unused)), 
         const char **argv __attribute__((unused)))
{
    auto cold = read_calibration_file("share/seek/cold.raw");
    auto hot  = read_calibration_file("share/seek/hot.raw");

    auto ffplay = popen("ffplay -i - -f rawvideo -video_size 208x156 -pixel_format gray", "w");
    setbuf(ffplay, NULL);

    setbuf(stdout, NULL);
    setbuf(stdin, NULL);

    auto raw = std::vector<uint16_t>(208 * 156);
    auto flat = std::vector<uint8_t>(208 * 156);

    while (true) {
        fread(raw.data(),
              sizeof(*(raw.data())) * raw.size(),
              1,
              stdin);

        assert(raw.size() == flat.size());
        for (size_t i = 0; i < raw.size(); ++i) {
            auto max = hot[i];
            auto min = cold[i];
            
            auto scaled = (float)(raw.data()[i] - min) / (max - min);
            if (scaled > 1.0) scaled = 1.0;
            if (scaled < 0.0) scaled = 0.0;
            flat[i] = ((1 << 8) - 1) * scaled;
        }

        fwrite(flat.data(),
               sizeof(*(flat.data())) * flat.size(),
               1,
               ffplay);

        fwrite(flat.data(),
               sizeof(*(flat.data())) * flat.size(),
               1,
               stdout);
    }
    pclose(ffplay);

    return 0;
}

std::vector<uint16_t> read_calibration_file(const std::string& filename)
{
    auto file = fopen(filename.c_str(), "rb");
    auto out = std::vector<uint16_t>(208 * 156);
    fread(out.data(),
          sizeof(*(out.data())) * 208 * 156,
          1,
          file);
    fclose(file);
    return out;
}
