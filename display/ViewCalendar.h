#ifndef VIEWCALENDAR_H
#define VIEWCALENDAR_H

//#include "AbsKeyboardEvent.h"
#include "FakeXEvent.h"
#include "AbsCalendarOpr.h"

//class ViewCalendar:public AbsKeyboardEvent
class ViewCalendar:public FakeXEvent
{
	public:
		~ViewCalendar();
		static ViewCalendar* GetInstance();
        GtkWidget* GetWindow(void);
		enum CalendarType{START, END};

		void CreateWindow(GtkWidget *parent, CalendarType flag, AbsCalendarOpr* opr);
		
	private:
        ViewCalendar();
        static ViewCalendar* m_ptrInstance;
		GtkWidget *m_window;
		GtkWidget *m_calendar;
		CalendarType m_flag;
		AbsCalendarOpr* m_opr;
		
		void DestroyEvent(GtkWidget *widget);
		void CalendarDaySelectedDoubleClick(GtkCalendar *calendar);
		
		// signal connect
		static void HandleDestroyEvent(GtkWidget *widget, ViewCalendar *data) { data->DestroyEvent(widget); }
		static void HandleCalendarDaySelectedDoubleClick(GtkCalendar *calendar, ViewCalendar *data) { data->CalendarDaySelectedDoubleClick(calendar); }
};

#endif
