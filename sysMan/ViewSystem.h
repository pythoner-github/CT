// -*- c++ -*-
#ifndef VIEW_SYSTEM_H
#define VIEW_SYSTEM_H
#include <gtk/gtk.h>

#include "display/FakeXEvent.h"
#include <string>
#include <vector>
#include "probe/ExamItem.h"
#include "sysMan/CalcSetting.h"
#define NUM_KB 10
#include <sqlite3.h>
//typedef void (*pSetExamPara)(void);
using namespace std;
#define MAXNUMBER 100

class SysCalculateSetting;
class SysNoteSetting;
class SysGeneralSetting;
class SysMeasurementSetting;
class SysOptions;

typedef struct
{
    std::string brand;
    std::string model;
} common_printer;

struct ExamPara
{
    string dept_name;
    string name;
    ExamItem::EItem index;
};
enum {EN, ZH, RU, PL,FR,ES, DE, LANG_MAX};

extern char user_configure[USERCONFIG_LEN];
extern FILE *fp_user;

int _system_(const char *cmd);

class ViewSystem : public FakeXEvent
{
public:
    ~ViewSystem();

#if defined(EMP_322)
		static const int MAX_KEY = 6;
#else
		static const int MAX_KEY = 10;//8;//9;
#endif

        static ViewSystem* GetInstance();
        void CreateWindow(void);
        void DestroyWindow(void);
        GtkWidget* GetWindow(void);
        void DoDicomTest();
        int GetProbeType(void);
        int GetCommentProbeType(void);
        int GetKbType(void);
        void UpdateHospitalandpart(int date_format, const char* hospital_name);
        const gchar* GetUserName(void);
        void ShowList(const char *name);
        void set_active_user(gint num);
        int get_active_user(void);
        void save_itemIndex(int itemIndex);
        int  get_itemIndex(void);
        void save_itemName(char *itemName);
        GtkWidget* GetVGACombobox();

    void get_itemName(char *itemName);
    //char *get_itemName(void);

    void SetImagePara(const ExamItem::ParaItem &item);
    void GetImagePara(ExamItem::ParaItem &item);
    void save_image_para(ExamItem::ParaItem &item, char *itemName);
    void update_specific_printer_model(void);
    //void GetItemListOfKb(char* KbModel, vector<enum PItem> *ptrItemList);
    void tree_auto_scroll(GtkTreeView *tree_view, GtkTreeIter *iter, GtkTreePath *path, gpointer user_data);
    void CreateDefineItemFormUsbToHost(char *name);
    void ChangeNoteBookPage(int page);
	void ChangeDicomNoteBookPage(int page);

    ////>option fuction
    void UpdateOptionStatus(bool status);
#ifdef VET
    void UpdateExamName(int probe_index);
#endif
#if 1 //add by jhuang
    bool CheckFlagFromReportTemplet(int id);
#endif
    void UpdateUserItem(void);

private:
    ViewSystem();
    void KeyEvent(unsigned char keyValue);
    int m_kbIndex;
	bool m_vgaInterl;
    GtkWidget *m_combobox_vga;
    //PItem m_itemIndex;
   // vector<enum PItem> m_vecItemIndex[NUM_KB];

    enum {
	PRT_BRAND,
	PRT_MODEL,
 	CNUM_COLS
    };

    enum {
        PRT_DEFAULT,
        PRT_NAME,
        PRT_STATUS,
        PRT_MESSAGE,
        SNUM_COLS
    };

    enum {
        NAME_COLUMN,
        INDEX_COLUMN,
        N_COLUMNS
    };

    static ViewSystem* m_ptrInstance;
    bool m_imageItemSensitive;
    int m_itemIndex;
    char *m_itemName;
    GtkCellRenderer *m_renderer1;
    GtkWidget *m_button_add;
    GtkWidget *m_button_rename;
    GtkWidget *m_button_delete_item;
    GtkWidget *m_frame_new_check_part;
    GtkWidget *m_label_check_part;
    int m_save_or_new_flag;
    GtkWidget *m_window;

    int m_current_note_page;
    // general
    GtkWidget *m_entry_hospital;
    GtkWidget *m_combobox_language;
    GtkWidget *m_combobox_date_format;
    GtkWidget *m_combobox_screen_saver;
    // GtkWidget *m_notebookPrinter;
    GtkWidget *m_radiobutton_common;
    GtkWidget *m_radiobutton_specific;
    GtkWidget *m_scrolledwindow_common;
    GtkWidget *m_fixed_specific;

    GtkTreeSelection *m_selected_common_printer;
    GtkTreeSelection *m_selected_specific_printer;
    GtkWidget *m_treeview_common_print;
    GtkWidget *m_treeview_specific_print;
    GtkWidget *m_calendar;
    GtkWidget *m_spinbutton_hour;
    GtkWidget *m_spinbutton_minute;
    GtkWidget *m_spinbutton_second;
    //options
    // GtkWidget *m_radiobutton_biopsy_on;
    // GtkWidget *m_radiobutton_biopsy_off;
    GtkWidget *m_radiobutton_TIC;
    GtkWidget *m_radiobutton_TIB;
    GtkWidget *m_radiobutton_TIS;
    GtkWidget *m_radiobutton_center_on;
    GtkWidget *m_radiobutton_center_off;
    GtkWidget *m_radiobutton_kb_sound_on;
    GtkWidget *m_radiobutton_kb_sound_off;
    GtkWidget *m_radiobutton_mouse_speed_low;
    GtkWidget *m_radiobutton_mouse_speed_middle;
    GtkWidget *m_radiobutton_mouse_speed_high;
    GtkWidget *m_combobox_img_format;
    GtkWidget *m_combobox_img_medium;
    GtkWidget *m_combobox_img_file;
    GtkWidget *m_combobox_video_format;
    GtkWidget *m_combobox_video_medium;
    GtkWidget *m_combobox_video_file;

    GtkWidget *m_radiobtn_total;
    GtkWidget *m_radiobtn_ud_11;
    GtkWidget *m_radiobtn_ud_21;
    GtkWidget *m_radiobtn_ud_12;
    GtkWidget *m_radiobtn_lr_11;
    GtkWidget *m_radiobtn_total_pw;
    GtkWidget *m_radiobtn_ud_11_pw;
    GtkWidget *m_radiobtn_ud_21_pw;
    GtkWidget *m_radiobtn_ud_12_pw;
    GtkWidget *m_radiobtn_lr_11_pw;
    GtkWidget *m_radiobtn_bm;
    GtkWidget *m_radiobtn_bpw;
    int m_bmIndex;
    int m_bpwIndex;

