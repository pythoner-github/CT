#include <string.h>
#include <stdio.h>
#include <time.h>

#include "KeyValueOpr.h"
#include "ViewCalendar.h"
#include "ViewArchive.h"
#include "gui_global.h"


ViewCalendar* ViewCalendar::m_ptrInstance = NULL;

ViewCalendar::ViewCalendar()
{
    m_flag = START;
}

ViewCalendar::~ViewCalendar()
{
	if(m_ptrInstance != NULL)
		delete m_ptrInstance;
}

ViewCalendar* ViewCalendar::GetInstance()
{
	if(m_ptrInstance == NULL)
		m_ptrInstance = new ViewCalendar;

	return m_ptrInstance;
}

GtkWidget* ViewCalendar::GetWindow(void)
{
    if (GTK_IS_WIDGET(m_window))
        return m_window;
    else 
        return NULL;
}

extern KeyValueOpr keyInterface;
void ViewCalendar::CreateWindow(GtkWidget *parent, CalendarType flag, AbsCalendarOpr* opr)
{
	m_flag = flag;
	m_opr = opr;

	m_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
//	gtk_widget_set_size_request (m_window, 150, 206);
	gtk_window_set_title (GTK_WINDOW (m_window), _("Calendar"));
	gtk_window_set_position (GTK_WINDOW (m_window), GTK_WIN_POS_MOUSE);
	gtk_window_set_modal (GTK_WINDOW (m_window), TRUE);
	gtk_window_set_resizable (GTK_WINDOW (m_window), FALSE);
	gtk_window_set_transient_for(GTK_WINDOW (m_window), GTK_WINDOW(parent)); 
	g_signal_connect (G_OBJECT (m_window), 
			"destroy", 
			G_CALLBACK (HandleDestroyEvent), 
			this);

	GtkWidget *vbox = gtk_vbox_new (FALSE, 2);
	gtk_container_add (GTK_CONTAINER (m_window), vbox);

	m_calendar = gtk_calendar_new();
	gtk_box_pack_start (GTK_BOX (vbox), m_calendar, FALSE, FALSE, 0);
//	gtk_container_add (GTK_CONTAINER (m_window), m_calendar);
	g_signal_connect (G_OBJECT (m_calendar), 
			"day-selected-double-click",
		    G_CALLBACK (HandleCalendarDaySelectedDoubleClick),
		    this);

	GtkWidget *label = gtk_label_new(_("Double-click to confirm"));
	gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);

	g_keyInterface.Push(this);
	gtk_widget_show_all(m_window);
}

void ViewCalendar::DestroyEvent(GtkWidget *widget)
{
	g_keyInterface.Pop();
//	g_keyInterface.Push(ViewArchive::GetInstance());

    guint y, m, d;
  	gtk_calendar_get_date(GTK_CALENDAR(m_calendar), &y, &m, &d);
    if (m_opr != NULL)
    {
        if(m_flag == START)
        {
            m_opr->SetStartDate(y, m+1, d);
        }
        else if(m_flag == END)
        {
            m_opr->SetEndDate(y, m+1, d);
        }
    }
}

void ViewCalendar::CalendarDaySelectedDoubleClick(GtkCalendar *calendar)
{
	gtk_widget_destroy(m_window);
}
