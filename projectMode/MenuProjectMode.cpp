
#include "gui_global.h"
#include "gui_func.h"
#include "MenuArea.h"
#include "TopArea.h"
#include "ViewMain.h"
#include "MenuProjectMode.h"
#include "MultiFuncFactory.h"
#include "KeyDef.h"
#include "ViewDialog.h"
#include "ProjectCalc2D.h"
#include "ViewSuperuser.h"
#ifdef EMP_PROJECT
using std::vector;
using std::string;

MenuProjectMode g_menuProjectMode;

MenuProjectMode::MenuProjectMode(void)
{
    m_show = true;
    m_fixed = NULL;
    m_messageTextView = NULL;

    m_vectorMode.clear();
    m_curentPage = 0;
    m_maxPage = 0;

    m_windowDelete = NULL;
    m_selectAll = false;
	m_messageString = "";

}

GtkWidget* MenuProjectMode::Create(void)
{
    m_fixed = gtk_fixed_new ();
    gtk_widget_set_size_request (m_fixed, MENU_AREA_W, 730);

	GtkWidget *tableMode = gtk_table_new (MODECOUNT, 1, TRUE);
    gtk_widget_set_size_request (tableMode, MENU_AREA_W, 200);
    gtk_fixed_put (GTK_FIXED (m_fixed), tableMode, 0, 0);

	for(int i = 0; i < MODECOUNT; ++i)
	{
        m_button[i] = gtk_button_new ();
		gtk_widget_modify_bg(m_button[i], GTK_STATE_NORMAL, g_deep);
        gtk_widget_modify_bg(m_button[i], GTK_STATE_INSENSITIVE, g_deepGray);
        gtk_button_set_focus_on_click(GTK_BUTTON(m_button[i]), FALSE);
    	g_object_set_data(G_OBJECT(m_button[i]), "id", GINT_TO_POINTER(i));
		gtk_table_attach_defaults(GTK_TABLE(tableMode), m_button[i], 0, 1, i, i+1);
        g_signal_connect(m_button[i], "clicked", G_CALLBACK(HandleModeClicked), this);
	}

	GtkWidget *tableButton = gtk_table_new (2, 2, TRUE);
    gtk_widget_set_size_request (tableButton, MENU_AREA_W, 70);
    gtk_fixed_put (GTK_FIXED (m_fixed), tableButton, 0, 200+10);

    m_buttonPageUp = gtk_button_new_with_mnemonic (_("Page Up"));
    gtk_widget_set_size_request (m_buttonPageUp, MENU_AREA_W/2-5 , 30);
	gtk_table_attach_defaults(GTK_TABLE(tableButton), m_buttonPageUp, 0, 1, 0, 1);
    g_signal_connect ((gpointer) m_buttonPageUp, "clicked", G_CALLBACK (HandlePageUpClicked), this);

    m_buttonPageDown = gtk_button_new_with_mnemonic (_("Page Down"));
    gtk_widget_set_size_request (m_buttonPageDown, MENU_AREA_W/2-5 , 30);
	gtk_table_attach_defaults(GTK_TABLE(tableButton), m_buttonPageDown, 1, 2, 0, 1);
    g_signal_connect ((gpointer) m_buttonPageDown, "clicked", G_CALLBACK (HandlePageDownClicked), this);

    GtkWidget *deleteB = gtk_button_new_with_mnemonic (_("Delete"));
    gtk_widget_set_size_request (deleteB, MENU_AREA_W/2-5 , 30);
	gtk_table_attach_defaults(GTK_TABLE(tableButton), deleteB, 0, 1, 1, 2);
    g_signal_connect ((gpointer) deleteB, "clicked", G_CALLBACK (HandleDeleteClicked), this);

    GtkWidget *defaultB = gtk_button_new_with_mnemonic (_("Save"));
    gtk_widget_set_size_request (defaultB, MENU_AREA_W/2-5 , 30);
	gtk_table_attach_defaults(GTK_TABLE(tableButton), defaultB, 1, 2, 1, 2);
    g_signal_connect ((gpointer) defaultB, "clicked", G_CALLBACK (HandleDefaultClicked), this);

    GtkWidget *emplabel = create_label(_("Hint Message"), MENU_AREA_W/2, 30, g_lightGray, NULL);
    gtk_fixed_put (GTK_FIXED (m_fixed), emplabel, 0, 200+10+70+10);

    GtkWidget *scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
    gtk_fixed_put (GTK_FIXED (m_fixed), scrolledwindow, 0, 200+10+70+40);
    gtk_widget_set_size_request (scrolledwindow, MENU_AREA_W-2 , 100+150+30);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_OUT);


    m_messageTextView = gtk_text_view_new ();
    gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (m_messageTextView), GTK_WRAP_WORD_CHAR);
    gtk_container_add (GTK_CONTAINER (scrolledwindow), m_messageTextView);
    gtk_text_view_set_editable (GTK_TEXT_VIEW (m_messageTextView), FALSE);
    gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (m_messageTextView), GTK_WRAP_WORD);
    gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (m_messageTextView), FALSE);
    gtk_text_buffer_set_text (gtk_text_view_get_buffer(GTK_TEXT_VIEW(m_messageTextView)), "", -1);
    gtk_widget_hide_all(m_fixed);

    UpdateLabel();
    return m_fixed;
}

