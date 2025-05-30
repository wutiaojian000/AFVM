#include "sys.h"

int check_SVM_support()
{
    int CPUID_ret;

    __asm("movl $0x80000001, %%eax;cpuid;movl %%ecx, %0":"=r"(CPUID_ret)::"eax","ebx","ecx","edx");

    if((CPUID_ret & CPUID_Fn8000_0001_ECX_SVM) == 0)
    {
        return ERROR_NOT_SUPPORT_SVM;
    }
    
    return 0;
}