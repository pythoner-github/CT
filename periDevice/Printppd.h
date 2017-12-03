/*
 * Printppd.h
 *
 *  Created on: 2012-8-13
 *      Author: yangyaoqing
 */

#ifndef PRINTPPD_H_
#define PRINTPPD_H_

#include <gtk/gtk.h>
#include <cups/ppd.h>
#include "Printlib.h"

class Printppd : public Printlib
{
public:
	Printppd(void);
	~Printppd(void);

	static Printppd* GetInstance();
	static GtkWidget *ppdTab;
	void PpdTabSetDefaults(void);
	void PPDTab(void);
private:
	static Printppd* m_ptrInstance ;
	static int ppd_entry_count;
	static GtkWidget *FrameHBox;
	static GtkWidget *FrameVBox;
	static GtkWidget *ppdFrame;

	int TogglePPDOption(ppd_group_t *group, int ct, int nummer, int enable);
	void EnablePPDOption(int nummer);
	void DisablePPDOption(int nummer);
	void AddPPDGroup(ppd_group_t *group);
	void PPDFrame(GtkWidget *container);

	void PPD_Multi_changed(GtkWidget  *widget, int  data);
	static void button_multi_change(GtkWidget *button, int  dat, Printppd *data)
		{data->PPD_Multi_changed(button, dat);}
	void PPD_DropDown_changed(GtkWidget *widget, gpointer dat);
	static void button_dropdown_change(GtkWidget *button, gpointer dat, Printppd *data)
	{
		printf("button_dropdown_change1\n");
		data->PPD_DropDown_changed(button, dat);
	}
};

#endif /* PRINTPPD_H_ */
