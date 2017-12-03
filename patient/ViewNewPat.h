// -*- c++ -*-
#ifndef VIEW_NEW_PAT_H
#define VIEW_NEW_PAT_H

#include "FakeXEvent.h"
#include "../include/AbsCalendarOpr.h"
#include "PatientInfo.h"
#include "DCMDef.h"

class TopArea;

class ViewNewPat:public FakeXEvent, public AbsCalendarOpr
{
public:
    ~ViewNewPat();
    static ViewNewPat* GetInstance();
    GtkWidget* GetWindow(void);
    void FillNewPat(const PatientInfo::Info &info);
    void SetSeneitive(bool sensitive);
    void FillPatInfo(const PatientInfo::Patient &pat_info);
    void GetPatInfo(PatientInfo::Info &info);
    void CreateWindow(void);
    void DestroyWindow(void);
    virtual void SetStartDate(int year, int month, int day);
    void ClearData(void);
    void ClearExamData(void);
    bool GetClearStatus(void) { return m_clearAll; }
    void LoadPatData(void);
    void AutoPatID(void);
    void UpdateNameLength();
 bool IsErrorChar(gchar *new_text){return (0 == strcmp(new_text, "'"));}
    void InsertPatientInfo(const char *ID, PatientInfo::Name patientName, string birthDate,string age,const char *sex,const char *size,const char *weight,const char *address,const char *doctor,const char *description);

    void SetStudyInfo(DCMWORKLISTELEMENT element);
    void SetStudyInfo(DCMSTUDYELEMENT element);
    void UpdateStudyInfo(void);
    void ClearStudyInfo(void);
    bool GetMPPSFlag(void){return m_flagMPPS;}
    void SetMPPSFlag(bool flag){ m_flagMPPS = flag;}
private:
    ViewNewPat();
    GtkWidget* create_note_general(void);
    GtkWidget* create_note_ob(void);
    GtkWidget* create_note_card(void);
    GtkWidget* create_note_uro(void);
    GtkWidget* create_note_other(void);
    bool AutoCalcAge(const int year, const int month, const int day);
    void KeyEvent(unsigned char keyValue);
    void CalcBSA(void);
    std::string GenPatID(void);
    void Calc_GA_EDD(const gchar *year, const gchar *month, const gchar *day);
    void CheckPatID(const gchar *pat_id);
    void FillExamInfo(const PatientInfo::ExamGeneral &exam_info);
    void FillObInfo(const PatientInfo::ObExam &ob);
    void FillCarInfo(const PatientInfo::CarExam &car);
    void FillUroInfo(const PatientInfo::UroExam &uro);

    bool m_langCN;
    int m_curYear;
    int m_curMonth;
    int m_curDay;
    bool m_clearAll;            // clear all or examinfo
    GtkWidget *m_button_worklist;
    GtkWidget *m_warming_label;


    DCMSTUDYELEMENT m_studyInfo;
    static ViewNewPat* m_ptrInstance;
    GtkWidget *m_window;
    GtkWidget *m_entryPatID;
    GtkWidget *m_checkbuttonPatID;
#ifdef VET
    GtkWidget *m_entryAnimalName;
    //GtkWidget *m_entryOwnerName;
    GtkWidget *m_comboboxSpecies;
    GtkWidget *m_comboboxOwnerName;
    GtkListStore *m_list_store;
#else
    GtkWidget *m_entryNameFirst;
    GtkWidget *m_entryNameMid;
    GtkWidget *m_entryNameLast;
#endif
    
    GtkWidget *m_entryAge;
    GtkWidget *m_entryBirthYear;
    GtkWidget *m_entryBirthMonth;
    GtkWidget *m_entryBirthDay;
    GtkWidget *m_comboboxGender;
    GtkWidget *m_comboboxAge;
//    GtkWidget *m_labelAgeUnit;
    GtkWidget *m_textview_comment;
    GtkWidget *m_comboboxentry_diagnostician;
    GtkWidget *m_comboboxentry_physician;
// general
    GtkWidget *m_entry_stature;
    GtkWidget *m_entry_weight;
    GtkWidget *m_entry_BSA;
// ob
    GtkWidget *m_entry_ob_year;
    GtkWidget *m_entry_ob_month;
    GtkWidget *m_entry_ob_day;
    GtkWidget *m_entry_ob_GA;
    GtkWidget *m_entry_ob_EDD;
    GtkWidget *m_entry_ob_gravida;
    GtkWidget *m_entry_ob_ectopic;
    GtkWidget *m_entry_ob_gestations;
    GtkWidget *m_entry_ob_para;
    GtkWidget *m_entry_ob_aborta;
    GtkWidget *m_combobox_ob_date;
// card
    GtkWidget *m_entry_hr;
    GtkWidget *m_entry_bp_high;
    GtkWidget *m_entry_bp_low;
// URO
    GtkWidget *m_entry_uro_psa;
// Other
    GtkWidget *m_entry_other_tel;
    GtkWidget *m_entry_other_address;

