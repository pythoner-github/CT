#ifndef CREATE_BITMAP_H
#define CREATE_BITMAP_H 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

 /*************************************************************************************************************************
 //Parameter Explanation: in the most important function CreateBitmap24()
 //                        the first parameter (unsigned char *inputfile) is input data,
 //                        the second parameter (FILE *outputjpgfile) is output file,
 //                        the third parameter (int WIDTH) is the width of the image,
 //                        the fourth parameter (int HEIGHT) is the height of the image,
 **************************************************************************************************************************/

bool CreateBitmap24(unsigned char* input, int fd_output, unsigned int width, unsigned int height);
bool ReadBitmap24(const char* path, unsigned char* buf, int width, int height);

#endif