    //comment
    GtkWidget *m_radiobutton_font_big;
    GtkWidget *m_radiobutton_font_mid;
    GtkWidget *m_radiobutton_font_small;
    GtkWidget *m_radiobutton_bodymark_big;
    GtkWidget *m_radiobutton_bodymark_mid;
    GtkWidget *m_radiobutton_bodymark_small;
    GtkWidget *m_combobox_comment_color;
    GtkWidget *m_combobox_bodymark_color;
    GtkWidget *m_combobox_probe_comment;
    GtkWidget *m_combobox_exam_comment;
    GtkWidget *m_combobox_department_comment;
    GtkWidget *fixed_comment;
    GtkWidget* m_treeView_comment;
    GtkWidget* m_scrolled_all;
    GtkWidget* m_treeView_all;
    GtkWidget* m_button_select_one;
    GtkWidget *m_button_back_all;
    GtkWidget *m_button_back_one;
    GtkWidget *m_button_select_all;
    GtkWidget *m_button_comment_add;
    GtkWidget *m_button_comment_delete;
    GtkWidget *m_button_comment_delete_all;
    GtkWidget *m_button_comment_up;
    GtkWidget *m_button_comment_down;
    GtkWidget *m_button_comment_delete_select;

    //vector<string> vecItemComment;
    vector<string> vecExamItem_comment;
 // measure
    GtkWidget *m_radiobutton_ml_on;
    GtkWidget *m_radiobutton_ml_off;
    GtkWidget *m_radiobutton_ml_high;
    GtkWidget *m_radiobutton_ml_mid;
    GtkWidget *m_radiobutton_ml_low;
    GtkWidget *m_radiobutton_cursor_big;
    GtkWidget *m_radiobutton_cursor_mid;
    GtkWidget *m_radiobutton_cursor_small;
    GtkWidget *m_radiobutton_seq_default;
    GtkWidget *m_radiobutton_seq_repeat;
    GtkWidget *m_radiobutton_seq_none;
    GtkWidget *m_combobox_current_line;
    GtkWidget *m_combobox_confirmed_line;
    // GtkWidget *m_radiobutton_trace_auto;
    // GtkWidget *m_radiobutton_trace_manual;
    GtkWidget *m_radiobutton_trace_point;
    GtkWidget *m_radiobutton_trace_track;
    GtkWidget *m_radiobutton_trace_auto;
    GtkWidget *m_radiobutton_report_last;
    GtkWidget *m_radiobutton_report_average;
    GtkWidget *m_checkbutton_autocalc_ps;
    GtkWidget *m_checkbutton_autocalc_ed;
    GtkWidget *m_checkbutton_autocalc_ri;
    GtkWidget *m_checkbutton_autocalc_sd;
    GtkWidget *m_checkbutton_autocalc_tamax;
    GtkWidget *m_checkbutton_autocalc_pi;
    GtkWidget *m_checkbutton_autocalc_time;
    GtkWidget *m_checkbutton_autocalc_hr;
    GtkWidget *m_checkbutton_autocalc_pgmax;
    GtkWidget *m_checkbutton_autocalc_pgmean;
    GtkWidget *m_radiobutton_result_small;
    GtkWidget *m_radiobutton_result_big;
	GtkWidget *m_combobox_heart_beat_cycle;
    GtkWidget *m_combobox_unit_dist;
    GtkWidget *m_combobox_unit_area;
    GtkWidget *m_combobox_unit_vol;
    GtkWidget *m_combobox_unit_time;
    GtkWidget *m_combobox_unit_vel;
    GtkWidget *m_combobox_unit_slope;
    GtkWidget *m_combobox_unit_accel;
    GtkWidget *m_combobox_unit_efw;

    //calculate
    GtkWidget *m_obWindow;
    // GtkWidget *m_combobox_adult_2d_edv;
    // GtkWidget *m_combobox_adult_2d_fs;
    // GtkWidget *m_combobox_adult_2d_lv_mass;
    // GtkWidget *m_combobox_adult_m_edv;
    // GtkWidget *m_combobox_adult_m_fs;
    // GtkWidget *m_combobox_adult_m_lv_mass;
    // GtkWidget *m_combobox_mva;
    GtkWidget *m_combobox_bsa;
    // GtkWidget *m_combobox_fetus_2d_edv;
    // GtkWidget *m_combobox_fetus_2d_fs;
    // GtkWidget *m_combobox_fetus_m_edv;
    // GtkWidget *m_combobox_fetus_m_fs;
    GtkWidget *m_combobox_ob_crl;
    GtkWidget *m_combobox_ob_fl;
    GtkWidget *m_combobox_ob_ac;
    GtkWidget *m_combobox_ob_bpd;
    GtkWidget *m_combobox_ob_gs;
    GtkWidget *m_combobox_ob_hc;
    GtkWidget *m_combobox_ob_hl;
    GtkWidget *m_combobox_ob_cer;
    GtkWidget *m_combobox_ob_tad;
    GtkWidget *m_combobox_ob_apad;
    GtkWidget *m_combobox_ob_thd;
    GtkWidget *m_combobox_ob_ofd;
    GtkWidget *m_combobox_ob_efw;
    GtkWidget *m_button_apply;
    GtkWidget *m_button_save;
    GtkWidget *m_button_exit;
    //GtkWidget *m_button_save;
    GtkWidget *m_notebook;
#if 1 //add by jhuang
    GtkTreeIter topIter;
    GtkCellRenderer *renderer;
    GtkWidget *m_combobox_childsection;
    GtkWidget *m_entry_templet;

    GtkWidget* show_window;
    GtkWidget* m_WindowABD;
    GtkWidget* m_WindowUR;
    GtkWidget* m_WindowAE;
    GtkWidget* m_WindowOB;
    GtkWidget* m_WindowGYN;
    GtkWidget* m_WindowSP;
    GtkWidget* m_WindowFE;
    GtkWidget* m_WindowOTHERS;
    GtkWidget* m_WindowVES;
    GtkWidget* m_WindowTD;

    GtkWidget* m_WindowANOB;
    GtkWidget *m_FixedReportSet;
    GtkWidget *m_TreeviewReportSet;

