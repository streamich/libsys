#include <sys/types.h>

static inline int32_t cmpxchg32(int32_t* ptr, int32_t oldval, int32_t newval) {
  int out;
  __asm__ __volatile__ ("lock; cmpxchg %2, %1;"
        : "=a" (out), "+m" (*(volatile int32_t*) ptr)
        : "r" (newval), "0" (oldval)
        : "memory");
  return out;
}
