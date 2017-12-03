#include "ImageArea.h"
#include "DrawHistogram.h"

#define HISTOGRAM_START_X (IMAGE_W-256)
#define HISTOGRAM_START_Y (IMAGE_H-5)

DrawHistogram *DrawHistogram::m_ptrInstance = NULL;

DrawHistogram::DrawHistogram()
{
    	int i;

	m_on_off = 0;

	for (i=0; i<256; i++)
	    	m_sum[i] = 0;

	m_ptrImg = ImageArea::GetInstance();
}

DrawHistogram::~DrawHistogram()
{	
    	if (m_ptrInstance != NULL)
		delete m_ptrInstance;
}

DrawHistogram* DrawHistogram::GetInstance()
{
	if (m_ptrInstance == NULL)
		m_ptrInstance = new DrawHistogram;

	return m_ptrInstance;
}

unsigned char DrawHistogram::GetOnOff(void)
{
    	return m_on_off;
}

void DrawHistogram::SetOnOff(unsigned char sign)
{
	m_on_off = sign;

	if (m_on_off == 0)
	{
		DrawCoorOrigin();
		ClearHistogramImage();
		ClearHistogramData();
	}
}

void DrawHistogram::SetArea(POINT area_start, POINT area_end)
{
	if (area_start.x <= area_end.x)
	{
    		m_area_start.x = area_start.x;
    		m_area_end.x = area_end.x;
	}
	else
	{
    		m_area_start.x = area_end.x;
    		m_area_end.x = area_start.x;
	}

	if (area_start.y <= area_end.y)
	{
    		m_area_start.y = area_start.y;
    		m_area_end.y = area_end.y;
	}
	else
	{
    		m_area_start.y = area_end.y;
    		m_area_end.y = area_start.y;
	}
	DrawCoorOrigin();
}

void DrawHistogram::DrawCoorOrigin(void)
{
    GdkGC *gc = m_ptrImg->NewHistogramDC(g_white, GDK_COPY);

    const POINT coor_origin={0, 150+1};//坐标原点
    POINT tmp1, tmp2;

    //画坐标系
    tmp1.x = coor_origin.x + 256;
    tmp1.y = coor_origin.y;
    tmp2.x = coor_origin.x;
    tmp2.y = coor_origin.y;
    m_ptrImg->DrawHistogram(gc, tmp1.x, tmp1.y, tmp2.x, tmp2.y);

    tmp1.x = coor_origin.x + 127;
    tmp1.y = coor_origin.y;
    tmp2.x = coor_origin.x + 127;
    tmp2.y = coor_origin.y - 5;
    m_ptrImg->DrawHistogram(gc, tmp1.x, tmp1.y, tmp2.x, tmp2.y);

    tmp1.x = coor_origin.x;
    tmp1.y = coor_origin.y;
    tmp2.x = coor_origin.x;
    tmp2.y = coor_origin.y - 150;
    m_ptrImg->DrawHistogram(gc, tmp1.x, tmp1.y, tmp2.x, tmp2.y);

    g_object_unref(gc);
}

void DrawHistogram::ClearHistogramData(void)
{
    int i;	

    for (i=0; i<256; i++)
        m_sum[i]=0;
}

void DrawHistogram::ClearHistogramImage(void)
{
    GdkGC *gc = m_ptrImg->NewHistogramDC(g_white, GDK_XOR);

    const POINT coor_origin={1, 150};//坐标原点
    POINT tmp1, tmp2;
    int temp;
    int i;
	
    for (i=0; i<256; i++) {
        temp = (int)m_sum[i];
        if (temp != 0) {
            tmp1.x = coor_origin.x + i;
            tmp1.y = coor_origin.y - temp;
            tmp2.x = coor_origin.x + i;
            tmp2.y = coor_origin.y - 1;
            m_ptrImg->DrawHistogram(gc, tmp1.x, tmp1.y, tmp2.x, tmp2.y);
        }
    }
    g_object_unref(gc);
}

void DrawHistogram::HistogramDraw(unsigned char *image_p)
{
	unsigned int i, j;
	unsigned int max = 0;
	unsigned int width, height;
	unsigned char color;
	unsigned char *field_p;
	float base;
	const POINT coor_origin={1, 150};//坐标原点
	POINT tmp1, tmp2;
	int temp;

        GdkGC *gc = m_ptrImg->NewHistogramDC(g_white, GDK_XOR);

	for (i=0; i<256; i++)
	{
		temp = m_sum[i];
		if (temp != 0)
		{
			tmp1.x = coor_origin.x + i;
			tmp1.y = coor_origin.y - temp;
			tmp2.x = coor_origin.x + i;
			tmp2.y = coor_origin.y - 1;
			m_ptrImg->DrawHistogram(gc, tmp1.x, tmp1.y, tmp2.x, tmp2.y);
		}
	}


	for (i=0; i<256; i++)
		m_sum[i]=0;

	if (m_on_off == 0)
		return;

	width = m_area_end.x - m_area_start.x;
	height = m_area_end.y - m_area_start.y;
	field_p = image_p + (IMG_W * m_area_start.y + m_area_start.x) * 3;

	for (i=0; i<height; i++)
	{
		for (j=0; j<width; j++)
		{
			color = (unsigned char)(0.2989*(*field_p) + 0.587*(*(field_p+1)) + 0.1141*(*(field_p+2)));//彩色直方图公式
			m_sum[color]++;
			field_p += BYTES_DOT;	
		}
		field_p += (IMG_W - width) * BYTES_DOT;
	}
	
	int m;
	for (i=0; i<256; i++)
	{
		if (m_sum[i] > max)
		{
			max = m_sum[i];
			m = i;
		}
	}
//	printf("max= %d\n", m);

	if(max != 0)
		base = 150.0 / (float)max;
	else
		base = 150.0;



	for (i=0; i<256; i++)
	{
		temp = (int)(m_sum[i]*base);
		if (temp != 0)
		{
			tmp1.x = coor_origin.x + i;
			tmp1.y = coor_origin.y - temp;
			tmp2.x = coor_origin.x + i;
			tmp2.y = coor_origin.y - 1;
			m_ptrImg->DrawHistogram(gc, tmp1.x, tmp1.y, tmp2.x, tmp2.y);
		}
		m_sum[i] = temp;
	}
        
        g_object_unref(gc);
}
