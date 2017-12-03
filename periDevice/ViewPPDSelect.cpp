#include "gui_func.h"
#include "gui_global.h"
#include "KeyValueOpr.h"
#include "KeyDef.h"
#include "FakeXEvent.h"
#include "ViewPPDSelect.h"
#include "../patient/FileMan.h"
#include "ViewPrinterAdd.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

ViewPPDSelect* ViewPPDSelect::m_ptrInstance = NULL;

namespace {
    const string ppd_path = "./res/printer/ppd_specific/";
}
ViewPPDSelect::ViewPPDSelect()
{
}

ViewPPDSelect::~ViewPPDSelect()
{
    if (m_ptrInstance != NULL)
	delete m_ptrInstance;
}

ViewPPDSelect* ViewPPDSelect::GetInstance()
{
    if (m_ptrInstance == NULL)
	m_ptrInstance = new ViewPPDSelect;

    return m_ptrInstance;
}

void ViewPPDSelect::CreateWindow(GtkWindow *parent)
{
    GtkWidget *fixed;
    GtkWidget *scrolledwindow;
    GtkWidget *treeview;
    GtkWidget *button_cancel;
    GtkWidget *button_open;

    m_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (m_window, 600, 400);
    gtk_window_set_title (GTK_WINDOW (m_window), _("PPD File Selection"));
    gtk_window_set_position (GTK_WINDOW (m_window), GTK_WIN_POS_CENTER_ALWAYS);
    gtk_window_set_modal (GTK_WINDOW (m_window), TRUE);
    gtk_window_set_resizable (GTK_WINDOW (m_window), FALSE);
    gtk_window_set_transient_for(GTK_WINDOW(m_window), parent);
    g_signal_connect (G_OBJECT(m_window), "delete-event", G_CALLBACK(on_window_delete_event), this);

    fixed = gtk_fixed_new ();
    gtk_widget_show (fixed);
    gtk_container_add (GTK_CONTAINER (m_window), fixed);

    scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show (scrolledwindow);
    gtk_fixed_put (GTK_FIXED (fixed), scrolledwindow, 20, 10);
    gtk_widget_set_size_request (scrolledwindow, 560, 300);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_IN);

    treeview = create_treeview();
    gtk_widget_show (treeview);
    gtk_container_add (GTK_CONTAINER (scrolledwindow), treeview);
    m_selected_ppd = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
    gtk_tree_selection_set_mode(m_selected_ppd, GTK_SELECTION_SINGLE);

    GtkWidget *image_open = gtk_image_new_from_stock ("gtk-open", GTK_ICON_SIZE_BUTTON);
    GtkWidget *label_open = gtk_label_new_with_mnemonic (_("Open"));
    button_open = create_button_icon(label_open, image_open);
    gtk_fixed_put (GTK_FIXED (fixed), button_open, 320, 330);
    g_signal_connect ((gpointer) button_open, "clicked", G_CALLBACK (on_button_open_clicked), this);

    GtkWidget *image_cancel = gtk_image_new_from_stock ("gtk-cancel", GTK_ICON_SIZE_BUTTON);
    GtkWidget *label_cancel = gtk_label_new_with_mnemonic (_("Cancel"));
    button_cancel = create_button_icon(label_cancel, image_cancel);
    gtk_fixed_put (GTK_FIXED (fixed), button_cancel, 460, 330);
    g_signal_connect ((gpointer) button_cancel, "clicked", G_CALLBACK (on_button_cancel_clicked), this);

    gtk_widget_show_all(m_window);

    g_keyInterface.Push(this);
    SetSystemCursorToCenter();
   
    return ;
}

void ViewPPDSelect::DestroyWindow(void)
{
    if(GTK_IS_WIDGET(m_window)) {
        g_keyInterface.Pop();
        gtk_widget_destroy(m_window);
        m_window = NULL;
    }
}

gboolean ViewPPDSelect::WindowDeleteEvent(GtkWidget *widget, GdkEvent *event)
{
    DestroyWindow();
    return FALSE;
}

void ViewPPDSelect::KeyEvent(unsigned char keyValue)
{
    FakeXEvent::KeyEvent(keyValue);
    
    switch(keyValue) {
    case KEY_ESC:
	BtnCancelClicked(NULL);
	break;
    default:
	break;
    }
}