void MenuProjectMode::AddMessage(const char *message)
{
	string buf = message;
	buf += "\n";
	buf += m_messageString;
	m_messageString = buf;
    gtk_text_buffer_set_text (gtk_text_view_get_buffer(GTK_TEXT_VIEW(m_messageTextView)), m_messageString.c_str(), -1);
}

void MenuProjectMode::SetMessage(const char *message)
{
	m_messageString = message;
    gtk_text_buffer_set_text (gtk_text_view_get_buffer(GTK_TEXT_VIEW(m_messageTextView)), m_messageString.c_str(), -1);
}

void MenuProjectMode::ModeClicked(GtkButton *button)
{
	int id = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "id"));
    m_curentMode = m_vectorMode[id+m_curentPage*MODECOUNT];
    UpdateLabel();

	//printf("ModeClicked---%d, %d, %s\n", id, id+m_curentPage*MODECOUNT, m_curentMode.c_str());
    ProjectCalc2D::GetInstance()->ReadCalcPara2D(m_curentMode.c_str());
}

void MenuProjectMode::PageUpClicked(GtkButton *button)
{
    //printf("ButtonModeClicked---%s\n", gtk_button_get_label(button));
    if(m_curentPage > 0)--m_curentPage;
    UpdateLabel();
}

void MenuProjectMode::PageDownClicked(GtkButton *button)
{
    //printf("ButtonModeClicked---%s\n", gtk_button_get_label(button));
    if(m_curentPage < m_maxPage-1)++m_curentPage;
    UpdateLabel();
}

//void MenuProjectMode::DeleteClicked(GtkButton *button)
//{
//    //printf("ButtonModeClicked---%s\n", gtk_button_get_label(button));
//    UpdateLabel();
//}

void MenuProjectMode::DefaultClicked(GtkButton *button)
{
    if(ViewSuperuser::GetInstance()->GetProjectModeStatus())
    {
        ViewProjectMode::GetInstance()->CreateWindow();
    }

    //  printf("ButtonModeClicked---%s\n", gtk_button_get_label(button));
}

void MenuProjectMode::Hide(void)
{
    m_show = false;
    if(NULL != m_fixed)
    {
        gtk_widget_hide_all(m_fixed);
    }
}

void MenuProjectMode::Show(void)
{
    m_show = true;
    if(NULL != m_fixed)
    {
        gtk_widget_show_all(m_fixed);
    }
    UpdateLabel();
}

void MenuProjectMode::UpdateLabel(const char *mode)
{
    m_vectorMode.clear();
    if(NULL != mode)
    {
        m_curentMode = mode;
    }

    //vector <string> m_vectorMode;
    //init m_vectorMode
	char path[256];
    sprintf(path, "%s%s", CFG_RES_PATH, PROJECT_DEBUG_FILE);
    FILE *fd;
    if((fd = fopen(path, "r")) == NULL)
    {
        perror("file is not exist!\n");
    }
    else
    {
        IniFile ini(path);
		IniFile *ptrIni= &ini;
		m_vectorMode = ptrIni->GetGroupName();
	}

    int maxMode = m_vectorMode.size();
    if(maxMode%MODECOUNT)
    {
        m_maxPage =  maxMode/MODECOUNT + 1;
    }
    else
    {
        m_maxPage =  maxMode/MODECOUNT;
    }
    if(m_curentPage > m_maxPage)
    {
        m_curentPage = m_maxPage;
    }


	for(int i = 0; i < MODECOUNT; ++i)
	{
	    int j = i + m_curentPage*MODECOUNT;
        if(j < maxMode)
        {
            gtk_widget_show(m_button[i]);
            if(0 == strcmp(m_curentMode.c_str(), m_vectorMode[j].c_str()))
            {
                char buf[128];
                sprintf(buf, "%s    >>", m_vectorMode[j].c_str());
                gtk_button_set_label(GTK_BUTTON(m_button[i]), buf);
            }
            else
            {
                gtk_button_set_label(GTK_BUTTON(m_button[i]), m_vectorMode[j].c_str());
            }
        }
        else
        {
            gtk_widget_hide(m_button[i]);
        }
    }

    gtk_widget_set_sensitive (m_buttonPageUp, TRUE);
    gtk_widget_set_sensitive (m_buttonPageDown, TRUE);

    if(0 == m_curentPage)
        gtk_widget_set_sensitive (m_buttonPageUp, FALSE);
    if(m_maxPage == m_curentPage-1)
        gtk_widget_set_sensitive (m_buttonPageDown, FALSE);

}


