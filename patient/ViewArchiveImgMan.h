#ifndef _VIEW_ARCHIVE_IMG_MAN_H
#define _VIEW_ARCHIVE_IMG_MAN_H

#include <vector>
#include <string>
#include "FakeXEvent.h"
#include "Def.h"
#include "VideoMan.h"

using std::vector;
using std::string;

/*
 * @brief dialog of image management in archive management
 * @note dialog must be global
 */
class ViewArchiveImgMan: public FakeXEvent
{
	public:
		~ViewArchiveImgMan();
		static ViewArchiveImgMan* GetInstance();

		void CreateWindow(vector<string> examID, const char* selected=NULL);
		void KeyEvent(unsigned char keyValue);
		void DestroyWindow(void);
		void WhenViewOneImg(void);
		static void DrawImgData(void* pBits, int nWidth, int nHeight);
		void DisplayFirstImage();
		void DisplayFirstCine();
		void DisplayCine(string CineName);
		void DisplaySelectedImage(int i);
		void DisplaySelectedCine(int i);

	private:
		ViewArchiveImgMan();
		static ViewArchiveImgMan* m_ptrInstance;

		gboolean m_isUdisk;

		enum {SNAP_ROW = 7, SNAP_COL = 1, SNAP_W = 100, SNAP_H = 80, CINE_SNAP_W = 100, CINE_SNAP_H = 70};
		enum {IMG_WIDTH = 840, IMG_HEIGHT = 640, CINE_WIDTH = Image::WIDTH, CINE_HEIGHT = Image::HEIGHT};
		enum FileType{IMG, CINE, REPORT, INFO};

		GtkWidget *m_window;
		GtkWidget *m_fixedContent;
		GtkWidget *m_fixedReport;
		GtkWidget *m_labImgSum;
		GtkWidget *m_labName;
#ifdef VET
		GtkWidget *m_labOwner;
		GtkWidget *m_labSpecies;
#endif
		GtkWidget *m_labSex;
		GtkWidget *m_labBirthday;
		GtkWidget *m_labExamNum;
		GtkWidget *m_labExamType;
		GtkWidget *m_labImgPage;
		GtkWidget *m_labStatInfo;
		GtkWidget *m_comboType;
		GtkWidget *m_hboxCineReplay;
		GtkWidget *m_progressBar;
		GtkWidget *m_hboxFunc;
		GtkWidget *m_btnShow;
		GtkWidget *m_btnSnap[SNAP_ROW * SNAP_COL];
		GtkWidget *m_checkBtnSnap[SNAP_ROW * SNAP_COL];
		GtkWidget *m_vbox[SNAP_ROW * SNAP_COL];
		char m_id[SNAP_ROW * SNAP_COL][10];
		GtkWidget *m_drawArea; // maybe we can add a pixmap here for background of pixbuf
		GtkWidget *m_viewport;
		GtkWidget *m_imageReport;
		GtkWidget *m_imagePage[5];
		GtkWidget *m_btnReplay;

		//pat info
		GtkWidget *m_fixedPatInfo;
		GtkWidget *m_labEntryName;
#ifdef VET
		GtkWidget *m_labEntrySpecies;
		GtkWidget *m_labEntryOwner;
#endif
		GtkWidget *m_labEntryExamID;
		GtkWidget *m_labEntryPatID;
		GtkWidget *m_labEntryAge;
		GtkWidget *m_labEntryGender;
		GtkWidget *m_labEntryBirth;
		GtkWidget *m_labEntryExamDate;
		GtkWidget *m_labEntryExamTime;
		GtkWidget *m_labEntryExamType;
		GtkWidget *m_labEntryPhysician;
		GtkWidget *m_labEntryDiagnostic;
		GtkWidget *m_textviewComment;
		GtkWidget *m_labEntryGenWeight;
		GtkWidget *m_labEntryGenStature;
		GtkWidget *m_labEntryGenBSA;
		GtkWidget *m_labEntryOBLMP;
		GtkWidget *m_labEntryOBOvuTime;
		GtkWidget *m_labEntryOBGravida;
		GtkWidget *m_labEntryOBEctopic;
		GtkWidget *m_labEntryOBGest;
		GtkWidget *m_labEntryOBPara;
		GtkWidget *m_labEntryOBAborta;
		GtkWidget *m_labEntryCarBP;
		GtkWidget *m_labEntryCarHR;
		GtkWidget *m_labEntryUroPSA;
		GtkWidget *m_labEntryOtherTele;
		GtkWidget *m_labEntryOtherAddress;

        GtkWidget *m_radiobutton_auto;
        GtkWidget *m_radiobutton_orignial;
        GtkWidget *m_scrolledwindow_auto;
        GtkWidget *m_scrolledwindow_orignial;
		
		GdkPixbuf *m_imgPixbuf;
		GdkPixbuf *m_cinePixbuf;
		unsigned char m_imgData[IMG_WIDTH * IMG_HEIGHT * 3];
		unsigned char m_cineData[CINE_WIDTH * CINE_HEIGHT * 3];

        GtkWidget *report_image[5]; //最多5页报告

		vector<string> m_vecExamID;
		int m_curPat;
		enum FileType m_fileType;

		vector<string> m_vecFileName; ///< all snap's name of current patient note:full path
		vector<bool> m_vecFileSel; ///< m_vecFileSel.size()= m_vecFileName.size(), select status of each snap
		int m_curImgPage;
		int m_maxImgPage;

		vector<string> m_vecCineName; ///< all snap's name of current patient note:full path
		vector<bool> m_vecCineSel; ///< m_vecFileSel.size()= m_vecFileName.size(), select status of each snap
		int m_curCinePage;
		int m_maxCinePage;

