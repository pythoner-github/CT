#include "ImageArea.h"
#include "ViewMain.h"
#include <math.h>
#include "../imageProc/Replay.h"
#include "../imageProc/MenuReview.h"
#include "ImageAreaPara.h"
#include "MeasureFactory.h"
#include "DrawHistogram.h"
#include "ImageAreaDraw.h"
#include "../imageProc/ImgProcPw.h"
#include "../imageProc/ImgProc2D.h"
#include <pango-1.0/pango/pangoft2.h>
#include "../lib/include/ImageMix.h"
#include "../lib/include/DSCTypes.h"
#include "../base/CalcTime.h"
#include "../measure/CDrawIMT.h"
#include "../sysMan/SysMeasurementSetting.h"
#include "../keyboard/KeyDef.h"
#include "SysGeneralSetting.h"
#include "ViewSystem.h"
namespace {
	//	const int MeasureResult_H = 17;	// 测量结果每行高度(像素)
	const int kHistogramWidth = 256+2;
	const int kHistogramHeight = 150+2;
	const int kHistogramX = IMAGE_X + IMAGE_W - kHistogramWidth;
	const int kHistogramY = IMAGE_Y + IMAGE_H - kHistogramHeight - 5;
	const int kEFOVSpeedbarWidth = 100;
}

GtkWidget *ImageArea::m_imageDA = 0;
unsigned char ImageArea::m_bitsImg[];
unsigned char ImageArea::m_TpbitsIMT[];
//unsigned char ImageArea::m_bitsReplay[];
unsigned char ImageArea::m_bitsLastReplay[];
unsigned char ImageArea::m_bitsReplayMix[];
unsigned char ImageArea::m_bitsMix[];
unsigned char ImageArea::m_bitsIMT[];
ImageArea::RectArea ImageArea::m_symbolArea = {IMAGE_X, IMAGE_Y, IMAGE_W, IMAGE_H};
ImageArea::RectArea ImageArea::m_spectraArea = {IMAGE_X, IMAGE_Y, IMAGE_W, IMAGE_H};

unsigned char ImageArea::m_bitsEFOV[];

int ImageArea::m_counts = 0;

bool g_flagImageData = true;

ImageArea* ImageArea::m_ptrInstance = NULL;
int ImageArea::m_cineRemoveImg = 0;

#ifdef EMP_355
extern int g_fps;
#endif

ImageArea* ImageArea::GetInstance()
{
	if (m_ptrInstance == NULL)
		m_ptrInstance = new ImageArea;
	return m_ptrInstance;
}

ImageArea::ImageArea(void)
{
	m_imgPixbuf = 0;
	m_snapPixbufBak = 0;
	m_drawPwTrace = false;

	m_imageSymbol = 0;
	m_imageIMT = 0; //hlx
	m_imageSpectra = 0;
	m_imagePara = 0;
	m_imagePreBox = 0;
	m_mixPixbuf = 0;
	m_mixReplayPixbuf = 0;
	m_frmPixbuf = 0;

	m_pixmapFps = 0;
	m_pixmapHistogram = 0;
	m_pixmapArea = 0;
	m_pixmapReplayBar = 0;
	m_pixmapPwTrace = 0;
	m_pixmapMeaResult = 0;
	for(int i=0; i<4; i++)
	{
		m_pixmapBDMK[i] = 0;
		m_bdmkPos[i].x = 0;
		m_bdmkPos[i].y = 0;
		m_bdmkWidth[i] = 0;
		m_bdmkHeight[i] = 0;
	}
	m_inBDMK = false;
	m_bdmkLast = 0;

	m_gcTrace = 0;
	m_gcMeaResult = 0;

	m_inReadImg = false;
	m_meaResultLineH = 18;
	m_meaResultPos.clear();
	m_tmFps = 0;
    m_counts = 0;
    m_baseFont = pango_font_description_from_string("DejaVu Sans Mono, Book, 10");
    m_cineRemoveImg = 0;
    m_speedbarDraw = false;
#ifdef TRANSDUCER
    m_pixmapTransducer =0;
    m_transducerGainBak = 0;
#endif
    m_curCountLines = 0;
}

ImageArea::~ImageArea()
{
	if (m_ptrInstance != NULL)
		delete m_ptrInstance;

	cvReleaseImage(&m_imageSymbol);
	cvReleaseImage(&m_imageIMT);
	cvReleaseImage(&m_imageSpectra);
	cvReleaseImage(&m_imagePara);
	cvReleaseImage(&m_imagePreBox);
	pango_font_description_free(m_baseFont);
	g_object_unref(m_gcTrace);
	g_object_unref(m_gcMeaResult);
	g_object_unref(m_layout);
}

static gboolean DscDraw(gpointer data)
{
	if (data) {
		ImageArea::GetInstance()->SetTraceDraw(true);
		gtk_widget_queue_draw((GtkWidget*)data);
		g_flagImageData = true;
	}
	return FALSE;
}

//draw image when unfreeze, called by DSC
void ImageArea::DrawImgData(void* pBits, int nWidth, int nHeight)
{
	if (ModeStatus::IsEFOVMode()) {
		if (nWidth==IMG_AREA_W)
			memcpy(m_bitsEFOV, pBits, IMG_AREA_W*IMG_AREA_H*IMG_BPP);
	}
	else {
		if (nWidth==IMAGE_W)
			memcpy(m_bitsImg, pBits, IMAGE_W*IMAGE_H*IMG_BPP);
	}

	g_idle_add_full(G_PRIORITY_HIGH_IDLE, DscDraw, (gpointer)m_imageDA, NULL);
	m_counts++;
}

//draw image when freeze, called by Replay
void ImageArea::DrawImgDataFreeze(void* pBits, int nWidth, int nHeight)
{
	memcpy(m_bitsLastReplay, pBits, nWidth*nHeight*IMG_BPP);

	gtk_widget_queue_draw(m_imageDA);
}

//draw image when pure freeze, called by DSC
void ImageArea::CopyImgDataToReplay(void)
{
	for (int j = 0; j < IMAGE_H; j ++)
	{
		for (int i = 0; i < IMAGE_W; i ++)
		{
			for (int k = 0; k < IMG_BPP; k ++)
			{
		    	m_bitsLastReplay[(IMAGE_Y + j) * IMG_AREA_W * IMG_BPP + (IMAGE_X + i) * IMG_BPP + k] = m_bitsImg[j * IMAGE_W * IMG_BPP + i * IMG_BPP + k];
				//m_bitsLastReplay[(IMAGE_Y + j) * IMG_AREA_W * IMG_BPP + (IMAGE_X + i) * IMG_BPP + k] = 0;
			}
		}
	}
	//memcpy(m_bitsLastReplay, m_bitsImg, IMAGE_W*IMAGE_H*IMG_BPP);
	gtk_widget_queue_draw(m_imageDA);
}

void ImageArea::UpdateSymbolArea(int x, int y, int width, int height)
{
	m_symbolArea.x = x;
	m_symbolArea.y = y;
	m_symbolArea.w = width;
	m_symbolArea.h = height;
}

void ImageArea::UpdateSpectraArea(int x, int y, int width, int height)
{
	m_spectraArea.x = x;
	m_spectraArea.y = y;
	m_spectraArea.w = width;
	m_spectraArea.h = height;
}

void ImageArea::UpdateMeaResultArea(int fontsize)
{
    UpdateMeasure update;
    if(fontsize == 0)
        fontsize = 10;
    else if(fontsize == 1)
        fontsize = 14;
    else
        fontsize = 10;
    m_meaResultFontSize = fontsize;
    PRINTF("fontSize:%d\n", fontsize);
    char str_font[256];
    sprintf(str_font, "%s, %d", "DejaVu Sans Mono, Book", fontsize);
    PangoFontDescription *des = pango_font_description_from_string(str_font);
    pango_layout_set_font_description(m_layout, des);
    pango_layout_get_pixel_size(m_layout, NULL, &m_meaResultLineH);
    PRINTF("Line = %d, first H:%d curlines:%d\n",size,  m_meaResultLineH, m_curCountLines);
    if(m_curCountLines > 1)
        m_meaResultLineH = m_meaResultLineH / m_curCountLines;
#ifdef TRANSDUCER
    int measure_line_max = 14;
    if(fontsize == 14)
    {
        measure_line_max =10;
        if(m_meaResultLineH > 25);
            m_meaResultLineH = 25;
    }
    else
    {
        if(m_meaResultLineH > 18)
            m_meaResultLineH = 18;
    }
    m_meaResultRect.h = measure_line_max * m_meaResultLineH;

#else
    if(fontsize == 14)
    {
        if(m_meaResultLineH > 25)
            m_meaResultLineH = 25;
    }
    else
    {
        if(m_meaResultLineH > 18)
            m_meaResultLineH = 18;
    }
    m_meaResultRect.h = MEASURE_RES_LINES_MAX * m_meaResultLineH;
#endif
    //printf("%s, H:%d h:%d\n", __FUNCTION__, m_meaResultLineH, m_meaResultRect.h);
	m_meaResultRect.y = IMG_AREA_H - m_meaResultRect.h;

	if (m_pixmapMeaResult)
		g_object_unref(m_pixmapMeaResult);
	m_pixmapMeaResult = gdk_pixmap_new(m_imageDA->window,
			m_meaResultRect.w,
			m_meaResultRect.h,
			-1);
	if (!m_gcMeaResult)
		m_gcMeaResult = gdk_gc_new(m_pixmapMeaResult);
	gdk_gc_set_foreground(m_gcMeaResult, g_black);
	gdk_draw_rectangle(m_pixmapMeaResult, m_gcMeaResult, TRUE, 0, 0, m_meaResultRect.w, m_meaResultRect.h);

    pango_font_description_free(des);
    update.UpdateResultForChgFont();
}

void ImageArea::DrawCFMPreBox(const RectArea& area)
{
	const unsigned char* symbolData = (unsigned char *)m_imageSymbol->imageData;
	unsigned char* preBoxData = (unsigned char *)m_imagePreBox->imageData;
	const int StartPt = IMAGE_W*area.y*3 + area.x*3;
	const int EndPt = ((area.y+area.h)*IMAGE_W - (IMAGE_W-area.x-area.w))*3;
	for (int i = StartPt; i < EndPt; i+=IMAGE_W*3) {
		for (int j = 0; j < area.w*3; j+=3) {
			if (symbolData[i+j] != 0 || symbolData[i+j+1] != 0 || symbolData[i+j+2] != 0) {
				preBoxData[i+j] = 255;
				preBoxData[i+j+1] = 255;
				preBoxData[i+j+2] = 255;
			}
		}
	}
}

void ImageArea::ClearCFMPreBox(const RectArea& area)
{
	unsigned char* preBoxData = (unsigned char *)m_imagePreBox->imageData;
	const int StartPt = IMAGE_W*area.y*3 + area.x*3;
	const int EndPt = ((area.y+area.h)*IMAGE_W - (IMAGE_W-area.x-area.w))*3;
	for (int i = StartPt; i < EndPt; i+=IMAGE_W*3) {
		for (int j = 0; j < area.w*3; j+=3) {
			preBoxData[i+j] = 0;
			preBoxData[i+j+1] = 0;
			preBoxData[i+j+2] = 0;
		}
	}
}

GtkWidget * ImageArea::Create(void)
{
	m_imageDA = gtk_drawing_area_new();
	gtk_drawing_area_size(GTK_DRAWING_AREA(m_imageDA), IMG_AREA_W, IMG_AREA_H);
	g_signal_connect(G_OBJECT(m_imageDA), "configure_event", G_CALLBACK(HandleImageAreaConfigure), this);
	g_signal_connect(G_OBJECT(m_imageDA), "expose_event", G_CALLBACK(HandleImageAreaExpose), this);
	return m_imageDA;
}

void ImageArea::ImageAreaConfigure(GtkWidget *widget, GdkEventConfigure *event)
{
    m_layout = gtk_widget_create_pango_layout(m_imageDA, NULL);
	pango_layout_set_text(m_layout, "我", -1); //for calculate realy size
	pango_layout_set_font_description(m_layout, m_baseFont);

	m_imgPixbuf = gdk_pixbuf_new_from_data((unsigned char*)m_bitsImg,
			GDK_COLORSPACE_RGB, FALSE, 8,
			IMAGE_W, IMAGE_H,
			IMAGE_W * 3,
			NULL, NULL);

	m_imagePara = cvCreateImage(cvSize(IMG_AREA_W, IMG_AREA_H), IPL_DEPTH_8U, 3);
	cvZero(m_imagePara);

	m_imageSymbol = cvCreateImage(cvSize(IMAGE_W, IMAGE_H), IPL_DEPTH_8U, 3);
	cvZero(m_imageSymbol);

	m_imageIMT = cvCreateImage(cvSize(IMAGE_W, IMAGE_H), IPL_DEPTH_8U, 3);
	cvZero(m_imageIMT);

	m_imageSpectra = cvCreateImage(cvSize(IMAGE_W, IMAGE_H), IPL_DEPTH_8U, 3);
	cvZero(m_imageSpectra);

	m_imagePreBox = cvCreateImage(cvSize(IMAGE_W, IMAGE_H), IPL_DEPTH_8U, 3);
	cvZero(m_imagePreBox);

	m_mixPixbuf = gdk_pixbuf_new_from_data((unsigned char*)m_bitsMix,
			GDK_COLORSPACE_RGB, FALSE, 8,
			IMG_AREA_W, IMG_AREA_H,
			IMG_AREA_W * 3,
			NULL, NULL);
	//hlx
	m_IMTPixbuf = gdk_pixbuf_new_from_data((unsigned char*)m_bitsIMT,
			GDK_COLORSPACE_RGB, FALSE, 8,
			IMG_AREA_W, IMG_AREA_H,
			IMG_AREA_W * 3,
			NULL, NULL);

	m_lastReplayPixbuf = gdk_pixbuf_new_from_data((unsigned char*)m_bitsLastReplay,
			GDK_COLORSPACE_RGB, FALSE, 8,
			IMG_AREA_W, IMG_AREA_H,
			IMG_AREA_W * 3,
			NULL, NULL);

	m_mixReplayPixbuf = gdk_pixbuf_new_from_data((unsigned char*)m_bitsReplayMix,
			GDK_COLORSPACE_RGB, FALSE, 8,
			IMG_AREA_W, IMG_AREA_H,
			IMG_AREA_W * 3,
			NULL, NULL);

	if (m_pixmapFps)
		g_object_unref(m_pixmapFps);
	m_pixmapFps = gdk_pixmap_new(widget->window, FPS_POS_W, FPS_POS_H, -1);
	GdkGC *gc_fps = gdk_gc_new(m_pixmapFps);
	gdk_gc_set_foreground(gc_fps, g_black);
	gdk_draw_rectangle(m_pixmapFps, gc_fps, TRUE, 0, 0, FPS_POS_W, FPS_POS_H);
	g_object_unref(gc_fps);

	// Histogram
	if (m_pixmapHistogram)
		g_object_unref(m_pixmapHistogram);
	m_pixmapHistogram = gdk_pixmap_new(widget->window, kHistogramWidth, kHistogramHeight, -1);
	GdkGC *gc_histogram = gdk_gc_new(m_pixmapHistogram);
	gdk_gc_set_foreground(gc_histogram, g_black);
	gdk_draw_rectangle(m_pixmapHistogram, gc_histogram, TRUE, 0, 0, kHistogramWidth, kHistogramHeight);
	g_object_unref(gc_histogram);

	if (m_pixmapArea)
		g_object_unref(m_pixmapArea);
	m_pixmapArea = gdk_pixmap_new(widget->window,
			IMAGE_W,
			IMAGE_H,
			-1);
	GdkGC *gc_meaArea = gdk_gc_new(m_pixmapArea);
	gdk_gc_set_foreground(gc_meaArea, g_black);
	gdk_draw_rectangle(m_pixmapArea, gc_meaArea, TRUE, 0, 0, IMAGE_W, IMAGE_H);
	g_object_unref(gc_meaArea);

	// Replay Area
	if (m_pixmapReplayBar)
		g_object_unref(m_pixmapReplayBar);
	m_pixmapReplayBar = gdk_pixmap_new(widget->window,
			REPLAY_BAR_W,
			REPLAY_BAR_H,
			-1);
	GdkGC *gc_replayBar = gdk_gc_new(m_pixmapReplayBar);
	gdk_gc_set_foreground(gc_replayBar, g_black);
	gdk_draw_rectangle(m_pixmapReplayBar, gc_replayBar, TRUE, 0, 0, REPLAY_BAR_W, REPLAY_BAR_H);
	g_object_unref(gc_replayBar);

	// Pw Trace Area
	if (m_pixmapPwTrace)
		g_object_unref(m_pixmapPwTrace);
	m_pixmapPwTrace = gdk_pixmap_new(widget->window,
			IMAGE_W,
			IMAGE_H,
			-1);
	if (!m_gcTrace)
		m_gcTrace = gdk_gc_new(m_pixmapPwTrace);
	gdk_gc_set_foreground(m_gcTrace, g_black);
	gdk_draw_rectangle(m_pixmapPwTrace, m_gcTrace, TRUE, 0, 0, IMAGE_W, IMAGE_H);

	// MeasureResult Area
    SysMeasurementSetting sysMeasure;
    m_meaResultFontSize = sysMeasure.GetMeasureResult();
#ifdef TRANSDUCER
    int measure_line_max = 14;
    int meaResult = 10;
    if(m_meaResultFontSize == 0)
        meaResult = 10;
    else if(m_meaResultFontSize == 1)
    {
        meaResult = 14;
        measure_line_max = 10;
    }
    else
        meaResult = 10;
    m_meaResultFontSize = meaResult;
    char str_font[256];
    sprintf(str_font, "%s, %d", "DejaVu Sans Mono, Book", meaResult);
    PangoFontDescription *des = pango_font_description_from_string(str_font);
	pango_layout_set_font_description(m_layout, des);
	pango_font_description_free(des);
	//pango_layout_set_font_description(m_layout, m_baseFont);
	pango_layout_set_alignment(m_layout, PANGO_ALIGN_LEFT);

    pango_layout_get_pixel_size(m_layout, NULL, &m_meaResultLineH);
    m_meaResultRect.w = 400;

    if(m_meaResultFontSize == 14)
    {
         measure_line_max = 10;
        if(m_meaResultLineH >25)
           m_meaResultLineH = 25;
    }
    else
    {
        if(m_meaResultLineH >18)
           m_meaResultLineH =18;
    }
    m_meaResultRect.h = measure_line_max * m_meaResultLineH;

#else
    int meaResult = 10;
    if(m_meaResultFontSize == 0)
        meaResult = 10;
    else if(m_meaResultFontSize == 1)
        meaResult = 14;
    else
        meaResult = 10;
    m_meaResultFontSize = meaResult;
    char str_font[256];
    sprintf(str_font, "%s, %d", "DejaVu Sans Mono, Book", meaResult);
    PangoFontDescription *des = pango_font_description_from_string(str_font);
	pango_layout_set_font_description(m_layout, des);
	pango_font_description_free(des);
	//pango_layout_set_font_description(m_layout, m_baseFont);
	pango_layout_set_alignment(m_layout, PANGO_ALIGN_LEFT);

    pango_layout_get_pixel_size(m_layout, NULL, &m_meaResultLineH);
    if(m_meaResultFontSize == 14)
    {
        if(m_meaResultLineH >25)
           m_meaResultLineH = 25;
    }
    else
    {
        if(m_meaResultLineH >18)
           m_meaResultLineH =18;
    }
    m_meaResultRect.w = 400;
	m_meaResultRect.h = MEASURE_RES_LINES_MAX * m_meaResultLineH;
#endif
	PRINTF("%s: m_meaResultLineH:%d\n", __FUNCTION__,  m_meaResultLineH);
    m_meaResultRect.x = 5;
	m_meaResultRect.y = IMG_AREA_H - m_meaResultRect.h;

	if (m_pixmapMeaResult)
		g_object_unref(m_pixmapMeaResult);
	m_pixmapMeaResult = gdk_pixmap_new(widget->window,
			m_meaResultRect.w,
			m_meaResultRect.h,
			-1);
	if (!m_gcMeaResult)
		m_gcMeaResult = gdk_gc_new(m_pixmapMeaResult);
	gdk_gc_set_foreground(m_gcMeaResult, g_black);
	gdk_draw_rectangle(m_pixmapMeaResult, m_gcMeaResult, TRUE, 0, 0, m_meaResultRect.w, m_meaResultRect.h);

    // BodyMark Area
	const int bdmk_width = BDMK_W * BDMK_MAX_SCALE;
	const int bdmk_height = BDMK_H * BDMK_MAX_SCALE;
	for(int i=0; i<4; i++)
	{
		if (m_pixmapBDMK[i])
			g_object_unref(m_pixmapBDMK[i]);
		m_pixmapBDMK[i] = gdk_pixmap_new(widget->window,
				bdmk_width,
				bdmk_height,
				-1);
		GdkGC *gc_bdmk = gdk_gc_new(m_pixmapBDMK[i]);
		gdk_gc_set_foreground(gc_bdmk, g_black);
		gdk_draw_rectangle(m_pixmapBDMK[i], gc_bdmk, TRUE, 0, 0, bdmk_width, bdmk_height);
		g_object_unref(gc_bdmk);
	}
}

