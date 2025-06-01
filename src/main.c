#include "sys_cmd.h"
#include "sys.h"
#include "print.h"

//#define DEBUG

#ifdef DEBUG
#include "stdio.h"
#endif

int main()
{
    //printf("%d\n", check_SVM_support());
    unsigned char ret = check_SVM_support();
    put_int(ret);
    //__asm("outb %b0, $0xf1"::"r"(ret):);
    while(1);
}