     static const int m_pat_name_max_len = 31;
     int m_pat_name_first;
     int m_pat_name_middle;
     int m_pat_name_last;
    
    bool m_flagMPPS;
    int m_date_format;

    DCMMPPSELEMENT GetMPPSElement(PatientInfo::Info &info);
    void EditStudyInfo(PatientInfo::Info &info);
    // signal handle
    gboolean WindowDeleteEvent(GtkWidget *widget, GdkEvent *event);
    void ChkBtnPatIDClicked(GtkButton *button);
    void BtnSearchClicked(GtkButton *button);
    void BtnOkClicked(GtkButton *button);
    void BtnCancelClicked(GtkButton *button);
//    void BtnExamPauseClicked(GtkButton *button);
    void BtnExamEndClicked(GtkButton *button);
    void BtnNewPatClicked(GtkButton *button);
    void BtnNewExamClicked(GtkButton *button);
    
    void BtnWorkListClicked(GtkButton *button);
    void CommentInsert(GtkTextBuffer *textbuffer, GtkTextIter *location, gchar *text, gint len);
    const gchar* GetLMP(void);
    void BirthDateYearInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);
    void BirthDateYearDelete(GtkEditable *editable, gint start_pos, gint end_pos);
    void BirthDateMonthInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);
    void BirthDateMonthDelete(GtkEditable *editable, gint start_pos, gint end_pos);
    void BirthDateDayInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);
    void BirthDateDayDelete(GtkEditable *editable, gint start_pos, gint end_pos);
    void EntryNumInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);
    void EntryAlNumInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);
    void EntryNameInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);
	//void EntryTextInsert(GtkTextBuffer *textbuffer, GtkTextIter *location, gchar *text, gint len);

#ifdef VET
    void EntryOwnerNameInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);
    void EntryOwnerNameDelete(GtkEditable *editable, gint start_pos, gint end_pos);
    void EntryOwnerNameChanged(GtkComboBox *combox_entry);
    void DoneOwnerName(string name);
#endif
    void EventBoxCalendarPress(GtkWidget *widget, GdkEventButton *event);
    void ComboboxOBDateChanged(GtkComboBox *widget);
    void PatIDFocusOut(GtkWidget *widget, GdkEventFocus *event);
    void OBFocusOut(GtkWidget *widget, GdkEventFocus *event);
    void StatureFocusOut(GtkWidget *widget, GdkEventFocus *event);
    void WeightFocusOut(GtkWidget *widget, GdkEventFocus *event);

// signal connect
    static gboolean on_window_delete_event(GtkWidget *widget, GdkEvent *event, ViewNewPat *data) { return data->WindowDeleteEvent(widget, event); }
    static void on_chkbutton_pad_id(GtkButton *button, ViewNewPat *data) { data->ChkBtnPatIDClicked(button); }
    static void on_button_search_clicked(GtkButton *button, ViewNewPat *data) { data->BtnSearchClicked(button); }
    static void on_button_ok_clicked(GtkButton *button, ViewNewPat *data) { data->BtnOkClicked(button); }
    static void on_button_cancel_clicked(GtkButton *button, ViewNewPat *data) { data->BtnCancelClicked(button); }
//    static void on_button_exam_pause_clicked(GtkButton *button, ViewNewPat *data) { data->BtnExamPauseClicked(button); }
    static void on_button_exam_end_clicked(GtkButton *button, ViewNewPat *data) { data->BtnExamEndClicked(button); }
    static void on_button_new_pat_clicked(GtkButton *button, ViewNewPat *data) { data->BtnNewPatClicked(button); }
    static void on_button_new_exam_clicked(GtkButton *button, ViewNewPat *data) { data->BtnNewExamClicked(button); }
