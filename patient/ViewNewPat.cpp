#include <gtk/gtk.h>
#include "display/gui_func.h"
#include "display/gui_global.h"
#include "patient/ViewNewPat.h"
#include "keyboard/KeyValueOpr.h"
#include "keyboard/KeyDef.h"
#include "ViewMain.h"
#include <unistd.h>
#include <stdlib.h>
#include <libavutil/time.h>
#include "sysMan/SysGeneralSetting.h"
#include "sysMan/SysCalculateSetting.h"
#include <string.h>
#include "patient/Database.h"
#include "patient/ViewPatSearch.h"
#include <sstream>
#include <glib.h>
#include "keyboard/KeyFunc.h"
#include "calcPeople/MeaCalcFun.h"
#include "measure/MeasureMan.h"
#include "display/ViewCalendar.h"
#include "display/ViewDialog.h"
#include "display/TopArea.h"
#include "sysMan/ViewSystem.h"
#include "patient/ViewWorkList.h"
#include "periDevice/DCMMan.h"
#include "sysMan/UpgradeMan.h"
#include "sysMan/SysDicomSetting.h"
#include "periDevice/DCMRegister.h"

using std::ostringstream;
using std::string;
ViewNewPat* ViewNewPat::m_ptrInstance = NULL;

ViewNewPat::ViewNewPat()
{
    m_langCN = 0;
    m_curYear = 0;
    m_curMonth = 0;
    m_curDay = 0;
    m_window = NULL;
    m_flagMPPS = false;
    UpdateNameLength();
}

ViewNewPat::~ViewNewPat()
{
    if (m_ptrInstance != NULL)
        delete m_ptrInstance;
}

ViewNewPat* ViewNewPat::GetInstance()
{
    if (m_ptrInstance == NULL)
        m_ptrInstance = new ViewNewPat;

    return m_ptrInstance;
}

void ViewNewPat::CreateWindow(void)
{
    GtkWidget *fixed_window;
    GtkWidget *frame;
    GtkWidget *fixed_general_info;
    GtkWidget *label_pat_id;
    GtkWidget *label_gender;
#ifdef VET
    GtkWidget *label_name_animal;
    GtkWidget *label_name_owner;
    GtkWidget *label_species;
#else
    GtkWidget *label_name_mid;
    GtkWidget *label_name_first;
    GtkWidget *label_name_last;
#endif
    GtkWidget *label_age;
    GtkWidget *label_birth_date;
    GtkWidget *label_spacing_day;
    GtkWidget *label_spacing_year;
    GtkWidget *label_spacing_month;
    GtkWidget *img_birth_calendar;
    GtkWidget *eventbox_birth_calendar;
    GtkWidget *button_search_pat_info;

    GtkWidget *label_general_info;
    //GtkWidget *button_title;
    GtkWidget *notebook;
    GtkWidget *label_general;
    GtkWidget *fixed_tab_general;
    GtkWidget *label_ob;
    GtkWidget *fixed_tab_ob;
    GtkWidget *label_card;
    GtkWidget *fixed_tab_card;
    GtkWidget *label_uro;
    GtkWidget *fixed_tab_uro;
    GtkWidget *label_other;
    GtkWidget *fixed_tab_other;
    GtkWidget *label_comment;
    GtkWidget *scrolledwindow_comment;
    GtkWidget *label_diagnostician;
    GtkWidget *label_physician;
    GtkWidget *hseparator;

    GtkWidget *button_exam_end;
    GtkWidget *image_exam_end;
    GtkWidget *label_exam_end;
    GtkWidget *button_new_pat;
    GtkWidget *image_new_pat;
    GtkWidget *label_new_pat;
    GtkWidget *button_new_exam;
    GtkWidget *image_new_exam;
    GtkWidget *label_new_exam;
    GtkWidget *button_ok;
    GtkWidget *image_ok;
    GtkWidget *label_ok;
    GtkWidget *button_cancel;
    GtkWidget *image_cancel;
    GtkWidget *label_cancel;

    GtkWidget *image_worklist;
    GtkWidget *label_worklist;
    const int pat_name_len = 60;
    const int pat_id_len = 15;

    MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::NONE);
#ifdef VET
    //BeforeEnterDialog();
#endif
    SysGeneralSetting *sgs = new SysGeneralSetting;
    if ((ZH == sgs->GetLanguage()) || (FR == sgs->GetLanguage()))
        m_langCN = true;
    else
        m_langCN = false;
    delete sgs;

    GetCurrentDate(m_curYear, m_curMonth, m_curDay);

    m_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (m_window, 840, 640);
    gtk_window_set_position (GTK_WINDOW (m_window), GTK_WIN_POS_CENTER);
    gtk_window_set_modal (GTK_WINDOW (m_window), TRUE);
    gtk_window_set_resizable (GTK_WINDOW (m_window), FALSE);
#ifdef VET
    gtk_window_set_title (GTK_WINDOW (m_window), _("Animal Information"));
#else
    gtk_window_set_title (GTK_WINDOW (m_window), _("Patient Information"));
#endif

    gtk_window_set_transient_for(GTK_WINDOW(m_window), GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()));
    g_signal_connect (G_OBJECT(m_window), "delete-event", G_CALLBACK(on_window_delete_event), this);

    fixed_window = gtk_fixed_new ();
    gtk_widget_show (fixed_window);
    gtk_container_add (GTK_CONTAINER (m_window), fixed_window);

    frame = gtk_frame_new (NULL);
    gtk_widget_show (frame);
    gtk_fixed_put (GTK_FIXED (fixed_window), frame, 20, 10);
    gtk_widget_set_size_request (frame, 800, 140);
    gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_OUT);

    fixed_general_info = gtk_fixed_new ();
    gtk_widget_show (fixed_general_info);
    gtk_container_add (GTK_CONTAINER (frame), fixed_general_info);

#ifdef VET
    label_pat_id = gtk_label_new (_("ID:"));
#else
    label_pat_id = gtk_label_new (_("Patient ID:"));
#endif

    gtk_widget_show (label_pat_id);
    gtk_fixed_put (GTK_FIXED (fixed_general_info), label_pat_id, 10, 8);
    gtk_widget_set_size_request (label_pat_id, 100, 30);
    gtk_misc_set_alignment (GTK_MISC (label_pat_id), 0.9, 0.5);

    m_entryPatID = gtk_entry_new ();
    gtk_widget_show (m_entryPatID);
    gtk_fixed_put (GTK_FIXED (fixed_general_info), m_entryPatID, 110+8, 8);
    gtk_widget_set_size_request (m_entryPatID, 180, 30);
    gtk_entry_set_max_length(GTK_ENTRY(m_entryPatID), pat_id_len);
    g_signal_connect(G_OBJECT(m_entryPatID), "insert_text", G_CALLBACK(on_entry_pat_id), this);
    g_signal_connect(G_OBJECT(m_entryPatID), "focus-out-event", G_CALLBACK(HandlePatIDFocusOut), this);

    m_checkbuttonPatID = gtk_check_button_new_with_mnemonic (_("Auto Generated ID"));
    gtk_widget_show (m_checkbuttonPatID);
    gtk_fixed_put (GTK_FIXED (fixed_general_info), m_checkbuttonPatID, 300+8, 8);
    gtk_widget_set_size_request (m_checkbuttonPatID, 150+100, 30);
    g_signal_connect (G_OBJECT(m_checkbuttonPatID), "clicked", G_CALLBACK(on_chkbutton_pad_id), this);

    button_search_pat_info = gtk_button_new_with_mnemonic (_("Search"));
    gtk_widget_show (button_search_pat_info);
    gtk_fixed_put (GTK_FIXED (fixed_general_info), button_search_pat_info, 630, 8);
    gtk_widget_set_size_request (button_search_pat_info, 120, 30);
    g_signal_connect ((gpointer) button_search_pat_info, "clicked", G_CALLBACK (on_button_search_clicked), this);

#ifdef VET
    label_name_animal = gtk_label_new (_("Animal Name:"));
    gtk_widget_show (label_name_animal);
    gtk_fixed_put (GTK_FIXED (fixed_general_info), label_name_animal, 10, 45);
    gtk_widget_set_size_request (label_name_animal, 100, 30);
    gtk_misc_set_alignment (GTK_MISC (label_name_animal), 0.9, 0.5);

    m_entryAnimalName = gtk_entry_new ();
    gtk_widget_show (m_entryAnimalName);
    gtk_fixed_put (GTK_FIXED (fixed_general_info), m_entryAnimalName, 118, 45);
    gtk_widget_set_size_request (m_entryAnimalName, 160, 30);
    gtk_entry_set_max_length(GTK_ENTRY(m_entryAnimalName), pat_name_len);
    g_signal_connect(G_OBJECT(m_entryAnimalName), "insert_text", G_CALLBACK(on_entry_name_insert), this);

    label_name_owner = gtk_label_new (_("Owner:"));
    gtk_widget_show (label_name_owner);
    gtk_fixed_put (GTK_FIXED (fixed_general_info), label_name_owner, 270, 45);
    gtk_widget_set_size_request (label_name_owner, 100, 30);
    gtk_misc_set_alignment (GTK_MISC (label_name_owner), 0.9, 0.5);

    GtkListStore *store =  gtk_list_store_new (1, G_TYPE_STRING);
    m_comboboxOwnerName = gtk_combo_box_entry_new_with_model(GTK_TREE_MODEL(store), 0);
    //gtk_combo_box_set_model(GTK_COMBO_BOX(m_comboboxOwnerName), GTK_TREE_MODEL(store));
    g_object_unref(store);
    gtk_widget_show(m_comboboxOwnerName);
    gtk_fixed_put (GTK_FIXED (fixed_general_info), m_comboboxOwnerName, 370, 45);
    gtk_widget_set_size_request (m_comboboxOwnerName, 120, 30);
    g_signal_connect(m_comboboxOwnerName, "notify::active", G_CALLBACK(HandleOwnerNameChanged), this);
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
    gtk_cell_layout_clear (GTK_CELL_LAYOUT(m_comboboxOwnerName));
    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (m_comboboxOwnerName), renderer, TRUE);
    gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (m_comboboxOwnerName), renderer, "text", 0, NULL);
    gtk_cell_layout_reorder (GTK_CELL_LAYOUT(m_comboboxOwnerName), renderer, 0);

    gtk_entry_set_max_length(GTK_ENTRY(gtk_bin_get_child(GTK_BIN(m_comboboxOwnerName))), pat_name_len);
    g_signal_connect(G_OBJECT(gtk_bin_get_child(GTK_BIN(m_comboboxOwnerName))), "insert_text", G_CALLBACK(HandleOwnerNameInsert), this);
    g_signal_connect(G_OBJECT(gtk_bin_get_child(GTK_BIN(m_comboboxOwnerName))), "delete_text", G_CALLBACK(HandleOwnerNameDelete), this);

#if 0
    m_entryOwnerName = gtk_entry_new ();
    gtk_widget_show (m_entryOwnerName);
    gtk_fixed_put (GTK_FIXED (fixed_general_info), m_entryOwnerName, 370, 45);
    gtk_widget_set_size_request (m_entryOwnerName, 120, 30);
    gtk_entry_set_max_length(GTK_ENTRY(m_entryOwnerName), pat_name_len);
    g_signal_connect(G_OBJECT(m_entryOwnerName), "insert_text", G_CALLBACK(on_entry_name_insert), this);
#endif
    label_species = gtk_label_new (_("Species:"));
    gtk_widget_show (label_species);
    gtk_fixed_put (GTK_FIXED (fixed_general_info), label_species, 530, 45);
    gtk_widget_set_size_request (label_species, 100, 30);
    gtk_misc_set_alignment (GTK_MISC (label_species), 0.9, 0.5);

    m_comboboxSpecies = gtk_combo_box_new_text ();
    gtk_widget_show (m_comboboxSpecies);
    gtk_fixed_put (GTK_FIXED (fixed_general_info), m_comboboxSpecies, 630, 45);
    gtk_widget_set_size_request (m_comboboxSpecies, 120, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxSpecies), _("Dog"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxSpecies), _("Cat"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxSpecies), _("Sheep"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxSpecies), _("Swine"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxSpecies), _("Bovine"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxSpecies), _("Equine"));
//    gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxSpecies), _("Other"));
    gtk_combo_box_set_active(GTK_COMBO_BOX (m_comboboxSpecies), 0);

#else
    label_name_last = gtk_label_new (_("Last Name:"));
    gtk_widget_show (label_name_last);
    gtk_fixed_put (GTK_FIXED (fixed_general_info), label_name_last, 10, 45);
    gtk_widget_set_size_request (label_name_last, 100, 30);
    gtk_misc_set_alignment (GTK_MISC (label_name_last), 0.9, 0.5);

    m_entryNameLast = gtk_entry_new ();
    gtk_widget_show (m_entryNameLast);
    gtk_fixed_put (GTK_FIXED (fixed_general_info), m_entryNameLast, 110+8, 45);
    gtk_widget_set_size_request (m_entryNameLast, 120, 30);
    gtk_entry_set_max_length(GTK_ENTRY(m_entryNameLast), m_pat_name_last);
    g_signal_connect(G_OBJECT(m_entryNameLast), "insert_text", G_CALLBACK(on_entry_name_insert), this);

    label_name_first = gtk_label_new (_("First Name:"));
    gtk_widget_show (label_name_first);
    gtk_fixed_put (GTK_FIXED (fixed_general_info), label_name_first, 270+40-50-10, 45);
    gtk_widget_set_size_request (label_name_first, 110, 30);
    gtk_misc_set_alignment (GTK_MISC (label_name_first), 0.9, 0.5);

    m_entryNameFirst = gtk_entry_new ();
    gtk_widget_show (m_entryNameFirst);
    gtk_fixed_put (GTK_FIXED (fixed_general_info), m_entryNameFirst, 370+40-50, 45);
    gtk_widget_set_size_request (m_entryNameFirst, 120, 30);
    gtk_entry_set_max_length(GTK_ENTRY(m_entryNameFirst), m_pat_name_first);
    g_signal_connect(G_OBJECT(m_entryNameFirst), "insert_text", G_CALLBACK(on_entry_name_insert), this);

    label_name_mid = gtk_label_new (_("Middle Name:"));
    gtk_widget_show (label_name_mid);
    gtk_fixed_put (GTK_FIXED (fixed_general_info), label_name_mid, 530-10-10, 45);
    gtk_widget_set_size_request (label_name_mid, 100+10+10, 30);
    gtk_misc_set_alignment (GTK_MISC (label_name_mid), 0.9, 0.5);

    m_entryNameMid = gtk_entry_new ();
    gtk_widget_show (m_entryNameMid);
    gtk_fixed_put (GTK_FIXED (fixed_general_info), m_entryNameMid, 630, 45);
    gtk_widget_set_size_request (m_entryNameMid, 120, 30);
    gtk_entry_set_max_length(GTK_ENTRY(m_entryNameMid), m_pat_name_middle);
    g_signal_connect(G_OBJECT(m_entryNameMid), "insert_text", G_CALLBACK(on_entry_name_insert), this);
