#include <gtk/gtk.h>
#include <sstream>
#include <iostream>
#include "gui_global.h"
#include "gui_func.h"
#include "ViewTemplet.h"
#include "KeyDef.h"
#include "KeyValueOpr.h"
#include "ViewMain.h"
#include "../display/ViewDialog.h"
#include "ViewReport.h"
#include <errno.h>
#include "SysGeneralSetting.h"
#include "ViewSystem.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

using std::string;
using std::ostringstream;

ViewTemplet* ViewTemplet::m_ptrInstance = NULL;
sqlite3 * ViewTemplet::m_db_templet = 0;
const unsigned int maxTextLen = 1000;
  namespace {
        static const char * const kGlossarySql_ch = "./res/glossary_ch.sql";
    }

    namespace {
        static const char * const kGlossarySql_en = "./res/glossary_en.sql";
    }
ViewTemplet::ViewTemplet()
{
}

ViewTemplet::~ViewTemplet()
{
    if(m_ptrInstance != NULL)
	delete m_ptrInstance;
}

ViewTemplet* ViewTemplet::GetInstance()
{
    if(m_ptrInstance == NULL)
	m_ptrInstance = new ViewTemplet;

    return m_ptrInstance;
}

void ViewTemplet::CreateWindow(void)
{
    GtkWidget *fixed;
    GtkWidget *scrolledwindow;
    // GtkWidget *treeview;
    GtkWidget *frame_indication;
    GtkWidget *alignment_indication;
    GtkWidget *fixed_indication;
    // GtkWidget *textview_indication;
    GtkWidget *label_indication;
    GtkWidget *frame_comments;
    GtkWidget *alignment_comments;
    GtkWidget *fixed_comments;
    // GtkWidget *textview_comments;
    GtkWidget *label_comments;
    // GtkWidget *button_insert;
    // GtkWidget *button_delete;
    // GtkWidget *button_copy;
    // GtkWidget *button_paste;
    // GtkWidget *button_transfer;
    // GtkWidget *button_exit;
    // GtkWidget *button_edit;
    // GtkWidget *button_save;
    // GtkWidget *button_return;

    m_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(m_window, SCREEN_WIDTH, SCREEN_HEIGHT);
    gtk_window_set_position (GTK_WINDOW (m_window), GTK_WIN_POS_CENTER);
    gtk_window_set_modal (GTK_WINDOW (m_window), TRUE);
    gtk_window_set_resizable (GTK_WINDOW (m_window), FALSE);
    gtk_window_set_destroy_with_parent (GTK_WINDOW (m_window), TRUE);
    gtk_window_set_decorated (GTK_WINDOW (m_window), FALSE);
	gtk_window_set_transient_for(GTK_WINDOW(m_window), GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()));

    fixed = gtk_fixed_new ();
    gtk_widget_show (fixed);
    gtk_container_add (GTK_CONTAINER (m_window), fixed);

    scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show (scrolledwindow);
    gtk_fixed_put (GTK_FIXED (fixed), scrolledwindow, 20, 20);
    gtk_widget_set_size_request (scrolledwindow, 300, 648);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_OUT);

    GtkTreeModel *model = CreateTreeModel();
    m_treeView = gtk_tree_view_new_with_model(model);
    g_object_unref (model);
    AddTreeViewColumn(GTK_TREE_VIEW(m_treeView));

    gtk_tree_view_set_enable_search (GTK_TREE_VIEW(m_treeView), FALSE);
    gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (m_treeView), TRUE);
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(m_treeView), FALSE);
    gtk_tree_view_expand_all(GTK_TREE_VIEW(m_treeView));

    GtkTreeSelection *selected;
    selected = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeView));
    gtk_tree_selection_set_mode (gtk_tree_view_get_selection(GTK_TREE_VIEW (m_treeView)), GTK_SELECTION_SINGLE);
    g_signal_connect(G_OBJECT(selected), "changed", G_CALLBACK(HandleTreeSelectionChanged), this);

    gtk_widget_add_events(GTK_WIDGET(m_treeView), (gtk_widget_get_events(GTK_WIDGET(m_treeView)) | GDK_BUTTON_RELEASE_MASK));
    g_signal_connect_after(m_treeView, "button_release_event", G_CALLBACK(HandleTempletBtnClicked), this);

    gtk_widget_show (m_treeView);
    gtk_container_add (GTK_CONTAINER (scrolledwindow), m_treeView);

    frame_indication = gtk_frame_new (NULL);
    gtk_widget_show (frame_indication);
    gtk_fixed_put (GTK_FIXED (fixed), frame_indication, 330, 20);
    gtk_widget_set_size_request (frame_indication, 674, 324);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_indication), GTK_SHADOW_OUT);

    alignment_indication = gtk_alignment_new (0.5, 0.5, 1, 1);
    gtk_widget_show (alignment_indication);
    gtk_container_add (GTK_CONTAINER (frame_indication), alignment_indication);

    fixed_indication = gtk_fixed_new ();
    gtk_widget_show (fixed_indication);
    gtk_container_add (GTK_CONTAINER (alignment_indication), fixed_indication);

    m_textview_indication = gtk_text_view_new ();
    gtk_widget_show (m_textview_indication);
    gtk_fixed_put (GTK_FIXED (fixed_indication), m_textview_indication, 10, 10);
    gtk_widget_set_size_request (m_textview_indication, 650, 284);
    gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (m_textview_indication), GTK_WRAP_WORD);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(m_textview_indication), FALSE);
    m_buffer_indication = gtk_text_view_get_buffer(GTK_TEXT_VIEW(m_textview_indication));

    label_indication = gtk_label_new (_("<b>Descriptor:</b>"));
    gtk_widget_show (label_indication);
    gtk_frame_set_label_widget (GTK_FRAME (frame_indication), label_indication);
    gtk_label_set_use_markup (GTK_LABEL (label_indication), TRUE);

    frame_comments = gtk_frame_new (NULL);
    gtk_widget_show (frame_comments);
    gtk_fixed_put (GTK_FIXED (fixed), frame_comments, 330, 354);
    gtk_widget_set_size_request (frame_comments, 674, 324);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_comments), GTK_SHADOW_OUT);

    alignment_comments = gtk_alignment_new (0.5, 0.5, 1, 1);
    gtk_widget_show (alignment_comments);
    gtk_container_add (GTK_CONTAINER (frame_comments), alignment_comments);

    fixed_comments = gtk_fixed_new ();
    gtk_widget_show (fixed_comments);
    gtk_container_add (GTK_CONTAINER (alignment_comments), fixed_comments);

    m_textview_comments = gtk_text_view_new ();
    gtk_widget_show (m_textview_comments);
    gtk_fixed_put (GTK_FIXED (fixed_comments), m_textview_comments, 10, 10);
    gtk_widget_set_size_request (m_textview_comments, 650, 284);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(m_textview_comments), FALSE);
    gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (m_textview_comments), GTK_WRAP_WORD);
    m_buffer_comments = gtk_text_view_get_buffer(GTK_TEXT_VIEW(m_textview_comments));

    label_comments = gtk_label_new (_("<b>Comments:</b>"));
    gtk_widget_show (label_comments);
    gtk_frame_set_label_widget (GTK_FRAME (frame_comments), label_comments);
    gtk_label_set_use_markup (GTK_LABEL (label_comments), TRUE);

    m_button_insert = gtk_button_new_with_mnemonic (_("Insert"));
    gtk_widget_show (m_button_insert);
    gtk_fixed_put (GTK_FIXED (fixed), m_button_insert, 20, 678);
    gtk_widget_set_size_request (m_button_insert, 120, 30);
    g_signal_connect(G_OBJECT(m_button_insert), "clicked", G_CALLBACK(HandleInsertClicked), this);

    m_button_delete = gtk_button_new_with_mnemonic (_("Delete"));
    gtk_widget_show (m_button_delete);
    gtk_fixed_put (GTK_FIXED (fixed), m_button_delete, 150, 678);
    gtk_widget_set_size_request (m_button_delete, 120, 30);
    g_signal_connect(G_OBJECT(m_button_delete), "clicked", G_CALLBACK(HandleDeleteClicked), this);

    m_button_copy = gtk_button_new_with_mnemonic (_("Copy"));
    gtk_widget_show (m_button_copy);
    gtk_fixed_put (GTK_FIXED (fixed), m_button_copy, 20, 718);
    gtk_widget_set_size_request (m_button_copy, 120, 30);
    g_signal_connect(G_OBJECT(m_button_copy), "clicked", G_CALLBACK(HandleCopyClicked), this);

    m_button_paste = gtk_button_new_with_mnemonic (_("Paste"));
    gtk_widget_show (m_button_paste);
    gtk_fixed_put (GTK_FIXED (fixed), m_button_paste, 150, 718);
    gtk_widget_set_size_request (m_button_paste, 120, 30);
    g_signal_connect(G_OBJECT(m_button_paste), "clicked", G_CALLBACK(HandlePasteClicked), this);

    m_button_edit = gtk_button_new_with_mnemonic (_("Edit Templet"));
    gtk_widget_show (m_button_edit);
    gtk_fixed_put (GTK_FIXED (fixed), m_button_edit, 624, 688);
    gtk_widget_set_size_request (m_button_edit, 120, 60);
    g_signal_connect (G_OBJECT(m_button_edit), "clicked", G_CALLBACK (HandleEditClicked), this);

    m_button_transfer = gtk_button_new_with_mnemonic (_("Transfer"));
    gtk_widget_show (m_button_transfer);
    gtk_fixed_put (GTK_FIXED (fixed), m_button_transfer, 754, 688);
    gtk_widget_set_size_request (m_button_transfer, 120, 60);
    g_signal_connect(G_OBJECT(m_button_transfer), "clicked", G_CALLBACK(HandleTransferClicked), this);

    m_button_exit = gtk_button_new_with_mnemonic (_("Exit"));
    gtk_widget_show (m_button_exit);
    gtk_fixed_put (GTK_FIXED (fixed), m_button_exit, 884, 688);
    gtk_widget_set_size_request (m_button_exit, 120, 60);
    g_signal_connect (G_OBJECT(m_button_exit), "clicked", G_CALLBACK (HandleExitClicked), this);

    m_button_save = gtk_button_new_with_mnemonic (_("Save Templet"));
    gtk_widget_show (m_button_save);
    gtk_fixed_put (GTK_FIXED (fixed), m_button_save, 754, 688);
    gtk_widget_set_size_request (m_button_save, 120, 60);
    g_signal_connect (G_OBJECT(m_button_save), "clicked", G_CALLBACK (HandleSaveClicked), this);

    m_button_default = gtk_button_new_with_mnemonic (_("Default\nFactory"));
    gtk_widget_show (m_button_default);
    gtk_fixed_put (GTK_FIXED (fixed), m_button_default, 624, 688);
    gtk_widget_set_size_request (m_button_default, 120+5, 60);
    g_signal_connect (G_OBJECT(m_button_default), "clicked", G_CALLBACK (HandleDefaultClicked), this);

    m_button_return = gtk_button_new_with_mnemonic (_("Return"));
    gtk_widget_show (m_button_return);
    gtk_fixed_put (GTK_FIXED (fixed), m_button_return, 884, 688);
    gtk_widget_set_size_request (m_button_return, 120, 60);
    g_signal_connect (G_OBJECT(m_button_return), "clicked", G_CALLBACK (HandleReturnClicked), this);

    g_keyInterface.Push(this);
    gtk_widget_show(m_window);

    InitTreeView();
    gtk_widget_hide(m_button_insert);
    gtk_widget_hide(m_button_delete);
    gtk_widget_hide(m_button_copy);
    gtk_widget_hide(m_button_paste);
    gtk_widget_hide(m_button_save);
    gtk_widget_hide(m_button_default);
    gtk_widget_hide(m_button_return);

    SetSystemCursorToCenter();
}

