#include <stdio.h>

int main() {
#if defined(__i386) || defined(__x86_64)
    printf("sse3:   \t%d\n", __builtin_cpu_supports ("sse3"));    // __v4sf
    printf("sse4.1: \t%d\n", __builtin_cpu_supports ("sse4.1"));  // __v4sf
    printf("avx:    \t%d\n", __builtin_cpu_supports ("avx"));     // __v8sf
    printf("avx2:   \t%d\n", __builtin_cpu_supports ("avx2"));    // __v8sf
    printf("avx512f:\t%d\n", __builtin_cpu_supports ("avx512f")); // __v16sf
#endif
    // if defined (__aarch64__)  ... NEON ok, 32 registers ("asimd" in /proc/cpuinfo)      --> float32x4_t
    // if defined (__arm__)  ... /proc/cpuinfo "Features" line "neon" word .. 16 registers --> float32x4_t
}