void MenuProjectMode::CreateDeleteWindow(void)
{
    MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::NONE);

	m_windowDelete = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request (m_windowDelete, 550, 500);
	gtk_window_set_position (GTK_WINDOW (m_windowDelete), GTK_WIN_POS_CENTER);
	gtk_window_set_modal (GTK_WINDOW (m_windowDelete), TRUE);
	gtk_window_set_resizable (GTK_WINDOW (m_windowDelete), FALSE);
	gtk_window_set_title (GTK_WINDOW (m_windowDelete), _("Delete Project Mode"));
	gtk_window_set_transient_for(GTK_WINDOW(m_windowDelete), GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()));
	g_signal_connect (G_OBJECT(m_windowDelete), "delete-event", G_CALLBACK(HandleWindowDeleteEvent), this);

	GtkWidget *fixed = gtk_fixed_new ();
	gtk_container_add (GTK_CONTAINER (m_windowDelete), fixed);


    GtkWidget *btnSelect = gtk_button_new_with_mnemonic (_("Select All"));
    gtk_fixed_put (GTK_FIXED (fixed), btnSelect, 50, 420);
    gtk_widget_set_size_request (btnSelect, 100, 40);
    g_signal_connect ( G_OBJECT(btnSelect), "clicked", G_CALLBACK (HandleBtnSelectClicked), this);

    GtkWidget *btnOK = gtk_button_new_with_mnemonic (_("OK"));
    gtk_fixed_put (GTK_FIXED (fixed), btnOK, 200, 420);
    gtk_widget_set_size_request (btnOK, 100, 40);
    g_signal_connect ( G_OBJECT(btnOK), "clicked", G_CALLBACK (HandleBtnOKClicked), this);

    GtkWidget *btnCancel = gtk_button_new_with_mnemonic (_("Cancel"));
    gtk_fixed_put (GTK_FIXED (fixed), btnCancel, 350, 420);
    gtk_widget_set_size_request (btnCancel, 100, 40);
    g_signal_connect ( G_OBJECT(btnCancel), "clicked", G_CALLBACK (HandleBtnCancelClicked), this);

    GtkWidget *frame = gtk_frame_new (NULL);
    gtk_widget_set_size_request (frame, 535, 390);
    gtk_fixed_put (GTK_FIXED (fixed), frame, 5, 10);

	GtkWidget *fixedFrame = gtk_fixed_new();
	gtk_container_add (GTK_CONTAINER (frame), fixedFrame);

    GtkWidget *scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_set_size_request (scrolledwindow, 530, 380);
    gtk_fixed_put (GTK_FIXED (fixedFrame), scrolledwindow, 5, 5);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_IN);

    GtkWidget *viewport = gtk_viewport_new (NULL, NULL);
    gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);

    GtkWidget *fixed2 = gtk_fixed_new ();
    m_vectorChectButton.clear();

	int maxMode = m_vectorMode.size();
    for(int t = 0; t < maxMode; t++)
    {
        //char buf[128];
        //sprintf(buf, "testsatasas-%d", t);
        GtkWidget* checkbutton = gtk_check_button_new_with_mnemonic (m_vectorMode[t].c_str());
        gtk_widget_set_size_request (checkbutton, 350, 25);
        gtk_fixed_put (GTK_FIXED (fixed2), checkbutton, 30, 10+t*30);
        m_vectorChectButton.push_back(checkbutton);
    }
    gtk_container_add (GTK_CONTAINER (viewport), fixed2);

    gtk_widget_show_all(m_windowDelete);
    g_keyInterface.Pop();
    g_keyInterface.Push(this);
    SetSystemCursorToCenter();
}

void MenuProjectMode::SelectProjectMode(GtkButton *button)
{
    for(unsigned int i=0; i< m_vectorChectButton.size(); ++i)
    {
        if(m_selectAll)
        {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_vectorChectButton[i]), false);
            gtk_button_set_label(GTK_BUTTON(button), "Select All");
        }
        else
        {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_vectorChectButton[i]), true);
            gtk_button_set_label(GTK_BUTTON(button), "Unselect All");
        }
    }
    m_selectAll = !m_selectAll;
}

