#ifndef __MENU_PROJECT_MODE_H__
#define __MENU_PROJECT_MODE_H__

#include "FakeXEvent.h"
#include <gtk/gtk.h>
#include "ExamItem.h"
#ifdef EMP_PROJECT
using std::vector;
using std::string;

class MenuProjectMode:public FakeXEvent
{
	public:
		MenuProjectMode();
		~MenuProjectMode() {}
        GtkWidget* Create(void);
        bool ShowStatus(void){return m_show;}
		void Hide(void);
		void Show(void);
        void SetMessage(const char *message);
        void AddMessage(const char *message);
        void UpdateLabel(const char *mode = NULL);
        void DestroyDeleteWindow(void);
        void DeleteProjectMode(void);
        void DeleteProjectModeFun(void);
        void SelectProjectMode(GtkButton *button);

    private:
        //signal handle
        void ModeClicked(GtkButton *button);
        static void HandleModeClicked(GtkButton *button, MenuProjectMode *data)
        {
            if(data)data->ModeClicked(button);
        }

        void PageUpClicked(GtkButton *button);
        static void HandlePageUpClicked(GtkButton *button, MenuProjectMode *data)
        {
            if(data)data->PageUpClicked(button);
        }

        void PageDownClicked(GtkButton *button);
        static void HandlePageDownClicked(GtkButton *button, MenuProjectMode *data)
        {
            if(data)data->PageDownClicked(button);
        }

        //void DeleteClicked(GtkButton *button);
        static void HandleDeleteClicked(GtkButton *button, MenuProjectMode *data)
        {
            if(data)data->CreateDeleteWindow();
        }

        static void DefaultClicked(GtkButton *button);
        static void HandleDefaultClicked(GtkButton *button, MenuProjectMode *data)
        {
            if(data)data->DefaultClicked(button);
        }


        void CreateDeleteWindow(void);
        static void HandleBtnCancelClicked(GtkButton *button, MenuProjectMode *data)
        {if(data)data->DestroyDeleteWindow();}
        static void HandleBtnOKClicked(GtkButton *button, MenuProjectMode *data)
        {if(data)data->DeleteProjectMode();if(data)data->DestroyDeleteWindow();}
        static void HandleBtnSelectClicked(GtkButton *button, MenuProjectMode *data)
        {if(data)data->SelectProjectMode(button);}
        static gboolean HandleWindowDeleteEvent(GtkWidget *widget, GdkEvent *event, MenuProjectMode *data)
        {if(data)data->DestroyDeleteWindow();return FALSE;}

        enum { MODECOUNT = 6 };
        GtkWidget *m_fixed;
        GtkWidget *m_button[MODECOUNT];
        GtkWidget *m_messageTextView;
        GtkWidget *m_buttonPageUp;
        GtkWidget *m_buttonPageDown;
        bool m_show;
        vector <string> m_vectorMode;
        string m_curentMode;
        int m_curentPage;
        int m_maxPage;

        bool m_selectAll;
        GtkWidget *m_windowDelete;
        vector <GtkWidget *> m_vectorChectButton;
        vector <string> m_vectorModeDelete;
	string m_messageString;

};

extern MenuProjectMode g_menuProjectMode;
/////////////////////////////////////////////////////////////////////////////////////

class ViewProjectMode:public FakeXEvent
{
	public:
		~ViewProjectMode() {}
        void CreateWindow(void);
        void DestroyWindow(void);
        static ViewProjectMode* GetInstance(void);

    private:
        //signal handle
		ViewProjectMode(){  }
        void SaveProjectModePara(void);
        void KeyEvent(unsigned char keyValue);
        static gboolean HandleWindowDeleteEvent(GtkWidget *widget, GdkEvent *event, ViewProjectMode *data)
        {if(data)data->DestroyWindow();return FALSE;}
        static void ModeClicked(GtkButton *button){ }
        static void HandleModeClicked(GtkButton *button, ViewProjectMode *data)
        {if(data)data->ModeClicked(button);}
        static void HandleBtnCancelClicked(GtkButton *button, ViewProjectMode *data)
        {if(data)data->DestroyWindow();}
        static void HandleBtnSaveClicked(GtkButton *button, ViewProjectMode *data)
        {if(data)data->SaveProjectModePara();}

        void EntryNameInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);
        static void OnEntryNewinsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewProjectMode *data)
    	{ if(data)data->EntryNameInsert(editable, new_text, new_text_length, position); }


        GtkWidget *m_window;
        GtkWidget *m_entryNew;
        GtkWidget *m_comboboxOverload;
        GtkWidget *m_radioButtonNew;
        static ViewProjectMode* m_ptrInstance;
        vector <string> m_vectorModeName;
};
#endif
#endif //__MENU_PROJECT_MODE_H__
