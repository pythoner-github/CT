#ifndef GUI_GLOBAL_H
#define GUI_GLOBAL_H

#include <locale.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>

#define LOCALEDIR "/usr/share/locale"  //locale file directory
#define PACKAGE "09001_lang"

#define PI 3.1415926

#define IMAGE_W IMG_W //628
#define IMAGE_H IMG_H //440

extern GdkColor* g_black;
extern GdkColor* g_white;
extern GdkColor* g_red;
extern GdkColor* g_green;
extern GdkColor* g_deep;
extern GdkColor* g_deepGray;
extern GdkColor* g_gray;
extern GdkColor* g_lightGray;
extern GdkColor* g_yellow;
extern GdkColor* g_blue;
extern GdkColor* g_lightBlue;
extern GdkColor* g_skyBlue;

extern const char* examType[];
extern const char* Toggle[2];

#endif
