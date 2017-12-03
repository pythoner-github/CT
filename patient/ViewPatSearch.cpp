#include <gtk/gtk.h>
#include "gui_func.h"
#include "gui_global.h"
#include "ViewPatSearch.h"
#include "KeyValueOpr.h"
#include "KeyDef.h"
#include "ViewNewPat.h"

using std::vector;

ViewPatSearch* ViewPatSearch::m_ptrInstance = NULL;

ViewPatSearch::ViewPatSearch()
{
}

ViewPatSearch::~ViewPatSearch()
{
    if (m_ptrInstance != NULL)
	delete m_ptrInstance;
}

ViewPatSearch* ViewPatSearch::GetInstance()
{
    if (m_ptrInstance == NULL)
	m_ptrInstance = new ViewPatSearch;

    return m_ptrInstance;
}

void ViewPatSearch::CreateWindow(GtkWindow *parent, const vector<Database::NewPatSearchResult>& result)
{
    GtkWidget *fixed;
    GtkWidget *button_ok;
    GtkWidget *button_exit;
    GtkWidget *scrolledwindow;
    GtkWidget *treeview;

    m_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (m_window, 660, 460);
    gtk_window_set_title (GTK_WINDOW (m_window), _("Search Info"));
    gtk_window_set_position (GTK_WINDOW (m_window), GTK_WIN_POS_CENTER_ALWAYS);
    gtk_window_set_modal (GTK_WINDOW (m_window), TRUE);
    gtk_window_set_resizable (GTK_WINDOW (m_window), FALSE);
    gtk_window_set_transient_for(GTK_WINDOW(m_window), parent);
    g_signal_connect (G_OBJECT(m_window), "delete-event", G_CALLBACK(on_window_delete_event), this);

    fixed = gtk_fixed_new ();
    gtk_widget_show (fixed);
    gtk_container_add (GTK_CONTAINER (m_window), fixed);

    GtkWidget *image_ok = gtk_image_new_from_stock ("gtk-ok", GTK_ICON_SIZE_BUTTON);
    GtkWidget *label_ok = gtk_label_new_with_mnemonic (_("OK"));
    button_ok = create_button_icon(label_ok, image_ok);
    gtk_fixed_put (GTK_FIXED (fixed), button_ok, 390, 380);
    g_signal_connect ((gpointer) button_ok, "clicked",
		      G_CALLBACK (on_button_ok_clicked),
		      this);
 
    GtkWidget *image_exit = gtk_image_new_from_stock ("gtk-cancel", GTK_ICON_SIZE_BUTTON);
    GtkWidget *label_exit = gtk_label_new_with_mnemonic (_("Exit"));
    button_exit = create_button_icon(label_exit, image_exit);
    gtk_fixed_put (GTK_FIXED (fixed), button_exit, 520, 380);
    g_signal_connect ((gpointer) button_exit, "clicked",
		      G_CALLBACK (on_button_exit_clicked),
		      this);
 
    scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show (scrolledwindow);
    gtk_fixed_put (GTK_FIXED (fixed), scrolledwindow, 20, 10);
    gtk_widget_set_size_request (scrolledwindow, 620, 360);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    GtkCellRenderer *render;
    GtkTreeViewColumn *col;
    treeview = gtk_tree_view_new ();
// ID column
    render = gtk_cell_renderer_text_new();
    col = gtk_tree_view_column_new_with_attributes(_("ID"), render, "text", ID_COL, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), col);
// Name column
#ifdef VET
    render = gtk_cell_renderer_text_new();
    col = gtk_tree_view_column_new_with_attributes(_("Animal Name"), render, "text", ANIMAL_COL, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), col);

    render = gtk_cell_renderer_text_new();
    col = gtk_tree_view_column_new_with_attributes(_("Owner"), render, "text", OWNER_COL, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), col);

    render = gtk_cell_renderer_text_new();
    col = gtk_tree_view_column_new_with_attributes(_("Species"), render, "text", VARIETY_COL, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), col);

#else

    render = gtk_cell_renderer_text_new();
    col = gtk_tree_view_column_new_with_attributes(_("Name"), render, "text", NAME_COL, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), col);
#endif
// Gender column
    render = gtk_cell_renderer_text_new();
    col = gtk_tree_view_column_new_with_attributes(_("Gender"), render, "text", GENDER_COL, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), col);