    GtkWidget *button_add, *button_save, *button_delete, *button_ok, *button_cancel;
    GtkWidget *label_templet, *label_childsection;
    int init_language;
    guint m_flag_notebook_coustomreport;
    guint m_flag_notebook_image;
    GtkWidget *m_entry_new_check_part;

    ///> optional
    GtkWidget *m_treeOption ;
    GtkWidget *m_frameRegisterInfo;
    GtkWidget *m_labelExport;
    GtkWidget *m_entryRegisterKey;
    GtkWidget *m_labelRegister;
    bool m_powerOffFlag;

    bool StrCmpSectionString(int section, const char *string, int *language);
    bool StrCmpCustomString(const char *string, int *language);
    bool StrCmpTemplet1String(const char *string, int *language);

    void InitReportVar(void);
    bool OpenDB(void);
    bool CloseDB(void);
    bool InitDB(void);
    bool LocalizationDB(void);

    void AddTempletBySection( sqlite3 *db);
    void BtnRecoveryClicked(GtkButton *button);

    void ChkBtnClicked(GtkButton *button);

    GtkWidget *AddFrameByTitle(char *title, int section, int startid, int endid, int *h);
    GtkWidget *AddFrameByTitle2(char *title, int section, int startid, int endid, int *h);
    GtkWidget *AddFrameForOthers(const char *title, int *h);

    GtkWidget *CreatWindowABD(void);
    GtkWidget *CreatWindowAE(void);
    GtkWidget *CreatWindowUR(void);
    GtkWidget *CreatWindowOB(void);
    GtkWidget *CreatWindowGYN(void);
    GtkWidget *CreatWindowSP(void);
    GtkWidget *CreatWindowFE(void);
    GtkWidget *CreatWindowOTHERS(void);
    GtkWidget *CreatWindowVES(void);
    GtkWidget *CreatWindowTD(void);
    GtkWidget *CreatWindowAnOB(void);

    GtkTreeModel *CreateTreeModel(void);
    void TreeSelectionChanged(GtkTreeSelection *selection);
    void BtnAddClicked(GtkButton *button);
    void BtnSave2Clicked(GtkButton *button);
    void BtnDeleteClicked(GtkButton *button);
    void BtnOkClicked(GtkButton *button);
    void BtnCancelClicked(GtkButton *button);
    void DeleteItemClicked(GtkButton *button);
    void EntryItemInsert(GtkCellRenderer *cell, GtkCellEditable *editable, const gchar *path);
    void ChangeCommentExamBox(int probe_type, char *check_part);
    void ChangeCommentExamBoxDelete(int probe_type);

    void EntryItemLengthInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);
    void CreateDefineItem_comment(int probeIndex, vector<string>& vecExamItem);
    void CreateDefineItem(int probeIndex, vector<ExamPara>& vecExamItem);
    bool RenameNotice(int probe_type_index, char *new_text, char *dialognotice, int group_length);
    void TransEnglish(char *strname, char str[256],char *str_indexname,char str_index[256]);

    std::vector <string> GetUserGroup(void);
#ifdef EMP_355
   static void  on_combobox_video_changed(GtkComboBox *widget,ViewSystem *data)
   {data->BtnComboVideoChanged();}
#endif
   ////>>>
  static void  on_entry_user_item_insert(GtkCellRenderer *cell, GtkCellEditable *editable, const gchar *path, ViewSystem *data)
    { data->EntryItemInsert(cell, editable, path); }
  ////>>>>

    static void on_entry_insert_custom_item(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewSystem *data)
    { data->EntryItemLengthInsert(editable, new_text, new_text_length, position); }

    void BtnComboVideoChanged();
    void CellRendererRename(GtkCellRendererText *renderer, gchar *path, gchar *new_text);
    void CellRendererRenameComment(GtkCellRendererText *m_cellrenderer_comment_text1, gchar *path, gchar *new_text);
    void CellRendererRenameSelectComment(GtkCellRendererText *m_cellrenderer_comment_text, gchar *path, gchar *new_text);
    void RenameItemClicked(GtkButton *button);
    static void HandleRenameItemClicked(GtkButton *button, ViewSystem *data) { data->RenameItemClicked(button); }
    static void HandleDeleteItemClicked(GtkButton *button, ViewSystem *data) { data->DeleteItemClicked(button); }
    static void HandleAddItemClicked(GtkButton *button, ViewSystem *data) { data->AddItemClicked(button); }
    static void HandleCellRendererRename(GtkCellRendererText *renderer, gchar *path, gchar *new_text, ViewSystem *data) { data->CellRendererRename(renderer, path, new_text); }
    static void HandleCellRendererRenameComment(GtkCellRendererText *m_cellrenderer_comment_text1, gchar *path, gchar *new_text, ViewSystem *data) { data->CellRendererRenameComment(m_cellrenderer_comment_text1, path, new_text); }
    static void HandleCellRendererRenameSelectComment(GtkCellRendererText *m_cellrenderer_comment_text, gchar *path, gchar *new_text, ViewSystem *data) { data->CellRendererRenameSelectComment(m_cellrenderer_comment_text, path, new_text); }

    bool ExamTypeTestRowExpandBefore(GtkTreeView *tree_view, GtkTreeIter *iter, GtkTreePath *path);
    gboolean BtnExamDepartmentClicked(GtkWidget *widget, GdkEventButton *event);

    static void HandleTreeSelectionChanged(GtkTreeSelection *selection, ViewSystem *data) { data->TreeSelectionChanged(selection); }
    static void HandleChkBtnClicked(GtkButton *button, ViewSystem *data) { data->ChkBtnClicked(button); }
    static void on_button_recovery_clicked(GtkButton *button, ViewSystem *data) { data->BtnRecoveryClicked(button); }
    static void on_button_add_clicked(GtkButton *button, ViewSystem *data) { data->BtnAddClicked(button); }
    static void on_button_save2_clicked(GtkButton *button, ViewSystem *data) { data->BtnSave2Clicked(button); }
    static void on_button_delete_clicked(GtkButton *button, ViewSystem *data) { data->BtnDeleteClicked(button); }
    static void on_button_ok_clicked(GtkButton *button, ViewSystem *data) { data->BtnOkClicked(button); }
    static void on_button_cancel_clicked(GtkButton *button, ViewSystem *data) { data->BtnCancelClicked(button); }

    GtkWidget *create_set_report(void);

    bool UniqueItem(const char *sections, const char *templet = "%", const char *childsection = "%");
    GtkTreeIter InsertUnique(GtkTreeModel *model, GtkTreeIter *iter, const char *str);
    GtkTreeIter InsertUniqueComment(GtkTreeModel *model, const char *str);

    bool InitRecordFromShowArr(const char *childsection, int *start, int *end);
    bool ReadRecordFromDB(const char * sections, const char* templet, const char * childsection);
    void apply_report_setting(void);

    void EntrytempletInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);
    //void EntryNewCheckPartInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);
    //void NewCheckPartFocusOut(GtkWidget *widget, GdkEventFocus *event);
    void BtnNewCheckPartOkClicked(GtkButton *button);
    void BtnNewCheckPartCancelClicked(GtkButton *button);

    static void on_entry_templet_insert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewSystem *data)
	{ data->EntrytempletInsert(editable, new_text, new_text_length, position); }

