#ifndef _DICOM_SERVICE_SETTING_H_
#define _DICOM_SERVICE_SETTING_H_

#include <gtk/gtk.h>
#include "display/FakeXEvent.h"
#include <string.h>
#include <stdio.h>
#include <vector>
#include "periDevice/DCMMan.h"

using std::vector;
class DicomServiceSetting
{
    public:

        static DicomServiceSetting* GetInstance();
        GtkWidget* CreateDicomWindow(GtkWidget *parent);

        enum{
            COL_STORAGE_DEVICE,
            COL_SERVICE_NAME,
            COL_AE_TITLE,
            COL_PORT,
            COL_DEFAULT,
            NUM_COLS
        };

        void init_service_setting();
	void ChangeServiceNoteBook(int page);
    private:
        DicomServiceSetting();
        ~DicomServiceSetting();

        static DicomServiceSetting* m_ptrInstance;

        GtkWidget *m_service_notebook;

        //service
        GtkWidget *m_combobox_device;
        GtkWidget *m_entry_name;
        GtkWidget *m_entry_AE;
        GtkWidget *m_entry_port;
        GtkWidget *m_treeview_storage;
        GtkWidget *m_checkbutton_report;
        GtkWidget *m_checkbutton_video;
        GtkWidget *m_combobox_video_frames;

        GtkWidget *m_combobox_worklist_device;
        GtkWidget *m_entry_worklist_name;
        GtkWidget *m_entry_worklist_AE;
        GtkWidget *m_entry_worklist_port;
        GtkWidget *m_treeview_worklist;
        GtkWidget *m_checkbutton_auto_query;

        //mpps
        GtkWidget *m_combobox_mpps_device;
        GtkWidget *m_entry_mpps_name;
        GtkWidget *m_entry_mpps_AE;
        GtkWidget *m_entry_mpps_port;
        GtkWidget *m_treeview_mpps;
        GtkWidget *m_checkbutton_mpps;

        GtkWidget *m_combobox_storage_commitment_device;
        GtkWidget *m_entry_storage_commitment_name;
        GtkWidget *m_entry_storage_commitment_AE;
        GtkWidget *m_entry_storage_commitment_port;
        GtkWidget *m_treeview_storage_commitment;
        GtkWidget *m_checkbutton_storage_commitment;

        GtkWidget *m_combobox_query_retrieve_device;
        GtkWidget *m_entry_query_retrieve_name;
        GtkWidget *m_entry_query_retrieve_AE;
        GtkWidget *m_entry_query_retrieve_port;
        GtkWidget *m_treeview_query_retrieve;

        int m_page_num;
        bool show_storage;
        bool show_worklist;
        string m_selectedDevice;

        GtkWidget* create_service_treeview();
        GtkTreeModel* create_service_model();

        GtkWidget* Create_note_storage(void);
        GtkWidget* Create_note_worklist(void);
        GtkWidget* Create_note_mpps(void);
        GtkWidget* Create_note_storage_commitment(void);
        GtkWidget* Create_note_query_retrieve(void);

        void init_storage_setting();
        void init_worklist_setting();
        void init_mpps_setting();
        void init_storage_commitment_setting();
        void init_query_retrieve_setting();

        void GetGroupDevice(void);

        void ButtonStorageAddClicked(GtkButton *button);
        void ButtonStorageClearClicked(GtkButton *button);
        void ButtonStorageDeleteClicked(GtkButton *button);
        void ButtonStorageDefaultClicked(GtkButton *button);
        void ButtonStorageConnectClicked(GtkButton *button);
        void ChkBtnSendReportToggled(GtkToggleButton *togglebutton);
        void ChkBtnSendVideoToggled(GtkToggleButton *togglebutton);
        void BtnComboboxVideoFrames(GtkComboBox *combobox);
        void StorageDeviceChanged(GtkComboBox *combobox);

        void ButtonWorkListAddClicked(GtkButton *button);
        void ButtonWorkListClearClicked(GtkButton *button);
        void ButtonWorkListDeleteClicked(GtkButton *button);
        void ButtonWorkListDefaultClicked(GtkButton *button);
        void ButtonWorkListConnectClicked(GtkButton *button);
        void WorklistDeviceChanged(GtkComboBox *combobox);
        void ChkBtnAutoQueryToggled(GtkToggleButton *togglebutton);

