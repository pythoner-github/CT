/*
 * Printmain.h
 *
 *  Created on: 2012-8-3
 *      Author: yangyaoqing
 */

#ifndef PRINTMAIN_H_
#define PRINTMAIN_H_

#include <gtk/gtk.h>
#include <cups/cups.h>
#include <cups/ppd.h>
#include <cups/http.h>

#include "FakeXEvent.h"
#include "Printdefaults.h"
#include "Printlib.h"

class Printmain : public Printlib
{
public:
	Printmain(void);
	~Printmain(void);
    static Printmain* GetInstance();
	void ViewPrintWindowCreat(GtkWindow *parent);
	void GetPrintOptions(void);
	void ButtonDestroy(GtkButton *button);

private:
    static Printmain* m_ptrInstance;

	void ButtonPrint(GtkButton *button);
	void ButtonReset(GtkButton *button);
	void ButtonSave(GtkButton *button);
	void ButtonAbbruch(GtkButton *button);
	static void button_print_clicked(GtkButton *button, Printmain *data) { data->ButtonPrint(button);}
	static void button_reset_clicked(GtkButton *button, Printmain *data) { data->ButtonReset(button); }
	static void button_save_clicked(GtkButton *button, Printmain *data) { data->ButtonSave(button); }
	static void button_abbruch_clicked(GtkButton *button, Printmain *data){ data->ButtonAbbruch(button); }// { data->BtnCancelClicked(button); }//
	static void button_destroy_clicked(GtkButton *button, Printmain *data) { data->ButtonDestroy(button); }
};

#endif /* PRINTMAIN_H_ */
