/*
 * Printimage.h
 *
 *  Created on: 2012-8-6
 *      Author: yangyaoqing
 */

#ifndef PRINTIMAGE_H_
#define PRINTIMAGE_H_

#include <gtk/gtk.h>
#include "Printlib.h"

class Printimage : public Printlib
{
public:
	static Printimage* GetInstance();
	Printimage(void);
	~Printimage(void);
	static GtkObject *imageHUEAdj;
	static GtkObject *imageSatAdj;
	static GtkWidget *imagePosFrame;
	static GtkWidget *imageScalingAllFrame;
	static GtkWidget *imageHUEFrame;
	static GtkWidget *imageSatFrame;
	static GtkWidget *imageTab;

	void ImageTabSetDefaults(void);
	void ImageTab(void);

private:
	static Printimage* m_ptrInstance;
	static GtkObject *imageScaleAllAdj;
	static GtkWidget *imageHUEButton;
	static GtkWidget *imageSatButton;
	static GtkWidget *scaleTypeCombo;
	static GtkWidget *imageScaleAllScale;

	void ImageScalingAdjChanged(GtkWidget *widget, gpointer data);
	void ImageScalingAllChanged(GtkWidget *widget, gpointer data);
	void ImageScalingAllFrame(GtkWidget *container);
	void ImageHUEFrame(GtkWidget *container);
	void ImageSatFrame(GtkWidget *container);

	void ButtonImageScalingAllChanged(GtkWidget *button, GtkWidget *widget, gpointer data);
	void ButtonImageScalingAdjChanged(GtkWidget *button, GtkWidget *widget);
	void ButtonReset(GtkButton *button, gpointer data);
	static void button_imagescalingallchanged(GtkWidget *button, GtkWidget *widget, gpointer dat, Printimage *data)
		{data->ButtonImageScalingAllChanged(button, widget, dat);}
	static void button_imageScalingadjchanged(GtkWidget *button, GtkWidget *widget, Printimage *data)
		{data->ButtonImageScalingAdjChanged(button, widget);}
	static void button_reset(GtkButton *button, gpointer dat, Printimage *data)
		{data->ButtonReset(button, dat);}
};

#endif /* PRINTIMAGE_H_ */