        //mpps
        void ButtonMPPSAddClicked(GtkButton *button);
        void ButtonMPPSClearClicked(GtkButton *button);
        void ButtonMPPSDeleteClicked(GtkButton *button);
        void ButtonMPPSDefaultClicked(GtkButton *button);
        void ButtonMPPSConnectClicked(GtkButton *button);
        void MPPSDeviceChanged(GtkComboBox *combobox);
        void ChkBtnMPPSToggled(GtkToggleButton *togglebutton);

        void ButtonStorageCommitmentAddClicked(GtkButton *button);
        void ButtonStorageCommitmentClearClicked(GtkButton *button);
        void ButtonStorageCommitmentDeleteClicked(GtkButton *button);
        void ButtonStorageCommitmentDefaultClicked(GtkButton *button);
        void ButtonStorageCommitmentConnectClicked(GtkButton *button);
        void StorageCommitmentDeviceChanged(GtkComboBox *combobox);
        void ChkBtnStorageCommitmentToggled(GtkToggleButton *togglebutton);

        void ButtonQueryRetrieveAddClicked(GtkButton *button);
        void ButtonQueryRetrieveClearClicked(GtkButton *button);
        void ButtonQueryRetrieveDeleteClicked(GtkButton *button);
        void ButtonQueryRetrieveDefaultClicked(GtkButton *button);
        void ButtonQueryRetrieveConnectClicked(GtkButton *button);
        void QueryRetrieveDeviceChanged(GtkComboBox *combobox);

        void notebookChanged(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num);
        gboolean KeyFilter(GtkWidget *entry, GdkEventKey *event);

        static void HandleButtonStorageAddClicked(GtkButton *button, DicomServiceSetting *data)
        {
            if (data)
                data->ButtonStorageAddClicked(button);
        }

        static void HandleButtonStorageClearClicked(GtkButton *button, DicomServiceSetting *data)
        {
            if (data)
                data->ButtonStorageClearClicked(button);
        }
        static void HandleButtonStorageDeleteClicked(GtkButton *button, DicomServiceSetting *data)
        {
            if (data)
                data->ButtonStorageDeleteClicked(button);
        }
        static void HandleButtonStorageDefaultClicked(GtkButton *button, DicomServiceSetting *data)
        {
            if (data)
                data->ButtonStorageDefaultClicked(button);
        }
        static void HandleButtonStorageConnectClicked(GtkButton *button, DicomServiceSetting *data)
        {
            if (data)
                data->ButtonStorageConnectClicked(button);
        }
        static void HandStorageDeviceChanged(GtkComboBox *combobox,DicomServiceSetting *data)
        {data->StorageDeviceChanged(combobox);}

        static void HandleSendReportToggled(GtkToggleButton *togglebutton, DicomServiceSetting *data) { data->ChkBtnSendReportToggled(togglebutton); }
        static void HandleSendVideoToggled(GtkToggleButton *togglebutton, DicomServiceSetting *data) { data->ChkBtnSendVideoToggled(togglebutton); }
        static void HandleComboVideoFrames(GtkComboBox *combobox, DicomServiceSetting *data)
        { data->BtnComboboxVideoFrames(combobox); }

        static void HandleButtonWorkListAddClicked(GtkButton *button, DicomServiceSetting *data)
        {
            if (data)
                data->ButtonWorkListAddClicked(button);
        }
        static void HandleButtonWorkListClearClicked(GtkButton *button, DicomServiceSetting *data)
        {
            if (data)
                data->ButtonWorkListClearClicked(button);
        }
        static void HandleButtonWorkListDeleteClicked(GtkButton *button, DicomServiceSetting *data)
        {
            if (data)
                data->ButtonWorkListDeleteClicked(button);
        }

        static void HandleButtonWorkListDefaultClicked(GtkButton *button, DicomServiceSetting *data)
        {
            if (data)
                data->ButtonWorkListDefaultClicked(button);
        }
        static void HandleButtonWorkListConnectClicked(GtkButton *button, DicomServiceSetting *data)
        {
            if (data)
                data->ButtonWorkListConnectClicked(button);
        }
        static void HandWorklistDeviceChanged(GtkComboBox *combobox,DicomServiceSetting *data)
        {data->WorklistDeviceChanged(combobox);}

        static void HandleAutoQueryToggled(GtkToggleButton *togglebutton, DicomServiceSetting *data) { data->ChkBtnAutoQueryToggled(togglebutton); }

