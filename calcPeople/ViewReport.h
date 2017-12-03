#ifndef VIEWREPORT_H
#define VIEWREPORT_H

#include <string>
#include <vector>
#include "FakeXEvent.h"
#include "AbsCalendarOpr.h"
#include "FileMan.h"
#include "MeaResult.h"
#include "DCMDef.h"

using std::string;
using std::vector;


//ADD BY JHUANG BEGIN
typedef struct CalcNode
{
    int id;
    int section;
    bool check;
    char title[256];
    char value[256];
    char gw[256];
    char edcb[256];    
    int row;
    int stamp;
    struct CalcNode *next;
}CalcNode_t;

#define MAX_CALC_COUNT		(MEA_END - MEA_START)
#define OB_CALC_COUNT		(OB_MEA_END - OB_MEA_START)
#define AverRes_COUNT		(6)
#define EFW_COUNT			(2)
#define PSAD_COUNT			(2)
#define OTHERS_COUNT            (2)
#define MAX_CALCARR_COUNT  ((MAX_CALC_COUNT + OB_CALC_COUNT) + OTHERS_COUNT + PSAD_COUNT + EFW_COUNT + AverRes_COUNT)

//OTHERS start
#define OTHERS_START_ID                             (MAX_CALC_COUNT + OB_CALC_COUNT)
#define OTHERS_COMMENT_SHOW_ID                      (OTHERS_START_ID) //show comment
#define OTHERS_INDICTION_SHOW_ID                    (OTHERS_START_ID + 1) //show indiction
#define OTHERS_END_ID                               (OTHERS_START_ID + OTHERS_COUNT)
//OTHERS end

#define OB_FETAL2_START_ID                          (MAX_CALC_COUNT)
#define PSAD_START_ID                               ((MAX_CALC_COUNT + OB_CALC_COUNT) + OTHERS_COUNT)
#define EFW_START_ID                                ((MAX_CALC_COUNT + OB_CALC_COUNT) + OTHERS_COUNT + PSAD_COUNT)
#define AVERRES_START_ID                            ((MAX_CALC_COUNT + OB_CALC_COUNT) + OTHERS_COUNT + PSAD_COUNT + EFW_COUNT)

//ADD BY JHUANG END

//class ViewReport:public AbsKeyboardEvent
class ViewReport:public FakeXEvent, public AbsCalendarOpr, public FileMan
{
public:
    ~ViewReport();
    static ViewReport* GetInstance();
    void CreateWindow(void);
    void DestroyWindow(void);
    GtkWidget* GetWindow(void);
    const char* GetCheckPart(void);
    const char* GetIndication(void) { return m_strReport[0].c_str(); }
    const char* GetComments(void) { return m_strReport[1].c_str(); }
    void SetIndication(const char* text);
    void SetComments(const char* text);
    void InsertIndication(const char* text);
    void InsertComments(const char* text);
    vector<string> GetSelectImage(void);
    bool ArchiveReport(guint exam_id);
    char* LoadReport(guint exam_id);
    char* GetArchivePath(guint exam_id);
#if 0
    void LoadMeasureData(guint page_num);
    //void LoadMeasureData(void);
#else
    //void LoadMeasureData(guint page_num);
    void LoadMeasureData(void);
#endif
    void LoadMeasureDataForArchive(void);
    void LoadImage(void);
    void InitTableData(void);
    void ClearIndicationandComments(void);

#if 1 //add by jhuang 
    int GetCountFromCalcArr(void);
    bool GetItemFromCalcArr(int seq, CalcNode_t *result);
    void DefaultCalcArr(void);
    int SearchIDFromCalcArr(int stamp, int row);
    bool SetCheckToCalcArr(int id, char *title, bool fixed);
#endif 

private:
    ViewReport();
    static ViewReport* m_ptrInstance;