void ViewPPDSelect::BtnOpenClicked(GtkButton *button)
{
    GtkTreeModel *model;
    GtkTreeIter iter;

    if (gtk_tree_selection_get_selected(m_selected_ppd, &model, &iter) != TRUE) {
        ViewPrinterAdd::GetInstance()->SetPPDName("", "");
        return ;
    }
    char *file_name;
    char *path_name;
    gtk_tree_model_get(model, &iter, NAME, &file_name, -1);
    gtk_tree_model_get(model, &iter, PATH, &path_name, -1);
    ViewPrinterAdd::GetInstance()->SetPPDName(file_name, path_name);
    delete file_name;
    delete path_name;
    DestroyWindow();
}

void ViewPPDSelect::BtnCancelClicked(GtkButton *button)
{
    DestroyWindow();
    ViewPrinterAdd::GetInstance()->SetPPDName("", "");
}

GtkWidget* ViewPPDSelect::create_treeview(void)
{
    GtkWidget *treeview;
    GtkTreeModel *model;

    GtkCellRenderer *render;
    GtkTreeViewColumn *col;
    
    treeview = gtk_tree_view_new ();
    gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview), FALSE);

    render = gtk_cell_renderer_text_new();
    col = gtk_tree_view_column_new_with_attributes(_("File Name"), render, "text", NAME, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), col);

    render = gtk_cell_renderer_text_new();
    col = gtk_tree_view_column_new_with_attributes(_("Size"), render, "text", SIZE, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), col);

    render = gtk_cell_renderer_text_new();
    col = gtk_tree_view_column_new_with_attributes(_("Modified Time"), render, "text", MODIFIED, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), col);

    model = create_tree_model();
    if (model != NULL)
        gtk_tree_view_set_model (GTK_TREE_VIEW(treeview), model);
    g_object_unref (model); /* destroy model automatically with view */

    return treeview;
}

namespace {
	struct ppd_info{
	string name;
	string path;
	string size;
	string time;
};

bool SortModel(const ppd_info &p1, const ppd_info &p2)
{
	return p1.name < p2.name;
}
}

GtkTreeModel* ViewPPDSelect::create_tree_model(void)
{
    GtkListStore  *store;
    GtkTreeIter    iter;

    store = gtk_list_store_new(NUM_COLS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

    GError *err = NULL;
    GDir * ppd_dir = g_dir_open(ppd_path.c_str(), 0, &err);
    if (ppd_dir == NULL) {
        printf("%s: g_dir_open with error: %s\n", __FUNCTION__, err->message);
        g_error_free(err);
        g_dir_close(ppd_dir);
        return NULL;
    }

	vector<ppd_info> vec;

    const char *file_name = g_dir_read_name (ppd_dir);
    FileMan fm;
    char str_size[64];
    char str_time[64];
    struct stat file_stat;
    while (file_name) {
		if (fm.CompareSuffix(file_name, "ppd") == 0) {
			std::string path = ppd_path + file_name;
			if (stat(path.c_str(), &file_stat) == -1) {
				g_dir_close(ppd_dir);
				perror("stat");
				return NULL;
			}
			sprintf(str_size, "%lld bytes", (long long)file_stat.st_size);
			sprintf(str_time, "%.24s", ctime(&file_stat.st_mtime));

			struct ppd_info ppd;
			ppd.name = file_name;
			ppd.path = path;
			ppd.size = str_size;
			ppd.time = str_time;
			vec.push_back(ppd);
		}
		file_name = g_dir_read_name (ppd_dir);
    }

	sort(vec.begin(), vec.end(), SortModel);

	for(vector<ppd_info>::iterator it=vec.begin(); it!=vec.end(); ++it)
	{
		gtk_list_store_append (store, &iter);
		gtk_list_store_set(store, &iter,
				NAME, it->name.c_str(),
				PATH, it->path.c_str(),
				SIZE, it->size.c_str(),
				MODIFIED, it->time.c_str(),
				-1);
	}

    g_dir_close(ppd_dir);
    return GTK_TREE_MODEL (store);
}

void ViewPPDSelect::update_tree_model(void)
{
    // GtkTreeModel* model = create_tree_model();
    // if (model != NULL)
    //     gtk_tree_view_set_model (GTK_TREE_VIEW(m_treeview), model);
    // g_object_unref (model); /* destroy model automatically with view */
}
