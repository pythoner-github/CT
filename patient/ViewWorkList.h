#ifndef _VIEW_WORKLIST_H_
#define _VIEW_WORKLIST_H_

#include <gtk/gtk.h>
#include "FakeXEvent.h"
#include <vector>
#include "DCMDef.h"
#include "../include/AbsCalendarOpr.h"
#include "PatientInfo.h"

class PatDetail:public FakeXEvent
{
    public:
        PatDetail();
        ~PatDetail();

        //static PatDetail* GetInstance();
        
        GtkWidget* CreatePatDetailWin(GtkWidget *parent);
        void DestroyPatDetailWin(void);
        GtkWidget *GetWindow(){return m_window_detail;}

        void InsertPatDetail(const char *ID,const char *name,string birthDate,string age,const char *sex,const char *size,const char *weight,const char *address,const char *doctor,const char *description,const char *requestedID,const char *accessionNumber,string scheduleDate);

        void ClearData();

    private:
       // static PatDetail* m_ptrInstance;
        GtkWidget *m_window_detail;
        GtkWidget *m_entry_ID;
        GtkWidget *m_entry_name;
        GtkWidget *m_entry_accession;
        GtkWidget *m_entry_birth_date;
        GtkWidget *m_entry_age;
        GtkWidget *m_entry_request;
        GtkWidget *m_entry_sex;
        GtkWidget *m_entry_date;
        GtkWidget *m_entry_doctor;
        GtkWidget *m_entry_stature;
        GtkWidget *m_entry_weight;
        GtkWidget *m_entry_address;
        GtkWidget *m_textview_comment;
        
        void ButtonQuitClicked(GtkButton *button);
       // KeyEvent(unsigned char keyValue);

        static void HandleButtonQuitClicked(GtkButton *button, PatDetail *data)
        {
            if (data)
              data->ButtonQuitClicked(button);
        }

        void KeyEvent(unsigned char keyValue);

};

class ViewWorkList:public FakeXEvent, public AbsCalendarOpr

{
    public:
        ViewWorkList();
        ~ViewWorkList();

        enum{
            COL_SCHEDULED_DATE,
            COL_PATIENT_ID,
            COL_NAME,
            COL_Gender,
            COL_ACCESSION_NUMBER,
            COL_BIRTH_DATE, 
            COL_EXAM_DESCRIPTION,
            NUM_COLS
        };
        
        GtkWidget *m_win_parent;
        static ViewWorkList* GetInstance();

        vector<DCMWORKLISTELEMENT> m_query;
        GtkWidget* CreateWorkListWin(GtkWidget *parent);
        GtkWidget *GetWindow(){return m_window;}
        
        int GetDateFormat(){return m_dateFormat;};
        void GetAndDisplayPatientRecord();
        void DestroyWin(void);
        void DotTransComma(char *Comma, unsigned int size);
        void GetSelectedPatInfo();
        int GetWorkListQuery(void);
        DCMSTUDYELEMENT EditStudyInfo(PatientInfo::Info &info);
        DCMMPPSELEMENT GetMPPSElement(PatientInfo::Info &info);
        void InserPatInfo();
        void ClearQueryRes(void);
        void ImportPatInfo();
    private:
        static ViewWorkList* m_ptrInstance;

        PatDetail m_patDetail;

        GtkWidget *m_fixed;
        GtkWidget* m_window;
        GtkWidget* m_entry_ID;
        GtkWidget* m_entry_name;
        GtkWidget* m_entry_accession;
        GtkWidget* m_entry_request;
        GtkWidget* m_entry_date1;
        GtkWidget* m_entry_date2;
        GtkWidget* m_treeview_worklist;

        GtkWidget* m_label_record;

        int m_dateFormat;
        char startDate[256];
        char endDate[256];
        bool m_autoQueryFlag;
        gint m_timeout;
        int selectedIndex;

        //vector<DCMWORKLISTELEMENT> m_query;
        vector<DCMWORKLISTELEMENT> m_autoQuery;

        GtkWidget* create_worklist_treeview();
        GtkTreeModel* create_worklist_model();


        void InitScheduledDate();

        void LoadPatientInfo();
        void DisplayPatientRecord();
        bool ArchiveWorkListPatInfo(bool startExam);

        void AutoQuery();

        void SetStartDate(int year, int month, int day);
        void SetEndDate(int year, int month, int day);

		void ChangePersonNameFormat(string wlPersonName, string &firstName, string &lastName, string &midName);
		void ChangePersonNameFormatForShow(string wlPersonName, string &name);

        void ButtonQueryClicked(GtkButton *button);
        void ButtonClearClicked(GtkButton *button);
        void ButtonNewExamClicked(GtkButton *button);
        void ButtonDetailClicked(GtkButton *button);
        void ButtonTransferClicked(GtkButton *button);
        void ButtonQuitClicked(GtkButton *button);
        void CloseWindow(GtkWidget *window);

        void StartDateInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);
        void StartDateDelete(GtkEditable *editable, gint start_pos, gint end_pos);
    
        void StartCalendarPress(GtkWidget *widget, GdkEventButton *event);
        void EndCalendarPress(GtkWidget *widget, GdkEventButton *event);

        
       // KeyEvent(unsigned char keyValue);
        //signal
        static void on_entry_insert_start_date(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewWorkList *data)
        { data->StartDateInsert(editable, new_text, new_text_length, position); }

        static void on_entry_delete_start_date(GtkEditable *editable, gint start_pos, gint end_pos, ViewWorkList *data)
        { data->StartDateDelete(editable, start_pos, end_pos); }

        static void HandleButtonQueryClicked(GtkButton *button, ViewWorkList *data)
        {
            if (data)
                data->ButtonQueryClicked(button);
        }
        static void HandleButtonClearClicked(GtkButton *button, ViewWorkList *data)
        {
            if (data)
                data->ButtonClearClicked(button);
        }
        static void HandleButtonNewExamClicked(GtkButton *button, ViewWorkList *data)
        {
            if (data)
              data->ButtonNewExamClicked(button);
        }
        static void HandleButtonDetailClicked(GtkButton *button, ViewWorkList *data)
        {
            if (data)
              data->ButtonDetailClicked(button);
        }
        static void HandleButtonTransferClicked(GtkButton *button, ViewWorkList *data)
        {
            if (data)
              data->ButtonTransferClicked(button);
        }
        static void HandleButtonQuitClicked(GtkButton *button, ViewWorkList *data)
        {
            if (data)
              data->ButtonQuitClicked(button);
        }

        static void HandleCloseWindow(GtkWidget *window,ViewWorkList *data)
        {
            if(data)
                data->CloseWindow(window);
        }

	
        static void HandleStartCalendarPress(GtkWidget *widget, GdkEventButton *event, ViewWorkList *data) { data->StartCalendarPress(widget, event); }
        
        static void HandleEndCalendarPress(GtkWidget *widget, GdkEventButton *event, ViewWorkList *data) { data->EndCalendarPress(widget, event); }

        void KeyEvent(unsigned char keyValue);
};
#endif
