#ifndef KRNLSTD_H
#define KRNLSTD_H

typedef unsigned long size; // since we don't have stdint.h
typedef unsigned short u16;
typedef unsigned char u8;
typedef unsigned int u32;


#define vgmem 0xB8000 // text mode memory
#define vgawidth 80 // text mode defs
#define vgaheight 25

static u16* vm = (u16*)vgmem;
static size vx = 0; // cursor x position
static size vy = 0; // cursor y pos
static u8 vgcolor = 0x07; // change as you wish

void scroll() {
    // Move all the lines up by one
    for (size y = 1; y < vgaheight; y++) {
        for (size x = 0; x < vgawidth; x++) {
            size from_index = y * vgawidth + x;
            size to_index = (y - 1) * vgawidth + x;
            vm[to_index] = vm[from_index];
        }
    }

    // Clear the last line
    for (size x = 0; x < vgawidth; x++) {
        vm[(vgaheight - 1) * vgawidth + x] = (u16)' ' | (vgcolor << 8);  // Clear the bottom line
    }
}



void putchr(char c) {
    if (c == '\n') {
        vx = 0;
        if (++vy >= vgaheight) {
            vy = vgaheight - 1; // might add scrolling soon
            // added scrolling 15 seconds later
            scroll();
        }

        return;

    }

    size index = vy * vgawidth + vx;
    vm[index] = (u16)c | (vgcolor << 8);

    if (++vx >= vgawidth) {
        vx = 0;
        if (++vy >= vgaheight) {
            vy = vgaheight - 1;
            scroll();
        }
    }
}

void putstr(const char *str) {
    while (*str) {
        putchr(*str++);
    }
}

void clear() {
    for (size y = 0; y < vgaheight; y++) {
        for (size x = 0; x < vgawidth; x++) {
            vm[y * vgawidth + x] = (u16)' ' | (vgcolor << 8);
        }
    }

    vx = 0;
    vy = 0;

    // here, you might add an automatic welcome message whenever the screen is cleared (e.g. "TYPE HELP", "WELCOME TO...")
}


int cmp(const char *str1, const char *str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }


    return *(const unsigned char*)str1 - *(const unsigned char*)str2; // returns 0 if equal, and not zero if false
}

int cmpn(const char *str1, const char *str2, size n) {
    while (n && *str1 && (*str1 == *str2)) {
        n--;
        str1++;
        str2++;
    }

    if (n == 0) {
        return 0;
    }

    return *(const unsigned char*)str1 - *(const unsigned char*)str2;
}

// Now for input handling. 

static const char asciimap[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`',
    0, '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static inline u8 inb(u16 port) {
    u8 ret;
    __asm__ __volatile__ ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

u8 getscan() {
    while (!(inb(0x64) & 1)) { }
    return inb(0x60);
}

char getch() {
    u8 scan = getscan();
    if (scan < 128) {
        return asciimap[scan];
    }

    return 0;
}

void read(char* buffer, size bufsize) {
    size pos = 0;
    while (1) {
        char c = getch();
        if (c) {
            if (c == '\n') {
                buffer[pos] = '\0';
                putchr('\n');
                break;
            }

            else if (c == '\b' && pos > 0) {
                pos--;
                vx = (vx == 0) ? vgawidth - 1 : vx - 1;
                putstr(" \b");
            }

            else if (pos < bufsize - 1) {
                buffer[pos++] = c;
                putchr(c);
            }
        }
    }
}

void reboot() {
    __asm__ __volatile__ (
        "int $0x19"
        :
        :
        : "memory"
    );
}


void parse(const char *cmd) {
    if (cmpn(cmd, "ECHO ", 5) == 0) {
        putstr(cmd + 5);
    }

    else if (cmp(cmd, "REBOOT") == 0) {
        reboot();
    }

    else {
        putstr("INVALID COMMAND: ");
        if (cmpn(cmd, "ECHO", 2) == 0) {
            putstr("DID YOU MEAN ECHO?");
        }

        else if (cmpn(cmd, "REBOOT", 2) == 0) {
            putstr("DID YOU MEAN REBOOT?");
        }
    }
}




#endif