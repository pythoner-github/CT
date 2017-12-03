#ifndef CREATE_EMP_H
#define CREATE_EMP_H 

#include <stdio.h>
#include <unistd.h>

bool CreateImageEmp(unsigned char* input, FILE* output, unsigned int width, unsigned int height);
bool ReadImageEmp(const char* path, unsigned char* buf, int width, int height);

bool CreateCineEmp(unsigned char* input[], FILE* output, unsigned int frames, unsigned int width, unsigned int height);
bool ReadCineEmp(FILE* input, unsigned char* buf, int SpecFrame, int width, int height);

#endif