void ImageArea::ImageAreaExpose(GtkWidget *widget, GdkEventExpose *event)
{
	unsigned char keyColor[3] = {0, 0, 0};

	// 重复包含头文件，待修改
	unsigned char histogram_status = DrawHistogram::GetInstance()->GetOnOff();
	if (histogram_status != 0) {
		DrawHistogram::GetInstance()->HistogramDraw(m_bitsImg);
	}

    bool traceStatus = ImgProcPw::GetInstance()->GetTraceStatus();
	unsigned char* pDst;
	unsigned char* pSrc0;
	unsigned char* pSrc1;
	int roiWidthSrc, roiHeightSrc, widthSrc0, heightSrc0, widthSrc1, heightSrc1;
	int widthDst = IMG_AREA_W;
	int heightDst = IMG_AREA_H;
	//	int rgbOrderNormal[3] = {0, 1, 2};
	int rgbOrderReversed[3] = {2, 1, 0};

	//Get GdkImage(about 5300~5400us) from drawable is faster than Gdkpixbuf(about 9700~9800us)
	//	GdkPixbuf *pixbufArea = gdk_pixbuf_get_from_drawable(NULL, m_pixmapArea, NULL, 0, 0, 0, 0, -1, -1);
	GdkImage *iA = gdk_drawable_get_image(m_pixmapArea, 0, 0, IMAGE_W, IMAGE_H); //why date order is BGR

	GdkGC *gc = gdk_gc_new(m_pixmapArea);
	gdk_gc_set_function(gc, GDK_XOR);

	//	PRINTF("m_inReadImg= %d, limit= %d\n", m_inReadImg, g_menuReview.GetLimit());
	if (!m_inReadImg) {
		if (ModeStatus::IsEFOVMode()) {
			if (ModeStatus::IsFreezeMode()) {
				if (ScanMode::GetInstance()->GetEFOVStatus() == ScanMode::REVIEW) {
					EFOVRECT<int> rect = ImgProc2D::GetInstance()->GetEFOVBoxRect();
					POINT leftup, leftdown, rightup, rightdown;
					leftup.x = rect.leftup.x; leftup.y = rect.leftup.y;
					leftdown.x = rect.leftdown.x; leftdown.y = rect.leftdown.y;
					rightup.x = rect.rightup.x; rightup.y = rect.rightup.y;
					rightdown.x = rect.rightdown.x; rightdown.y = rect.rightdown.y;
					ImageAreaDraw::GetInstance()->DrawEFOVViewBox(leftup, leftdown, rightup, rightdown);
				}

				memcpy(m_bitsMix, m_imagePara->imageData, IMG_AREA_W*IMG_AREA_H*IMG_BPP);

				// 844 * 560
				pDst = pSrc0 = m_bitsMix;
				pSrc1 = m_bitsEFOV;
				widthSrc0 = IMG_AREA_W;
				heightSrc0 = IMG_AREA_H;
				widthSrc1 = IMG_AREA_W;
				heightSrc1 = IMG_AREA_H;
				roiWidthSrc = IMG_AREA_W;
				roiHeightSrc = IMG_AREA_H;
				ImageMixC3R(pDst, pSrc0, pSrc1, widthDst, heightDst, widthSrc0, heightSrc0, widthSrc1, heightSrc1, roiWidthSrc, roiHeightSrc, keyColor);

				//2011.21.1
				if (ScanMode::GetInstance()->GetEFOVStatus() == ScanMode::REVIEW && Replay::GetInstance()->IsEFOVAutoReplay()) {
					Image::AutoTracePara para;
					Image image((unsigned int*)m_bitsMix, para);
					Replay::GetInstance()->AddOneImg(image);
				}

				// PixmapArea的叠加(pixmap area-> m_bitsMix)
				pDst = pSrc1 = m_bitsMix + (IMG_AREA_W*IMAGE_Y*IMG_BPP + IMAGE_X*IMG_BPP);
				widthSrc1 = IMG_AREA_W;
				heightSrc1 = IMG_AREA_H;
				pSrc0 = (unsigned char*)(iA->mem);
				widthSrc0 = IMAGE_W;
				heightSrc0 = IMAGE_H;
				roiWidthSrc = IMAGE_W;
				roiHeightSrc = IMAGE_H;
				ImageMixC3C4R(pDst, pSrc0, pSrc1, widthDst, heightDst, widthSrc0, heightSrc0, widthSrc1, heightSrc1, roiWidthSrc, roiHeightSrc, keyColor, rgbOrderReversed);

				gdk_draw_pixbuf(widget->window,
						widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
						m_mixPixbuf,
						0, 0,
						0, 0,
						IMG_AREA_W, IMG_AREA_H,
						GDK_RGB_DITHER_NORMAL, 0, 0);

			} else {            // 实时情况下
				if (ScanMode::GetInstance()->GetEFOVStatus() == ScanMode::CAPTURE) {
					EFOVRECT<int> rect = ImgProc2D::GetInstance()->GetEFOVBoxRect();
					POINT leftup, leftdown, rightup, rightdown;
					leftup.x = rect.leftup.x; leftup.y = rect.leftup.y;
					leftdown.x = rect.leftdown.x; leftdown.y = rect.leftdown.y;
					rightup.x = rect.rightup.x; rightup.y = rect.rightup.y;
					rightdown.x = rect.rightdown.x; rightdown.y = rect.rightdown.y;
					ImageAreaDraw::GetInstance()->DrawEFOVViewBox(leftup, leftdown, rightup, rightdown);
					ImageAreaDraw::GetInstance()->DrawEFOVScaleY();
					ImageAreaDraw::GetInstance()->DrawEFOVScaleX();

					int cur, total;
					float speed;
					ImgProc2D::GetInstance()->GetEFOVSpeed(cur, total, speed);
					DrawEFOVSpeedbar(cur, total, speed);
				}
				memcpy(m_bitsMix, m_imagePara->imageData, IMG_AREA_W*IMG_AREA_H*IMG_BPP);

				// 844 * 560
				pDst = pSrc0 = m_bitsMix;
				pSrc1 = m_bitsEFOV;
				widthSrc0 = IMG_AREA_W;
				heightSrc0 = IMG_AREA_H;
				widthSrc1 = IMG_AREA_W;
				heightSrc1 = IMG_AREA_H;
				roiWidthSrc = IMG_AREA_W;
				roiHeightSrc = IMG_AREA_H;
				ImageMixC3R(pDst, pSrc0, pSrc1, widthDst, heightDst, widthSrc0, heightSrc0, widthSrc1, heightSrc1, roiWidthSrc, roiHeightSrc, keyColor);
				gdk_draw_pixbuf(widget->window,
						widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
						m_mixPixbuf,
						0, 0,
						0, 0,
						IMG_AREA_W, IMG_AREA_H,
						GDK_RGB_DITHER_NORMAL, 0, 0);

				gdk_draw_drawable(widget->window,
						gc,
						m_pixmapReplayBar,
						0, 0,
						EFOV_SPEEDBAR_X, EFOV_SPEEDBAR_Y,
						REPLAY_BAR_W, REPLAY_BAR_H);
			}
		} else {
			if (ModeStatus::IsPureFreezeMode() || ModeStatus::IsFreezeMode() || ModeStatus::IsAutoReplayMode()) {
				//
				if (ModeStatus::IsPureFreezeMode()) {
					//printf("%s: IsPureFreezeMode\n", __FUNCTION__);
					// m_bitsReplayMix = para
					memcpy(m_bitsReplayMix, m_imagePara->imageData, IMG_AREA_W*IMG_AREA_H*IMG_BPP);

					// 用回放区最后一帧数据(含字符数据)显示
					// replay data layer(replay -> m_bitsReplayMix)
					pDst = pSrc1 = m_bitsReplayMix + (IMG_AREA_W*IMAGE_Y*IMG_BPP + IMAGE_X*IMG_BPP);
					pSrc0 =  m_bitsLastReplay + (IMG_AREA_W*IMAGE_Y*IMG_BPP + IMAGE_X*IMG_BPP);
					widthSrc1 = IMG_AREA_W;
					heightSrc1 = IMG_AREA_H;
					widthSrc0 = IMG_AREA_W;
					heightSrc0 = IMG_AREA_H;
					roiWidthSrc = IMAGE_W;
					roiHeightSrc = IMAGE_H;
					ImageMixC3R(pDst, pSrc0, pSrc1, widthDst, heightDst, widthSrc0, heightSrc0, widthSrc1, heightSrc1, roiWidthSrc, roiHeightSrc, keyColor);
#if 0
					// 用DSC传来的最后一帧数据(无字符数据)显示
					// replay data layer(replay -> m_bitsReplayMix)
					pDst = pSrc1 = m_bitsReplayMix + (IMG_AREA_W*IMAGE_Y*IMG_BPP + IMAGE_X*IMG_BPP);
					pSrc0 = m_bitsImg;
					widthSrc1 = IMG_AREA_W;
					heightSrc1 = IMG_AREA_H;
					widthSrc0 = IMAGE_W;
					heightSrc0 = IMAGE_H;
					roiWidthSrc = IMAGE_W;
					roiHeightSrc = IMAGE_H;
					ImageMixC3R(pDst, pSrc0, pSrc1, widthDst, heightDst, widthSrc0, heightSrc0, widthSrc1, heightSrc1, roiWidthSrc, roiHeightSrc, keyColor);
#endif

					if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::LOCAL_ZOOM) {
						// imageSymbol叠加(symbol -> m_bitsReplayMix)
						unsigned char* symbolData = (unsigned char *)m_imageSymbol->imageData;
						pDst = pSrc1 = m_bitsReplayMix + (IMG_AREA_W*IMAGE_Y*IMG_BPP + IMAGE_X*IMG_BPP);
						widthSrc1 = IMG_AREA_W;
						heightSrc1 = IMG_AREA_H;
						widthSrc0 = IMAGE_W;
						heightSrc0 = IMAGE_H;
						if (ModeStatus::IsColorVS2DMode()) {
							pSrc0 = symbolData;
							roiWidthSrc = IMAGE_W;
							roiHeightSrc = IMAGE_H;
						}
						else {
							pSrc0 = symbolData + ((m_symbolArea.y-IMAGE_Y)*IMAGE_W + (m_symbolArea.x-IMAGE_X))*IMG_BPP;
							roiWidthSrc = m_symbolArea.w;
							roiHeightSrc = m_symbolArea.h;
						}
						ImageMixC3R(pDst, pSrc0, pSrc1, widthDst, heightDst, widthSrc0, heightSrc0, widthSrc1, heightSrc1, roiWidthSrc, roiHeightSrc, keyColor);
					}
				}
				else {
					//printf("%s: IsFreezeMode or replay!\n", __FUNCTION__);
					memcpy(m_bitsReplayMix, m_bitsLastReplay, IMG_AREA_W*IMG_AREA_H*IMG_BPP);
				}

				// PixmapArea的叠加(pixmap area -> m_bitsReplayMix)
				pDst = pSrc1 = m_bitsReplayMix + (IMG_AREA_W*IMAGE_Y*IMG_BPP + IMAGE_X*IMG_BPP);
				widthSrc1 = IMG_AREA_W;
				heightSrc1 = IMG_AREA_H;
				pSrc0 = (unsigned char*)(iA->mem);
				widthSrc0 = IMAGE_W;
				heightSrc0 = IMAGE_H;
				roiWidthSrc = IMAGE_W;
				roiHeightSrc = IMAGE_H;
				ImageMixC3C4R(pDst, pSrc0, pSrc1, widthDst, heightDst, widthSrc0, heightSrc0, widthSrc1, heightSrc1, roiWidthSrc, roiHeightSrc, keyColor, rgbOrderReversed);

#if 1
				//ImageIMT lihuamei 2012.07.06
				unsigned char* ImtData = (unsigned char *)m_imageIMT->imageData;
				pDst = pSrc1 = m_bitsReplayMix + (IMG_AREA_W*IMAGE_Y*3 + IMAGE_X*3);
				pSrc0 = ImtData;// + ((IMAGE_Y-IMAGE_Y)*IMAGE_W + (IMAGE_X-IMAGE_X))*3
				widthSrc1 = IMG_AREA_W;
				heightSrc1 = IMG_AREA_H;
				widthSrc0 = IMAGE_W;
				roiWidthSrc = IMAGE_W;
				roiHeightSrc = IMAGE_H;
				ImageMixC3R(pDst, pSrc0, pSrc1, widthDst, heightDst, widthSrc0, heightSrc0, widthSrc1, heightSrc1, roiWidthSrc, roiHeightSrc, keyColor);
#endif
				//
				gdk_draw_pixbuf(widget->window,
						widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
						// m_replayPixbuf,
						m_IMTPixbuf,
						0, 0,
						0, 0,
						IMG_AREA_W, IMG_AREA_H,
						GDK_RGB_DITHER_NORMAL, 0, 0);


				gdk_draw_pixbuf(widget->window,
						widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
						m_mixReplayPixbuf,
						0, 0,
						0, 0,
						IMG_AREA_W, IMG_AREA_H,
						GDK_RGB_DITHER_NORMAL, 0, 0);
				if (ModeStatus::IsFreezeMode()) {
					gdk_draw_drawable(widget->window,
							widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
							m_pixmapFps,
							0, 0,
							FPS_POS_X, FPS_POS_Y,
							FPS_POS_W, FPS_POS_H);
				}
				gdk_draw_drawable(widget->window,
						widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
						m_pixmapReplayBar,
						0, 0,
						REPLAY_BAR_X, REPLAY_BAR_Y,
						REPLAY_BAR_W, REPLAY_BAR_H);

				if (traceStatus) {
					const Image::AutoTracePara *para = Replay::GetInstance()->GetCurrentTraceData();
					ImageAreaDraw::GetInstance()->DrawPwTraceAuto(ImgProcPw::GetInstance()->GetAutoCalc(), para);
				} else {
					//ClearTrace();
				}

			} else {		// 实时状况下
				Image::AutoTracePara* para;
				if (ImgProcPw::GetInstance()->GetDscTraceStatus()) {
                    para = ImageAreaDraw::GetInstance()->NewPwTracePara();
                    if (traceStatus) {
						ImageAreaDraw::GetInstance()->DrawPwTraceAuto(ImgProcPw::GetInstance()->GetAutoCalc(), para);
					} else {
						//ClearTrace();
					}
				}

				// m_bitsMix = para
				memcpy(m_bitsMix, m_imagePara->imageData, IMG_AREA_W*IMG_AREA_H*IMG_BPP);

				// data layer(image -> m_bitsMix)
				pDst = pSrc0 = m_bitsMix + (IMG_AREA_W*IMAGE_Y*IMG_BPP + IMAGE_X*IMG_BPP);
				pSrc1 = m_bitsImg;
				widthSrc0 = IMG_AREA_W;
				heightSrc0 = IMG_AREA_H;
				widthSrc1 = IMAGE_W;
				heightSrc1 = IMAGE_H;
				roiWidthSrc = IMAGE_W;
				roiHeightSrc = IMAGE_H;
				ImageMixC3R(pDst, pSrc0, pSrc1, widthDst, heightDst, widthSrc0, heightSrc0, widthSrc1, heightSrc1, roiWidthSrc, roiHeightSrc, keyColor);

				// imageSymbol(symbol -> m_bitsMix)
				unsigned char* symbolData = (unsigned char *)m_imageSymbol->imageData;
				pDst = pSrc1 = m_bitsMix + (IMG_AREA_W*m_symbolArea.y*IMG_BPP + m_symbolArea.x*IMG_BPP);
				pSrc0 = symbolData + ((m_symbolArea.y-IMAGE_Y)*IMAGE_W + (m_symbolArea.x-IMAGE_X))*IMG_BPP;
				widthSrc1 = IMG_AREA_W;
				heightSrc1 = IMG_AREA_H;
				widthSrc0 = IMAGE_W;
				heightSrc0 = IMAGE_H;
				roiWidthSrc = m_symbolArea.w;
				roiHeightSrc = m_symbolArea.h;
				ImageMixC3R(pDst, pSrc0, pSrc1, widthDst, heightDst, widthSrc0, heightSrc0, widthSrc1, heightSrc1, roiWidthSrc, roiHeightSrc, keyColor);

				// imageSpectra叠加
				if (ModeStatus::IsMImgMode() || ModeStatus::IsAnatomicMode() || ModeStatus::IsPWImgMode() || ModeStatus::IsCWImgMode()) {
					unsigned char* spectraData = (unsigned char *)m_imageSpectra->imageData;
					pDst = pSrc1 = m_bitsMix + (IMG_AREA_W*m_spectraArea.y*IMG_BPP + m_spectraArea.x*IMG_BPP);
					pSrc0 = spectraData + ((m_spectraArea.y-IMAGE_Y)*IMAGE_W + (m_spectraArea.x-IMAGE_X))*IMG_BPP;
					widthSrc1 = IMG_AREA_W;
					heightSrc1 = IMG_AREA_H;
					widthSrc0 = IMAGE_W;
					heightSrc0 = IMAGE_H;
					roiWidthSrc = m_spectraArea.w;
					roiHeightSrc = m_spectraArea.h;
					ImageMixC3R(pDst, pSrc0, pSrc1, widthDst, heightDst, widthSrc0, heightSrc0, widthSrc1, heightSrc1, roiWidthSrc, roiHeightSrc, keyColor);
				}

				// preBox叠加(box -> m_bitsMix)
				if (ModeStatus::IsColorMode()) {
					unsigned char* preBoxData = (unsigned char *)m_imagePreBox->imageData;
					pDst = pSrc1 = m_bitsMix + (IMG_AREA_W*IMAGE_Y*IMG_BPP + IMAGE_X*IMG_BPP);
					widthSrc1 = IMG_AREA_W;
					heightSrc1 = IMG_AREA_H;
					pSrc0 = preBoxData;
					widthSrc0 = IMAGE_W;
					heightSrc0 = IMAGE_H;
					roiWidthSrc = IMAGE_W;
					roiHeightSrc = IMAGE_H;
					ImageMixC3R(pDst, pSrc0, pSrc1, widthDst, heightDst, widthSrc0, heightSrc0, widthSrc1, heightSrc1, roiWidthSrc, roiHeightSrc, keyColor);
				}

#if 1

				// draw IMT when unfreeze
				if (CDrawIMT::GetInstance()->GetIMTRealStatus())
				{
					//CalcTime ct;
					//ct.Begin(); //计时开始
					CDrawIMT::GetInstance()->ClearInAdventIMT();
					if (m_TpbitsIMT == NULL)
					{}
					else
						CDrawIMT::GetInstance()->UpdateIMT();

					//ct.End(); //计时结束，并且打印所用时间
					memcpy(m_TpbitsIMT, m_bitsImg, IMAGE_W*IMAGE_H*IMG_BPP);
					//memcpy(m_TpbitsIMT, m_imagePara->imageData, IMAGE_W*IMAGE_H*IMG_BPP);

				}
				else
				{}

				//ImageIMT
				unsigned char* ImtData = (unsigned char *)m_imageIMT->imageData;
				pDst = pSrc1 = m_bitsMix + (IMG_AREA_W*IMAGE_Y*IMG_BPP + IMAGE_X*IMG_BPP);
				pSrc0 = ImtData;// + ((IMAGE_Y-IMAGE_Y)*IMAGE_W + (IMAGE_X-IMAGE_X))*3;
				widthSrc1 = IMG_AREA_W;
				heightSrc1 = IMG_AREA_H;
				widthSrc0 = IMAGE_W;
				heightSrc0 = IMAGE_H;
				roiWidthSrc = IMAGE_W;
				roiHeightSrc = IMAGE_H;
				ImageMixC3R(pDst, pSrc0, pSrc1, widthDst, heightDst, widthSrc0, heightSrc0, widthSrc1, heightSrc1, roiWidthSrc, roiHeightSrc, keyColor);
#endif

				// 存储叠加后的数据到回放
				if (g_flagImageData) {
					if (m_cineRemoveImg == 0) {
						memcpy(m_bitsLastReplay, m_bitsMix, IMG_AREA_W*IMG_AREA_H*IMG_BPP);
						Image::AutoTracePara para;
						if (ImgProcPw::GetInstance()->GetDscTraceStatus())
							ImageAreaDraw::GetInstance()->GetPwTracePara(para);
						Image image((unsigned int*)m_bitsLastReplay, para);
						Replay::GetInstance()->AddOneImg(image);
						g_flagImageData = false;
					} else if (m_cineRemoveImg < 0) {
						;
					} else if (m_cineRemoveImg > 0) {
						m_cineRemoveImg--;
					}
				}

				// PixmapArea的叠加(pixmap area-> m_bitsMix)
				pDst = pSrc1 = m_bitsMix + (IMG_AREA_W*IMAGE_Y*IMG_BPP + IMAGE_X*IMG_BPP);
				widthSrc1 = IMG_AREA_W;
				heightSrc1 = IMG_AREA_H;
				pSrc0 = (unsigned char*)(iA->mem);
				widthSrc0 = IMAGE_W;
				heightSrc0 = IMAGE_H;
				roiWidthSrc = IMAGE_W;
				roiHeightSrc = IMAGE_H;
				ImageMixC3C4R(pDst, pSrc0, pSrc1, widthDst, heightDst, widthSrc0, heightSrc0, widthSrc1, heightSrc1, roiWidthSrc, roiHeightSrc, keyColor, rgbOrderReversed);

				// 绘制叠加完成后的数据
				gdk_draw_pixbuf(widget->window,
						widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
						m_mixPixbuf,
						0, 0,
						0, 0,
						IMG_AREA_W, IMG_AREA_H,
						GDK_RGB_DITHER_NORMAL, 0, 0);
			}
		}
	} else {
		if(g_menuReview.GetLimit() != 2 ) {
			if(m_frmPixbuf) {
				//	printf("%s-%s: limit != 2\n", __FILE__, __FUNCTION__);
				memcpy(m_bitsMix, gdk_pixbuf_get_pixels(m_frmPixbuf), IMG_AREA_W*IMG_AREA_H*IMG_BPP);

				// PixmapArea的叠加(pixmap area-> m_bitsMix)
				pDst = pSrc1 = m_bitsMix + (IMG_AREA_W*IMAGE_Y*IMG_BPP + IMAGE_X*IMG_BPP);
				widthSrc1 = IMG_AREA_W;
				heightSrc1 = IMG_AREA_H;
				pSrc0 = (unsigned char*)(iA->mem);
				widthSrc0 = IMAGE_W;
				heightSrc0 = IMAGE_H;
				roiWidthSrc = IMAGE_W;
				roiHeightSrc = IMAGE_H;
				ImageMixC3C4R(pDst, pSrc0, pSrc1, widthDst, heightDst, widthSrc0, heightSrc0, widthSrc1, heightSrc1, roiWidthSrc, roiHeightSrc, keyColor, rgbOrderReversed);

				gdk_draw_pixbuf(widget->window,
						widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
						m_mixPixbuf,
						//m_frmPixbuf,
						0, 0, 0, 0,
						IMG_AREA_W, IMG_AREA_H,//-1, -1,
						GDK_RGB_DITHER_NORMAL, 0, 0);
			}
		}
		else {
			//	printf("%s-%s: limit = 2\n", __FILE__, __FUNCTION__);
			//printf("pixbuf width = %d, height = %d\n", gdk_pixbuf_get_width(m_snapPixbufBak), gdk_pixbuf_get_height(m_snapPixbufBak));
			memcpy(m_bitsMix, gdk_pixbuf_get_pixels(m_snapPixbufBak) + IMG_AREA_W*IMG_AREA_Y*IMG_BPP, IMG_AREA_W*IMG_AREA_H*IMG_BPP);

			// PixmapArea的叠加(pixmap area-> m_bitsMix)
			pDst = pSrc1 = m_bitsMix + (IMG_AREA_W*IMAGE_Y*IMG_BPP + IMAGE_X*IMG_BPP);
			widthSrc1 = IMG_AREA_W;
			heightSrc1 = IMG_AREA_H;
			pSrc0 = (unsigned char*)(iA->mem);
			widthSrc0 = IMAGE_W;
			heightSrc0 = IMAGE_H;
			roiWidthSrc = IMAGE_W;
			roiHeightSrc = IMAGE_H;
			ImageMixC3C4R(pDst, pSrc0, pSrc1, widthDst, heightDst, widthSrc0, heightSrc0, widthSrc1, heightSrc1, roiWidthSrc, roiHeightSrc, keyColor, rgbOrderReversed);

			gdk_draw_pixbuf(widget->window,
					widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
					m_mixPixbuf,
					//m_frmPixbuf,
					0, 0, 0, 0,
					IMG_AREA_W, IMG_AREA_H,//-1, -1,
					GDK_RGB_DITHER_NORMAL, 0, 0);
		}
	}

	if (histogram_status != 0) {
		gdk_draw_drawable(widget->window,
				gc,
				m_pixmapHistogram,
				0, 0,
				kHistogramX, kHistogramY,
				kHistogramWidth, kHistogramHeight);
	}