#endif  //add by jhuang end

    void GetImageNoteSelection(int &probeIndex, int &itemIndex, char *&itemName);

    //signal handle
    gboolean WindowDeleteEvent(GtkWidget *widget, GdkEvent *event);

    //signal
    static gboolean on_window_delete_event(GtkWidget *widget, GdkEvent *event, ViewSystem *data) { return data->WindowDeleteEvent(widget, event); }

    //2014.07.29 lihuamei
    /*
       static void on_entry_new_check_part(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewSystem *data)
       { data->EntryNewCheckPartInsert(editable, new_text, new_text_length, position); }
       static gboolean HandleNewCheckPartFocusOut(GtkWidget *widget, GdkEventFocus *event, ViewSystem *data)
       { data->NewCheckPartFocusOut(widget, event); return FALSE;}
     */
    static void HandleNewCheckPartBtnOk(GtkButton *button, ViewSystem *data) { data->BtnNewCheckPartOkClicked(button); }
    static void HandleNewCheckPartBtnCancel(GtkButton *button, ViewSystem *data) { data->BtnNewCheckPartCancelClicked(button); }

    //calc
    GtkWidget *m_calc_notebook;
    int  m_calc_page_num ;

    // TV Out
    GtkWidget *m_fixed_tvout;
    GtkWidget *m_combobox_video;
    GtkWidget *m_combobox_connect_type;
    GtkWidget *m_radiobutton_format_1;
    GtkWidget *m_radiobutton_format_2;
    GtkWidget *m_radiobutton_format_3;
    int m_flag_notebook_tvout;

    //dicom
    GtkWidget *m_dicom_notebook;
    int  m_page_num ;

    // key define
    bool m_p1Selected;
    GtkWidget *m_entry_function_p1;
    GtkWidget *m_entry_function_p2;
    GtkWidget *m_entry_function_p3;
    int m_p1_func_index;
    int m_p2_func_index;
    int m_p3_func_index;

    GtkWidget *m_radiobutton_key_p1[MAX_KEY];
    GtkWidget *m_radiobutton_key_p2[MAX_KEY];
    GtkWidget *m_radiobutton_key_p3[MAX_KEY];
    //user imagepreset
    GtkWidget *m_comboboxentry_user_select;

    //info
    GtkWidget *m_textview_version;

    //image
    GtkWidget *m_combobox_probe_type;
    GtkWidget *m_treeview_exam_type;
    GtkWidget *m_combobox_mbp;
    GtkWidget *m_combobox_ao_power;
    GtkWidget *m_spinbutton_2d_gain;
    GtkWidget *m_combobox_agc;
    GtkWidget *m_combobox_edge_enh;
    GtkWidget *m_combobox_steer;
    GtkWidget *m_combobox_focus_sum;
    GtkWidget *m_combobox_focus_pos;
    GtkWidget *m_combobox_depth;
    GtkWidget *m_combobox_chroma;
    GtkWidget *m_combobox_lr;
    GtkWidget *m_combobox_ud;

  //comment
    GtkWidget *scrolledwindow_item_comment1;
    GtkWidget *scrolledwindow_item_comment;

    GtkWidget *m_treeview_item_comment;
    GtkWidget *m_treeview_item_comment1;
    GtkWidget *m_combobox_polarity;
    GtkWidget *m_combobox_frame;
    GtkWidget *m_combobox_line_aver;
    GtkWidget *m_combobox_gray_trans; //lhm
    GtkWidget *m_combobox_space_compound;
    GtkWidget *m_combobox_freq_compound;
    GtkWidget *m_combobox_restric;
    GtkWidget *m_combobox_thi;
    GtkWidget *m_combobox_tsi;
    GtkWidget *m_combobox_imgEnh;
    GtkWidget *m_spinbutton_m_gain;
    GtkWidget *m_combobox_scan_range;
    GtkWidget *m_combobox_dynamic_range;
    GtkWidget *m_combobox_2d_freq;
    GtkWidget* m_combobox_thi_freq;
    GtkWidget *m_combobox_2d_line_density;
    GtkWidget *m_combobox_2d_smooth;
    GtkWidget *m_spinbutton_pw_gain;
    GtkWidget *m_spinbutton_pw_angle;
    GtkWidget *m_combobox_pw_scale_prf;
    GtkWidget *m_combobox_time_resolution;
    GtkWidget *m_combobox_pw_freq;
    GtkWidget *m_combobox_pw_wallfilter;
    GtkWidget *m_combobox_pw_invert;
    GtkWidget *m_combobox_cfm_line_density;
    GtkWidget *m_combobox_cfm_scale_prf;
    GtkWidget *m_spinbutton_cfm_gain;
    GtkWidget *m_combobox_cfm_wallfilter;
    GtkWidget *m_combobox_variance;
    GtkWidget *m_combobox_cfm_invert;
    GtkWidget *m_combobox_persistence;
    GtkWidget *m_combobox_cfm_smooth;
    GtkWidget *m_combobox_color_rejection;
    // GtkWidget *m_spinbutton_pdi_gain;
    GtkWidget *m_combobox_sensitivity;
    gchar *m_str_index;
    GtkTreeIter m_optionIter;

#ifdef VET
	GtkCellRenderer *m_cellrenderer_text;
#endif
	GtkCellRenderer *m_cellrenderer_comment_text;
	GtkCellRenderer *m_cellrenderer_comment_text1;
	//hlx
