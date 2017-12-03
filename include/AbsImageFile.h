/**
* @file AbsImageFile.h
* extract interface for return RGB datas, width, height and frameno of multi-type image file.
*
* @version: 1.0
* @author: hehao
* @date: 2009-11-5
*/

#ifndef ABS_IMAGE_FILE_H
#define ABS_IMAGE_FILE_H

#include <string>
using std::string;

class CAbsImageFile
{
public:
	CAbsImageFile(){};
	virtual ~CAbsImageFile(){};
	virtual unsigned char * LoadImageFile(string filename) = 0;
	virtual unsigned int GetImageWidth() = 0;
	virtual unsigned int GetImageHeight() = 0;
	virtual unsigned int GetImageFrameNo() = 0;
};

#endif //ABS_IMAGE_FILE_H
