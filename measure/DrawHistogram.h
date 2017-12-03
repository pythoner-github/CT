#include "MeasureDraw.h"

class DrawHistogram
{
    public:
	~DrawHistogram();
	
	static DrawHistogram* GetInstance();
	unsigned char GetOnOff();
	void SetOnOff(unsigned char sign);
	void SetArea(POINT area_start, POINT area_end);
	void HistogramDraw(unsigned char *image_p);
	void ClearHistogramData(void);
	void DrawCoorOrigin(void);
	
    private:
	DrawHistogram();
	void ClearHistogramImage(void);
	unsigned char m_on_off;
	POINT m_area_start; 
	POINT m_area_end; 
	unsigned int m_sum[256];
	ImageArea *m_ptrImg;
	static DrawHistogram* m_ptrInstance;
};
