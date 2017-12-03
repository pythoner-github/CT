/*
 * Printtext.h
 *
 *  Created on: 2012-8-6
 *      Author: yangyaoqing
 */

#ifndef PRINTTEXT_H_
#define PRINTTEXT_H_

#include <gtk/gtk.h>
#include "Printlib.h"

class Printtext : public Printlib
{
public:
	static Printtext* GetInstance();
	static GtkWidget *textSizesFrame;
	static GtkWidget *textMarginsFrame;
	static GtkWidget *textPrettyFrame;
	static GtkWidget *textTab;
	static GtkWidget *textCPIField;
	static GtkWidget *textLPIField;
	static GtkWidget *textCPPField;

	Printtext(void);
	~Printtext(void);
	void TextTabSetDefaults(void);
	void TextTab(void);

private:
	static Printtext* m_ptrInstance;
	static GtkWidget *wrapCheck;
	static GtkWidget *textLeftField;
	static GtkWidget *textRightField;
	static GtkWidget *textTopField;
	static GtkWidget *textBottomField;
	static GtkWidget *textLeftField_I;
	static GtkWidget *textRightField_I;
	static GtkWidget *textTopField_I;
	static GtkWidget *textBottomField_I;
	static GtkWidget *textLeftField_C;
	static GtkWidget *textRightField_C;
	static GtkWidget *textTopField_C;
	static GtkWidget *textBottomField_C;
	static GtkWidget *prettyCheck;

	void TextSizesChanged(gpointer data);
	void TextSizes(GtkWidget *container);
	void TextMarginsChanged(gpointer data);
	void MarginSizes(GtkWidget *container);
	void WantPrettyCheck(gpointer data);
	void TextForm(GtkWidget *container);
	void WantWrapCheck(GtkWidget *widget,gpointer data);

	void ButtonTextSizesChange(GtkWidget *button, gpointer data);
	void ButtonReset(GtkWidget *button,  gpointer data);
	void ButtonTextMarginsChanged(GtkWidget *button, gpointer data);
	void ButtonWantPrettyCheck(GtkWidget *button, gpointer data);

	static void button_textsizeschange(GtkWidget *button,  gpointer dat, Printtext *data)
		{data->ButtonTextSizesChange(button, dat);}
	static void button_reset(GtkWidget *button, gpointer dat, Printtext *data)
		{ data->ButtonReset(button, dat);}
	static void button_textmarginschanged(GtkWidget *button,  gpointer dat, Printtext *data)
		{ data->ButtonTextMarginsChanged(button, dat);}
	static void button_wantprettycheck(GtkWidget *button,  gpointer dat, Printtext *data)
		{ data->ButtonWantPrettyCheck(button, dat);}
};

#endif /* PRINTTEXT_H_ */