#ifdef VET
	GtkWidget *button_edit_dept;
	GtkWidget *button_return_dept;
    GtkWidget *button_get_current;
    GtkWidget *button_default_image;
#endif
    GtkWidget* create_note_general(void);
    void init_general_setting(SysGeneralSetting* sysGeneralSetting);
    void save_general_setting(void);
    GtkWidget* create_note_options(void);
    void init_option_setting(SysOptions* sysOptions);
    void save_option_setting(void);
    GtkWidget* create_note_image(void);
    void init_image_setting(void);
    void save_image_setting(void);
    void save_new_item(void);
    void init_printer(SysGeneralSetting *sysGeneralSetting);
    void CreatePrinter();
    void create_note_tvout(void);
    void init_tvout_setting(SysGeneralSetting *sysGeneralSetting);
    void save_tvout_setting(void);
    GtkWidget* create_note_dicom(void);
    void init_dicom_setting(void);
    void save_dicom_setting(void);
    GtkWidget* create_note_comment(void);
    void init_comment_setting(SysNoteSetting* sysNote);
    void save_comment_setting(void);
    GtkWidget* create_note_measure(void);
    void init_measure_setting(SysMeasurementSetting* sysMeasure);
    void save_measure_setting(void);
    GtkWidget* create_note_calc(void);
    GtkWidget* create_note_calc_measure(void);
    void init_calc_setting(SysCalculateSetting* sysCalc);
    void save_calc_setting(void);
    GtkWidget* create_note_key_config(void);
    void init_key_config(void);
    void save_key_config(void);
    GtkWidget* create_note_info(void);
    void init_info_setting(void);
    GtkTreeModel* create_common_print_model(void);
    GtkTreeModel* create_specific_print_model(void);
    GtkWidget* create_common_print_treeview(void);
    GtkWidget* create_specific_print_treeview(void);

    int common_printer_selection(void);
    std::string specific_printer_selection(void);

    // void add_print_column(GtkTreeView *treeview);
    GtkWidget * create_key_function_treeview(const std::string function_list[], unsigned int size);
    GtkTreeModel* create_exam_item_model(std::vector<int> index);
    GtkTreeModel* create_item_comment_model(int index);
    GtkTreeModel* create_item_comment_model1();

    int  DepartmentIndex();
    void create_exam_comment_model(std::vector<int> index);
    void  DepartmentName(char department[256], int index);
    void CreateItemList_Comment(char *department,vector<ExamPara>& vecItemComment, int number);
    void CreateItemList_Note(char *department,vector<ExamPara>& vecItemComment, int number);
    void CreateItemList_Comment1(string probe_exam,vector<ExamPara>& vecItemComment1);
    void CreateItemList_Delete_Comment1(char *select_name, string probe_exam,vector<ExamPara>& vecItemComment1);
    void CreateItemList_Delete_Comment(char *select_name, char *department, vector<ExamPara>& vecDeleteComment);
        void add_exam_item_column(GtkTreeView *treeview);
    void init_image_para(void);
    void set_image_item_sensitive(bool status);
    void image_para_combo_box_set(GtkWidget *combobox, int value);
    void image_para_restrict(int probeIndex);
    // signal handle
    void EntryHospitalInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);
    void BtnAdjustTimeClicked(GtkButton *button);
    void CommonRadioToggled(GtkToggleButton *togglebutton);
    void SpecificRadioToggled(GtkToggleButton *togglebutton);
    void BtnAddPrinterClicked(GtkButton *button);
    void BtnDelPrinterClicked(GtkButton *button);
    void BtnApplyClicked(GtkButton *button);
    void BtnSaveClicked(GtkButton *button);
    void BtnExitClicked(GtkButton *button);
    void BtnGeneralDefaultClicked(GtkButton *button);
    void BtnOptionsDefaultClicked(GtkButton *button);
    void BtnMeasureDefaultClicked(GtkButton *button);
    void BtnCalculateDefaultClicked(GtkButton *button);
    void BtnCommentDefaultClicked(GtkButton *button);
    void BtnTVOutDefaultClicked(GtkButton *button);
    void ComboNetMethodChanged(GtkComboBox *widget);
    void BtnDicomTest(GtkButton *button);
    void BtnDicomReset(GtkButton *button);
    void EntryIPInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);
    //    void EntryIPChanged(GtkEditable *editable);
    void EntryDigitInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);
    void EntryFracDigitInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);
    void BtnOBCustomClicked(GtkButton *button);
    void EntryNameInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position); //addey by LL 2012-9-17

    void ChkBtnTAmaxToggled(GtkToggleButton *togglebutton);
    void ChkBtnPIToggled(GtkToggleButton *togglebutton);
    void ChkBtnHRToggled(GtkToggleButton *togglebutton);

    //void ChkBtnTotalToggled(GtkToggleButton *togglebutton);
    void AddItemClicked(GtkButton *button);
    void AddCheckPart(char *checkpart);

  #ifdef VET
#else
    void RadioP1Toggled(GtkToggleButton *togglebutton);
#endif
    void RadioP2Toggled(GtkToggleButton *togglebutton);
    void TreeFuncChanged(GtkTreeSelection *selection);
    void ExamTypeChanged(GtkTreeSelection *selection);
    void ProbeTypeChanged(GtkComboBox *widget);
    void ProbeCommentChanged(GtkComboBox *widget);
    void ExamCommentChanged(GtkComboBox *widget);
    void DepartmentCommentChanged(GtkComboBox *widget);

    void UserChanged(GtkComboBox *widget);
    void BtnUpgradeClicked(GtkButton *button);
    void BtnImageDefaultClicked(GtkButton *button);
    void image_default_setting();
    void BtnImageGetCurrentClicked(GtkButton *button);
    void BtnImageSaveClicked(GtkButton *button);
    void get_current_and_save_image_para(char *itemName);
    void BtnImageNewClicked(GtkButton *button);
    void BtnImageExportToUSBClicked(GtkButton *button);
    void BtnImageImportFromUSBClicked(GtkButton *button);
    void BtnDeleteUserClicked(GtkButton *button);
    void ComboFocusSum(GtkComboBox *box);
    void ComboPwPrf(GtkComboBox *box);
    void ComboCfmPrf(GtkComboBox *box);
    void ComboSensitivity(GtkComboBox *box);
    void ClearComboBox(GtkComboBox *box);
    void SpinbuttonInsertTime(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);
    void SpinbuttonInsertHour(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);
    void SpinbuttonInsertGain(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);
	void add_columns_comment(GtkTreeView *treeview);
	void add_columns_comment1(GtkTreeView *treeview);

