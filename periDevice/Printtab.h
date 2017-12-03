/*
 * Printtab.h
 *
 *  Created on: 2012-8-6
 *      Author: yangyaoqing
 */
#ifndef PRINTTAB_H_
#define PRINTTAB_H_

#include <gtk/gtk.h>
#include "Printlib.h"

class Printtab : public Printlib
{
public:
	static Printtab* GetInstance();
	Printtab(void);
	~Printtab(void);
	static 	GtkWidget *prefsFrame;
	static 	GtkWidget *gtklpViewable;
	static 	GtkWidget *pathesFrame;
	static 	GtkWidget *gtklpTab;

	void GtklpTabSetDefaults(void);
	void GtkPrinterTab(void);

private:
	static  Printtab* m_ptrInstance;
	static 	GtkWidget *saveSizePosCheck;
	static 	GtkWidget *saveAnzCheck;
	static 	GtkWidget *gtklpViewOut;
	static 	GtkWidget *gtklpViewText;
	static 	GtkWidget *gtklpViewImage;
	static 	GtkWidget *gtklpViewHpgl2;
	static 	GtkWidget *gtklpViewSpecial;
	static 	GtkWidget *gtklpViewPPD;
	static 	GtkWidget *remPrinterCheck;
	static  GtkWidget *remTabCheck;
	static  GtkWidget *saveExitCheck;
//	static 	GtkWidget *jobIDOnExitCheck;
	static 	GtkWidget *wantconstCheck;
	static 	GtkWidget *clearFileListCheck;

	struct Printtabdata{
		GtkWidget *button;
		gpointer data;
	};

	void GtklpViewable(GtkWidget *container);
	void PrefsChanged(gpointer data);
	void ConstButton(GtkWidget *widget);
	void PrefsFrame(GtkWidget *container);
	GtkWidget *GtklpTabFrame(GtkWidget *container);

	void ViewChangedOut(GtkWidget *button,  gpointer data);
	void ViewChangedText(GtkWidget *button, gpointer data);
	void ViewChangedImage(GtkWidget *button, gpointer data);
	void ViewChangedHpgl2(GtkWidget *button, gpointer data);
	void ViewChangedPPD(GtkWidget *button, gpointer data);
	void ButtonPrefsChange(GtkWidget *button, gpointer data);
	void ButtonConstButton(GtkWidget *button, GtkWidget *widget);

	static void button_pview_changeout(GtkWidget *button, Printtabdata *dat, Printtab *data)
		{data->ViewChangedOut(button, dat);}
	static void button_pview_changetext(GtkWidget *button, gpointer dat, Printtab *data)
		{data->ViewChangedText(button, dat);}
	static void button_pview_changeimage(GtkWidget *button, gpointer dat, Printtab *data)
		{data->ViewChangedImage(button, dat);}
	static void button_pview_changehpgl2(GtkWidget *button, gpointer dat, Printtab *data)
		{data->ViewChangedHpgl2(button, dat);}
	static void button_pview_changeppd(GtkWidget *button, gpointer dat, Printtab *data)
		{data->ViewChangedPPD(button, dat);}
	static void button_prefs_change(GtkWidget *button, gpointer dat, Printtab *data)
		{data->ButtonPrefsChange(button, dat);}
	static void button_constbutton(GtkWidget *button, GtkWidget *widget, Printtab *data)
		{data->ButtonConstButton(button, widget);}
};

#endif /* PRINTTAB_H_ */