#endif
    label_birth_date = gtk_label_new (_("Date of Birth:"));
    //label_birth_date = gtk_label_new (_("Birth Date:"));
    gtk_widget_show (label_birth_date);
    gtk_fixed_put (GTK_FIXED (fixed_general_info), label_birth_date, 10, 80);
    gtk_widget_set_size_request (label_birth_date, 100+8, 30);
    gtk_misc_set_alignment (GTK_MISC (label_birth_date), 0.9, 0.5);
    SysGeneralSetting *sys_date = new SysGeneralSetting;
    m_date_format = sys_date->GetDateFormat();
    if(0==m_date_format)
    {
        m_entryBirthYear = gtk_entry_new ();
        gtk_widget_show (m_entryBirthYear);
        gtk_fixed_put (GTK_FIXED (fixed_general_info), m_entryBirthYear, 110+8, 80);
        gtk_widget_set_size_request (m_entryBirthYear, 48, 30);
        gtk_entry_set_max_length(GTK_ENTRY(m_entryBirthYear), 4);
        g_signal_connect(G_OBJECT(m_entryBirthYear), "insert_text", G_CALLBACK(on_entry_birth_date_year), this);
        g_signal_connect(G_OBJECT(m_entryBirthYear), "delete_text", G_CALLBACK(on_entry_delete_birth_date_year), this);

        label_spacing_year = gtk_label_new (_("Y  "));
        gtk_widget_show (label_spacing_year);
        gtk_fixed_put (GTK_FIXED (fixed_general_info), label_spacing_year, 158+8, 80);
        gtk_widget_set_size_request (label_spacing_year, 12+12, 30);

        m_entryBirthMonth = gtk_entry_new ();
        gtk_widget_show (m_entryBirthMonth);
        gtk_fixed_put (GTK_FIXED (fixed_general_info), m_entryBirthMonth, 170+8+12, 80);
        gtk_widget_set_size_request (m_entryBirthMonth, 24, 30);
        gtk_entry_set_max_length(GTK_ENTRY(m_entryBirthMonth), 2);
        g_signal_connect(G_OBJECT(m_entryBirthMonth), "insert_text", G_CALLBACK(on_entry_birth_date_month), this);
        g_signal_connect(G_OBJECT(m_entryBirthMonth), "delete_text", G_CALLBACK(on_entry_delete_birth_date_month), this);

        label_spacing_month = gtk_label_new (_("M  "));
        gtk_widget_show (label_spacing_month);
        gtk_fixed_put (GTK_FIXED (fixed_general_info), label_spacing_month, 194+8+12, 80);
        gtk_widget_set_size_request (label_spacing_month, 12+12, 30);

        m_entryBirthDay = gtk_entry_new ();
        gtk_widget_show (m_entryBirthDay);
        gtk_fixed_put (GTK_FIXED (fixed_general_info), m_entryBirthDay, 206+8+24, 80);
        gtk_widget_set_size_request (m_entryBirthDay, 24, 30);
        gtk_entry_set_max_length(GTK_ENTRY(m_entryBirthDay), 2);
        g_signal_connect(G_OBJECT(m_entryBirthDay), "insert_text", G_CALLBACK(on_entry_birth_date_day), this);
        g_signal_connect(G_OBJECT(m_entryBirthDay), "delete_text", G_CALLBACK(on_entry_delete_birth_date_day), this);

        label_spacing_day = gtk_label_new (_("D  "));
        gtk_widget_show (label_spacing_day);
        gtk_fixed_put (GTK_FIXED (fixed_general_info), label_spacing_day, 240+24, 80);
        gtk_widget_set_size_request (label_spacing_day, 12+12, 30);
    }

    else if(2==m_date_format)
    {
        m_entryBirthYear = gtk_entry_new ();
        gtk_widget_show (m_entryBirthYear);
        gtk_fixed_put (GTK_FIXED (fixed_general_info), m_entryBirthYear, 206+8, 80);
        gtk_widget_set_size_request (m_entryBirthYear, 48, 30);
        gtk_entry_set_max_length(GTK_ENTRY(m_entryBirthYear), 4);
        g_signal_connect(G_OBJECT(m_entryBirthYear), "insert_text", G_CALLBACK(on_entry_birth_date_year), this);
        g_signal_connect(G_OBJECT(m_entryBirthYear), "delete_text", G_CALLBACK(on_entry_delete_birth_date_year), this);

        label_spacing_year = gtk_label_new(_("D  "));
        gtk_widget_show (label_spacing_year);
        gtk_fixed_put (GTK_FIXED (fixed_general_info), label_spacing_year, 158+8-24, 80);
        gtk_widget_set_size_request (label_spacing_year, 12+12, 30);

        m_entryBirthMonth = gtk_entry_new ();
        gtk_widget_show (m_entryBirthMonth);
        gtk_fixed_put (GTK_FIXED (fixed_general_info), m_entryBirthMonth, 170+8-24+12, 80);
        gtk_widget_set_size_request (m_entryBirthMonth, 24, 30);
        gtk_entry_set_max_length(GTK_ENTRY(m_entryBirthMonth), 2);
        g_signal_connect(G_OBJECT(m_entryBirthMonth), "insert_text", G_CALLBACK(on_entry_birth_date_month), this);
        g_signal_connect(G_OBJECT(m_entryBirthMonth), "delete_text", G_CALLBACK(on_entry_delete_birth_date_month), this);

        label_spacing_month = gtk_label_new (_("M  "));
        gtk_widget_show (label_spacing_month);
        gtk_fixed_put (GTK_FIXED (fixed_general_info), label_spacing_month, 194+8-24+12, 80);
        gtk_widget_set_size_request (label_spacing_month, 12+12, 30);

        m_entryBirthDay = gtk_entry_new ();
        gtk_widget_show (m_entryBirthDay);
        gtk_fixed_put (GTK_FIXED (fixed_general_info), m_entryBirthDay, 118, 80);
        gtk_widget_set_size_request (m_entryBirthDay, 24, 30);
        gtk_entry_set_max_length(GTK_ENTRY(m_entryBirthDay), 2);
        g_signal_connect(G_OBJECT(m_entryBirthDay), "insert_text", G_CALLBACK(on_entry_birth_date_day), this);
        g_signal_connect(G_OBJECT(m_entryBirthDay), "delete_text", G_CALLBACK(on_entry_delete_birth_date_day), this);

        label_spacing_day = gtk_label_new (_("Y  "));
        gtk_widget_show (label_spacing_day);
        gtk_fixed_put (GTK_FIXED (fixed_general_info), label_spacing_day, 240+24, 80);
        gtk_widget_set_size_request (label_spacing_day, 12+12, 30);
    }

    else
    {
        m_entryBirthYear = gtk_entry_new ();
        gtk_widget_show (m_entryBirthYear);
        gtk_fixed_put (GTK_FIXED (fixed_general_info), m_entryBirthYear, 206+8-24+24, 80);
        gtk_widget_set_size_request (m_entryBirthYear, 48, 30);
        gtk_entry_set_max_length(GTK_ENTRY(m_entryBirthYear), 4);
        g_signal_connect(G_OBJECT(m_entryBirthYear), "insert_text", G_CALLBACK(on_entry_birth_date_year), this);
        g_signal_connect(G_OBJECT(m_entryBirthYear), "delete_text", G_CALLBACK(on_entry_delete_birth_date_year), this);

        label_spacing_year = gtk_label_new (_("M  "));
        gtk_widget_show (label_spacing_year);
        gtk_fixed_put (GTK_FIXED (fixed_general_info), label_spacing_year, 158+8-24, 80);
        gtk_widget_set_size_request (label_spacing_year, 12+12, 30);

        m_entryBirthMonth = gtk_entry_new ();
        gtk_widget_show (m_entryBirthMonth);
        gtk_fixed_put (GTK_FIXED (fixed_general_info), m_entryBirthMonth, 118, 80);
        gtk_widget_set_size_request (m_entryBirthMonth, 24, 30);
        gtk_entry_set_max_length(GTK_ENTRY(m_entryBirthMonth), 2);
        g_signal_connect(G_OBJECT(m_entryBirthMonth), "insert_text", G_CALLBACK(on_entry_birth_date_month), this);
        g_signal_connect(G_OBJECT(m_entryBirthMonth), "delete_text", G_CALLBACK(on_entry_delete_birth_date_month), this);

        label_spacing_month = gtk_label_new (_("D  "));
        gtk_widget_show (label_spacing_month);
        gtk_fixed_put (GTK_FIXED (fixed_general_info), label_spacing_month, 194+8-24+12, 80);
        gtk_widget_set_size_request (label_spacing_month, 12+12, 30);

        m_entryBirthDay = gtk_entry_new ();
        gtk_widget_show (m_entryBirthDay);
        gtk_fixed_put (GTK_FIXED (fixed_general_info), m_entryBirthDay, 170+8-24+12, 80);
        gtk_widget_set_size_request (m_entryBirthDay, 24, 30);
        gtk_entry_set_max_length(GTK_ENTRY(m_entryBirthDay), 2);
        g_signal_connect(G_OBJECT(m_entryBirthDay), "insert_text", G_CALLBACK(on_entry_birth_date_day), this);
        g_signal_connect(G_OBJECT(m_entryBirthDay), "delete_text", G_CALLBACK(on_entry_delete_birth_date_day), this);

        label_spacing_day = gtk_label_new (_("Y  "));
        gtk_widget_show (label_spacing_day);
        gtk_fixed_put (GTK_FIXED (fixed_general_info), label_spacing_day, 240+24, 80);
        gtk_widget_set_size_request (label_spacing_day, 12+12, 30);
    }
    img_birth_calendar = gtk_image_new_from_file ("./res/icon/Calendar.png");
    gtk_widget_show (img_birth_calendar);
    eventbox_birth_calendar = gtk_event_box_new ();
    gtk_widget_show (eventbox_birth_calendar);
    gtk_widget_set_size_request (eventbox_birth_calendar, 40, 30);
    gtk_container_add (GTK_CONTAINER (eventbox_birth_calendar), img_birth_calendar);
    gtk_fixed_put (GTK_FIXED (fixed_general_info), eventbox_birth_calendar, 254+36, 80);
    g_signal_connect (G_OBJECT(eventbox_birth_calendar), "button_press_event", G_CALLBACK(HandleEventBoxCalendarPress), this);

    m_entryAge = gtk_entry_new ();
    gtk_widget_show (m_entryAge);
    gtk_fixed_put (GTK_FIXED (fixed_general_info), m_entryAge, 370+40, 80);
    gtk_widget_set_size_request (m_entryAge, 70, 30);
    gtk_entry_set_max_length(GTK_ENTRY(m_entryAge), 3);
    g_signal_connect(G_OBJECT(m_entryAge), "insert_text", G_CALLBACK(on_entry_age), this);

    m_comboboxAge = gtk_combo_box_new_text ();
    gtk_widget_show (m_comboboxAge);
    gtk_fixed_put (GTK_FIXED (fixed_general_info), m_comboboxAge, 440+40, 80);
    gtk_widget_set_size_request (m_comboboxAge, 50, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxAge), _("Y"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxAge), _("M"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxAge), _("D"));

    label_gender = gtk_label_new (_("Gender:"));
    gtk_widget_show (label_gender);
    gtk_fixed_put (GTK_FIXED (fixed_general_info), label_gender, 530, 80);
    gtk_widget_set_size_request (label_gender, 100, 30);
    gtk_misc_set_alignment (GTK_MISC (label_gender), 0.9, 0.5);

    m_comboboxGender = gtk_combo_box_new_text ();
    gtk_widget_show (m_comboboxGender);
    gtk_fixed_put (GTK_FIXED (fixed_general_info), m_comboboxGender, 630, 80);
    gtk_widget_set_size_request (m_comboboxGender, 122, 32);
#ifdef VET
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxGender), _("Female "));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxGender), _("Male "));
#else
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxGender), _("Female"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxGender), _("Male"));
#endif
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxGender), _("Other"));
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboboxGender), -1);

    label_age = gtk_label_new (_("Age:"));
    gtk_widget_show (label_age);
    gtk_fixed_put (GTK_FIXED (fixed_general_info), label_age, 270+40, 80);
    gtk_widget_set_size_request (label_age, 100, 30);
    gtk_misc_set_alignment (GTK_MISC (label_age), 0.9, 0.5);

    label_general_info = gtk_label_new (_("General Information"));
    gtk_widget_show (label_general_info);
    gtk_frame_set_label_widget (GTK_FRAME (frame), label_general_info);
    gtk_label_set_use_markup (GTK_LABEL (label_general_info), TRUE);

    // button_title = gtk_button_new_with_mnemonic (_("Patient Infomation"));
    // gtk_widget_show (button_title);
    // gtk_fixed_put (GTK_FIXED (fixed_window), button_title, 0, 0);
    // gtk_widget_set_size_request (button_title, 800, 30);
    // GTK_WIDGET_UNSET_FLAGS (button_title, GTK_CAN_FOCUS);
    // gtk_button_set_focus_on_click (GTK_BUTTON (button_title), FALSE);

    notebook = gtk_notebook_new ();
    gtk_widget_show (notebook);
    gtk_fixed_put (GTK_FIXED (fixed_window), notebook, 20, 160);
    gtk_widget_set_size_request (notebook, 800, 250);
//hlx
#ifdef VET
	fixed_tab_card = create_note_card();
    gtk_container_add (GTK_CONTAINER (notebook), fixed_tab_card);

    label_card = gtk_label_new (_("CARD"));
    gtk_widget_show (label_card);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), 0), label_card);

    fixed_tab_uro = create_note_uro();
    gtk_container_add (GTK_CONTAINER (notebook), fixed_tab_uro);

    label_uro = gtk_label_new (_("URO"));
    gtk_widget_show (label_uro);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), 1), label_uro);

    fixed_tab_other = create_note_other();
    gtk_container_add(GTK_CONTAINER(notebook), fixed_tab_other);

    label_other = gtk_label_new (_("Other"));
    gtk_widget_show (label_other);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), 2), label_other);
#else
    fixed_tab_general = create_note_general();
    gtk_container_add (GTK_CONTAINER (notebook), fixed_tab_general);

    label_general = gtk_label_new (_("General"));
    gtk_widget_show (label_general);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), 0), label_general);

    fixed_tab_ob = create_note_ob();
    gtk_container_add (GTK_CONTAINER (notebook), fixed_tab_ob);

    label_ob = gtk_label_new (_("OB"));
    gtk_widget_show (label_ob);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), 1), label_ob);

    fixed_tab_card = create_note_card();
    gtk_container_add (GTK_CONTAINER (notebook), fixed_tab_card);

    label_card = gtk_label_new (_("CARD"));
    gtk_widget_show (label_card);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), 2), label_card);

    fixed_tab_uro = create_note_uro();
    gtk_container_add (GTK_CONTAINER (notebook), fixed_tab_uro);

    label_uro = gtk_label_new (_("URO"));
    gtk_widget_show (label_uro);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), 3), label_uro);

    fixed_tab_other = create_note_other();
    gtk_container_add(GTK_CONTAINER(notebook), fixed_tab_other);

    label_other = gtk_label_new (_("Other"));
    gtk_widget_show (label_other);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), 4), label_other);
#endif
    label_comment = gtk_label_new (_("Comment:"));
    gtk_widget_show (label_comment);
    gtk_fixed_put (GTK_FIXED (fixed_window), label_comment, 20, 420);
    gtk_widget_set_size_request (label_comment, 100, 30);
    gtk_misc_set_alignment (GTK_MISC (label_comment), 0.9, 0.5);

    scrolledwindow_comment = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show (scrolledwindow_comment);
    gtk_fixed_put (GTK_FIXED (fixed_window), scrolledwindow_comment, 120, 420);
    gtk_widget_set_size_request (scrolledwindow_comment, 700, 60);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_comment), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow_comment), GTK_SHADOW_OUT);

    m_textview_comment = gtk_text_view_new ();
    gtk_widget_show (m_textview_comment);
    gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (m_textview_comment), GTK_WRAP_WORD_CHAR);
    gtk_container_add (GTK_CONTAINER (scrolledwindow_comment), m_textview_comment);
    g_signal_connect(G_OBJECT(gtk_text_view_get_buffer(GTK_TEXT_VIEW(m_textview_comment))), "insert-text", G_CALLBACK(on_textview_comment_insert), this);
    // label_accession = gtk_label_new (_("Accession #:"));
    // gtk_widget_show (label_accession);
    // gtk_fixed_put (GTK_FIXED (fixed_window), label_accession, 5, 475);
    // gtk_widget_set_size_request (label_accession, 100, 30);
    // gtk_misc_set_alignment (GTK_MISC (label_accession), 0.9, 0.5);

    // entry_accession = gtk_entry_new ();
    // gtk_widget_show (entry_accession);
    // gtk_fixed_put (GTK_FIXED (fixed_window), entry_accession, 105, 475);
    // gtk_widget_set_size_request (entry_accession, 120, 30);
    // gtk_entry_set_invisible_char (GTK_ENTRY (entry_accession), 8226);

    label_diagnostician = gtk_label_new (_("Diagnostician:"));
    gtk_widget_show (label_diagnostician);
    gtk_fixed_put (GTK_FIXED (fixed_window), label_diagnostician, 20, 490);
    gtk_widget_set_size_request (label_diagnostician, 100, 30);
    gtk_misc_set_alignment (GTK_MISC (label_diagnostician), 0.9, 0.5);

    m_comboboxentry_diagnostician = gtk_combo_box_entry_new_text ();
    gtk_widget_show (m_comboboxentry_diagnostician);
    gtk_fixed_put (GTK_FIXED (fixed_window), m_comboboxentry_diagnostician, 120, 490);
    gtk_widget_set_size_request (m_comboboxentry_diagnostician, 120, 30);
    GtkWidget *bin_entry_dia = gtk_bin_get_child (GTK_BIN(m_comboboxentry_diagnostician));
    gtk_entry_set_max_length(GTK_ENTRY(bin_entry_dia), 45);
    //g_signal_connect(G_OBJECT(bin_entry_dia), "insert_text", G_CALLBACK(on_entry_name_insert), this);

    label_physician = gtk_label_new (_("Physician:"));
    gtk_widget_show (label_physician);
    gtk_fixed_put (GTK_FIXED (fixed_window), label_physician, 240, 490);
    gtk_widget_set_size_request (label_physician, 100, 30);
    gtk_misc_set_alignment (GTK_MISC (label_physician), 0.9, 0.5);

    m_comboboxentry_physician = gtk_combo_box_entry_new_text ();
    gtk_widget_show (m_comboboxentry_physician);
    gtk_fixed_put (GTK_FIXED (fixed_window), m_comboboxentry_physician, 340, 490);
    gtk_widget_set_size_request (m_comboboxentry_physician, 120, 30);
    GtkWidget *bin_entry_phy = gtk_bin_get_child (GTK_BIN(m_comboboxentry_physician));
    gtk_entry_set_max_length(GTK_ENTRY(bin_entry_phy), 45);
    g_signal_connect(G_OBJECT(bin_entry_phy), "insert_text", G_CALLBACK(on_entry_name_insert), this);

    Database db;
    vector<string> doc_name;
    vector<string>::const_iterator iter_doc;
    if (db.DoctorSearch(&doc_name)) {
        for (iter_doc = doc_name.begin(); iter_doc != doc_name.end(); ++iter_doc) {
            gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxentry_diagnostician), (*iter_doc).c_str());
            gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxentry_physician), (*iter_doc).c_str());
        }
    }

    hseparator = gtk_hseparator_new ();
    gtk_widget_show (hseparator);
    gtk_fixed_put (GTK_FIXED (fixed_window), hseparator, 20, 540);
    gtk_widget_set_size_request (hseparator, 800, 15);

    image_exam_end = gtk_image_new_from_stock ("gtk-media-stop", GTK_ICON_SIZE_BUTTON);
    label_exam_end = gtk_label_new_with_mnemonic (_("End Exam"));
    button_exam_end = create_button_icon (label_exam_end, image_exam_end);
    gtk_fixed_put (GTK_FIXED (fixed_window), button_exam_end, 20, 560);
    gtk_button_set_focus_on_click(GTK_BUTTON(button_exam_end), TRUE);

    image_new_pat = gtk_image_new_from_stock ("gtk-orientation-portrait", GTK_ICON_SIZE_BUTTON);
