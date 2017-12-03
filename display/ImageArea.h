#ifndef _IMAGEAREA_H_
#define _IMAGEAREA_H_

#define IMG_AREA_W 844 //ModLayout 844 PreLayout 804  /* 1024-180 */
#define IMG_AREA_H 560 //ModLayout 560 PreLayout 540

#define IMAGE_X 110 //ModLayout 120 PreLayout 80
#define IMAGE_Y 30 //ModLayout 30 PreLayout 20

#define MEASURE_RES_LINES_MAX 13

//#define MEASURE_RES_AREA_W 400
//#define MEASURE_RES_AREA_H 272 //MEASURE_RES_LINES_MAX * MeasureResult_H
//#define MEASURE_RES_AREA_X 5
//#define MEASURE_RES_AREA_Y (IMG_AREA_H-MEASURE_RES_AREA_H)

#define REPLAY_BAR_X 425
#define REPLAY_BAR_Y 505
#define REPLAY_BAR_W 380  //370
#define REPLAY_BAR_H 24

#define EFOV_SPEEDBAR_X 600
#define EFOV_SPEEDBAR_Y 10

#if (defined(EMP_322) || defined(EMP_313))
#define FPS_POS_X 85
#else
#define FPS_POS_X 109
#endif
#define FPS_POS_Y 5
#define FPS_POS_W 55
#define FPS_POS_H 17

#include <string.h>
#include <gtk/gtk.h>
#include "Def.h"
#include "gui_global.h"
#include "MultiFuncFactory.h"
#include <vector>
#include "gui_func.h"
#include "keyboard/KeyDef.h"
// freetype2 support
#include <ft2build.h>
#include FT_FREETYPE_H

class ImageArea
{
public:
    ~ImageArea();
    static ImageArea* GetInstance();

    // enum DotSize{LARGE, MID, SMALL};

    enum DrawArea{SYMBOL, SPECTRA, PARA, PIXMAP, IMT};
    union DrawMode
    {
	bool cvDraw; 		// OpenCV draw or eraser;
	GdkFunction gdkMode;	// GDK drawing function
    };
    struct DrawAttr{
	DrawArea area;
	DrawMode mode;
    };
    struct RectArea {
	int x;
	int y;
	int w;
	int h;
    };

    static void DrawImgData(void* pBits, int nWidth, int nHeight);
    static void DrawImgDataFreeze(void* pBits, int nWidth, int nHeight);
    static void CopyImgDataToReplay(void);

    GtkWidget* Create(void);
    inline void UpdateImgArea(void);
    void ClearArea(bool update = false);
    void ClearScreen(bool update=true);
	void ClearScreenUnFreeze(bool update = false);
    static void UpdateSymbolArea(int x, int y, int width, int height);
    static void UpdateSpectraArea(int x, int y, int width, int height);
    void UpdateMeaResultArea(int fontsize);
    void DrawCFMPreBox(const RectArea& area);
    void ClearCFMPreBox(const RectArea& area);

    void SetReadImg(bool status);
    void ResetReadImg(void);
    void GetImgPtr(unsigned char ** image);//获取图像区(m_bits)的首地址
    GdkPixbuf* GetDrawingAreaImage(void);
    void PutDrawingArea(GdkPixbuf *pixbuf);
    void GetFrmData(guchar **pBits, int *nWidth, int *nHeight);
    void LoadFrm(void);
    void GetSnapData(guchar **pBits, int *nWidth, int *nHeight);
    void DrawSnap(GdkPixbuf *pixbuf, int src_x, int src_y, int width, int height);
    gboolean GetReadImg(void) { return m_inReadImg; }

    GdkGC* NewHistogramDC(GdkColor * color, GdkFunction mode);
    void DrawHistogram(GdkGC *gc, int x1, int y1, int x2, int y2);

    GdkGC* GetPixmapDC(GdkColor* color, GdkFunction mode);
    void DrawPixmapLine(GdkGC *gc, int x1, int y1, int x2, int y2);
    // void DrawLine(int x1, int y1, int x2, int y2, bool draw, const GdkColor* const color, DrawArea area);
    // void DrawLine(GdkFunction mode, const GdkColor* const color, int x1, int y1, int x2, int y2);
    // void DrawMeasureLine(GdkFunction mode, const GdkColor* const color, int x1, int y1, int x2, int y2);
    // void DrawLine(POINT p, int angle, int length, bool draw, const GdkColor* const color);
    // void DrawLineNoUpdate(const GdkColor* const color, int x1, int y1, int x2, int y2);
    void DrawLine(DrawAttr &attr, const GdkColor* const color, int x1, int y1, int x2, int y2, bool update=true);

    // void DrawPoint(GdkFunction mode, const GdkColor* const color, int x, int y);
    // void DrawPoint(int x, int y, bool draw, const GdkColor* const color);
    // void DrawScalePoint(int x, int y, bool draw, const GdkColor* const color);
    // void DrawMeasurePoint(GdkFunction mode, const GdkColor* const color, int x, int y);
    void DrawPoint(DrawAttr &attr, const GdkColor* const color, int x, int y, bool update=true);

