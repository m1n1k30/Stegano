#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image.h"

// ─── helper ───────────────────────────────────────────────────────────────────

static int ends_with(const char *str, const char *suffix) {
    int str_len    = strlen(str);
    int suffix_len = strlen(suffix);
    if (suffix_len > str_len) return 0;
    return strcmp(str + str_len - suffix_len, suffix) == 0;
}

// ─── BMP ──────────────────────────────────────────────────────────────────────

static Image *load_bmp(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (f == NULL) {
        printf("Error: could not open file %s\n", filename);
        return NULL;
    }

    Image *img = malloc(sizeof(Image));
    if (img == NULL) {
        printf("Error: could not allocate memory\n");
        fclose(f);
        return NULL;
    }

    fread(&img->file_header, sizeof(BMPFileHeader), 1, f);
    fread(&img->info_header, sizeof(BMPInfoHeader), 1, f);

    if (img->file_header.type != 0x4D42) {
        printf("Error: not a BMP file\n");
        free(img);
        fclose(f);
        return NULL;
    }

    img->width  = img->info_header.width;
    img->height = img->info_header.height;
    img->is_png = 0;

    fseek(f, img->file_header.offset, SEEK_SET);

    img->pixels = malloc(img->height * sizeof(Pixel *));
    for (int i = 0; i < img->height; i++) {
        img->pixels[i] = malloc(img->width * sizeof(Pixel));
    }

    int padding = (4 - (img->width * 3) % 4) % 4;

    for (int i = img->height - 1; i >= 0; i--) {
        for (int j = 0; j < img->width; j++) {
            fread(&img->pixels[i][j].b, 1, 1, f);
            fread(&img->pixels[i][j].g, 1, 1, f);
            fread(&img->pixels[i][j].r, 1, 1, f);
        }
        fseek(f, padding, SEEK_CUR);
    }

    fclose(f);
    return img;
}

static void save_bmp(Image *img, const char *filename) {
    FILE *f = fopen(filename, "wb");
    if (f == NULL) {
        printf("Error: could not create file %s\n", filename);
        return;
    }

    fwrite(&img->file_header, sizeof(BMPFileHeader), 1, f);
    fwrite(&img->info_header, sizeof(BMPInfoHeader), 1, f);

    fseek(f, img->file_header.offset, SEEK_SET);

    int padding = (4 - (img->width * 3) % 4) % 4;
    unsigned char pad_byte = 0;

    for (int i = img->height - 1; i >= 0; i--) {
        for (int j = 0; j < img->width; j++) {
            fwrite(&img->pixels[i][j].b, 1, 1, f);
            fwrite(&img->pixels[i][j].g, 1, 1, f);
            fwrite(&img->pixels[i][j].r, 1, 1, f);
        }
        for (int p = 0; p < padding; p++) {
            fwrite(&pad_byte, 1, 1, f);
        }
    }

    fclose(f);
}

// ─── PNG ──────────────────────────────────────────────────────────────────────

static Image *load_png(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (f == NULL) {
        printf("Error: could not open file %s\n", filename);
        return NULL;
    }

    png_structp png  = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop   info = png_create_info_struct(png);

    if (setjmp(png_jmpbuf(png))) {
        printf("Error: failed to read PNG file\n");
        fclose(f);
        return NULL;
    }

    png_init_io(png, f);
    png_read_info(png, info);

    int width      = png_get_image_width(png, info);
    int height     = png_get_image_height(png, info);
    png_byte color = png_get_color_type(png, info);
    png_byte depth = png_get_bit_depth(png, info);

    if (depth == 16)
        png_set_strip_16(png);
    if (color == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);
    if (color == PNG_COLOR_TYPE_GRAY && depth < 8)
        png_set_expand_gray_1_2_4_to_8(png);
    if (png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);
    if (color == PNG_COLOR_TYPE_RGB  ||
        color == PNG_COLOR_TYPE_GRAY ||
        color == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
    if (color == PNG_COLOR_TYPE_GRAY ||
        color == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);

    png_read_update_info(png, info);

    png_bytep *rows = malloc(height * sizeof(png_bytep));
    for (int i = 0; i < height; i++)
        rows[i] = malloc(png_get_rowbytes(png, info));

    png_read_image(png, rows);
    fclose(f);
    png_destroy_read_struct(&png, &info, NULL);

    Image *img  = malloc(sizeof(Image));
    img->width  = width;
    img->height = height;
    img->is_png = 1;

    img->pixels = malloc(height * sizeof(Pixel *));
    for (int i = 0; i < height; i++) {
        img->pixels[i] = malloc(width * sizeof(Pixel));
        for (int j = 0; j < width; j++) {
            img->pixels[i][j].r = rows[i][j * 4 + 0];
            img->pixels[i][j].g = rows[i][j * 4 + 1];
            img->pixels[i][j].b = rows[i][j * 4 + 2];
        }
        free(rows[i]);
    }
    free(rows);

    return img;
}

static void save_png(Image *img, const char *filename) {
    FILE *f = fopen(filename, "wb");
    if (f == NULL) {
        printf("Error: could not create file %s\n", filename);
        return;
    }

    png_structp png  = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop   info = png_create_info_struct(png);

    if (setjmp(png_jmpbuf(png))) {
        printf("Error: failed to write PNG file\n");
        fclose(f);
        return;
    }

    png_init_io(png, f);
    png_set_IHDR(png, info,
        img->width, img->height, 8,
        PNG_COLOR_TYPE_RGB,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);

    png_bytep *rows = malloc(img->height * sizeof(png_bytep));
    for (int i = 0; i < img->height; i++) {
        rows[i] = malloc(img->width * 3);
        for (int j = 0; j < img->width; j++) {
            rows[i][j * 3 + 0] = img->pixels[i][j].r;
            rows[i][j * 3 + 1] = img->pixels[i][j].g;
            rows[i][j * 3 + 2] = img->pixels[i][j].b;
        }
    }

    png_write_image(png, rows);
    png_write_end(png, NULL);

    for (int i = 0; i < img->height; i++)
        free(rows[i]);
    free(rows);

    png_destroy_write_struct(&png, &info);
    fclose(f);
}

// ─── public API ───────────────────────────────────────────────────────────────

Image *load_image(const char *filename) {
    if (ends_with(filename, ".bmp") || ends_with(filename, ".BMP"))
        return load_bmp(filename);
    else if (ends_with(filename, ".png") || ends_with(filename, ".PNG"))
        return load_png(filename);
    else {
        printf("Error: unsupported format. Use .bmp or .png\n");
        return NULL;
    }
}

void save_image(Image *img, const char *filename) {
    if (img->is_png)
        save_png(img, filename);
    else
        save_bmp(img, filename);
}

void free_image(Image *img) {
    for (int i = 0; i < img->height; i++)
        free(img->pixels[i]);
    free(img->pixels);
    free(img);
}