void MenuProjectMode::DestroyDeleteWindow(void)
{
    m_selectAll = false;
    if(GTK_IS_WIDGET(m_windowDelete))
    {
        g_keyInterface.Pop();
        gtk_widget_destroy(m_windowDelete);
        m_windowDelete = NULL;
        if (g_keyInterface.Size() == 1)
        {
            SetSystemCursor(SYSCURSOR_X, SYSCUROSR_Y);
        }
    }
}

int DeleteCallBack(gpointer data)
{
    g_menuProjectMode.DeleteProjectModeFun();
    return 0;
}

void MenuProjectMode::DeleteProjectModeFun(void)
{
    for(unsigned int i=0; i< m_vectorModeDelete.size(); ++i)
    {
        //remove ProjectMode
        printf("remove: %s\n", m_vectorModeDelete[i].c_str());
		char path[256];
		sprintf(path, "%s%s", CFG_RES_PATH, PROJECT_DEBUG_FILE);
		FILE *fd;
		if((fd = fopen(path, "r")) == NULL)
		{
			perror("file is not exist!\n");
		}
		else
		{
			IniFile ini(path);
			IniFile *ptrIni= &ini;
			ptrIni->RemoveGroup(m_vectorModeDelete[i].c_str());
			ptrIni->SyncConfigFile();
		}
	}
	UpdateLabel();
}

void MenuProjectMode::DeleteProjectMode(void)
{
    m_vectorModeDelete.clear();
    for(unsigned int i=0; i< m_vectorChectButton.size(); ++i)
    {
        if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_vectorChectButton[i])))
        {
            //remove ProjectMode
            m_vectorModeDelete.push_back(gtk_button_get_label(GTK_BUTTON(m_vectorChectButton[i])));
        }
    }
	const char* info = N_("Sure Delete Project Mode?");
	ViewDialog::GetInstance()->Create(
        GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()),
        ViewDialog::QUESTION,
        _(info),
        DeleteCallBack);
}

/////////////////////////////////////////////////////////////////////////////////////////////

ViewProjectMode* ViewProjectMode::m_ptrInstance = NULL;
ViewProjectMode* ViewProjectMode::GetInstance(void)
{
    if (m_ptrInstance == NULL)
	m_ptrInstance  = new ViewProjectMode;
    return m_ptrInstance;
}

void ViewProjectMode::CreateWindow(void)
{
    MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::NONE);

	m_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request (m_window, 550, 400);
	gtk_window_set_position (GTK_WINDOW (m_window), GTK_WIN_POS_CENTER);
	gtk_window_set_modal (GTK_WINDOW (m_window), TRUE);
	gtk_window_set_resizable (GTK_WINDOW (m_window), FALSE);
	gtk_window_set_title (GTK_WINDOW (m_window), _("Save Project Mode Para"));
	gtk_window_set_transient_for(GTK_WINDOW(m_window), GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()));
	g_signal_connect (G_OBJECT(m_window), "delete-event", G_CALLBACK(HandleWindowDeleteEvent), this);

	GtkWidget *fixed = gtk_fixed_new ();
	gtk_container_add (GTK_CONTAINER (m_window), fixed);

	GSList *radioButtonParaModeGroup = NULL;
    m_radioButtonNew = gtk_radio_button_new_with_mnemonic (NULL, _("New Para Mode"));
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radioButtonNew), radioButtonParaModeGroup);
    radioButtonParaModeGroup = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radioButtonNew));
    gtk_fixed_put (GTK_FIXED (fixed), m_radioButtonNew, 20, 80);

    GtkWidget *labelNew = gtk_label_new_with_mnemonic ("ProbeType - Frequency -");
    gtk_widget_set_size_request (labelNew, 160, 30);
    gtk_fixed_put (GTK_FIXED (fixed), labelNew, 30, 120);

    m_entryNew = gtk_entry_new ();
    gtk_fixed_put (GTK_FIXED (fixed), m_entryNew, 200, 120);
    gtk_widget_set_size_request (m_entryNew, 220, 30);
    gtk_entry_set_max_length(GTK_ENTRY(m_entryNew), 32);
    g_signal_connect(G_OBJECT(m_entryNew), "insert_text", G_CALLBACK(OnEntryNewinsert), this);

    GtkWidget *radioButtonOverload = gtk_radio_button_new_with_mnemonic (NULL, _("Overload Para Mode"));
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (radioButtonOverload), radioButtonParaModeGroup);
    radioButtonParaModeGroup = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radioButtonOverload));
    gtk_fixed_put (GTK_FIXED (fixed), radioButtonOverload, 20, 180);

	// read ini file
	char path[256];
    sprintf(path, "%s%s", CFG_RES_PATH, PROJECT_DEBUG_FILE);
    FILE *fd;
    if((fd = fopen(path, "r")) == NULL)
    {
        perror("file is not exist!\n");
    }
    else
    {
        IniFile ini(path);
		IniFile *ptrIni = &ini;
		m_vectorModeName = ptrIni->GetGroupName();
	}

    m_comboboxOverload = gtk_combo_box_new_text ();
    gtk_fixed_put (GTK_FIXED (fixed), m_comboboxOverload, 40, 220);
    gtk_widget_set_size_request (m_comboboxOverload, 380, 30);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboboxOverload), -1);

    for(unsigned int i=0; i<m_vectorModeName.size(); ++i)
    {
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxOverload), m_vectorModeName[i].c_str());
    }
    if(m_vectorModeName.size() > 0)
    {
        gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboboxOverload), 0);
    }

    GtkWidget *btnSave = gtk_button_new_with_mnemonic (_("Save"));
    gtk_fixed_put (GTK_FIXED (fixed), btnSave, 150, 300);
    gtk_widget_set_size_request (btnSave, 100, 40);
    g_signal_connect ( G_OBJECT(btnSave), "clicked", G_CALLBACK (HandleBtnSaveClicked), this);

    GtkWidget *btnCancel = gtk_button_new_with_mnemonic (_("Cancel"));
    gtk_fixed_put (GTK_FIXED (fixed), btnCancel, 300, 300);
    gtk_widget_set_size_request (btnCancel, 100, 40);
    g_signal_connect ( G_OBJECT(btnCancel), "clicked", G_CALLBACK (HandleBtnCancelClicked), this);

    gtk_widget_show_all(m_window);
    g_keyInterface.Pop();
    g_keyInterface.Push(this);
    SetSystemCursorToCenter();
}

