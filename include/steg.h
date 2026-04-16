#ifndef STEG_H
#define STEG_H

#include "image.h"

void embed_message(Image *img, const char *message, const char *password);
char *extract_message(Image *img, const char *password);

#endif