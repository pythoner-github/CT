#ifndef CREATE_AVI_H 
#define CREATE_AVI_H 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define AVI_HEAD	224
#define AVI_FRAME_HEAD	8

 /*************************************************************************************************************************
 //Parameter Explanation: in the most important function CreateAvi()
 //                        the first parameter (unsigned char *inputfile) is input data,
 //                        the second parameter (FILE *outputjpgfile) is output file,
 //                        the third parameter (int WIDTH) is the width of the image,
 //                        the fourth parameter (int HEIGHT) is the height of the image,
 //                        the fifth parameter (int NumberOfFrames) is the number of the frames,
 //                        the sixth parameter (int FRAMERATE) is the rate of the avi(number of the frame per second)
 **************************************************************************************************************************/

void CreateAvi(unsigned char* inputfile[], FILE *outputfile, int WIDTH, int HEIGHT, int NumberOfFrames, int FRAMERATE);
bool ReadAvi(FILE *fs, unsigned char* buf, unsigned int spec_frame, int width, int height);

bool read_avifile_width(FILE *file, int& WIDTH);
bool read_avifile_height(FILE *file, int& HEIGHT);
bool read_avifile_frames(FILE *file, int& NumberOfFrames);
bool read_avifile_framerate(FILE *file, int& FRAMERATE);  
bool get_avifile_info(FILE *file,int& WIDTH,int& HEIGHT,int& NumberOfFrames,int& FRAMERATE);

#endif
