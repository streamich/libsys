#include <sys/types.h>

inline int64_t syscall6(int64_t num, int64_t arg1, int64_t arg2, int64_t arg3, int64_t arg4, int64_t arg5, int64_t arg6);
inline int64_t syscall5(int64_t num, int64_t arg1, int64_t arg2, int64_t arg3, int64_t arg4, int64_t arg5);
inline int64_t syscall4(int64_t num, int64_t arg1, int64_t arg2, int64_t arg3, int64_t arg4);
inline int64_t syscall3(int64_t num, int64_t arg1, int64_t arg2, int64_t arg3);
inline int64_t syscall2(int64_t num, int64_t arg1, int64_t arg2);
inline int64_t syscall1(int64_t num, int64_t arg1);
inline int64_t syscall0(int64_t num);