#ifdef VET
    label_new_pat = gtk_label_new_with_mnemonic (_("New Animal"));
#else
    label_new_pat = gtk_label_new_with_mnemonic (_("New Patient"));
#endif
    button_new_pat = create_button_icon(label_new_pat, image_new_pat);
    gtk_fixed_put (GTK_FIXED (fixed_window), button_new_pat, 150, 560);

    image_new_exam = gtk_image_new_from_stock ("gtk-new", GTK_ICON_SIZE_BUTTON);
    label_new_exam = gtk_label_new_with_mnemonic (_("New Exam"));
    button_new_exam = create_button_icon(label_new_exam, image_new_exam);
    gtk_fixed_put (GTK_FIXED (fixed_window), button_new_exam, 280, 560);
#ifndef VET
#ifdef EMP_3410
    if(CManRegister::GetInstance()->IsAuthorize(CManRegister::Optional[0]))
#else
        if(CDCMRegister::GetMe()->IsAuthorize())
#endif
    {
        //worklist
        image_worklist = gtk_image_new_from_stock ("gtk-info", GTK_ICON_SIZE_BUTTON);
        label_worklist = gtk_label_new_with_mnemonic (_(" Worklist"));
        m_button_worklist = create_button_icon(label_worklist, image_worklist);
        gtk_fixed_put (GTK_FIXED (fixed_window), m_button_worklist, 410, 560);
        g_signal_connect ((gpointer) m_button_worklist, "clicked", G_CALLBACK (on_button_worklist_clicked), this);
    }
#endif
    image_ok = gtk_image_new_from_stock ("gtk-ok", GTK_ICON_SIZE_BUTTON);
    label_ok = gtk_label_new_with_mnemonic (_("OK"));
    button_ok = create_button_icon(label_ok, image_ok);
    gtk_fixed_put (GTK_FIXED (fixed_window), button_ok, 570, 560);
    gtk_button_set_focus_on_click(GTK_BUTTON(button_ok), TRUE);

    image_cancel = gtk_image_new_from_stock ("gtk-cancel", GTK_ICON_SIZE_BUTTON);
    label_cancel = gtk_label_new_with_mnemonic (_("Cancel"));
    button_cancel = create_button_icon(label_cancel, image_cancel);
    gtk_fixed_put (GTK_FIXED (fixed_window), button_cancel, 700, 560);

    g_signal_connect ((gpointer) button_exam_end, "clicked", G_CALLBACK (on_button_exam_end_clicked), this);
    g_signal_connect ((gpointer) button_new_pat, "clicked", G_CALLBACK (on_button_new_pat_clicked), this);
    g_signal_connect ((gpointer) button_new_exam, "clicked", G_CALLBACK (on_button_new_exam_clicked), this);

    g_signal_connect ((gpointer) button_ok, "clicked", G_CALLBACK (on_button_ok_clicked), this);
    g_signal_connect ((gpointer) button_cancel, "clicked", G_CALLBACK (on_button_cancel_clicked), this);

    gtk_widget_show(m_window);

    g_keyInterface.Push(this);
    SetSystemCursorToCenter();
#ifndef VET
    if (m_langCN)
    {
        gtk_widget_hide (m_entryNameMid);
        gtk_widget_hide (label_name_mid);
    }
#endif

    PatientInfo::Info info;
    g_patientInfo.GetInfo(info);
    FillNewPat(info);

    if (info.p.id.empty())
    {
        SysGeneralSetting sgs;
        if(sgs.GetAutoGeneratedID())AutoPatID();
    }
    else
    {
        gtk_widget_set_sensitive(m_entryPatID, FALSE);
        gtk_widget_set_sensitive(m_checkbuttonPatID, FALSE);
    }

    delete(sys_date);
    return ;
}

namespace{
int CallBackLoadPatData(gpointer data)
{
    ViewNewPat::GetInstance()->LoadPatData();
    return 0;
}
int CallBackAutoPatID(gpointer data)
{
    ViewNewPat::GetInstance()->AutoPatID();
    return 0;
}
}

void ViewNewPat::CheckPatID(const gchar *pat_id)
{
    Database db;
    if ((db.GetPatIDExist(pat_id)).empty()) {
        return ;
    } else {
#ifdef VET
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::QUESTION, _("Animal ID exist, load data?"), CallBackLoadPatData, CallBackAutoPatID);
#else
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::QUESTION, _("Patient ID exist, load data?"), CallBackLoadPatData, CallBackAutoPatID);
#endif

    }
}

void ViewNewPat::LoadPatData(void)
{
    const gchar *pat_id = gtk_entry_get_text(GTK_ENTRY(m_entryPatID));
    Database db;
    PatientInfo::Patient pat_info;
    db.GetPatInfo(pat_id, pat_info);
    FillPatInfo(pat_info);
    gtk_widget_set_sensitive(m_entryPatID, FALSE);
    gtk_widget_set_sensitive(m_checkbuttonPatID, FALSE);
}

void ViewNewPat::UpdateNameLength()
{
	SysGeneralSetting sgs;
    if(ZH == sgs.GetLanguage())
    {
        m_pat_name_last = 31;
        m_pat_name_middle = 0;
        m_pat_name_first = 31;
    }
    else if(RU == sgs.GetLanguage())
    {
        m_pat_name_last = 30;
        m_pat_name_middle = 16;
        m_pat_name_first = 16;
    }
    else if(FR == sgs.GetLanguage())
    {
        m_pat_name_last = 31;
        m_pat_name_middle = 0;
        m_pat_name_first = 31;
    }
    else
    {
        m_pat_name_last = 20;
        m_pat_name_middle = 20;
        m_pat_name_first = 20;
    }
}

void ViewNewPat::AutoPatID(void)
{
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_checkbuttonPatID), TRUE);
    string ID = GenPatID();
    gtk_entry_set_text(GTK_ENTRY(m_entryPatID), ID.c_str());
}

void ViewNewPat::PatIDFocusOut(GtkWidget *widget, GdkEventFocus *event)
{
    const gchar *pat_id = gtk_entry_get_text(GTK_ENTRY(widget));
    CheckPatID(pat_id);
}

void ViewNewPat::BirthDateYearInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position)
{
    if (!g_ascii_isdigit(*new_text) && g_ascii_strcasecmp(new_text, ".") != 0)
        g_signal_stop_emission_by_name((gpointer)editable, "insert_text");

    string old_str = gtk_entry_get_text(GTK_ENTRY(editable));
    string new_str = new_text;
    old_str.insert(*position, new_str);
    if (old_str.length() == 4) {
        int year = atoi(old_str.c_str());
        if (year < 1900) {
            *position = 4;      // 移动光标之输入框末尾
            gtk_entry_set_text(GTK_ENTRY(m_entryBirthYear), "1900");
            year = 1900;
        } else if (year > m_curYear) {
            *position = 4;
            char curYear[5];
            sprintf(curYear, "%d", m_curYear);
            gtk_entry_set_text(GTK_ENTRY(m_entryBirthYear), curYear);
            year = m_curYear;
        }
        const char *month_text = gtk_entry_get_text(GTK_ENTRY(m_entryBirthMonth));
        const char *day_text = gtk_entry_get_text(GTK_ENTRY(m_entryBirthDay));
        AutoCalcAge(year, atoi(month_text), atoi(day_text));
        gtk_widget_grab_focus(m_entryBirthMonth);
    }
    return;
}

void ViewNewPat::BirthDateYearDelete(GtkEditable *editable, gint start_pos, gint end_pos)
{
    string new_str = gtk_entry_get_text(GTK_ENTRY(editable));
    new_str.erase(start_pos, end_pos);
    if (new_str.length() != 4) {
     	gtk_entry_set_text(GTK_ENTRY(m_entryAge), "");
        gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboboxAge), 0);
    }
}

void ViewNewPat::BirthDateMonthInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position)
{
    if (!g_ascii_isdigit(*new_text) && g_ascii_strcasecmp(new_text, ".") != 0)
        g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
    string old_str = gtk_entry_get_text(GTK_ENTRY(editable));
    string new_str = new_text;
    old_str.insert(*position, new_str);
    if (old_str.length() != 0) {
        int month = atoi(old_str.c_str());
        if (month < 1) {
            g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
            return ;
        } else if (month > 12) {
            *position = 2;      // 移动光标之输入框末尾
            gtk_entry_set_text(GTK_ENTRY(m_entryBirthMonth), "12");
            month = 12;
        }
        const char *year_text = gtk_entry_get_text(GTK_ENTRY(m_entryBirthYear));
        if (strlen(year_text) != 4)
            return;
        const char *day_text = gtk_entry_get_text(GTK_ENTRY(m_entryBirthDay));
        AutoCalcAge(atoi(year_text), month, atoi(day_text));
        if (old_str.length() == 2)
            gtk_widget_grab_focus(m_entryBirthDay);
    }
    return;
}

void ViewNewPat::BirthDateMonthDelete(GtkEditable *editable, gint start_pos, gint end_pos)
{
    string new_str = gtk_entry_get_text(GTK_ENTRY(editable));
    new_str.erase(start_pos, end_pos);

    if (new_str.length() != 0) {
        int month = atoi(new_str.c_str());
        if (month < 1) {
            g_signal_stop_emission_by_name((gpointer)editable, "delete_text");
            return ;
        }
        const char *year_text = gtk_entry_get_text(GTK_ENTRY(m_entryBirthYear));
        if (strlen(year_text) != 4)
            return;
        const char *day_text = gtk_entry_get_text(GTK_ENTRY(m_entryBirthDay));
        AutoCalcAge(atoi(year_text), month, atoi(day_text));
    } else {
     	gtk_entry_set_text(GTK_ENTRY(m_entryAge), "");
        gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboboxAge), 0);
    }
}

void ViewNewPat::BirthDateDayInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position)
{
    if (!g_ascii_isdigit(*new_text) && g_ascii_strcasecmp(new_text, ".") != 0)
        g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
    string old_str = gtk_entry_get_text(GTK_ENTRY(editable));
    string new_str = new_text;
    old_str.insert(*position, new_str);
    int day = atoi(old_str.c_str());

    if (day < 1) {
        g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
        return ;
    }
    const char *year_text = gtk_entry_get_text(GTK_ENTRY(m_entryBirthYear));
    const char *month_text = gtk_entry_get_text(GTK_ENTRY(m_entryBirthMonth));
    if (strlen(year_text) == 0 || strlen(year_text) < 4 || strlen(month_text) == 0) {
        if (day > 31) {
            *position = 2;      // 移动光标之输入框末尾
            gtk_entry_set_text(GTK_ENTRY(m_entryBirthDay), "31");
        }
        return ;
    } else {
        int year = atoi(year_text);
        int month = atoi(month_text);
        int last_day = g_date_get_days_in_month (GDateMonth(month), GDateYear(year));
        if (day > last_day) {
            *position = 2;      // 移动光标之输入框末尾
            char str_last_day[3];
            sprintf(str_last_day, "%d", last_day);
            gtk_entry_set_text(GTK_ENTRY(m_entryBirthDay), str_last_day);
            day = last_day;
        }
        AutoCalcAge(year, month, day);
    }
    return;
}

void ViewNewPat::BirthDateDayDelete(GtkEditable *editable, gint start_pos, gint end_pos)
{
    string new_str = gtk_entry_get_text(GTK_ENTRY(editable));
    new_str.erase(start_pos, end_pos);

    if (new_str.length() != 0) {
        int day = atoi(new_str.c_str());
        if (day < 1) {
            g_signal_stop_emission_by_name((gpointer)editable, "delete_text");
            return ;
        }
        const char *year_text = gtk_entry_get_text(GTK_ENTRY(m_entryBirthYear));
        const char *month_text = gtk_entry_get_text(GTK_ENTRY(m_entryBirthMonth));
        if (strlen(year_text) == 0 || strlen(year_text) < 4 || strlen(month_text) == 0) {
            return ;
        } else {
            int year = atoi(year_text);
            int month = atoi(month_text);
            AutoCalcAge(year, month, day);
        }
    } else {
     	gtk_entry_set_text(GTK_ENTRY(m_entryAge), "");
        gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboboxAge), 0);
    }
}

gboolean ViewNewPat::WindowDeleteEvent(GtkWidget *widget, GdkEvent *event)
{
    DestroyWindow();

    return FALSE;
}

void ViewNewPat::DestroyWindow(void)
{
    if(GTK_IS_WIDGET(m_window))	{
        g_keyInterface.Pop();
        gtk_widget_destroy(m_window);
        if (g_keyInterface.Size() == 1)
            SetSystemCursor(SYSCURSOR_X, SYSCUROSR_Y);
        m_window = NULL;
    }
}
GtkWidget* ViewNewPat::create_note_general(void)
{
	GtkWidget *fixed_tab_general;
	// GtkWidget *entry_stature;
	// GtkWidget *entry_weight;
	// GtkWidget *entry_BSA;
	GtkWidget *label_stature;
	GtkWidget *label_weight;
	GtkWidget *label_BSA;
	GtkWidget *label_cm;
	GtkWidget *label_kg;
	GtkWidget *label_m2;

	fixed_tab_general = gtk_fixed_new ();
	gtk_widget_show (fixed_tab_general);

	m_entry_stature = gtk_entry_new ();
	gtk_widget_show (m_entry_stature);
	gtk_fixed_put (GTK_FIXED (fixed_tab_general), m_entry_stature, 110, 10);
	gtk_widget_set_size_request (m_entry_stature, 100, 30);
	gtk_entry_set_max_length(GTK_ENTRY(m_entry_stature), 3);
	g_signal_connect(G_OBJECT(m_entry_stature), "insert_text", G_CALLBACK(on_entry_stature), this);
	g_signal_connect(G_OBJECT(m_entry_stature), "focus-out-event", G_CALLBACK(HandleStatureFocusOut), this);

	m_entry_weight = gtk_entry_new ();
	gtk_widget_show (m_entry_weight);
	gtk_fixed_put (GTK_FIXED (fixed_tab_general), m_entry_weight, 110, 45);
	gtk_entry_set_max_length(GTK_ENTRY(m_entry_weight), 6);
	gtk_widget_set_size_request (m_entry_weight, 100, 30);
	g_signal_connect(G_OBJECT(m_entry_weight), "insert_text", G_CALLBACK(on_entry_weight), this);
	g_signal_connect(G_OBJECT(m_entry_weight), "focus-out-event", G_CALLBACK(HandleWeightFocusOut), this);

	m_entry_BSA = gtk_entry_new ();
	gtk_widget_show (m_entry_BSA);
	gtk_fixed_put (GTK_FIXED (fixed_tab_general), m_entry_BSA, 110, 80);
	gtk_widget_set_size_request (m_entry_BSA, 100, 30);
	gtk_editable_set_editable (GTK_EDITABLE(m_entry_BSA), FALSE);

	label_stature = gtk_label_new (_("Stature:"));
	gtk_widget_show (label_stature);
	gtk_fixed_put (GTK_FIXED (fixed_tab_general), label_stature, 10, 10);
	gtk_widget_set_size_request (label_stature, 100, 30);
	gtk_misc_set_alignment (GTK_MISC (label_stature), 0.9, 0.5);

	label_weight = gtk_label_new (_("Weight:"));
	gtk_widget_show (label_weight);
	gtk_fixed_put (GTK_FIXED (fixed_tab_general), label_weight, 10, 45);
	gtk_widget_set_size_request (label_weight, 100, 30);
	gtk_misc_set_alignment (GTK_MISC (label_weight), 0.9, 0.5);

	label_BSA = gtk_label_new (_("BSA:"));
	gtk_widget_show (label_BSA);
	gtk_fixed_put (GTK_FIXED (fixed_tab_general), label_BSA, 10, 80);
	gtk_widget_set_size_request (label_BSA, 100, 30);
	gtk_misc_set_alignment (GTK_MISC (label_BSA), 0.9, 0.5);

	label_cm = gtk_label_new ("cm");
	gtk_widget_show (label_cm);
	gtk_fixed_put (GTK_FIXED (fixed_tab_general), label_cm, 210, 10);
	gtk_widget_set_size_request (label_cm, 30, 30);

	label_kg = gtk_label_new ("kg");
	gtk_widget_show (label_kg);
	gtk_fixed_put (GTK_FIXED (fixed_tab_general), label_kg, 210, 45);
	gtk_widget_set_size_request (label_kg, 30, 30);

	label_m2 = gtk_label_new ("m²");
	gtk_widget_show (label_m2);
	gtk_fixed_put (GTK_FIXED (fixed_tab_general), label_m2, 210, 80);
	gtk_widget_set_size_request (label_m2, 30, 30);

	return fixed_tab_general;
}

