#include <sys/types.h>

// Note: RCX and R11 are clobbered by Intel, see SYSCALL mnemonic manual.

#define CARRY_FLAG_BIT 1
#define RETURN_SYSCALL_RESULT(result, flags) return (flags & CARRY_FLAG_BIT) ? -result : result;

inline int64_t syscall6(int64_t num, int64_t arg1, int64_t arg2, int64_t arg3, int64_t arg4, int64_t arg5, int64_t arg6) {
    int64_t result;
    int64_t flags;

    __asm__ __volatile__ (
        "movq %6, %%r10;\n"
        "movq %7, %%r8;\n"
        "movq %8, %%r9;\n"
        "syscall;\n"
        "movq %%r11, %1;\n"
        : "=a" (result), "=r" (flags)
        : "a" (num), "D" (arg1), "S" (arg2), "d" (arg3), "r" (arg4), "r" (arg5), "r" (arg6)
        : "%r10", "%r8", "%r9", "%rcx", "%r11"
    );

    RETURN_SYSCALL_RESULT(result, flags);
}

inline int64_t syscall5(int64_t num, int64_t arg1, int64_t arg2, int64_t arg3, int64_t arg4, int64_t arg5) {
    int64_t result;
    int64_t flags;

    __asm__ __volatile__ (
        "movq %6, %%r10;\n"
        "movq %7, %%r8;\n"
        "syscall;\n"
        "movq %%r11, %1;\n"
        : "=a" (result), "=r" (flags)
        : "a" (num), "D" (arg1), "S" (arg2), "d" (arg3), "r" (arg4), "r" (arg5)
        : "%r10", "%r8", "%rcx", "%r11"
    );

    RETURN_SYSCALL_RESULT(result, flags);
}

inline int64_t syscall4(int64_t num, int64_t arg1, int64_t arg2, int64_t arg3, int64_t arg4) {
    int64_t result;
    int64_t flags;

    __asm__ __volatile__ (
        "movq %6, %%r10;\n"
        "syscall;\n"
        "movq %%r11, %1;\n"
        : "=a" (result), "=r" (flags)
        : "a" (num), "D" (arg1), "S" (arg2), "d" (arg3), "r" (arg4)
        : "%r10", "%rcx", "%r11"
    );

    RETURN_SYSCALL_RESULT(result, flags);
}

inline int64_t syscall3(int64_t num, int64_t arg1, int64_t arg2, int64_t arg3) {
    int64_t result;
    int64_t flags;

    __asm__ __volatile__ (
        "syscall;\n"
        "movq %%r11, %1;\n"
        : "=a" (result), "=r" (flags)
        : "a" (num), "D" (arg1), "S" (arg2), "d" (arg3)
        : "%rcx", "%r11"
    );

    RETURN_SYSCALL_RESULT(result, flags);
}

inline int64_t syscall2(int64_t num, int64_t arg1, int64_t arg2) {
    int64_t result;
    int64_t flags;

    __asm__ __volatile__ (
        "syscall;\n"
        "movq %%r11, %1;\n"
        : "=a" (result), "=r" (flags)
        : "a" (num), "D" (arg1), "S" (arg2)
        : "%rcx", "%r11"
    );

    RETURN_SYSCALL_RESULT(result, flags);
}

inline int64_t syscall1(int64_t num, int64_t arg1) {
    int64_t result;
    int64_t flags;

    __asm__ __volatile__ (
        "syscall;\n"
        "movq %%r11, %1;\n"
        : "=a" (result), "=r" (flags)
        : "a" (num), "D" (arg1)
        : "%rcx", "%r11"
    );

    RETURN_SYSCALL_RESULT(result, flags);
}

inline int64_t syscall0(int64_t num) {
    int64_t result;
    int64_t flags;

    __asm__ __volatile__ (
        "syscall;\n"
        "movq %%r11, %1;\n"
        : "=a" (result), "=r" (flags)
        : "a" (num)
        : "%rcx", "%r11"
    );

    RETURN_SYSCALL_RESULT(result, flags);
}