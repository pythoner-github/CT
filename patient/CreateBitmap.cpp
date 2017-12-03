#include "CreateBitmap.h"
#include <errno.h>

#pragma pack(1)
typedef struct{
	unsigned short bfType;
	unsigned int bfSize;
	unsigned int bfReserved;
	unsigned int bfOffBits;
}BITMAPFILEHEADER;
#pragma pack()

typedef struct{
	unsigned int biSize;
	long biWidth;
	long biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned int biCompression;
	unsigned int biSizeImage;
	long biXPelsPerMeter;
	long biYPelsPerMeter;
	unsigned int biClrUsed;
	unsigned int biClrImportant;
}BITMAPINFOHEADER;

bool CreateBitmap24(unsigned char* input, int fd_output, unsigned int width, unsigned int height)
{
	unsigned int FileSize, ImgSize;
	BITMAPFILEHEADER bf;
	BITMAPINFOHEADER bi;

	ImgSize = width * height * 3;
	FileSize = 14 + sizeof(BITMAPINFOHEADER) + ImgSize;

	bf.bfType = 0x4d42;
	bf.bfSize = FileSize;
	bf.bfReserved = 0;
	bf.bfOffBits = FileSize - ImgSize;

	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	bi.biHeight = height;
	bi.biPlanes = 1;
	bi.biBitCount = 24;
	bi.biCompression = 0;
	bi.biSizeImage = ImgSize;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	if(write(fd_output, (const void*)&bf, 14)==-1)
	{
		perror("Failed to write bitmap file header, error:");
		return false;
	}
	if(write(fd_output, (const void*)&bi, sizeof(BITMAPINFOHEADER))==-1)
	{
		perror("Failed to write bitmap info header, error:");
		return false;
	}

	unsigned char data[width*height*3];
	unsigned int i, j, n;
	for(i=0; i<height; i++)
		for(j=0; j<width; j++)
			for(n=0; n<3; n++)
				*(data+(height-i-1)*width*3+j*3+n) = *(input+(i*3*width+j*3+(2-n)));	//上下颠倒，BGR格式

	if(write(fd_output, data, ImgSize)==-1)
	{
		perror("Failed to write bitmap info header, error:");
		return false;
	}

	return true;
}

bool ReadBitmap24(const char* path, unsigned char* buf, int width, int height)
{
	FILE *fs = fopen(path, "rb");
	unsigned int size = width * height * 3;
	
	if(!fs)
	{
		perror("fopen error:");
		return false;
	}
	
	if(fseek(fs, 14+sizeof(BITMAPINFOHEADER), SEEK_SET) < 0)
	{
		perror("fseek error:");
		fclose(fs);
		return false;
	}
	else
	{
		if(fread(buf, sizeof(char), size, fs) != size)
		{
			perror("fread error:");
			fclose(fs);
			return false;
		}
	}
	fclose(fs);
	return true;
}
