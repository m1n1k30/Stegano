#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>
#include <png.h>

#pragma pack(1)

typedef struct {
    uint16_t type;
    uint32_t file_size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
} BMPFileHeader;

typedef struct {
    uint32_t header_size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bits_per_pixel;
    uint32_t compression;
    uint32_t image_size;
    int32_t x_pixels_per_meter;
    int32_t y_pixels_per_meter;
    uint32_t colors_in_table;
    uint32_t important_color_count;
} BMPInfoHeader;

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Pixel;

typedef struct {
    BMPFileHeader file_header;
    BMPInfoHeader info_header;
    Pixel **pixels;
    int width;
    int height;
    int is_png;
} Image;

Image *load_image(const char *filename);
void save_image(Image *img, const char *filename);
void free_image(Image *img);

#endif