#if 0
	// 绘制PixmapArea
	gdk_draw_drawable(widget->window,
			gc,
			m_pixmapArea,
			0, 0,
			IMAGE_X, IMAGE_Y,
			IMAGE_W, IMAGE_H);
#endif

	if (m_inBDMK) {
		gdk_gc_set_function(gc, GDK_XOR);
		for(guint i=0; i<4; i++)
		{
			if(m_pixmapBDMK[i])
				gdk_draw_drawable(widget->window,
						gc,
						m_pixmapBDMK[i],
						0, 0,
						m_bdmkPos[i].x, m_bdmkPos[i].y,
						m_bdmkWidth[i], m_bdmkHeight[i]);
		}
	}

	//    if (m_drawPwTrace) {
	gdk_gc_set_function(gc, GDK_XOR);
	gdk_draw_drawable(widget->window,
			gc,
			m_pixmapPwTrace,
			0, 0,
			IMAGE_X, IMAGE_Y,
			IMAGE_W, IMAGE_H);
	//   }

	// 绘制测量结果
	gdk_gc_set_function(gc, GDK_COPY);
	vector<MeasurePos>::iterator iter;
	for (iter = m_meaResultPos.begin(); iter != m_meaResultPos.end(); ++iter) {
        gdk_draw_drawable(widget->window,
                gc,
                m_pixmapMeaResult,
                0, iter->posY,
                m_meaResultRect.x, m_meaResultRect.y + iter->posY,
                iter->width, (iter->lines)*m_meaResultLineH);
    }
	g_object_unref(iA);
	g_object_unref(gc);
}

void ImageArea::ClearScreenUnFreeze(bool update)
{
	GdkGC *gc = gdk_gc_new(m_pixmapArea);
	gdk_gc_set_foreground(gc, g_black);
	//gdk_draw_rectangle(m_pixmapArea, gc, TRUE, 0, 0, IMAGE_W, IMAGE_H);
	//gdk_draw_rectangle(m_pixmapPwTrace, gc, TRUE, 0, 0, IMAGE_W, IMAGE_H);
	//gdk_draw_rectangle(m_pixmapMeaResult, gc, TRUE, 0, 0, m_meaResultRect.w, m_meaResultRect.h);
	if(update)
		UpdateImgArea();
	g_object_unref(gc);
}

void ImageArea::ClearScreen(bool update)
{
	GdkGC *gc = gdk_gc_new(m_pixmapArea);
	gdk_gc_set_foreground(gc, g_black);
	gdk_draw_rectangle(m_pixmapArea, gc, TRUE, 0, 0, IMAGE_W, IMAGE_H);
	gdk_draw_rectangle(m_pixmapPwTrace, gc, TRUE, 0, 0, IMAGE_W, IMAGE_H);
	gdk_draw_rectangle(m_pixmapMeaResult, gc, TRUE, 0, 0, m_meaResultRect.w, m_meaResultRect.h);
    POINT ps, ed;
    ps.x = ed.x = 0;
    ps.y = ed.y = IMAGE_H/2;
    ImageAreaDraw::GetInstance()->SetPsEdValue(ps, ed, 1);
	if(update)
		UpdateImgArea();
	g_object_unref(gc);
}

void ImageArea::ClearArea(bool update)
{
	cvZero(m_imageSymbol);
	cvZero(m_imageSpectra);
	cvZero(m_imagePara);
	if (!update)
		cvZero(m_imagePreBox);

	ClearScreen();
}

void ImageArea::SetReadImg(bool status)
{
    m_inReadImg = status;

    if (status)
        DelTimeOutFps();
    else
        AddTimeOutFps();
}

void ImageArea::ResetReadImg(void)
{
	if (!m_snapPixbufBak)
		return;

	int width = gdk_pixbuf_get_width(m_snapPixbufBak);
	int height = gdk_pixbuf_get_height(m_snapPixbufBak);

	gdk_draw_pixbuf(m_imageDA->window,
			m_imageDA->style->fg_gc[GTK_WIDGET_STATE(m_imageDA)],
			m_snapPixbufBak,
			0, IMG_AREA_Y, 0, 0,
			width, height-IMG_AREA_Y,
			GDK_RGB_DITHER_NORMAL, 0, 0);
}

void ImageArea::GetImgPtr(unsigned char ** image)
{
	*image = m_bitsImg;
}

GdkPixbuf* ImageArea::GetDrawingAreaImage()
{
	return gdk_pixbuf_get_from_drawable(NULL,
			m_imageDA->window,
			gdk_colormap_get_system(),
			IMAGE_X, IMAGE_Y,
			0, 0,
			IMAGE_W, IMAGE_H);
}

