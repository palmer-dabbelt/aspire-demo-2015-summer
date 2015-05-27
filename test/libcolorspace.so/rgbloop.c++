#include "harness.h++"

int main(...)
{
    for (size_t r = 0; r <= 255; ++r) {
        for (size_t g = 0; g <= 255; ++g) {
            for (size_t b = 0; b <= 255; ++b) {
                test(r, g, b);
            }
        }
    }
}

