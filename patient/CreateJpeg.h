#ifndef CREATE_JPEG_H
#define CREATE_JPEG_H


//void CreateJpeg(unsigned char *input, FILE *output, int Q, int width, int height);
void CreateJpeg(unsigned char *image_buffer, FILE *outfile, int quality, int image_width, int image_height);

#endif

