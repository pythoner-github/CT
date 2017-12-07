#ifndef PRINTFUNCTIONS_H_
#define PRINTFUNCTIONS_H_

#include <gtk/gtk.h>
#include <cups/cups.h>
#include <cups/ppd.h>
#include <glib/gi18n.h>

#include "periDevice/Printlib.h"
#include "periDevice/Printmain.h"
#include "periDevice/Printgeneral.h"
#include "periDevice/Printfunctions.h"
#include "periDevice/Printtab.h"
#include "periDevice/Printhpgl2.h"
#include "periDevice/Printimage.h"
#include "periDevice/Printoutput.h"
#include "periDevice/Printppd.h"
#include "periDevice/Printtext.h"

class Printfunctions : public Printlib
{
public:
    static Printfunctions *GetInstance();
    Printfunctions(void);
    ~Printfunctions(void);

    void GetPaths(void);
    void GetOptions(GtkWindow *parent, char *printername, int which);
    void SetDefaults(void);
    int GetPPDOpts(char *printername);
    void Constraints(int prob);
    void PPD_DropDown_changed(GtkWidget *widget, gpointer data);
    void ResetButton(gpointer data);
    void CreateWidgets(void);
    void ShowWidgets(void);
    void ResetFunc(void);
    void PrintFunc(const char *path);
    void SaveGtkLPPrefs(GtkButton *widget);
    int AddAllOptions(cups_option_t **optionsptr, int num_options);
    bool PrinterforPC(void);
private:
    static Printfunctions *m_ptrInstance;
    static GtkWidget *button_inst_save;
    static GtkWidget *button_inst_remove;
    static GtkWidget *instwin;

    int Teststdin(void);
    int AddAllOptionsHelp(ppd_group_t *group, cups_option_t **optionsptr, int num_options);
    void SetPrinterAsDefault(char *printername);
    int SaveOptions(char *printername, int onlyglobal);
    int Freadline(FILE *file, char *String, unsigned howmuch);
    void SaveFunc(GtkWidget *widget, gpointer instance);
    void PrintPPDoptsHelp(ppd_group_t *group);
    void InstWinEntryChanged(GtkWidget *widget);
    //single
    void ButtonWinEntryChange(GtkWidget *widget);
    static void button_win_entry_change(Printfunctions *data, GtkWidget *widget){data->ButtonWinEntryChange(widget);}
};

#endif /* PRINTFUNCTIONS_H_ */