    pthread_t m_pid1, m_pid2, m_pid3;
    int m_dateFormat;
    int row_height;
    int m_TableAbd[ABD_MEA_END-ABD_MEA_START][CALC_MAX];
    int m_TableUr[UR_MEA_END-UR_MEA_START][CALC_MAX];
    int m_TableAdult[ADULT_MEA_END-ADULT_MEA_START][CALC_MAX];
    int m_TableOb[OB_MEA_END-OB_MEA_START][CALC_MAX];
    int m_TableGyn[GYN_MEA_END-GYN_MEA_START][CALC_MAX];
    int m_TableSp[SP_MEA_END-SP_MEA_START][CALC_MAX];
    int m_TableVs[VS_MEA_END-VS_MEA_START][CALC_MAX];
    int m_TableFetal[FETAL_MEA_END-FETAL_MEA_START][CALC_MAX];
    int m_TableTcd[TCD_MEA_END-TCD_MEA_START][CALC_MAX];
    int m_TableOrtho[ORTHO_MEA_END-ORTHO_MEA_START][CALC_MAX];
    int m_TableUser[USER_END-USER_START][CALC_MAX];
    bool IsETypeInSection(int etype, int Section);
	int GetSectionForDepartment(const char *department);
#ifdef VET
//hlx 
		int m_TableTD[TD_MEA_END-TD_MEA_START][CALC_MAX];
//hlx 
		int m_TableAnOB[ANOB_MEA_END-ANOB_MEA_START][CALC_MAX];
#endif
    static const int SNAP_W = 100;
    static const int SNAP_H = 80;
//hlx 
#ifdef VET
		enum{
			NB_ABD,
			NB_UR,
			NB_AE,
		//	NB_OB,
			NB_ANOB,
			NB_SP,
			NB_VES,
			NB_FE,
			NB_TCD,
			NB_ORT,
			NB_TD
		};
#else
		
#if (defined(EMP_322) || defined(EMP_313))
enum{
        NB_ABD,
        NB_UR,
        NB_AE,
        NB_OB,
        NB_GYN,
        NB_SP,
        NB_FE,
        NB_ORT
    };
#else
    enum{
        NB_ABD,
        NB_UR,
        NB_AE,
        NB_OB,
        NB_GYN,
        NB_SP,
        NB_VES,
        NB_FE,
        NB_TCD,
        NB_ORT,
        NB_USER
    };
#endif
#endif
    enum{
    	 C_L_GEN_COL, //ADD BY JHUANG
        T_L_GEN_COL,
        V_L_GEN_COL,
        V1_L_GEN_COL,
        V2_L_GEN_COL,
        V3_L_GEN_COL,
        N_GEN_COLS
    };

    enum{
	 C_OB_COL,//ADD BY JHUANG
        T_OB_COL,
        V_OB_COL,
        V1_OB_COL,
        V2_OB_COL,
        V3_OB_COL,
        GW_OB_COL,
        EDCB_OB_COL,
        N_OB_COLS
    };

    enum{
        C_OB_AFI_COL,
        T_OB_AFI_COL,
        V_OB_AFI_COL,
        V1_OB_AFI_COL,
        V2_OB_AFI_COL,
        V3_OB_AFI_COL,
        N_OB_AFI_COLS
    };

    GtkWidget *m_treeview_left, *m_treeview_right, *m_treeview_ob; //add by huangj
    GtkWidget *m_window, *m_notebook, *m_fixed, *m_frameImage, *m_frameEdit, *m_swEdit;


#ifdef VET
		GtkWidget *m_entryAnimalName;
		GtkWidget *m_entryOwnerName;
		GtkWidget *m_entrySpecies;
#endif
    GtkWidget *m_entryHos, *m_entryName, *m_entryID, *m_entrySID, *m_entryDate, *m_entryStudy, *m_entryItem, *m_entryUDr, *m_entryRDr, *m_entrySex, *m_entryAge;
    GtkWidget *m_viewportABD, *m_viewportUR, *m_viewportAE, *m_viewportOB, *m_viewportGYN, *m_viewportSP, *m_viewportFE, *m_viewportORT;
//hlx 
	GtkWidget *m_viewportTD;
	GtkWidget *m_viewportAnOB;



    GtkWidget *m_btnTransfer;



#if not defined(EMP_322)
#if not defined(EMP_313)
        GtkWidget *m_viewportTCD,*m_viewportVES;
#endif
#endif
   		GtkWidget *m_viewportImage;

#ifdef REPORTPICS_6
		static const guint MAX_SEL = 6;
#else
		static const guint MAX_SEL = 2;
#endif

		void SetPatientInfo(void);
		void KeyEvent(unsigned char keyValue);
		GtkWidget* CreateImageView(void);
		void ShowImageView(void);
		void HideImageView(void);
		void ShowEditView(void);
		void HideEditView(void);
		GtkWidget* CreateImageItem(int sid, int id);
		static bool Sort(const string s1, const string s2);
		void LoadSnapInfo();
		bool m_showFlag;
		vector<string> m_vecName, m_vecNameSel, m_vecNameTmp;
		vector<GtkWidget*> m_vecChkBtn;
		string m_strReport[2];

