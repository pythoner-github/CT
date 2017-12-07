#ifndef CREATE_AVI_H
#define CREATE_AVI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define AVI_HEAD	224
#define AVI_FRAME_HEAD	8

void CreateAvi(unsigned char* inputfile[], FILE *outputfile, int WIDTH, int HEIGHT, int NumberOfFrames, int FRAMERATE);
bool ReadAvi(FILE *fs, unsigned char* buf, unsigned int spec_frame, int width, int height);

bool read_avifile_width(FILE *file, int& WIDTH);
bool read_avifile_height(FILE *file, int& HEIGHT);
bool read_avifile_frames(FILE *file, int& NumberOfFrames);
bool read_avifile_framerate(FILE *file, int& FRAMERATE);
bool get_avifile_info(FILE *file,int& WIDTH,int& HEIGHT,int& NumberOfFrames,int& FRAMERATE);

#endif