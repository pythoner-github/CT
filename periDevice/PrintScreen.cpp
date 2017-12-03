#include "PrintScreen.h"
#include "ViewMain.h"
#include "ModeStatus.h"
#include "FreezeMode.h"
#include "ViewPrintPreview.h"
#include "Usblp.h"
#include "Def.h"
#include "SysGeneralSetting.h"
#include "TopArea.h"

#define SCREENSHOT_PATH "/tmp/screenshot"

PrintScreen::PrintScreen()
{
}

PrintScreen::~PrintScreen()
{
}

gboolean PrintScreen::PrintP1()
{
	if(!ScreenShotToFile())return FALSE;

    ViewPrintPreview::GetInstance()->QuickPrintScreen(SCREENSHOT_PATH);
	return TRUE;
}

gboolean PrintScreen::PrintP2()
{
	if(!ScreenShotToFile())return FALSE;

    ViewPrintPreview::GetInstance()->CreateWindow(SCREENSHOT_PATH, ViewMain::GetInstance()->GetMainWindow());
	return TRUE;
}

gboolean PrintScreen::PrintP3()
{
	if(!ScreenShotToFile())return FALSE;

    ViewPrintPreview::GetInstance()->CreateWindow(SCREENSHOT_PATH, ViewMain::GetInstance()->GetMainWindow());
	return TRUE;
}

gboolean PrintScreen::ScreenShotToFile()
{
//    double imagescale = 1.0;
	GdkScreen* screen;
	GdkWindow* window;
	GdkPixbuf* screen_pixbuf = NULL;
	GdkPixbuf* rotate_pixbuf = NULL;
	GError* error = NULL;
	screen = gdk_screen_get_default();
	window = gdk_screen_get_root_window(screen);

    int src_x, src_y, width, height;
    SysGeneralSetting *sysGeneralSetting = new SysGeneralSetting;    
    int tvout_format = sysGeneralSetting->GetVideoFormat();
    delete sysGeneralSetting;

    switch (tvout_format) 
    {
        case 0:
            src_x = 0; 
            src_y = 0; 
            width = SCREEN_WIDTH; 
            height = SCREEN_HEIGHT;
            break;
        case 1:
            src_x = KNOB_X; 
            src_y = 0; 
            width = TOP_AREA_W; 
            height = KNOB_Y;
            break;
        case 2:
            src_x = KNOB_X; 
            src_y = TOP_AREA_H; 
            width = TOP_AREA_W; 
            height = KNOB_Y - TOP_AREA_H;
            break;
    }
	screen_pixbuf = gdk_pixbuf_get_from_drawable(NULL, 
			window, 
			gdk_colormap_get_system(),
			src_x, src_y, 0, 0, width, height);

//	rotate_pixbuf = gdk_pixbuf_rotate_simple(screen_pixbuf, GDK_PIXBUF_ROTATE_CLOCKWISE);
	rotate_pixbuf = gdk_pixbuf_rotate_simple(screen_pixbuf, GDK_PIXBUF_ROTATE_COUNTERCLOCKWISE);
	if(G_IS_OBJECT(screen_pixbuf))g_object_unref(screen_pixbuf);

    remove(SCREENSHOT_PATH);
	if(!gdk_pixbuf_save(rotate_pixbuf, SCREENSHOT_PATH, "bmp", &error, NULL))
	{
		PRINTF("%s: %s\n", __FUNCTION__, error->message);
		if(G_IS_OBJECT(rotate_pixbuf))
			g_object_unref(rotate_pixbuf);
		return FALSE;
	}
    else
    {
    	if(G_IS_OBJECT(rotate_pixbuf))
    		g_object_unref(rotate_pixbuf);
    	return TRUE;
    }
}