		bool m_side;	//false: left side   true: right side
		bool m_right;
		typedef struct{
			const char *title;
			float data[MEA_TIMES+1];
			const char *units;
			int id; //add by jhuang
			int section;
		}ResultTable;
		typedef struct{
			const char *title;
			float data[MEA_TIMES+1];
			const char *units;
			const char *GW;
			const char *EDCB;
			int id; //add by jhuang
			int section;
		}ResultObTable;

		int m_dataGC[2];
		GdkPixmap *m_pixmapGC;
		GtkWidget *m_comboboxGC;
		GtkWidget *m_drawingareaGC;
		GtkWidget *m_entryDes[9];
		int m_grade[7];
		GtkTreeView* create_gen_treeview(GtkTreeModel *model);
		GtkTreeView* create_ob_treeview(GtkTreeModel *model);
		GtkTreeView* create_ob_afi_treeview(GtkTreeModel *model);

		gint AddResultToTree( GtkWidget *treeview, ResultTable result, bool side, gint rows);
		gint AddResultToObTree(GtkWidget *treeview, ResultObTable result, int fetal, int row);


        DCMSRELEMENT GetSRElement();
        string GetExamDoctor();
        string GetHospitalName();

#if 1 //add by jhuang
        void ShowMeasureData(guint page_num);
        void ShowAllCalcArr(void);
        int AddResultToTree2( GtkWidget *treeview, char *title, char *value, int id, int row); 
        int AddResultToTree3( GtkWidget *treeview, char *title, char *value, char *sd, int id, int row); 
        void SetIDToCalcArr(int stamp, int row, bool fixed, int id, int section, const char *title, const char *value, const char *gw, const char *edcb);
        bool GetCheckFromCalcArr(int id, const char *title);
#endif 
        void AddAverRes(GtkWidget *fixed, guint *y, const char *title);

//Ìí¼Ó hlx 

	void AddTDRes(GtkWidget *fixed, guint *y, const char *title);
        void AddOBEfwTable(GtkWidget *fixed, guint *y, const char *title, int fetal=FETAL_BY_SET);
        void AddTable(GtkWidget *fixed, guint *y, const char *title, int section, int start, int end, int map_table[][CALC_MAX], int fetal=FETAL_BY_SET);
        int CheckItemType(int i);
        GtkWidget* CreateGeneralTable(int section, int start, int end, int map_table[][CALC_MAX], int map_start);
        GtkWidget* CreateObTable(int start, int end, int map_table[][CALC_MAX], int map_start, int fetal=FETAL_BY_SET);
		void UpdateTotalGrade(int type);
		void AddPSAD(GtkWidget *fixed, guint *y);
		void AddGrowthCurve(GtkWidget *fixed, guint *y);
		void AddBiophysicalProfile(GtkWidget *fixed, guint *y);
		void MapHTable(int table[][CALC_MAX], int currItem, int selectItem, int secStart, int *position, CalcInfoP* ptrInfo);
		void InitMapTable(int table[][CALC_MAX], int start, int end, const SingleItemInfo info[]);
		void InitMapTable(int table[][CALC_MAX], int start, int end, const MultiItemInfo info[]);

		void UpdateGrowthCurve(int flag);
		void DrawGrowthCurve(int ObTable[][3], int StartDate, int MeasureNum[], int GWDate, char sign);
		void GrowthCurveConfigure(GtkWidget *widget, GdkEventConfigure *event);
		void GrowthCurveExpose(GtkWidget *widget, GdkEventExpose *event);
		void GrowthCurveChanged(GtkComboBox *combobox);
		void SpinValueChanged(GtkSpinButton *spin);

		static gboolean HandleGrowthCurveConfigure(GtkWidget *widget, GdkEventConfigure *event, ViewReport *data) 
		{ data->GrowthCurveConfigure(widget, event); return FALSE;}
		static gboolean HandleGrowthCurveExpose(GtkWidget *widget, GdkEventExpose *event, ViewReport *data) 
		{ data->GrowthCurveExpose(widget, event); return FALSE;}
		static void HandleGrowthCurveChanged(GtkComboBox *combobox, ViewReport *data) { data->GrowthCurveChanged(combobox); }
		static void HandleSpinValueChanged(GtkSpinButton *spin, ViewReport *data) { data->SpinValueChanged(spin); }

		GtkWidget* CreateNotePageABD(void);
		GtkWidget* CreateNotePageUR(void);
		GtkWidget* CreateNotePageAE(void);
		GtkWidget* CreateNotePageOB(void);
		GtkWidget* CreateNotePageGYN(void);
		GtkWidget* CreateNotePageSP(void);
		GtkWidget* CreateNotePageVES(void);
		GtkWidget* CreateNotePageFE(void);
		GtkWidget* CreateNotePageTCD(void);
		GtkWidget* CreateNotePageORT(void);
       
#ifdef VET
//hlx
		GtkWidget* CreateNotePageTD(void);
//hlx 
		GtkWidget* CreateNotePageAnOB(void);
#endif


