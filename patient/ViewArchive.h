#ifndef VIEWARCHIVE_H
#define VIEWARCHIVE_H

#include <vector>
#include <string>
#include "display/FakeXEvent.h"
#include "patient/Database.h"
#include "AbsCalendarOpr.h"

using std::vector;
using std::string;

class CustomType:public FakeXEvent
{
    public:
        CustomType();
        ~CustomType();
        static CustomType* GetInstance();
        GtkWidget* CreateWin(GtkWidget *parent);
        void DestroyWin(void);
        GtkWidget *GetWindow(){return m_window;};

        void ExportStudy(void);
    private:
        GtkWidget *m_window;
        static CustomType* m_ptrInstance1;
        GtkWidget *m_radiobutton_dicom;
        GtkWidget *m_radiobutton_emp;
        int m_exportNum;

        void DicomRadioToggled(GtkToggleButton *togglebutton);
        void EmpRadioToggled(GtkToggleButton *togglebutton);

        void BtnOkClicked(GtkButton *button);
        void BtnCancelClicked(GtkButton *button);
        gboolean WindowDeleteEvent(GtkWidget *widget, GdkEvent *event);
		void KeyEvent(unsigned char keyValue);

        static void on_dicom_radio_button_toggled(GtkToggleButton *togglebutton, CustomType *data) { data->DicomRadioToggled(togglebutton); }
        static void on_emp_radio_button_toggled(GtkToggleButton *togglebutton, CustomType *data) { data->EmpRadioToggled(togglebutton); }
		static void HandleBtnOkClicked(GtkButton *button, CustomType *data) { data->BtnOkClicked(button); }
		static void HandleBtnCancelClicked(GtkButton *button, CustomType *data) { data->BtnCancelClicked(button); }
        static gboolean on_window_delete_event(GtkWidget *widget, GdkEvent *event, CustomType *data) { return data->WindowDeleteEvent(widget, event); }
        static void PROGRESSSTATUS(int nPos);
};

class ViewArchive:public FakeXEvent, public AbsCalendarOpr
{
	public:
		~ViewArchive();
		static ViewArchive* GetInstance();

		void CreateWindow(void);
		GtkWidget* GetWindow(void);
		void DestroyWindow(void);
		void GetSelToDicom();
		void GetSelToFlash();

		void StartUsbExport(GtkWidget *parent);

     	//vector<string> GetItemFlash() { return vec; }
     	vector<string> GetItemFlash() { return m_vecFlashItem; }
		vector<string> GetSelSid() { return m_vecFlashSid; }
		vector<string> GetSelName() { return m_vecFlashName; }
		vector<string> GetCheckId() { return m_vecCheckId; }

//hlx
#ifdef VET
		enum{
			COL_CHECKED,
			COL_NAME,
			COL_ID,
			COL_OWNER,
			COL_VARIETY,
			COL_SEX,
			COL_BIRTHDAY,
			COL_AGE,
			COL_EXAMDATE,
			COL_EXAMTYPE,
			COL_EXAMID,
		   COL_BACKUPSTATUS,
	      NUM_COLS
		};
#else
		enum{
			COL_CHECKED,
			COL_NAME,
			COL_ID,
			COL_SEX,
			COL_BIRTHDAY,
			COL_AGE,
			COL_EXAMDATE,
			COL_EXAMTYPE,
			COL_EXAMID,
            COL_BACKUPSTATUS,
			NUM_COLS
		};
#endif
		GtkWidget* GetTreeView() { return m_treeview; }
		void TreeSelectionChanged(GtkTreeSelection *selection);
		vector<string> GetSelExamID(void);
        int GetSelNum(void) {return m_selNum;}

		virtual void SetStartDate(int year, int month, int day);
        virtual void SetEndDate(int year, int month, int day);
        string GetExamType();
    private:
		ViewArchive();
		static ViewArchive* m_ptrInstance;
		static const int SNAP_W = 100;
		static const int SNAP_H = 80;

		GCancellable *m_cancelCopy;
		vector<string> m_vecFlashSid, m_vecFlashName;
        vector<string> m_vecCheckId, m_vecFlashItem;

		enum SOURCE
		{
			HARDDISK,
			FLASH
		};
		enum UNIT
		{
			UNIT_KB,
			UNIT_MB
		};

        int m_cond, m_count, m_total;
        int m_iStart,m_iEnd;
        int m_curYear;
        int m_curMonth;
        int m_curDay;

		GtkWidget *m_window;
		GtkWidget *m_treeview;
		GtkWidget *m_entryID;
		GtkWidget *m_entryName;
#ifdef VET
		GtkWidget *m_entryOwnerName;
		GtkWidget *m_entrySpecies;
		GtkWidget *m_comboboxSpecies;

