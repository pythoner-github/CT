#include <gtk/gtk.h>
#include "MenuSystem.h"
#include "gui_global.h"
#include "gui_func.h"
#include "MenuArea.h"
#include "ViewSystem.h"
#include "ModeStatus.h"
#include "FreezeMode.h"
#include "KeyFunc.h"
 
MenuSystem g_menuSystem;

MenuSystem::MenuSystem(void)
{
    m_table = 0;
}

GtkWidget* MenuSystem::Create(void)
{
    m_table = gtk_table_new(16, 1, TRUE);

    m_labelGeneral = create_label("", 0, 0, g_lightGray, NULL);
    GtkWidget *btn_general = gtk_button_new();
    gtk_widget_modify_bg(btn_general, GTK_STATE_NORMAL, g_deep);
    gtk_container_add(GTK_CONTAINER(btn_general), m_labelGeneral);
    gtk_table_attach_defaults(GTK_TABLE(m_table), btn_general, 0, 1, 0, 1);
    g_signal_connect(btn_general, "clicked", G_CALLBACK(HandleBtnGeneral), this);
//    gtk_button_set_focus_on_click(GTK_BUTTON(btn_general), FALSE);

    m_labelBiopsy = create_label("", 0, 0, g_lightGray, NULL);
    GtkWidget *btn_biopsy = gtk_button_new();
    gtk_widget_modify_bg(btn_biopsy, GTK_STATE_NORMAL, g_deep);
    gtk_container_add(GTK_CONTAINER(btn_biopsy), m_labelBiopsy);
    gtk_table_attach_defaults(GTK_TABLE(m_table), btn_biopsy, 0, 1, 1, 2);

    // 穿刺功能暂时屏蔽(2900pluss,配有穿刺线探头）打开功能
    //gtk_widget_modify_bg(btn_biopsy, GTK_STATE_INSENSITIVE, g_deep);
   // gtk_widget_set_sensitive(btn_biopsy, FALSE);

    g_signal_connect(btn_biopsy, "clicked", G_CALLBACK(HandleBtnBiopsy), this);
//    gtk_button_set_focus_on_click(GTK_BUTTON(btn_biopsy), FALSE);
    
    UpdateLabel();
    gtk_widget_set_usize(m_table, WIDTH_SYSTEM_MENU, HEIGHT_SYSTEM_MENU);
    return m_table;
}

void MenuSystem::UpdateLabel(void)
{
    gtk_label_set_text(GTK_LABEL(m_labelGeneral), _("General System Setting"));
    gtk_label_set_text(GTK_LABEL(m_labelBiopsy), _("Biopsy Setting"));
}

void MenuSystem::Show(void)
{
    gtk_widget_show_all(m_table);
}

void MenuSystem::Hide(void)
{
    gtk_widget_hide_all(m_table);
}

void MenuSystem::BtnGeneral(GtkButton *button)
{
    if (ModeStatus::IsAutoReplayMode())
        FreezeMode::GetInstance()->ChangeAutoReplay();
    else if (ModeStatus::IsUnFreezeMode())	
		FreezeMode::GetInstance()->PressFreeze();

    ViewSystem::GetInstance()->CreateWindow();
    ViewSystem::GetInstance()->ChangeNoteBookPage(0);
}

void MenuSystem::BtnBiopsy(GtkButton *buuton)
{
    MenuArea::GetInstance()->ShowBiopsyMenu();
}
