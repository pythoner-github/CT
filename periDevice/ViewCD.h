#ifndef VIEWCD_H
#define VIEWCD_H

#include <vector>
#include <string>
//#include "AbsKeyboardEvent.h"
#include "FakeXEvent.h"

using std::string;
using std::vector;

//class ViewCD:public AbsKeyboardEvent
class ViewCD:public FakeXEvent
{
	public:
		~ViewCD();
        enum {READY, SCAN, EMPTY, OVER, USB_READY, USB_SCAN, USB_EMPTY, USB_OVER};
        static ViewCD* GetInstance();
        //	void CreateWindow(GtkWidget *parent, vector<string> vec);
        void CreateWindow(GtkWidget *parent, vector<string> vec, bool is_cd);
        GtkWidget* GetWindow(void);
        void UpdateSize(bool is_cd);
		void SetBackupStatus(guchar status);
	
		gboolean MakeIsoCallback(GIOChannel *source, GIOCondition condition);
		gboolean BurnCDCallback(GIOChannel *source, GIOCondition condition);
		gboolean BurnDVDCallback(GIOChannel *source, GIOCondition condition);

		vector<string> GetSelFolder() { return m_vecFolder;}
		vector<string> GetSelPath() { return m_vecPath;}

    ///> lhm 2017.05.23
		vector<string> GetSelFolderSrc() { return m_vecFolderNew;}
		vector<string> GetSelPathSrc() { return m_vecPathNew;}

        //> lhm 2017.05.22
        void LoadFlashItem(vector<string> item);

	private:
		ViewCD();
		static ViewCD* m_ptrInstance;

		GtkWidget *m_window;
		GtkWidget *m_treeFrom;
		GtkWidget *m_treeTo;
		GtkWidget *m_entryEstimated;
		GtkWidget *m_entryRemain;
		GtkWidget *m_entryAvail;
		GtkWidget *m_btnBackup;
		GtkWidget *m_statusbar;
		vector<string> m_vecFolder;
		vector<string> m_vecPath;
		vector<string> m_vecFolderNew;
		vector<string> m_vecPathNew;
        vector<string> m_vecTotalId;

        guint64 m_estimated, m_remain, m_available;
        gint m_iscd;	
        vector<string> m_vecFlashItem;

		GtkTreeModel* create_from_model(vector<string> vec);
		void add_column(GtkTreeView *treeview);
		void KeyEvent(unsigned char keyValue);
		void AppendParent(const char* parent, vector<string> child);
		void AppendChild(const char* parent, const char* child);
		void SetEstimatedSize(float size);
		void SetRemainSize(float size);
		void SetAvailSize(float size);
		void HintDialog(guint timeout, int type, const char *info);
		void UpdateStatusBar(const char* msg);

		gboolean PrepareData(vector<string> folder, vector<string> path);
		gboolean MakeIso();
		gboolean BurnCD(const char *device);
		gboolean BurnDVD(gboolean is_blank, const char *device);

		// signal handle
		void DestroyWindow();
		void BtnAddClicked(GtkButton *button);
		void BtnAddAllClicked(GtkButton *button);
		void BtnDelClicked(GtkButton *button);
		void BtnDelAllClicked(GtkButton *button);
		void BtnBackupClicked(GtkButton *button);
		void BtnExportClicked(GtkButton *button);
		void BtnEraseClicked(GtkButton *button);
		void BtnExitClicked(GtkButton *button);
        gboolean WindowDeleteEvent(GtkWidget *widget, GdkEvent *event);
		// signal connect
		static void HandleBtnAddClicked(GtkButton *button, ViewCD *data) { data->BtnAddClicked(button); }
		static void HandleBtnAddAllClicked(GtkButton *button, ViewCD *data) { data->BtnAddAllClicked(button); }
		static void HandleBtnDelClicked(GtkButton *button, ViewCD *data) { data->BtnDelClicked(button); }
		static void HandleBtnDelAllClicked(GtkButton *button, ViewCD *data) { data->BtnDelAllClicked(button); }
		static void HandleBtnBackupClicked(GtkButton *button, ViewCD *data) { data->BtnBackupClicked(button); }
		static void HandleBtnExportClicked(GtkButton *button, ViewCD *data) { data->BtnExportClicked(button); }
		static void HandleBtnEraseClicked(GtkButton *button, ViewCD *data) { data->BtnEraseClicked(button); }
		static void HandleBtnExitClicked(GtkButton *button, ViewCD *data) { data->BtnExitClicked(button); }
        static gboolean on_window_delete_event(GtkWidget *widget, GdkEvent *event, ViewCD *data) { return data->WindowDeleteEvent(widget, event); }
	//	static gboolean HandleCallbackMakeIso(GIOChannel *source, GIOCondition condition, ViewCD *data)
	//	{ return data->MakeIsoCallback(source, condition); }
};


#endif