static gboolean ExitWindow(gpointer data)
{
    ((ViewProjectMode *)data)->DestroyWindow();
    return FALSE;
}

void ViewProjectMode::KeyEvent(unsigned char keyValue)
{
	FakeXEvent::KeyEvent(keyValue);

    switch(keyValue)
    {
        case KEY_ESC:
            g_timeout_add(100, ExitWindow, this);
            FakeEscKey();
            break;
        default: break;
    }
}

void ViewProjectMode::DestroyWindow(void)
{
    if(GTK_IS_WIDGET(m_window))
    {
        g_keyInterface.Pop();
        gtk_widget_destroy(m_window);
        if (g_keyInterface.Size() == 1)
        {
            SetSystemCursor(SYSCURSOR_X, SYSCUROSR_Y);
        }
    }
}

void ViewProjectMode::SaveProjectModePara(void)
{
    char saveName[128];
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radioButtonNew)))
    {
        string freq;
        TopArea *ptrTopArea = TopArea::GetInstance();
        string probe = ptrTopArea->GetProbeType();
        ptrTopArea->GetFreq(freq);
        char *name = (char*)gtk_entry_get_text(GTK_ENTRY(m_entryNew));
       // sprintf(saveName, "%s-%s-%s", probe.c_str(), freq.c_str(), name);
		sprintf(saveName, "%s-%s", probe.c_str(), name);

        for(int i=0; i<m_vectorModeName.size(); ++i)
        {
            if(0 == strcmp(m_vectorModeName[i].c_str(), saveName))
            {
            	const char* info = N_("The Name Existed! Please Rename!");
            	ViewDialog::GetInstance()->Create(
                    GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()),
                    ViewDialog::QUESTION,
                    _(info),
                    DeleteCallBack);
                return;
            }
        }
    }
    else
    {
        sprintf(saveName, "%s", (char*)gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_comboboxOverload)));

    }

    printf("ViewProjectMode::SaveProjectModePara(%s)\n", saveName);

    //add some code for save para to ini file
    ProjectCalc2D::GetInstance()->SaveCalcPara2D(saveName);

    DestroyWindow();

    //update MenuProjectMode
    g_menuProjectMode.UpdateLabel(saveName);
}

void ViewProjectMode::EntryNameInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position)
{
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radioButtonNew)))
    {
        gint old_text_length = strlen(gtk_entry_get_text(GTK_ENTRY(editable)));
        if(old_text_length + new_text_length > gtk_entry_get_max_length(GTK_ENTRY(editable)))
        {
            gtk_signal_emit_stop(GTK_OBJECT(editable), g_signal_lookup("insert-text", GTK_TYPE_EDITABLE));
        }
    }
}
#endif