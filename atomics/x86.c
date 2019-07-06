#include <sys/types.h>

static inline int8_t cmpxchg16(int8_t* ptr, int8_t oldval, int8_t newval) {
    int8_t out;
    __asm__ __volatile__ ("lock; cmpxchg %2, %1;"
        : "=a" (out), "+m" (*(volatile int8_t*) ptr)
        : "r" (newval), "0" (oldval)
        : "memory");
    return out;
}

static inline int16_t cmpxchg16(int16_t* ptr, int16_t oldval, int16_t newval) {
    int16_t out;
    __asm__ __volatile__ ("lock; cmpxchg %2, %1;"
        : "=a" (out), "+m" (*(volatile int16_t*) ptr)
        : "r" (newval), "0" (oldval)
        : "memory");
    return out;
}

static inline int32_t cmpxchg32(int32_t* ptr, int32_t oldval, int32_t newval) {
    int32_t out;
    __asm__ __volatile__ ("lock; cmpxchg %2, %1;"
        : "=a" (out), "+m" (*(volatile int32_t*) ptr)
        : "r" (newval), "0" (oldval)
        : "memory");
    return out;
}
