#ifndef UTIL_HXX
#define UTIL_HXX

#include <stdlib.h>
#include <stdio.h>

static inline void fwriteall(void *buf,
                             size_t count,
                             FILE *file)
{
    size_t written = 0;
    while (written < count) {
        auto out = fwrite(((char *)buf) + written, 1, count - written, file);
        if (out <= 0)
            abort();
        written += out;
    }
}

static inline void freadall(void *buf,
                            size_t count,
                            FILE *file)
{
    size_t readed = 0;
    while (readed < count) {
        auto out = fread(((char *)buf) + readed, 1, count - readed, file);
        if (out <= 0)
            abort();
        readed += out;
    }
}

#endif