void ImageArea::PutDrawingArea(GdkPixbuf *pixbuf)
{
}

//only used for save snap
void ImageArea::GetFrmData(guchar **pBits, int *nWidth, int *nHeight)
{
    int size = IMG_AREA_W * IMG_AREA_H * 3;
    guchar *data = (unsigned char*)malloc(size);
    GdkPixbuf *pixbuf = NULL;
    if(!pBits || !nWidth || !nHeight || !data)
    {
        perror("----getfrmdata error: \n");
        return;
    }
	if (!m_inReadImg) {
      		if (ModeStatus::IsEFOVMode()) {
			pixbuf = m_mixPixbuf;
		} else {
            pixbuf = m_lastReplayPixbuf;
#if 0
			if (ModeStatus::IsFreezeMode() || ModeStatus::IsAutoReplayMode()) {
				pixbuf = m_mixReplayPixbuf;
			} else {
				pixbuf = m_mixPixbuf;
			}
#endif
		}
	} else {
        pixbuf = m_frmPixbuf;
	}

    if (pixbuf)
    {
        memcpy(data, gdk_pixbuf_get_pixels(pixbuf), size);
    }
    else
    {
        perror("---pixbuf error: \n");
        return;
    }
    *pBits = data;
    *nWidth = gdk_pixbuf_get_width(pixbuf);
    *nHeight = gdk_pixbuf_get_height(pixbuf);
}

//only used for read snap
void ImageArea::LoadFrm(void)
{
    PRINTF("%s: load frm file %s\n", __FUNCTION__, g_menuReview.GetReadFrm());
	ImgMan::ImgItem item;
	if (ImgMan::GetInstance()->ReadSnap(g_menuReview.GetReadFrm(), &item) != 0) {
		PRINTF("%s: Error with ReadSnap!\n", __FUNCTION__);
		return;
	}

	if(m_frmPixbuf)
		g_object_unref(m_frmPixbuf);
	m_frmPixbuf = gdk_pixbuf_copy(item.pixbuf);

	g_object_unref(item.pixbuf);
}

void ImageArea::GetSnapData(guchar **pBits, int *nWidth, int *nHeight)
{
    GdkPixbuf *pixbuf = gdk_pixbuf_get_from_drawable(NULL,
            ViewMain::GetInstance()->GetMainWindow()->window,
            gdk_colormap_get_system(),
            IMG_AREA_X, 0,
            0, 0,
            IMG_AREA_W, IMG_AREA_H + TOP_AREA_H + GAP_AREA_H);

    int size = IMG_AREA_W * (IMG_AREA_H+TOP_AREA_H+GAP_AREA_H) * 3;
    guchar *data = (unsigned char*)malloc(size);
    if (pixbuf)
        memcpy(data, gdk_pixbuf_get_pixels(pixbuf), size);
    else
        PRINTF("---pixbuf is null\n");

	*pBits = data;

	*nWidth = gdk_pixbuf_get_width(pixbuf);
	*nHeight = gdk_pixbuf_get_height(pixbuf);
	g_object_unref(pixbuf);
}

void ImageArea::DrawSnap(GdkPixbuf *pixbuf, int src_x, int src_y, int width, int height)
{
	if (m_snapPixbufBak) {
		g_object_unref(m_snapPixbufBak);
		m_snapPixbufBak = 0;
	}
	m_snapPixbufBak = gdk_pixbuf_copy(pixbuf);

	gdk_draw_pixbuf(m_imageDA->window,
			m_imageDA->style->fg_gc[GTK_WIDGET_STATE(m_imageDA)],
			pixbuf,
			src_x, src_y, 0, 0,
			width, height,
			GDK_RGB_DITHER_NORMAL, 0, 0);
}

GdkGC* ImageArea::GetPixmapDC(GdkColor* color, GdkFunction mode)
{
	GdkGC *gc = gdk_gc_new(m_pixmapArea);
	gdk_gc_set_foreground(gc, color);
	gdk_gc_set_function(gc, mode);
	return gc;
}

GdkGC* ImageArea::NewHistogramDC(GdkColor * color, GdkFunction mode)
{
	GdkGC *gc = gdk_gc_new(m_pixmapHistogram);
	gdk_gc_set_foreground(gc, color);
	gdk_gc_set_function(gc, mode);
	return gc;
}

void ImageArea::DrawHistogram(GdkGC *gc, int x1, int y1, int x2, int y2)
{
	gdk_draw_line(m_pixmapHistogram, gc, x1, y1, x2, y2);
}

void ImageArea::DrawPixmapLine(GdkGC *gc, int x1, int y1, int x2, int y2)
{
	gdk_draw_line(m_pixmapArea, gc, x1, y1, x2, y2);
}

void ImageArea::DrawLine(DrawAttr &attr, const GdkColor* const color, int x1, int y1, int x2, int y2, bool update)
{
  	switch (attr.area) {
		case IMT:
			if (attr.mode.cvDraw)
				cvLine(m_imageIMT, cvPoint(x1, y1), cvPoint(x2, y2), CV_RGB(color->blue>>8, color->green>>8, color->red>>8));
			else
				cvLine(m_imageIMT, cvPoint(x1, y1), cvPoint(x2, y2), CV_RGB(0, 0, 0));
			break;
		case SYMBOL:
			if (attr.mode.cvDraw)
				cvLine(m_imageSymbol, cvPoint(x1, y1), cvPoint(x2, y2), CV_RGB(color->blue>>8, color->green>>8, color->red>>8));
			else
				cvLine(m_imageSymbol, cvPoint(x1, y1), cvPoint(x2, y2), CV_RGB(0, 0, 0));
			break;
		case SPECTRA:
			if (attr.mode.cvDraw)
				cvLine(m_imageSpectra, cvPoint(x1, y1), cvPoint(x2, y2), CV_RGB(color->blue>>8, color->green>>8, color->red>>8));
			else
				cvLine(m_imageSpectra, cvPoint(x1, y1), cvPoint(x2, y2), CV_RGB(0, 0, 0));
			break;
		case PARA:
			if (attr.mode.cvDraw)
				cvLine(m_imagePara, cvPoint(x1, y1), cvPoint(x2, y2), CV_RGB(color->blue>>8, color->green>>8, color->red>>8));
			else
				cvLine(m_imagePara, cvPoint(x1, y1), cvPoint(x2, y2), CV_RGB(0, 0, 0));
			break;
		case PIXMAP:
			{
                GdkGC *gc = gdk_gc_new(m_pixmapArea);
				gdk_gc_set_foreground(gc, color);
				gdk_gc_set_function(gc, attr.mode.gdkMode);

				gdk_draw_line(m_pixmapArea, gc, x1, y1, x2, y2);
				g_object_unref(gc);
			}
			break;
		default:
			break;
	}
	if (update)
		UpdateImgArea();
}

void ImageArea::DrawPoint(DrawAttr &attr, const GdkColor* const color, int x, int y, bool update)
{
	switch (attr.area) {
		case SYMBOL:
			if (attr.mode.cvDraw)
				cvRectangle(m_imageSymbol, cvPoint(x, y), cvPoint(x+1, y+1), CV_RGB(color->blue>>8, color->green>>8, color->red>>8), CV_FILLED);
			else
				cvRectangle(m_imageSymbol, cvPoint(x, y), cvPoint(x+1, y+1), CV_RGB(0, 0, 0), CV_FILLED);
			break;
		case SPECTRA:
			if (attr.mode.cvDraw)
				cvRectangle(m_imageSpectra, cvPoint(x, y), cvPoint(x+1, y+1), CV_RGB(color->blue>>8, color->green>>8, color->red>>8), CV_FILLED);
			else
				cvRectangle(m_imageSpectra, cvPoint(x, y), cvPoint(x+1, y+1), CV_RGB(0, 0, 0), CV_FILLED);
			break;
		case PARA:
			if (attr.mode.cvDraw)
				cvRectangle(m_imagePara, cvPoint(x, y), cvPoint(x+1, y+1), CV_RGB(color->blue>>8, color->green>>8, color->red>>8), CV_FILLED);
			else
				cvRectangle(m_imagePara, cvPoint(x, y), cvPoint(x+1, y+1), CV_RGB(0, 0, 0), CV_FILLED);
			break;
		case PIXMAP:
			{
				GdkGC *gc = gdk_gc_new(m_pixmapArea);
				gdk_gc_set_foreground(gc, color);
				gdk_gc_set_function(gc, attr.mode.gdkMode);
				gdk_draw_point(m_pixmapArea, gc, x, y);
				g_object_unref(gc);
			}
			break;
		default:
			break;
	}
	if (update)
		UpdateImgArea();
}

void ImageArea::DrawCircle(DrawAttr &attr, const GdkColor* const color, int x, int y, int radius, bool filled, bool update)
{
	switch (attr.area) {
		case SYMBOL:
			if (attr.mode.cvDraw) {
				if (filled)
					cvCircle(m_imageSymbol, cvPoint(x, y), radius, CV_RGB(color->blue>>8, color->green>>8, color->red>>8), CV_FILLED);
				else
					cvCircle(m_imageSymbol, cvPoint(x, y), radius, CV_RGB(color->blue>>8, color->green>>8, color->red>>8));
			} else {
				if (filled)
					cvCircle(m_imageSymbol, cvPoint(x, y), radius, CV_RGB(0, 0, 0), CV_FILLED);
				else
					cvCircle(m_imageSymbol, cvPoint(x, y), radius, CV_RGB(0, 0, 0));
			}
			break;
		case SPECTRA:
			if (attr.mode.cvDraw) {
				if (filled)
					cvCircle(m_imageSpectra, cvPoint(x, y), radius, CV_RGB(color->blue>>8, color->green>>8, color->red>>8), CV_FILLED);
				else
					cvCircle(m_imageSpectra, cvPoint(x, y), radius, CV_RGB(color->blue>>8, color->green>>8, color->red>>8));
			} else {
				if (filled)
					cvCircle(m_imageSpectra, cvPoint(x, y), radius, CV_RGB(0, 0, 0), CV_FILLED);
				else
					cvCircle(m_imageSpectra, cvPoint(x, y), radius, CV_RGB(0, 0, 0));
			}
			break;
		case PARA:
			if (attr.mode.cvDraw) {
				if (filled)
					cvCircle(m_imagePara, cvPoint(x, y), radius, CV_RGB(color->blue>>8, color->green>>8, color->red>>8), CV_FILLED);
				else
					cvCircle(m_imagePara, cvPoint(x, y), radius, CV_RGB(color->blue>>8, color->green>>8, color->red>>8));
			} else {
				if (filled)
					cvCircle(m_imagePara, cvPoint(x, y), radius, CV_RGB(0, 0, 0), CV_FILLED);
				else
					cvCircle(m_imagePara, cvPoint(x, y), radius, CV_RGB(0, 0, 0));
			}
			break;
		case PIXMAP:
			{
				GdkGC *gc = gdk_gc_new(m_pixmapArea);
				gdk_gc_set_foreground(gc, color);
				gdk_gc_set_function(gc, attr.mode.gdkMode);
				gdk_draw_arc(m_pixmapArea, gc, filled, x-radius, y-radius, radius*2, radius*2, 0, 360*64);
				g_object_unref(gc);
			}
			break;
		default:
			break;
	}
	if (update)
		UpdateImgArea();
}

void ImageArea::DrawArc(DrawAttr &attr, const GdkColor* const color, int x, int y, int width, int height, double angle1, double angle2, bool update)
{
	switch (attr.area) {
		case SYMBOL:
			if (attr.mode.cvDraw)
				cvEllipse(m_imageSymbol, cvPoint(x+width/2, y+height/2), cvSize(width/2, height/2), 0, angle1, angle2, CV_RGB(color->blue>>8, color->green>>8, color->red>>8));
			else
				cvEllipse(m_imageSymbol, cvPoint(x+width/2, y+height/2), cvSize(width/2, width/2), 0, angle1, angle2, CV_RGB(0, 0, 0));
			break;
		case SPECTRA:
			if (attr.mode.cvDraw)
				cvEllipse(m_imageSpectra, cvPoint(x+width/2, y+height/2), cvSize(width/2, height/2), 0, angle1, angle2, CV_RGB(color->blue>>8, color->green>>8, color->red>>8));
			else
				cvEllipse(m_imageSpectra, cvPoint(x+width/2, y+height/2), cvSize(width/2, width/2), 0, angle1, angle2, CV_RGB(0, 0, 0));
			break;
		case PARA:
			if (attr.mode.cvDraw)
				cvEllipse(m_imagePara, cvPoint(x+width/2, y+height/2), cvSize(width/2, height/2), 0, angle1, angle2, CV_RGB(color->blue>>8, color->green>>8, color->red>>8));
			else
				cvEllipse(m_imagePara, cvPoint(x+width/2, y+height/2), cvSize(width/2, width/2), 0, angle1, angle2, CV_RGB(0, 0, 0));
			break;
		case PIXMAP:
			{
				GdkGC *gc = gdk_gc_new(m_pixmapArea);
				gdk_gc_set_foreground(gc, color);
				gdk_gc_set_function(gc, attr.mode.gdkMode);
				gdk_draw_arc(m_pixmapArea, gc, false, x, y, width, height, angle1*64, angle2*64);
				g_object_unref(gc);
			}
			break;
		default:
			break;
	}
	if (update)
		UpdateImgArea();
}

void ImageArea::DrawDashArc(DrawAttr &attr, const GdkColor* const color, int x, int y, int width, int height, double angle1, double angle2, bool update, double step)
{
	if (width == 0)
		return;

	double a = (double)step * 360 / PI / width;
	double i = (double)0;
	int xPos = 0, yPos = 0;
	for (i = (double)0; i < (double)fabs(angle2-angle1); i+=a)
	{
		xPos = x + cos((angle1 + i) * PI / 180) * width / 2 + (double)width / 2 + 0.5;
		yPos = y - sin((angle1 + i) * PI / 180) * width / 2 + (double)height / 2 + 0.5;

		switch (attr.area) {
			case SYMBOL:
				if (attr.mode.cvDraw)
					cvRectangle(m_imageSymbol, cvPoint(xPos, yPos), cvPoint(xPos+1, yPos+1), CV_RGB(color->blue>>8, color->green>>8, color->red>>8), 1);
				else
					cvRectangle(m_imageSymbol, cvPoint(xPos, yPos), cvPoint(xPos+1, yPos+1), CV_RGB(0, 0, 0), 1);
				break;
			case SPECTRA:
				if (attr.mode.cvDraw)
					cvRectangle(m_imageSpectra, cvPoint(xPos, yPos), cvPoint(xPos+1, yPos+1), CV_RGB(color->blue>>8, color->green>>8, color->red>>8), 1);
				else
					cvRectangle(m_imageSpectra, cvPoint(xPos, yPos), cvPoint(xPos+1, yPos+1), CV_RGB(0, 0, 0), 1);
				break;
			case PARA:
				if (attr.mode.cvDraw)
					cvRectangle(m_imagePara, cvPoint(xPos, yPos), cvPoint(xPos+1, yPos+1), CV_RGB(color->blue>>8, color->green>>8, color->red>>8), 1);
				else
					cvRectangle(m_imagePara, cvPoint(xPos, yPos), cvPoint(xPos+1, yPos+1), CV_RGB(0, 0, 0), 1);
				break;
			case PIXMAP:
				{
					GdkGC *gc = gdk_gc_new(m_pixmapArea);
					gdk_gc_set_foreground(gc, color);
					gdk_gc_set_function(gc, attr.mode.gdkMode);
					gdk_draw_rectangle(m_pixmapArea, gc, true, xPos, yPos, 2, 2);
					g_object_unref(gc);
				}
				break;
			default:
				break;
		}
	}

	if (update)
		UpdateImgArea();
}

