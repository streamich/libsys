inline long syscall6(long num, long arg1, long arg2, long arg3, long arg4, long arg5, long arg6) {
    long result;

    __asm__ __volatile__ (
        "movq %5, %%r10;\n"
        "movq %6, %%r8;\n"
        "movq %7, %%r9;\n"
        "syscall;\n"
        : "=a" (result)
        : "a" (num), "D" (arg1), "S" (arg2), "d" (arg3), "r" (arg4), "r" (arg5), "r" (arg6)
        : "%r10", "%r8", "%r9"
    );

    return result;
}

inline long syscall5(long num, long arg1, long arg2, long arg3, long arg4, long arg5) {
    long result;

    __asm__ __volatile__ (
        "movq %5, %%r10;\n"
        "movq %6, %%r8;\n"
        "syscall;\n"
        : "=a" (result)
        : "a" (num), "D" (arg1), "S" (arg2), "d" (arg3), "r" (arg4), "r" (arg5)
        : "%r10", "%r8"
    );

    return result;
}

inline long syscall4(long num, long arg1, long arg2, long arg3, long arg4) {
    long result;

    __asm__ __volatile__ (
        "movq %5, %%r10;\n"
        "syscall;\n"
        : "=a" (result)
        : "a" (num), "D" (arg1), "S" (arg2), "d" (arg3), "r" (arg4)
        : "%r10"
    );

    return result;
}

inline long syscall3(long num, long arg1, long arg2, long arg3) {
    long result;

    __asm__ __volatile__ (
        "syscall"
        : "=a" (result)
        : "a" (num), "D" (arg1), "S" (arg2), "d" (arg3)
        :
    );

    return result;
}

inline long syscall2(long num, long arg1, long arg2) {
    long result;

    __asm__ __volatile__ (
        "syscall"
        : "=a" (result)
        : "a" (num), "D" (arg1), "S" (arg2)
        :
    );

    return result;
}

inline long syscall1(long num, long arg1) {
    long result;

    __asm__ __volatile__ (
        "syscall"
        : "=a" (result)
        : "a" (num), "D" (arg1)
        :
    );

    return result;
}

inline long syscall0(long num) {
    long result;

    __asm__ __volatile__ (
        "syscall"
        : "=a" (result)
        : "a" (num)
        :
    );

    return result;
}
