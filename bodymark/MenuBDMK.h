// -*- c++ -*-
#ifndef MENU_BDMK_H
#define MENU_BDMK_H

#include <string>
#include <vector>

using std::string;
using std::vector;

class MenuBDMK
{
	public:
		MenuBDMK();
		~MenuBDMK() {}
		void Hide(void);
		void Show(void);
		void UpdateLabel(void);

		GtkWidget* Create(void);

	private:
		static const int MAX_IMAGE = 10;//15;
		static const int MAX_ROWS = 5;

		GtkWidget *m_fixed;
		GtkWidget *m_combobox;
		GtkWidget *m_labelPage;
		GtkWidget *m_labelHide;
		GtkWidget *m_btnLeft;
		GtkWidget *m_btnRight;
		GtkWidget *m_btn[MAX_IMAGE];
		gint m_study;
		gint m_numTotal;
		gint m_pageCur;
		GDir *m_dir;
		vector<string> m_vecName;

		GdkPixbuf * m_pixbufSel;

		void SetPageValue(guint page, guint total_page);
		void ChangeBDMKStudy(guchar study);
		void UpdateImage(const char *path, int page);
		void HideAllButton();
		gint GetFileNum(const char *file);
		static bool Sort(const string s1, const string s2);
		gint SortFileName(gconstpointer p1, gconstpointer p2);
		char* GetBdmkPath(const char *item);

		static gint HandleSortFileName(gconstpointer p1, gconstpointer p2, MenuBDMK *data) 
		{ return (data->SortFileName(p1, p2)); }

        gchar* GetFilePath(int study);
		//signal handle 
		void StudyChanged(GtkComboBox *combobox);
		void BDMKClicked(GtkButton *button);
		void HideClicked(GtkButton *button);
		void LeftClicked(GtkButton *button);
		void RightClicked(GtkButton *button);
		
		//signal connect
		static void HandleStudyChanged(GtkComboBox *combobox, MenuBDMK *data) { data->StudyChanged(combobox); }
		static void HandleBDMKClicked(GtkButton *button, MenuBDMK *data) { data->BDMKClicked(button); }
		static void HandleHideClicked(GtkButton *button, MenuBDMK *data) { data->HideClicked(button); }
		static void HandleLeftClicked(GtkButton *button, MenuBDMK *data) { data->LeftClicked(button); }
		static void HandleRightClicked(GtkButton *button, MenuBDMK *data) { data->RightClicked(button); }
};

extern MenuBDMK g_menuBDMK;

#endif