void ViewTemplet::DestroyWindow(void)
{
	if(GTK_IS_WIDGET(m_window))
	{
		g_keyInterface.Pop();
		gtk_widget_destroy(m_window);
        m_window = NULL;
	}
}

static gboolean ExitWindow(gpointer data)
{
    ViewTemplet *tmp;
    tmp = (ViewTemplet *)data;
    tmp->DestroyWindow();
    return FALSE;
}

void ViewTemplet::KeyEvent(unsigned char keyValue)
{
	FakeXEvent::KeyEvent(keyValue);

	switch(keyValue)
	{
		case KEY_ESC:
			if(GTK_WIDGET_VISIBLE(m_button_return))
				ReturnClicked(GTK_BUTTON(m_button_return));
			else
			{
				g_timeout_add(100, ExitWindow, this);
				FakeEscKey();
			}
		    break;

		default:
			break;
	}
}

void ViewTemplet::tree_auto_scroll(GtkTreeView *tree_view, GtkTreeIter *iter, GtkTreePath *path, gpointer user_data)
{
	const char *str="";
	static char lastStr[30] = "";
	bool d_press = (bool)user_data;
    str = gtk_tree_path_to_string(path);
    if (d_press)
    {
        gtk_tree_view_collapse_row(tree_view, 
                gtk_tree_path_new_from_string(str));
        return;
    }
    if((!strstr(str, ":")) && (strcmp(lastStr, str)))
    {
        PRINTF("pathaaaaa:%s %s\n", str, lastStr);
        if(strcmp(lastStr, ""))
            gtk_tree_view_collapse_row(tree_view, 
                   gtk_tree_path_new_from_string(lastStr));
        strcpy(lastStr, str);
    }
   
    gtk_tree_view_expand_row(tree_view, path, FALSE);
	gtk_tree_view_set_cursor(GTK_TREE_VIEW(tree_view), path, NULL, FALSE);
	gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(tree_view), path, NULL, TRUE, 0.5, 0.5);
 }

gboolean ViewTemplet::BtnTempletClicked(GtkWidget *widget, GdkEventButton *event)
{
    if (event->button == 3)
        return FALSE;
    else if (event->button == 2)
        return FALSE;
    else if (event->button == 1)
    {
        if (event->type == GDK_BUTTON_RELEASE)
        {
            GtkTreeIter iter;
            GtkTreeModel *model;

            GtkTreeSelection *selection;
            selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widget));

           
            if (gtk_tree_selection_get_selected(selection, &model, &iter) == TRUE)
            {
                GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
                int tree_depth = gtk_tree_path_get_depth(path);
                gtk_tree_path_free (path);

                if(gtk_tree_model_iter_has_child(model, &iter))
                {
                    GtkTreePath *path=gtk_tree_model_get_path(model, &iter);
                    tree_auto_scroll(GTK_TREE_VIEW(widget), &iter, path, (gpointer)0);
                    gtk_tree_path_free(path);
                }
            }
            return TRUE;
        }
    }
    return FALSE;
}

GtkTreeModel* ViewTemplet::CreateTreeModel(void)
{
    GtkTreeIter first_iter;
    GtkTreeIter second_iter;
    GtkTreeIter third_iter;
    GtkTreeIter fourth_iter;

    GtkTreeStore *store = gtk_tree_store_new(1, G_TYPE_STRING);

    gtk_tree_store_append(store, &m_topIter, NULL);
    gtk_tree_store_set(store, &m_topIter, 
		       0, _("Ultrasonic"),
		       -1);

    OpenDB();
    sqlite3_stmt *stmt_f1=NULL;
    string sql_f1 = "SELECT DISTINCT f1 FROM templet";
    if (sqlite3_prepare(m_db_templet, sql_f1.c_str(), sql_f1.size(), &stmt_f1, 0) != SQLITE_OK) {
    	ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, 
    					  _("Database error!"), NULL);//数据库操作失败!
	PRINTF("%s\n", sqlite3_errmsg(m_db_templet));
	return NULL;
    }
    while (sqlite3_step(stmt_f1) != SQLITE_DONE) {
	const char *f1 = (const char *)sqlite3_column_text(stmt_f1, 0);
	if (strcmp(f1, "") == 0)
	    continue;
	gtk_tree_store_append(store, &first_iter, &m_topIter);
	gtk_tree_store_set(store, &first_iter, 0, f1, -1);

	sqlite3_stmt *stmt_f2 = NULL;
	string sql_f2 = "SELECT DISTINCT f2 FROM templet WHERE f1 = ?";
	if (sqlite3_prepare(m_db_templet, sql_f2.c_str(), sql_f2.size(), &stmt_f2, 0) != SQLITE_OK) {
	    PRINTF("%s\n", sqlite3_errmsg(m_db_templet));
	    return NULL;
	}
	if (sqlite3_bind_text(stmt_f2, 1, f1, strlen(f1), SQLITE_TRANSIENT) != SQLITE_OK) {
	    PRINTF("%s\n", sqlite3_errmsg(m_db_templet));
	    sqlite3_finalize(stmt_f2);
	    return NULL;
	}
	while (sqlite3_step(stmt_f2) != SQLITE_DONE) {
	    const char *f2 = (const char *)sqlite3_column_text(stmt_f2, 0);
	    if (strcmp(f2, "") == 0)
		continue;
	    gtk_tree_store_append(store, &second_iter, &first_iter);
	    gtk_tree_store_set(store, &second_iter, 0, f2, -1);

	    sqlite3_stmt *stmt_f3 = NULL;
	    string sql_f3 = "SELECT DISTINCT f3 FROM templet WHERE f1 = ? AND f2 = ?";
	    if (sqlite3_prepare(m_db_templet, sql_f3.c_str(), sql_f3.size(), &stmt_f3, 0) != SQLITE_OK) {
		PRINTF("%s\n", sqlite3_errmsg(m_db_templet));
		return NULL;
	    }
	    if (sqlite3_bind_text(stmt_f3, 1, f1, strlen(f1), SQLITE_TRANSIENT) != SQLITE_OK) {
		PRINTF("%s\n", sqlite3_errmsg(m_db_templet));
		sqlite3_finalize(stmt_f3);
		return NULL;
	    }
	    if (sqlite3_bind_text(stmt_f3, 2, f2, strlen(f2), SQLITE_TRANSIENT) != SQLITE_OK) {
		PRINTF("%s\n", sqlite3_errmsg(m_db_templet));
		sqlite3_finalize(stmt_f3);
		return NULL;
	    }
	    while (sqlite3_step(stmt_f3) != SQLITE_DONE) {
		const char *f3 = (const char *)sqlite3_column_text(stmt_f3, 0);
		if (strcmp(f3, "") == 0)
		    continue;
		gtk_tree_store_append(store, &third_iter, &second_iter);
		gtk_tree_store_set(store, &third_iter, 0, f3, -1);

		sqlite3_stmt *stmt_f4 = NULL;
		string sql_f4 = "SELECT DISTINCT f4 FROM templet WHERE f1 = ? AND f2 = ? AND f3 = ?";
		if (sqlite3_prepare(m_db_templet, sql_f4.c_str(), sql_f4.size(), &stmt_f4, 0) != SQLITE_OK) {
		    PRINTF("%s\n", sqlite3_errmsg(m_db_templet));
		    return NULL;
		}
		if (sqlite3_bind_text(stmt_f4, 1, f1, strlen(f1), SQLITE_TRANSIENT) != SQLITE_OK) {
		    PRINTF("%s\n", sqlite3_errmsg(m_db_templet));
		    sqlite3_finalize(stmt_f4);
		    return NULL;
		}
		if (sqlite3_bind_text(stmt_f4, 2, f2, strlen(f2), SQLITE_TRANSIENT) != SQLITE_OK) {
		    PRINTF("%s\n", sqlite3_errmsg(m_db_templet));
		    sqlite3_finalize(stmt_f4);
		    return NULL;
		}
		if (sqlite3_bind_text(stmt_f4, 3, f3, strlen(f3), SQLITE_TRANSIENT) != SQLITE_OK) {
		    PRINTF("%s\n", sqlite3_errmsg(m_db_templet));
		    sqlite3_finalize(stmt_f4);
		    return NULL;
		}
		while (sqlite3_step(stmt_f4) != SQLITE_DONE) {
		    const char *f4 = (const char *)sqlite3_column_text(stmt_f4, 0);
		    if (strcmp(f4, "") == 0)
			continue;
		    gtk_tree_store_append(store, &fourth_iter, &third_iter);
		    gtk_tree_store_set(store, &fourth_iter, 0, f4, -1);
		}
		sqlite3_finalize(stmt_f4);
	    }
	    sqlite3_finalize(stmt_f3);    
	}
	sqlite3_finalize(stmt_f2);
    }
    sqlite3_finalize(stmt_f1);

    CloseDB();
    return GTK_TREE_MODEL(store);
}
//treeview展开之前做的事情，展开之前先收起所有的菜单
bool ViewTemplet::TempletTestRowExpandBefore(GtkTreeView *treeview, GtkTreeIter *iter, GtkTreePath *path)
{
  gtk_tree_view_collapse_all(treeview);
    return FALSE; //必须要有返回值,才能触发事件
}

