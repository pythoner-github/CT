#include "CreateEmp.h"
#include <errno.h>

bool CreateImageEmp(unsigned char* input, FILE* output, unsigned int width, unsigned int height)
{
	unsigned int size = width * height * 3;

    fwrite(&width, sizeof(char), 2, output);
    fwrite(&height, sizeof(char), 2, output);
    int bitCount = 24;
    fwrite(&bitCount, sizeof(char), 2, output);

	if(fwrite(input, sizeof(char), size, output) != size)
	{
		perror("Failed to write emp file, error:");
		return false;
	}
	return true;
}

bool ReadImageEmp(const char* path, unsigned char* buf, int width, int height)
{
	FILE *fs = fopen(path, "rb");
	unsigned int size = width * height * 3;

    char tmp[6];
    fread(tmp, sizeof(char), 6, fs);
	if(fread(buf, sizeof(char), size, fs) != size)
	{
		perror("fread error:");
		fclose(fs);
		return false;
	}
	fclose(fs);
	return true;
}

bool CreateCineEmp(unsigned char* input[], FILE* output, unsigned int frames, unsigned int width , unsigned int height)
{
	unsigned int i;
	unsigned char *ptr;
	unsigned int frame_size = width * height *3;

    fwrite(&width, sizeof(char), 2, output);
    fwrite(&height, sizeof(char), 2, output);
    fwrite(&frames, sizeof(char), 2, output);
    int bitCount = 24;
    fwrite(&bitCount, sizeof(char), 2, output);

	for(i=0; i<frames; i++)
	{
		ptr = input[i];
	//	PRINTF("ptr=0x%x\n", ptr);
		if(fwrite(ptr, sizeof(char), frame_size, output) != frame_size)
		{
			perror("Failed to write video data, error:");
			return false;
		}
	}
	return true;
}

bool ReadCineEmp(FILE* input, unsigned char* buf, int SpecFrame, int width, int height)
{
	unsigned int frame_size = width * height * 3;
	
	if(fseek(input, frame_size*SpecFrame+8, SEEK_SET) < 0)
	{
		perror("fseek error:");
		return false;
	}
	else
	{
		if(fread(buf, sizeof(char), frame_size, input) != frame_size)
		{
			perror("fread error:");
			return false;
		}
	}
	return true;
}