#ifdef VET
    static void HandleOwnerNameChanged(GtkComboBox *combox_entry, ViewNewPat *data)
       { data->EntryOwnerNameChanged(combox_entry);}
    static void HandleOwnerNameInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewNewPat *data) 
       { data->EntryOwnerNameInsert(editable, new_text, new_text_length, position);}
    static void HandleOwnerNameDelete(GtkEditable *editable,gint start_pos, gint end_pos, ViewNewPat *data) 
       { data->EntryOwnerNameDelete(editable, start_pos, end_pos);}
#endif
    static void on_button_worklist_clicked(GtkButton *button, ViewNewPat *data) { data->BtnWorkListClicked(button); }

    static void on_entry_pat_id(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewNewPat *data) 
	{ data->EntryAlNumInsert(editable, new_text, new_text_length, position); }
    static void on_entry_name_insert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewNewPat *data)
	{ data->EntryNameInsert(editable, new_text, new_text_length, position); }
    static void on_entry_age(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewNewPat *data) 
	{ data->EntryNumInsert(editable, new_text, new_text_length, position); }
    static gboolean HandlePatIDFocusOut(GtkWidget *widget, GdkEventFocus *event, ViewNewPat *data)
	{ data->PatIDFocusOut(widget, event); return FALSE;}
    static gboolean HandleOBFocusOut(GtkWidget *widget, GdkEventFocus *event, ViewNewPat *data)
	{ data->OBFocusOut(widget, event); return FALSE;}
    static gboolean HandleStatureFocusOut(GtkWidget *widget, GdkEventFocus *event, ViewNewPat *data)
	{ data->StatureFocusOut(widget, event); return FALSE;}
    static gboolean HandleWeightFocusOut(GtkWidget *widget, GdkEventFocus *event, ViewNewPat *data)
	{ data->WeightFocusOut(widget, event); return FALSE;}

//    static void on_entry_age(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewNewPat *data)
//	{ data->EntryNumInsert(editable, new_text, new_text_length, position); }

    static void on_textview_comment_insert(GtkTextBuffer *textbuffer, GtkTextIter *location, gchar *text, gint len, ViewNewPat *data)
    { 
        data->CommentInsert(textbuffer, location, text, len);
    }
    static void on_entry_birth_date_year(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewNewPat *data)
        { data->BirthDateYearInsert(editable, new_text, new_text_length, position); }
    static void on_entry_delete_birth_date_year(GtkEditable *editable, gint start_pos, gint end_pos, ViewNewPat *data)
        { data->BirthDateYearDelete(editable, start_pos, end_pos); }
    static void on_entry_birth_date_month(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewNewPat *data)
	{ data->BirthDateMonthInsert(editable, new_text, new_text_length, position); }
    static void on_entry_delete_birth_date_month(GtkEditable *editable, gint start_pos, gint end_pos, ViewNewPat *data)
        { data->BirthDateMonthDelete(editable, start_pos, end_pos); }
    static void on_entry_birth_date_day(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewNewPat *data)
	{ data->BirthDateDayInsert(editable, new_text, new_text_length, position); }
    static void on_entry_delete_birth_date_day(GtkEditable *editable, gint start_pos, gint end_pos, ViewNewPat *data)
        { data->BirthDateDayDelete(editable, start_pos, end_pos); }

    static void on_entry_stature(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewNewPat *data)
	{ data->EntryNumInsert(editable, new_text, new_text_length, position); }
    static void on_entry_weight(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewNewPat *data)
	{ data->EntryNumInsert(editable, new_text, new_text_length, position); }

    static void on_combobox_ob_date_changed(GtkComboBox *widget, ViewNewPat *data)
        { data->ComboboxOBDateChanged(widget); }
    static void on_entry_ob_year(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewNewPat *data)
	{ data->EntryNumInsert(editable, new_text, new_text_length, position); }
    static void on_entry_ob_month(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewNewPat *data)
	{ data->EntryNumInsert(editable, new_text, new_text_length, position); }
    static void on_entry_ob_day(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewNewPat *data)
	{ data->EntryNumInsert(editable, new_text, new_text_length, position); }

    static void on_entry_hr(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewNewPat *data)
	{ data->EntryNumInsert(editable, new_text, new_text_length, position); }
	static void HandleEventBoxCalendarPress(GtkWidget *widget, GdkEventButton *event, ViewNewPat *data) { data->EventBoxCalendarPress(widget, event); }

};

inline GtkWidget* ViewNewPat::GetWindow(void)
{
    return m_window;
}

#endif
