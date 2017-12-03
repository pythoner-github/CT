/*
 * Printgeneral.h
 *
 *  Created on: 2012-8-3
 *      Author: yangyaoqing
 */

#ifndef PRINTGENERAL_H_
#define PRINTGENERAL_H_

#include <gtk/gtk.h>
#include "Printlib.h"

class Printgeneral : public Printlib
{
public:
	static Printgeneral* GetInstance();
	Printgeneral(void);
	~Printgeneral(void);

	static GtkWidget *generalTab;
	static GtkWidget *printerFrame;
	static GtkWidget *printerFrameCombo;
	static GtkWidget *printerFrameInfoText;
	static GtkWidget *printerNumCopiesFrame;
	static GtkWidget *collateCheck;
	static GtkWidget *printerDuplexFrame;
	static GtkWidget *printerMediaFrame;

	void GeneralTab(void);
	void GeneralTabSetDefaults(void);
	void PrinterChanged(gpointer data);

private:
	static Printgeneral* m_ptrInstance;
	static GtkWidget *printerNumField;
	static GtkWidget *printerMediaSizeCombo;
	static GtkWidget *printerMediaTypeCombo;
	static GtkWidget *printerMediaSourceCombo;
	static GtkWidget *MediaTypeFrame;
	static GtkWidget *MediaSizeFrame;
	static GtkWidget *MediaSourceFrame;
	static GSList *DuplexButtons;
	static GtkWidget *printerFrameLocationText;


	void NumOfCopiesChanged(GtkWidget *widget);
	void PrinterMedia(GtkWidget *container);
	void PrinterNumCopies(GtkWidget *container);
	void PrinterFrame(GtkWidget *container);
	void WantCollateCheck(GtkWidget *widget);

	void ButtonPrinterChange(GtkWidget *button, gpointer data);
	void ButtonPPDDropDownChanged(GtkWidget *button, gpointer data);
	void ButtonNumOfCopiesChanged(GtkWidget * button, GtkWidget *widget);
	void ButtonWantCollateCheck(GtkWidget * button, GtkWidget *widget);

	static void button_print_change(GtkWidget *button, gpointer dat, Printgeneral *data)
		{data->ButtonPrinterChange(button, dat);}
	static void button_ppd_drop_down_change(GtkWidget *button, gpointer dat, Printgeneral *data)
		{data->ButtonPPDDropDownChanged(button, dat);}
	static void button_numofcopies_change(GtkWidget * button, GtkWidget *widget, Printgeneral *data)
		{data->ButtonNumOfCopiesChanged(button, widget);}
	static void button_wantcollate_check(GtkWidget * button, GtkWidget *widget, Printgeneral *data)
		{data->ButtonWantCollateCheck(button, widget);}
};

#endif /* PRINTGENERAL_H_ */
