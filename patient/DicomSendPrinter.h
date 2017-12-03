#ifndef DICOMSENDPRINTER_H_
#define DICOMSENDPRINTER_H_

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <vector>
#include <string>
#include "FakeXEvent.h"
#include "Def.h"
#include "VideoMan.h"
#include "DicomSend.h"
#include "DCMDef.h"
using namespace std;

typedef struct
{
	int x;
	int y;
	int width;
	int height;

}areaImgInfo_t;



typedef struct
{
	GdkGC *gc;
	int x;
	int y;
	int width;
	int height;
	GdkColor* g_color;

}RectangleInfo_t;

typedef struct
{
	int x;
	int y;
	int width;
	int height;
	GdkPixbuf *imgPixBuf;

}DrawImgInfo_t;
class DicomSendPrinter: public FakeXEvent
{
	enum {SNAP_ROW = 7, SNAP_COL = 1, SNAP_W = 100, SNAP_H = 80, CINE_SNAP_W = 100, CINE_SNAP_H = 80};
	enum {IMG_WIDTH = 840, IMG_HEIGHT = 640, CINE_WIDTH = IMG_AREA_W, CINE_HEIGHT = IMG_AREA_H};

	enum Format{ONEXONE,ONEXTWO,TWOXTWO,TWOXTHREE,THREEXTHREE,THREEXFOUR,THREEXFIVE,FOURXFOUR,FOURXFIVE,FOURXSIX};
	enum FilmSize{IN8XIN10,IN10XIN12,IN10XIN14,IN11XIN14,IN14XIN14,IN14XIN17,CM24XCM24,CM24XCM30,A4};
	enum ORIENTATION{PORTRAIT,LANDSCAPE};
	enum E_ERRNO{NODEFUALT,FAIL_TO_GET,NORMAL};
	vector<DCMPRINTIMAGEINFO> m_ImageInfo;
public:
	DicomSendPrinter();
	~DicomSendPrinter();

	static DicomSendPrinter *GetInstance();

	GtkWidget* CreateItem(GtkWidget **button, GtkWidget **checkButton, char *id);
	void InitParameter(vector<string> examID);
	void CreateWindow(vector<string> examID,Format format,FilmSize filmsize,ORIENTATION orientation,GdkColor *bgColor);


	void UpdateCurPat(string examID, int curPat, int totalPat, bool cur);
	void GetSnapsName(string examID, vector<string> &vec);
	void DrawAreaExpose(GtkWidget *widget, GdkEventExpose *event);


	void BtnNextImgPageClicked(GtkButton* button);
	void BtnLastImgPageClicked(GtkButton* button);
	void BtnSnapImgClicked(GtkButton* button);
	void BtnNextPatClicked(GtkButton* button);
	void BtnLastPatClicked(GtkButton* button);

	void UpdateSnapColor();
	void HideAllBtn(void);
	void BtnExitClicked();

	vector<string> GetSelExamID(void);
	void BtnDisplayClicked(GtkMenuItem *menuitem);

	void DisplayImage(string fileName,int index);
	void UpdateImg(void);
	void UpdateSnap(void);
	void HideAllFixed(void);
	GtkWidget* GetWindow(void);

	float getMultiple(FilmSize filesize,ORIENTATION orientation);

	void getPositionSize(areaImgInfo_t positionsize,FilmSize filmsize,ORIENTATION orientation);

	void handleLeftButtonEvent(int x,int y);
	void InitRectangle();
	void HandleAllFormatArea(Format format,areaImgInfo_t areaImgInfo,FilmSize filmsize);
	void DrawRectangle(int index);
	void SetRectangleColor(int index,GdkColor* g_color);
	void InitImageInfo(int index);
	void DestroyDrawData();
	void DestroyImgData();
	void ConvertToSystemSetting();

	int getImageTotal(Format format);
	void KeyEvent(unsigned char keyValue);

	static DicomSendPrinter *m_pInstance;
	char m_id[SNAP_ROW * SNAP_COL][10];
	GtkWidget *m_vbox[SNAP_ROW * SNAP_COL];
	GtkWidget *m_btnSnap[SNAP_ROW * SNAP_COL];
	GtkWidget *m_checkBtnSnap[SNAP_ROW * SNAP_COL];
	GtkWidget *m_btnLeft;
	GtkWidget *m_btnRight;
	GtkWidget *m_labImgPage;
	GtkWidget *m_drawArea;
	GtkWidget *m_hboxFunc;
	GtkWidget *m_btnShow;
	GtkWidget *m_btnSlideExit;

    GtkWidget *m_fixedContent;

	gboolean m_isUdisk;
	vector<string> m_vecExamID;
	int m_curPat;
	vector<string> m_vecFileName; ///< all snap's name of current patient note:full path
	vector<bool> m_vecFileSel; ///< m_vecFileSel.size()= m_vecFileName.size(), select status of each snap
	int m_curImgPage;
	int m_maxImgPage;
	gint m_idShowed;


		GtkWidget *m_window;
		GtkWidget *m_labName;
	GtkWidget *m_labSex;
	GtkWidget *m_labExamNum;
	GtkWidget *m_labBirthday;
	GtkWidget *m_labStatInfo;

	gint m_numTotal;
	GtkWidget *m_labOwner;
	GtkWidget *m_labSpecies;

		Format m_format;
		FilmSize m_filmsize;
		areaImgInfo_t m_positionsize;
		vector<RectangleInfo_t> m_rectangleInfo;
		RectangleInfo_t m_rectanglePositionSize;
		int m_multiple;
		int m_index;
		vector<DrawImgInfo_t> m_drawImgInfo;
		int m_total;
		E_ERRNO m_errno;

static gboolean ExitWindow(gpointer data);

static void HandlePrintSettingClicked(GtkButton *button, DicomSendPrinter* data)
{
	if(data)
	data->ConvertToSystemSetting();

}

static void HandleBtnPrintClicked(GtkButton *button, DicomSendPrinter* data);
static void HandleBtnClearClicked(GtkButton *button, DicomSendPrinter* data)
{
		data->DestroyImgData();

}
static bool dealMousePress(GtkWidget *widget,GdkEventButton *event);


	static void HandleDicomSendPrinterClicked(GtkMenuItem *menuitem, gpointer *data)
	{
		DicomSendPrinter::GetInstance()->BtnDisplayClicked(menuitem);
	}

	static gboolean HandleDrawAreaExpose(GtkWidget *widget, GdkEventExpose *event, DicomSendPrinter *data)
		{
			data->DrawAreaExpose(widget, event);
			return FALSE;
		}

	static void HandleBtnNextImgPageClicked(GtkButton *button, DicomSendPrinter *data)
		{
			data->BtnNextImgPageClicked(button);
		}
	static void HandleBtnLastImgPageClicked(GtkButton *button, DicomSendPrinter* data)
		{
		data->BtnLastImgPageClicked(button);
		}
	static void HandleSnapImgClicked(GtkButton *button, DicomSendPrinter *data)
		{
			data->BtnSnapImgClicked(button);
		}

		static void HandleBtnNextPatClicked(GtkButton *button, DicomSendPrinter* data)
		{
			data->BtnNextPatClicked(button);


		}
		static void HandleBtnLastPatClicked(GtkButton *button, DicomSendPrinter* data)


		{ data->BtnLastPatClicked(button); }
		static void HandleBtnExitClicked(GtkButton *button, DicomSendPrinter* data)
			{
			data->BtnExitClicked();
			}


};
#endif