/* Copyright 2015 Palmer Dabbelt <palmer@dabbelt.com> */

#include <stdio.h>
#include <seek.hpp>
#include <stdint.h>
#include <vector>
#include <assert.h>
#include <random>

#include "generate-calibration.h++"

#define PROB_NUM 1
#define PROB_DENOM 75

#define CHANGE_NUM 1
#define CHANGE_DENOM 100
#define CHANGE_SIZE 5

#define MOVE_RADIUS 1

#define SMOOTH_RADIUS 3

int main(int argc __attribute__((unused)), 
         const char **argv __attribute__((unused)))
{
    auto cold = generate_calibration::cold;
    auto hot  = generate_calibration::hot;

    setbuf(stdout, NULL);

    auto flat = std::vector<uint8_t>(208 * 156);
    auto raw = std::vector<uint16_t>(208 * 156);

    std::default_random_engine generator(0);
    std::uniform_int_distribution<int> distribution(0, PROB_DENOM);
    std::uniform_int_distribution<int> change_distrib(-CHANGE_SIZE, CHANGE_SIZE);
    std::uniform_int_distribution<int> change_prob(0, CHANGE_DENOM);

    /* Begin with a square in the middle of the image. */
    for (size_t i = 0; i < flat.size(); ++i) {
        auto x = i % 208;
        auto y = i / 208;

        if ((x >= 70) && (x <= 90) && (y >= 80) && (y <= 150))
            flat[i] = 200;
        else
            flat[i] = 100;
    }

    while (true) {
        /* Randomly perturb the edge of the image to produce the image
         * on the next frame. */
        for (size_t i = 0; i < flat.size(); ++i) {
            auto x = i % 208;
            auto y = i / 208;

            if (change_prob(generator) <= CHANGE_NUM)
                flat[i] += change_distrib(generator);

            if ((x == 0) || (x == 207) || (y == 0) || (y == 155))
                continue;

            /* Compute a gradient to check and see if we're in an
             * interesting transition region. */
            auto grad = abs(flat[i-1] - flat[i+1]) + abs(flat[i-208] - flat[i+208]);
            if (grad < 100)
                continue;

            if (distribution(generator) > PROB_NUM)
                continue;

            auto copy = flat[i];
            if (abs(flat[i-1] - flat[i+1]) > abs(flat[i-208] - flat[i+208]))
                if (change_distrib(generator) > 0)
                    copy = flat[i+1];
                else
                    copy = flat[i-1];
            else
                if (change_distrib(generator) > 0)
                    copy = flat[i+208];
                else
                    copy = flat[i-208];

            for (auto dy = -MOVE_RADIUS; dy <= MOVE_RADIUS; ++dy)
                for (auto dx = -MOVE_RADIUS; dx <= MOVE_RADIUS; ++dx)
                    flat[(y+dy)*208 + (x+dx)] = copy;
                
        }

        /* Apply a smoothing filter to try and make the edges not
         * super random looking. */
        for (size_t i = 0; i < flat.size(); ++i) {
            auto x = i % 208;
            auto y = i / 208;

            auto hi_count = 0;
            auto hi_sum = 0;
            auto lo_count = 0;
            auto lo_sum = 0;
            for (auto dy = -SMOOTH_RADIUS; dy <= SMOOTH_RADIUS; ++dy) {
                for (auto dx = -SMOOTH_RADIUS; dx <= SMOOTH_RADIUS; ++dx) {
                    if (flat[(y+dy)*208 + (x+dx)] > 127) {
                        hi_count++;
                        hi_sum += flat[(y+dy)*208 + (x+dx)];
                    } else {
                        lo_count++;
                        lo_sum += flat[(y+dy)*208 + (x+dx)];
                    }
                }
            }

            if (hi_count > 3*lo_count) {
                for (auto dy = -SMOOTH_RADIUS; dy <= SMOOTH_RADIUS; ++dy) {
                    for (auto dx = -SMOOTH_RADIUS; dx <= SMOOTH_RADIUS; ++dx) {
                        if (flat[(y+dy)*208 + (x+dx)] > 127) {
                        } else {
                            flat[i] = hi_sum / hi_count;
                        }
                    }
                }
            }

            if (lo_count > 3*hi_count) {
                for (auto dy = -SMOOTH_RADIUS; dy <= SMOOTH_RADIUS; ++dy) {
                    for (auto dx = -SMOOTH_RADIUS; dx <= SMOOTH_RADIUS; ++dx) {
                        if (flat[(y+dy)*208 + (x+dx)] > 127) {
                            flat[i] = lo_sum / lo_count;
                        } else {
                        }
                    }
                }
            }
        }

        assert(raw.size() == flat.size());
        
        for (size_t i = 0; i < raw.size(); ++i) {
            auto max = hot[i];
            auto min = cold[i];

            auto scaled = ((double)flat[i]) / ((1 << 8) - 1);
            raw[i] = (scaled * (max - min)) + min;
        }

        fwrite(raw.data(),
               sizeof(*(raw.data())) * raw.size(),
               1,
               stdout);
    }

    return 0;
}
