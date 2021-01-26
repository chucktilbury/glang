#include <stdio.h>
#include <stdint.h>
#include <string.h>

void add_buffer_int(int val) {

    char tbuf[9]; // up to 4 bytes for 32 bit word
    char blist[4][3];
    int idx = 0;

    sprintf(tbuf, "%08X", (uint32_t)val);
    memset(blist, 0, sizeof(blist));
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 2; j++) {
            blist[i][j] = tbuf[idx++];
        }
    }

    idx = 0;
    // skip leading zero bytes
    for(; idx < 4 && blist[idx][0] != 0; idx++)
        if(strcmp(blist[idx], "00"))
            break;

    if(idx >= 4) {
        printf("%d, %s\n", idx, "00");
    }
    else {
        // write the remaining bytes, if any
        for(; idx < 4 && blist[idx][0] != 0; idx++) {
            //add_buffer_char(chbuf, (int)strtol(blist[idx], NULL, 16));
            if(blist[idx][0] == 0)
                break;
            printf("%d, %s\n", idx, blist[idx]);
        }
    }
}

int main() {

    int lst[] = {123, 234, 345, 0, 10230495, -123, -234};
    for(int i = 0; i < 7; i++) {
        printf("value: %d, 0x%08X\n", lst[i], lst[i]);
        add_buffer_int(lst[i]);
    }

    return 0;
}