void ViewTemplet::AddTreeViewColumn(GtkTreeView *treeview)
{
    GtkTreeViewColumn *column;

    m_renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("", m_renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
}

bool ViewTemplet::OpenDB(void)
{
    SysGeneralSetting sysGS;
    if(sysGS.GetLanguage() ==ZH)
    {
        if (sqlite3_open(TEMPLET_CH_DB, &m_db_templet) != SQLITE_OK) {
            PRINTF("Database Open Error!\n");
            return false;
        }
    }
    else
    {
        if (sqlite3_open(TEMPLET_EN_DB, &m_db_templet) != SQLITE_OK) {
            PRINTF("Database Open Error!\n");
            return false;
        } 
    }
    return true;
}

bool ViewTemplet::CloseDB(void)
{
    if (sqlite3_close(m_db_templet) != SQLITE_OK) {
	PRINTF("Database Close Error:%s\n", sqlite3_errmsg(m_db_templet));
	return false;
    }
    m_db_templet = 0;
    return true;
}

void ViewTemplet::TreeSelectionChanged(GtkTreeSelection *selection)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    //如果没有选中任何结点,直接返回
    if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE)
	return; 
    //如果选中的结点不是叶子结点,设置indication和comment为空,返回
    if (gtk_tree_model_iter_has_child(model, &iter)) {
	gtk_text_buffer_set_text(m_buffer_indication, "", -1);
	gtk_text_buffer_set_text(m_buffer_comments, "", -1);
	return;
    }
    GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
    int tree_depth = gtk_tree_path_get_depth(path);
    gtk_tree_path_free (path);

    char *str1, *str2, *str3, *str4;
    ostringstream stream;

    if (tree_depth == 1) {
	return;
    }
    if (tree_depth == 2) {
	gtk_tree_model_get(model, &iter, 0, &str1, -1);
	stream << "SELECT indication, comments FROM templet WHERE f1 = '" << str1 << "'";
    }
    else if (tree_depth == 3) {
	GtkTreeIter iter_parent;
	gtk_tree_model_get(model, &iter, 0, &str2, -1);
	gtk_tree_model_iter_parent(model, &iter_parent, &iter);
	gtk_tree_model_get(model, &iter_parent, 0, &str1, -1);
	stream << "SELECT indication, comments FROM templet WHERE f1 = '" << str1 << "' AND f2 = '" << str2 << "'";
    }
    else if (tree_depth == 4) {
	GtkTreeIter iter_parent0;
	GtkTreeIter iter_parent1;
	gtk_tree_model_get(model, &iter, 0, &str3, -1);
	gtk_tree_model_iter_parent(model, &iter_parent0, &iter);
	gtk_tree_model_get(model, &iter_parent0, 0, &str2, -1);
	gtk_tree_model_iter_parent(model, &iter_parent1, &iter_parent0);
	gtk_tree_model_get(model, &iter_parent1, 0, &str1, -1);
	stream << "SELECT indication, comments FROM templet WHERE f1 = '" << str1 << "' AND f2 = '" << str2 << "' AND f3 = '" << str3 << "'";
    }
    else if (tree_depth == 5) {
	GtkTreeIter iter_parent0;
	GtkTreeIter iter_parent1;
	GtkTreeIter iter_parent2;
	gtk_tree_model_get(model, &iter, 0, &str4, -1);
	gtk_tree_model_iter_parent(model, &iter_parent0, &iter);
	gtk_tree_model_get(model, &iter_parent0, 0, &str3, -1);
	gtk_tree_model_iter_parent(model, &iter_parent1, &iter_parent0);
	gtk_tree_model_get(model, &iter_parent1, 0, &str2, -1);
	gtk_tree_model_iter_parent(model, &iter_parent2, &iter_parent1);
	gtk_tree_model_get(model, &iter_parent2, 0, &str1, -1);
	stream << "SELECT indication, comments FROM templet WHERE f1 = '" << str1 << "' AND f2 = '" << str2 << "' AND f3 = '" << str3 << "' AND f4 = '" << str4 << "'";
    }

    sqlite3_stmt *stmt = NULL;
    string sql = stream.str();
    string indication, comments;

    OpenDB();
    if (sqlite3_prepare(m_db_templet, sql.c_str(), sql.size(), &stmt, 0) != SQLITE_OK) {
	ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, 
					  _("Database error!"), NULL);//数据库操作失败!
	PRINTF("SELECT ERROR:%s!\n", sqlite3_errmsg(m_db_templet));
	return;
    }
    while (sqlite3_step(stmt) != SQLITE_DONE) {
	if (strcmp(sqlite3_column_name(stmt, 0), "indication") == 0)
	    indication.assign((const char *)sqlite3_column_text(stmt, 0));
	if (strcmp(sqlite3_column_name(stmt, 1), "comments") == 0)
	    comments.assign((const char *)sqlite3_column_text(stmt, 1));
    }
    sqlite3_finalize(stmt);
    
    CloseDB();

    gtk_text_buffer_set_text(m_buffer_indication, indication.c_str(), -1);
    gtk_text_buffer_set_text(m_buffer_comments, comments.c_str(), -1);
}

