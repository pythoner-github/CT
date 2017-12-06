#ifndef INIT_H
#define INIT_H

#include <gtk/gtk.h>
#include <DSCTypes.h>
#include "imageControl/FpgaCtrl2D.h"
#include "keyboard/KeyValueOpr.h"

#include "imageControl/Update2D.h"
#include "imageControl/UpdatePw.h"
#include "imageControl/UpdateCfm.h"

#include "imageControl/CalcPw.h"
#include "imageControl/CalcCfm.h"

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