    // void DrawCircle(int x, int y, int radius, bool filled, bool draw, const GdkColor* const color);
    // void DrawCircle(GdkFunction mode, const GdkColor* const color, int x, int y, int radius, gboolean filled);
    void DrawCircle(DrawAttr &attr, const GdkColor* const color, int x, int y, int radius, bool filled, bool update=true);

    // void DrawArc(int x, int y, int width, int height, double angle1, double angle2, bool draw, const GdkColor* const color);
    void DrawArc(DrawAttr &attr, const GdkColor* const color, int x, int y, int width, int height, double angle1, double angle2, bool update=true);
    void DrawDashArc(DrawAttr &attr, const GdkColor* const color, int x, int y, int width, int height, double angle1, double angle2, bool update=true, double step = 5);

    // void DrawRectangle(int x, int y, int width, int height, bool filled, const GdkColor* const color);
    // void DrawRectangle(GdkFunction mode, const GdkColor* const color, gboolean filled, int x, int y, int width, int height);
    void DrawRectangle(DrawAttr &attr, const GdkColor* const color, int x, int y, int width, int height, bool filled, bool update=true);

    // void DrawPolygon(GdkPoint *points, int nPoint, bool draw, const GdkColor* const color);
    void DrawPolygon(DrawAttr &attr, const GdkColor* const color, GdkPoint *points, int nPoint, bool update=true);

    // void DrawDashLine(int x1, int y1, int x2, int y2, bool draw, const GdkColor* const color);
    // void DrawMeasureDashLine(GdkFunction mode, const GdkColor* const color, int x1, int y1, int x2, int y2, DotSize size=MID);

    void DrawDashLine(DrawAttr &attr, const GdkColor* const color, int x1, int y1, int x2, int y2, bool update=true, int distance=10);
    void DrawDashLine(DrawAttr &attr, const GdkColor* const color, int x1, int y1, int x2, int y2, int size, bool update=true);

    void ClearReplayBar(void);
    void DrawReplayBar(int cur_img, int total_img, int left, int right);

    GdkGC* NewPwTraceGC(GdkColor *color, GdkFunction mode);
    void DrawTrace(GdkFunction mode, const GdkColor* const color, int x1, int y1, int x2, int y2);
    void DrawTracePt(GdkFunction mode, const GdkColor* const color, int x, int y);
    void DrawTraceTag(GdkFunction mode, const GdkColor* const color, int x, int y, bool update=false);
    void ClearTrace(void);
    void SetTraceDraw(const bool drawPwTrace) { m_drawPwTrace = drawPwTrace; }

    void DrawProbeMark(int x, int y, guint direction, guint directionMax, double scale, guint colorIndex, guint index);

    // void DrawCursor(GdkFunction mode, int type, int size, const GdkColor* const color, int x, int y);
    void DrawMeaCursor(GdkFunction mode, int type, int size, const GdkColor* const color, int x, int y);
    // measure result area
    void DrawMeasureResult(const char *result, int pos, int lines, int cursorType, const GdkColor* const color, bool update=true);

    int GetMeasurePosSize(void);
    void ClearLastMeasureResult(void);
    void ClearMeasureAll(void);

    // Draw Measure Ellipse only
    GdkGC* NewMeasureGC(GdkColor *color, GdkFunction mode);
    void DrawPixmapPt(GdkGC *gc, int x, int y);

    void DrawNoteText(const char *str, int x, int y, const GdkColor* const color, int font_size);
    void DrawString(GdkGC *gc, int x, int y, const char *str);
	void DrawString(const char *str, int x, int y, const GdkColor* const color=g_white);
    void DrawString(const char *str, int x, int y, const GdkColor* const color, bool xorMode);

    void DrawStringFps(const char *str, int x, int y, const GdkColor* const color=g_white);

    void DrawBodyMark(int x, int y, GdkPixbuf* pixbuf, double scale, guint colorIndex, guint index);
    void HideBodyMark(void);
    void ClearBodyMark(guint index);
	void RedrawBodyMarkWhenModeChanged();
	void ChangeCurrentBDMK(guint index);

    void DrawMagnifier(int x, int y);

    void DrawArrowHollow(int x, int y, guint direction, guint directionMax, double scale, guint colorIndex);
    void DrawArrowSimple(int x, int y, guint direction, guint directionMax, double scale, guint colorIndex);
//    void DrawArrow(int x, int y, guint direction, guint directionMax, double scale, guint colorIndex);
    void DrawArrow(POINT head, POINT tail, bool draw, const GdkColor* const color, bool update=true);

    void AddTimeOutFps();
    void DelTimeOutFps();
    void ClearUpdateCounts();

    gboolean UpdateFps();
    static void SetCineRemoveImg(int count);

