#ifndef UTIL_HXX
#define UTIL_HXX

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

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

template<class T>
T *aligned_alloc(size_t alignment, size_t count __attribute__((unused)), T *out)
{
    while ((((uintptr_t)out) % alignment) != 0)
        out++;
    return (T*)out;
}

template<class T>
T *aligned_alloc(size_t alignment, size_t count)
{
    char *out = new char[sizeof(T) * count * 2];
    return aligned_alloc(alignment, count, out);
}

#endif