void ViewTemplet::InsertClicked(GtkButton *button)
{
    char *new_string = _("New Item");//临时插入结点

    GtkTreeSelection *selected_node = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeView));

    GtkTreeModel *model;
    GtkTreeIter iter;
    if (gtk_tree_selection_get_selected(selected_node, &model, &iter) != TRUE) {
    	ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, 
    					  _("Please select the father node before insert!"), NULL); //请先选择待插入结点的父结点
    	return;
    }

    GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
    gint tree_depth = gtk_tree_path_get_depth(path);
    gtk_tree_path_free (path);

    ostringstream stream;

    if (tree_depth == 1) {
    	if (!UniqueItem(new_string)) {
	    GtkTreeIter iter_tmp = InsertUnique(model, &iter, new_string);
    	    gtk_tree_selection_select_iter(selected_node, &iter_tmp);
    	    ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, 
    					      _("Please rename the New Item!"), NULL); //请重命名新增结点!
    	    return;
    	}
	stream << "INSERT INTO templet VALUES('" << new_string << "', '', '', '', '', '')";
    }
    //如果选中的结点为非叶子结点,则在数据库中插入数据,为叶子结点,则更新数据
    else if (tree_depth == 2) {
	char *str;
    	gtk_tree_model_get(model, &iter, 0, &str, -1);
    	if (!UniqueItem(str, new_string)) {
    	    GtkTreeIter iter_tmp = InsertUnique(model, &iter, new_string);
    	    gtk_tree_selection_select_iter(selected_node, &iter_tmp);
    	    ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, 
    					      _("Please rename the New Item!"), NULL);
    	    return;
    	}
    	if (gtk_tree_model_iter_has_child(model, &iter) == TRUE)
	    stream << "INSERT INTO templet VALUES('" << str << "', '" << new_string << "', '', '', '', '')";
    	else
	    stream << "UPDATE templet SET f2 = '" << new_string << "' WHERE f1 = '" << str << "'";
    }
    else if (tree_depth == 3) {
	char *str0, *str1;
	GtkTreeIter parent;
    	gtk_tree_model_get(model, &iter, 0, &str1, -1);
    	gtk_tree_model_iter_parent(model, &parent, &iter);
    	gtk_tree_model_get(model, &parent, 0, &str0, -1);

    	if (!UniqueItem(str0, str1, new_string)) {
    	    GtkTreeIter iter_tmp = InsertUnique(model, &iter, new_string);
    	    gtk_tree_selection_select_iter(selected_node, &iter_tmp);
    	    ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, 
    					      _("Please rename the New Item!"), NULL);
    	    return;
    	}
    	if (gtk_tree_model_iter_has_child(model, &iter) == TRUE)
	    stream << "INSERT INTO templet VALUES('" 
		   << str0 << "', '" 
		   << str1 << "', '" 
		   << new_string << "', '', '', '')";
    	else
	    stream << "UPDATE templet SET f3 = '" << new_string 
		   << "' WHERE f1 = '" << str0 
		   << "' AND f2 = '" << str1 << "'";
    }
    else if (tree_depth == 4) {
	char *str0, *str1, *str2;
	GtkTreeIter parent1, parent2;
    	gtk_tree_model_get(model, &iter, 0, &str2, -1);
    	gtk_tree_model_iter_parent(model, &parent1, &iter);
    	gtk_tree_model_get(model, &parent1, 0, &str1, -1);
    	gtk_tree_model_iter_parent(model, &parent2, &parent1);
    	gtk_tree_model_get(model, &parent2, 0, &str0, -1);

    	if (!UniqueItem(str0, str1, str2, new_string)) {
    	    GtkTreeIter iter_tmp = InsertUnique(model, &iter, new_string);
    	    gtk_tree_selection_select_iter(selected_node, &iter_tmp);
    	    ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, 
    					      _("Please rename the New Item!"), NULL);
    	    return;
    	}
    	if (gtk_tree_model_iter_has_child(model, &iter) == TRUE)
	    stream << "INSERT INTO templet VALUES('" 
		   << str0 << "', '" 
		   << str1 << "', '" 
		   << str2 << "', '" 
		   << new_string << "', '', '')";
    	else
	    stream << "UPDATE templet SET f4 = '" << new_string 
		   << "' WHERE f1 = '" << str0 
		   << "' AND f2 = '" << str1 
		   << "' AND f3 = '" << str2 << "'";
    }
    else if (tree_depth == 5) {
    	ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, 
    					  _("Insert failure! Already the leaf node!"), NULL);//插入失败!已经到达末结点!
    	return;
    }

    char *errmsg = NULL;
    string sql = stream.str();
    OpenDB();
    if (sqlite3_exec(m_db_templet, sql.c_str(), NULL, NULL, &errmsg) != SQLITE_OK) {
    	ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, 
    					  _("Database error!"), NULL);//数据库操作失败!
    	PRINTF("INSERT or UPDATE error:%s!\n", errmsg);
    	return;
    }
    CloseDB();

    GtkTreeIter iter_new;
    gtk_tree_store_append(GTK_TREE_STORE(model), &iter_new, &iter);
    gtk_tree_store_set(GTK_TREE_STORE(model), &iter_new, 0, new_string, -1);
  //选中并展开新增加的结点
    GtkTreePath *new_path = gtk_tree_model_get_path(model, &iter_new);
    gtk_tree_view_expand_to_path(GTK_TREE_VIEW(m_treeView), new_path);

    //设置结点为编辑状态
    GtkTreeViewColumn *column_tree_view;
    column_tree_view = gtk_tree_view_get_column(GTK_TREE_VIEW(m_treeView), 0);

    gtk_tree_view_set_cursor_on_cell(GTK_TREE_VIEW(m_treeView),
    				     new_path,
    				     column_tree_view,
    				     m_renderer,
    				     TRUE);
    gtk_tree_path_free (new_path);

  }

void ViewTemplet::DeleteClicked(GtkButton *button)
{
    GtkTreeSelection *selected_node = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeView));

    GtkTreeModel *model;
    GtkTreeIter iter;
    if (gtk_tree_selection_get_selected(selected_node, &model, &iter) != TRUE) {
    	ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, 
    					  _("Please select one node to be save!"), NULL); //请先选择待删除的结点!
    	return;
    }

    GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
    gint tree_depth = gtk_tree_path_get_depth(path);
    gtk_tree_path_free(path);

    if (tree_depth == 1) {
    	ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, 
    					  _("Delete failure! Can not delete the root!"), NULL); //删除失败!不能删除根结点!
    	return;
    }
    ostringstream stream;
    if (tree_depth == 2) {
	char *str;
	gtk_tree_model_get(model, &iter, 0, &str, -1);
	stream << "DELETE FROM templet WHERE f1 = '" << str << "'";
    }
    else if (tree_depth == 3) {
	char *str0, *str1;
	GtkTreeIter parent;
	gtk_tree_model_get(model, &iter, 0, &str0, -1);
	gtk_tree_model_iter_parent(model, &parent, &iter);
	gtk_tree_model_get(model, &parent, 0, &str1, -1);
	int nChild = gtk_tree_model_iter_n_children(model, &parent);
	if (nChild == 1)
	    stream << "UPDATE templet SET f2 = '' " << "WHERE f1 = '" << str1 << "'";
	else if (nChild > 1)
	    stream << "DELETE FROM templet WHERE f1 = '" << str1 << "' AND f2 = '" << str0 << "'";
	else {
	    PRINTF("Delete Error!");
	    return;
	}
    }
    else if (tree_depth == 4) {
	char *str0, *str1, *str2;
	GtkTreeIter parent1, parent2;
	gtk_tree_model_get(model, &iter, 0, &str0, -1);
	gtk_tree_model_iter_parent(model, &parent1, &iter);
	gtk_tree_model_get(model, &parent1, 0, &str1, -1);
	gtk_tree_model_iter_parent(model, &parent2, &parent1);
	gtk_tree_model_get(model, &parent2, 0, &str2, -1);
	int nChild = gtk_tree_model_iter_n_children(model, &parent1);
	if (nChild == 1)
	    stream << "UPDATE templet SET f3 = '' " << "WHERE f1 = '" << str2 << "' AND f2 = '" << str1 << "'";
	else if (nChild > 1)
	    stream << "DELETE FROM templet WHERE f1 = '" << str2 << "' AND f2 = '" << str1 << "' AND f3 = '" << str0 << "'";
	else {
	    PRINTF("Delete Error!");
	    return;
	}
    }
    else if (tree_depth == 5) {
	char *str0, *str1, *str2, *str3;
	GtkTreeIter parent1, parent2, parent3;
	gtk_tree_model_get(model, &iter, 0, &str0, -1);
	gtk_tree_model_iter_parent(model, &parent1, &iter);
	gtk_tree_model_get(model, &parent1, 0, &str1, -1);
	gtk_tree_model_iter_parent(model, &parent2, &parent1);
	gtk_tree_model_get(model, &parent2, 0, &str2, -1);
	gtk_tree_model_iter_parent(model, &parent3, &parent2);
	gtk_tree_model_get(model, &parent3, 0, &str3, -1);
	int nChild = gtk_tree_model_iter_n_children(model, &parent1);
	if (nChild == 1)
	    stream << "UPDATE templet SET f4 = '' " 
		   << "WHERE f1 = '" 
		   << str3 << "' AND f2 = '" 
		   << str2 << "' AND f3 = '"
		   << str1 << "'";
	else if (nChild > 1)
	    stream << "DELETE FROM templet WHERE f1 = '" 
		   << str3 << "' AND f2 = '" 
		   << str2 << "' AND f3 = '" 
		   << str1 << "' AND f4 = '" 
		   << str0 << "'";
	else {
	    PRINTF("Delete Error!");
	    return;
	}
    }

    char *errmsg = NULL;
    string sql = stream.str();

    OpenDB();
    if (sqlite3_exec(m_db_templet, sql.c_str(), NULL, NULL, &errmsg) != SQLITE_OK) {
    	ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, 
    					  _("Database error!"), NULL);
    	PRINTF("DELETE ERROR:%s!\n", errmsg);
    	return;
    }
    CloseDB();

    //如果将要删除的结点为叶子结点,则删除indication和comments文本框中的值
    if (gtk_tree_model_iter_has_child(model, &iter) == FALSE) {
    	gtk_text_buffer_set_text(m_buffer_indication, "", -1);
    	gtk_text_buffer_set_text(m_buffer_comments, "", -1);
    }

    //记录当前所选结点的父结点
    GtkTreeIter parent_iter;
    gtk_tree_model_iter_parent(model, &parent_iter, &iter);
    GtkTreePath *parent_path = gtk_tree_model_get_path(model, &parent_iter);

    //刷新treeview
    gtk_tree_store_remove(GTK_TREE_STORE(model), &m_topIter);
    gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeView), CreateTreeModel());

    //展开被删除结点的父结点
    gtk_tree_view_expand_to_path(GTK_TREE_VIEW(m_treeView), parent_path);

    gtk_tree_path_free (parent_path);
}