const gchar* ViewNewPat::GetLMP(void)
{
    return gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_ob_date));
}

void ViewNewPat::OBFocusOut(GtkWidget *widget, GdkEventFocus *event)
{
    time_t now;
    time(&now);

    struct tm *ct;
    ct = localtime(&now);

    int year_now = ct->tm_year + 1900;
    int month_now = ct->tm_mon + 1;
    int day_now = ct->tm_mday;

    const gchar *year, *month, *day;
    if(m_date_format == 0)
    {//年/月/日
        year = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_year));
        month = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_month));
        day = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_day));
    }
    else if(m_date_format == 1)
    {//月/日/年
        month = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_year));
        day = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_month));
        year = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_day));
    }
    else
    {//日/月/年
        day = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_year));
        month = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_month));
        year = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_day));
    }

    unsigned char monthdays[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if(!(atoi(year) % 4)) monthdays[1] = 29; ///闰年

    char warming[512];
    if(atoi(month) > 12)
    {
        sprintf(warming, "%s", _("Month out of range"));
        gtk_label_set_text(GTK_LABEL(m_warming_label), warming);
        gtk_widget_show(m_warming_label);
        return;
    }
    else
    {
        gtk_label_set_text(GTK_LABEL(m_warming_label), "");
    }

    if(atoi(day) > monthdays[atoi(month) - 1])
    {
        sprintf(warming, "%s", _("Day out of range"));
        gtk_label_set_text(GTK_LABEL(m_warming_label), warming);
        gtk_widget_show(m_warming_label);
        return;
    }
    else
    {
        gtk_label_set_text(GTK_LABEL(m_warming_label), "");
    }

	//year = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_year));
	if (strlen(year)) {
		if ((g_date_valid_year((GDateYear)atoi(year)) == FALSE) || (atoi(year) > year_now)
                || (abs(atoi(year) - year_now) > 1)
                || (((abs(atoi(year) - year_now)) > 0) && (atoi(month) != 0) && (atoi(month) < month_now)))
        {
            //gtk_entry_set_text(GTK_ENTRY(widget), "");
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_GA), "");
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_EDD), "");
            ///>The entered date of LMP is under the range:

            if(strcmp(_("LMP"), GetLMP())== 0)
            {
                if(m_date_format == 0)
                {//年/月/日
                    sprintf(warming, "%s%d/%d/%d-%d/%d/%d", _("The entered date of LMP is under the range: "),  year_now-1, month_now, day_now, year_now, month_now, day_now);

                }
                else if(m_date_format == 1)
                {//月/日/年
                    sprintf(warming, "%s%d/%d/%d-%d/%d/%d", _("The entered date of LMP is under the range: "),  month_now, day_now, year_now-1, month_now, day_now, year_now);
                }
                else
                {//日/月/年
                    sprintf(warming, "%s%d/%d/%d-%d/%d/%d", _("The entered date of LMP is under the range: "),  day_now, month_now, year_now-1, day_now, month_now, year_now);
                }
            }
            else
            {
                if(m_date_format == 0)
                {//年/月/日
                    sprintf(warming, "%s%d/%d/%d-%d/%d/%d", _("The entered date of Ovul. Date is under the range: "),  year_now-1, month_now, day_now, year_now, month_now, day_now);

                }
                else if(m_date_format == 1)
                {//月/日/年
                    sprintf(warming, "%s%d/%d/%d-%d/%d/%d", _("The entered date of Ovul. Date is under the range: "),  month_now, day_now, year_now-1, month_now, day_now, year_now);
                }
                else
                {//日/月/年
                    sprintf(warming, "%s%d/%d/%d-%d/%d/%d", _("The entered date of Ovul. Date is under the range: "),  day_now, month_now, year_now-1, day_now, month_now, year_now);
                }
            }
            gtk_label_set_text(GTK_LABEL(m_warming_label), warming);
            gtk_widget_show(m_warming_label);
            PRINTF("Invalid Year!\n");
            return;
        }
        else
        {
            ///>clear
            gtk_label_set_text(GTK_LABEL(m_warming_label), "");
        }
	}
	else
	{
		gtk_entry_set_text(GTK_ENTRY(m_entry_ob_GA), "");
		gtk_entry_set_text(GTK_ENTRY(m_entry_ob_EDD), "");
        return;
	}
	//month = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_month));
	if (strlen(month)) {
		if ((g_date_valid_month((GDateMonth)atoi(month)) == FALSE)
                || ((atoi(year) == year_now) && (atoi(month) > month_now))
                || ((atoi(year) < year_now) && (atoi(month) < month_now)))
        {
            if(strcmp(_("LMP"), GetLMP())== 0)
            {
                if(m_date_format == 0)
                {//年/月/日
                    sprintf(warming, "%s%d/%d/%d-%d/%d/%d", _("The entered date of LMP is under the range: "),  year_now-1, month_now, day_now, year_now, month_now, day_now);

                }
                else if(m_date_format == 1)
                {//月/日/年
                    sprintf(warming, "%s%d/%d/%d-%d/%d/%d", _("The entered date of LMP is under the range: "),  month_now, day_now, year_now-1, month_now, day_now, year_now);
                }
                else
                {//日/月/年
                    sprintf(warming, "%s%d/%d/%d-%d/%d/%d", _("The entered date of LMP is under the range: "),  day_now, month_now, year_now-1, day_now, month_now, year_now);
                }
            }
            else
            {
                if(m_date_format == 0)
                {//年/月/日
                    sprintf(warming, "%s%d/%d/%d-%d/%d/%d", _("The entered date of Ovul. Date is under the range: "),  year_now-1, month_now, day_now, year_now, month_now, day_now);

                }
                else if(m_date_format == 1)
                {//月/日/年
                    sprintf(warming, "%s%d/%d/%d-%d/%d/%d", _("The entered date of Ovul. Date is under the range: "),  month_now, day_now, year_now-1, month_now, day_now, year_now);
                }
                else
                {//日/月/年
                    sprintf(warming, "%s%d/%d/%d-%d/%d/%d", _("The entered date of Ovul. Date is under the range: "),  day_now, month_now, year_now-1, day_now, month_now, year_now);
                }
            }

            gtk_label_set_text(GTK_LABEL(m_warming_label), warming);
            //gtk_entry_set_text(GTK_ENTRY(widget), "");
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_GA), "");
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_EDD), "");
            PRINTF("Invalid Month!\n");
			return;
		}
        else
            gtk_label_set_text(GTK_LABEL(m_warming_label), "");
	}
    else
    {
        gtk_label_set_text(GTK_LABEL(m_warming_label), "");
        gtk_entry_set_text(GTK_ENTRY(m_entry_ob_GA), "");
        gtk_entry_set_text(GTK_ENTRY(m_entry_ob_EDD), "");
        return;
    }
	//day = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_day));
	if (strlen(day)) {
		if ((g_date_valid_day((GDateDay)atoi(day)) == FALSE)
                || ((atoi(year) < year_now) && (atoi(month) == month_now) && (atoi(day) < day_now))
                || ((atoi(year) == year_now) && (atoi(month) == month_now) && (atoi(day) > day_now)))
        {

           if(strcmp(_("LMP"), GetLMP())== 0)
            {
                if(m_date_format == 0)
                {//年/月/日
                    sprintf(warming, "%s%d/%d/%d-%d/%d/%d", _("The entered date of LMP is under the range: "),  year_now-1, month_now, day_now, year_now, month_now, day_now);

                }
                else if(m_date_format == 1)
                {//月/日/年
                    sprintf(warming, "%s%d/%d/%d-%d/%d/%d", _("The entered date of LMP is under the range: "),  month_now, day_now, year_now-1, month_now, day_now, year_now);
                }
                else
                {//日/月/年
                    sprintf(warming, "%s%d/%d/%d-%d/%d/%d", _("The entered date of LMP is under the range: "),  day_now, month_now, year_now-1, day_now, month_now, year_now);
                }
            }
            else
            {
                if(m_date_format == 0)
                {//年/月/日
                    sprintf(warming, "%s%d/%d/%d-%d/%d/%d", _("The entered date of Ovul. Date is under the range: "),  year_now-1, month_now, day_now, year_now, month_now, day_now);

                }
                else if(m_date_format == 1)
                {//月/日/年
                    sprintf(warming, "%s%d/%d/%d-%d/%d/%d", _("The entered date of Ovul. Date is under the range: "),  month_now, day_now, year_now-1, month_now, day_now, year_now);
                }
                else
                {//日/月/年
                    sprintf(warming, "%s%d/%d/%d-%d/%d/%d", _("The entered date of Ovul. Date is under the range: "),  day_now, month_now, year_now-1, day_now, month_now, year_now);
                }
            }

            gtk_label_set_text(GTK_LABEL(m_warming_label), warming);
			//gtk_entry_set_text(GTK_ENTRY(widget), "");
			gtk_entry_set_text(GTK_ENTRY(m_entry_ob_GA), "");
			gtk_entry_set_text(GTK_ENTRY(m_entry_ob_EDD), "");
			PRINTF("Invalid Day!\n");
			return;
		}
        else
            gtk_label_set_text(GTK_LABEL(m_warming_label), "");
	}
	else
	{
        gtk_label_set_text(GTK_LABEL(m_warming_label), "");
		gtk_entry_set_text(GTK_ENTRY(m_entry_ob_GA), "");
		gtk_entry_set_text(GTK_ENTRY(m_entry_ob_EDD), "");
		return;
	}

    gtk_widget_show(m_warming_label);
    Calc_GA_EDD(year, month, day);
}

void ViewNewPat::Calc_GA_EDD(const gchar *year, const gchar *month, const gchar *day)
{
    if(g_date_valid_dmy((GDateDay)atoi(day), (GDateMonth)atoi(month), (GDateYear)atoi(year))) {
        int cur_year, cur_month, cur_day;
        GetCurrentDate(cur_year, cur_month, cur_day);
        GDate* cur_date = g_date_new_dmy((GDateDay)cur_day, (GDateMonth)cur_month, (GDateYear)cur_year);
        GDate* date = g_date_new_dmy((GDateDay)atoi(day), (GDateMonth)atoi(month), (GDateYear)atoi(year));
        if(g_date_compare(cur_date, date) > 0) {
            char tmp[20];
            if (gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_ob_date)) == 0) {
                int gw = MeaCalcFun::CalcGW_LMP(atoi(year), atoi(month), atoi(day));
                sprintf(tmp, "%dw%dd", gw/7, gw%7);
                gtk_entry_set_text(GTK_ENTRY(m_entry_ob_GA), tmp);
            } else {
                int gw = MeaCalcFun::CalcGW_Ovul(atoi(year), atoi(month), atoi(day));
                sprintf(tmp, "%dw%dd", gw/7, gw%7);
                gtk_entry_set_text(GTK_ENTRY(m_entry_ob_GA), tmp);
            }

            char edcb[20];
            if (gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_ob_date)) == 0) {
                MeaCalcFun::CalcEDCB_LMP(atoi(year), atoi(month), atoi(day), edcb);
                gtk_entry_set_text(GTK_ENTRY(m_entry_ob_EDD), edcb);
            } else {
                MeaCalcFun::CalcEDCB_Ovul(atoi(year), atoi(month), atoi(day), edcb);
                gtk_entry_set_text(GTK_ENTRY(m_entry_ob_EDD), edcb);
            }
        } else {
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_GA), "");
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_EDD), "");
        }
        g_date_free(cur_date);
        g_date_free(date);
    } else {
        gtk_entry_set_text(GTK_ENTRY(m_entry_ob_GA), "");
        gtk_entry_set_text(GTK_ENTRY(m_entry_ob_EDD), "");
        //printf("The Date is not exist!\n");
    }
}

void ViewNewPat::CalcBSA(void)
{
	if (!gtk_entry_get_text_length(GTK_ENTRY(m_entry_stature)) ||
			       !gtk_entry_get_text_length(GTK_ENTRY(m_entry_weight)))
		return ;
	double stature = atof(gtk_entry_get_text(GTK_ENTRY(m_entry_stature)));
	double weight = atof(gtk_entry_get_text(GTK_ENTRY(m_entry_weight)));

	double bsa;
	SysCalculateSetting sysCalc;
	if (sysCalc.GetBSAMethod())
		bsa = pow(weight, 0.425)*pow(stature, 0.725)*71.84/10000; //Occidental
	else
		bsa = pow(weight, 0.425)*pow(stature, 0.725)*73.58/10000; //Oriental
	char bsa_str[10];
	sprintf(bsa_str, "%.2f", bsa);
	gtk_entry_set_text(GTK_ENTRY(m_entry_BSA), bsa_str);

}
void ViewNewPat::StatureFocusOut(GtkWidget *widget, GdkEventFocus *event)
{
	CalcBSA();
}

void ViewNewPat::WeightFocusOut(GtkWidget *widget, GdkEventFocus *event)
{
	CalcBSA();
}

