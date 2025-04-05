#include "krnlstd.h" // example of use:

void parse(const char *cmd) {
    if (cmp(cmd, "REBOOT") == 0) {
        reboot();
    }

    else if (cmpn(cmd, "ECHO ", 5) == 0) {
        putstr(cmd + 5);
    }

    else {
        putstr("INVALID COMMAND! ");
        if (cmpn(cmd, "ECHO", 2) == 0) { // if the first 2 letters of ECHO and input match
            putstr("DID YOU MEAN ECHO?");
        }

        else if (cmpn(cmd, "REBOOT", 2) == 0) {
            putstr("DID YOU MEAN REBOOT?");
        }
    }
}


void krnlMain() { // probably called by an assembly entry point, e.g. "start.asm"
    clear();
    char inbuf[32]; // not allocating much memory
    while (1) {
        putstr("A[boot[kernel.bin]] > "); // use a custom prompt here.
        read(inbuf, sizeof(inbuf));
        parse(inbuf);
        pustr("\n");
    }
}