void ViewTemplet::CopyClicked(GtkButton *button)
{
    GtkTreeSelection *selected_node = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeView));
    GtkTreeModel *model;
    GtkTreeIter iter;
    if (gtk_tree_selection_get_selected(selected_node, &model, &iter) != TRUE) {
    	ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, 
    					  _("Please select one node to be copy!"), NULL); //请先选择待复制的结点!
    	return;
    }
    GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
    gint tree_depth = gtk_tree_path_get_depth(path);
    gtk_tree_path_free (path);

    if (tree_depth == 1) {
    	ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, 
    					  _("Copy failure! Can not copy the root!"), NULL); //复制失败!不能复制根结点
    	return;
    }
    // Init m_copyNode
    for (int i = 0; i < 4; i++)
	m_copyNode[i] = "%";

    if (tree_depth == 2) {
	gchar *strtmp;
	GtkTreeIter parent;
    	m_flagDepth = 2;
    	gtk_tree_model_get(model, &iter, 0, &strtmp, -1);
	m_copyNode[0] = strtmp;
    	gtk_tree_model_iter_parent(model, &parent, &iter);
    	gtk_tree_model_get(model, &parent, 0, &strtmp, -1);
	m_parentNode = strtmp;
    }
    else if (tree_depth == 3) {
	gchar *strtmp;
	GtkTreeIter parent;
    	m_flagDepth = 3;
    	gtk_tree_model_get(model, &iter, 0, &strtmp, -1);
	m_copyNode[1] = strtmp;
    	gtk_tree_model_iter_parent(model, &parent, &iter);
    	gtk_tree_model_get(model, &parent, 0, &strtmp, -1);
	m_parentNode = strtmp;
	m_copyNode[0] = strtmp;
    }
    else if (tree_depth == 4) {
	gchar *strtmp;
	GtkTreeIter parent1, parent2;
    	m_flagDepth = 4;
    	gtk_tree_model_get(model, &iter, 0, &strtmp, -1);
	m_copyNode[2] = strtmp;
    	gtk_tree_model_iter_parent(model, &parent1, &iter);
    	gtk_tree_model_get(model, &parent1, 0, &strtmp, -1);
    	m_copyNode[1] = strtmp;
    	m_parentNode =  strtmp;
    	gtk_tree_model_iter_parent(model, &parent2, &parent1);
    	gtk_tree_model_get(model, &parent2, 0, &strtmp, -1);
    	m_copyNode[0] = strtmp;
    }
    else if (tree_depth == 5) {
	gchar *strtmp;
	GtkTreeIter parent1, parent2, parent3;
    	m_flagDepth = 5;
    	gtk_tree_model_get(model, &iter, 0, &strtmp, -1);
    	m_copyNode[3] = strtmp;
    	gtk_tree_model_iter_parent(model, &parent1, &iter);
    	gtk_tree_model_get(model, &parent1, 0, &strtmp, -1);
    	m_copyNode[2] = strtmp;
    	m_parentNode = strtmp;
    	gtk_tree_model_iter_parent(model, &parent2, &parent1);
    	gtk_tree_model_get(model, &parent2, 0, &strtmp, -1);
    	m_copyNode[1] = strtmp;
    	gtk_tree_model_iter_parent(model, &parent3, &parent2);
    	gtk_tree_model_get(model, &parent3, 0, &strtmp, -1);
    	m_copyNode[0] = strtmp;
    }

    ostringstream stream;
    stream << "SELECT * FROM templet WHERE f1 LIKE '" 
	   << m_copyNode[0] << "' AND f2 LIKE '"
	   << m_copyNode[1] << "' AND f3 LIKE '"
	   << m_copyNode[2] << "' AND f4 LIKE '"
	   << m_copyNode[3] << "'";
    
    string sql = stream.str();

    char *errmsg = NULL;
    sqlite3_stmt *stmt = NULL;
    string f1, f2, f3, f4, indication, comments;
    OpenDB();
    // 清空templet_copy数据库
    if (sqlite3_exec(m_db_templet, "DELETE FROM templet_copy", 0, 0, &errmsg) != SQLITE_OK) {
	ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, 
					  _("Database error!"), NULL);//数据库操作失败!
	PRINTF("DELETE templet_copy ERROR:%s!\n", errmsg);
	return;
    }

    if (sqlite3_exec(m_db_templet, "begin transaction", 0, 0, &errmsg) != SQLITE_OK) {
	ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, 
					  _("Database error!"), NULL);//数据库操作失败!
	PRINTF("Transaction error: %s\n", sqlite3_errmsg(m_db_templet));
	sqlite3_free(errmsg);
	return ;
    }

    if (sqlite3_prepare(m_db_templet, sql.c_str(), sql.size(), &stmt, 0) != SQLITE_OK) {
	ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, 
					  _("Database error!"), NULL);//数据库操作失败!
	PRINTF("SELECT ERROR:%s!\n", sqlite3_errmsg(m_db_templet));
	return;
    }
    while (sqlite3_step(stmt) != SQLITE_DONE) {
	if (strcmp(sqlite3_column_name(stmt, 0), "f1") == 0)
	    f1.assign((const char *)sqlite3_column_text(stmt, 0));
	if (strcmp(sqlite3_column_name(stmt, 1), "f2") == 0)
	    f2.assign((const char *)sqlite3_column_text(stmt, 1));
	if (strcmp(sqlite3_column_name(stmt, 2), "f3") == 0)
	    f3.assign((const char *)sqlite3_column_text(stmt, 2));
	if (strcmp(sqlite3_column_name(stmt, 3), "f4") == 0)
	    f4.assign((const char *)sqlite3_column_text(stmt, 3));
	if (strcmp(sqlite3_column_name(stmt, 4), "indication") == 0)
	    indication.assign((const char *)sqlite3_column_text(stmt, 4));
	if (strcmp(sqlite3_column_name(stmt, 5), "comments") == 0)
	    comments.assign((const char *)sqlite3_column_text(stmt, 5));
	stream.str(""); // 清空stream
	stream << "INSERT INTO templet_copy VALUES('" << f1 << "', '" << f2 << "', '" << f3 << "', '" << f4 << "', '"
	       << indication << "', '" << comments << "')";
	sql = stream.str();
	if (sqlite3_exec(m_db_templet, sql.c_str(), 0, 0, &errmsg) != SQLITE_OK) {
	    ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, 
					      _("Database error!"), NULL);//数据库操作失败!
	    PRINTF("INSERT ERROR:%s!\n", errmsg);
	    sqlite3_free(errmsg);
	    sqlite3_exec(m_db_templet, "rollback transaction", 0, 0, 0);
	    return;
	}
    }
    sqlite3_exec(m_db_templet, "commit transaction", 0, 0, 0);
    sqlite3_finalize(stmt);
    
    CloseDB();

    //求结点的高度
    int max_height = 0;
    m_nodeHeight = 0;

    GtkTreeIter child, cur_child, grand_child;
    bool flag;
    flag = gtk_tree_model_iter_children(model, &cur_child, &iter);
    child = cur_child;

    while (flag == TRUE) {
    	m_nodeHeight++;
    	flag = gtk_tree_model_iter_children(model, &grand_child, &cur_child);
    	if (flag == TRUE)
    	    cur_child = grand_child;
    	else {
    	    max_height = (m_nodeHeight > max_height) ? m_nodeHeight : max_height ;
    	    m_nodeHeight = 0;
    	    flag = gtk_tree_model_iter_next(model, &child);
    	    cur_child = child;
    	}
    }
    m_nodeHeight = max_height;
}

