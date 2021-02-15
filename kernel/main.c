#include <intr.h>
#include <init.h>
#include <console.h>

int main()
{
    disable_intr();
    pminit();
    vminit();
    device_init();
    trap_init();
    fsinit();
    printf("init complete!");
    while(1);
/*
    disable_intr();
    pminit();
    vminit();
    device_init();
    while(1);
    enable_intr();
    uinit();
*/
}