// BirthDate column
    render = gtk_cell_renderer_text_new();
    col = gtk_tree_view_column_new_with_attributes(_("BirthDate"), render, "text", BIRTHDATE_COL, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), col);
// Age column
    render = gtk_cell_renderer_text_new();
    col = gtk_tree_view_column_new_with_attributes(_("Age"), render, "text", AGE_COL, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), col);
// ExamDate column
    render = gtk_cell_renderer_text_new();
    col = gtk_tree_view_column_new_with_attributes(_("ExamDate"), render, "text", EXAMDATE_COL, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), col);
// ExamID column
    render = gtk_cell_renderer_text_new();
    col = gtk_tree_view_column_new_with_attributes(_("ExamID"), render, "text", EXAMID_COL, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), col);
// ExamType column
    render = gtk_cell_renderer_text_new();
    col = gtk_tree_view_column_new_with_attributes(_("ExamType"), render, "text", EXAMTYPE_COL, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), col);
#ifdef VET
    GtkListStore *store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
#else
    GtkListStore *store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
#endif


 
    GtkTreeIter treeIter;
    vector<Database::NewPatSearchResult>::const_iterator iter;
    for (iter = result.begin(); iter != result.end(); ++iter) {
	gtk_list_store_append (store, &treeIter);
#ifdef VET
	gtk_list_store_set(store, &treeIter,
			   ID_COL, iter->ID.c_str(),
			   ANIMAL_COL, iter->animal_name.c_str(),
			   OWNER_COL, iter->owner_name.c_str(),
			   VARIETY_COL, iter->species.c_str(),
			   GENDER_COL, iter->gender.c_str(),
			   BIRTHDATE_COL, iter->birthDate.c_str(),
			   AGE_COL, iter->age.c_str(),
			   EXAMDATE_COL, iter->examDate.c_str(),
			   EXAMID_COL, iter->examID.c_str(),
			   EXAMTYPE_COL, iter->examType.c_str(),
			   -1);

#else
	gtk_list_store_set(store, &treeIter,
			   ID_COL, iter->ID.c_str(),
			   NAME_COL, iter->name.c_str(),
			   GENDER_COL, iter->gender.c_str(),
			   BIRTHDATE_COL, iter->birthDate.c_str(),
			   AGE_COL, iter->age.c_str(),
			   EXAMDATE_COL, iter->examDate.c_str(),
			   EXAMID_COL, iter->examID.c_str(),
			   EXAMTYPE_COL, iter->examType.c_str(),
			   -1);
#endif
    }
    gtk_tree_view_set_model (GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));
    g_object_unref (store);

    gtk_widget_show (treeview);
    m_selected = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
    gtk_tree_selection_set_mode(m_selected, GTK_SELECTION_SINGLE);
    gtk_container_add (GTK_CONTAINER (scrolledwindow), treeview);
 
    gtk_widget_show(m_window);

    g_keyInterface.Push(this);
    SetSystemCursorToCenter();
   
    return ;
}

gboolean ViewPatSearch::WindowDeleteEvent(GtkWidget *widget, GdkEvent *event)
{
    DestroyWindow();
    return FALSE;
}

void ViewPatSearch::BtnOkClicked(GtkButton *button)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *exam_id;

    if(gtk_tree_selection_get_selected(m_selected, &model, &iter)) {
        gtk_tree_model_get (model, &iter, EXAMID_COL, &exam_id, -1);

        DestroyWindow();

        PatientInfo::Info info;
        Database db;
        db.GetPatExamInfo(exam_id, info);
        ViewNewPat::GetInstance()->FillNewPat(info);
        ViewNewPat::GetInstance()->SetSeneitive(false);
    }
}

void ViewPatSearch::BtnExitClicked(GtkButton *button)
{
    DestroyWindow();
}

void ViewPatSearch::DestroyWindow(void)
{
	if(GTK_IS_WIDGET(m_window))
	{
		g_keyInterface.Pop();
		gtk_widget_destroy(m_window);
        m_window = NULL;
	}
}

void ViewPatSearch::KeyEvent(unsigned char keyValue)
{
    FakeXEvent::KeyEvent(keyValue);
    
    switch(keyValue) {
    case KEY_ESC:
	BtnExitClicked(NULL);
	break;
    default:
	break;
    }
}