void ImageArea::DrawRectangle(DrawAttr &attr, const GdkColor* const color, int x, int y, int width, int height, bool filled, bool update)
{
	switch (attr.area) {
		case IMT:
			if (attr.mode.cvDraw) {
				if (filled)
					cvRectangle(m_imageIMT, cvPoint(x, y), cvPoint(x+width, y+height), CV_RGB(color->blue>>8, color->green>>8, color->red>>8), CV_FILLED);
				else
					cvRectangle(m_imageIMT, cvPoint(x, y), cvPoint(x+width, y+height), CV_RGB(color->blue>>8, color->green>>8, color->red>>8));
			} else {
				if (filled)
					cvRectangle(m_imageIMT, cvPoint(x, y), cvPoint(x+width, y+height), CV_RGB(0, 0, 0), CV_FILLED);
				else
					cvRectangle(m_imageIMT, cvPoint(x, y), cvPoint(x+width, y+height), CV_RGB(0, 0, 0));
			}
			break;
		case SYMBOL:
			if (attr.mode.cvDraw) {
				if (filled)
					cvRectangle(m_imageSymbol, cvPoint(x, y), cvPoint(x+width, y+height), CV_RGB(color->blue>>8, color->green>>8, color->red>>8), CV_FILLED);
				else
					cvRectangle(m_imageSymbol, cvPoint(x, y), cvPoint(x+width, y+height), CV_RGB(color->blue>>8, color->green>>8, color->red>>8));
			} else {
				if (filled)
					cvRectangle(m_imageSymbol, cvPoint(x, y), cvPoint(x+width, y+height), CV_RGB(0, 0, 0), CV_FILLED);
				else
					cvRectangle(m_imageSymbol, cvPoint(x, y), cvPoint(x+width, y+height), CV_RGB(0, 0, 0));
			}
			break;
		case SPECTRA:
			if (attr.mode.cvDraw) {
				if (filled)
					cvRectangle(m_imageSpectra, cvPoint(x, y), cvPoint(x+width, y+height), CV_RGB(color->blue>>8, color->green>>8, color->red>>8), CV_FILLED);
				else
					cvRectangle(m_imageSpectra, cvPoint(x, y), cvPoint(x+width, y+height), CV_RGB(color->blue>>8, color->green>>8, color->red>>8));
			} else {
				if (filled)
					cvRectangle(m_imageSpectra, cvPoint(x, y), cvPoint(x+width, y+height), CV_RGB(0, 0, 0), CV_FILLED);
				else
					cvRectangle(m_imageSpectra, cvPoint(x, y), cvPoint(x+width, y+height), CV_RGB(0, 0, 0));
			}
			break;
		case PARA:
			if (attr.mode.cvDraw) {
				if (filled)
					cvRectangle(m_imagePara, cvPoint(x, y), cvPoint(x+width, y+height), CV_RGB(color->blue>>8, color->green>>8, color->red>>8), CV_FILLED);
				else
					cvRectangle(m_imagePara, cvPoint(x, y), cvPoint(x+width, y+height), CV_RGB(color->blue>>8, color->green>>8, color->red>>8));
			} else {
				if (filled)
					cvRectangle(m_imagePara, cvPoint(x, y), cvPoint(x+width, y+height), CV_RGB(0, 0, 0), CV_FILLED);
				else
					cvRectangle(m_imagePara, cvPoint(x, y), cvPoint(x+width, y+height), CV_RGB(0, 0, 0));
			}
			break;
		case PIXMAP:
			{
				GdkGC *gc = gdk_gc_new(m_pixmapArea);
				gdk_gc_set_foreground(gc, color);
				gdk_gc_set_function(gc, attr.mode.gdkMode);
				gdk_draw_rectangle(m_pixmapArea, gc, filled, x, y, width, height);
				g_object_unref(gc);
			}
			break;
		default:
			break;
	}
	if (update)
		UpdateImgArea();
}

void ImageArea::DrawPolygon(DrawAttr &attr, const GdkColor* const color, GdkPoint *points, int nPoint, bool update)
{
	CvPoint pts[nPoint];

	for (int i = 0; i < nPoint; i++) {
		pts[i].x = points[i].x;
		pts[i].y = points[i].y;
	}

	switch (attr.area) {
		case SYMBOL:
			if (attr.mode.cvDraw)
				cvFillConvexPoly(m_imageSymbol, pts, nPoint, CV_RGB(color->blue>>8, color->green>>8, color->red>>8));
			else
				cvFillConvexPoly(m_imageSymbol, pts, nPoint, CV_RGB(0, 0, 0));
			break;
		case SPECTRA:
			if (attr.mode.cvDraw)
				cvFillConvexPoly(m_imageSpectra, pts, nPoint, CV_RGB(color->blue>>8, color->green>>8, color->red>>8));
			else
				cvFillConvexPoly(m_imageSpectra, pts, nPoint, CV_RGB(0, 0, 0));
			break;
		case PARA:
			if (attr.mode.cvDraw)
				cvFillConvexPoly(m_imagePara, pts, nPoint, CV_RGB(color->blue>>8, color->green>>8, color->red>>8));
			else
				cvFillConvexPoly(m_imagePara, pts, nPoint, CV_RGB(0, 0, 0));
			break;
		case PIXMAP:
			{
				GdkGC *gc = gdk_gc_new(m_pixmapArea);
				gdk_gc_set_foreground(gc, color);
				gdk_gc_set_function(gc, attr.mode.gdkMode);
				gdk_draw_polygon(m_pixmapArea, gc, TRUE, points, nPoint);
				g_object_unref(gc);
			}
			break;
		default:
			break;
	}
	if (update)
		UpdateImgArea();
}

void ImageArea::DrawDashLine(DrawAttr &attr, const GdkColor* const color, int x1, int y1, int x2, int y2, int size, bool update)
{
	GdkGC *gc = NULL;
	if (attr.area == PIXMAP) {
		gc = gdk_gc_new(m_pixmapArea);
		gdk_gc_set_foreground(gc, color);
		gdk_gc_set_function(gc, attr.mode.gdkMode);
	}

	int x, y, count;

	int dist_x = x2 - x1;
	int dist_y = y2 - y1;

	int length = (int)(sqrt(dist_x*dist_x + dist_y*dist_y));

	if (length > 10) {
		count = (length / 10) * 10;
		while (count >= 10) {
			x = x1 + (int)((float)dist_x * (float)count / (float)length);
			y = y1 + (int)((float)dist_y * (float)count / (float)length);

			switch (attr.area) {
				case SYMBOL:
					if (attr.mode.cvDraw)
						cvRectangle(m_imageSymbol, cvPoint(x, y), cvPoint(x+1, y+1), CV_RGB(color->blue>>8, color->green>>8, color->red>>8),size*2,CV_AA);
					else
						cvRectangle(m_imageSymbol, cvPoint(x, y), cvPoint(x+1, y+1), CV_RGB(0, 0, 0),size);
					break;
				case SPECTRA:
					if (attr.mode.cvDraw)
						cvRectangle(m_imageSpectra, cvPoint(x, y), cvPoint(x+1, y+1), CV_RGB(color->blue>>8, color->green>>8, color->red>>8));
					else
						cvRectangle(m_imageSpectra, cvPoint(x, y), cvPoint(x+1, y+1), CV_RGB(0, 0, 0));
					break;
				case PARA:
					if (attr.mode.cvDraw)
						cvRectangle(m_imagePara, cvPoint(x, y), cvPoint(x+1, y+1), CV_RGB(color->blue>>8, color->green>>8, color->red>>8));
					else
						cvRectangle(m_imagePara, cvPoint(x, y), cvPoint(x+1, y+1), CV_RGB(0, 0, 0));
					break;
				case PIXMAP:
					gdk_draw_point(m_pixmapArea, gc, x,   y);
					gdk_draw_point(m_pixmapArea, gc, x+1, y);
					gdk_draw_point(m_pixmapArea, gc, x,   y+1);
					gdk_draw_point(m_pixmapArea, gc, x+1, y+1);
					break;
				default:
					break;
			}
			count -= 10;
		}
	}
	if (attr.area == PIXMAP)
		g_object_unref(gc);
	if (update)
		UpdateImgArea();
}
void ImageArea::DrawDashLine(DrawAttr &attr, const GdkColor* const color, int x1, int y1, int x2, int y2, bool update, int distance)
{
	GdkGC *gc = NULL;
	if (attr.area == PIXMAP) {
		gc = gdk_gc_new(m_pixmapArea);
		gdk_gc_set_foreground(gc, color);
		gdk_gc_set_function(gc, attr.mode.gdkMode);
	}

	int x, y, count;

	int dist_x = x2 - x1;
	int dist_y = y2 - y1;

	int length = (int)(sqrt(dist_x*dist_x + dist_y*dist_y));

	if (length > distance) {
		count = (length / distance) * distance;
		while (count >= distance) {
			x = x1 + (int)((float)dist_x * (float)count / (float)length);
			y = y1 + (int)((float)dist_y * (float)count / (float)length);

			switch (attr.area) {
				case SYMBOL:
					if (attr.mode.cvDraw)
						cvRectangle(m_imageSymbol, cvPoint(x, y), cvPoint(x+1, y+1), CV_RGB(color->blue>>8, color->green>>8, color->red>>8));
					else
						cvRectangle(m_imageSymbol, cvPoint(x, y), cvPoint(x+1, y+1), CV_RGB(0, 0, 0));
					break;
				case SPECTRA:
					if (attr.mode.cvDraw)
						cvRectangle(m_imageSpectra, cvPoint(x, y), cvPoint(x+1, y+1), CV_RGB(color->blue>>8, color->green>>8, color->red>>8));
					else
						cvRectangle(m_imageSpectra, cvPoint(x, y), cvPoint(x+1, y+1), CV_RGB(0, 0, 0));
					break;
				case PARA:
					if (attr.mode.cvDraw)
						cvRectangle(m_imagePara, cvPoint(x, y), cvPoint(x+1, y+1), CV_RGB(color->blue>>8, color->green>>8, color->red>>8));
					else
						cvRectangle(m_imagePara, cvPoint(x, y), cvPoint(x+1, y+1), CV_RGB(0, 0, 0));
					break;
				case PIXMAP:
					gdk_draw_point(m_pixmapArea, gc, x,   y);
					gdk_draw_point(m_pixmapArea, gc, x+1, y);
					gdk_draw_point(m_pixmapArea, gc, x,   y+1);
					gdk_draw_point(m_pixmapArea, gc, x+1, y+1);
					break;
				default:
					break;
			}
			count -= distance;
		}
	}
	if (attr.area == PIXMAP)
		g_object_unref(gc);
	if (update)
		UpdateImgArea();
}

void ImageArea::ClearReplayBar(void)
{
	GdkGC *gc_replayBar = gdk_gc_new(m_pixmapReplayBar);
	gdk_gc_set_foreground(gc_replayBar, g_black);
	gdk_draw_rectangle(m_pixmapReplayBar, gc_replayBar, TRUE, 0, 0, REPLAY_BAR_W, REPLAY_BAR_H);
	g_object_unref(gc_replayBar);
}

void ImageArea::DrawReplayBar(int cur_img, int total_img, int left, int right)
{
	ClearReplayBar();

	const int bar_w = 300;
	const int bar_h = 12;
	const int x = 0;
	const int y = 0;

	int slip_pos = (int)(((float)(cur_img)/(float)total_img) * bar_w);
	int slip_left = (int)(((float)(left)/(float)total_img) * bar_w);
	int slip_right = (int)(((float)(right+1)/(float)total_img) * bar_w);

	GdkGC *gc_replayBar = gdk_gc_new(m_pixmapReplayBar);

	gdk_gc_set_foreground(gc_replayBar, g_white);
	gdk_draw_rectangle(m_pixmapReplayBar, gc_replayBar, FALSE, x, y, bar_w, bar_h);
	if (right != 0) {
		gdk_gc_set_foreground(gc_replayBar, g_green);
		gdk_draw_rectangle(m_pixmapReplayBar, gc_replayBar, FALSE, x+slip_left, y, abs(slip_right-slip_left), bar_h);
	}

	gdk_gc_set_foreground(gc_replayBar, g_white);
	gdk_draw_line(m_pixmapReplayBar, gc_replayBar, x+slip_pos, y, x+slip_pos, y+bar_h);

	GdkPoint init_point[3] = {{0, 0}, {-5, 6}, {5, 6}};
	GdkPoint points[3];
	for (int i=0; i<3; i++) {
		points[i].x = init_point[i].x + x + slip_pos;
		points[i].y = init_point[i].y + y + bar_h;
	}
	gdk_draw_polygon(m_pixmapReplayBar, gc_replayBar, TRUE, points, 3);

	char buf[20];
	sprintf(buf, "%3d/%3d", cur_img, total_img);
	PangoLayout *layout = gtk_widget_create_pango_layout(m_imageDA, buf);
	pango_layout_set_font_description(layout, m_baseFont);
#if 1
	pango_layout_set_text(layout, buf, -1);
	gdk_draw_layout(m_pixmapReplayBar, gc_replayBar, x+bar_w+5, y-2, layout);
#else
	pango_layout_set_text(m_layout, buf, -1);
	gdk_draw_layout(m_pixmapReplayBar, gc_replayBar, x+bar_w+5, y-2, m_layout);
#endif

	g_object_unref(layout);
	g_object_unref(gc_replayBar);
}

void ImageArea::ClearEFOVSpeedbar(void)
{
	GdkGC *gc_replayBar = gdk_gc_new(m_pixmapReplayBar);
	gdk_gc_set_foreground(gc_replayBar, g_black);
	gdk_draw_rectangle(m_pixmapReplayBar, gc_replayBar, TRUE, 0, 0, REPLAY_BAR_W, REPLAY_BAR_H);
	g_object_unref(gc_replayBar);
	m_speedbarDraw = false;
}

void ImageArea::DrawEFOVSpeedbar(int cur, int total, float speed)
{
	const int bar_w = kEFOVSpeedbarWidth; // 256
	const int bar_h = 12;
	const int hint_w = 60;
	const int x = hint_w;
	const int y = 0;
	const int optimal_speed_x = 12;
	const int optimal_speed_width = 45;

	GdkPoint init_point[3] = {{0, 0}, {-5, 6}, {5, 6}};
	GdkPoint points[3];

	int slip_pos = (int)(((float)cur/(float)total) * bar_w);

	GdkGC *gc = gdk_gc_new(m_pixmapReplayBar);
	PangoLayout *layout = gtk_widget_create_pango_layout(m_imageDA, NULL);
	pango_layout_set_font_description(layout, m_baseFont);
	if (m_speedbarDraw) {
		gdk_gc_set_foreground(gc, g_white);
		gdk_gc_set_function(gc, GDK_XOR);
		gdk_draw_line(m_pixmapReplayBar, gc, x+m_slipPosEFOV, y, x+m_slipPosEFOV, y+bar_h);

		for (int i=0; i<3; i++) {
			points[i].x = init_point[i].x + x + m_slipPosEFOV;
			points[i].y = init_point[i].y + y + bar_h;
		}
		gdk_draw_polygon(m_pixmapReplayBar, gc, TRUE, points, 3);
	} else {
        pango_layout_set_text(layout, _("Speed: "), -1);
        //pango_layout_set_text(m_layout, _("Speed: "), -1);
        //pango_layout_set_text(m_layout, N_("Focus:"), -1);

		gdk_gc_set_function(gc, GDK_COPY);
		gdk_gc_set_foreground(gc, g_black);
		gdk_draw_rectangle(m_pixmapReplayBar, gc, TRUE, 5, y-2, hint_w, 17);
		gdk_gc_set_foreground(gc, g_white);
		gdk_draw_layout(m_pixmapReplayBar, gc, 5, y-2, layout);
		//gdk_draw_layout(m_pixmapReplayBar, gc, 5, y-2, m_layout);

		RGBTRIPLE table[ImgProc2D::MAX_EFOV_COLOR];
		ImgProc2D::GetInstance()->GetEFOVSpeedColormap(table);
		gdk_gc_set_function(gc, GDK_COPY);
		for (int i = 0; i < 100; ++i) {
			GdkColor *color = NULL;
			color = new_color(table[i].rgbtRed*0xffff/0xff, table[i].rgbtGreen*0xffff/0xff, table[i].rgbtBlue*0xffff/0xff);
			gdk_gc_set_foreground(gc, color);
			gdk_draw_line(m_pixmapReplayBar, gc, x + i, y, x + i, y + bar_h);
			free_color(color);
		}
		gdk_gc_set_foreground(gc, g_yellow);
		gdk_draw_rectangle(m_pixmapReplayBar, gc, FALSE, x+optimal_speed_x, y, optimal_speed_width, bar_h+1);
	}

	gdk_gc_set_foreground(gc, g_white);
	gdk_gc_set_function(gc, GDK_XOR);
	gdk_draw_line(m_pixmapReplayBar, gc, x+slip_pos, y, x+slip_pos, y+bar_h);

	for (int i=0; i<3; i++) {
		points[i].x = init_point[i].x + x + slip_pos;
		points[i].y = init_point[i].y + y + bar_h;
	}
	m_slipPosEFOV = slip_pos;
	gdk_draw_polygon(m_pixmapReplayBar, gc, TRUE, points, 3);

	char buf[20];
	sprintf(buf, "%.1fmm/s", speed);
	pango_layout_set_text(layout, buf, -1);
	//pango_layout_set_text(m_layout, buf, -1);
	gdk_gc_set_function(gc, GDK_COPY);
	gdk_gc_set_foreground(gc, g_black);
	gdk_draw_rectangle(m_pixmapReplayBar, gc, TRUE, x+bar_w+5, y-2, 100, 17);
	gdk_gc_set_foreground(gc, g_white);
	gdk_draw_layout(m_pixmapReplayBar, gc, x+bar_w+5, y-2, layout);
	//gdk_draw_layout(m_pixmapReplayBar, gc, x+bar_w+5, y-2, m_layout);

	g_object_unref(layout);
	g_object_unref(gc);
	m_speedbarDraw = true;
}

GdkGC* ImageArea::NewPwTraceGC(GdkColor *color, GdkFunction mode)
{
	GdkGC *gc = gdk_gc_new(m_pixmapPwTrace);
	gdk_gc_set_foreground(gc, color);
	gdk_gc_set_function(gc, mode);
	return gc;
}

