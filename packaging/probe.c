#include "neuronte.h"

#include <stdio.h>

int main(void) {
    uint32_t scalar = 0;
    size_t used = neuronte_utf8_decode("A", 1, &scalar);
    if (used != 1 || scalar != 0x41u) {
        return 1;
    }
    puts("neuronte loaded");
    return 0;
}
