#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "steg.h"
#include "utils.h"

static unsigned char xor_byte(unsigned char byte, const char *password, int index) {
    if (password == NULL || strlen(password) == 0)
        return byte;
    int pass_len = strlen(password);
    return byte ^ (unsigned char)password[index % pass_len];
}

void embed_message(Image *img, const char *message, const char *password) {
    int msg_len   = strlen(message) + 1;
    int max_bytes = (img->width * img->height * 3) / 8;

    if (msg_len > max_bytes) {
        printf("Error: message too long for this image\n");
        return;
    }

    int bit_index = 0;

    for (int i = 0; i < msg_len; i++) {
        unsigned char c = xor_byte((unsigned char)message[i], password, i);

        for (int bit = 0; bit < 8; bit++) {
            unsigned char msg_bit = get_bit(c, bit);

            int total_bit = bit_index;
            int channel   = total_bit % 3;
            int pixel_num = total_bit / 3;
            int row       = pixel_num / img->width;
            int col       = pixel_num % img->width;

            if (channel == 0)
                img->pixels[row][col].r = set_bit(img->pixels[row][col].r, 0, msg_bit);
            else if (channel == 1)
                img->pixels[row][col].g = set_bit(img->pixels[row][col].g, 0, msg_bit);
            else
                img->pixels[row][col].b = set_bit(img->pixels[row][col].b, 0, msg_bit);

            bit_index++;
        }
    }
}

char *extract_message(Image *img, const char *password) {
    int max_bytes = (img->width * img->height * 3) / 8;
    char *message = malloc(max_bytes);
    if (message == NULL) {
        printf("Error: could not allocate memory\n");
        return NULL;
    }

    int bit_index = 0;

    for (int i = 0; i < max_bytes; i++) {
        unsigned char c = 0;

        for (int bit = 0; bit < 8; bit++) {
            int total_bit = bit_index;
            int channel   = total_bit % 3;
            int pixel_num = total_bit / 3;
            int row       = pixel_num / img->width;
            int col       = pixel_num % img->width;

            unsigned char pixel_bit;
            if (channel == 0)
                pixel_bit = get_bit(img->pixels[row][col].r, 0);
            else if (channel == 1)
                pixel_bit = get_bit(img->pixels[row][col].g, 0);
            else
                pixel_bit = get_bit(img->pixels[row][col].b, 0);

            c = set_bit(c, bit, pixel_bit);
            bit_index++;
        }

        c = xor_byte(c, password, i);
        message[i] = c;

        if (c == '\0')
            break;
    }

    return message;
}