GtkWidget* ViewNewPat::create_note_ob(void)
{
	GtkWidget *fixed_tab_ob;
	// GtkWidget *entry_ob_year;
	// GtkWidget *entry_ob_month;
	// GtkWidget *entry_ob_day;
	GtkWidget *label_ob_year_spacing;
	GtkWidget *label_ob_month_spacing;
	//    GtkWidget *entry_ob_GA;
	//    GtkWidget *entry_ob_EDD;
	//    GtkWidget *entry_ob_gravida;
	//    GtkWidget *entry_ob_ectopic;
	//    GtkWidget *entry_ob_gestations;
	//    GtkWidget *entry_ob_para;
	//    GtkWidget *entry_ob_aborta;
	GtkWidget *label_ob_gravida;
	GtkWidget *label_ob_ectopic;
	GtkWidget *label_ob_gestations;
	GtkWidget *label_ob_para;
	GtkWidget *label_ob_aborta;
	GtkWidget *label_ob_GA;
	GtkWidget *label_ob_edd;
	//    GtkWidget *combobox_ob_date;

	fixed_tab_ob = gtk_fixed_new ();
	gtk_widget_show (fixed_tab_ob);

	m_entry_ob_year = gtk_entry_new ();
	gtk_widget_show (m_entry_ob_year);
	gtk_fixed_put (GTK_FIXED (fixed_tab_ob), m_entry_ob_year, 110, 10);
	gtk_widget_set_size_request (m_entry_ob_year, 48, 30);
	gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_ob_year), 9679);
	g_signal_connect(G_OBJECT(m_entry_ob_year), "insert_text", G_CALLBACK(on_entry_ob_year), this);
	g_signal_connect(G_OBJECT(m_entry_ob_year), "focus-out-event", G_CALLBACK(HandleOBFocusOut), this);

	m_entry_ob_month = gtk_entry_new ();
	gtk_widget_show (m_entry_ob_month);
	gtk_fixed_put (GTK_FIXED (fixed_tab_ob), m_entry_ob_month, 170+5, 10);
	gtk_widget_set_size_request (m_entry_ob_month, /*24*/48, 30);
	gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_ob_month), 9679);
	g_signal_connect(G_OBJECT(m_entry_ob_month), "insert_text", G_CALLBACK(on_entry_ob_month), this);
	g_signal_connect(G_OBJECT(m_entry_ob_month), "focus-out-event", G_CALLBACK(HandleOBFocusOut), this);

	m_entry_ob_day = gtk_entry_new ();
	gtk_widget_show (m_entry_ob_day);
	gtk_fixed_put (GTK_FIXED (fixed_tab_ob), m_entry_ob_day, /*206*/230+13, 10);
	gtk_widget_set_size_request (m_entry_ob_day, /*24*/48, 30);
	gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_ob_day), 9679);
	g_signal_connect(G_OBJECT(m_entry_ob_day), "insert_text", G_CALLBACK(on_entry_ob_day), this);
	g_signal_connect(G_OBJECT(m_entry_ob_day), "focus-out-event", G_CALLBACK(HandleOBFocusOut), this);
    GtkWidget *label_ob_day_spacing;
    if(m_date_format == 0)
    {
        gtk_entry_set_max_length(GTK_ENTRY(m_entry_ob_year), 4);
        gtk_entry_set_max_length(GTK_ENTRY(m_entry_ob_month), 2);
        gtk_entry_set_max_length(GTK_ENTRY(m_entry_ob_day), 2);
        label_ob_year_spacing = gtk_label_new (_("Y  "));
        label_ob_month_spacing = gtk_label_new (_("M  "));
        label_ob_day_spacing = gtk_label_new(_("D  "));
    }
    else if(m_date_format == 1)
    {
        gtk_entry_set_max_length(GTK_ENTRY(m_entry_ob_year), 2);
        gtk_entry_set_max_length(GTK_ENTRY(m_entry_ob_month), 2);
        gtk_entry_set_max_length(GTK_ENTRY(m_entry_ob_day), 4);
        label_ob_year_spacing = gtk_label_new (_("M  "));
        label_ob_month_spacing = gtk_label_new (_("D  "));
        label_ob_day_spacing = gtk_label_new(_("Y  "));
    }
    else
    {
        gtk_entry_set_max_length(GTK_ENTRY(m_entry_ob_year), 2);
        gtk_entry_set_max_length(GTK_ENTRY(m_entry_ob_month), 2);
        gtk_entry_set_max_length(GTK_ENTRY(m_entry_ob_day), 4);
        label_ob_year_spacing = gtk_label_new (_("D  "));
        label_ob_month_spacing = gtk_label_new (_("M  "));
        label_ob_day_spacing = gtk_label_new(_("Y  "));
    }
	gtk_widget_show (label_ob_year_spacing);
	gtk_fixed_put (GTK_FIXED (fixed_tab_ob), label_ob_year_spacing, 158, 10);
	gtk_widget_set_size_request (label_ob_year_spacing, 12+5, 30);

	gtk_widget_show (label_ob_month_spacing);
	gtk_fixed_put (GTK_FIXED (fixed_tab_ob), label_ob_month_spacing, 194+24+5, 10);
	gtk_widget_set_size_request (label_ob_month_spacing, 12+8, 30);

	gtk_widget_show (label_ob_day_spacing);
	gtk_fixed_put (GTK_FIXED (fixed_tab_ob), label_ob_day_spacing, 230+48+13, 10);
	gtk_widget_set_size_request (label_ob_day_spacing, 12+5, 30);

	m_entry_ob_GA = gtk_entry_new ();
	gtk_widget_show (m_entry_ob_GA);
	gtk_fixed_put (GTK_FIXED (fixed_tab_ob), m_entry_ob_GA, 110, 45);
	gtk_widget_set_size_request (m_entry_ob_GA, 100, 30);
	gtk_editable_set_editable (GTK_EDITABLE (m_entry_ob_GA), FALSE);
	gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_ob_GA), 9679);

	m_entry_ob_EDD = gtk_entry_new ();
	gtk_widget_show (m_entry_ob_EDD);
	gtk_fixed_put (GTK_FIXED (fixed_tab_ob), m_entry_ob_EDD, 110, 80);
	gtk_widget_set_size_request (m_entry_ob_EDD, 100, 30);
	gtk_editable_set_editable (GTK_EDITABLE (m_entry_ob_EDD), FALSE);
	gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_ob_EDD), 9679);

    ///>提示语
	m_warming_label = gtk_label_new ("");
	gtk_widget_hide (m_warming_label);
	gtk_fixed_put (GTK_FIXED (fixed_tab_ob), m_warming_label, 10, 180);
	gtk_widget_set_size_request (m_warming_label, 450, 30);
	gtk_misc_set_alignment (GTK_MISC (m_warming_label), 0, 0.5); ///左对齐
    gtk_widget_modify_fg(m_warming_label, GTK_STATE_NORMAL, g_red);

	m_entry_ob_gravida = gtk_entry_new ();
	gtk_widget_show (m_entry_ob_gravida);
	gtk_fixed_put (GTK_FIXED (fixed_tab_ob), m_entry_ob_gravida, 450, 10);
	gtk_widget_set_size_request (m_entry_ob_gravida, 100, 30);
	gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_ob_gravida), 8226);

	m_entry_ob_ectopic = gtk_entry_new ();
	gtk_widget_show (m_entry_ob_ectopic);
	gtk_fixed_put (GTK_FIXED (fixed_tab_ob), m_entry_ob_ectopic, 450, 45);
	gtk_widget_set_size_request (m_entry_ob_ectopic, 100, 30);
	gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_ob_ectopic), 8226);

	m_entry_ob_gestations = gtk_entry_new ();
	gtk_widget_show (m_entry_ob_gestations);
	gtk_fixed_put (GTK_FIXED (fixed_tab_ob), m_entry_ob_gestations, 450, 80);
	gtk_widget_set_size_request (m_entry_ob_gestations, 100, 30);
	gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_ob_gestations), 8226);

	m_entry_ob_para = gtk_entry_new ();
	gtk_widget_show (m_entry_ob_para);
	gtk_fixed_put (GTK_FIXED (fixed_tab_ob), m_entry_ob_para, 450, 115);
	gtk_widget_set_size_request (m_entry_ob_para, 100, 30);
	gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_ob_para), 8226);

	m_entry_ob_aborta = gtk_entry_new ();
	gtk_widget_show (m_entry_ob_aborta);
	gtk_fixed_put (GTK_FIXED (fixed_tab_ob), m_entry_ob_aborta, 450, 150);
	gtk_widget_set_size_request (m_entry_ob_aborta, 100, 30);
	gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_ob_aborta), 8226);

	label_ob_gravida = gtk_label_new (_("Gravida:"));
	gtk_widget_show (label_ob_gravida);
	gtk_fixed_put (GTK_FIXED (fixed_tab_ob), label_ob_gravida, 350, 10);
	gtk_widget_set_size_request (label_ob_gravida, 100, 30);
	gtk_misc_set_alignment (GTK_MISC (label_ob_gravida), 0.9, 0.5);

	label_ob_ectopic = gtk_label_new (_("Ectopic:"));
	gtk_widget_show (label_ob_ectopic);
	gtk_fixed_put (GTK_FIXED (fixed_tab_ob), label_ob_ectopic, 350, 45);
	gtk_widget_set_size_request (label_ob_ectopic, 100, 30);
	gtk_misc_set_alignment (GTK_MISC (label_ob_ectopic), 0.9, 0.5);

	label_ob_gestations = gtk_label_new (_("Gestations:"));
	gtk_widget_show (label_ob_gestations);
	gtk_fixed_put (GTK_FIXED (fixed_tab_ob), label_ob_gestations, 350, 80);
	gtk_widget_set_size_request (label_ob_gestations, 100, 30);
	gtk_misc_set_alignment (GTK_MISC (label_ob_gestations), 0.9, 0.5);

	label_ob_para = gtk_label_new (_("Para:"));
	gtk_widget_show (label_ob_para);
	gtk_fixed_put (GTK_FIXED (fixed_tab_ob), label_ob_para, 350, 115);
	gtk_widget_set_size_request (label_ob_para, 100, 30);
	gtk_misc_set_alignment (GTK_MISC (label_ob_para), 0.9, 0.5);

	label_ob_aborta = gtk_label_new (_("Aborta:"));
	gtk_widget_show (label_ob_aborta);
	gtk_fixed_put (GTK_FIXED (fixed_tab_ob), label_ob_aborta, 350, 150);
	gtk_widget_set_size_request (label_ob_aborta, 100, 30);
	gtk_misc_set_alignment (GTK_MISC (label_ob_aborta), 0.9, 0.5);

	label_ob_GA = gtk_label_new (_("GW:"));
	gtk_widget_show (label_ob_GA);
	gtk_fixed_put (GTK_FIXED (fixed_tab_ob), label_ob_GA, 10, 45);
	gtk_widget_set_size_request (label_ob_GA, 100, 30);
	gtk_misc_set_alignment (GTK_MISC (label_ob_GA), 0.9, 0.5);

	label_ob_edd = gtk_label_new (_("EDD:"));
	gtk_widget_show (label_ob_edd);
	gtk_fixed_put (GTK_FIXED (fixed_tab_ob), label_ob_edd, 10, 80);
	gtk_widget_set_size_request (label_ob_edd, 100, 30);
	gtk_misc_set_alignment (GTK_MISC (label_ob_edd), 0.9, 0.5);

	m_combobox_ob_date = gtk_combo_box_new_text ();
	gtk_widget_show (m_combobox_ob_date);
	gtk_fixed_put (GTK_FIXED (fixed_tab_ob), m_combobox_ob_date, 10, 10);
	gtk_widget_set_size_request (m_combobox_ob_date, 100, 30);
	gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_date), _("LMP"));
	gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_date), _("Ovul. Date"));
	gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ob_date), 0);
	g_signal_connect(G_OBJECT(m_combobox_ob_date), "changed", G_CALLBACK(on_combobox_ob_date_changed), this);

	return fixed_tab_ob;
}

void ViewNewPat::ComboboxOBDateChanged(GtkComboBox *widget)
{
    const gchar *year, *month, *day;
    if(m_date_format == 0)
    {//年/月/日
        year = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_year));
        month = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_month));
        day = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_day));
    }
    else if(m_date_format == 1)
    {//月/日/年
        month = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_year));
        day = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_month));
        year = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_day));
    }
    else
    {//日/月/年
        day = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_year));
        month = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_month));
        year = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_day));
    }

    Calc_GA_EDD(year, month, day);
}

GtkWidget* ViewNewPat::create_note_card(void)
{
	GtkWidget *fixed_tab_card;
	//    GtkWidget *entry_hr;
	//    GtkWidget *entry_bp_high;
	//    GtkWidget *entry_bp_low;
	GtkWidget *label_bp_entry;
	GtkWidget *label_bp;
	GtkWidget *label_hr;
	GtkWidget *label_bpm;
	GtkWidget *label_mmhg;

	fixed_tab_card = gtk_fixed_new ();
	gtk_widget_show (fixed_tab_card);

	m_entry_hr = gtk_entry_new ();
	gtk_widget_show (m_entry_hr);
	gtk_fixed_put (GTK_FIXED (fixed_tab_card), m_entry_hr, 115+28, 15);
	gtk_widget_set_size_request (m_entry_hr, 100, 30);
	gtk_entry_set_max_length(GTK_ENTRY(m_entry_hr), 4);
	gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_hr), 8226);
	g_signal_connect(G_OBJECT(m_entry_hr), "insert_text", G_CALLBACK(on_entry_hr), this);

	m_entry_bp_high = gtk_entry_new ();
	gtk_widget_show (m_entry_bp_high);
	gtk_fixed_put (GTK_FIXED (fixed_tab_card), m_entry_bp_high, 115+28, 55);
	gtk_widget_set_size_request (m_entry_bp_high, 50, 30);
	gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_bp_high), 8226);

	label_bp_entry = gtk_label_new ("/");
	gtk_widget_show (label_bp_entry);
	gtk_fixed_put (GTK_FIXED (fixed_tab_card), label_bp_entry, 165+28, 55);
	gtk_widget_set_size_request (label_bp_entry, 15, 30);

	m_entry_bp_low = gtk_entry_new ();
	gtk_widget_show (m_entry_bp_low);
	gtk_fixed_put (GTK_FIXED (fixed_tab_card), m_entry_bp_low, 180+28, 55);
	gtk_widget_set_size_request (m_entry_bp_low, 50, 30);
	gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_bp_low), 8226);

	label_hr = gtk_label_new (_("HR:"));
	gtk_widget_show (label_hr);
	gtk_fixed_put (GTK_FIXED (fixed_tab_card), label_hr, 5, 15);
	gtk_widget_set_size_request (label_hr, 110, 30);
	gtk_misc_set_alignment (GTK_MISC (label_hr), 0.9, 0.5);

	label_bp = gtk_label_new (_("BP(High/Low):"));
	gtk_widget_show (label_bp);
	gtk_fixed_put (GTK_FIXED (fixed_tab_card), label_bp, 5, 55);
	gtk_widget_set_size_request (label_bp, 110+28, 30);
	gtk_misc_set_alignment (GTK_MISC (label_bp), 0.9, 0.5);

	label_bpm = gtk_label_new ("bpm");
	gtk_widget_show (label_bpm);
	gtk_fixed_put (GTK_FIXED (fixed_tab_card), label_bpm, 215+28, 15);
	gtk_widget_set_size_request (label_bpm, 40, 30);

	label_mmhg = gtk_label_new ("mmHg");
	gtk_widget_show (label_mmhg);
	gtk_fixed_put (GTK_FIXED (fixed_tab_card), label_mmhg, 230+28, 55);
	gtk_widget_set_size_request (label_mmhg, 50, 30);

	return fixed_tab_card;
}

GtkWidget* ViewNewPat::create_note_uro(void)
{
	GtkWidget *fixed_tab_uro;
	//    GtkWidget *entry_uro_psa;
	GtkWidget *label_uro_psa;
	GtkWidget *label_uro_psa_unit;

	fixed_tab_uro = gtk_fixed_new ();
	gtk_widget_show (fixed_tab_uro);

	m_entry_uro_psa = gtk_entry_new ();
	gtk_widget_show (m_entry_uro_psa);
	gtk_fixed_put (GTK_FIXED (fixed_tab_uro), m_entry_uro_psa, 115, 16);
	gtk_widget_set_size_request (m_entry_uro_psa, 100, 30);
	gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_uro_psa), 8226);

	label_uro_psa = gtk_label_new (_("PSA:"));
	gtk_widget_show (label_uro_psa);
	gtk_fixed_put (GTK_FIXED (fixed_tab_uro), label_uro_psa, 15, 15);
	gtk_widget_set_size_request (label_uro_psa, 100, 30);
	gtk_misc_set_alignment (GTK_MISC (label_uro_psa), 0.9, 0.5);

	label_uro_psa_unit = gtk_label_new ("ng / ml");
	gtk_widget_show (label_uro_psa_unit);
	gtk_fixed_put (GTK_FIXED (fixed_tab_uro), label_uro_psa_unit, 215, 15);
	gtk_widget_set_size_request (label_uro_psa_unit, 60, 30);

	return fixed_tab_uro;
}

GtkWidget* ViewNewPat::create_note_other(void)
{
	GtkWidget *fixed_tab_other;

	GtkWidget *label_other_tel;
	//    GtkWidget *entry_other_tel;
	GtkWidget *label_other_address;
	//    GtkWidget *entry_other_address;

	fixed_tab_other = gtk_fixed_new ();
	gtk_widget_show (fixed_tab_other);

//hlx
#ifdef VET
	label_other_tel = gtk_label_new (_("Owner Telephone:"));
#else
	label_other_tel = gtk_label_new (_("Telephone:"));
#endif
	gtk_widget_show (label_other_tel);
	gtk_fixed_put (GTK_FIXED (fixed_tab_other), label_other_tel, 10, 10);
#ifdef VET
	gtk_widget_set_size_request (label_other_tel, 140, 30);
#else
	gtk_widget_set_size_request (label_other_tel, 100, 30);
#endif

	m_entry_other_tel = gtk_entry_new ();
    gtk_entry_set_max_length(GTK_ENTRY(m_entry_other_tel), 20);
	gtk_widget_show (m_entry_other_tel);
#ifdef VET
	gtk_fixed_put (GTK_FIXED (fixed_tab_other), m_entry_other_tel, 150, 10);
#else
	gtk_fixed_put (GTK_FIXED (fixed_tab_other), m_entry_other_tel, 110, 10);
#endif
	gtk_widget_set_size_request (m_entry_other_tel, 150, 30);
	gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_other_tel), 9679);
#ifdef VET
    g_signal_connect(G_OBJECT(m_entry_other_tel), "insert_text", G_CALLBACK(on_entry_name_insert), this);

	label_other_address = gtk_label_new (_("Owner Address:"));
#else
	label_other_address = gtk_label_new (_("Address:"));
#endif
	gtk_widget_show (label_other_address);
	gtk_fixed_put (GTK_FIXED (fixed_tab_other), label_other_address, 10, 45);
#ifdef VET
	gtk_widget_set_size_request (label_other_address, 140, 30);
#else
	gtk_widget_set_size_request (label_other_address, 100, 30);
#endif

	m_entry_other_address = gtk_entry_new ();
    gtk_entry_set_max_length(GTK_ENTRY(m_entry_other_address), 100);
	gtk_widget_show (m_entry_other_address);
#ifdef VET
	gtk_fixed_put (GTK_FIXED (fixed_tab_other), m_entry_other_address, 150, 45);
#else
	gtk_fixed_put (GTK_FIXED (fixed_tab_other), m_entry_other_address, 110, 45);
#endif

	gtk_widget_set_size_request (m_entry_other_address, 350, 30);
	gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_other_address), 9679);

#ifdef VET
	g_signal_connect(G_OBJECT(m_entry_other_address), "insert_text", G_CALLBACK(on_entry_name_insert), this);
#endif
	return fixed_tab_other;
}

void ViewNewPat::FillPatInfo(const PatientInfo::Patient &pat_info)
{
    ostringstream strm;
    gtk_entry_set_text(GTK_ENTRY(m_entryPatID), pat_info.id.c_str());
    //gtk_widget_set_sensitive(m_entryPatID, FALSE);
    //gtk_widget_set_sensitive(m_checkbuttonPatID, FALSE);
#ifdef VET
    gtk_entry_set_text(GTK_ENTRY(m_entryAnimalName), pat_info.animal_name.c_str());
    gtk_entry_set_text(GTK_ENTRY(gtk_bin_get_child(GTK_BIN(m_comboboxOwnerName))), pat_info.owner_name.c_str());
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboboxSpecies), pat_info.species);
#else
    gtk_entry_set_text(GTK_ENTRY(m_entryNameLast), pat_info.name.last.c_str());
    gtk_entry_set_text(GTK_ENTRY(m_entryNameFirst), pat_info.name.first.c_str());
    gtk_entry_set_text(GTK_ENTRY(m_entryNameMid), pat_info.name.mid.c_str());