		///< cine
		int m_cineCur;
		int m_cineTotal;
		int m_fps;
		int m_timer;
		bool m_statusReplay;
        int   m_reportNum;
        char m_archivePath[256];


		void InitParameter(vector<string> examID, const char *selected);
		GtkWidget* CreateReportArea(GtkWidget *fixed);
		GtkWidget* CreateImgArea(GtkWidget *fixed);
		GtkWidget* CreatePatInfoArea(GtkWidget *fixed);

		void DrawNewImgData(void* pBits, int nWidth, int nHeight);
		GtkWidget* CreateItem(GtkWidget **button, GtkWidget **checkButton, char *id);
		void UpdateCurPat(string examID, int curPatIndex, int totalPat); //index start from 0, totalPat start from 1
		void UpdateReport(string examID);
		void UpdateSnap(void);
		void UpdateImg(void);
		void UpdateCine(void);
		void DisplayImage(string fileName);
		void GetSnapsName(string examID, vector<string> &vec);
		void GetCinesName(string examID, vector<string> &vec);
		void HideAllBtn(void);
		void ReadCineToReplay(VideoMan::VideoItem *videoItem);
		void DisplayCineMenu(bool yes);
		void UpdateCineInfo(int current, int total);
		void EnterCine(void);
		void ExitCine(void);

		void HideAllFixed(void);
		void UpdatePatInfo(string examID);

		//signal
		void BtnExitClicked(GtkButton* button);
		void BtnEditClicked(GtkButton* button);
	//	void BtniShowUdiskClicked(GtkButton* button);
		void BtniShowClicked(GtkButton* button);
		void BtnDeleteClicked(GtkButton* button);
		void BtnUnselAllClicked(GtkButton* button);
		void BtnSelAllClicked(GtkButton* button);
		void BtnLastPatClicked(GtkButton* button);
		void BtnNextPatClicked(GtkButton* button);
		void BtnLastImgPageClicked(GtkButton* button);
		void BtnNextImgPageClicked(GtkButton* button);
		void BtnSnapImgClicked(GtkButton* button);
		void ChkBtnClicked(GtkButton* button);
		void ComboTypeClicked(GtkComboBox *combobox);
		void BtnReplayClicked(GtkButton* button);
		void BtnPauseClicked(GtkButton* button);
		void BtnPrintClicked(GtkButton* button);
		void DrawLoadingInfo();
		void DrawAreaConfigure(GtkWidget *widget, GdkEventConfigure *event);
        void DrawAreaExpose(GtkWidget *widget, GdkEventExpose *event);

		// call back
		static void HandleBtnExitClicked(GtkButton *button, ViewArchiveImgMan* data) { data->BtnExitClicked(button); }
		static void HandleBtnEditClicked(GtkButton *button, ViewArchiveImgMan* data) { data->BtnEditClicked(button); }
	//	static void HandleBtnShowUdiskClicked(GtkButton *button, ViewArchiveImgMan* data) { data->BtniShowUdiskClicked(button); }
		static void HandleBtnShowClicked(GtkButton *button, ViewArchiveImgMan* data) { data->BtniShowClicked(button); }
		static void HandleBtnDeleteClicked(GtkButton *button, ViewArchiveImgMan* data) { data->BtnDeleteClicked(button); }
		static void HandleBtnUnselAllClicked(GtkButton *button, ViewArchiveImgMan* data) { data->BtnUnselAllClicked(button); }
		static void HandleBtnSelAllClicked(GtkButton *button, ViewArchiveImgMan* data) { data->BtnSelAllClicked(button); }
		static void HandleBtnLastPatClicked(GtkButton *button, ViewArchiveImgMan* data) { data->BtnLastPatClicked(button); }
		static void HandleBtnNextPatClicked(GtkButton *button, ViewArchiveImgMan* data) { data->BtnNextPatClicked(button); }
		static void HandleBtnLastImgPageClicked(GtkButton *button, ViewArchiveImgMan* data) { data->BtnLastImgPageClicked(button); }
		static void HandleBtnNextImgPageClicked(GtkButton *button, ViewArchiveImgMan* data) { data->BtnNextImgPageClicked(button); }
		static void HandleSnapImgClicked(GtkButton *button, ViewArchiveImgMan* data) { data->BtnSnapImgClicked(button); }
		static void HandleCheckBtnClicked(GtkButton *button, ViewArchiveImgMan* data) { data->ChkBtnClicked(button); }
		static void HandleComboTypeClicked(GtkComboBox *combobox, ViewArchiveImgMan* data) { data->ComboTypeClicked(combobox); }
		static void HandleBtnReplayClicked(GtkButton *button, ViewArchiveImgMan* data) { data->BtnReplayClicked(button); }
		static void HandleBtnPrintClicked(GtkButton *button, ViewArchiveImgMan* data) { data->BtnPrintClicked(button); }
		static gboolean HandleDrawAreaConfigure(GtkWidget *widget, GdkEventConfigure *event, ViewArchiveImgMan *data) 
		{
			data->DrawAreaConfigure(widget, event);
			return FALSE;
		}
		static gboolean HandleDrawAreaExpose(GtkWidget *widget, GdkEventExpose *event, ViewArchiveImgMan *data) 
		{
			data->DrawAreaExpose(widget, event);
			return FALSE;
		}
        
        static void on_auto_radio_button_toggled(GtkToggleButton *togglebutton, ViewArchiveImgMan *data) { data->AutoRadioToggled(togglebutton); }
        static void on_orignial_radio_button_toggled(GtkToggleButton *togglebutton, ViewArchiveImgMan *data) { data->OrignialRadioToggled(togglebutton); }
        void AutoRadioToggled(GtkToggleButton *togglebutton);
        void OrignialRadioToggled(GtkToggleButton *togglebutton);
        GtkWidget* CreateReport(string examID, int mode);
};
#endif