void ImageArea::DrawTrace(GdkFunction mode, const GdkColor* const color, int x1, int y1, int x2, int y2)
{
	gdk_gc_set_foreground(m_gcTrace, color);
	gdk_gc_set_function(m_gcTrace, mode);
	gdk_draw_line(m_pixmapPwTrace, m_gcTrace, x1, y1, x2, y2);
}

void ImageArea::DrawTracePt(GdkFunction mode, const GdkColor* const color, int x, int y)
{
	gdk_gc_set_foreground(m_gcTrace, color);
	gdk_gc_set_function(m_gcTrace, mode);
	gdk_draw_point(m_pixmapPwTrace, m_gcTrace, x, y);
}

void ImageArea::ClearTrace(void)
{
	gdk_gc_set_foreground(m_gcTrace, g_black);
	gdk_gc_set_function(m_gcTrace, GDK_COPY);
	gdk_draw_rectangle(m_pixmapPwTrace, m_gcTrace, TRUE, 0, 0, IMAGE_W, IMAGE_H);
}

void ImageArea::DrawTraceTag(GdkFunction mode, const GdkColor* const color, int x, int y, bool update)
{
	gdk_gc_set_foreground(m_gcTrace, color);
	gdk_gc_set_function(m_gcTrace, mode);

	GdkPoint init_point[3] = {{0, 0}, {-5, -6}, {5, -6}};
	GdkPoint points[3];
	for (int i=0; i<3; i++) {
		points[i].x = init_point[i].x + x;
		points[i].y = init_point[i].y + y;
	}
    gdk_draw_polygon(m_pixmapPwTrace, m_gcTrace, TRUE, points, 3);
    if(update)
        UpdateImgArea();
}

// @oara colorIndex[in] color index of body mark, range (0-5) white, gray ,red, green ,yellow, blue
void ImageArea::DrawProbeMark(int x, int y, guint direction, guint directionMax, double scale, guint colorIndex, guint index)
{
	GdkGC *gc = gdk_gc_new(m_pixmapArea);
	GdkColor *color;
	switch (colorIndex) {
		case 0:
			color = g_white; break;
		case 1:
			color = g_gray; break;
		case 2:
			color = g_red; break;
		case 3:
			color = g_green; break;
		case 4:
			color = g_yellow; break;
		default:
			color = g_blue; break;
	}
	gdk_gc_set_foreground(gc, color);
	gdk_gc_set_function(gc, GDK_XOR);

	double p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, p4_x, p4_y;
	double p5_x, p5_y, p6_x, p6_y, p7_x, p7_y, p8_x, p8_y;
	int t1_x, t1_y, t2_x, t2_y, t3_x, t3_y, t4_x, t4_y;
	int t5_x, t5_y, t6_x, t6_y, t7_x, t7_y, t8_x, t8_y;
	int devide = 6;
	double t0_x, t0_y;
	double cosy, siny, cosAngle, sinAngle;
	double Len;
	double slopy = (directionMax - direction) * PI * 2 / (float)directionMax;
	double angle = 30 * PI / (float)180;

	cosy = cos( slopy );
	siny = sin( slopy );
	cosAngle = cos(angle);
	sinAngle = sin(angle);

	Len = 24.0 * scale;

	//head
	p1_x = - (Len * 3 / (float)devide);
	p1_y = Len * 1 / (float)(devide*2);

	p2_x = - (Len * 3 / (float)devide);
	p2_y = - (Len * 1 / (float)(devide*2));

	p3_x = - (Len * 2 / (float)devide) - 2;
	p3_y = (Len * 1 / (float)(devide*2));

	p4_x = - (Len * 2 / (float)devide) - 2;
	p4_y = - (Len * 1 / (float)(devide*2));

	//tail
	p5_x = - (Len * 1 / (float)devide);
	p5_y = (Len * 1 / (float)(devide*2));

	p6_x = - (Len * 1 / (float)devide);
	p6_y = - (Len * 1 / (float)(devide*2));

	p7_x = (Len * 3 / (float)devide);
	p7_y = (Len * 1 / (float)(devide*2));

	p8_x = (Len * 3 / (float)devide);
	p8_y = - (Len * 1 / (float)(devide*2));

	//printf("1-4: (%.3f, %.3f),(%.3f, %.3f),(%.3f, %.3f),(%.3f, %.3f)\n", p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, p4_x, p4_y);
	//printf("5-8: (%.3f, %.3f),(%.3f, %.3f),(%.3f, %.3f),(%.3f, %.3f)\n", p5_x, p5_y, p6_x, p6_y, p7_x, p7_y, p8_x, p8_y);

	// rotate coordinate system
	t0_x = x;
	t0_y = y;
	t1_x = t0_x + p1_x*cosy + p1_y*siny + 0.5;
	t1_y = t0_y + p1_y*cosy - p1_x*siny + 0.5;
	t2_x = t0_x + p2_x*cosy + p2_y*siny + 0.5;
	t2_y = t0_y + p2_y*cosy - p2_x*siny + 0.5;
	t3_x = t0_x + p3_x*cosy + p3_y*siny + 0.5;
	t3_y = t0_y + p3_y*cosy - p3_x*siny + 0.5;
	t4_x = t0_x + p4_x*cosy + p4_y*siny + 0.5;
	t4_y = t0_y + p4_y*cosy - p4_x*siny + 0.5;

	t5_x = t0_x + p5_x*cosy + p5_y*siny + 0.5;
	t5_y = t0_y + p5_y*cosy - p5_x*siny + 0.5;
	t6_x = t0_x + p6_x*cosy + p6_y*siny + 0.5;
	t6_y = t0_y + p6_y*cosy - p6_x*siny + 0.5;
	t7_x = t0_x + p7_x*cosy + p7_y*siny + 0.5;
	t7_y = t0_y + p7_y*cosy - p7_x*siny + 0.5;
	t8_x = t0_x + p8_x*cosy + p8_y*siny + 0.5;
	t8_y = t0_y + p8_y*cosy - p8_x*siny + 0.5;
	cosAngle = cos(angle);
	sinAngle = sin(angle);

	//PRINTF("1-4t: (%f, %f),(%f, %f),(%f, %f),(%f, %f)\n", t1_x, t1_y, t2_x, t2_y, t3_x, t3_y, t4_x, t4_y);
	//printf("1-4t: (%d, %d),(%d, %d),(%d, %d),(%d, %d)\n", t1_x, t1_y, t2_x, t2_y, t3_x, t3_y, t4_x, t4_y);
	//printf("5-8t: (%d, %d),(%d, %d),(%d, %d),(%d, %d)\n", t5_x, t5_y, t6_x, t6_y, t7_x, t7_y, t8_x, t8_y);


	// draw probe mark
	//gdk_draw_line(m_pixmapBDMK[index], gc, t1_x, t1_y, t2_x, t2_y);
	//gdk_draw_line(m_pixmapBDMK[index], gc, t3_x, t3_y, t4_x, t4_y);
	//gdk_draw_line(m_pixmapBDMK[index], gc, t1_x, t1_y, t3_x, t3_y);
	//gdk_draw_line(m_pixmapBDMK[index], gc, t2_x, t2_y, t4_x, t4_y);
	gdk_gc_set_line_attributes(gc, 1, GDK_LINE_SOLID, GDK_CAP_ROUND, GDK_JOIN_MITER);
	gdk_draw_arc(m_pixmapBDMK[index], gc, TRUE, (t1_x+t3_x)/2-3, (t1_y+t3_y)/2-3, 6, 6, 0, 360*64);

	gdk_gc_set_line_attributes(gc, 5, GDK_LINE_SOLID, GDK_CAP_ROUND, GDK_JOIN_MITER);
	//gdk_draw_line(m_pixmapBDMK[index], gc, t5_x, t5_y, t6_x, t6_y);
	//gdk_draw_line(m_pixmapBDMK[index], gc, t7_x, t7_y, t8_x, t8_y);
	gdk_draw_line(m_pixmapBDMK[index], gc, t5_x, t5_y, t7_x, t7_y);
	//gdk_draw_line(m_pixmapBDMK[index], gc, t6_x, t6_y, t8_x, t8_y);


	g_object_unref(gc);
	UpdateImgArea();
}

void ImageArea::DrawMeaCursor(GdkFunction mode, int type, int size, const GdkColor* const color, int x, int y)
{
	GdkGC *gc = gdk_gc_new(m_pixmapArea);
	gdk_gc_set_foreground(gc, color);
	gdk_gc_set_function(gc, mode);

	switch (type) {
		case 0:
			switch (size) {
				case 0: // big
					gdk_draw_line(m_pixmapArea, gc, x-7, y,   x+7, y);
					gdk_draw_line(m_pixmapArea, gc, x,   y-7, x,   y+7);
					break;
				case 1: // mid
					gdk_draw_line(m_pixmapArea, gc, x-5, y,   x+5, y);
					gdk_draw_line(m_pixmapArea, gc, x,   y-5, x,   y+5);
					break;
				case 2: // small
					gdk_draw_line(m_pixmapArea, gc, x-3, y,   x+3, y);
					gdk_draw_line(m_pixmapArea, gc, x,   y-3, x,   y+3);
			}
			break;
		case 1:
			switch (size) {
				case 0:
					gdk_draw_line(m_pixmapArea, gc, x+7, y-7, x-7, y+7);
					gdk_draw_line(m_pixmapArea, gc, x-7, y-7, x+7, y+7);
					break;
				case 1:
					gdk_draw_line(m_pixmapArea, gc, x+5, y-5, x-5, y+5);
					gdk_draw_line(m_pixmapArea, gc, x-5, y-5, x+5, y+5);
					break;
				case 2:
					gdk_draw_line(m_pixmapArea, gc, x+3, y-3, x-3, y+3);
					gdk_draw_line(m_pixmapArea, gc, x-3, y-3, x+3, y+3);
					break;
			}
			break;
		case 2:
			switch (size) {
				case 0:
					gdk_draw_line(m_pixmapArea, gc, x-6, y,   x+6, y);
					gdk_draw_line(m_pixmapArea, gc, x-7, y-3, x-7, y+3);
					gdk_draw_line(m_pixmapArea, gc, x+7, y-3, x+7, y+3);
					gdk_draw_line(m_pixmapArea, gc, x,   y-6, x,   y+6);
					gdk_draw_line(m_pixmapArea, gc, x-3, y-7, x+3, y-7);
					gdk_draw_line(m_pixmapArea, gc, x-3, y+7, x+3, y+7);
					break;
				case 1:
					gdk_draw_line(m_pixmapArea, gc, x-4, y,   x+4, y);
					gdk_draw_line(m_pixmapArea, gc, x-5, y-2, x-5, y+2);
					gdk_draw_line(m_pixmapArea, gc, x+5, y-2, x+5, y+2);
					gdk_draw_line(m_pixmapArea, gc, x,   y-4, x,   y+4);
					gdk_draw_line(m_pixmapArea, gc, x-2, y-5, x+2, y-5);
					gdk_draw_line(m_pixmapArea, gc, x-2, y+5, x+2, y+5);
					break;
				case 2:
					gdk_draw_line(m_pixmapArea, gc, x-2, y,   x+2, y);
					gdk_draw_line(m_pixmapArea, gc, x-3, y-1, x-3, y+1);
					gdk_draw_line(m_pixmapArea, gc, x+3, y-1, x+3, y+1);
					gdk_draw_line(m_pixmapArea, gc, x,   y-2, x,   y+2);
					gdk_draw_line(m_pixmapArea, gc, x-1, y-3, x+1, y-3);
					gdk_draw_line(m_pixmapArea, gc, x-1, y+3, x+1, y+3);
					break;
			}
			break;
		case 3:
			switch (size) {
				case 0:
					gdk_draw_line(m_pixmapArea, gc, x+7, y-7, x-7, y+7);
					gdk_draw_line(m_pixmapArea, gc, x-7, y-7, x+7, y+7);
					gdk_draw_line(m_pixmapArea, gc, x-7, y,   x+7, y);
					break;
				case 1:
					gdk_draw_line(m_pixmapArea, gc, x+5, y-5, x-5, y+5);
					gdk_draw_line(m_pixmapArea, gc, x-5, y-5, x+5, y+5);
					gdk_draw_line(m_pixmapArea, gc, x-5, y,   x+5, y);
					break;
				case 2:
					gdk_draw_line(m_pixmapArea, gc, x+3, y-3, x-3, y+3);
					gdk_draw_line(m_pixmapArea, gc, x-3, y-3, x+3, y+3);
					gdk_draw_line(m_pixmapArea, gc, x-3, y,   x+3, y);
					break;
			}
	}
	g_object_unref(gc);
	UpdateImgArea();
}


GdkGC* ImageArea::NewMeasureGC(GdkColor *color, GdkFunction mode)
{
	GdkGC *gc = gdk_gc_new(m_pixmapArea);
	gdk_gc_set_foreground(gc, color);
	gdk_gc_set_function(gc, mode);
	return gc;
}

// Draw Measure Ellipse only
void ImageArea::DrawPixmapPt(GdkGC *gc, int x, int y)
{
	gdk_draw_point(m_pixmapArea, gc, x, y);
}

// index from buttom to top
void ImageArea::DrawMeasureResult(const char *result, int pos, int lines, int cursorType, const GdkColor* const color, bool update)
{
	int width, height;
	const int x = 0;
   	//PRINTF("%s-%s: result=\n%s\n", __FILE__, __FUNCTION__, result);
	PRINTF("%s-%s: pos=%d, lines=%d\n", __FILE__, __FUNCTION__, pos, lines);
    m_curCountLines = lines;

    if(m_meaResultFontSize == 14)
    {
        if(m_meaResultLineH > 25)
            m_meaResultLineH = 25;
    }
    else
    {
        if(m_meaResultLineH > 18)
            m_meaResultLineH = 18;
    }

    int y = (pos - lines)*m_meaResultLineH;
    //从下往上显示
    //int y = m_meaResultRect.h - pos*m_meaResultLineH;
    //PRINTF("%s-%s: y=%d\n", __FILE__, __FUNCTION__, y);

    //first to draw order number
    char str[4] = {0};
    snprintf(str, 4, "%d.", cursorType);
    PangoLayout *layout = gtk_widget_create_pango_layout(m_imageDA, str);
    char str_font[256];
    sprintf(str_font, "%s, %d", "DejaVu Sans Mono, Book", m_meaResultFontSize);
    PangoFontDescription *des = pango_font_description_from_string(str_font);
    pango_layout_set_font_description(layout, des);
    pango_font_description_free(des);
    pango_layout_set_alignment(layout, PANGO_ALIGN_LEFT);
    pango_layout_get_pixel_size(layout, &width, &height);

    int cursor_w = 25;
    if(width < 40 && width > 0)
        cursor_w = width;

	pango_layout_set_text(m_layout, result, -1);
	pango_layout_get_pixel_size(m_layout, &width, &height);

    PRINTF("%s-%s: string height = %d\n", __FILE__, __FUNCTION__, height);
	PRINTF("%s: current line %d, total height %d, single height %d, size = %d\n", __FUNCTION__, lines, height, m_meaResultLineH, size);

	std::vector<MeasurePos>::iterator iter = m_meaResultPos.end() - 1;
	if (!m_meaResultPos.empty() && iter->posY == y && iter->lines == lines) {
		iter->width = width + cursor_w;
	} else {
		MeasurePos meaPos;
		meaPos.posY = y;
		meaPos.lines = lines;
		meaPos.width = width + cursor_w;
		m_meaResultPos.push_back(meaPos);
	}

    gdk_gc_set_foreground(m_gcMeaResult, g_black);

    //从下往上显示
    //gdk_draw_rectangle(m_pixmapMeaResult, m_gcMeaResult, TRUE, x, 0, m_meaResultRect.w, y+height);
    gdk_draw_rectangle(m_pixmapMeaResult, m_gcMeaResult, TRUE, x, y, m_meaResultRect.w, m_meaResultRect.h - y);
    PRINTF("%s-%s: draw rectange w=%d, h=%d\n", __FILE__, __FUNCTION__, m_meaResultRect.w, y+height);
	gdk_gc_set_foreground(m_gcMeaResult, color);
    //DrawMeasureResultCursor(m_gcMeaResult, cursorType, color, x+7, y+7);

    gdk_draw_layout(m_pixmapMeaResult, m_gcMeaResult, x, y+1, layout);
    g_object_unref(layout);

    gdk_draw_layout(m_pixmapMeaResult, m_gcMeaResult, x + cursor_w, y+1, m_layout);

    if (update)
        UpdateImgArea();
}


void ImageArea::DrawStringFps(const char *str, int x, int y, const GdkColor* const color)
{
	int width, height;

	GdkGC *gc = gdk_gc_new(m_pixmapFps);
	PangoLayout *layout = gtk_widget_create_pango_layout(m_imageDA, str);
	pango_layout_set_font_description(layout, m_baseFont);
	pango_layout_set_alignment(layout, PANGO_ALIGN_LEFT);
	pango_layout_get_pixel_size(layout, &width, &height);
	gdk_gc_set_foreground(gc, g_black);
	gdk_draw_rectangle(m_pixmapFps, gc, TRUE, x, y, width, height);

	gdk_gc_set_foreground(gc, color);
	gdk_draw_layout(m_pixmapFps, gc, x, y, layout);

	g_object_unref(layout);
	g_object_unref(gc);
	UpdateImgArea();
}