#endif

    gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboboxGender), pat_info.sex);

    if (pat_info.birthDate.year) {
        strm.str("");
        strm << pat_info.birthDate.year;
        gtk_entry_set_text(GTK_ENTRY(m_entryBirthYear), strm.str().c_str());
    }
    if (pat_info.birthDate.month) {
        strm.str("");
        strm << pat_info.birthDate.month;
        gtk_entry_set_text(GTK_ENTRY(m_entryBirthMonth), strm.str().c_str());
    }
    if (pat_info.birthDate.day) {
        strm.str("");
        strm << pat_info.birthDate.day;
        gtk_entry_set_text(GTK_ENTRY(m_entryBirthDay), strm.str().c_str());
    }

    if (pat_info.age) {
        strm.str("");
        strm << pat_info.age;
        gtk_entry_set_text(GTK_ENTRY(m_entryAge), strm.str().c_str());
        gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboboxAge), pat_info.ageUnit);
    }
    // Other
    gtk_entry_set_text(GTK_ENTRY(m_entry_other_address), pat_info.address.c_str());
    gtk_entry_set_text(GTK_ENTRY(m_entry_other_tel), pat_info.telephone.c_str());
}

void ViewNewPat::FillExamInfo(const PatientInfo::ExamGeneral &exam_info)
{
    ostringstream strm;
    GtkTextBuffer *buffer;
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(m_textview_comment));
    gtk_text_buffer_set_text(buffer, exam_info.comment.c_str(), -1);

#ifdef VET
#else
    // general info
    if (exam_info.height > 0) {
        strm.str("");
        strm << exam_info.height;
        gtk_entry_set_text(GTK_ENTRY(m_entry_stature), strm.str().c_str());
    }
    if (exam_info.weight > 0) {
        strm.str("");
        strm << exam_info.weight;
        gtk_entry_set_text(GTK_ENTRY(m_entry_weight), strm.str().c_str());
    }
    if (exam_info.BSA > 0) {
        strm.str("");
        strm << exam_info.BSA;
        gtk_entry_set_text(GTK_ENTRY(m_entry_BSA), strm.str().c_str());
    }
#endif
   if (!exam_info.examDoctor.empty()) {
        int index = GetComboBoxIndex(GTK_COMBO_BOX (m_comboboxentry_diagnostician), exam_info.examDoctor);
        if (index < 0) {
            gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxentry_diagnostician), exam_info.examDoctor.c_str());
            index = GetComboBoxIndex(GTK_COMBO_BOX (m_comboboxentry_diagnostician), exam_info.examDoctor);
        }
        gtk_combo_box_set_active(GTK_COMBO_BOX (m_comboboxentry_diagnostician), index);
    }
    if (!exam_info.reportDoctor.empty()) {
        int index = GetComboBoxIndex(GTK_COMBO_BOX (m_comboboxentry_physician), exam_info.reportDoctor);
        if (index < 0) {
            gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxentry_physician), exam_info.reportDoctor.c_str());
            index = GetComboBoxIndex(GTK_COMBO_BOX (m_comboboxentry_physician), exam_info.reportDoctor);
        }
        gtk_combo_box_set_active(GTK_COMBO_BOX (m_comboboxentry_physician), index);
    }
}

void ViewNewPat::FillObInfo(const PatientInfo::ObExam &ob)
{
    ostringstream strm;
    string year, month, day;
    if (ob.LMPDate.year != 0 && ob.LMPDate.month != 0 && ob.LMPDate.day != 0) {
        gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ob_date), 0);
        strm.str("");
        strm << ob.LMPDate.year;
        year = strm.str();
        strm.str("");
        strm << ob.LMPDate.month;
        month = strm.str();
        strm.str("");
        strm << ob.LMPDate.day;
        day = strm.str();
        if(m_date_format == 0)
        {//年/月/日
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_year), year.c_str());
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_month), month.c_str());
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_day), day.c_str());
        }
        else if(m_date_format == 1)
        {//月/日/年
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_year), month.c_str());
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_month), day.c_str());
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_day), year.c_str());
        }
        else
        {//日/月/年
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_year), day.c_str());
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_month), month.c_str());
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_day), year.c_str());
        }

        Calc_GA_EDD(year.c_str(), month.c_str(), day.c_str());
    }
    else if (ob.OvulateDate.year != 0 && ob.OvulateDate.month != 0 && ob.OvulateDate.day != 0)
    {
        gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ob_date), 1);
        strm.str("");
        strm << ob.OvulateDate.year;
        year = strm.str();
        strm.str("");
        strm << ob.OvulateDate.month;
        month = strm.str();
        strm.str("");
        strm << ob.OvulateDate.day;
        day = strm.str();
        if(m_date_format == 0)
        {//年/月/日
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_year), year.c_str());
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_month), month.c_str());
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_day), day.c_str());
        }
        else if(m_date_format == 1)
        {//月/日/年
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_year), month.c_str());
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_month), day.c_str());
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_day), year.c_str());
        }
        else
        {//日/月/年
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_year), day.c_str());
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_month), month.c_str());
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_day), year.c_str());
        }

        Calc_GA_EDD(year.c_str(), month.c_str(), day.c_str());
    }

    if (ob.pregCount != -1) {
        strm.str("");
        strm << ob.pregCount;
        gtk_entry_set_text(GTK_ENTRY(m_entry_ob_gravida), strm.str().c_str());
    }
    if (ob.abnormalPregCount != -1) {
        strm.str("");
        strm << ob.abnormalPregCount;
        gtk_entry_set_text(GTK_ENTRY(m_entry_ob_ectopic), strm.str().c_str());
    }
    if (ob.fetusCount != -1) {
        strm.str("");
        strm << ob.fetusCount;
        gtk_entry_set_text(GTK_ENTRY(m_entry_ob_gestations), strm.str().c_str());
    }
    if (ob.deliveryCount != -1) {
        strm.str("");
        strm << ob.deliveryCount;
        gtk_entry_set_text(GTK_ENTRY(m_entry_ob_para), strm.str().c_str());
    }
    if (ob.miscarryCount != -1) {
        strm.str("");
        strm << ob.miscarryCount;
        gtk_entry_set_text(GTK_ENTRY(m_entry_ob_aborta), strm.str().c_str());
    }
}

void ViewNewPat::FillCarInfo(const PatientInfo::CarExam &car)
{
    ostringstream strm;
    //card
    if (car.HR != -1) {
        strm.str("");
        strm << car.HR;
        gtk_entry_set_text(GTK_ENTRY(m_entry_hr), strm.str().c_str());
    }
    if (car.pressureHigh != -1) {
        strm.str("");
        strm << car.pressureHigh;
        gtk_entry_set_text(GTK_ENTRY(m_entry_bp_high), strm.str().c_str());
    }
    if (car.pressureHigh != -1) {
        strm.str("");
        strm << car.pressureLow;
        gtk_entry_set_text(GTK_ENTRY(m_entry_bp_low), strm.str().c_str());
    }
}

void ViewNewPat::FillUroInfo(const PatientInfo::UroExam &uro)
{
    ostringstream strm;
    // URO
    if (uro.PSA != -1) {
        strm.str("");
        strm << uro.PSA;
        gtk_entry_set_text(GTK_ENTRY(m_entry_uro_psa), strm.str().c_str());
    }
}

void ViewNewPat::FillNewPat(const PatientInfo::Info &info)
{
    FillPatInfo(info.p);
    FillExamInfo(info.e);
#ifndef VET
    FillObInfo(info.ob);
#endif
    FillCarInfo(info.car);
    FillUroInfo(info.uro);
}

void ViewNewPat::SetSeneitive(bool sensitive)
{
    gtk_widget_set_sensitive(m_entryPatID, sensitive);
    gtk_widget_set_sensitive(m_checkbuttonPatID, sensitive);
}

void ViewNewPat::BtnSearchClicked(GtkButton *button)
{
    string id, name_last, name_first, name_mid, birth_year, birth_month, birth_day, age;
#ifdef VET
    string animal_name, owner_name;
    char species[2];

#endif
    char gender[2], age_unit[2];
    GetEntryTextForDB(m_entryPatID, id);

#ifdef VET
    GetEntryTextForDB(m_entryAnimalName, animal_name);
    GetEntryTextForDB(gtk_bin_get_child(GTK_BIN(m_comboboxOwnerName)), owner_name);
    int species_index = gtk_combo_box_get_active(GTK_COMBO_BOX(m_comboboxSpecies));
    if (species_index == -1)
        sprintf(species, "%%");// %
    else
        sprintf(species, "%d", species_index);
#else
    GetEntryTextForDB(m_entryNameLast, name_last);
    GetEntryTextForDB(m_entryNameFirst, name_first);
    GetEntryTextForDB(m_entryNameMid, name_mid);
#endif
    GetEntryTextForDB(m_entryBirthYear, birth_year);
    GetEntryTextForDB(m_entryBirthMonth, birth_month);
    GetEntryTextForDB(m_entryBirthDay, birth_day);
    GetEntryTextForDB(m_entryAge, age);

    int gender_index = gtk_combo_box_get_active(GTK_COMBO_BOX(m_comboboxGender));
    if (gender_index == -1)
        sprintf(gender, "%%");// %
    else
        sprintf(gender, "%d", gender_index);

    Database::NewPatSearchTerm term;
    term.id = id;

#ifdef VET
    term.animal_name = animal_name;
    term.owner_name = owner_name;
    term.species = species;
#else
    term.name.last = name_last;
    term.name.first = name_first;
    term.name.mid = name_mid;
#endif
    term.gender = gender;
    term.birthYear = birth_year;

    // format month and day
    char buf[3];
    if (birth_month != "%") {
        int iMon = atoi(birth_month.c_str());
        sprintf(buf, "%d%d", iMon/10, iMon%10);
        birth_month = buf;
    }
    if (birth_day != "%") {
        int iDay = atoi(birth_day.c_str());
        sprintf(buf, "%d%d", iDay/10, iDay%10);
        birth_day = buf;
    }
    term.birthMonth = birth_month;
    term.birthDay = birth_day;
    if (age != "%") {
        int age_unit_index = gtk_combo_box_get_active(GTK_COMBO_BOX(m_comboboxAge));
        sprintf(age_unit, "%d", age_unit_index);
    } else {
        sprintf(age_unit, "%%");
    }
    term.age = age;
    term.ageUnit = age_unit;

    Database db;
    vector<Database::NewPatSearchResult> vecResult;
    db.NewPatSearch(term, vecResult);
    if (vecResult.empty()) {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::INFO, _("No result found!"), NULL);
        return;
    }
    ViewPatSearch::GetInstance()->CreateWindow(GTK_WINDOW(m_window), vecResult);
}

static gboolean ExitWindow(gpointer data)
{
	ViewNewPat *tmp;
    tmp = (ViewNewPat *)data;
    tmp->DestroyWindow();

    return FALSE;
}

void ViewNewPat::KeyEvent(unsigned char keyValue)
{
	FakeXEvent::KeyEvent(keyValue);

	switch(keyValue)
	{
		case KEY_ESC:
		case KEY_PATIENT:
			g_timeout_add(100, ExitWindow, this);
			FakeEscKey();
			break;

		default:
			break;
	}
}
void ViewNewPat::BtnCancelClicked(GtkButton *button)
{
    DestroyWindow();

}

void ViewNewPat::BtnOkClicked(GtkButton *button)
{
    PatientInfo::Info info;
    GetPatInfo(info);

    Database db;
    if (!(db.GetPatIDExist(info.p.id.c_str())).empty() && GTK_WIDGET_IS_SENSITIVE(m_entryPatID)) {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window),
                ViewDialog::ERROR,
                _("Please enter patient ID again!"),
                NULL);
        return ;
    }

    db.GetExamIDNext(info.e.examNum);
#ifndef VET
    //MPPS
#ifdef EMP_3410
    if(CManRegister::GetInstance()->IsAuthorize(CManRegister::Optional[0]))
#else
        if(CDCMRegister::GetMe()->IsAuthorize())
#endif
    {
		EditStudyInfo(info);
        SysDicomSetting sysDicomSetting;
        if(!g_patientInfo.GetExist())
        {
            if(sysDicomSetting.GetMPPS())
            {
                if(!m_flagMPPS)
                {
                    if(CDCMMan::GetMe()->GetDefaultMPPSServiceDevice()=="")
                    {
                        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, _("Please Set the default MPPS service in system setting"), NULL);
                        return ;
                    }
                    CDCMMan::GetMe()->StartMPPS(GetMPPSElement(info));
                    m_flagMPPS = true;
                }
            }
        }
    }
#endif
    g_patientInfo.SetInfo(info);
    TopArea* ptrTopArea = TopArea::GetInstance();
    if(ptrTopArea->GetReadImg())
        ptrTopArea->SetReadImg(TRUE);

    DestroyWindow();
}

#ifndef VET
DCMMPPSELEMENT ViewNewPat::GetMPPSElement(PatientInfo::Info &info)
{
    DCMMPPSELEMENT ms;
    if(ViewWorkList::GetInstance()->GetWorkListQuery()==0)
    {
        ms.msSpecificCharacterSet = "";
        ms.msPatientName = info.p.name.first + info.p.name.mid +info.p.name.last;
        ms.msPatientID = info.p.id;

        string year, mon, day, birthDate,studyDate;
        ChangeDateFormatToString(info.p.birthDate.year, info.p.birthDate.month, info.p.birthDate.day, year, mon, day);
        birthDate = year+mon+day;
        ms.msPatientBirthDate = birthDate;

        if(info.p.sex == 0)
            ms.msPatientSex = "F";
        else if(info.p.sex ==1 )
            ms.msPatientSex = "M";
        else if(info.p.sex ==0 )
            ms.msPatientSex = "O";

        ms.msRequestedProcedureID = "";
        ms.msStudyInstanceUID = "";
        ms.msAccessionNumber = "";
        ms.msStudyDescription = "";
        ms.msRequestedProcedureDescription ="";
        ms.msScheduledProcedureStepID = "";

        ChangeDateFormatToString(info.e.examDate.year, info.e.examDate.month, info.e.examDate.day, year, mon, day);
        studyDate = year+mon+day;
        ms.msStudyStartDate = studyDate;

        string hour, min, sec, studyTime;
        ChangeTimeFormatToString(info.e.examTime.hour, info.e.examTime.minute, info.e.examTime.second, hour, min, sec);
        studyTime = hour + min + sec;
        ms.msStudyStartTime = studyTime;
    }
    else
    {
        ms = ViewWorkList::GetInstance()->GetMPPSElement(info);
    }
    return ms;
}

void ViewNewPat::EditStudyInfo(PatientInfo::Info &info)
{
	SysGeneralSetting sgs;
	 int lang = sgs.GetLanguage();
	 if(!lang)
        m_studyInfo.stPatientName = info.p.name.last + '^' + info.p.name.first + '^' + info.p.name.mid;
	 else
		 m_studyInfo.stPatientName = info.p.name.last + info.p.name.first;

        m_studyInfo.stPatientID = info.p.id;
        string year, mon, day, birthDate,studyDate;
        ChangeDateFormatToString(info.p.birthDate.year, info.p.birthDate.month, info.p.birthDate.day, year, mon, day);
        birthDate = year+mon+day;
        m_studyInfo.stPatientBirthDate = birthDate;
        //st.stPatientBirthDate = info.p.birthDate.year + info.p.birthDate.month + info.p.birthDate.day;
        if(info.p.sex == 0)
            m_studyInfo.stPatientSex = "F";
        else if(info.p.sex ==1 )
            m_studyInfo.stPatientSex = "M";
       else if(info.p.sex ==2 )
            m_studyInfo.stPatientSex = "O";
       else
           m_studyInfo.stPatientSex = "";

        string age = "";
        string ageUnit = "";
        if (info.p.ageUnit == 0)
            ageUnit = "Y";
        else if (info.p.ageUnit == 1)
            ageUnit = "M";
        else if (info.p.ageUnit == 2)
            ageUnit = "D";

        char ageTemp[4];
        if (info.p.age > 999 || info.p.age <= 0)
            age = "";
        else
        {
            sprintf(ageTemp, "%03d", info.p.age);
            age = ageTemp + ageUnit;
        }
        m_studyInfo.stPatientAge = age;

        char size[20]= "\0";
        sprintf(size,"%.2f",info.e.height*0.01);
        CommaToDotLocaleNumeric(size, sizeof(size));
        m_studyInfo.stPatientSize =(string)size;
        char weight[30]= "\0";
        sprintf(weight,"%.1f",info.e.weight);
        CommaToDotLocaleNumeric(weight, sizeof(weight));
        m_studyInfo.stPatientWeight =(string)weight;
        m_studyInfo.stStudyDescription =info.e.comment;
        m_studyInfo.stStudyDoctor = info.e.examDoctor;
        string hospital;
        string machine;
        TopArea::GetInstance()->GetHospitalName(hospital);
        UpgradeMan::GetInstance()->GetMachineType(machine);

        ChangeDateFormatToString(info.e.examDate.year, info.e.examDate.month, info.e.examDate.day, year, mon, day);
        studyDate = year+mon+day;
        m_studyInfo.stStudyDate =studyDate;

        string hour, min, sec, studyTime;
        ChangeTimeFormatToString(info.e.examTime.hour, info.e.examTime.minute, info.e.examTime.second, hour, min, sec);
        studyTime = hour + min + sec;
        m_studyInfo.stStudyTime = studyTime;

        m_studyInfo.stFactoryName = "EMP";
        m_studyInfo.stHospital = hospital;
        m_studyInfo.stMechineType = machine;
        m_studyInfo.stMechineSN ="";
        m_studyInfo.stStudyNo = atoi(info.e.examNum.c_str());

        CDCMMan::GetMe()->EditStudyInfo(m_studyInfo);
}