void ViewTemplet::PasteClicked(GtkButton *button)
{
    GtkTreeSelection *selected_node = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeView));

    // if (m_copyNode[0].empty()) {
    // 	ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, 
    // 					  _("Please copy the node before paste it!"), NULL);//请先复制结点!再进行粘贴
    // 	return;
    // }
    GtkTreeModel *model;
    GtkTreeIter iter;
    if (gtk_tree_selection_get_selected(selected_node, &model, &iter) != TRUE) {
    	ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, 
    					  _("Please select one node to be paste!"), NULL);//请先选择待粘贴的结点!
    	return;
    }

    GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
    int tree_depth = gtk_tree_path_get_depth(path);

    if (m_nodeHeight >= 3 || tree_depth + m_nodeHeight > 4) {
    	ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, 
    					  _("Can not paste it!"), NULL);//所选结点不能进行粘贴!
    	return;
    }

    gchar *strtmp;
    gtk_tree_model_get(model, &iter, 0, &strtmp, -1);
    if (strcmp(strtmp, m_parentNode.c_str()) == 0) {
    	ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, 
    					  _("Can not paste it in the same parent!"), NULL);//不能在同一父结点下进行粘贴!
    	return;
    }

    sqlite3_stmt *stmt = NULL;
    string f1, f2, f3, f4, indication, comments;
    char *errmsg = NULL;
    string sql = "SELECT * FROM templet_copy";

    ostringstream stream;
    OpenDB();

    if (sqlite3_exec(m_db_templet, "begin transaction", 0, 0, &errmsg) != SQLITE_OK) {
	ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, 
					  _("Database error!"), NULL);//数据库操作失败!
	PRINTF("Transaction error: %s\n", sqlite3_errmsg(m_db_templet));
	sqlite3_free(errmsg);
	return ;
    }

    if (sqlite3_prepare(m_db_templet, sql.c_str(), sql.size(), &stmt, 0) != SQLITE_OK) {
	ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, 
					  _("Database error!"), NULL);//数据库操作失败!
	PRINTF("SELECT ERROR:%s!\n", sqlite3_errmsg(m_db_templet));
	return;
    }
    while (sqlite3_step(stmt) != SQLITE_DONE) {
	if (strcmp(sqlite3_column_name(stmt, 0), "f1") == 0)
	    f1.assign((const char *)sqlite3_column_text(stmt, 0));
	if (strcmp(sqlite3_column_name(stmt, 1), "f2") == 0)
	    f2.assign((const char *)sqlite3_column_text(stmt, 1));
	if (strcmp(sqlite3_column_name(stmt, 2), "f3") == 0)
	    f3.assign((const char *)sqlite3_column_text(stmt, 2));
	if (strcmp(sqlite3_column_name(stmt, 3), "f4") == 0)
	    f4.assign((const char *)sqlite3_column_text(stmt, 3));
	if (strcmp(sqlite3_column_name(stmt, 4), "indication") == 0)
	    indication.assign((const char *)sqlite3_column_text(stmt, 4));
	if (strcmp(sqlite3_column_name(stmt, 5), "comments") == 0)
	    comments.assign((const char *)sqlite3_column_text(stmt, 5));
	stream.str(""); // 清空stream
	switch (m_flagDepth) {
	case 2:
    	    if (tree_depth == 2) {
		char *str_f1;
    		gtk_tree_model_get(model, &iter, 0, &str_f1, -1);
		stream << "INSERT INTO templet VALUES(" << "'"
		       << str_f1 << "', '" << f1 << "', '" << f2 << "', '" << f3 << "', '"
		       << indication << "', '" << comments << "')";
    	    }
    	    else if (tree_depth == 3) {
		char *str_f1, *str_f2;
		GtkTreeIter parent;
    		gtk_tree_model_get(model, &iter, 0, &str_f2, -1);
    		gtk_tree_model_iter_parent(model, &parent, &iter);
    		gtk_tree_model_get(model, &parent, 0, &str_f1, -1);
		stream << "INSERT INTO templet VALUES(" << "'"
		       << str_f1 << "', '" << str_f2 << "', '" << f1 << "', '" << f2 << "', '"
		       << indication << "', '" << comments << "')";
    	    }
    	    else if (tree_depth == 4) {
		char *str_f1, *str_f2, *str_f3;
		GtkTreeIter parent1, parent2;
    		gtk_tree_model_get(model, &iter, 0, &str_f3, -1);
    		gtk_tree_model_iter_parent(model, &parent1, &iter);
    		gtk_tree_model_get(model, &parent1, 0, &str_f2, -1);
    		gtk_tree_model_iter_parent(model, &parent2, &parent1);
    		gtk_tree_model_get(model, &parent2, 0, &str_f1, -1);
		stream << "INSERT INTO templet VALUES(" << "'"
		       << str_f1 << "', '" << str_f2 << "', '" << str_f3 << "', '" << f1 << "', '"
		       << indication << "', '" << comments << "')";
    	    }
	    break;

	case 3:
    	    if (tree_depth == 1){
		stream << "INSERT INTO templet VALUES(" << "'"
		       << f2 << "', '" << f3 << "', '" << f4 << "', '', '"
		       << indication << "', '" << comments << "')";
    	    }
    	    else if (tree_depth == 2) {
		char *str_f1;
    		gtk_tree_model_get(model, &iter, 0, &str_f1, -1);
		stream << "INSERT INTO templet VALUES(" << "'"
		       << str_f1 << "', '" << f2 << "', '" << f3 << "', '" << f4 << "', '"
		       << indication << "', '" << comments << "')";
    	    }
    	    else if (tree_depth == 3) {
		char *str_f1, *str_f2;
		GtkTreeIter parent;
    		gtk_tree_model_get(model, &iter, 0, &str_f2, -1);
    		gtk_tree_model_iter_parent(model, &parent, &iter);
    		gtk_tree_model_get(model, &parent, 0, &str_f1, -1);
		stream << "INSERT INTO templet VALUES(" << "'"
		       << str_f1 << "', '" << str_f2 << "', '" << f2 << "', '" << f3 << "', '"
		       << indication << "', '" << comments << "')";
    	    }
    	    if (tree_depth == 4) {
		char *str_f1, *str_f2, *str_f3;
		GtkTreeIter parent1, parent2;
    		gtk_tree_model_get(model, &iter, 0, &str_f3, -1);
    		gtk_tree_model_iter_parent(model, &parent1, &iter);
    		gtk_tree_model_get(model, &parent1, 0, &str_f2, -1);
    		gtk_tree_model_iter_parent(model, &parent2, &parent1);
    		gtk_tree_model_get(model, &parent2, 0, &str_f1, -1);
		stream << "INSERT INTO templet VALUES(" << "'"
		       << str_f1 << "', '" << str_f2 << "', '" << str_f3 << "', '" << f2 << "', '"
		       << indication << "', '" << comments << "')";
    	    }
	    break;

	case 4:
    	    if (tree_depth == 1) {
		stream << "INSERT INTO templet VALUES(" << "'"
		       << f3 << "', '" << f4 << "', '', '', '"
		       << indication << "', '" << comments << "')";
    	    }
    	    else if (tree_depth == 2) {
		char *str_f1;
    		gtk_tree_model_get(model, &iter, 0, &str_f1, -1);
		stream << "INSERT INTO templet VALUES(" << "'"
		       << str_f1 << "', '" << f3 << "', '" << f4 << "', '', '"
		       << indication << "', '" << comments << "')";
    	    }
    	    else if (tree_depth == 3) {
		char *str_f1, *str_f2;
		GtkTreeIter parent;
    		gtk_tree_model_get(model, &iter, 0, &str_f2, -1);
    		gtk_tree_model_iter_parent(model, &parent, &iter);
    		gtk_tree_model_get(model, &parent, 0, &str_f1, -1);
		stream << "INSERT INTO templet VALUES(" << "'"
		       << str_f1 << "', '" << str_f2 << "', '" << f3 << "', '" << f4 << "', '"
		       << indication << "', '" << comments << "')";
    	    }
    	    else if (tree_depth == 4) {
		char *str_f1, *str_f2, *str_f3;
		GtkTreeIter parent1, parent2;
    		gtk_tree_model_get(model, &iter, 0, &str_f3, -1);
    		gtk_tree_model_iter_parent(model, &parent1, &iter);
    		gtk_tree_model_get(model, &parent1, 0, &str_f2, -1);
    		gtk_tree_model_iter_parent(model, &parent2, &parent1);
    		gtk_tree_model_get(model, &parent2, 0, &str_f1, -1);
		stream << "INSERT INTO templet VALUES(" << "'"
		       << str_f1 << "', '" << str_f2 << "', '" << str_f3 << "', '" << f3 << "', '"
		       << indication << "', '" << comments << "')";
    	    }
	    break;
	case 5:
    	    if (tree_depth == 1) {
		stream << "INSERT INTO templet VALUES(" << "'"
		       << f4 << "', '', '', '', '"
		       << indication << "', '" << comments << "')";
    	    }
    	    else if (tree_depth == 2) {
		char *str_f1;
    		gtk_tree_model_get(model, &iter, 0, &str_f1, -1);
		stream << "INSERT INTO templet VALUES(" << "'"
		       << str_f1 << "', '" << f4 << "', '', '', '"
		       << indication << "', '" << comments << "')";
    	    }
    	    else if (tree_depth == 3) {
		char *str_f1, *str_f2;
		GtkTreeIter parent;
    		gtk_tree_model_get(model, &iter, 0, &str_f2, -1);
    		gtk_tree_model_iter_parent(model, &parent, &iter);
    		gtk_tree_model_get(model, &parent, 0, &str_f1, -1);
		stream << "INSERT INTO templet VALUES(" << "'"
		       << str_f1 << "', '" << str_f2 << "', '" << f4 << "', '', '"
		       << indication << "', '" << comments << "')";
    	    }
    	    else if (tree_depth == 4) {
		char *str_f1, *str_f2, *str_f3;
		GtkTreeIter parent1, parent2;
    		gtk_tree_model_get(model, &iter, 0, &str_f3, -1);
    		gtk_tree_model_iter_parent(model, &parent1, &iter);
    		gtk_tree_model_get(model, &parent1, 0, &str_f2, -1);
    		gtk_tree_model_iter_parent(model, &parent2, &parent1);
    		gtk_tree_model_get(model, &parent2, 0, &str_f1, -1);
		stream << "INSERT INTO templet VALUES(" << "'"
		       << str_f1 << "', '" << str_f2 << "', '" << str_f3 << "', '" << f4 << "', '"
		       << indication << "', '" << comments << "')";
    	    }
	    break;
	}
	sql = stream.str();
	if (sqlite3_exec(m_db_templet, sql.c_str(), 0, 0, &errmsg) != SQLITE_OK) {
    	    ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, _("Database error!"), NULL);// 数据库操作失败!
    	    PRINTF("PASTE ERROR:%s!\n", errmsg);
	    sqlite3_exec(m_db_templet, "rollback transaction", 0, 0, 0);
    	    return;
    	}
    }
    sqlite3_exec(m_db_templet, "commit transaction", 0, 0, 0);
    sqlite3_finalize(stmt);
    CloseDB();

    //刷新treeview,展开将要粘贴的结点
    gtk_tree_store_remove(GTK_TREE_STORE(model), &m_topIter);
    gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeView), CreateTreeModel());
    gtk_tree_view_expand_to_path(GTK_TREE_VIEW(m_treeView), path);
    gtk_tree_path_free (path);
}