#ifdef VET
	void add_columns(GtkTreeView *treeview);
	void BtnImageEditClicked(GtkButton *button);
	void BtnImageReturnClicked(GtkButton *button);
	void BtnEditCellChanged(GtkCellRendererText *cell, gchar *path_string, gchar *new_text);
#endif
  void SpinbuttonInsertAngle(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);
    gboolean SpinbuttonOutputAngle(GtkSpinButton *spin);
    void CommonTreeviewSelectionChanged(GtkTreeSelection *treeselection);
    void SpecificTreeviewSelectionChanged(GtkTreeSelection *treeselection);
    void ModebmRadioToggled(GtkToggleButton *togglebutton);
    void ModebpwRadioToggled(GtkToggleButton *togglebutton);
    void OnRadiobtnDisplay_total(GtkToggleButton *togglebutton);
    void OnRadiobtnDisplay_ud11(GtkToggleButton *togglebutton);
    void OnRadiobtnDisplay_ud21(GtkToggleButton *togglebutton);
    void OnRadiobtnDisplay_ud12(GtkToggleButton *togglebutton);
    void OnRadiobtnDisplay_lr11(GtkToggleButton *togglebutton);

    void notebookChanged(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num);
    void DicomnotebookChanged(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num);
    void CalcnotebookChanged(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num);

    void LanguageChanged(GtkComboBox *box);
    void VgaChanged(GtkComboBox *box);

    void UserSelectFocusOut(GtkWidget *widget, GdkEventFocus *event);

    void ButtonSelectOneCommentClicked(GtkButton *button);
    void ButtonSelectAllCommentClicked(GtkButton *button);

    //    void ButtonEditClicked(GtkButton *button);
    void ButtonDeleteClicked(GtkButton *button);
    void ButtonAddClicked(GtkButton *button);
    void ButtonDeleteSelectClicked(GtkButton *button);
    void ButtonDeleteAllClicked(GtkButton *button);
    void ButtonUpClicked(GtkButton *button);

    ///>optional function
    void TreeCursorChanged(GtkTreeView *treeview);
    int OptionOptEvent(GtkWidget *widget, GdkEvent *event);
    void MenuInstallActivate(GtkMenuItem *menuitem);
    void MenuUninstallActivate(GtkMenuItem *menuitem);
    void BtnExportClicked(GtkButton *button);
    void BtnRegisterClicked(GtkButton *button);

    void ButtonDownClicked(GtkButton *button);
// signal connect
        static void on_entry_hospital_insert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewSystem *data)
    { data->EntryHospitalInsert(editable, new_text, new_text_length, position); }
    static void on_button_adjust_time_clicked(GtkButton *button, ViewSystem *data) { data->BtnAdjustTimeClicked(button); }
    static void on_common_radio_button_toggled(GtkToggleButton *togglebutton, ViewSystem *data) { data->CommonRadioToggled(togglebutton); }
    static void on_specific_radio_button_toggled(GtkToggleButton *togglebutton, ViewSystem *data) { data->SpecificRadioToggled(togglebutton); }
    static void on_button_add_printer_clicked(GtkButton *button, ViewSystem *data) { data->BtnAddPrinterClicked(button); }
    static void on_button_del_printer_clicked(GtkButton *button, ViewSystem *data) { data->BtnDelPrinterClicked(button); }
    static void on_button_apply_clicked(GtkButton *button, ViewSystem *data) { data->BtnApplyClicked(button); }
    static void on_button_save_clicked(GtkButton *button, ViewSystem *data) { data->BtnSaveClicked(button); }
    static void on_button_exit_clicked(GtkButton *button, ViewSystem *data) { data->BtnExitClicked(button); }
    static void on_button_general_default_clicked(GtkButton *button, ViewSystem *data) { data->BtnGeneralDefaultClicked(button); }
    static void on_button_options_default_clicked(GtkButton *button, ViewSystem *data) { data->BtnOptionsDefaultClicked(button); }
    static void on_button_measure_default_clicked(GtkButton *button, ViewSystem *data) { data->BtnMeasureDefaultClicked(button); }
    static void on_button_calc_default_clicked(GtkButton *button, ViewSystem *data) { data->BtnCalculateDefaultClicked(button); }
    static void on_button_comment_default_clicked(GtkButton *button, ViewSystem *data) { data->BtnCommentDefaultClicked(button); }
    static void on_button_tvout_default_clicked(GtkButton *button, ViewSystem *data) { data->BtnTVOutDefaultClicked(button); }
    //static void HandleNetMethodChanged(GtkComboBox *widget, ViewSystem *data) { data->ComboNetMethodChanged(widget); }
    static void HandleDicomTest(GtkButton *button, ViewSystem *data) { data->BtnDicomTest(button); }
    static void HandleDicomReset(GtkButton *button, ViewSystem *data) { data->BtnDicomReset(button); }
    static void on_entry_ip_insert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewSystem *data)
    { data->EntryFracDigitInsert(editable, new_text, new_text_length, position); }
    //static void on_entry_ip_changed(GtkEditable *editable, ViewSystem *data) { data->EntryIPChanged(editable); }
    static void on_entry_host_port(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewSystem *data)
    { data->EntryDigitInsert(editable, new_text, new_text_length, position); }

    static void on_button_upgrade_clicked(GtkButton *button, ViewSystem *data) { data->BtnUpgradeClicked(button); }

    static void HandleTAmaxToggled(GtkToggleButton *togglebutton, ViewSystem *data) { data->ChkBtnTAmaxToggled(togglebutton); }
    static void HandlePIToggled(GtkToggleButton *togglebutton, ViewSystem *data) { data->ChkBtnPIToggled(togglebutton); }
    static void HandleHRToggled(GtkToggleButton *togglebutton, ViewSystem *data) { data->ChkBtnHRToggled(togglebutton); }

    //static void HandleChkTotalToggled(GtkToggleButton *togglebutton, ViewSystem *data) { data->ChkBtnTotalToggled(togglebutton); }
#ifdef VET
#else
    static void HandleP1Toggled(GtkToggleButton *togglebutton, ViewSystem *data) { data->RadioP1Toggled(togglebutton); }