		GtkWidget *m_entryGender;
		GtkWidget *m_comboboxGenderUnit;

#endif

		GtkWidget *m_entryAge;
		GtkWidget *m_comboboxAgeUnit;
		GtkWidget *m_comboboxSex;
		GtkWidget *m_entryYearStart;
		GtkWidget *m_entryMonthStart;
		GtkWidget *m_entryDayStart;
		GtkWidget *m_entryYearEnd;
		GtkWidget *m_entryMonthEnd;
		GtkWidget *m_entryDayEnd;
		GtkWidget *m_menu;
		GtkWidget *m_menu_udisk;
		GtkWidget *m_labelFreeSize;
		GtkWidget *m_labelEndtalSize;
		GtkWidget *m_progressbarSource;
		GtkWidget *m_radiobtnKb;
        GtkWidget *m_radiobtnMb;
        GtkWidget *m_radiobtnGb;

        GtkWidget *comboboxQSearch;

        CustomType m_custom_win;
		vector<string> vec;
        int m_selNum ;
        //	GtkWidget *m_comboboxSource;

		GtkWidget *m_framePreview;
		GtkWidget *m_viewportPreview;
		GtkWidget *m_tablePreview;
		vector<string> m_vecImage;
		vector<GtkWidget*> m_vecItem;
		GtkWidget* CreatePreview(void);
		void ShowPreview(void);
		void HidePreview(void);
		void ClearPreview();
		void LoadPreview(int sid);
		static bool Sort(const string s1, const string s2);
		GtkWidget* CreateImageItem(int sid, int id);

		GtkWidget* create_record_treeview();
		GtkTreeModel* create_record_model();

		//vector<string> GetSelExamID(void);
		void KeyEvent(unsigned char keyValue);
		void DisplaySearchResult(vector<Database::NewPatSearchResult> &Result);
		void GetTimeBefore(int daysBefore, int &year, int &mon, int &day);
		void FillDiskSpace(void);

        void SetProgressBar(double fraction);
		// signal handle
		void EventBoxCalendarStartPress(GtkWidget *widget, GdkEventButton *event);
		void EventBoxCalendarEndPress(GtkWidget *widget, GdkEventButton *event);
		void BtnClearClicked(GtkButton *button);
		void BtnUdiskClicked(GtkButton *button);
		void BtnSearchClicked(GtkButton *button);
		void BtnDisplayClicked(GtkButton *button);
		void BtnQueryRetrieveClicked(GtkButton *button);