void ViewNewPat::SetStudyInfo(DCMWORKLISTELEMENT element)
{
    m_studyInfo.stSpecificCharacterSet = element.wlSpecificCharacterSet;
    m_studyInfo.stPatientName = element.wlPatientName;
    m_studyInfo.stPatientID = element.wlPatientID;
    m_studyInfo.stPatientBirthDate = element.wlPatientBirthDate;
    m_studyInfo.stPatientSex = element.wlPatientSex;
    m_studyInfo.stPatientAge = element.wlPatientAge;
    m_studyInfo.stPatientSize = element.wlPatientSize;
    m_studyInfo.stPatientWeight = element.wlPatientWeight;
    m_studyInfo.stStudyInstanceUID = element.wlStudyInstanceUID;
    m_studyInfo.stAccessionNumber = element.wlAccessionNumber;
    m_studyInfo.stStudyDoctor = element.wlStudyDoctor;
    m_studyInfo.stStudyDescription = element.wlStudyDescription;

}

void ViewNewPat::SetStudyInfo(DCMSTUDYELEMENT element)
{
    m_studyInfo = element;
}

void ViewNewPat::UpdateStudyInfo(void)
{
    Database db;
    string examNum;
    db.GetExamIDCurrent(examNum);
    m_studyInfo.stStudyNo = atoi(examNum.c_str());
    CDCMMan::GetMe()->EditStudyInfo(m_studyInfo);
}

void ViewNewPat::ClearStudyInfo(void)
{
    m_studyInfo.stSpecificCharacterSet ="";
    m_studyInfo.stPatientName = "";
    m_studyInfo.stPatientID = "";
    m_studyInfo.stPatientBirthDate = "";
    m_studyInfo.stPatientSex = "";
    m_studyInfo.stPatientAge = "";
    m_studyInfo.stPatientSize = "";
    m_studyInfo.stPatientWeight = "";
    m_studyInfo.stStudyInstanceUID = "";
    m_studyInfo.stAccessionNumber = "";
    m_studyInfo.stStudyDoctor = "";
   m_studyInfo.stStudyDescription = "";
   m_studyInfo.stStudyDate ="";
   m_studyInfo.stStudyTime ="";
   m_studyInfo.stFactoryName ="";
   m_studyInfo.stHospital ="";
   m_studyInfo.stMechineType ="";
   m_studyInfo.stMechineSN ="";
   m_studyInfo.stStudyNo=0;

}
#endif
// DESCRIPTION: 自动生成ID, ID的生成规则, 获取当前系统时间和日期, 按规则排序生成标准ID号,
//              如果标准ID号与数据库中的ID发生冲突, 在标准ID号后加上校验位, 校验位从0开始累加。
// RETURN: 生成的ID号
string ViewNewPat::GenPatID(void)
{
    const unsigned int id_len = 14;
    char id_std[id_len+1];
    int Year, Month, Day, Hour, Minute, Second;
    GetCurrentDateTime(Year, Month, Day, Hour, Minute, Second);
    sprintf(id_std, "%02d%02d%4d%02d%02d%02d", Month, Day, Year, Hour, Minute, Second);
    string ID = id_std;
    Database db;
    while (!(db.GetPatIDExist(ID.c_str())).empty()) {
        if (ID.size() == id_len) {
            ID = ID + "0";
        } else if (ID.size() > id_len) {
            string IDStd = ID.substr(0, 14);
            string IDChk = ID.substr(14);
            int ChkNum = atoi(IDChk.c_str()) + 1;
            ostringstream strm;
            strm << ChkNum;
            ID = IDStd + strm.str();
        }
    }
    return ID;
}

void ViewNewPat::ChkBtnPatIDClicked(GtkButton *button)
{
    SysGeneralSetting sys;

    gboolean value = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button));
    if (value)
    {
        string ID = GenPatID();
	    gtk_entry_set_text(GTK_ENTRY(m_entryPatID), ID.c_str());
        sys.SetAutoGeneratedID(1);
        sys.SyncFile();
    }
    else
    {
        gtk_entry_set_text(GTK_ENTRY(m_entryPatID), "");
        gtk_widget_grab_focus(m_entryPatID);
        sys.SetAutoGeneratedID(0);
        sys.SyncFile();
    }
}

/*
void ViewNewPat::BtnExamPauseClicked(GtkButton *button)
{
	// set patient info
    PatientInfo::Info info;
    GetPatInfo(info);
    g_patientInfo.SetInfo(info);

	// archive patient info to temp area

	// clear data in dialog
    ClearData();
}
*/
#ifndef VET
void ViewNewPat::BtnWorkListClicked(GtkButton *button)
{
    string device = CDCMMan::GetMe()->GetDefaultWorklistServiceDevice();
    if (device == "")
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewNewPat::GetInstance()->GetWindow()), ViewDialog::ERROR, _("Please Set the default worklist service in system setting"), NULL);
        return ;
    }
    ViewWorkList::GetInstance()->CreateWorkListWin(m_window);
}
#endif
void ViewNewPat::BtnExamEndClicked(GtkButton *button)
{
    PatientInfo::Info info;
    GetPatInfo(info);

    Database db;
    if (!(db.GetPatIDExist(info.p.id.c_str())).empty() && GTK_WIDGET_IS_SENSITIVE(m_entryPatID)) {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window),
                ViewDialog::ERROR,
                _("Please enter patient ID again!"),
                NULL);
        return ;
    }

    db.GetExamIDNext(info.e.examNum);

    g_patientInfo.SetInfo(info);
#ifndef VET
#ifdef EMP_3410
    if(CManRegister::GetInstance()->IsAuthorize(CManRegister::Optional[0]))
#else
        if(CDCMRegister::GetMe()->IsAuthorize())
#endif
    {
        SysDicomSetting sysDicomSetting;
        if(!g_patientInfo.GetExist())
        {
            if(sysDicomSetting.GetMPPS())
            {
                if(!m_flagMPPS)
                {
                    if(CDCMMan::GetMe()->GetDefaultMPPSServiceDevice()=="")
                    {
                        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, _("Please Set the default MPPS service in system setting"), NULL);
                        return ;
                    }
                    CDCMMan::GetMe()->StartMPPS(GetMPPSElement(info));
                    m_flagMPPS = true;
                }
            }
        }
    }
#endif
    // end exam
    m_clearAll = true;
    KeyEndExam kee;
    kee.Execute();
}

void ViewNewPat::BtnNewPatClicked(GtkButton *button)
{
    if (g_patientInfo.GetExist()) {
        // end exam
        m_clearAll = true;
        KeyEndExam kee;
        kee.Execute();
    } else {
        //clear patient and exam data in dialog
        ClearData();
        // clear data in patient management
        g_patientInfo.ClearAll();
      #ifndef VET
       ClearStudyInfo();
#endif

    }
#ifdef VET
 if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_checkbuttonPatID)))
    {
        string ID = GenPatID();
        gtk_entry_set_text(GTK_ENTRY(m_entryPatID), ID.c_str());
    }
    else
    {
        gtk_widget_grab_focus(m_entryPatID);
    }
#else
 gtk_widget_grab_focus(m_entryPatID);
#endif
}

void ViewNewPat::BtnNewExamClicked(GtkButton *button)
{
    if (g_patientInfo.GetExist()) {
    // end exam
        m_clearAll = false;
        KeyEndExam kee;
        kee.Execute();
    } else {
        //clear exam data in dialog
        ClearExamData();
        // clear exam data in patient management
        g_patientInfo.ClearExam();
    }
}
#ifdef VET
void ViewNewPat::DoneOwnerName(string name)
{
    Database db;
    vector<string> owner_name;
    vector<string>::const_iterator iter_name;
    vector<string> list_name;
    const unsigned int len = strlen(name.c_str());
    unsigned int cnt = len;

    list_name.clear();
    GtkListStore *store;
    store = GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(m_comboboxOwnerName)));
    gtk_list_store_clear(store);
    if (db.OwnerNameSearch(&owner_name))
    {
        for (iter_name = owner_name.begin(); iter_name<owner_name.end(); iter_name++)
        {
            if (len > strlen((*iter_name).c_str()))
                cnt = strlen((*iter_name).c_str());
            else
                cnt = len;
            if ((strlen(name.c_str())!=0) && (strncmp(name.c_str(), (*iter_name).c_str(), cnt) == 0))
            {
               int size = list_name.size();
               int i = 0;
               if (size>0)
                   for (i=0;i<size;i++)
                       if (strcmp(list_name[i].c_str(), (*iter_name).c_str()) == 0)
                           break;
               if (i==size)
                   list_name.push_back((*iter_name).c_str());
            }
        }
    }

    GtkTreeIter iter;
    for (unsigned int i=0;i<list_name.size();i++)
    {
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, list_name[i].c_str(), -1);
        //printf("name[%d]:%s\n", i,list_name[i].c_str());
    }
}
void ViewNewPat::EntryOwnerNameDelete(GtkEditable *editable, gint start_pos, gint end_pos)
{
    PRINTF("%d %d\n", start_pos, end_pos);
    const gchar *entry_text = gtk_entry_get_text(GTK_ENTRY(gtk_bin_get_child(GTK_BIN(m_comboboxOwnerName))));
    string name = entry_text;
    name = name.erase(start_pos, end_pos-start_pos);
    DoneOwnerName(name);
}

void ViewNewPat::EntryOwnerNameInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position)
{
    gint old_text_length = strlen(gtk_entry_get_text(GTK_ENTRY(editable)));
    const gchar *entry_text;
    entry_text = gtk_entry_get_text(GTK_ENTRY(gtk_bin_get_child(GTK_BIN(m_comboboxOwnerName))));
    string name = entry_text;

    if((old_text_length + new_text_length > gtk_entry_get_max_length(GTK_ENTRY(editable)))
        || (( 1 == new_text_length )&&(IsErrorChar(new_text))))
    {
        gtk_signal_emit_stop(GTK_OBJECT(editable), g_signal_lookup("insert-text", GTK_TYPE_EDITABLE));
    }
    name = name.insert(*position, new_text, new_text_length);
    DoneOwnerName(name);
}

void ViewNewPat::EntryOwnerNameChanged(GtkComboBox *combobox_entry)
{
    const gchar *entry_text = gtk_entry_get_text(GTK_ENTRY(gtk_bin_get_child(GTK_BIN(combobox_entry))));
    //if (entry_text == NULL || strlen(entry_text) == 0)
    //    return;
    PRINTF("entry_text:%s\n", entry_text);
}
#endif

void ViewNewPat::CommentInsert(GtkTextBuffer *textbuffer, GtkTextIter *location, gchar *text, gint len)
{
    if (g_ascii_strcasecmp(text, "'") == 0)
        g_signal_stop_emission_by_name((gpointer)textbuffer, "insert_text");
    return;
}

void ViewNewPat::EntryNumInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position)
{
    if (!g_ascii_isdigit(*new_text) && g_ascii_strcasecmp(new_text, ".") != 0)
        g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
    return;
}

//>加入new_text_length==1, 为的时候setText的时候，当字符串首字符为-\_时，不会激活stop信号 lhm06.14
void ViewNewPat::EntryAlNumInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position)
{
    if (!g_ascii_isalnum(*new_text))
    {
        if(((g_ascii_strcasecmp(new_text, "-") !=0) && (g_ascii_strcasecmp(new_text, "_") !=0) &&  (g_ascii_strcasecmp(new_text, "/") !=0)) && (new_text_length == 1))
        {
            g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
        }
    }
    return;
}

void ViewNewPat::EntryNameInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position)
{
    gint old_text_length = strlen(gtk_entry_get_text(GTK_ENTRY(editable)));

    if((old_text_length + new_text_length > gtk_entry_get_max_length(GTK_ENTRY(editable)))||(g_ascii_strcasecmp(new_text, "'")==0)){
        gtk_signal_emit_stop(GTK_OBJECT(editable), g_signal_lookup("insert-text", GTK_TYPE_EDITABLE));
    }
}

// void ViewNewPat::AgeFocusIn(GtkWidget *widget, GdkEventFocus *event)
// {
//     const char *year_text = gtk_entry_get_text(GTK_ENTRY(m_entryBirthYear));
//     const char *month_text = gtk_entry_get_text(GTK_ENTRY(m_entryBirthMonth));
//     const char *day_text = gtk_entry_get_text(GTK_ENTRY(m_entryBirthDay));
//     int year = atoi(year_text);
//     int month = atoi(month_text);
//     int day = atoi(day_text);

//     if (year != 0) {
//         if (month == 0)
//             month = 1;
//         if (day == 0)
//             day = 1;
//         AutoCalcAge(year, month, day);
//     }
// }

bool ViewNewPat::AutoCalcAge(const int year, const int month, const int day)
{
    // const char *year_text = gtk_entry_get_text(GTK_ENTRY(m_entryBirthYear));
    // const char *month_text = gtk_entry_get_text(GTK_ENTRY(m_entryBirthMonth));
    // const char *day_text = gtk_entry_get_text(GTK_ENTRY(m_entryBirthDay));

    // if (strlen(year_text) == 0 || strlen(year_text) == 0 || strlen(year_text) == 0)
    //     return false;

    // int year = atoi(year_text);
    // int month = atoi(month_text);
    // int day = atoi(day_text);

    if (g_date_valid_dmy((GDateDay)day, (GDateMonth)month, (GDateYear)year) == FALSE) {
	return false;
    }

    int age, age_unit;
    if (CalcAge (year, month, day, age, age_unit)) {
	ostringstream strm;
	strm << age;
	gtk_entry_set_text(GTK_ENTRY(m_entryAge), strm.str().c_str());
        gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboboxAge), age_unit);
        return true;
    } else {
	gtk_entry_set_text(GTK_ENTRY(m_entryAge), "");
        gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboboxAge), 0);
        return false;
    }
}

void ViewNewPat::ClearData(void)
{
    if (!GTK_IS_WIDGET(m_window))
        return ;

    gtk_entry_set_text(GTK_ENTRY(m_entryPatID), "");
    gtk_widget_set_sensitive(m_entryPatID, TRUE);
    gtk_widget_set_sensitive(m_checkbuttonPatID, TRUE);
#ifdef VET
    gtk_entry_set_text(GTK_ENTRY(m_entryAnimalName), "");
    gtk_entry_set_text(GTK_ENTRY(gtk_bin_get_child(GTK_BIN(m_comboboxOwnerName))), "");
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboboxSpecies), 0);
#else
    gtk_entry_set_text(GTK_ENTRY(m_entryNameFirst), "");
    gtk_entry_set_text(GTK_ENTRY(m_entryNameMid), "");
    gtk_entry_set_text(GTK_ENTRY(m_entryNameLast), "");
#endif
    gtk_entry_set_text(GTK_ENTRY(m_entryAge), "");
    gtk_entry_set_text(GTK_ENTRY(m_entryBirthYear), "");
    gtk_entry_set_text(GTK_ENTRY(m_entryBirthMonth), "");
    gtk_entry_set_text(GTK_ENTRY(m_entryBirthDay), "");
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboboxGender), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboboxAge), 0);
#if 0
    GtkTextBuffer *buffer;
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(m_textview_comment));
    gtk_text_buffer_set_text(buffer, "", -1);

    gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboboxentry_diagnostician), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboboxentry_physician), -1);
#endif
    ClearExamData();
//    MeasureMan::GetInstance()->ClearAllValue();
}

void ViewNewPat::ClearExamData(void)
{
    if (!GTK_IS_WIDGET(m_window))
        return ;
#ifndef VET
    // general
    if(m_clearAll)
    {
        gtk_entry_set_text(GTK_ENTRY(m_entry_stature), "");
        gtk_entry_set_text(GTK_ENTRY(m_entry_weight), "");
        gtk_entry_set_text(GTK_ENTRY(m_entry_BSA), "");
    }
    //ob
    gtk_entry_set_text(GTK_ENTRY(m_entry_ob_year), "");
    gtk_entry_set_text(GTK_ENTRY(m_entry_ob_month), "");
    gtk_entry_set_text(GTK_ENTRY(m_entry_ob_day), "");
    gtk_entry_set_text(GTK_ENTRY(m_entry_ob_GA), "");
    gtk_entry_set_text(GTK_ENTRY(m_entry_ob_EDD), "");
    gtk_entry_set_text(GTK_ENTRY(m_entry_ob_gravida), "");
    gtk_entry_set_text(GTK_ENTRY(m_entry_ob_ectopic), "");
    gtk_entry_set_text(GTK_ENTRY(m_entry_ob_gestations), "");
    gtk_entry_set_text(GTK_ENTRY(m_entry_ob_para), "");
    gtk_entry_set_text(GTK_ENTRY(m_entry_ob_aborta), "");
    //card
    gtk_entry_set_text(GTK_ENTRY(m_entry_hr), "");
    gtk_entry_set_text(GTK_ENTRY(m_entry_bp_high), "");
    gtk_entry_set_text(GTK_ENTRY(m_entry_bp_low), "");
    // URO
    gtk_entry_set_text(GTK_ENTRY(m_entry_uro_psa), "");
    // Other
    if(m_clearAll)
    {
        gtk_entry_set_text(GTK_ENTRY(m_entry_other_tel), "");
        gtk_entry_set_text(GTK_ENTRY(m_entry_other_address), "");
    }

    GtkTextBuffer *buffer;
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(m_textview_comment));
    gtk_text_buffer_set_text(buffer, "", -1);

    gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboboxentry_diagnostician), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboboxentry_physician), -1);
   #endif
}