void ViewTemplet::TransferClicked(GtkButton *button)
{
    GtkTextIter it1_start, it1_end;
    GtkTextIter it2_start, it2_end;

    gtk_text_buffer_get_start_iter(m_buffer_indication, &it1_start);
    gtk_text_buffer_get_end_iter(m_buffer_indication, &it1_end);
    gtk_text_buffer_get_start_iter(m_buffer_comments, &it2_start);
    gtk_text_buffer_get_end_iter(m_buffer_comments, &it2_end);

    char *text_indication = gtk_text_buffer_get_text(m_buffer_indication, &it1_start, &it1_end, FALSE);
    char *text_comments = gtk_text_buffer_get_text(m_buffer_comments, &it2_start, &it2_end, FALSE);

    ViewReport::GetInstance()->InsertIndication(text_indication);
    ViewReport::GetInstance()->InsertComments(text_comments);

    // gtk_text_iter_free(&it1_start);
    // gtk_text_iter_free(&it1_end);
    // gtk_text_iter_free(&it2_start);
    // gtk_text_iter_free(&it2_end);
    DestroyWindow();
}

void ViewTemplet::ExitClicked(GtkButton *button)
{
    DestroyWindow();
}

void ViewTemplet::EditClicked(GtkButton *button)
{
    gtk_widget_show(m_button_insert);
    gtk_widget_show(m_button_delete);
    gtk_widget_show(m_button_copy);
    gtk_widget_show(m_button_paste);
    gtk_widget_show(m_button_save);
    gtk_widget_show(m_button_default);
    gtk_widget_show(m_button_return);
    gtk_widget_hide(m_button_exit);
    gtk_widget_hide(m_button_transfer);
    gtk_widget_hide(m_button_edit);

    g_object_set(m_renderer, "editable", TRUE, NULL);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(m_textview_indication), TRUE);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(m_textview_comments), TRUE);
    g_signal_connect(m_renderer, "edited", G_CALLBACK(HandleCellRendererEdited), this);
}

void ViewTemplet::SaveClicked(GtkButton *button)
{
    GtkTreeSelection *selected_node = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeView));
    GtkTreeModel *model;
    GtkTreeIter iter;
    if (gtk_tree_selection_get_selected(selected_node, &model, &iter) != TRUE) {
    	ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, 
    					  _("Please select one node to be save!"), NULL);//请先选择待保存的结点!
    	return;
    }
    GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
    gint tree_depth = gtk_tree_path_get_depth(path);
    gtk_tree_path_free (path);

    if (gtk_tree_model_iter_has_child(model, &iter)) {
    	ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, 
    					  _("Save failure! Please select leaf node!"), NULL);//保存失败!请选择末结点!
    	return;
    }

    GtkTextIter it1_start, it1_end;
    GtkTextIter it2_start, it2_end;
    gtk_text_buffer_get_start_iter(m_buffer_indication, &it1_start);
    gtk_text_buffer_get_end_iter(m_buffer_indication, &it1_end);
    gtk_text_buffer_get_start_iter(m_buffer_comments, &it2_start);
    gtk_text_buffer_get_end_iter(m_buffer_comments, &it2_end);

    char *text_indication = gtk_text_buffer_get_text(m_buffer_indication, &it1_start, &it1_end, FALSE);
    char *text_comments = gtk_text_buffer_get_text(m_buffer_comments, &it2_start, &it2_end, FALSE);

    if (strlen(text_indication) > maxTextLen)	{
    	ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, 
    					  _("The indication is too long!"), NULL);//超声所见超出字数范围限制
    	return;
    }

    if (strlen(text_comments) > maxTextLen) {
    	ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, 
    					  _("The comments are too long!"), NULL);//诊断意见超出字数范围限制
    	return;
    }

    ostringstream stream;
    if (tree_depth == 2) {
	char *str_f1;
	gtk_tree_model_get(model, &iter, 0, &str_f1, -1);
	stream << "UPDATE templet SET indication = '" << text_indication << "', comments = '" << text_comments
	       << "' WHERE f1 = '" << str_f1 << "'";
    }
    else if (tree_depth == 3) {
	char *str_f1, *str_f2;
	GtkTreeIter parent;
    	gtk_tree_model_get(model, &iter, 0, &str_f2, -1);
    	gtk_tree_model_iter_parent(model, &parent, &iter);
    	gtk_tree_model_get(model, &parent, 0, &str_f1, -1);
	stream << "UPDATE templet SET indication = '" << text_indication << "', comments = '" << text_comments
	       << "' WHERE f1 = '" << str_f1 << "' AND f2 = '" << str_f2 << "'";
    }
    else if (tree_depth == 4) {
	char *str_f1, *str_f2, *str_f3;
	GtkTreeIter parent1, parent2;
	gtk_tree_model_get(model, &iter, 0, &str_f3, -1);
	gtk_tree_model_iter_parent(model, &parent1, &iter);
	gtk_tree_model_get(model, &parent1, 0, &str_f2, -1);
	gtk_tree_model_iter_parent(model, &parent2, &parent1);
	gtk_tree_model_get(model, &parent2, 0, &str_f1, -1);
	stream << "UPDATE templet SET indication = '" << text_indication << "', comments = '" << text_comments
	       << "' WHERE f1 = '" << str_f1 << "' AND f2 = '" << str_f2 << "' AND f3 = '" << str_f3 << "'";
    	}
    else if (tree_depth == 5) {
	char *str_f1, *str_f2, *str_f3, *str_f4;
	GtkTreeIter parent1, parent2, parent3;

    	gtk_tree_model_get(model, &iter, 0, &str_f4, -1);
    	gtk_tree_model_iter_parent(model, &parent1, &iter);
    	gtk_tree_model_get(model, &parent1, 0, &str_f3, -1);
    	gtk_tree_model_iter_parent(model, &parent2, &parent1);
    	gtk_tree_model_get(model, &parent2, 0, &str_f2, -1);
    	gtk_tree_model_iter_parent(model, &parent3, &parent2);
    	gtk_tree_model_get(model, &parent3, 0, &str_f1, -1);
	stream << "UPDATE templet SET indication = '" << text_indication << "', comments = '" << text_comments
	       << "' WHERE f1 = '" << str_f1 << "' AND f2 = '" << str_f2 
	       << "' AND f3 = '" << str_f3 << "' AND f4 = '" << str_f4 << "'";
    }

    string sql = stream.str();
    char *errmsg = NULL;
    OpenDB();
    if (sqlite3_exec(m_db_templet, sql.c_str(), 0, 0, &errmsg) != SQLITE_OK) {
    	ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, _("Database error!"), NULL); //数据库操作失败!
    	PRINTF("save error:%s!\n", errmsg);
    }
    CloseDB();
}

static int DefaultTemplet(gpointer data)
{
    ViewTemplet::GetInstance()->DefaultFactory();
    return 0;
}

void ViewTemplet::DefaultClicked(GtkButton *button)
{
    ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), 
                                      ViewDialog::QUESTION, 
                                      _("Default Factory will remove all the users modified data!"), // 恢复出厂设置将删除所有用户修改的数据
                                      DefaultTemplet); 
}

void ViewTemplet::ReturnClicked(GtkButton *button)
{
    gtk_widget_hide(m_button_insert);
    gtk_widget_hide(m_button_delete);
    gtk_widget_hide(m_button_copy);
    gtk_widget_hide(m_button_paste);
    gtk_widget_hide(m_button_save);
    gtk_widget_hide(m_button_default);
    gtk_widget_hide(m_button_return);
    gtk_widget_show(m_button_exit);
    gtk_widget_show(m_button_transfer);
    gtk_widget_show(m_button_edit);

    g_object_set(m_renderer, "editable", FALSE, NULL);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(m_textview_indication), FALSE);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(m_textview_comments), FALSE);
}

