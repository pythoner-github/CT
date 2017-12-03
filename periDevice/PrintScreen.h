#ifndef PRINTSCREEN_H
#define PRINTSCREEN_H

#include <gtk/gtk.h>

class PrintScreen
{
	public:
		PrintScreen();
		~PrintScreen();

		gboolean PrintP1();
		gboolean PrintP2();
		gboolean PrintP3();
		gboolean ScreenShotToFile();
	
	private:

};

#endif
