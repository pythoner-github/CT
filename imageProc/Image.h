#ifndef IMAGE_H_
#define IMAGE_H_

#include "Def.h"
#include "../display/ImageArea.h"

class Image
{
	public:
        struct AutoTracePara
        {
            std::vector<POINT> vecTrace;
            std::vector<POINT> vecMaxTrace;
            int cycleBegin;
            int cycleEnd;
        };

		Image(unsigned int* ptrImgSrc, AutoTracePara para);
		Image(const Image &obj);
		~Image();

		static const int HEIGHT = IMG_AREA_H;
		static const int WIDTH = IMG_AREA_W;

		unsigned int* GetImg();
        const AutoTracePara* GetTraceData(void);

	private:
		static const char BYTES = 3; ///< bits of one pixel is BYTES * 8 = 24
		static const int SIZE = HEIGHT * WIDTH * BYTES / 4;

		unsigned int *m_ptrImg;
        AutoTracePara m_tracePara;

};
#endif
