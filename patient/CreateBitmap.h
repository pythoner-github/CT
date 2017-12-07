#ifndef CREATE_BITMAP_H
#define CREATE_BITMAP_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

bool CreateBitmap24(unsigned char* input, int fd_output, unsigned int width, unsigned int height);
bool ReadBitmap24(const char* path, unsigned char* buf, int width, int height);

#endif