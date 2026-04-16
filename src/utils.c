#include "utils.h"

unsigned char get_bit(unsigned char byte, int position) {
    return (byte >> position) & 1;
}

unsigned char set_bit(unsigned char byte, int position, int value) {
    if (value == 1) {
        return byte | (1 << position);
    } else {
        return byte & ~(1 << position);
    }
}