/* Copyright 2015 Palmer Dabbelt <palmer@dabbelt.com> */

#include <stdio.h>
#include <seek.hpp>
#include <stdint.h>
#include <vector>

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

    LibSeek::Imager interface;
    interface.init();

    LibSeek::Frame frame;
    interface.frame_init(frame);
    if ((frame.width() != 208) || (frame.height() != 156))
        abort();
        
    auto image = std::vector<uint8_t>(208 * 156);

    while (true) {
        interface.frame_acquire(frame);

        for (auto i = 0*image.size(); i < image.size(); ++i) {
            auto max = hot[i];
            auto min = cold[i];
            
            auto scaled = (float)(frame.data()[i] - min) / (max - min);
            if (scaled > 1.0) scaled = 1.0;
            if (scaled < 0.0) scaled = 0.0;
            image[i] = ((1 << 8) - 1) * scaled;
        }

        fwrite(image.data(),
               sizeof(*(image.data())) * frame.width() * frame.height(),
               1,
               ffplay);

        fwrite(image.data(),
               sizeof(*(image.data())) * frame.width() * frame.height(),
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