#endif
    static void HandleP2Toggled(GtkToggleButton *togglebutton, ViewSystem *data) { data->RadioP2Toggled(togglebutton); }

    static void HandleTreeFuncChanged(GtkTreeSelection *selection, ViewSystem *data) { data->TreeFuncChanged(selection); }
    static void HandleExamCommentChanged(GtkComboBox *widget, ViewSystem *data) {data->ExamCommentChanged(widget); }
    static void HandleProbeCommentChanged(GtkComboBox *widget, ViewSystem *data) {data->ProbeCommentChanged(widget); }
    static void HandleProbeTypeChanged(GtkComboBox *widget, ViewSystem *data) {data->ProbeTypeChanged(widget); }
    static void HandleDepartmentCommentChanged(GtkComboBox *widget, ViewSystem *data) {data->DepartmentCommentChanged(widget); }

    static void HandleUserChanged(GtkComboBox *widget, ViewSystem *data) {data->UserChanged(widget); }
    static void HandleExamTypeChanged(GtkTreeSelection *selection, ViewSystem *data) { data->ExamTypeChanged(selection); }
    static bool HandleExamTypeExpandBefore(GtkTreeView *tree_view, GtkTreeIter *iter, GtkTreePath *path, ViewSystem *data)
    {
        return data->ExamTypeTestRowExpandBefore(tree_view, iter, path);
    }
    static void HandleExamDepartmentBtnClicked(GtkWidget *widget, GdkEventButton *event, ViewSystem *data) { data->BtnExamDepartmentClicked(widget, event); }

    static void HandleImageBtnDefault(GtkButton *button, ViewSystem *data) { data->BtnImageDefaultClicked(button); }
    static void HandleImageBtnGetCurrent(GtkButton *button, ViewSystem *data) { data->BtnImageGetCurrentClicked(button); }
    static void HandleImageBtnSave(GtkButton *button, ViewSystem *data) { data->BtnImageSaveClicked(button); }
    static void HandleImageBtnNew(GtkButton *button, ViewSystem *data) { data->BtnImageNewClicked(button); }
    static void HandleImageBtnExportToUSB(GtkButton *button, ViewSystem *data) { data->BtnImageExportToUSBClicked(button); }
    static void HandleImageBtnImportFromUSB(GtkButton *button, ViewSystem *data) { data->BtnImageImportFromUSBClicked(button); }
    static void HandleImageBtnDeleteUser(GtkButton *button, ViewSystem *data) { data->BtnDeleteUserClicked(button); }

    static void HandleComboFocusSum(GtkComboBox *box, ViewSystem *data) { data->ComboFocusSum(box); }
    static void HandleComboPwPrf(GtkComboBox *box, ViewSystem *data) { data->ComboPwPrf(box); }
    static void HandleComboCfmPrf(GtkComboBox *box, ViewSystem *data) { data->ComboCfmPrf(box); }
    static void HandleComboCfmSensitivity(GtkComboBox *box, ViewSystem *data) { data->ComboSensitivity(box); }
    static void on_spinbutton_insert_hour(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewSystem *data)
    { data->SpinbuttonInsertHour(editable, new_text, new_text_length, position); }
    static void on_spinbutton_insert_time(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewSystem *data)
    { data->SpinbuttonInsertTime(editable, new_text, new_text_length, position); }
    static void on_spinbutton_insert_gain(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewSystem *data)
    { data->SpinbuttonInsertGain(editable, new_text, new_text_length, position); }
    static void on_spinbutton_insert_angle(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewSystem *data)
    { data->SpinbuttonInsertAngle(editable, new_text, new_text_length, position); }
    static gboolean on_spinbutton_output_angle(GtkSpinButton *spin, ViewSystem *data)
    { return data->SpinbuttonOutputAngle(spin); }
#ifdef VET
	static void HandleImageBtnEditDept(GtkButton *button, ViewSystem *data) {data->BtnImageEditClicked(button);}
	static void HandleImageBtnReturnDept(GtkButton *button, ViewSystem *data) {data->BtnImageReturnClicked(button);}
	static void on_button_edit_cell_callback(GtkCellRendererText *cell, gchar *path_string, gchar *new_text, ViewSystem *data){
		data->BtnEditCellChanged(cell, path_string, new_text);
	}
