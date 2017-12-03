#ifndef _AVI_ENCDEC_H264_H
#define _AVI_ENCDEC_H264_H

typedef void (*PTR_SAVE)(uint8_t *buf, int width, int height);

int CreateAviEncode( unsigned char* inputdata[], FILE * outputfile, 
                     int WIDTH, int HEIGHT, int frame_total, int frame_rate);
bool ReadAviData(const char *src_filename, uint8_t *buf,int frame_total, int width, int height, PTR_SAVE ptrFunc);
#endif 