int ImageArea::GetMeasurePosSize(void)
{
	return m_meaResultPos.size();
}

void ImageArea::ClearLastMeasureResult(void)
{
	if (m_meaResultPos.empty())
		return ;

	vector<MeasurePos>::iterator iter = m_meaResultPos.end() - 1;
	int y = iter->posY;
	int lines = iter->lines;
	const int x = 0;

	m_meaResultPos.pop_back();

	gdk_gc_set_foreground(m_gcMeaResult, g_black);
	gdk_gc_set_function(m_gcMeaResult, GDK_COPY);
    gdk_draw_rectangle(m_pixmapMeaResult, m_gcMeaResult, TRUE, x, y, m_meaResultRect.w, m_meaResultLineH*lines);
	UpdateImgArea();
}

void ImageArea::ClearMeasureAll(void)
{
	m_meaResultPos.clear();
	gdk_gc_set_foreground(m_gcMeaResult, g_black);
	gdk_gc_set_function(m_gcMeaResult, GDK_COPY);
	gdk_draw_rectangle(m_pixmapMeaResult, m_gcMeaResult, TRUE, 0, 0, m_meaResultRect.w, m_meaResultRect.h);
}

void ImageArea::DrawMeasureResultCursor(GdkGC *gc, int type, const GdkColor* const color, int x, int y)
{
	/*switch (type) {
		case 0:
			gdk_draw_line(m_pixmapMeaResult, gc, x-5, y,   x+5, y);
			gdk_draw_line(m_pixmapMeaResult, gc, x,   y-5, x,   y+5);
			break;
		case 1:
			gdk_draw_line(m_pixmapMeaResult, gc, x+5, y-5, x-5, y+5);
			gdk_draw_line(m_pixmapMeaResult, gc, x-5, y-5, x+5, y+5);
			break;
		case 2:
			gdk_draw_line(m_pixmapMeaResult, gc, x-4, y,   x+4, y);
			gdk_draw_line(m_pixmapMeaResult, gc, x-5, y-2, x-5, y+2);
			gdk_draw_line(m_pixmapMeaResult, gc, x+5, y-2, x+5, y+2);
			gdk_draw_line(m_pixmapMeaResult, gc, x,   y-4, x,   y+4);
			gdk_draw_line(m_pixmapMeaResult, gc, x-2, y-5, x+2, y-5);
			gdk_draw_line(m_pixmapMeaResult, gc, x-2, y+5, x+2, y+5);
			break;
		case 3:
			gdk_draw_line(m_pixmapMeaResult, gc, x+5, y-5, x-5, y+5);
			gdk_draw_line(m_pixmapMeaResult, gc, x-5, y-5, x+5, y+5);
			gdk_draw_line(m_pixmapMeaResult, gc, x-5, y,   x+5, y);
			break;
	}*/

	char str[4] = {0};
	snprintf(str, 4, "%d.", type);
	PangoLayout *layout = gtk_widget_create_pango_layout(m_imageDA, str);
    char str_font[256];
    sprintf(str_font, "%s, %d", "DejaVu Sans Mono, Book", m_meaResultFontSize);
    PangoFontDescription *des = pango_font_description_from_string(str_font);
	//pango_layout_set_font_description(layout, m_baseFont);
    pango_layout_set_font_description(layout, des);
    pango_font_description_free(des);
	pango_layout_set_alignment(layout, PANGO_ALIGN_LEFT);
    gdk_draw_layout(m_pixmapMeaResult, gc, x, y, layout);
	g_object_unref(layout);
}

void ImageArea::DrawNoteText(const char *str, int x, int y, const GdkColor* const color, int font_size)
{
	int width, height;
	char font_desc[100];

	GdkGC *gc = gdk_gc_new(m_pixmapArea);
	PangoLayout *layout = gtk_widget_create_pango_layout(m_imageDA, str);
	sprintf(font_desc, "%s, %d", FONT_STRING, font_size);
	PangoFontDescription *font = pango_font_description_from_string(font_desc);
	pango_layout_set_font_description(layout, font);
	pango_layout_set_alignment(layout, PANGO_ALIGN_LEFT);
	pango_layout_get_pixel_size(layout, &width, &height);

	gdk_gc_set_foreground(gc, g_black);
	gdk_draw_rectangle(m_pixmapArea, gc, TRUE, x, y, width, height);

	gdk_gc_set_foreground(gc, color);
	gdk_draw_layout(m_pixmapArea, gc, x, y, layout);

	g_object_unref(layout);
	g_object_unref(gc);
	UpdateImgArea();
}

void ImageArea::DrawFTBitmap(FT_Bitmap* bitmap, int x, int y, const GdkColor* const color)
{
	int width = bitmap->pitch;
	int height = bitmap->rows;

	unsigned char* data = (unsigned char *)m_imagePara->imageData;

	for (int i = IMG_AREA_W*y*3 + x*3, k=0; i < (IMG_AREA_W*(y+height)-(IMG_AREA_W-x-width))*3 && k < height; i+=IMG_AREA_W*3, k++) {
		for (int j = 0, n = 0; j < width * 3 && n < width; j+=3, n++) {
			data[i+j] = (guchar)((float)(color->red>>8) * (float)(bitmap->buffer[n+k*width]) / (float)255);
			data[i+j+1] = (guchar)((float)(color->green>>8) * (float)(bitmap->buffer[n+k*width]) / (float)255);
			data[i+j+2] = (guchar)((float)(color->blue>>8) * (float)(bitmap->buffer[n+k*width]) / (float)255);
		}
	}
	//  UpdateImgArea();
}

void ImageArea::DrawFTBitmap(FT_Bitmap* bitmap, int x, int y, const GdkColor* const color, bool xorMode)
{
	int width = bitmap->pitch;
	int height = bitmap->rows;

	GdkImage *iA = gdk_drawable_get_image(m_pixmapArea, 0, 0, IMAGE_W, IMAGE_H);
	unsigned char* data = (unsigned char *)iA->mem;
	GdkColor colorT;

	for (int i = IMAGE_W*y*4 + x*4, k=0; i < (IMAGE_W*(y+height)-(IMAGE_W-x-width))*4 && k < height; i+=IMAGE_W*4, k++)
	{
		for (int j = 0, n = 0; j < width * 4 && n < width; j+=4, n++)
		{
			if (bitmap->buffer[n+k*width] > 0)
			{
				if (xorMode)
				{
					data[i+j] = data[i+j] ^ ((guchar)((float)(color->blue>>8) * (float)(bitmap->buffer[n+k*width]) / (float)255));
					data[i+j+1] = data[i+j+1] ^ ((guchar)((float)(color->green>>8) * (float)(bitmap->buffer[n+k*width]) / (float)255));
					data[i+j+2] = data[i+j+2] ^ ((guchar)((float)(color->red>>8) * (float)(bitmap->buffer[n+k*width]) / (float)255));
					data[i+j+3] = 0;
				}
				else
				{
					data[i+j] = (guchar)((float)(color->blue>>8) * (float)(bitmap->buffer[n+k*width]) / (float)255);
					data[i+j+1] = (guchar)((float)(color->green>>8) * (float)(bitmap->buffer[n+k*width]) / (float)255);
					data[i+j+2] = (guchar)((float)(color->red>>8) * (float)(bitmap->buffer[n+k*width]) / (float)255);
					data[i+j+3] = 0;
				}
			}
		}
	}

	GdkGC *gc = gdk_gc_new(m_pixmapArea);
	gdk_gc_set_function(gc, GDK_COPY);
	gdk_draw_image(m_pixmapArea, gc, iA, 0, 0, 0, 0, IMAGE_W, IMAGE_H);
	g_object_unref(gc);
	g_object_unref(iA);
}

void ImageArea::DrawString(GdkGC *gc, int x, int y, const char *str)
{
    //GdkFont *font;
    //font=gdk_font_load("-adobe-helvetica-medium-r-normal--17-*-*-*-*-*-iso8859-1");
    //gdk_draw_string(m_pixmapArea, font, gc, x, y, str);
#if 1
    cairo_t *cr;
    cr = gdk_cairo_create(m_pixmapArea);
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_select_font_face(cr, "Purisa",       CAIRO_FONT_SLANT_NORMAL,       CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 13);
    cairo_move_to(cr, x, y);
    cairo_show_text(cr, str);
    cairo_destroy(cr);
#endif
}

void ImageArea::DrawString(const char *str, int x, int y, const GdkColor* const color)
{
	const int xdpi = 96;
	const int ydpi = 96;
	PangoFontMap *PFM = pango_ft2_font_map_new();
	pango_ft2_font_map_set_resolution(PANGO_FT2_FONT_MAP(PFM), xdpi, ydpi);
	PangoContext *context = pango_font_map_create_context(PFM);
	pango_context_set_base_gravity(context, PANGO_GRAVITY_SOUTH);
	pango_context_set_gravity_hint(context, PANGO_GRAVITY_HINT_NATURAL);
	pango_context_set_language(context, pango_language_get_default());
	pango_context_set_base_dir(context, PANGO_DIRECTION_LTR);

	PangoLayout *layout = pango_layout_new(context);
	pango_layout_set_text(layout, str, -1);
	pango_layout_set_auto_dir(layout, 1);
	pango_layout_set_ellipsize(layout, PANGO_ELLIPSIZE_NONE);
	pango_layout_set_justify(layout, 0);
	pango_layout_set_single_paragraph_mode(layout, 0);
	pango_layout_set_wrap(layout, PANGO_WRAP_WORD_CHAR);
	pango_layout_set_alignment(layout, PANGO_ALIGN_LEFT);

	// PangoFontDescription *des = pango_font_description_from_string("DejaVu Sans Mono, Book, 10");
	pango_layout_set_font_description(layout, m_baseFont);
	pango_layout_set_markup (layout, str, -1);
	// pango_font_description_free(des);

	int width, height;
	pango_layout_get_pixel_size(layout, &width, &height);

	FT_Bitmap* bitmap;
	bitmap = g_slice_new(FT_Bitmap);

	bitmap->width = width;
	bitmap->rows = height;
	bitmap->pitch = (bitmap->width+3) & ~3;
	guchar *graybitmap = (guchar*)g_malloc(bitmap->pitch*bitmap->rows);
	memset(graybitmap, 0x00, bitmap->pitch*bitmap->rows);
	bitmap->buffer = graybitmap;
	bitmap->num_grays = 256;
	bitmap->pixel_mode = ft_pixel_mode_grays;//FT_PIXEL_MODE_GRAY;
	DrawFTBitmap(bitmap, x, y, color);
	pango_ft2_render_layout(bitmap, layout, 0, 0);
	DrawFTBitmap(bitmap, x, y, color);
	g_free(graybitmap);

	g_object_unref(layout);
	g_object_unref(PFM);
	g_object_unref(context);
}

void ImageArea::DrawString(const char *str, int x, int y, const GdkColor* const color, bool xorMode)
{
	const int xdpi = 96;
	const int ydpi = 96;
	PangoFontMap *PFM = pango_ft2_font_map_new();
	pango_ft2_font_map_set_resolution(PANGO_FT2_FONT_MAP(PFM), xdpi, ydpi);
	PangoContext *context = pango_font_map_create_context(PFM);
	pango_context_set_base_gravity(context, PANGO_GRAVITY_SOUTH);
	pango_context_set_gravity_hint(context, PANGO_GRAVITY_HINT_NATURAL);
	pango_context_set_language(context, pango_language_get_default());
	pango_context_set_base_dir(context, PANGO_DIRECTION_LTR);

	PangoLayout *layout = pango_layout_new(context);
	pango_layout_set_text(layout, str, -1);
	pango_layout_set_auto_dir(layout, 1);
	pango_layout_set_ellipsize(layout, PANGO_ELLIPSIZE_NONE);
	pango_layout_set_justify(layout, 0);
	pango_layout_set_single_paragraph_mode(layout, 0);
	pango_layout_set_wrap(layout, PANGO_WRAP_WORD_CHAR);
	pango_layout_set_alignment(layout, PANGO_ALIGN_LEFT);

	// PangoFontDescription *des = pango_font_description_from_string("DejaVu Sans Mono, Book, 10");
	pango_layout_set_font_description(layout, m_baseFont);
	pango_layout_set_markup (layout, str, -1);
	// pango_font_description_free(des);

	int width, height;
	pango_layout_get_pixel_size(layout, &width, &height);

	FT_Bitmap* bitmap;
	bitmap = g_slice_new(FT_Bitmap);

	bitmap->width = width;
	bitmap->rows = height;
	bitmap->pitch = (bitmap->width+3) & ~3;
	guchar *graybitmap = (guchar*)g_malloc(bitmap->pitch*bitmap->rows);
	memset(graybitmap, 0x00, bitmap->pitch*bitmap->rows);
	bitmap->buffer = graybitmap;
	bitmap->num_grays = 256;
	bitmap->pixel_mode = ft_pixel_mode_grays;//FT_PIXEL_MODE_GRAY;
	pango_ft2_render_layout(bitmap, layout, 0, 0);
	DrawFTBitmap(bitmap, x, y, color, xorMode);
	g_free(graybitmap);

	g_object_unref(layout);
	g_object_unref(PFM);
	g_object_unref(context);
}


void ImageArea::HideBodyMark(void)
{
	m_inBDMK = false;
}

void ImageArea::ClearBodyMark(guint index)
{
	const int bdmk_width = BDMK_W * BDMK_MAX_SCALE;
	const int bdmk_height = BDMK_H * BDMK_MAX_SCALE;
	if(index < 4)
	{
		GdkGC *gc = gdk_gc_new(m_pixmapBDMK[index]);
		gdk_gc_set_foreground(gc, g_black);
		gdk_draw_rectangle(m_pixmapBDMK[index], gc, TRUE, 0, 0, bdmk_width, bdmk_height);
		g_object_unref(gc);
	}
	else
	{
		for(guint i=0; i<4; i++)
		{
			GdkGC *gc = gdk_gc_new(m_pixmapBDMK[i]);
			gdk_gc_set_foreground(gc, g_black);
			gdk_draw_rectangle(m_pixmapBDMK[i], gc, TRUE, 0, 0, bdmk_width, bdmk_height);
			g_object_unref(gc);
		}
	}
}

void ImageArea::ChangeCurrentBDMK(guint index)
{
	m_bdmkLast = index;
}

void ImageArea::RedrawBodyMarkWhenModeChanged()
{
	for(guint i=0; i<4; i++)
	{
		if(i != m_bdmkLast)
		{
			ClearBodyMark(i);
			m_bdmkPos[i].x = 0;
			m_bdmkPos[i].y = 0;
			m_bdmkWidth[i] = 0;
			m_bdmkHeight[i] = 0;;
		}
	}
#if 1
	//UpdateImgArea();
	//swap BodyMark
	GdkPixmap *pixmap = m_pixmapBDMK[0];
	m_bdmkWidth[0] = m_bdmkWidth[m_bdmkLast];
	m_bdmkHeight[0] = m_bdmkHeight[m_bdmkLast];
	m_pixmapBDMK[0] = m_pixmapBDMK[m_bdmkLast];
	m_pixmapBDMK[m_bdmkLast] = pixmap;
	//m_bdmkWidth[m_bdmkLast] = 0;
	//m_bdmkHeight[m_bdmkLast] = 0;
#endif

	ModeStatus ms;
	Format2D::EFormat2D format2D = ms.GetFormat2D();
	FormatCfm::EFormatCfm formatCfm = ms.GetFormatCfm();
	int curB = ms.Get2DCurrentB();

	// BB Mode
	if( (ms.IsD2Mode() && format2D == Format2D::BB)
			|| ((ms.IsCFMMode() || ms.IsPDIMode()) && formatCfm == FormatCfm::BB) )
	{
		if(curB == 0)
		{
			m_bdmkPos[0].x = IMG_W / 2 - BDMK_W * BDMK_MAX_SCALE + IMAGE_X;
			m_bdmkPos[0].y = IMG_H - BDMK_H * BDMK_MAX_SCALE + IMAGE_Y;
		}
		else
		{
			m_bdmkPos[0].x = IMG_W - BDMK_W * BDMK_MAX_SCALE + IMAGE_X;
			m_bdmkPos[0].y = IMG_H - BDMK_H * BDMK_MAX_SCALE + IMAGE_Y;
		}
	}
	// 4B Mode
	else if( (ms.IsD2Mode() && format2D == Format2D::B4)
			|| ((ms.IsCFMMode() || ms.IsPDIMode()) && formatCfm == FormatCfm::B4) )
	{
		if(curB == 0)
		{
			m_bdmkPos[0].x = IMG_W / 2 - BDMK_W * BDMK_MAX_SCALE + IMAGE_X;
			m_bdmkPos[0].y = IMG_H / 2 - BDMK_H * BDMK_MAX_SCALE + IMAGE_Y;
		}
		else if(curB == 1)
		{
			m_bdmkPos[0].x = IMG_W - BDMK_W * BDMK_MAX_SCALE + IMAGE_X;
			m_bdmkPos[0].y = IMG_H / 2 - BDMK_H * BDMK_MAX_SCALE + IMAGE_Y;
		}
		else if(curB == 2)
		{
			m_bdmkPos[0].x = IMG_W / 2 - BDMK_W * BDMK_MAX_SCALE + IMAGE_X;
			m_bdmkPos[0].y = IMG_H - BDMK_H * BDMK_MAX_SCALE + IMAGE_Y;
		}
		else
		{
			m_bdmkPos[0].x = IMG_W - BDMK_W * BDMK_MAX_SCALE + IMAGE_X;
			m_bdmkPos[0].y = IMG_H - BDMK_H * BDMK_MAX_SCALE + IMAGE_Y;
		}
	}
	//Single B Mode
	else
	{
		m_bdmkPos[0].x = IMG_W - BDMK_W * BDMK_MAX_SCALE + IMAGE_X;
		m_bdmkPos[0].y = IMG_H - BDMK_H * BDMK_MAX_SCALE + IMAGE_Y;
	}
	m_bdmkLast = 0;

	UpdateImgArea();
}