#endif
    static void on_common_treeview_selection_changed(GtkTreeSelection *treeselection, ViewSystem *data)
    { data->CommonTreeviewSelectionChanged(treeselection); }
    static void on_specific_treeview_selection_changed(GtkTreeSelection *treeselection, ViewSystem *data)
    { data->SpecificTreeviewSelectionChanged(treeselection); }

    static void on_button_ob_custom_clicked(GtkButton *button, ViewSystem *data) { data->BtnOBCustomClicked(button); }

    static void on_modebm_radio_button_toggled(GtkToggleButton *togglebutton, ViewSystem *data) {data->ModebmRadioToggled(togglebutton); }
    static void on_modebpw_radio_button_toggled(GtkToggleButton *togglebutton, ViewSystem *data) {data->ModebpwRadioToggled(togglebutton); }
    static void on_display_ud11_radio_button_toggled(GtkToggleButton *togglebutton, ViewSystem *data)
    { data->OnRadiobtnDisplay_ud11(togglebutton); }
    static void on_display_ud21_radio_button_toggled(GtkToggleButton *togglebutton, ViewSystem *data)
    { data->OnRadiobtnDisplay_ud21(togglebutton); }
    static void on_display_ud12_radio_button_toggled(GtkToggleButton *togglebutton, ViewSystem *data)
    { data->OnRadiobtnDisplay_ud12(togglebutton); }
    static void on_display_total_radio_button_toggled(GtkToggleButton *togglebutton, ViewSystem *data)
    { data->OnRadiobtnDisplay_total(togglebutton); }
    static void on_display_lr11_radio_button_toggled(GtkToggleButton *togglebutton, ViewSystem *data)
    { data->OnRadiobtnDisplay_lr11(togglebutton); }

    static void on_entry_name_insert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewSystem *data)
    { data->EntryNameInsert(editable, new_text, new_text_length, position); }

    static gboolean HandleUserSelectFocusOut(GtkWidget *widget, GdkEventFocus *event, ViewSystem *data)
    { data->UserSelectFocusOut(widget, event); return FALSE;}

    static void on_notebook_switch_page(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, ViewSystem *data)
    { data->notebookChanged(notebook, page, page_num); }

    static void on_dicom_notebook_switch_page(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, ViewSystem *data)
    { data->DicomnotebookChanged(notebook, page, page_num); }

    static void on_calc_notebook_switch_page(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, ViewSystem *data)
    { data->CalcnotebookChanged(notebook, page, page_num); }

    static void on_language_changed(GtkComboBox *widget, ViewSystem *data) {data->LanguageChanged(widget); }
    static void on_vga_changed(GtkComboBox *widget, ViewSystem *data) {data->VgaChanged(widget); }

    static void HandleButtonSelectOneCommentClicked(GtkButton *button, ViewSystem *data)
    {
        if (data)
            data->ButtonSelectOneCommentClicked(button);
    }
    static void HandleButtonSelectAllCommentClicked(GtkButton *button, ViewSystem *data)
    {
        if (data)
            data->ButtonSelectAllCommentClicked(button);
    }

  /*  static void HandleButtonEditClicked(GtkButton *button, CalcSetting *data)
    {
        if (data)
            data->ButtonEditClicked(button);
    }*/
    static void HandleButtonDeleteClicked(GtkButton *button, ViewSystem *data)
    {
        if (data)
            data->ButtonDeleteClicked(button);
    }
    static void HandleButtonAddClicked(GtkButton *button, ViewSystem *data)
    {
        if (data)
            data->ButtonAddClicked(button);
    }
   static void HandleButtonDeleteSelectClicked(GtkButton *button, ViewSystem *data)
    {
        if (data)
            data->ButtonDeleteSelectClicked(button);
    }

    static void HandleButtonDeleteAllClicked(GtkButton *button, ViewSystem *data)
    {
        if (data)
            data->ButtonDeleteAllClicked(button);
    }
    static void HandleButtonDownClicked(GtkButton *button, ViewSystem *data)
    {
        if (data)
            data->ButtonDownClicked(button);
    }
    static void HandleButtonUpClicked(GtkButton *button, ViewSystem *data)
    {
        if (data)
            data->ButtonUpClicked(button);
    }

    ///>optional functions
    static void HandleOptionCursorChanged(GtkTreeView *treeview, ViewSystem *data) { return data->TreeCursorChanged(treeview); }
    static int HandleOptionOptEvent(GtkWidget *widget, GdkEvent *event, ViewSystem *data) { return data->OptionOptEvent(widget, event); }
    static void HandleMenuInstallActivate(GtkMenuItem *menuitem, ViewSystem *data) { data->MenuInstallActivate(menuitem); }
    static void HandleMenuUninstallActivate(GtkMenuItem *menuitem, ViewSystem *data) { data->MenuUninstallActivate(menuitem); }
    static void HandleBtnExportClicked(GtkButton *widget, ViewSystem *data) { return data->BtnExportClicked(widget); }
    static void HandleBtnRegisterClicked(GtkButton *widget, ViewSystem *data) { return data->BtnRegisterClicked(widget); }

};

class ViewCustomOB: public FakeXEvent
{
    public:
        ~ViewCustomOB();

        static ViewCustomOB* GetInstance();
        void CreateWindow(GtkWidget *parent);
        void DestroyWindow(void);
    private:
        ViewCustomOB();

        static ViewCustomOB* m_ptrInstance;

        GtkWidget *m_obWindow;
        // custom OB
        typedef struct
        {
            char *gw;
            char *gw1;
            char *gw2;
            char *gw3;
            char *gw4;
            char *gw5;
            char *gw6;
            char *gw7;
            char *gw8;
            char *gw9;
            char *gw10;
        }CustomData;
        enum
        {
            COL_GW,
            COL_GW1,
            COL_GW2,
            COL_GW3,
            COL_GW4,
            COL_GW5,
            COL_GW6,
            COL_GW7,
            COL_GW8,
            COL_GW9,
            COL_GW10,
            NUM_COLUMNS
        };
        CustomData m_data;
        GtkWidget *m_combobox_customOB;
        GtkWidget *m_treeCustomOB;
        GArray *m_arrayGW;
        GtkTreeModel *m_modelCustomOB;

        GtkWidget* create_customOB_window(void);
        void init_customOB(void);
        void save_customOB(void);
        void insert_list(GtkTreeModel *model, int table[]);
        void update_list(int type);

        void KeyEvent(unsigned char keyValue);

        void CustomSelectChanged(GtkComboBox *combobox);
        void CustomCellEditingStarted(GtkCellRenderer *cell, GtkCellEditable *editable, const gchar *path);
        void CustomCellEdited(GtkCellRenderer *cell, gchar *path_string, gchar *new_text);
        void EntryFracDigitInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);

        static void changed_custom_ob(GtkComboBox *combobox, ViewCustomOB *data) { data->CustomSelectChanged(combobox); }
        static void cell_editing_started_custom_ob(GtkCellRenderer *cell, GtkCellEditable *editable, const gchar *path, ViewCustomOB *data)
        { data->CustomCellEditingStarted(cell, editable, path); }
        static void cell_edited_custom_ob(GtkCellRenderer *cell, gchar *path_string, gchar *new_text, ViewCustomOB *data)
        { data->CustomCellEdited(cell, path_string, new_text); }
        static void on_entry_insert_custom_ob(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewCustomOB *data)
        { data->EntryFracDigitInsert(editable, new_text, new_text_length, position); }

        //signal handle
        void BtnOBCustomClearClicked(GtkButton *button);
        void BtnOBCustomSaveClicked(GtkButton *button);
        void BtnOBCustomExitClicked(GtkButton *button);
        gboolean WindowDeleteEvent(GtkWidget *widget, GdkEvent *event);

        //signal
        static gboolean on_window_delete_event(GtkWidget *widget, GdkEvent *event, ViewCustomOB*data) { return data->WindowDeleteEvent(widget, event); }
        static void on_button_ob_custom_clear_clicked(GtkButton *button, ViewCustomOB *data) { data->BtnOBCustomClearClicked(button); }
        static void on_button_ob_custom_save_clicked(GtkButton *button, ViewCustomOB *data) { data->BtnOBCustomSaveClicked(button); }
        static void on_button_ob_custom_exit_clicked(GtkButton *button, ViewCustomOB *data) { data->BtnOBCustomExitClicked(button); }
};

#endif