void ViewTemplet::CellRendererEdited(GtkCellRendererText *renderer, gchar *path_str, gchar *new_text)
{
    char *old_text;
    GtkTreeIter iter;

    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeView));
    GtkTreePath *path = gtk_tree_path_new_from_string(path_str);
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_model_get(model, &iter, 0, &old_text, -1);

    gint tree_depth = gtk_tree_path_get_depth(path);
    gtk_tree_path_free(path);

    //如果重命名前后renderertext中的文本没有改变,则返回
    if (strcmp(new_text, old_text) == 0)
    	return;

    if (strcmp(new_text, "") == 0) {
    	ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, _("Node name can not be null!"), NULL); //结点名称不能为空
    	return;
    }

    string str1 = "%"; // "%" is sqlite3 wildcard
    string str2 = "%";
    string str3 = "%";
    string str4 = "%";
    ostringstream stream;

    if (tree_depth == 1) {
    	ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, _("Can not rename the root!"), NULL); //不能重命名根结点
    	return;
    }
    else if (tree_depth == 2) {
	stream << "UPDATE templet SET f1 = '" << new_text << "' WHERE f1 = '" << old_text << "'";
	str1 = new_text;
    }
    else if (tree_depth == 3) {
	GtkTreeIter iter_parent;
	char *str_tmp;
    	gtk_tree_model_iter_parent(model, &iter_parent, &iter);
    	gtk_tree_model_get(model, &iter_parent, 0, &str_tmp, -1);
	str1 = str_tmp;
	stream << "UPDATE templet SET f2 = '" << new_text << "' WHERE f1 = '" << str1 << "' AND f2 = '" << old_text << "'";
    	str2 = new_text;
    }
    else if (tree_depth == 4) {
	GtkTreeIter iter_parent0;
	GtkTreeIter iter_parent1;
	char *str_tmp;
    	gtk_tree_model_iter_parent(model, &iter_parent0, &iter);
    	gtk_tree_model_get(model, &iter_parent0, 0, &str_tmp, -1);
	str2 = str_tmp;
    	gtk_tree_model_iter_parent(model, &iter_parent1, &iter_parent0);
    	gtk_tree_model_get(model, &iter_parent1, 0, &str_tmp, -1);
	str1 = str_tmp;
	stream << "UPDATE templet SET f3 = '" << new_text 
	       << "' WHERE f1 = '" << str1 
	       << "' AND f2 = '" << str2 
	       << "' AND f3 = '" << old_text << "'";
    	str3 = new_text;
    }
    else if (tree_depth == 5) {
	GtkTreeIter iter_parent0;
	GtkTreeIter iter_parent1;
	GtkTreeIter iter_parent2;
	char *str_tmp;
    	gtk_tree_model_iter_parent(model, &iter_parent0, &iter);
    	gtk_tree_model_get(model, &iter_parent0, 0, &str_tmp, -1);
	str3 = str_tmp;
    	gtk_tree_model_iter_parent(model, &iter_parent1, &iter_parent0);
    	gtk_tree_model_get(model, &iter_parent1, 0, &str_tmp, -1);
	str2 = str_tmp;
    	gtk_tree_model_iter_parent(model, &iter_parent2, &iter_parent1);
    	gtk_tree_model_get(model, &iter_parent2, 0, &str_tmp, -1);
	str1 = str_tmp;
	stream << "UPDATE templet SET f4 = '" << new_text 
	       << "' WHERE f1 = '" << str1 
	       << "' AND f2 = '" << str2 
	       << "' AND f3 = '" << str3
	       << "' AND f4 = '" << old_text << "'";
    	str4 = new_text;
    }

    if (!UniqueItem(str1.c_str(), str2.c_str(), str3.c_str(), str4.c_str()) != 0) {
    	ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, _("Rename failed! The node has been exist!"), NULL); //重命名失败!该结点已存在
    	return;
    }

    char *errmsg = NULL;
    string sql = stream.str();
    OpenDB();
    if (sqlite3_exec(m_db_templet, sql.c_str(), 0, 0, &errmsg) != SQLITE_OK) {
    	ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, _("Database error!"), NULL); //数据库操作失败!
    	PRINTF("UPDATE ERROR:%s!\n", errmsg);
    	return;
    }
    CloseDB();

    gtk_tree_store_set(GTK_TREE_STORE(model), &iter, 0, new_text, -1);
}

bool ViewTemplet::UniqueItem(const char *str1, const char *str2, const char *str3, const char *str4)
{
    sqlite3_stmt *stmt = NULL;
    ostringstream stream;
    int result = 0;

    stream << "SELECT COUNT(*) AS num_result FROM templet "
	   << "WHERE f1 LIKE '" << str1 << "' "
	   << "AND f2 LIKE '" << str2 << "' "
	   << "AND f3 LIKE '" << str3 << "' "
	   << "AND f4 LIKE '" << str4 << "'";
    string sql = stream.str();
    OpenDB();
    if (sqlite3_prepare(m_db_templet, sql.c_str(), sql.size(), &stmt, 0) != SQLITE_OK) {
	PRINTF("SELECT ERROR:%s!\n", sqlite3_errmsg(m_db_templet));
	return false;
    }
    while (sqlite3_step(stmt) != SQLITE_DONE) {
	if (strcmp(sqlite3_column_name(stmt, 0), "num_result") == 0)
	    result = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    CloseDB();
    if (result)
	return false;
    else
	return true;
}

//在一个指定结点下查找其儿子结点为str的结点
//主要是避免在插入操作是插入重复的New_Item结点
GtkTreeIter ViewTemplet::InsertUnique(GtkTreeModel *model, GtkTreeIter *iter, const char *str)
{
    GtkTreeIter tmp_iter;
    char *strtmp = NULL;
    gboolean has_node;

    has_node = gtk_tree_model_iter_children(model, &tmp_iter, iter);
    while (has_node == TRUE) {
    	gtk_tree_model_get(model, &tmp_iter, 0, &strtmp, -1);
    	if (strcmp(strtmp, str) == 0)
    	    return tmp_iter;
    	else
    	    has_node = gtk_tree_model_iter_next(model, &tmp_iter);
    }
    return tmp_iter;
}

void ViewTemplet::InitTreeView(void)
{
    gboolean has_node;
    GtkTreeIter tmp_iter;
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeView));

    //折叠起第一级结点
    has_node = gtk_tree_model_iter_children(GTK_TREE_MODEL(model), &tmp_iter, &m_topIter);
    while (has_node == TRUE) {
    	GtkTreePath *path = gtk_tree_model_get_path(GTK_TREE_MODEL(model), &tmp_iter);
    	gtk_tree_view_collapse_row(GTK_TREE_VIEW(m_treeView), path);
    	has_node = gtk_tree_model_iter_next(GTK_TREE_MODEL(model), &tmp_iter);
	gtk_tree_path_free(path);
    }
}

static int RestartTemplet(gpointer data)
{
    ViewTemplet::GetInstance()->DestroyWindow();    
    return 0;
}

bool ViewTemplet::DefaultFactory(void)
{
    SysGeneralSetting sysGS;
    if(sysGS.GetLanguage() == ZH)
    {
        if (remove(TEMPLET_CH_DB)) {
            PRINTF("Templet DefaultFactory rm error: %s\n", strerror(errno));
            ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, _("Recovery failed!"), NULL); 
            return false;
        }
    }
    else
    {
        if (remove(TEMPLET_EN_DB)) {
            PRINTF("Templet DefaultFactory rm error: %s\n", strerror(errno));
            ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, _("Recovery failed!"), NULL); 
            return false;
        }
    }
    int fd;
    if(sysGS.GetLanguage() ==ZH)
    {
        fd = open(kGlossarySql_ch, O_RDONLY);
    }
    else
    {
        fd = open(kGlossarySql_en, O_RDONLY);

    }
    if (fd < 0) {
        PRINTF("Templet DefaultFactory open error: %s\n", strerror(errno));
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, _("Recovery failed!"), NULL); 
        return false;
    }
    char buffer[200*1024];      // 假设文件大小为200k
    memset(buffer, 0, sizeof(buffer));
    if (read(fd, buffer, sizeof(buffer)) < 0) {
        PRINTF("Templet DefaultFactory read error: %s\n", strerror(errno));
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, _("Recovery failed!"), NULL); 
        close(fd);
        return false;
    }

    if(sysGS.GetLanguage() == ZH)
    {

        if (access(TEMPLET_CH_DB, F_OK)) {
            sqlite3 *db = 0;
            if (sqlite3_open(TEMPLET_CH_DB, &db) != SQLITE_OK) {
                PRINTF("Open Templet Database Error!\n");
                ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, _("Recovery failed!"), NULL); 
                close(fd);
                return false;
            }
            char *errmsg = NULL;
            if (sqlite3_exec(db, buffer, 0, 0, &errmsg) != SQLITE_OK) {
                ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, _("Recovery failed!"), NULL); 
                PRINTF("Init Templet Error: %s\n", sqlite3_errmsg(db));
                sqlite3_free(errmsg);
                close(fd);
                return false;
            }
            sqlite3_close(db);
            close(fd);

            ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), 
                    ViewDialog::INFO, 
                    _("Recovery succeed! Restart request!"), // 恢复数据成功，需要重新开启该功能
                    RestartTemplet); 

            return true;
        } 
        else {
            PRINTF("Access Templet File Error!\n");
            ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, _("Recovery failed!"), NULL); 
            close(fd);
            return false;
        }

    }
    else
    {

        if (access(TEMPLET_EN_DB, F_OK)) {
            sqlite3 *db = 0;
            if (sqlite3_open(TEMPLET_EN_DB, &db) != SQLITE_OK) {
                PRINTF("Open Templet Database Error!\n");
                ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, _("Recovery failed!"), NULL); 
                close(fd);
                return false;
            }
            char *errmsg = NULL;
            if (sqlite3_exec(db, buffer, 0, 0, &errmsg) != SQLITE_OK) {
                ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, _("Recovery failed!"), NULL); 
                PRINTF("Init Templet Error: %s\n", sqlite3_errmsg(db));
                sqlite3_free(errmsg);
                close(fd);
                return false;
            }
            sqlite3_close(db);
            close(fd);

            ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), 
                    ViewDialog::INFO, 
                    _("Recovery succeed! Restart request!"), // 恢复数据成功，需要重新开启该功能
                    RestartTemplet); 

            return true;
        } 

        else {
            PRINTF("Access Templet File Error!\n");
            ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, _("Recovery failed!"), NULL); 
            close(fd);
            return false;
        }

    }
}