        //mpps
        static void HandleButtonMPPSAddClicked(GtkButton *button, DicomServiceSetting *data)
        {
            if (data)
                data->ButtonMPPSAddClicked(button);
        }
        static void HandleButtonMPPSClearClicked(GtkButton *button, DicomServiceSetting *data)
        {
            if (data)
                data->ButtonMPPSClearClicked(button);
        }
        static void HandleButtonMPPSDeleteClicked(GtkButton *button, DicomServiceSetting *data)
        {
            if (data)
                data->ButtonMPPSDeleteClicked(button);
        }

        static void HandleButtonMPPSDefaultClicked(GtkButton *button, DicomServiceSetting *data)
        {
            if (data)
                data->ButtonMPPSDefaultClicked(button);
        }
        static void HandleButtonMPPSConnectClicked(GtkButton *button, DicomServiceSetting *data)
        {
            if (data)
                data->ButtonMPPSConnectClicked(button);
        }
        static void HandMPPSDeviceChanged(GtkComboBox *combobox,DicomServiceSetting *data)
        {data->MPPSDeviceChanged(combobox);}

        static void HandleMPPSToggled(GtkToggleButton *togglebutton, DicomServiceSetting *data) { data->ChkBtnMPPSToggled(togglebutton); }

        //storage commitment
        static void HandleButtonStorageCommitmentAddClicked(GtkButton *button, DicomServiceSetting *data)
        {
            if (data)
                data->ButtonStorageCommitmentAddClicked(button);
        }
        static void HandleButtonStorageCommitmentClearClicked(GtkButton *button, DicomServiceSetting *data)
        {
            if (data)
                data->ButtonStorageCommitmentClearClicked(button);
        }
        static void HandleButtonStorageCommitmentDeleteClicked(GtkButton *button, DicomServiceSetting *data)
        {
            if (data)
                data->ButtonStorageCommitmentDeleteClicked(button);
        }

        static void HandleButtonStorageCommitmentDefaultClicked(GtkButton *button, DicomServiceSetting *data)
        {
            if (data)
                data->ButtonStorageCommitmentDefaultClicked(button);
        }
        static void HandleButtonStorageCommitmentConnectClicked(GtkButton *button, DicomServiceSetting *data)
        {
            if (data)
                data->ButtonStorageCommitmentConnectClicked(button);
        }
        static void HandStorageCommitmentDeviceChanged(GtkComboBox *combobox,DicomServiceSetting *data)
        {data->StorageCommitmentDeviceChanged(combobox);}

        static void HandleStorageCommitmentToggled(GtkToggleButton *togglebutton, DicomServiceSetting *data) { data->ChkBtnStorageCommitmentToggled(togglebutton); }

//query retrieve
        static void HandleButtonQueryRetrieveAddClicked(GtkButton *button, DicomServiceSetting *data)
        {
            if (data)
                data->ButtonQueryRetrieveAddClicked(button);
        }
        static void HandleButtonQueryRetrieveClearClicked(GtkButton *button, DicomServiceSetting *data)
        {
            if (data)
                data->ButtonQueryRetrieveClearClicked(button);
        }
        static void HandleButtonQueryRetrieveDeleteClicked(GtkButton *button, DicomServiceSetting *data)
        {
            if (data)
                data->ButtonQueryRetrieveDeleteClicked(button);
        }

        static void HandleButtonQueryRetrieveDefaultClicked(GtkButton *button, DicomServiceSetting *data)
        {
            if (data)
                data->ButtonQueryRetrieveDefaultClicked(button);
        }
        static void HandleButtonQueryRetrieveConnectClicked(GtkButton *button, DicomServiceSetting *data)
        {
            if (data)
                data->ButtonQueryRetrieveConnectClicked(button);
        }

        static void HandQueryRetrieveDeviceChanged(GtkComboBox *combobox,DicomServiceSetting *data)
        {data->QueryRetrieveDeviceChanged(combobox);}

        static void GetSingleServiceAttribute(string device, string serviceName,string aeTitle, int port, bool isDefault,void *data);

        static void on_notebook_switch_page(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, DicomServiceSetting *data)
        { data->notebookChanged(notebook, page, page_num); }

        static gboolean StorageConnectTimeout(gpointer data);
        static gboolean WorklistConnectTimeout(gpointer data);
        static gboolean MPPSConnectTimeout(gpointer data);
        static gboolean StorageCommitmentConnectTimeout(gpointer data);
        static gboolean QueryRetrieveConnectTimeout(gpointer data);

        static gboolean HandleKeyFilter(GtkWidget *entry, GdkEventKey *event, gpointer data)
        {
            ((DicomServiceSetting *)data)->KeyFilter(entry, event);
        }
};
#endif