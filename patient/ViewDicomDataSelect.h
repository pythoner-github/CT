#ifndef _VIEWDICOMDATA_SELECT_H_
#define _VIEWDICOMDATA_SELECT_H_

#include <string>
#include <vector>
#include <gtk/gtk.h>
#include <deque>
#include "FakeXEvent.h"
#include "DCMDef.h"
#include "PatientInfo.h"
#include "Image.h"

using std::string;
using std::vector;
using std::deque;

class ViewDicomDataSelect: public FakeXEvent
{
    public:
        static ViewDicomDataSelect* GetInstance();
        void CreateWindow(GtkWindow *parent);
        GtkWidget* GetWindow() {return m_window;}
        void ImportStudy(void);
        void DestroyWindow(void);
        void GetInfo(PatientInfo::Info &info){ info =m_info;}

    private:
        enum{
		COL_CHECKED,
		COL_NAME,
		NUM_COLS
	};
        enum
        {
            COL_QUALITY,
            COL_VALUE,
            NUM_COL
        };

        ViewDicomDataSelect(void);
        ~ViewDicomDataSelect(void);
        static ViewDicomDataSelect* m_ptrInstance;

        GtkWidget *m_window;
        GtkWidget *m_treeSrc;
        GtkWidget *m_treeDe;
        GList *m_listBranch;
        GtkWidget *m_button_import;
        int m_studyNo;
        int m_studyIndex;
        int m_imageNum;
        string m_srComments;
        vector<string> m_srReferenceImageFilename;
        string m_srIndication;
    
        PatientInfo::Info m_info;

        deque<Image> deq;	///< each frames in deque (input)
        vector<DCMDICOMDIRSTUDYLEVEL> vecStudyLevel;

        static PatientInfo m_patientInfo;
        void UpdateDicomDirList(void);
        void add_column(GtkTreeView *treeview);
        GtkTreeModel* create_root_model(void);

        void CreateExamDir(int examNum);
        GtkWidget* create_treeview_list();
        GtkTreeModel* create_model_list();

        void InsertInfo1(int index);
        void InsertInfo2(int index1,int index2);
        void InsertInfo3(int index1,int index2,int index3);
        gboolean WindowDeleteEvent(GtkWidget *widget, GdkEvent *event);
        void TreeSelectionChanged(GtkTreeSelection *selection);
        void ButtonImportClicked(GtkButton *button);
        void ButtonCancelClicked(GtkButton *button);
#ifndef VET
        void KeyEvent(unsigned char keyValue);
#endif
        static gboolean on_window_delete_event(GtkWidget *widget, GdkEvent *event, ViewDicomDataSelect* data) { return data->WindowDeleteEvent(widget, event); }
        static void HandleTreeSelectionChanged(GtkTreeSelection *selection, ViewDicomDataSelect *data){ data->TreeSelectionChanged(selection);}
        static void HandleButtonImportClicked(GtkButton *button, ViewDicomDataSelect *data)
        {
            if (data)
                data->ButtonImportClicked(button);
        }
        static void HandleButtonCancelClicked(GtkButton *button, ViewDicomDataSelect *data)
        {
            if (data)
                data->ButtonCancelClicked(button);
        }

        static unsigned int  GetDCMStudyElement(DCMSTUDYELEMENT element);
        static string  GetDCMImageElement(DCMIMAGEELEMENT element,unsigned char *pImageBuf, unsigned long bufLen);
       static void  GetDCMSRElement(DCMSRELEMENT element);

        static void PROGRESSSTATUS(int nPos);
};
#endif
