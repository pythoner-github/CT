#ifndef __SCREENSAVER_h__
#define __SCREENSAVER_h__

#include <gtk/gtk.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

class ScreenSaver
{
	public:
		~ScreenSaver();
		static ScreenSaver* GetInstance();

		void Init();
		void Reset();
		gboolean Timer();
		void SetPeriod(guint time); 
		guint GetTimerCount() { return m_count; }
		void BlackScreen();
		void EnterScreenSaver2();
		void EnterScreenSaver3();
        
	private:
		ScreenSaver();
        
		static ScreenSaver* m_ptrInstance;
		static const guint m_interval = 5; //second

		void EnterScreenSaver();
		void ExitScreenSaver();
		void DrawHintMessage();
		void BlackScreen2();
		void BlackScreen3();
		gboolean m_state;
		guint m_timer;	
		guint m_count;
		guint m_period;	

		Display *m_dpy;
		Window m_win;
};

#endif