    void EFOVZoomMiniatureBox(int start_x, int start_y, int end_x, int end_y);
    void ClearEFOVSpeedbar(void);
    void DrawEFOVSpeedbar(int cur, int total, float speed);
    void ClearEFOVImg();
#ifdef TRANSDUCER
    void DrawTransducerResult(int x, int y, int width, int height, int TransducerGain);
#endif
    void DrawBiopsyLine(GdkFunction mode, const GdkColor* const color, int x1, int y1, int x2, int y2, bool update=true, int distance=10);

public:
    static unsigned char m_TpbitsIMT[IMAGE_W*IMAGE_H*IMG_BPP];
    static unsigned char m_bitsImg[IMAGE_W*IMAGE_H*IMG_BPP];

private:
    ImageArea();
    void DrawMeasureResultCursor(GdkGC *gc, int type, const GdkColor* const color, int x, int y);
	void DrawFTBitmap(FT_Bitmap* bitmap, int x, int y, const GdkColor* const color);
    void DrawFTBitmap(FT_Bitmap* bitmap, int x, int y, const GdkColor* const color, bool xorMode);

    static ImageArea* m_ptrInstance;

    static GtkWidget *m_imageDA;

    PangoFontDescription *m_baseFont;

	GdkPixbuf *m_imgPixbuf;

	GdkPixbuf *m_snapPixbufBak;

	//static unsigned char m_bitsImg[IMAGE_W*IMAGE_H*IMG_BPP];
	//used for mix when in EFOV
	static unsigned char m_bitsEFOV[IMG_AREA_W*IMG_AREA_H*IMG_BPP];
	//the last frame replay data
	static unsigned char m_bitsLastReplay[IMG_AREA_W*IMG_AREA_H*IMG_BPP];
	//used for mix when in freeze mode
	static unsigned char m_bitsReplayMix[IMG_AREA_W*IMG_AREA_H*IMG_BPP];
	//used for mix when in unfreeze mode
	static unsigned char m_bitsMix[IMG_AREA_W*IMG_AREA_H*IMG_BPP];
    static unsigned char m_bitsIMT[IMG_AREA_W*IMG_AREA_H*3];//hlx
    static int m_counts;
    bool m_drawPwTrace;

    IplImage* m_imageIMT;//hlx
    IplImage* m_imageSymbol;
    IplImage* m_imageSpectra;
    IplImage* m_imagePara;
    IplImage* m_imagePreBox;
    GdkPixbuf *m_IMTPixbuf;
    GdkPixbuf *m_mixPixbuf;
    GdkPixbuf *m_lastReplayPixbuf;
    GdkPixbuf *m_mixReplayPixbuf;
    GdkPixbuf *m_frmPixbuf;

    static RectArea m_symbolArea;	// update m_imageSymbol area
    static RectArea m_spectraArea;      // update m_imageSpectra area
    GdkPixmap *m_pixmapArea;
    GdkPixmap *m_pixmapReplayBar;
    GdkPixmap *m_pixmapPwTrace;
    GdkPixmap *m_pixmapMeaResult;
    GdkPixmap *m_pixmapBDMK[4];
    GdkPixmap *m_pixmapFps;
    GdkPixmap *m_pixmapHistogram;
    GdkGC *m_gcTrace;
    GdkGC *m_gcMeaResult;
    PangoLayout *m_layout;

    bool m_inReadImg;
    bool m_inBDMK;
    GdkPoint m_bdmkPos[4];
    int m_bdmkWidth[4];
    int m_bdmkHeight[4];
	guint m_bdmkLast;

#ifdef TRANSDUCER
    GdkPixmap *m_pixmapTransducer;
    int m_transducerGainBak;
#endif

    struct MeasurePos {
	int posY;
	int lines;
	int width;
    };
    std::vector<MeasurePos> m_meaResultPos;
	int m_meaResultLineH; //测量结果每行高度（像素），通过pango_layout_get_size()得到，注意：调用该函数时如果layout中无字符，得到的结果可能不对
	struct RectArea m_meaResultRect;
    int m_curCountLines;

    guint m_tmFps;
    static int m_cineRemoveImg;

	int m_slipPosEFOV;
	bool m_speedbarDraw;
    int m_meaResultFontSize;

//signal handle
    void ImageAreaConfigure(GtkWidget *widget, GdkEventConfigure *event);
    void ImageAreaExpose(GtkWidget *widget, GdkEventExpose *event);

//signal connect
    static gboolean HandleImageAreaConfigure(GtkWidget *widget, GdkEventConfigure *event, ImageArea *data)
	{ data->ImageAreaConfigure(widget, event); return FALSE; }
    static gboolean HandleImageAreaExpose(GtkWidget *widget, GdkEventExpose *event, ImageArea *data)
	{ data->ImageAreaExpose(widget, event); return FALSE; }
};

void ImageArea::UpdateImgArea()
{
    gtk_widget_queue_draw(m_imageDA);
}

#endif /* _IMAGEAREA_H_ */