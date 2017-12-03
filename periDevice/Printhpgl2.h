/*
 * Printhpgl2.h
 *
 *  Created on: 2012-8-6
 *      Author: yangyaoqing
 */

#ifndef PRINTHPGL2_H_
#define PRINTHPGL2_H_

#include <gtk/gtk.h>
#include "Printlib.h"

class Printhpgl2 : public Printlib
{
public:
	static Printhpgl2* GetInstance();
	Printhpgl2(void);
	~Printhpgl2(void);

	static int hpgl2black;
	static int hpgl2fit;
	static GtkWidget *hpgl2PenFrame;
	static GtkWidget *hpgl2OptFrame;
	static GtkWidget *hpgl2Tab;
	static GtkWidget *hpgl2PenField;

	void Hpgl2TabSetDefaults(void);
	void Hpgl2Tab(void);

private:
	static Printhpgl2* m_ptrInstance;
	static GtkWidget *hpgl2BlackCheck;
	static GtkWidget *hpgl2FitCheck;

	void Hpgl2CheckChanged(gpointer data);
	void Hpgl2OptFrame(GtkWidget *container);
	void Hpgl2PenFrame(GtkWidget *container);
	void Hpgl2PenChanged(GtkWidget *widget);

	void ButtonPrinthpgl2Clicked(GtkWidget *button, gpointer data);
	void ButtonHpgl2PenFrame(GtkWidget *button, GtkWidget *widget);
	void ButtonHpgl2PenChanged(GtkWidget *button,GtkWidget *widget);
	void ButtonHpgl2ResetButton(gpointer data);

    static void on_button_printhpgl2clicked(GtkWidget *button, gpointer dat, Printhpgl2 *data)
    { data->ButtonPrinthpgl2Clicked(button, dat);}
    static void on_button_hpgl2penframe(GtkWidget *button, GtkWidget *widget, Printhpgl2 *data)
    { data->ButtonHpgl2PenFrame(button, widget);}
    static void on_button_hpgl2penchange(GtkWidget *button, GtkWidget *widget, Printhpgl2 *data)
    {data->ButtonHpgl2PenChanged(button, widget);}
    static void on_button_hpgl2resetbutton(Printhpgl2 *data, gpointer dat)
    { data->ButtonHpgl2ResetButton(dat);}
};

#endif /* PRINTHPGL2_H_ */
