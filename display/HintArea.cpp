#include <gtk/gtk.h>
#include "HintArea.h"
#include "gui_func.h"
#include "gui_global.h"
#include "Def.h"
#include "MenuProjectMode.h"  
#include "ViewSuperuser.h"    

HintArea* HintArea::m_ptrInstance = NULL;

HintArea* HintArea::GetInstance()
{
    if (m_ptrInstance == NULL)
	m_ptrInstance = new HintArea;
    return m_ptrInstance;
}

HintArea::HintArea(void)
{
    m_pixmapHint = 0;
    m_width = 0;
    m_height = 0;
}

HintArea::~HintArea()
{ 
    if (m_ptrInstance != NULL) 
    	delete m_ptrInstance; 
}

gboolean HandleClearHint(gpointer data)
{
	HintArea *tmp;
	tmp = (HintArea *)data;
	tmp->ClearHint();
	tmp->m_timeout = 0;
	return FALSE;
}

/*
 * @brief display operation suggestion in hint area
 *
 * @para text[in] content to be pop up in hint area
 * @para timeout[in] time to clean hint area after some message is displayed, unit:ms
 */
void HintArea::UpdateHint(const char *text, int timeout)
{
#ifdef EMP_PROJECT
   if(ViewSuperuser::GetInstance()->GetProjectModeStatus())
    {
        g_menuProjectMode.AddMessage(text);
    }
    else
#endif   
 {
        ClearHint();
        char font_desc[100];
        int width, height;
        PangoLayout *layout = gtk_widget_create_pango_layout(m_hintArea, text);  
        pango_layout_get_pixel_size(layout, &width, &height);
        sprintf(font_desc, "%s, 13", FONT_STRING);
        PangoFontDescription *font = pango_font_description_from_string(font_desc);
        //printf("font width: %d\n", width);
        if (width > 645)
        {    
            DrawString(text, 5, 0, g_white, font);
        }
        else
        {
            DrawString(text, 5, 5, g_white, font);
        }

    if (timeout == 0)
	return;

        if(m_timeout > 0) {
            g_source_remove(m_timeout);
            m_timeout = 0;
        }
        m_timeout = g_timeout_add_seconds(timeout, HandleClearHint, this);
    }
}

void HintArea::ClearHint(void)
{
#ifdef EMP_PROJECT
    if(ViewSuperuser::GetInstance()->GetProjectModeStatus())
    {
        g_menuProjectMode.SetMessage("");
    }
    else
#endif
    {
        GdkGC *gc = gdk_gc_new(m_pixmapHint);
        gdk_gc_set_foreground(gc, g_black);
        gdk_draw_rectangle(m_pixmapHint, gc, TRUE, 0, 0, HINT_AREA_W, HINT_AREA_H);
        UpdateHintArea();
    }
}

GtkWidget * HintArea::Create(void)
{
    m_hintArea = gtk_drawing_area_new();
    gtk_drawing_area_size(GTK_DRAWING_AREA(m_hintArea), HINT_AREA_W, HINT_AREA_H);
    g_signal_connect(G_OBJECT(m_hintArea), "configure_event", G_CALLBACK(HandleHintAreaConfigure), this);
    g_signal_connect(G_OBJECT(m_hintArea), "expose_event", G_CALLBACK(HandleHintAreaExpose), this);

    return m_hintArea;
}

void HintArea::HintAreaConfigure(GtkWidget *widget, GdkEventConfigure *event)
{
    if (m_pixmapHint)
	g_object_unref(m_pixmapHint);
    m_pixmapHint = gdk_pixmap_new(widget->window,
				  widget->allocation.width,
				  widget->allocation.height,
				  -1);

    GdkGC *gc = gdk_gc_new(m_pixmapHint);
    gdk_gc_set_foreground(gc, g_black);
    gdk_draw_rectangle(m_pixmapHint, gc, TRUE, 0, 0, widget->allocation.width, widget->allocation.height);
    g_object_unref(gc);
}

void HintArea::HintAreaExpose(GtkWidget *widget, GdkEventExpose *event)
{
    gdk_draw_drawable(widget->window,
		      widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
		      m_pixmapHint,
		      0, 0,
		      0, 0,
		      HINT_AREA_W, HINT_AREA_H);
}

void HintArea::DrawString(const char *str, int x, int y, GdkColor *color, PangoFontDescription *font)
{
    int width, height;

    GdkGC *gc = gdk_gc_new(m_pixmapHint);
    PangoLayout *layout = gtk_widget_create_pango_layout(m_hintArea, str); 
    if (font)
    	pango_layout_set_font_description(layout, font);
    pango_layout_set_alignment(layout, PANGO_ALIGN_LEFT);
    pango_layout_get_pixel_size(layout, &width, &height);

    gdk_gc_set_foreground(gc, g_black);
    gdk_draw_rectangle(m_pixmapHint, gc, TRUE, x, y, m_width, m_height);

    gdk_gc_set_foreground(gc, color);
    gdk_draw_layout(m_pixmapHint, gc, x, y, layout);

    m_width = width;
    m_height = height;

    g_object_unref(layout);
    g_object_unref(gc);

    UpdateHintArea();
}

void HintArea::UpdateHintArea(void)
{
#ifdef EMP_PROJECT
    if(ViewSuperuser::GetInstance()->GetProjectModeStatus())
    {
        gtk_widget_hide_all(m_hintArea);
    }
    else
#endif
    {
        //gtk_widget_show_all(m_hintArea);
        gtk_widget_queue_draw(m_hintArea);
    }
}