/*
 * @brief draw body mark
 *
 * @para x[in] x coordinate of left corner of body mark
 * @para y[in] y coordinate of left corner of body mark
 * @para scale[in] zoom scale
 * @oara colorIndex[in] color index of body mark, range (0-5) white, gray ,red, green ,yellow, blue
 * @oara index[in] index of body mark, range (0-3)
 */
void ImageArea::DrawBodyMark(int x, int y, GdkPixbuf* pixbuf, double scale, guint colorIndex, guint index)
{
	if(index > 3)
	{
		//printf("Out of range, the bodymark index's range is 0~3 !\n");
		return;
	}

	m_bdmkLast = index;
	m_inBDMK = true;
	m_bdmkPos[index].x = x;
	m_bdmkPos[index].y = y;
	m_bdmkWidth[index] = gdk_pixbuf_get_width(pixbuf) * scale;
	m_bdmkHeight[index] = gdk_pixbuf_get_height(pixbuf) * scale;

	GdkPixbuf *pixbuf_scale = gdk_pixbuf_scale_simple(pixbuf, m_bdmkWidth[index], m_bdmkHeight[index], GDK_INTERP_NEAREST);
	GdkGC *gc = gdk_gc_new(m_pixmapBDMK[index]);
	gdk_gc_set_function(gc, GDK_COPY);
	GdkColor *color;
	switch (colorIndex) {
		case 0:
			color = g_white; break;
		case 1:
			color = g_gray; break;
		case 2:
			color = g_green; break;
		case 3:
			color = g_yellow; break;
		case 4:
			color = g_blue; break;
		default:
			color = g_green; break;
	}
	gdk_gc_set_foreground(gc, color);
	gdk_draw_pixbuf(m_pixmapBDMK[index], gc, pixbuf_scale, 0, 0, 0, 0, -1, -1, GDK_RGB_DITHER_NONE, 0, 0);
	g_object_unref(pixbuf_scale);
	g_object_unref(gc);

	UpdateImgArea();
}

gboolean ImageArea::UpdateFps()
{
#ifndef EMP_355
	ImageAreaPara::GetInstance()->UpdateGenFps(m_counts, TRUE);
#else
	ImageAreaPara::GetInstance()->UpdateGenFps(g_fps, m_counts, TRUE);
#endif
	ClearUpdateCounts();

	return TRUE;
}

void ImageArea::ClearUpdateCounts()
{
    m_counts = 0;
#ifdef EMP_355
    g_fps = 0;
#endif
}

void ImageArea::SetCineRemoveImg(int count)
{
	m_cineRemoveImg = count;
}

gboolean HandleUpdateFps(gpointer data)
{
	return ImageArea::GetInstance()->UpdateFps();
}

void ImageArea::AddTimeOutFps()
{
	if(m_tmFps > 0){
		g_source_remove(m_tmFps);
		m_tmFps = 0;
	}

	m_tmFps = g_timeout_add_seconds(1, HandleUpdateFps, NULL);
}

void ImageArea::DelTimeOutFps()
{
	if(m_tmFps > 0) {
		g_source_remove(m_tmFps);
		m_tmFps = 0;
	}
}

void ImageArea::DrawArrowHollow(int x, int y, guint direction, guint directionMax, double scale, guint colorIndex)
{
	GdkGC *gc = gdk_gc_new(m_pixmapArea);
	GdkColor *color;
	switch (colorIndex) {
		case 0:
			color = g_white; break;
		case 1:
			color = g_gray; break;
		case 2:
			color = g_red; break;
		case 3:
			color = g_green; break;
		case 4:
			color = g_yellow; break;
		default:
			color = g_blue; break;
	}
	gdk_gc_set_foreground(gc, color);
	gdk_gc_set_function(gc, GDK_XOR);

	double p0_x, p0_y, p1_x, p1_y, p2_x, p2_y;
	double p3_x, p3_y, p4_x, p4_y, p5_x, p5_y, p6_x, p6_y;
	double t0_x, t0_y, t1_x, t1_y, t2_x, t2_y;
	double t3_x, t3_y, t4_x, t4_y, t5_x, t5_y, t6_x, t6_y;
	double cosy, siny, cosAngle, sinAngle;
	double Len;
	double slopy = (directionMax - direction) * PI * 2 / (float)directionMax;
	double angle = 30 * PI / (float)180;

	cosy = cos( slopy );
	siny = sin( slopy );
	cosAngle = cos(angle);
	sinAngle = sin(angle);

	Len = 10.0 * scale;

	// head
	p0_x = 0;
	p0_y = 0;

	// bias line
	p1_x = Len * cosAngle;
	p1_y = Len * sinAngle;

	p2_x = Len * cosAngle;
	p2_y = - (Len * sinAngle);

	// straight line head
	p3_x = Len * cosAngle;
	p3_y = Len / 6.0;

	p4_x = Len * cosAngle;
	p4_y = - (Len / 6.0);

	// straight line tail
	p5_x = Len * cosAngle * 2;
	p5_y = Len / 6.0;

	p6_x = Len * cosAngle * 2;
	p6_y = - (Len / 6.0);

	// rotate coordinate system
	t0_x = x;
	t0_y = y;
	t1_x = t0_x + p1_x*cosy + p1_y*siny;
	t1_y = t0_y + p1_y*cosy - p1_x*siny;
	t2_x = t0_x + p2_x*cosy + p2_y*siny;
	t2_y = t0_y + p2_y*cosy - p2_x*siny;
	t3_x = t0_x + p3_x*cosy + p3_y*siny;
	t3_y = t0_y + p3_y*cosy - p3_x*siny;
	t4_x = t0_x + p4_x*cosy + p4_y*siny;
	t4_y = t0_y + p4_y*cosy - p4_x*siny;
	t5_x = t0_x + p5_x*cosy + p5_y*siny;
	t5_y = t0_y + p5_y*cosy - p5_x*siny;
	t6_x = t0_x + p6_x*cosy + p6_y*siny;
	t6_y = t0_y + p6_y*cosy - p6_x*siny;

	// head line
	//gdk_draw_line(m_pixmapArea, gc, t0_x, t0_y, t1_x, t1_y);
	//gdk_draw_line(m_pixmapArea, gc, t0_x, t0_y, t2_x, t2_y);
	gdk_draw_line(m_pixmapArea, gc, t1_x, t1_y, t0_x, t0_y);
	gdk_draw_line(m_pixmapArea, gc, t2_x, t2_y, t0_x, t0_y);
	gdk_draw_line(m_pixmapArea, gc, t1_x, t1_y, t3_x, t3_y);
	gdk_draw_line(m_pixmapArea, gc, t2_x, t2_y, t4_x, t4_y);

	// tail line
	gdk_draw_line(m_pixmapArea, gc, t3_x, t3_y, t5_x, t5_y);
	gdk_draw_line(m_pixmapArea, gc, t4_x, t4_y, t6_x, t6_y);
	gdk_draw_line(m_pixmapArea, gc, t5_x, t5_y, t6_x, t6_y);

	g_object_unref(gc);
	UpdateImgArea();
}


// @brief draw arrow with direction, size, color
// @para x,y the center coordinate
// @para direction(enum DIRECTION) value:0~23
// @oara colorIndex[in] color index of body mark, range (0-5) white, gray ,red, green ,yellow, blue
void ImageArea::DrawArrowSimple(int x, int y, guint direction, guint directionMax, double scale, guint colorIndex)
{
	GdkGC *gc = gdk_gc_new(m_pixmapArea);
	GdkColor *color;
	switch (colorIndex) {
		case 0:
			color = g_white; break;
		case 1:
			color = g_gray; break;
		case 2:
			color = g_red; break;
		case 3:
			color = g_green; break;
		case 4:
			color = g_yellow; break;
		default:
			color = g_blue; break;
	}
	gdk_gc_set_foreground(gc, color);
	gdk_gc_set_function(gc, GDK_XOR);

	double p0_x, p0_y, p1_x, p1_y, p2_x, p2_y, p3_x, p3_y;
	double t0_x, t0_y, t1_x, t1_y, t2_x, t2_y, t3_x, t3_y;
	double cosy, siny, cosAngle, sinAngle;
	double Len;
	double slopy = (directionMax - direction) * PI * 2 / (float)directionMax;
	double angle = 30 * PI / (float)180;

	cosy = cos( slopy );
	siny = sin( slopy );
	cosAngle = cos(angle);
	sinAngle = sin(angle);

	Len = 10.0 * scale;

	// head
	p0_x = 0;
	p0_y = 0;

	// bias line
	p1_x = Len * cosAngle;
	p1_y = Len * sinAngle;

	p2_x = Len * cosAngle;
	p2_y = - (Len * sinAngle);

	// tail
	p3_x = Len * cosAngle * 2;
	p3_y = 0;

	// rotate coordinate system
	t0_x = x;
	t0_y = y;
	t1_x = t0_x + p1_x*cosy + p1_y*siny;
	t1_y = t0_y + p1_y*cosy - p1_x*siny;
	t2_x = t0_x + p2_x*cosy + p2_y*siny;
	t2_y = t0_y + p2_y*cosy - p2_x*siny;
	t3_x = t0_x + p3_x*cosy + p3_y*siny;
	t3_y = t0_y + p3_y*cosy - p3_x*siny;

	// draw arrow
	//gdk_draw_line(m_pixmapArea, gc, t0_x, t0_y, t1_x, t1_y);
	//gdk_draw_line(m_pixmapArea, gc, t0_x, t0_y, t2_x, t2_y);
	gdk_draw_line(m_pixmapArea, gc, t1_x, t1_y, t0_x, t0_y);
	gdk_draw_line(m_pixmapArea, gc, t2_x, t2_y, t0_x, t0_y);
	gdk_draw_line(m_pixmapArea, gc, t0_x, t0_y, t3_x, t3_y);

	g_object_unref(gc);
	UpdateImgArea();
}

void ImageArea::DrawArrow(POINT head, POINT tail, bool draw, const GdkColor* const color, bool update)
{
	const int len = 10;
	double angle = atan2(head.y-tail.y, head.x-tail.x);
	double sineAngle = sin(angle);
	double cosineAngle = cos(angle);

	POINT tmpP;
	tmpP.x = head.x - len * cosineAngle;
	tmpP.y = head.y - len * sineAngle;
	// left line
	int leftPartX = tmpP.x + (len/2.0 * sineAngle);
	int leftPartY = tmpP.y - (len/2.0 * cosineAngle);
	// right line
	int rightPartX = tmpP.x - (len/2.0 * sineAngle);
	int rightPartY = tmpP.y + (len/2.0 * cosineAngle);

	if (draw) {
		cvLine(m_imageSymbol, cvPoint(head.x, head.y), cvPoint(tail.x, tail.y), CV_RGB(color->blue>>8, color->green>>8, color->red>>8));
		cvLine(m_imageSymbol, cvPoint(head.x, head.y), cvPoint(leftPartX, leftPartY), CV_RGB(color->blue>>8, color->green>>8, color->red>>8));
		cvLine(m_imageSymbol, cvPoint(head.x, head.y), cvPoint(rightPartX, rightPartY), CV_RGB(color->blue>>8, color->green>>8, color->red>>8));
	} else {
		cvLine(m_imageSymbol, cvPoint(head.x, head.y), cvPoint(tail.x, tail.y), CV_RGB(0, 0, 0));
		cvLine(m_imageSymbol, cvPoint(head.x, head.y), cvPoint(leftPartX, leftPartY), CV_RGB(0, 0, 0));
		cvLine(m_imageSymbol, cvPoint(head.x, head.y), cvPoint(rightPartX, rightPartY), CV_RGB(0, 0, 0));
	}
	if (update)
		UpdateImgArea();
}

// void ImageArea::DrawArrow(int x, int y, guint direction, guint directionMax, double scale, guint colorIndex)
// {
//     // unimplement
// }

#ifdef TRANSDUCER
void ImageArea::DrawTransducerResult(int x, int y, int width, int height, int TransducerGain)
{
    ImageArea::DrawAttr imgAttr;
    imgAttr.area = ImageArea::PARA;
    imgAttr.mode.gdkMode = GDK_XOR;


    int heightdiv=height/5;

    int i;

    DrawRectangle(imgAttr, g_white, x, y , width, height, false, false);

    DrawLine(imgAttr, g_white, x + width, y+heightdiv*5, x + width + 3, y+heightdiv*5);
	DrawLine(imgAttr, g_white, x + width, y+heightdiv*4, x + width + 3, y+heightdiv*4);
	DrawLine(imgAttr, g_white, x + width, y+heightdiv*3, x + width + 3, y+heightdiv*3);
	DrawLine(imgAttr, g_white, x + width, y+heightdiv*2, x + width + 3, y+heightdiv*2);
    DrawLine(imgAttr, g_white, x + width, y+heightdiv*1, x + width + 3, y+heightdiv*1);
    DrawLine(imgAttr, g_white, x + width, y+heightdiv*0, x + width + 3, y+heightdiv*0);

    DrawRectangle(imgAttr, g_black, x, y + height - m_transducerGainBak , width, m_transducerGainBak, true, false);

    DrawRectangle(imgAttr, g_white, x, (y + height - TransducerGain) , width, TransducerGain, true, false);

    DrawRectangle(imgAttr, g_white, x, y , width, height, false, false);

    m_transducerGainBak = TransducerGain;
}
#endif

void ImageArea::DrawMagnifier(int x, int y)
{
	const int size = 15;
	GdkGC *gc = gdk_gc_new(m_pixmapArea);
	gdk_gc_set_foreground(gc, g_green);
	gdk_gc_set_function(gc, GDK_XOR);

	gdk_draw_arc(m_pixmapArea, gc, false, x-size/2, y-size/2, size, size, 0, 360*64);
	gdk_draw_arc(m_pixmapArea, gc, false, x-size/2-2, y-size/2-2, size+4, size+4, 0, 360*64);
	gdk_draw_line(m_pixmapArea, gc, x+size/2, y+size/2, x+size+9, y+size+9);
	gdk_draw_line(m_pixmapArea, gc, x+size-2, y+size-4, x+size+9+1, y+size+9-1);
	gdk_draw_line(m_pixmapArea, gc, x+size-4, y+size-2, x+size+9-1, y+size+9+1);
	g_object_unref(gc);
}

void ImageArea::ClearEFOVImg()
{
	memset(m_bitsEFOV, 0, IMG_AREA_W*IMG_AREA_H*3);
}

void ImageArea::DrawBiopsyLine(GdkFunction mode, const GdkColor* const color, int x1, int y1, int x2, int y2, bool update, int distance)
{
    GdkGC *gc = NULL;
    gc = gdk_gc_new(m_pixmapPwTrace);
    gdk_gc_set_foreground(gc, color);
    gdk_gc_set_function(gc, mode);

    int x, y, count;

    int dist_x = x2 - x1;
    int dist_y = y2 - y1;

    int length = (int)(sqrt(dist_x*dist_x + dist_y*dist_y));

    if (length > distance)
    {
        count = (length / distance) * distance;
        while (count >= distance)
        {
            x = x1 + (int)((float)dist_x * (float)count / (float)length);
            y = y1 + (int)((float)dist_y * (float)count / (float)length);


            gdk_draw_point(m_pixmapPwTrace, gc, x,   y);
            gdk_draw_point(m_pixmapPwTrace, gc, x+1, y);
            gdk_draw_point(m_pixmapPwTrace, gc, x,   y+1);
            gdk_draw_point(m_pixmapPwTrace, gc, x+1, y+1);
            count -= distance;
        }
    }
    g_object_unref(gc);
    if (update)
        UpdateImgArea();
}