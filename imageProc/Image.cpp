#include <string.h>
#include "imageProc/Image.h"
#include "Def.h"

Image::Image(unsigned int* ptrImgSrc, AutoTracePara para)
{
    m_ptrImg = new unsigned int[SIZE];
    memset(m_ptrImg, 0, SIZE*4);

    if (ptrImgSrc != NULL)
    {
        memcpy(m_ptrImg, ptrImgSrc, SIZE*4);
    }
    m_tracePara = para;
}

Image::Image(const Image &obj)
{
    m_ptrImg = new unsigned int[SIZE];
    memset(m_ptrImg, 0, SIZE*4);
    if (obj.m_ptrImg != NULL)
        memcpy(m_ptrImg, obj.m_ptrImg, SIZE*4);
    m_tracePara = obj.m_tracePara;
}

Image::~Image()
{
    delete []m_ptrImg;
    m_ptrImg = NULL;
}

unsigned int* Image::GetImg()
{
    return m_ptrImg;
}

const Image::AutoTracePara* Image::GetTraceData(void)
{
    return &m_tracePara;
}