#ifndef INIT_H
#define INIT_H

#include <gtk/gtk.h>
#include "DSCTypes.h"
#include "FpgaCtrl2D.h"
#include "KeyValueOpr.h"

#include "Update2D.h"
#include "UpdatePw.h"
#include "UpdateCfm.h"

#include "CalcPw.h"
#include "CalcCfm.h"

class Init
{
	public:
		Init(); 
		~Init() {}

		void SystemInit(int argc, char *argv[]);
		void ParaInit();
		void ProbeCheck();
		void WriteLogHead();
		void CreatMainWindow(void);
		void ExitSystem();
	private:
		static GtkWidget *main_window;

		///> func
		static gboolean UpdateTopArea(gpointer data);
};
#endif