void ViewNewPat::GetPatInfo(PatientInfo::Info &info)
{
	const gchar *pat_id = gtk_entry_get_text(GTK_ENTRY(m_entryPatID));

#ifdef VET
    const gchar *animal_name = gtk_entry_get_text(GTK_ENTRY(m_entryAnimalName));
    const gchar *owner_name = gtk_entry_get_text(GTK_ENTRY(gtk_bin_get_child(GTK_BIN(m_comboboxOwnerName))));
    int species_index = gtk_combo_box_get_active(GTK_COMBO_BOX(m_comboboxSpecies));
#else
	const gchar *name_last = gtk_entry_get_text(GTK_ENTRY(m_entryNameLast));
	const gchar *name_first = gtk_entry_get_text(GTK_ENTRY(m_entryNameFirst));
	const gchar *name_mid = NULL;
	if (!m_langCN)
		name_mid = gtk_entry_get_text(GTK_ENTRY(m_entryNameMid));
	else
		name_mid = "";
#endif

	const gchar *age_text = gtk_entry_get_text(GTK_ENTRY(m_entryAge));
	int age_unit_index = gtk_combo_box_get_active(GTK_COMBO_BOX(m_comboboxAge));
	const gchar *birth_year = gtk_entry_get_text(GTK_ENTRY(m_entryBirthYear));
	const gchar *birth_month = gtk_entry_get_text(GTK_ENTRY(m_entryBirthMonth));
	const gchar *birth_day = gtk_entry_get_text(GTK_ENTRY(m_entryBirthDay));

	int gender_index = gtk_combo_box_get_active(GTK_COMBO_BOX(m_comboboxGender));
	// if (gender_index == -1)
	// 	gender_index = 0;

	GtkTextBuffer *buffer;
	GtkTextIter start, end;
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(m_textview_comment));
	gtk_text_buffer_get_start_iter(buffer, &start);
	gtk_text_buffer_get_end_iter(buffer, &end);
	const gchar *comment = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

	const gchar *telephone = gtk_entry_get_text(GTK_ENTRY(m_entry_other_tel));
	const gchar *address = gtk_entry_get_text(GTK_ENTRY(m_entry_other_address));

	gchar *diagnostician = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_comboboxentry_diagnostician));
	gchar *physician = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_comboboxentry_physician));

	info.p.id = pat_id;

#ifdef VET
    info.p.animal_name = animal_name;
    info.p.owner_name = owner_name;
    info.p.species = species_index;
#else
	info.p.name.last = name_last;
	info.p.name.first = name_first;
	info.p.name.mid = name_mid;
#endif
	info.p.sex = gender_index;
	info.p.age = atoi(age_text);
	info.p.ageUnit = age_unit_index;
	info.p.birthDate.year = atoi(birth_year);
	info.p.birthDate.month = atoi(birth_month);
	info.p.birthDate.day = atoi(birth_day);
	info.e.comment = comment;
	info.e.examDoctor = diagnostician;
	info.e.reportDoctor = physician;

	int examYear, examMonth, examDay;
	int examHour, examMin, examSec;
	GetCurrentDateTime(examYear, examMonth, examDay, examHour, examMin, examSec);
	info.e.examDate.year = examYear;
	info.e.examDate.month = examMonth;
	info.e.examDate.day = examDay;
	info.e.examTime.hour = examHour;
	info.e.examTime.minute = examMin;
	info.e.examTime.second = examSec;

	TopArea::GetInstance()->GetCheckPart(info.e.examType);

	// general
#ifndef VET
	if (gtk_entry_get_text_length(GTK_ENTRY(m_entry_stature))) {
	    const char *stature = gtk_entry_get_text(GTK_ENTRY(m_entry_stature));
	    info.e.height = atoi(stature);
	} else {
	    info.e.height = 0;
	}
	if (gtk_entry_get_text_length(GTK_ENTRY(m_entry_weight))) {
	    const char *weight = gtk_entry_get_text(GTK_ENTRY(m_entry_weight));
	    info.e.weight = atof(weight);
	} else {
	    info.e.weight = 0.0;
	}
	if (gtk_entry_get_text_length(GTK_ENTRY(m_entry_BSA))) {
	    const char* BSA = gtk_entry_get_text(GTK_ENTRY(m_entry_BSA));
	    info.e.BSA = atof(BSA);
	} else {
	    info.e.BSA = 0.0;
	}

    //ob
    int iYear, iMonth, iDay;
    if(m_date_format == 0)
    {//年/月/日
        iYear = atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_ob_year)));
        iMonth = atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_ob_month)));
        iDay = atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_ob_day)));
    }
    else if(m_date_format == 1)
    {//月/日/年
        iMonth = atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_ob_year)));
        iDay = atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_ob_month)));
        iYear = atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_ob_day)));
    }
    else
    {//日/月/年
        iDay = atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_ob_year)));
        iMonth = atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_ob_month)));
        iYear = atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_ob_day)));
    }
	if (gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_ob_date))) {
        /*
		info.ob.OvulateDate.year = atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_ob_year)));
		info.ob.OvulateDate.month = atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_ob_month)));
		info.ob.OvulateDate.day = atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_ob_day)));
        */
        info.ob.OvulateDate.year = iYear;
        info.ob.OvulateDate.month = iMonth;
        info.ob.OvulateDate.day = iDay;
        info.ob.LMPDate.year = 0;
        info.ob.LMPDate.month = 0;
        info.ob.LMPDate.day = 0;
    } else {
        /*
		info.ob.LMPDate.year = atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_ob_year)));
		info.ob.LMPDate.month = atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_ob_month)));
		info.ob.LMPDate.day = atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_ob_day)));
        */
        info.ob.LMPDate.year = iYear;
        info.ob.LMPDate.month = iMonth;
        info.ob.LMPDate.day = iDay;
        info.ob.OvulateDate.year = 0;
		info.ob.OvulateDate.month = 0;
		info.ob.OvulateDate.day = 0;
	}
	if (gtk_entry_get_text_length(GTK_ENTRY(m_entry_ob_gravida)))
	    info.ob.pregCount= atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_ob_gravida)));
	else
	    info.ob.pregCount = -1;

	if (gtk_entry_get_text_length(GTK_ENTRY(m_entry_ob_ectopic)))
	    info.ob.abnormalPregCount = atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_ob_ectopic)));
	else
	    info.ob.abnormalPregCount = -1;

	if (gtk_entry_get_text_length(GTK_ENTRY(m_entry_ob_gestations)))
	    info.ob.fetusCount= atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_ob_gestations)));
	else
	    info.ob.fetusCount = -1;

	if (gtk_entry_get_text_length(GTK_ENTRY(m_entry_ob_para)))
	    info.ob.deliveryCount= atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_ob_para)));
	else
	    info.ob.deliveryCount = -1;

	if (gtk_entry_get_text_length(GTK_ENTRY(m_entry_ob_aborta)))
	    info.ob.miscarryCount= atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_ob_aborta)));
	else
	    info.ob.miscarryCount = -1;
#endif

	// card
	if (gtk_entry_get_text_length(GTK_ENTRY(m_entry_hr)))
	    info.car.HR = atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_hr)));
	else
	    info.car.HR = -1;

	if (gtk_entry_get_text_length(GTK_ENTRY(m_entry_bp_high)))
	    info.car.pressureHigh = atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_bp_high)));
	else
	    info.car.pressureHigh = -1;

	if (gtk_entry_get_text_length(GTK_ENTRY(m_entry_bp_low)))
	    info.car.pressureLow = atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_bp_low)));
	else
	    info.car.pressureLow = -1;
	// URO
	if (gtk_entry_get_text_length(GTK_ENTRY(m_entry_uro_psa)))
	    info.uro.PSA = atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_uro_psa)));
	else
	    info.uro.PSA = -1;
	// Other
	info.p.telephone = telephone;
	info.p.address = address;
}

void ViewNewPat::EventBoxCalendarPress(GtkWidget *widget, GdkEventButton *event)
{
	ViewCalendar::GetInstance()->CreateWindow(m_window, ViewCalendar::START, this);
}

void ViewNewPat::SetStartDate(int year, int month, int day)
{
	guint i;
    int pos, iCurrent;
	char tmp[10];
	char str[2];

    //add restirction
    time_t now;
    struct tm *tm_now;
    now = time(NULL);
    tm_now = localtime(&now);
    iCurrent =  (tm_now->tm_year + 1900) * 10000 + (tm_now->tm_mon + 1) * 100 + tm_now->tm_mday * 1;
    int iTime = year * 10000 + month * 100 + day * 1;
    if (iTime > iCurrent)
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewNewPat::GetInstance()->GetWindow()),
                ViewDialog::ERROR,
                _("The time input is not correct!"),
                NULL);
    }
    else
    {

        pos = 0;
        gtk_entry_set_text(GTK_ENTRY(m_entryBirthYear), "");
        sprintf(tmp, "%d", year);
        for(i=0; i<strlen(tmp); i++)
        {
            sprintf(str, "%c", tmp[i]);
            gtk_editable_insert_text(GTK_EDITABLE(m_entryBirthYear), str, 1, &pos);
        }

        pos = 0;
        gtk_entry_set_text(GTK_ENTRY(m_entryBirthMonth), "");
        sprintf(tmp, "%d", month);
        for(i=0; i<strlen(tmp); i++)
        {
            sprintf(str, "%c", tmp[i]);
            gtk_editable_insert_text(GTK_EDITABLE(m_entryBirthMonth), str, 1, &pos);
        }

        pos = 0;
        gtk_entry_set_text(GTK_ENTRY(m_entryBirthDay), "");
        sprintf(tmp, "%d", day);
        for(i=0; i<strlen(tmp); i++)
        {
            sprintf(str, "%c", tmp[i]);
            gtk_editable_insert_text(GTK_EDITABLE(m_entryBirthDay), str, 1, &pos);
        }

        // auto fill age
        if (!AutoCalcAge(year, month, day)) {
            PRINTF("Calc Age Error!\n");
        }
    }
}
#ifdef VET
#else
void ViewNewPat::InsertPatientInfo(const char *ID, PatientInfo::Name patientName, string birthDate,string age,const char *sex,const char *size,const char *weight,const char *address,const char *doctor,const char *description)
{
    m_clearAll = true;
    ClearData();

    char size_tmp[256];
    char str[2];

    char birthDate_tmp[256];
    char birth_str[2];
    int birth_pos ;

    gtk_entry_set_text(GTK_ENTRY(m_entryPatID),ID);
    // CheckPatID(ID);
    Database db;
    if(!((db.GetPatIDExist(ID)).empty() && GTK_WIDGET_IS_SENSITIVE(m_entryPatID)))
    {
        gtk_widget_set_sensitive(m_entryPatID,false);
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::QUESTION, _("Patient ID exist, load data?"), CallBackLoadPatData, CallBackAutoPatID);

    }
    else
    {
		char name_last[m_pat_name_max_len+1] = {0};
		char name_first[m_pat_name_max_len+1]= {0};
		char name_mid[m_pat_name_max_len+1]= {0};
		strncpy(name_last, patientName.last.c_str(), m_pat_name_max_len);
		strncpy(name_first, patientName.first.c_str(), m_pat_name_max_len);
		strncpy(name_mid, patientName.mid.c_str(), m_pat_name_max_len);
        gtk_entry_set_text(GTK_ENTRY(m_entryNameLast), name_last);
        gtk_entry_set_text(GTK_ENTRY(m_entryNameFirst), name_first);
        gtk_entry_set_text(GTK_ENTRY(m_entryNameMid), name_mid);

        //Age
        if(strlen(age.c_str())!=0)
        {
        int age_len = strlen(age.c_str());
        char age_tmp[256]="\0";
        char str[256]="\0";
        sprintf(str,"%c%c%c",age[0],age[1],age[2]);
        sprintf(age_tmp,"%d",atoi(str));
            gtk_entry_set_text(GTK_ENTRY(m_entryAge),age_tmp);

            char str_tmp[2];
            sprintf(str_tmp,"%c",age[age_len-1]);
            if(strcmp(str_tmp,"Y")==0)
                gtk_combo_box_set_active (GTK_COMBO_BOX (m_comboboxAge),0);
            else if(strcmp(str_tmp,"M")==0)
                gtk_combo_box_set_active (GTK_COMBO_BOX (m_comboboxAge),1);
            else if(strcmp(str_tmp,"D")==0)
                gtk_combo_box_set_active (GTK_COMBO_BOX (m_comboboxAge),2);
        }
        else
        {
            gtk_entry_set_text(GTK_ENTRY(m_entryAge)," ");
            gtk_combo_box_set_active (GTK_COMBO_BOX (m_comboboxAge),-1);
        }
        //sex
        if (sex !=NULL)
        {
            if(strcmp(sex ,"F")==0)
                gtk_combo_box_set_active (GTK_COMBO_BOX (m_comboboxGender),0);
            else if(strcmp(sex , "M")==0)
                gtk_combo_box_set_active(GTK_COMBO_BOX (m_comboboxGender), 1);
            else if(strcmp(sex , "O")==0)
                gtk_combo_box_set_active(GTK_COMBO_BOX (m_comboboxGender), 2);

        }
        else
            gtk_combo_box_set_active(GTK_COMBO_BOX (m_comboboxGender),-1);

        //birthDate
        if(strlen(birthDate.c_str())!=0)
        {
            int pos ;
            char birth_year[100] = "\0";
            char birth_month[100] = "\0";
            char birth_day[100] = "\0";
            sprintf(birth_year,"%c%c%c%c",birthDate[0],birthDate[1],birthDate[2],birthDate[3]);
            sprintf(birth_month,"%c%c",birthDate[4],birthDate[5]);
            sprintf(birth_day,"%c%c",birthDate[6],birthDate[7]);
            pos = 0;
            gtk_editable_insert_text(GTK_EDITABLE(m_entryBirthYear),birth_year,4,&pos);

            pos = 0;
            gtk_editable_insert_text(GTK_EDITABLE(m_entryBirthMonth),birth_month,2,&pos);

            pos = 0;
            gtk_editable_insert_text(GTK_EDITABLE(m_entryBirthDay),birth_day,2,&pos);
#if 0

            if(birthDate!=NULL)
            {
                birth_pos = 0;
                int birth_len = strlen(birthDate);
                for(int i = 0;i<birth_len-4;i++)
                {
                    sprintf(birth_str,"%c",birthDate[i]);
                    gtk_editable_insert_text(GTK_EDITABLE(m_entryBirthYear), birth_str, 1, &birth_pos);
                }

                birth_pos = 0;
                for(int i = 4;i<birth_len-2;i++)
                {
                    sprintf(birth_str,"%c",birthDate[i]);
                    gtk_editable_insert_text(GTK_EDITABLE(m_entryBirthMonth), birth_str, 1, &birth_pos);
                }
                birth_pos = 0;
                for(int i = 6;i<birth_len;i++)
                {
                    sprintf(birth_str,"%c",birthDate[i]);
                    gtk_editable_insert_text(GTK_EDITABLE(m_entryBirthDay), birth_str, 1, &birth_pos);
                }
            }
#endif
        }
        else
        {
            gtk_entry_set_text(GTK_ENTRY(m_entryBirthYear)," ");
            gtk_entry_set_text(GTK_ENTRY(m_entryBirthMonth)," ");
            gtk_entry_set_text(GTK_ENTRY(m_entryBirthDay)," ");
        }

        char sizeT[256];
        strcpy(sizeT,size);
        char weightT[256];
        strcpy(weightT,weight);
        DotToCommaLocaleNumeric(sizeT, sizeof(sizeT));

        float stature = atof(sizeT);
        int stature_real=stature*100;
        sprintf(size_tmp,"%d",stature_real);
        gtk_entry_set_text(GTK_ENTRY(m_entry_stature),size_tmp);

        char weight_buf[30]="\0";
        DotToCommaLocaleNumeric(weightT, sizeof(weightT));

        float weight_tmp = atof(weightT);
        sprintf(weight_buf,"%.1f",weight_tmp);
        gtk_entry_set_text(GTK_ENTRY(m_entry_weight),weight_buf);
        CalcBSA();
        gtk_entry_set_text(GTK_ENTRY(m_entry_other_address),address);

    }

    GtkWidget *entry = gtk_bin_get_child(GTK_BIN(m_comboboxentry_diagnostician));
    gtk_entry_set_text(GTK_ENTRY(entry),doctor);
    //GtkEntryBuffer *bufferDoctor = gtk_entry_get_buffer(GTK_ENTRY(entry));
    //gtk_entry_buffer_set_text(bufferDoctor, doctor, -1);

    GtkTextBuffer *buffer;
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(m_textview_comment));
    gtk_text_buffer_set_text(buffer, description, -1);

}
#endif