		void BtnDeleteClicked(GtkButton *button);
		void BtnSelectAllClicked(GtkButton *button);
		void BtnUnselectAllClicked(GtkButton *button);
		void BtnSendClicked();
		int BtnExportEvent(GtkWidget *widget, GdkEvent *event);
		int BtnImportEvent(GtkWidget *widget, GdkEvent *event);
		void BtnExitClicked(GtkButton *button);
        void ToggleRecord(GtkCellRendererToggle *cell, gchar *path_str);
		void BtnPreviewClicked(GtkButton *button);
        void BirthDateYearInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);
        void BirthDateMonthInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);
        void BirthDateDayInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);
        void BirthDateEndYearInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);
        void BirthDateEndMonthInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);
        void BirthDateEndDayInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);
        void EntryOnlyNumInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);

        void MenuItemFlashActivate(GtkMenuItem *menuitem);
        void MenuItemCDActivate(GtkMenuItem *menuitem);

        void MenuItemEmpActivate(GtkMenuItem *menuitem);
        void MenuItemDicomActivate(GtkMenuItem *menuitem);

		void BtnComboboxQSearch(GtkComboBox *combobox);
	//	void BtnComboboxSource(GtkComboBox *combobox);
	//	void BtnToggleKb(GtkToggleButton *toggleButton);
	//	void BtnToggleMb(GtkToggleButton *toggleButton);
	//	void BtnToggleGb(GtkToggleButton *toggleButton);

		// signal connect
		static void HandleEventBoxCalendarStartPress(GtkWidget *widget, GdkEventButton *event, ViewArchive *data) { data->EventBoxCalendarStartPress(widget, event); }
		static void HandleEventBoxCalendarEndPress(GtkWidget *widget, GdkEventButton *event, ViewArchive *data) { data->EventBoxCalendarEndPress(widget, event); }
	   static void HandleBtnUdiskClicked(GtkButton *button, ViewArchive *data) { data->BtnUdiskClicked(button); }
		static void HandleBtnClearClicked(GtkButton *button, ViewArchive *data) { data->BtnClearClicked(button); }
		static void HandleBtnSearchClicked(GtkButton *button, ViewArchive *data) { data->BtnSearchClicked(button); }
		static void HandleBtnDisplayClicked(GtkButton *button, ViewArchive *data) { data->BtnDisplayClicked(button); }
		static void HandleBtnDeleteClicked(GtkButton *button, ViewArchive *data) { data->BtnDeleteClicked(button); }
		static void HandleBtnSelectAllClicked(GtkButton *button, ViewArchive *data) { data->BtnSelectAllClicked(button); }
		static void HandleBtnUnselectAllClicked(GtkButton *button, ViewArchive *data) { data->BtnUnselectAllClicked(button); }
		static void HandleBtnSendClicked(GtkButton *button, ViewArchive *data) { data->BtnSendClicked(); }
		static void HandleBtnSendDicomClicked(GtkMenuItem *menuitem, gpointer *data) {  ViewArchive::GetInstance()->BtnSendClicked(); }

		static int HandleBtnExportEvent(GtkWidget *widget, GdkEvent *event, ViewArchive *data) { return data->BtnExportEvent(widget, event); }
		static int HandleBtnUdiskEvent(GtkWidget *widget, GdkEvent *event, ViewArchive *data) { return data->BtnImportEvent(widget, event); }
		static void HandleBtnExitClicked(GtkButton *button, ViewArchive *data) { data->BtnExitClicked(button); }
		static void HandleToggleRecord(GtkCellRendererToggle *cell, gchar *path_str, ViewArchive *data) { data->ToggleRecord(cell, path_str);
        }
		static void HandleTreeSelectionChanged(GtkTreeSelection *selection, ViewArchive *data) { data->TreeSelectionChanged(selection); }
		static void HandleBtnPreviewClicked(GtkButton *button, ViewArchive *data) { data->BtnPreviewClicked(button); }

         static void on_entry_birth_date_year(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewArchive *data)
        { data->BirthDateYearInsert(editable, new_text, new_text_length, position); }
        static void on_entry_birth_date_month(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewArchive *data)
        { data->BirthDateMonthInsert(editable, new_text, new_text_length, position); }
        static void on_entry_birth_date_day(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewArchive *data)
        { data->BirthDateDayInsert(editable, new_text, new_text_length, position); }

        static void on_entry_birth_date_end_year(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewArchive *data)
        { data->BirthDateEndYearInsert(editable, new_text, new_text_length, position); }
        static void on_entry_birth_date_end_month(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewArchive *data)
        { data->BirthDateEndMonthInsert(editable, new_text, new_text_length, position); }
        static void on_entry_birth_date_end_day(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewArchive *data)
        { data->BirthDateEndDayInsert(editable, new_text, new_text_length, position); }

        static void HandleEntryOnlyNumInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewArchive *data)
        { data->EntryOnlyNumInsert(editable, new_text, new_text_length, position); }
         static void HandleMenuItemFlashActivate(GtkMenuItem *menuitem, ViewArchive *data)
		{ data->MenuItemFlashActivate(menuitem); }
		static void HandleMenuItemCDActivate(GtkMenuItem *menuitem, ViewArchive *data)
		{ data->MenuItemCDActivate(menuitem); }
static void HandleMenuItemEmpActivate(GtkMenuItem *menuitem, ViewArchive *data)
		{ data->MenuItemEmpActivate(menuitem); }
		static void HandleMenuItemDicomActivate(GtkMenuItem *menuitem, ViewArchive *data)
		{ data->MenuItemDicomActivate(menuitem); }
		static void HandleComboQSearch(GtkComboBox *combobox, ViewArchive *data)
        { data->BtnComboboxQSearch(combobox); }
	//	static void HandleComboSource(GtkComboBox *combobox, ViewArchive *data)
	//	{ data->BtnComboboxSource(combobox); }
		static void HandleBtnToggleKb(GtkToggleButton *toggleButton, ViewArchive *data) { data->FillDiskSpace(); }
		static void HandleBtnToggleMb(GtkToggleButton *toggleButton, ViewArchive *data) { data->FillDiskSpace(); }
		static void HandleBtnToggleGb(GtkToggleButton *toggleButton, ViewArchive *data) { data->FillDiskSpace(); }
#ifndef VET
        static void HandleBtnQueryRetrieveClicked(GtkButton *button, ViewArchive *data) { data->BtnQueryRetrieveClicked(button); }
#endif
        static void PROGRESSSTATUS(int nPos);
};

#endif