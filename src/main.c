#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image.h"
#include "steg.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage:\n");
        printf("  Encode: ./stegano encode input.bmp output.bmp \"message\" [password]\n");
        printf("  Decode: ./stegano decode input.bmp [password]\n");
        return 1;
    }

    if (strcmp(argv[1], "encode") == 0) {
        if (argc < 5) {
            printf("Error: encode requires input output and a message\n");
            return 1;
        }

        char *input_file  = argv[2];
        char *output_file = argv[3];
        char *message     = argv[4];
        char *password    = argc >= 6 ? argv[5] : NULL;

        Image *img = load_image(input_file);
        if (img == NULL) return 1;

        embed_message(img, message, password);
        save_image(img, output_file);
        free_image(img);

        if (password != NULL)
            printf("Message embedded with password protection into %s\n", output_file);
        else
            printf("Message embedded successfully into %s\n", output_file);

    } else if (strcmp(argv[1], "decode") == 0) {
        if (argc < 3) {
            printf("Error: decode requires input file\n");
            return 1;
        }

        char *input_file = argv[2];
        char *password   = argc >= 4 ? argv[3] : NULL;

        Image *img = load_image(input_file);
        if (img == NULL) return 1;

        char *message = extract_message(img, password);
        if (message != NULL) {
            printf("Hidden message: %s\n", message);
            free(message);
        }

        free_image(img);

    } else {
        printf("Error: unknown command '%s'\n", argv[1]);
        printf("Use 'encode' or 'decode'\n");
        return 1;
    }

    return 0;
}