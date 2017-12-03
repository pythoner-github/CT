#ifndef _VIEW_QUERY_RETRIEVE_H_
#define _VIEW_QUERY_RETRIEVE_H_

#include <deque>
#include <gtk/gtk.h>
#include "FakeXEvent.h"
#include "../include/AbsCalendarOpr.h"
#include <vector>
#include "DCMDef.h"
#include "ImageArea.h"
#include "Image.h"
#include "PatientInfo.h"

using std::deque;
class ViewQueryRetrieve:public FakeXEvent,public AbsCalendarOpr
{
    public:
        ViewQueryRetrieve();
        ~ViewQueryRetrieve();

        enum{
            COL_CHECKED_S,
            COL_PATIENT_ID_S,
            COL_NAME_S,
            COL_Gender_S,
            COL_ACCESSION_NUMBER_S,
            COL_BIRTH_DATE_S, 
            COL_EXAM_DATE_S,
            COL_EXAM_DESCRIPTION_S,
            NUM_COLS_S
        };
        
        enum{
            COL_PATIENT_ID_D,
            COL_NAME_D,
            COL_Gender_D,
            COL_AGE_D,
            COL_SIZE_D,
            COL_WEIGHT_D,
            COL_ACCESSION_NUMBER_D,
            COL_BIRTH_DATE_D, 
            COL_EXAM_DATE_D,
            COL_EXAM_TIME_D,
            COL_EXAM_DOCTOR_D,
            COL_EXAM_DESCRIPTION_D,
            NUM_COLS_D
        };

        static ViewQueryRetrieve* GetInstance();

        GtkWidget* CreateQueryRetrieveWin(GtkWidget *parent);

        void DestroyWin(void);
        void GetInfo(PatientInfo::Info &info){ info =m_info;}
        void SetRetrieveFlag(bool retrieveFlag);
        bool GetRetrieveFlag(void);
        //static unsigned int  GetDCMStudyElement(DCMSTUDYELEMENT element);
        //static string  GetDCMImageElement(DCMIMAGEELEMENT element,unsigned char *pImageBuf, unsigned long bufLen);
       //static void  GetDCMSRElement(DCMSRELEMENT element);
		static void ChangePersonNameFormat(string wlPersonName, string &firstName, string &lastName, string &midName);
		static void ChangePersonNameFormatForShow(string wlPersonName, string &name);
    private:
        static ViewQueryRetrieve* m_ptrInstance;
        GtkWidget* m_window;
        GtkWidget* m_entry_ID;
        GtkWidget* m_entry_name;
        GtkWidget* m_entry_accession;
        GtkWidget* m_entry_exam1;
        GtkWidget* m_entry_exam2;
        GtkWidget* m_treeview_source;
        GtkWidget* m_treeview_detination;
        GtkWidget* m_checkbutton_select_all;
        GtkWidget* m_combobox_device;
        GtkWidget *m_button_retrieve; 
        GtkWidget*  m_progress_bar;

        int m_dateFormat;
        char startDate[256];
        char endDate[256];
        int m_studyNo;
        bool m_isSelect;
        int m_itemIndex;
        int m_imageNum; 
        bool m_retrieveFlag;
        unsigned char m_videoBit[IMG_AREA_W*IMG_AREA_H*IMG_BPP];
        static PatientInfo m_patientInfo;
        string m_srComments;
        vector<string> m_srReferenceImageFilename;
        string m_srIndication;
        int m_count_success;
        int m_count_failed;


        PatientInfo::Info m_info;
        vector<DCMQRQUERYRES> QueryRes;
        vector<int> vecIndex;
        deque<Image> deq;	///< each frames in deque (input)
        GtkWidget* create_query_treeview();
        GtkTreeModel* create_query_model();
        GtkWidget* create_retrieve_treeview();
        GtkTreeModel* create_retrieve_model();
       
        void ToggleRecord(GtkCellRendererToggle *cell, gchar *path_str);
        void ButtonQueryClicked(GtkButton *button);
        void ButtonClearClicked(GtkButton *button);
        void ButtonQuitClicked(GtkButton *button);
        void ButtonRetrieveClicked(GtkButton *button);
        void ChkSelectAllToggled(GtkToggleButton *togglebutton);
        void CreateExamDir(int examNum);

        void StartCalendarPress(GtkWidget *widget, GdkEventButton *event);
        void EndCalendarPress(GtkWidget *widget, GdkEventButton *event);
#ifndef VET

        void SetStartDate(int year, int month, int day);
        void SetEndDate(int year, int month, int day);
#endif
        void InsertPatInfo(int index ,string device,int num);
        void SetProgressBar(double fraction);
        static void HandleButtonQueryClicked(GtkButton *button, ViewQueryRetrieve *data)
        {
            if (data)
                data->ButtonQueryClicked(button);
        }
        static void HandleButtonClearClicked(GtkButton *button, ViewQueryRetrieve *data)
        {
            if (data)
                data->ButtonClearClicked(button);
        }
        
        static void HandleButtonRetrieveClicked(GtkButton *button, ViewQueryRetrieve *data)
        {
            if (data)
              data->ButtonRetrieveClicked(button);
        }

        static void HandleButtonQuitClicked(GtkButton *button, ViewQueryRetrieve *data)
        {
            if (data)
                data->ButtonQuitClicked(button);
        }
        
        static void HandleStartCalendarPress(GtkWidget *widget, GdkEventButton *event, ViewQueryRetrieve *data) { data->StartCalendarPress(widget, event); }
        
        static void HandleEndCalendarPress(GtkWidget *widget, GdkEventButton *event, ViewQueryRetrieve *data) { data->EndCalendarPress(widget, event); }
        
		static void HandleToggleRecord(GtkCellRendererToggle *cell, gchar *path_str, ViewQueryRetrieve *data) { data->ToggleRecord(cell, path_str); 
        }
        static void HandleSelectAllToggled(GtkToggleButton *togglebutton, ViewQueryRetrieve *data) { data->ChkSelectAllToggled(togglebutton); }
        static unsigned int  GetDCMStudyElement(DCMSTUDYELEMENT element);
        static string  GetDCMImageElement(DCMIMAGEELEMENT element,unsigned char *pImageBuf, unsigned long bufLen);
       static void  GetDCMSRElement(DCMSRELEMENT element);
       static void SetProgressPos(int nPos);
#ifndef VET
       void KeyEvent(unsigned char keyValue);
#endif
};
#endif