		GtkWidget* CreateSW2NB(GtkWidget *notebook, gint num, const char *title);

		GtkWidget* CreateEditView(void);
		GtkWidget *m_textview_indication;
		GtkTextBuffer *m_buffer_indication;
		GtkWidget *m_textview_comments;
		GtkTextBuffer *m_buffer_comments;

		// signal handle
		void BtnAddImageClicked(GtkButton *button);
		void ChkBtnImageClicked(GtkButton *button);
		void BtnAddOKClicked(GtkButton *button);
		void BtnAddCancelClicked(GtkButton *button);
		void BtnEditOKClicked(GtkButton *button);
		void BtnEditCancelClicked(GtkButton *button);
		void BtnEditClicked(GtkButton *button);
	//	void BtnExportClicked(GtkButton *button);
        void BtnPrintClicked(GtkButton *button);
        void BtnSaveClicked(GtkButton *button);
        void BtnExitClicked(GtkButton *button);
        void EventBoxCalendarPress(GtkWidget *widget, GdkEventButton *event);
        void BtnTransferClicked(GtkButton *button);
        void BtnClearCommentsClicked(GtkButton *button);
        void BtnClearIndicationClicked(GtkButton *button);
        void NotebookChanged(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num);
        void TextIndicationInsert(GtkTextBuffer *textbuffer, GtkTextIter *location, gchar *text, gint len);
        void TextCommentInsert(GtkTextBuffer *textbuffer, GtkTextIter *location, gchar *text, gint len);

		// signal connect
		static void HandleBtnAddImageClicked(GtkButton *button, ViewReport *data) 
		{ data->BtnAddImageClicked(button); }
		static void HandleChkBtnImageClicked(GtkButton *button, ViewReport *data) 
		{ data->ChkBtnImageClicked(button); }
		static void HandleBtnAddOKClicked(GtkButton *button, ViewReport *data) 
		{ data->BtnAddOKClicked(button); }
		static void HandleBtnAddCancelClicked(GtkButton *button, ViewReport *data) 
		{ data->BtnAddCancelClicked(button); }
		static void HandleBtnEditOKClicked(GtkButton *button, ViewReport *data) 
		{ data->BtnEditOKClicked(button); }
		static void HandleBtnEditCancelClicked(GtkButton *button, ViewReport *data) 
		{ data->BtnEditCancelClicked(button); }
		static void HandleBtnEditClicked(GtkButton *button, ViewReport *data) 
		{ data->BtnEditClicked(button); }
	//	static void HandleBtnExportClicked(GtkButton *button, ViewReport *data) 
	//	{ data->BtnExportClicked(button); }
		static void HandleBtnPrintClicked(GtkButton *button, ViewReport *data) 
		{ data->BtnPrintClicked(button); }
	//	static void HandleBtnSaveClicked(GtkButton *button, ViewReport *data) 
	//	{ data->BtnSaveClicked(button); }
		static void HandleBtnExitClicked(GtkButton *button, ViewReport *data) 
		{ data->BtnExitClicked(button); }
		static void HandleEventBoxCalendarPress(GtkWidget *widget, GdkEventButton *event, ViewReport *data) 
		{ data->EventBoxCalendarPress(widget, event); }
		static void HandleBtnTransferClicked(GtkButton *button, ViewReport *data) 
		{ data->BtnTransferClicked(button); }
		static void HandleBtnClearIndicationClicked(GtkButton *button, ViewReport *data) 
		{ data->BtnClearIndicationClicked(button); }
		static void HandleBtnClearCommentsClicked(GtkButton *button, ViewReport *data) 
		{ data->BtnClearCommentsClicked(button); }
		static void HandleNotebookChanged(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, ViewReport *data) 
		{ data->NotebookChanged(notebook, page, page_num); }
		static void HandleTextIndicationInsert(GtkTextBuffer *textbuffer, GtkTextIter *location, gchar *text, gint len, ViewReport *data)
		{ data->TextIndicationInsert(textbuffer, location, text, len); }
		static void HandleTextCommentInsert(GtkTextBuffer *textbuffer, GtkTextIter *location, gchar *text, gint len, ViewReport *data)
		{ data->TextCommentInsert(textbuffer, location, text, len); }

};

inline GtkWidget* ViewReport::GetWindow(void)
{
    return m_window;
}
#endif
