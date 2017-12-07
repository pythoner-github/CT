#ifndef IMAGE_FILE_H
#define IMAGE_FILE_H

#include "AbsImageFile.h"

class CImageFile: public CAbsImageFile
{
public:
    CImageFile();
    virtual ~CImageFile();
    virtual unsigned char * LoadImageFile(string filename);
    virtual unsigned int GetImageWidth();
    virtual unsigned int GetImageHeight();
    virtual unsigned int GetImageFrameNo();

private:
    unsigned char * LoadJPG(string fileName);
    unsigned char * LoadBMP(string fileName);
    unsigned char * LoadAVI(string fileName);

    unsigned char *m_pBuffer;
    unsigned int m_width;
    unsigned int m_height;
    unsigned int m_frameNo;

    enum IMGType{JPG, BMP, AVI};
    IMGType m_type;
};

#endif //IMAGE_FILE_H