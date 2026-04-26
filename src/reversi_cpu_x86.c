#include <intrin.h>

int __cdecl ReversiDetectModernCpu(int *has_avx)
{
    int regs[4];
    *has_avx = 0;

    __cpuid(regs, 0);
    if (regs[0] < 1) {
        return 0;
    }

    __cpuid(regs, 1);
    if (!(regs[3] & (1 << 26))) {
        return 0;
    }

    if ((regs[2] & (1 << 27)) && (regs[2] & (1 << 28))) {
        unsigned __int64 xcr0 = _xgetbv(0);
        *has_avx = (xcr0 & 0x6) == 0x6;
    }

    return 1;
}
