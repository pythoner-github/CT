#include "display/gui_func.h"
#include "keyboard/KeyDef.h"
#include <stdlib.h>
#include <unistd.h>
#include <gdk/gdkkeysyms.h>
#include "ViewMain.h"
#include <libavutil/time.h>
#include <glib.h>
#include <iostream>
#include <math.h>
#include "display/gui_global.h"
#include "display/ImageArea.h"
#include "display/MenuArea.h"
#include "sysMan/SysGeneralSetting.h"
#include <sstream>
#include <string>
#include "periDevice/ManRegister.h"
#include "sysMan/ViewSystem.h"
#include "sysMan/SysNoteSetting.h"
#include "periDevice/DCMRegister.h"

using std::string;
using std::cout;
using std::endl;

GdkColor* g_black;
GdkColor* g_white;
GdkColor* g_red;
GdkColor* g_green;
GdkColor* g_blue;
GdkColor* g_deep;
GdkColor* g_deepGray;
GdkColor* g_gray;
GdkColor* g_lightGray;
GdkColor* g_yellow;
GdkColor* g_lightBlue;
GdkColor* g_skyBlue;

GdkColor* new_color(guint16 red, guint16 green, guint16 blue)
{
    GdkColor *color = new GdkColor;
    color->red = red;
    color->green = green;
    color->blue = blue;

    GdkColormap *colormap = gdk_colormap_get_system();
    if (gdk_colormap_alloc_color(colormap, color, FALSE, TRUE) == TRUE)
	return color;
    else
	return NULL;
}

void free_color(GdkColor *color)
{
    if (color)
	delete color;
    else
	return;
}

bool init_colors(void)
{
    if ((g_black = new_color(0, 0, 0)) == NULL)
	return false;
    if ((g_white = new_color(65535, 65535, 65535)) == NULL)
	return false;
    if ((g_red = new_color(65535, 0, 0)) == NULL)
	return false;
    if ((g_green = new_color(0, 65535, 0)) == NULL)
	return false;
    if ((g_blue = new_color(0, 0, 65535)) == NULL)
	return false;
    if ((g_deep = new_color(42*65535/255, 47*65535/255, 58*65535/255)) == NULL)
	return false;
    if ((g_deepGray = new_color(85*65535/255, 89*65535/255, 97*65535/255)) == NULL)
	return false;
    if ((g_gray = new_color(127*65535/255, 130*65535/255, 137*65535/255)) == NULL)
	return false;
    if ((g_lightGray = new_color(170*65535/255, 172*65535/255, 176*65535/255)) == NULL)
	return false;
    if ((g_yellow = new_color(255*65535/255, 255*65535/255, 0)) == NULL)
	return false;
    if ((g_lightBlue = new_color(173*65535/255, 216*65535/255, 230*65535/255)) == NULL)
	return false;
    if ((g_skyBlue = new_color(0, 255*65535/255, 255*65535/255)) == NULL)
	return false;
    return true;
}

void free_init_colors(void)
{
    delete g_black;
    delete g_white;
    delete g_red;
    delete g_green;
    delete g_blue;
    delete g_deep;
    delete g_deepGray;
    delete g_gray;
    delete g_lightGray;
    delete g_lightBlue;
    delete g_skyBlue;
    delete g_yellow;
}

/*
 * UTF8编码常见格式
   U-00000000 – U-0000007F:  0xxxxxxx
   U-00000080 – U-000007FF:  110xxxxx 10xxxxxx
   U-00000800 – U-0000FFFF:  1110xxxx 10xxxxxx 10xxxxxx
   U-00010000 – U-001FFFFF:  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
   U-00200000 – U-03FFFFFF:  111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
   U-04000000 – U-7FFFFFFF:  1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
 */
void CountUTF8(const void* pBuffer, long size, int *ascii, int *utf8)
{
	int a = 0;
	int u = 0;
	unsigned char* start = (unsigned char*)pBuffer;
	unsigned char* end = (unsigned char*)pBuffer + size;

	while(start < end)
	{
		if (*start < 0x80)
		{
			a++;
			start++;
		}
		else
		{
			u++;
			start += 3;
		}
	}

	*ascii = a;
	*utf8 = u;
}

void GetCurrentDate(int &Year, int &Month, int &Day)
{
    time_t now;
    struct tm *now_tm;
    time(&now);
    now_tm = localtime(&now);
    Year = now_tm->tm_year+1900;
    Month = now_tm->tm_mon+1;
    Day = now_tm->tm_mday;
}

void GetCurrentDateTime(int &Year, int &Month, int &Day, int &Hour, int &Minute, int &Second)
{
    time_t now;
    struct tm *now_tm;
    time(&now);
    now_tm = localtime(&now);
    Year = now_tm->tm_year+1900;
    Month = now_tm->tm_mon+1;
    Day = now_tm->tm_mday;
    Hour = now_tm->tm_hour;
    Minute = now_tm->tm_min;
    Second = now_tm->tm_sec;
}

void GetEntryTextForDB(GtkWidget *entry, string& text)
{
    if (gtk_entry_get_text_length(GTK_ENTRY(entry))) {
        text = gtk_entry_get_text(GTK_ENTRY(entry));
        trim(text);
    }
    else
        text = "%";
}

bool CalcAge(const int birth_year, const int birth_month, const int birth_day, int& age, int& age_unit)
{
    int cur_year, cur_month, cur_day;
    GetCurrentDate(cur_year, cur_month, cur_day);
    GDate* cur_date = g_date_new_dmy((GDateDay)cur_day, (GDateMonth)cur_month, (GDateYear)cur_year);
    if (birth_year == 0 && birth_month == 0 && birth_day == 0) {
	age = 0;
	age_unit = 0;
	g_date_free(cur_date);
	return false;
    }
    GDate* birth_date = g_date_new_dmy((GDateDay)birth_day, (GDateMonth)birth_month, (GDateYear)birth_year);
    if (g_date_compare(cur_date, birth_date) < 0) {
	g_date_free(cur_date);
	g_date_free(birth_date);
	return false;
    }

    int days = g_date_days_between(birth_date, cur_date);
    g_date_free(cur_date);
    g_date_free(birth_date);

    if (days >= 0 && days <=  100) {
	age = days;
	age_unit = 2;
	return true;
    } else if (days > 100 && days < 365) {
	if (birth_year == cur_year) {
	    if (cur_day >= birth_day)
		age = cur_month - birth_month;
	    else
		age = cur_month - birth_month - 1;
	} else {
	    if (cur_day >= birth_day)
		age = 12 - birth_month + cur_month;
	    else
		age = 12 - birth_month + cur_month - 1;
	}
	age_unit = 1;
	return true;
    } else if (days >= 365) {
	if (cur_month == birth_month) {
	    if (cur_day >= birth_day)
		age = cur_year - birth_year;
	    else
		age = cur_year - birth_year - 1;
	} else if (cur_month < birth_month) {
	    age = cur_year - birth_year - 1;
	} else {
	    age = cur_year - birth_year;
	}
	age_unit = 0;
	return true;
    } else {
	return false;
    }
}

void ChangeDateFormatToString(int iYear, int iMon, int iDay, string &sYear, string &sMon, string &sDay)
{
    char buf[20];

    sprintf(buf, "%d%d%d%d", iYear/1000, (iYear%1000)/100, (iYear%100)/10, iYear%10);
    sYear = buf;
    sprintf(buf, "%d%d", iMon/10, iMon%10);
    sMon = buf;
    sprintf(buf, "%d%d", iDay/10, iDay%10);
    sDay = buf;
}

void ChangeTimeFormatToString(int iHour, int iMin, int iSec, string &sHour, string &sMin, string &sSec)
{
    char buf[20];

    sprintf(buf, "%d%d", iHour/10, iHour%10);
    sHour = buf;
    sprintf(buf, "%d%d", iMin/10, iMin%10);
    sMin = buf;
    sprintf(buf, "%d%d", iSec/10, iSec%10);
    sSec = buf;

    //cout << "time is " << sHour << sMin << sSec << endl;

}

void GenerateDataString(string year, string mon, string day, string &date)
{
    char buf[100];

    SysGeneralSetting sgs;
    int dateFormat = sgs.GetDateFormat();
    switch (dateFormat) {
    case 0:
        sprintf(buf, "%s-%s-%s", year.c_str(), mon.c_str(), day.c_str());
        break;
    case 1:
        sprintf(buf, "%s-%s-%s", mon.c_str(), day.c_str(), year.c_str());
        break;
    case 2:
        sprintf(buf, "%s-%s-%s", day.c_str(), mon.c_str(), year.c_str());
        break;
    }
    date = buf;
}

void QuichSearchDateString(string year, string mon, string day, string &date)
{
    char buf[100];
    sprintf(buf, "%s-%s-%s", year.c_str(), mon.c_str(), day.c_str());
    date = buf;
}

void GetDateString(int iYear, int iMon, int iDay, string &date)
{
    string year, mon, day;
    ChangeDateFormatToString(iYear, iMon, iDay, year, mon, day);
    GenerateDataString(year, mon, day, date);
}

void GetTimeString(int iHour, int iMin, int iSec, string &time)
{
    string h, m, s;
    char buf[100];

    ChangeTimeFormatToString(iHour, iMin, iSec, h, m, s);
    sprintf(buf, "%s:%s:%s", h.c_str(), m.c_str(), s.c_str());
    time = buf;
}

void ValidDateFormat(string &sYear, string &sMon, string &sDay)
{
    int iYear = atoi(sYear.c_str());
    int iMon = atoi(sMon.c_str());
    int iDay = atoi(sDay.c_str());

    ChangeDateFormatToString(iYear, iMon, iDay, sYear, sMon, sDay);
}

/*
 * @brief get age from birthday.
 *
 * @para age[output] age in string format, 3numeric+1character, such as 080Y
 */
void GetAge(int year, int mon, int day, string &age)
{
    int iAge, ageUnit;

    if (CalcAge(year, mon, day, iAge, ageUnit))
        GenAgeStr(iAge, ageUnit, age);
    else
        GenAgeStr(0, 0, age);
}

void GenAgeStr(int age, int ageUnit, string &strAge)
{
    string unit;
    char buf[50];  //big because of russian

	switch (ageUnit)
	{
		case 0:
			unit = _("Y"); break;
		case 1:
			unit = _("M"); break;
		case 2:
			unit = _("D"); break;
		default:
			unit = _("Y"); break;
	}

    sprintf(buf, "%d%s", age, unit.c_str());
    strAge = buf;
}

void GetSexString(int iSex, string &sSex)
{
	switch (iSex)
	{
#ifdef VET
		case 0:
			sSex = _("Female "); break;
		case 1:
			sSex = _("Male "); break;
#else
		case 0:
			sSex = _("Female"); break;
		case 1:
			sSex = _("Male"); break;
#endif
		case 2:
			sSex = _("Other"); break;
		default:
			sSex = " "; break;
	}
}

#ifdef VET
void GetAnimalSpeciesString(int index, string &species)
{
	switch (index) {
		case 0:
			species = _("Dog"); break;
		case 1:
			species = _("Cat"); break;
		case 2:
			species = _("Sheep"); break;
		case 3:
			species = _("Swine"); break;
		case 4:
			species = _("Bovine"); break;
		case 5:
			species = _("Equine"); break;
		case 6:
			species = _("Other"); break;

		default:
			break;
	}
}
#endif

void GetTypeStr(string &src, string &des)
{
    string str1("");
    str1 = src;
    string str2 = '"' +str1 + '"';
    string str3 = '(' + str2 +')';
    string str4 = '_' + str3;
    des = str4;
    PRINTF("********src des %s %s\n",src.c_str(),des.c_str());
}

int CalcLength(POINT p1, POINT p2)
{
	int dx = p1.x - p2.x;
	int dy = p1.y - p2.y;

	int len = sqrt(dx*dx + dy*dy);

	return len;
}

void CommaToDotLocaleNumeric(char *Dot, unsigned int size)
{
    SysGeneralSetting sysGS;
    int language= sysGS.GetLanguage();

    if ((PL == language)||(RU == language)||(FR == language)||(DE == language)||(ES == language))
    {

        for(unsigned int i=0;i<size;i++)
        {
            if(Dot[i]==',')
            {
                Dot[i]='.';
                break;
            }
        }
    }

}

void DotToCommaLocaleNumeric(char *Comma, unsigned int size)
{

    SysGeneralSetting sysGS;
    int language= sysGS.GetLanguage();

    if ((PL == language)||(RU == language)||(FR == language)||(DE == language)||(ES == language))
    {
        for(unsigned int i=0; i<size; i++)
        {
            if(Comma[i]=='.')
            {
                Comma[i]=',';
                break;
            }
        }
    }

}

GtkWidget *create_label(const char *str, guint width, guint height, const GdkColor* const color, PangoFontDescription *font)
{
	GtkWidget *label = gtk_label_new(str);
	gtk_label_set_use_markup(GTK_LABEL(label), TRUE);
	if (width != 0 && height != 0)
		gtk_widget_set_usize(label, width, height);
	else if(width != 0 && height ==0)
		gtk_widget_set_usize(label, width, -1);
	else if(width ==0 && height !=0)
		gtk_widget_set_usize(label, -1, height);
	if (color)
		gtk_widget_modify_fg(label, GTK_STATE_NORMAL, color);
	if (font)
		gtk_widget_modify_font(label, font);
	return label;
}

GtkWidget *create_button(GtkWidget *label, guint width, guint height, const GdkColor* const color)
{
	GtkWidget *button = gtk_button_new();
	if (width != 0 && height != 0)
		gtk_widget_set_usize(button, width, height);
	else if(width != 0 && height ==0)
		gtk_widget_set_usize(button, width, -1);
	else if(width ==0 && height !=0)
		gtk_widget_set_usize(button, -1, height);
	if (label)
		gtk_container_add(GTK_CONTAINER(button), label);
	if (color)
		gtk_widget_modify_bg(button, GTK_STATE_NORMAL, color);
	gtk_button_set_focus_on_click(GTK_BUTTON(button), FALSE);
//	gtk_button_set_focus_on_click(GTK_BUTTON(button), true);//test--2016.08.11

	return button;
}

GtkWidget* create_button_icon(GtkWidget *label, GtkWidget *icon_image)
{
    int size =0;
#ifdef EMP_3410
    if(CManRegister::GetInstance()->IsAuthorize(CManRegister::Optional[0]))
#else
        if(CDCMRegister::GetMe()->IsAuthorize())
#endif
        size = 20;
    else
        size = 0;
    GtkWidget *button = gtk_button_new();
    gtk_widget_set_size_request (button, 120, 70-size);

    GtkWidget *alignment = gtk_alignment_new (0.5, 0.5, 0, 0);
    gtk_container_add (GTK_CONTAINER (button), alignment);

    GtkWidget *hbox = gtk_hbox_new (FALSE, 2);
    gtk_container_add (GTK_CONTAINER (alignment), hbox);
    gtk_box_pack_start (GTK_BOX (hbox), icon_image, FALSE, FALSE, 0);
	if (label)
	    gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);

	gtk_button_set_focus_on_click(GTK_BUTTON(button), FALSE);
    gtk_widget_show_all(button);

    return button;
}

GtkWidget* create_button_icon_only(GtkWidget *icon_image, guint width, guint height, const GdkColor *color)
{
    GtkWidget *button = gtk_button_new();

 	if (width != 0 && height != 0)
		gtk_widget_set_usize(button, width, height);
	else if(width != 0 && height ==0)
		gtk_widget_set_usize(button, width, -1);
	else if(width ==0 && height !=0)
		gtk_widget_set_usize(button, -1, height);
	if (icon_image)
		gtk_container_add(GTK_CONTAINER(button), icon_image);
	if (color)
		gtk_widget_modify_bg(button, GTK_STATE_NORMAL, color);

	gtk_button_set_focus_on_click(GTK_BUTTON(button), FALSE);
    gtk_widget_show_all(button);

    return button;
}

GtkWidget* create_combobox(guint width, guint height, const char *layouttype, GtkTreeModel *treemodel)
{
    GtkWidget* combobox;
    if(treemodel)
	combobox = gtk_combo_box_new_with_model(treemodel);
    else
	combobox = gtk_combo_box_new();
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combobox), renderer, TRUE);
    gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combobox), renderer, layouttype, 0, NULL);
    if (width != 0 && height != 0)
	gtk_widget_set_usize (combobox, width, height);
    gtk_combo_box_set_active (GTK_COMBO_BOX (combobox), 0);
    g_object_unref (treemodel);

    return combobox;
}

GtkWidget* create_combo_box_with_text(const int width, const int height, const vector<string> text)
{
    GtkTreeIter treeIter;
    GtkListStore *liststore = gtk_list_store_new(1, G_TYPE_STRING);
    for (vector<string>::const_iterator iter = text.begin(); iter != text.end(); ++iter) {
	gtk_list_store_append(liststore, &treeIter);
	gtk_list_store_set(liststore, &treeIter, 0, _((*iter).c_str()), -1);
    }

    GtkWidget *combobox = gtk_combo_box_new_with_model(GTK_TREE_MODEL(liststore));
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combobox), renderer, TRUE);
    gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combobox), renderer, "text", 0, NULL);

    if (width != 0 && height != 0)
	gtk_widget_set_usize(combobox, width, height);

    g_object_unref (liststore);
    return combobox;
}

#if 0
PangoLayout * create_pango_text(GtkWidget *widget,
				const char *text,
				int width,
				PangoFontDescription *font,
				const GdkColor* const color)
{
    PangoLayout *layout;
    PangoAttrList *attrs;
    PangoAttribute *attr;

    layout = gtk_widget_create_pango_layout(widget, text);

    if (width != 0)
	pango_layout_set_width(layout, width);
    if (font)
	pango_layout_set_font_description(layout, font);
    if (color) {
	attrs = pango_attr_list_new();
	attr = pango_attr_foreground_new(color->red, color->green, color->blue);
	pango_attr_list_change(attrs, attr);
	pango_layout_set_attributes(layout, attrs);
    }

    return layout;	//layout not be free
}
#endif

GtkWidget* create_pixmap(const gchar *filename)
{
    GtkWidget *pixmap;

    if (!filename || !filename[0])
	return gtk_image_new ();

    if (g_file_test(filename, G_FILE_TEST_EXISTS) == FALSE) {
	g_warning ("Couldn't find pixmap file: %s", filename);
	return gtk_image_new ();
    }
    pixmap = gtk_image_new_from_file(filename);
    return pixmap;
}

Display * getXDisplay(void)
{
    int ourEvent, ourError, ourMajor, ourMinor;

    Display *ourDisplay = XOpenDisplay(NULL);
    if (!ourDisplay) {
	PRINTF("\n ERROR: This program is made to be run in X Windows.\n\n");
	exit(1);
    }
    if (!XTestQueryExtension(ourDisplay, &ourEvent, &ourError, &ourMajor, &ourMinor)) {
	PRINTF("XServer %s doesn't support the Xtest extensions we need...\n", DisplayString(ourDisplay));
	XCloseDisplay(ourDisplay);
	exit(1);
    }
    XSync(ourDisplay, True);
    return ourDisplay;
}

void doKeyEvent(const unsigned int keycode, int is_press)
{
    Display *display = getXDisplay();
    unsigned long delay = 0;
    if (is_press)
	XTestFakeKeyEvent(display, keycode, True, delay);
    else
	XTestFakeKeyEvent(display, keycode, False, delay);
    usleep(20000);
    XFlush(display);
    XCloseDisplay(display);
}

void doKeysymEvent(const unsigned int keysym, int is_press)
{
    Display *display = getXDisplay();

	unsigned int keycode = XKeysymToKeycode(display, keysym);
    unsigned long delay = 0;
    if (is_press)
	XTestFakeKeyEvent(display, keycode, True, delay);
    else
	XTestFakeKeyEvent(display, keycode, False, delay);
    usleep(20000);
    XFlush(display);
    XCloseDisplay(display);
}

void doBtnEvent(const unsigned int button, int is_press)
{
    Display *display = getXDisplay();
    unsigned long delay = 0;
    if (is_press)
	XTestFakeButtonEvent(display, button, True, delay);
    else
	XTestFakeButtonEvent(display, button, False, delay);
    XCloseDisplay(display);
}

void doMotionEvent(int x, int y)
{
}

static POINT g_sysCursor;
void SetSystemCursor(int x, int y)
{
    g_sysCursor.x = x;
    g_sysCursor.y = y;

    Display* display;
    display = getXDisplay();
    XTestFakeMotionEvent(display, DefaultScreen(display), x, y, 0);
    XFlush(display);
    XCloseDisplay(display);
}
void ResetIfOutOfRange(void)
{
    if ((g_sysCursor.x > MENU_AREA_W) || (g_sysCursor.y > MENU_AREA_H))
        SetSystemCursor(SYSCURSOR_X, SYSCUROSR_Y);
}

void SetSystemCursorToCenter()
{
    int x = SCREEN_WIDTH / 2;
    int y = SCREEN_HEIGHT / 2;

    SetSystemCursor(x, y);
}

void fakeXMotionEvent(int offsetX, int offsetY)
{
    fakeXMotionEventMenu(g_sysCursor.x, g_sysCursor.y, offsetX, offsetY);
}

void fakeXMotionEventFullRange(int offsetX, int offsetY)
{
    fakeXMotionEventScreen(g_sysCursor.x, g_sysCursor.y, offsetX, offsetY);
}

void fakeXMotionEventImage(int &x0, int &y0, int offsetX, int offsetY)
{
    int x_start = IMG_AREA_X + IMAGE_X;
    int y_start = IMG_AREA_Y + IMAGE_Y;

    SysNoteSetting sns;
    int adjust;
    switch(sns.GetFontSize())
    {
        case 0:	adjust = 15+30;	break;
        case 1:	adjust =30;	break;
        case 2:	adjust =20;	break;
        default:	adjust =30;		break;
    }

    //Mouse x direction Move
    if (((x0+offsetX) > x_start) && (x0+offsetX) < (x_start + IMAGE_W))
        x0 += offsetX;
    if (((y0-offsetY) > y_start) && ((y0-offsetY) < (y_start + IMAGE_H-adjust)))
        y0 -= offsetY;

    Display *display = getXDisplay();
    XTestFakeMotionEvent(display, DefaultScreen(display), x0, y0, 0);
    XFlush(display);
    XCloseDisplay(display);
}

void fakeXMotionEventMenu(int &x0, int &y0, int offsetX, int offsetY)
{
   int x_start = 0;
   int y_start = 0;

   //Mouse x direction Move
   if (((x0+offsetX) > x_start) && (x0+offsetX) < (x_start + MENU_AREA_W))
	   x0 += offsetX;

   if (((y0-offsetY) > y_start) && ((y0-offsetY) < (y_start + MENU_AREA_H)))
	   y0 -= offsetY;

   Display *display = getXDisplay();
   XTestFakeMotionEvent(display, DefaultScreen(display), x0, y0, 0);
   XFlush(display);
   XCloseDisplay(display);
}

void fakeXMotionEventScreen(int &x0, int &y0, int offsetX, int offsetY)
{
	int x_start = 0;
	int y_start = 0;
	int w = SCREEN_WIDTH;
	int h = SCREEN_HEIGHT;

	//将鼠标移动范围锁定在SCREEN_WIDTH, SCREEN_HEIGHT之内
#if 0
	// get system size
	Display *temp;
	if(!(temp = XOpenDisplay(0))) {
		PRINTF("fake motion event screen: cannot open display\n");
		return;
	}
	int screen = DefaultScreen(temp);
	w = DisplayWidth(temp, screen);
	h = DisplayHeight(temp, screen);
	XCloseDisplay(temp);
#endif

	//Mouse x direction Move
	Display *display = getXDisplay();
	if (((x0+offsetX) > x_start) && (x0+offsetX) < (x_start + w))
		x0 += offsetX;

	if (((y0-offsetY) > y_start) && ((y0-offsetY) < (y_start + h)))
		y0 -= offsetY;

	XTestFakeMotionEvent(display, DefaultScreen(display), x0, y0, 0);
	XFlush(display);
	XCloseDisplay(display);
}
static int g_cursorRangeY0=128;
static int g_cursorRangeY1=250;
void SetMenuBiopsyCursorYRange(int y0,int y1)//2016.08.25
{
	g_cursorRangeY0=y0;
	g_cursorRangeY1=y1;
}
void fakeXMotionEventMenuBiopsy(int offsetX, int offsetY)//2016.08.24
{
   int x_start = 0;
  // int y_start =98+30; //TOP_AREA_H;
   int x0=g_sysCursor.x;
   int y0=g_sysCursor.y;

   //Mouse x direction Move
   if (((x0+offsetX) > x_start) && (x0+offsetX) < (x_start + MENU_AREA_W))
	   x0 += offsetX;

  /* if (((y0-offsetY) > y_start) && ((y0-offsetY) < (y_start + 100)))
	   y0 -= offsetY;*/
   if (((y0-offsetY) > g_cursorRangeY0) && ((y0-offsetY) < g_cursorRangeY1))
	   y0 -= offsetY;

   g_sysCursor.x=x0;
   g_sysCursor.y=y0;

   Display *display = getXDisplay();
   XTestFakeMotionEvent(display, DefaultScreen(display), x0, y0, 0);
   XFlush(display);
   XCloseDisplay(display);

}

unsigned char key_note;
gboolean FakeNumKey(unsigned char key)
{
     key_note=key;
    switch (key) {
        case 0x31:			// 1
        case 0x11:			// caps + number
	doKeyEvent(10, 1); doKeyEvent(10, 0); return TRUE;
    case 0x32:			// 2
    case 0x12:			// caps + number
	doKeyEvent(11, 1); doKeyEvent(11, 0); return TRUE;
    case 0x33:			// 3
    case 0x13:			// caps + number
	doKeyEvent(12, 1); doKeyEvent(12, 0); return TRUE;
    case 0x34:			// 4
    case 0x14:			// caps + number
	doKeyEvent(13, 1); doKeyEvent(13, 0); return TRUE;
    case 0x35:		       	// 5
    case 0x15:			// caps + number
	doKeyEvent(14, 1); doKeyEvent(14, 0); return TRUE;
    case 0x36:			// 6
    case 0x16:			// caps + number
	doKeyEvent(15, 1); doKeyEvent(15, 0); return TRUE;
    case 0x37:			// 7
    case 0x17:			// caps + number
	doKeyEvent(16, 1); doKeyEvent(16, 0); return TRUE;
    case 0x38:			// 8
    case 0x1C:			// caps + number
	doKeyEvent(17, 1); doKeyEvent(17, 0); return TRUE;
    case 0x39:			// 9
    case 0x1D:			// caps + number
	doKeyEvent(18, 1); doKeyEvent(18, 0); return TRUE;
    case 0x30:			// 0
    case 0x1E:			// caps + number
	doKeyEvent(19, 1); doKeyEvent(19, 0); return TRUE;
    }
    return FALSE;
}

#if defined(EMP_355) || defined(EMP_322)
gboolean FakeAlphabet(unsigned char key)
{
    key_note=key;
    SysGeneralSetting sysGS;
    int language= sysGS.GetLanguage();
    if(FR == language)
    {
        switch (key) {
            case 0x61:			// a
                doKeyEvent(38, 1); doKeyEvent(38, 0); return TRUE;

            case 0x41:			// A
                doKeyEvent(50, 1); doKeyEvent(38, 1);
                doKeyEvent(38, 0); doKeyEvent(50, 0); return TRUE;

            case 0x62:			// b
                doKeyEvent(56, 1); doKeyEvent(56, 0); return TRUE;
            case 0x42:			// B
                doKeyEvent(50, 1); doKeyEvent(56, 1);
                doKeyEvent(56, 0); doKeyEvent(50, 0); return TRUE;

            case 0x63:			// c
                doKeyEvent(54, 1); doKeyEvent(54, 0); return TRUE;
            case 0x43:			// C
                doKeyEvent(50, 1); doKeyEvent(54, 1);
                doKeyEvent(54, 0); doKeyEvent(50, 0); return TRUE;

            case 0x64:			// d
                doKeyEvent(40, 1); doKeyEvent(40, 0); return TRUE;
            case 0x44:			// D
                doKeyEvent(50, 1); doKeyEvent(40, 1);
                doKeyEvent(40, 0); doKeyEvent(50, 0); return TRUE;

            case 0x65:			// e
                doKeyEvent(26, 1); doKeyEvent(26, 0); return TRUE;
            case 0x45:			// E
                doKeyEvent(50, 1); doKeyEvent(26, 1);
                doKeyEvent(26, 0); doKeyEvent(50, 0); return TRUE;

            case 0x66:			// f
                doKeyEvent(41, 1); doKeyEvent(41, 0); return TRUE;
            case 0x46:			// F
                doKeyEvent(50, 1); doKeyEvent(41, 1);
                doKeyEvent(41, 0); doKeyEvent(50, 0); return TRUE;

            case 0x67:			// g
                doKeyEvent(42, 1); doKeyEvent(42, 0); return TRUE;
            case 0x47:			// G
                doKeyEvent(50, 1); doKeyEvent(42, 1);
                doKeyEvent(42, 0); doKeyEvent(50, 0); return TRUE;

            case 0x68:			// h
                doKeyEvent(43, 1); doKeyEvent(43, 0); return TRUE;
            case 0x48:			// H
                doKeyEvent(50, 1); doKeyEvent(43, 1);
                doKeyEvent(43, 0); doKeyEvent(50, 0); return TRUE;

            case 0x69:			// i
                doKeyEvent(31, 1); doKeyEvent(31, 0); return TRUE;
            case 0x49:			// I
                doKeyEvent(50, 1); doKeyEvent(31, 1);
                doKeyEvent(31, 0); doKeyEvent(50, 0); return TRUE;

            case 0x6A:			// j
                doKeyEvent(44, 1); doKeyEvent(44, 0); return TRUE;
            case 0x4A:			// J
                doKeyEvent(50, 1); doKeyEvent(44, 1);
                doKeyEvent(44, 0); doKeyEvent(50, 0); return TRUE;

            case 0x6B:			// k
                doKeyEvent(45, 1); doKeyEvent(45, 0); return TRUE;
            case 0x4B:			// K
                doKeyEvent(50, 1); doKeyEvent(45, 1);
                doKeyEvent(45, 0); doKeyEvent(50, 0); return TRUE;

            case 0x6C:			// l
                doKeyEvent(46, 1); doKeyEvent(46, 0); return TRUE;
            case 0x4C:			// L
                doKeyEvent(50, 1); doKeyEvent(46, 1);
                doKeyEvent(46, 0); doKeyEvent(50, 0); return TRUE;
            case 0x4D:			// M
            case 0x6D:			// m
                doKeyEvent(58, 1); doKeyEvent(58, 0); return TRUE;
                //  case 0x4D:			// M
            case 0x6E:			// n
                doKeyEvent(57, 1); doKeyEvent(57, 0); return TRUE;
            case 0x4E:			// N
                doKeyEvent(50, 1); doKeyEvent(57, 1);
                doKeyEvent(57, 0); doKeyEvent(50, 0); return TRUE;

            case 0x6F:			// o
                doKeyEvent(32, 1); doKeyEvent(32, 0); return TRUE;
            case 0x4F:			// O
                doKeyEvent(50, 1); doKeyEvent(32, 1);
                doKeyEvent(32, 0); doKeyEvent(50, 0); return TRUE;

            case 0x70:			// p
                doKeyEvent(33, 1); doKeyEvent(33, 0); return TRUE;
            case 0x50:			// P
                doKeyEvent(50, 1); doKeyEvent(33, 1);
                doKeyEvent(33, 0); doKeyEvent(50, 0); return TRUE;

            case 0x71:			// q
                doKeyEvent(24, 1); doKeyEvent(24, 0); return TRUE;
            case 0x51:			// Q
                doKeyEvent(50, 1); doKeyEvent(24, 1);
                doKeyEvent(24, 0); doKeyEvent(50, 0); return TRUE;

            case 0x72:			// r
                doKeyEvent(27, 1); doKeyEvent(27, 0); return TRUE;
            case 0x52:			// R
                doKeyEvent(50, 1); doKeyEvent(27, 1);
                doKeyEvent(27, 0); doKeyEvent(50, 0); return TRUE;

            case 0x73:			// s
                doKeyEvent(39, 1); doKeyEvent(39, 0); return TRUE;
            case 0x53:			// S
                doKeyEvent(50, 1); doKeyEvent(39, 1);
                doKeyEvent(39, 0); doKeyEvent(50, 0); return TRUE;

            case 0x74:			// t
                doKeyEvent(28, 1); doKeyEvent(28, 0); return TRUE;
            case 0x54:			// T
                doKeyEvent(50, 1); doKeyEvent(28, 1);
                doKeyEvent(28, 0); doKeyEvent(50, 0); return TRUE;

            case 0x75:			// u
                doKeyEvent(30, 1); doKeyEvent(30, 0); return TRUE;
            case 0x55:			// U
                doKeyEvent(50, 1); doKeyEvent(30, 1);
                doKeyEvent(30, 0); doKeyEvent(50, 0); return TRUE;

            case 0x76:			// v
                doKeyEvent(55, 1); doKeyEvent(55, 0); return TRUE;
            case 0x56:			// V
                doKeyEvent(50, 1); doKeyEvent(55, 1);
                doKeyEvent(55, 0); doKeyEvent(50, 0); return TRUE;

            case 0x77:			// w
                doKeyEvent(25, 1); doKeyEvent(25, 0); return TRUE;
            case 0x57:			// W
                doKeyEvent(50, 1); doKeyEvent(25, 1);
                doKeyEvent(25, 0); doKeyEvent(50, 0); return TRUE;

            case 0x78:			// x
                doKeyEvent(53, 1); doKeyEvent(53, 0); return TRUE;
            case 0x58:			// X
                doKeyEvent(50, 1); doKeyEvent(53, 1);
                doKeyEvent(53, 0); doKeyEvent(50, 0); return TRUE;

            case 0x79:			// y
                doKeyEvent(29, 1); doKeyEvent(29, 0); return TRUE;
            case 0x59:			// Y
                doKeyEvent(50, 1); doKeyEvent(29, 1);
                doKeyEvent(29, 0); doKeyEvent(50, 0); return TRUE;

            case 0x7A:			// z
                doKeyEvent(52, 1); doKeyEvent(52, 0); return TRUE;
            case 0x5A:			// Z
                doKeyEvent(50, 1); doKeyEvent(52, 1);
                doKeyEvent(52, 0); doKeyEvent(50, 0); return TRUE;
        }
    }
    else
    {

        switch (key) {
            case 0x61:			// a
                doKeyEvent(38, 1); doKeyEvent(38, 0); return TRUE;

            case 0x41:			// A
                doKeyEvent(50, 1); doKeyEvent(38, 1);
                doKeyEvent(38, 0); doKeyEvent(50, 0); return TRUE;

            case 0x62:			// b
                doKeyEvent(56, 1); doKeyEvent(56, 0); return TRUE;
            case 0x42:			// B
                doKeyEvent(50, 1); doKeyEvent(56, 1);
                doKeyEvent(56, 0); doKeyEvent(50, 0); return TRUE;

            case 0x63:			// c
                doKeyEvent(54, 1); doKeyEvent(54, 0); return TRUE;
            case 0x43:			// C
                doKeyEvent(50, 1); doKeyEvent(54, 1);
                doKeyEvent(54, 0); doKeyEvent(50, 0); return TRUE;

            case 0x64:			// d
                doKeyEvent(40, 1); doKeyEvent(40, 0); return TRUE;
            case 0x44:			// D
                doKeyEvent(50, 1); doKeyEvent(40, 1);
                doKeyEvent(40, 0); doKeyEvent(50, 0); return TRUE;

            case 0x65:			// e
                doKeyEvent(26, 1); doKeyEvent(26, 0); return TRUE;
            case 0x45:			// E
                doKeyEvent(50, 1); doKeyEvent(26, 1);
                doKeyEvent(26, 0); doKeyEvent(50, 0); return TRUE;

            case 0x66:			// f
                doKeyEvent(41, 1); doKeyEvent(41, 0); return TRUE;
            case 0x46:			// F
                doKeyEvent(50, 1); doKeyEvent(41, 1);
                doKeyEvent(41, 0); doKeyEvent(50, 0); return TRUE;

            case 0x67:			// g
                doKeyEvent(42, 1); doKeyEvent(42, 0); return TRUE;
            case 0x47:			// G
                doKeyEvent(50, 1); doKeyEvent(42, 1);
                doKeyEvent(42, 0); doKeyEvent(50, 0); return TRUE;

            case 0x68:			// h
                doKeyEvent(43, 1); doKeyEvent(43, 0); return TRUE;
            case 0x48:			// H
                doKeyEvent(50, 1); doKeyEvent(43, 1);
                doKeyEvent(43, 0); doKeyEvent(50, 0); return TRUE;

            case 0x69:			// i
                doKeyEvent(31, 1); doKeyEvent(31, 0); return TRUE;
            case 0x49:			// I
                doKeyEvent(50, 1); doKeyEvent(31, 1);
                doKeyEvent(31, 0); doKeyEvent(50, 0); return TRUE;

            case 0x6A:			// j
                doKeyEvent(44, 1); doKeyEvent(44, 0); return TRUE;
            case 0x4A:			// J
                doKeyEvent(50, 1); doKeyEvent(44, 1);
                doKeyEvent(44, 0); doKeyEvent(50, 0); return TRUE;

            case 0x6B:			// k
                doKeyEvent(45, 1); doKeyEvent(45, 0); return TRUE;
            case 0x4B:			// K
                doKeyEvent(50, 1); doKeyEvent(45, 1);
                doKeyEvent(45, 0); doKeyEvent(50, 0); return TRUE;

            case 0x6C:			// l
                doKeyEvent(46, 1); doKeyEvent(46, 0); return TRUE;
            case 0x4C:			// L
                doKeyEvent(50, 1); doKeyEvent(46, 1);
                doKeyEvent(46, 0); doKeyEvent(50, 0); return TRUE;
            case 0x6D:			// m
                doKeyEvent(58, 1); doKeyEvent(58, 0); return TRUE;
            case 0x4D:			// M
                doKeyEvent(50, 1); doKeyEvent(58, 1);
                doKeyEvent(58, 0); doKeyEvent(50, 0); return TRUE;

            case 0x6E:			// n
                doKeyEvent(57, 1); doKeyEvent(57, 0); return TRUE;
            case 0x4E:			// N
                doKeyEvent(50, 1); doKeyEvent(57, 1);
                doKeyEvent(57, 0); doKeyEvent(50, 0); return TRUE;

            case 0x6F:			// o
                doKeyEvent(32, 1); doKeyEvent(32, 0); return TRUE;
            case 0x4F:			// O
                doKeyEvent(50, 1); doKeyEvent(32, 1);
                doKeyEvent(32, 0); doKeyEvent(50, 0); return TRUE;

            case 0x70:			// p
                doKeyEvent(33, 1); doKeyEvent(33, 0); return TRUE;
            case 0x50:			// P
                doKeyEvent(50, 1); doKeyEvent(33, 1);
                doKeyEvent(33, 0); doKeyEvent(50, 0); return TRUE;

            case 0x71:			// q
                doKeyEvent(24, 1); doKeyEvent(24, 0); return TRUE;
            case 0x51:			// Q
                doKeyEvent(50, 1); doKeyEvent(24, 1);
                doKeyEvent(24, 0); doKeyEvent(50, 0); return TRUE;

            case 0x72:			// r
                doKeyEvent(27, 1); doKeyEvent(27, 0); return TRUE;
            case 0x52:			// R
                doKeyEvent(50, 1); doKeyEvent(27, 1);
                doKeyEvent(27, 0); doKeyEvent(50, 0); return TRUE;

            case 0x73:			// s
                doKeyEvent(39, 1); doKeyEvent(39, 0); return TRUE;
            case 0x53:			// S
                doKeyEvent(50, 1); doKeyEvent(39, 1);
                doKeyEvent(39, 0); doKeyEvent(50, 0); return TRUE;

            case 0x74:			// t
                doKeyEvent(28, 1); doKeyEvent(28, 0); return TRUE;
            case 0x54:			// T
                doKeyEvent(50, 1); doKeyEvent(28, 1);
                doKeyEvent(28, 0); doKeyEvent(50, 0); return TRUE;

            case 0x75:			// u
                doKeyEvent(30, 1); doKeyEvent(30, 0); return TRUE;
            case 0x55:			// U
                doKeyEvent(50, 1); doKeyEvent(30, 1);
                doKeyEvent(30, 0); doKeyEvent(50, 0); return TRUE;

            case 0x76:			// v
                doKeyEvent(55, 1); doKeyEvent(55, 0); return TRUE;
            case 0x56:			// V
                doKeyEvent(50, 1); doKeyEvent(55, 1);
                doKeyEvent(55, 0); doKeyEvent(50, 0); return TRUE;

            case 0x77:			// w
                doKeyEvent(25, 1); doKeyEvent(25, 0); return TRUE;
            case 0x57:			// W
                doKeyEvent(50, 1); doKeyEvent(25, 1);
                doKeyEvent(25, 0); doKeyEvent(50, 0); return TRUE;

            case 0x78:			// x
                doKeyEvent(53, 1); doKeyEvent(53, 0); return TRUE;
            case 0x58:			// X
                doKeyEvent(50, 1); doKeyEvent(53, 1);
                doKeyEvent(53, 0); doKeyEvent(50, 0); return TRUE;

            case 0x79:			// y
                doKeyEvent(29, 1); doKeyEvent(29, 0); return TRUE;
            case 0x59:			// Y
                doKeyEvent(50, 1); doKeyEvent(29, 1);
                doKeyEvent(29, 0); doKeyEvent(50, 0); return TRUE;

            case 0x7A:			// z
                doKeyEvent(52, 1); doKeyEvent(52, 0); return TRUE;
            case 0x5A:			// Z
                doKeyEvent(50, 1); doKeyEvent(52, 1);
                doKeyEvent(52, 0); doKeyEvent(50, 0); return TRUE;
        }

    }
    return FALSE;
}
#else
gboolean FakeAlphabet(unsigned char key)
{
    key_note=key;
    SysGeneralSetting sysGS;
    int language= sysGS.GetLanguage();
    if(FR == language)
    {
        switch (key) {
            case 0x61:			// a
                doKeyEvent(38, 1); doKeyEvent(38, 0); return TRUE;

            case 0x41:			// A
            case 0xA1:          //shift+
                doKeyEvent(50, 1); doKeyEvent(38, 1);
                doKeyEvent(38, 0); doKeyEvent(50, 0); return TRUE;

            case 0x62:			// b
                doKeyEvent(56, 1); doKeyEvent(56, 0); return TRUE;
            case 0x42:			// B
            case 0xA2:          //shift+
                doKeyEvent(50, 1); doKeyEvent(56, 1);
                doKeyEvent(56, 0); doKeyEvent(50, 0); return TRUE;

            case 0x63:			// c
                doKeyEvent(54, 1); doKeyEvent(54, 0); return TRUE;
            case 0x43:			// C
            case 0xA3:          //shift+
                doKeyEvent(50, 1); doKeyEvent(54, 1);
                doKeyEvent(54, 0); doKeyEvent(50, 0); return TRUE;

            case 0x64:			// d
                doKeyEvent(40, 1); doKeyEvent(40, 0); return TRUE;
            case 0x44:			// D
            case 0xA4:          //shift+
                doKeyEvent(50, 1); doKeyEvent(40, 1);
                doKeyEvent(40, 0); doKeyEvent(50, 0); return TRUE;

            case 0x65:			// e
                doKeyEvent(26, 1); doKeyEvent(26, 0); return TRUE;
            case 0x45:			// E
            case 0xA5:          //shift+
                doKeyEvent(50, 1); doKeyEvent(26, 1);
                doKeyEvent(26, 0); doKeyEvent(50, 0); return TRUE;

            case 0x66:			// f
                doKeyEvent(41, 1); doKeyEvent(41, 0); return TRUE;
            case 0x46:			// F
            case 0xA6:          //shift+
                doKeyEvent(50, 1); doKeyEvent(41, 1);
                doKeyEvent(41, 0); doKeyEvent(50, 0); return TRUE;

            case 0x67:			// g
                doKeyEvent(42, 1); doKeyEvent(42, 0); return TRUE;
            case 0x47:			// G
            case 0xA7:          //shift+
                doKeyEvent(50, 1); doKeyEvent(42, 1);
                doKeyEvent(42, 0); doKeyEvent(50, 0); return TRUE;

            case 0x68:			// h
                doKeyEvent(43, 1); doKeyEvent(43, 0); return TRUE;
            case 0x48:			// H
            case 0xA8:          //shift+
                doKeyEvent(50, 1); doKeyEvent(43, 1);
                doKeyEvent(43, 0); doKeyEvent(50, 0); return TRUE;

            case 0x69:			// i
                doKeyEvent(31, 1); doKeyEvent(31, 0); return TRUE;
            case 0x49:			// I
            case 0xB0:          //shift+
                doKeyEvent(50, 1); doKeyEvent(31, 1);
                doKeyEvent(31, 0); doKeyEvent(50, 0); return TRUE;

            case 0x6A:			// j
                doKeyEvent(44, 1); doKeyEvent(44, 0); return TRUE;
            case 0x4A:			// J
            case 0xB1:          //shift+
                doKeyEvent(50, 1); doKeyEvent(44, 1);
                doKeyEvent(44, 0); doKeyEvent(50, 0); return TRUE;

            case 0x6B:			// k
                doKeyEvent(45, 1); doKeyEvent(45, 0); return TRUE;
            case 0x4B:			// K
            case 0xB2:          //shift+
                doKeyEvent(50, 1); doKeyEvent(45, 1);
                doKeyEvent(45, 0); doKeyEvent(50, 0); return TRUE;

            case 0x6C:			// l
                doKeyEvent(46, 1); doKeyEvent(46, 0); return TRUE;
            case 0x4C:			// L
            case 0xB4:          //shift+
                doKeyEvent(50, 1); doKeyEvent(46, 1);
                doKeyEvent(46, 0); doKeyEvent(50, 0); return TRUE;
            case 0x4D:			// M
            case 0x6D:			// m
                doKeyEvent(58, 1); doKeyEvent(58, 0); return TRUE;
                //  case 0x4D:			// M
            case 0xB5:          //shift+
                doKeyEvent(50, 1); doKeyEvent(58, 1);
                doKeyEvent(58, 0); doKeyEvent(50, 0); return TRUE;

            case 0x6E:			// n
                doKeyEvent(57, 1); doKeyEvent(57, 0); return TRUE;
            case 0x4E:			// N
            case 0xB7:          //shift+
                doKeyEvent(50, 1); doKeyEvent(57, 1);
                doKeyEvent(57, 0); doKeyEvent(50, 0); return TRUE;

            case 0x6F:			// o
                doKeyEvent(32, 1); doKeyEvent(32, 0); return TRUE;
            case 0x4F:			// O
                doKeyEvent(50, 1); doKeyEvent(32, 1);
                doKeyEvent(32, 0); doKeyEvent(50, 0); return TRUE;

            case 0x70:			// p
                doKeyEvent(33, 1); doKeyEvent(33, 0); return TRUE;
            case 0x50:			// P
                doKeyEvent(50, 1); doKeyEvent(33, 1);
                doKeyEvent(33, 0); doKeyEvent(50, 0); return TRUE;

            case 0x71:			// q
                doKeyEvent(24, 1); doKeyEvent(24, 0); return TRUE;
            case 0x51:			// Q
            case 0xC1:          //shift+
                doKeyEvent(50, 1); doKeyEvent(24, 1);
                doKeyEvent(24, 0); doKeyEvent(50, 0); return TRUE;

            case 0x72:			// r
                doKeyEvent(27, 1); doKeyEvent(27, 0); return TRUE;
            case 0x52:			// R
            case 0xC2:          //shift+
                doKeyEvent(50, 1); doKeyEvent(27, 1);
                doKeyEvent(27, 0); doKeyEvent(50, 0); return TRUE;

            case 0x73:			// s
                doKeyEvent(39, 1); doKeyEvent(39, 0); return TRUE;
            case 0x53:			// S
            case 0xC3:          //shift+
                doKeyEvent(50, 1); doKeyEvent(39, 1);
                doKeyEvent(39, 0); doKeyEvent(50, 0); return TRUE;

            case 0x74:			// t
                doKeyEvent(28, 1); doKeyEvent(28, 0); return TRUE;
            case 0x54:			// T
            case 0xC4:          //shift+
                doKeyEvent(50, 1); doKeyEvent(28, 1);
                doKeyEvent(28, 0); doKeyEvent(50, 0); return TRUE;

            case 0x75:			// u
                doKeyEvent(30, 1); doKeyEvent(30, 0); return TRUE;
            case 0x55:			// U
            case 0xC5:          //shift+
                doKeyEvent(50, 1); doKeyEvent(30, 1);
                doKeyEvent(30, 0); doKeyEvent(50, 0); return TRUE;

            case 0x76:			// v
                doKeyEvent(55, 1); doKeyEvent(55, 0); return TRUE;
            case 0x56:			// V
            case 0xC6:          //shift+
                doKeyEvent(50, 1); doKeyEvent(55, 1);
                doKeyEvent(55, 0); doKeyEvent(50, 0); return TRUE;

            case 0x77:			// w
                doKeyEvent(25, 1); doKeyEvent(25, 0); return TRUE;
            case 0x57:			// W
            case 0xC7:          //shift+
                doKeyEvent(50, 1); doKeyEvent(25, 1);
                doKeyEvent(25, 0); doKeyEvent(50, 0); return TRUE;

            case 0x78:			// x
                doKeyEvent(53, 1); doKeyEvent(53, 0); return TRUE;
            case 0x58:			// X
            case 0xC8:          //shift+
                doKeyEvent(50, 1); doKeyEvent(53, 1);
                doKeyEvent(53, 0); doKeyEvent(50, 0); return TRUE;

            case 0x79:			// y
                doKeyEvent(29, 1); doKeyEvent(29, 0); return TRUE;
            case 0x59:			// Y
            case 0xC9:          //shift+
                doKeyEvent(50, 1); doKeyEvent(29, 1);
                doKeyEvent(29, 0); doKeyEvent(50, 0); return TRUE;

            case 0x7A:			// z
                doKeyEvent(52, 1); doKeyEvent(52, 0); return TRUE;
            case 0x5A:			// Z
            case 0xCA:          //shift+
                doKeyEvent(50, 1); doKeyEvent(52, 1);
                doKeyEvent(52, 0); doKeyEvent(50, 0); return TRUE;
        }
    }
    else
    {

        switch (key) {
            case 0x61:			// a
                doKeyEvent(38, 1); doKeyEvent(38, 0); return TRUE;

            case 0x41:			// A
            case 0xA1:          //shift+
                doKeyEvent(50, 1); doKeyEvent(38, 1);
                doKeyEvent(38, 0); doKeyEvent(50, 0); return TRUE;

            case 0x62:			// b
                doKeyEvent(56, 1); doKeyEvent(56, 0); return TRUE;
            case 0x42:			// B
            case 0xA2:          //shift+
                doKeyEvent(50, 1); doKeyEvent(56, 1);
                doKeyEvent(56, 0); doKeyEvent(50, 0); return TRUE;

            case 0x63:			// c
                doKeyEvent(54, 1); doKeyEvent(54, 0); return TRUE;
            case 0x43:			// C
            case 0xA3:          //shift+
                doKeyEvent(50, 1); doKeyEvent(54, 1);
                doKeyEvent(54, 0); doKeyEvent(50, 0); return TRUE;

            case 0x64:			// d
                doKeyEvent(40, 1); doKeyEvent(40, 0); return TRUE;
            case 0x44:			// D
            case 0xA4:          //shift+
                doKeyEvent(50, 1); doKeyEvent(40, 1);
                doKeyEvent(40, 0); doKeyEvent(50, 0); return TRUE;

            case 0x65:			// e
                doKeyEvent(26, 1); doKeyEvent(26, 0); return TRUE;
            case 0x45:			// E
            case 0xA5:          //shift+
                doKeyEvent(50, 1); doKeyEvent(26, 1);
                doKeyEvent(26, 0); doKeyEvent(50, 0); return TRUE;

            case 0x66:			// f
                doKeyEvent(41, 1); doKeyEvent(41, 0); return TRUE;
            case 0x46:			// F
            case 0xA6:          //shift+
                doKeyEvent(50, 1); doKeyEvent(41, 1);
                doKeyEvent(41, 0); doKeyEvent(50, 0); return TRUE;

            case 0x67:			// g
                doKeyEvent(42, 1); doKeyEvent(42, 0); return TRUE;
            case 0x47:			// G
            case 0xA7:          //shift+
                doKeyEvent(50, 1); doKeyEvent(42, 1);
                doKeyEvent(42, 0); doKeyEvent(50, 0); return TRUE;

            case 0x68:			// h
                doKeyEvent(43, 1); doKeyEvent(43, 0); return TRUE;
            case 0x48:			// H
            case 0xA8:          //shift+
                doKeyEvent(50, 1); doKeyEvent(43, 1);
                doKeyEvent(43, 0); doKeyEvent(50, 0); return TRUE;

            case 0x69:			// i
                doKeyEvent(31, 1); doKeyEvent(31, 0); return TRUE;
            case 0x49:			// I
            case 0xB0:          //shift+
                doKeyEvent(50, 1); doKeyEvent(31, 1);
                doKeyEvent(31, 0); doKeyEvent(50, 0); return TRUE;

            case 0x6A:			// j
                doKeyEvent(44, 1); doKeyEvent(44, 0); return TRUE;
            case 0x4A:			// J
            case 0xB1:          //shift+
                doKeyEvent(50, 1); doKeyEvent(44, 1);
                doKeyEvent(44, 0); doKeyEvent(50, 0); return TRUE;

            case 0x6B:			// k
                doKeyEvent(45, 1); doKeyEvent(45, 0); return TRUE;
            case 0x4B:			// K
            case 0xB2:          //shift+
                doKeyEvent(50, 1); doKeyEvent(45, 1);
                doKeyEvent(45, 0); doKeyEvent(50, 0); return TRUE;

            case 0x6C:			// l
                doKeyEvent(46, 1); doKeyEvent(46, 0); return TRUE;
            case 0x4C:			// L
            case 0xB4:          //shift+
                doKeyEvent(50, 1); doKeyEvent(46, 1);
                doKeyEvent(46, 0); doKeyEvent(50, 0); return TRUE;
            case 0x6D:			// m
                doKeyEvent(58, 1); doKeyEvent(58, 0); return TRUE;
            case 0x4D:			// M
            case 0xB5:          //shift+
                doKeyEvent(50, 1); doKeyEvent(58, 1);
                doKeyEvent(58, 0); doKeyEvent(50, 0); return TRUE;

            case 0x6E:			// n
                doKeyEvent(57, 1); doKeyEvent(57, 0); return TRUE;
            case 0x4E:			// N
            case 0xB7:          //shift+
                doKeyEvent(50, 1); doKeyEvent(57, 1);
                doKeyEvent(57, 0); doKeyEvent(50, 0); return TRUE;

            case 0x6F:			// o
                doKeyEvent(32, 1); doKeyEvent(32, 0); return TRUE;
            case 0x4F:			// O
                doKeyEvent(50, 1); doKeyEvent(32, 1);
                doKeyEvent(32, 0); doKeyEvent(50, 0); return TRUE;

            case 0x70:			// p
                doKeyEvent(33, 1); doKeyEvent(33, 0); return TRUE;
            case 0x50:			// P
                doKeyEvent(50, 1); doKeyEvent(33, 1);
                doKeyEvent(33, 0); doKeyEvent(50, 0); return TRUE;

            case 0x71:			// q
                doKeyEvent(24, 1); doKeyEvent(24, 0); return TRUE;
            case 0x51:			// Q
            case 0xC1:          //shift+
                doKeyEvent(50, 1); doKeyEvent(24, 1);
                doKeyEvent(24, 0); doKeyEvent(50, 0); return TRUE;

            case 0x72:			// r
                doKeyEvent(27, 1); doKeyEvent(27, 0); return TRUE;
            case 0x52:			// R
            case 0xC2:          //shift+
                doKeyEvent(50, 1); doKeyEvent(27, 1);
                doKeyEvent(27, 0); doKeyEvent(50, 0); return TRUE;

            case 0x73:			// s
                doKeyEvent(39, 1); doKeyEvent(39, 0); return TRUE;
            case 0x53:			// S
            case 0xC3:          //shift+
                doKeyEvent(50, 1); doKeyEvent(39, 1);
                doKeyEvent(39, 0); doKeyEvent(50, 0); return TRUE;

            case 0x74:			// t
                doKeyEvent(28, 1); doKeyEvent(28, 0); return TRUE;
            case 0x54:			// T
            case 0xC4:          //shift+
                doKeyEvent(50, 1); doKeyEvent(28, 1);
                doKeyEvent(28, 0); doKeyEvent(50, 0); return TRUE;

            case 0x75:			// u
                doKeyEvent(30, 1); doKeyEvent(30, 0); return TRUE;
            case 0x55:			// U
            case 0xC5:          //shift+
                doKeyEvent(50, 1); doKeyEvent(30, 1);
                doKeyEvent(30, 0); doKeyEvent(50, 0); return TRUE;

            case 0x76:			// v
                doKeyEvent(55, 1); doKeyEvent(55, 0); return TRUE;
            case 0x56:			// V
            case 0xC6:          //shift+
                doKeyEvent(50, 1); doKeyEvent(55, 1);
                doKeyEvent(55, 0); doKeyEvent(50, 0); return TRUE;

            case 0x77:			// w
                doKeyEvent(25, 1); doKeyEvent(25, 0); return TRUE;
            case 0x57:			// W
            case 0xC7:          //shift+
                doKeyEvent(50, 1); doKeyEvent(25, 1);
                doKeyEvent(25, 0); doKeyEvent(50, 0); return TRUE;

            case 0x78:			// x
                doKeyEvent(53, 1); doKeyEvent(53, 0); return TRUE;
            case 0x58:			// X
            case 0xC8:          //shift+
                doKeyEvent(50, 1); doKeyEvent(53, 1);
                doKeyEvent(53, 0); doKeyEvent(50, 0); return TRUE;

            case 0x79:			// y
                doKeyEvent(29, 1); doKeyEvent(29, 0); return TRUE;
            case 0x59:			// Y
            case 0xC9:          //shift+
                doKeyEvent(50, 1); doKeyEvent(29, 1);
                doKeyEvent(29, 0); doKeyEvent(50, 0); return TRUE;

            case 0x7A:			// z
                doKeyEvent(52, 1); doKeyEvent(52, 0); return TRUE;
            case 0x5A:			// Z
            case 0xCA:          //shift+
                doKeyEvent(50, 1); doKeyEvent(52, 1);
                doKeyEvent(52, 0); doKeyEvent(50, 0); return TRUE;
        }

    }
    return FALSE;
}
#endif

gboolean FakeMouseButton(unsigned char key)
{
    switch (key) {
        case KEY_SET:		//left button
            doBtnEvent(1, 1);
            doBtnEvent(1, 0);
            return TRUE;
    }
    return FALSE;
}

gboolean FakePunctuation(unsigned char key)
{
    printf("Falc key = 0x%x", key);
    key_note=key;
    SysGeneralSetting sysGS;
    int language= sysGS.GetLanguage();
    if((FR == language) ||(ES == language)||(PL == language))
    {
        switch (key) {
            case 0x21:			// !
                doKeyEvent(50, 1); doKeyEvent(10, 1);
                doKeyEvent(10, 0); doKeyEvent(50, 0); return TRUE;
            case 0x40:			// @
                doKeyEvent(50, 1); doKeyEvent(11, 1);
                doKeyEvent(11, 0); doKeyEvent(50, 0); return TRUE;
            case 0x23:			// #
                doKeyEvent(50, 1); doKeyEvent(12, 1);
                doKeyEvent(12, 0); doKeyEvent(50, 0); return TRUE;
            case 0x24:			// $
                doKeyEvent(50, 1); doKeyEvent(13, 1);
                doKeyEvent(13, 0); doKeyEvent(50, 0); return TRUE;
            case 0x25:		    // %
                doKeyEvent(50, 1); doKeyEvent(14, 1);
                doKeyEvent(14, 0); doKeyEvent(50, 0); return TRUE;
            case 0x5E:			// ^
                doKeyEvent(50, 1); doKeyEvent(15, 1);
                doKeyEvent(15, 0); doKeyEvent(50, 0); return TRUE;
            case 0x26:			// &
                doKeyEvent(50, 1); doKeyEvent(16, 1);
                doKeyEvent(16, 0); doKeyEvent(50, 0); return TRUE;
            case 0x2A:			// *
                doKeyEvent(50, 1); doKeyEvent(17, 1);
                doKeyEvent(17, 0); doKeyEvent(50, 0); return TRUE;
            case 0x28:			// (
                doKeyEvent(50, 1); doKeyEvent(18, 1);
                doKeyEvent(18, 0); doKeyEvent(50, 0); return TRUE;
            case 0x29:			// )
                doKeyEvent(50, 1); doKeyEvent(19, 1);
                doKeyEvent(19, 0); doKeyEvent(50, 0); return TRUE;
            case 0x2C:			// ,
            case 0xF7:          // , caps + syb
                doKeyEvent(59, 1); doKeyEvent(59, 0); return TRUE;
            case 0x2E:			// .
            case 0xF8:			// . caps + syb
                doKeyEvent(60, 1); doKeyEvent(60, 0); return TRUE;
            case 0x2D:			// -
            case 0xF0:			// - caps + syb
                doKeyEvent(20, 1); doKeyEvent(20, 0); return TRUE;
            case 0x3D:			// =
            case 0xF1:			// = caps + syb
                doKeyEvent(21, 1); doKeyEvent(21, 0); return TRUE;
            case 0x5C:		    // "\"
            case 0xF4:			// "\" caps + syb
                doKeyEvent(51, 1); doKeyEvent(51, 0); return TRUE;
            case 0x5B:			// [
            case 0xF2:			// [ caps + syb
                doKeyEvent(34, 1); doKeyEvent(34, 0); return TRUE;
            case 0x5D:			// ]
            case 0xF3:			// ] caps + syb
                doKeyEvent(35, 1); doKeyEvent(35, 0); return TRUE;
            case 0x3B:			// ;
                doKeyEvent(47, 1); doKeyEvent(47, 0); return TRUE;
            case 0x27:			// '单引号
            case 0xF6:			// '单引号 caps + syb
                doKeyEvent(48, 1); doKeyEvent(48, 0); return TRUE;
            case 0x2F:			// /
            case 0xF9:			// / caps + syb
                doKeyEvent(61, 1); doKeyEvent(61, 0); return TRUE;
            case 0x60:			// `ESC上标点
            case 0xFA:			// `ESC上标点 caps + syb
                doKeyEvent(49, 1); doKeyEvent(49, 0); return TRUE;
            case 0x3C:			// <
                doKeyEvent(50, 1); doKeyEvent(59, 1);
                doKeyEvent(59, 0); doKeyEvent(50, 0); return TRUE;
            case 0x3E:			// >
                doKeyEvent(50, 1); doKeyEvent(60, 1);
                doKeyEvent(60, 0); doKeyEvent(50, 0); return TRUE;
            case 0x5F:			// _
                doKeyEvent(50, 1); doKeyEvent(20, 1);
                doKeyEvent(20, 0); doKeyEvent(50, 0); return TRUE;
            case 0x2B:		    // +
                doKeyEvent(50, 1); doKeyEvent(21, 1);
                doKeyEvent(21, 0); doKeyEvent(50, 0); return TRUE;
            case 0x7C:			// |
                doKeyEvent(50, 1); doKeyEvent(51, 1);
                doKeyEvent(51, 0); doKeyEvent(50, 0); return TRUE;
            case 0x7B:			// {
                doKeyEvent(50, 1); doKeyEvent(34, 1);
                doKeyEvent(34, 0); doKeyEvent(50, 0); return TRUE;
            case 0x7D:			// }
                doKeyEvent(50, 1); doKeyEvent(35, 1);
                doKeyEvent(35, 0); doKeyEvent(50, 0); return TRUE;
            case 0x3A:			// :
            case 0xF5:			// : caps + syb
                doKeyEvent(50, 1); doKeyEvent(47, 1);
                doKeyEvent(47, 0); doKeyEvent(50, 0); return TRUE;
            case 0x22:			// "
                doKeyEvent(50, 1); doKeyEvent(48, 1);
                doKeyEvent(48, 0); doKeyEvent(50, 0); return TRUE;
            case 0x3F:			// ?
                doKeyEvent(50, 1); doKeyEvent(61, 1);
                doKeyEvent(61, 0); doKeyEvent(50, 0); return TRUE;
            case 0x7E:			// ~
                doKeyEvent(50, 1); doKeyEvent(49, 1);
                doKeyEvent(49, 0); doKeyEvent(50, 0); return TRUE;

                // Add func key
            case 0x08:			// BackSpace
                doKeyEvent(22, 1); doKeyEvent(22, 0); return TRUE;
            case 0xA0:			// Delete
                doKeyEvent(119, 1); doKeyEvent(119, 0); return TRUE;
            case 0x20:			// space
                doKeyEvent(65, 1); doKeyEvent(65, 0); return TRUE;
            case 0x0D:			// enter
                doKeyEvent(36, 1); doKeyEvent(36, 0); return TRUE;
            case 0x09:			// tab
                doKeyEvent(23, 1); doKeyEvent(23, 0); return TRUE;
            case 0x18:			// up
                doKeyEvent(111, 1); doKeyEvent(111, 0); return TRUE;
            case 0x19:			// down
                doKeyEvent(116, 1); doKeyEvent(116, 0); return TRUE;
            case 0x1A:			// right
                doKeyEvent(114, 1); doKeyEvent(114, 0); return TRUE;
            case 0x1B:			// left
                doKeyEvent(113, 1); doKeyEvent(113, 0); return TRUE;
            case 0xAD:			// malesymbol
                doKeysymEvent(GDK_malesymbol, 1); doKeysymEvent(GDK_malesymbol, 0); return TRUE;
            case 0xAC:			// femalesymbol
                doKeysymEvent(GDK_femalesymbol, 1); doKeysymEvent(GDK_femalesymbol, 0); return TRUE;
#if defined(EMP_313)
            case 0xE7:			// page up
                doKeyEvent(112, 1); doKeyEvent(112, 0); return TRUE;
            case 0xE0:			// page down
                doKeyEvent(117, 1); doKeyEvent(117, 0); return TRUE;
#else
            case 0xE0:			// page up
                doKeyEvent(112, 1); doKeyEvent(112, 0); return TRUE;
            case 0xE7:			// page down
                doKeyEvent(117, 1); doKeyEvent(117, 0); return TRUE;
#endif
        }
    }
    else if(DE == language)
    {

 switch (key) {
            case 0x21:			// !
                doKeyEvent(50, 1); doKeyEvent(10, 1);
                doKeyEvent(10, 0); doKeyEvent(50, 0); return TRUE;
            case 0x40:			// @
                doKeyEvent(50, 1); doKeyEvent(11, 1);
                doKeyEvent(11, 0); doKeyEvent(50, 0); return TRUE;
            case 0x23:			// #
                doKeyEvent(50, 1); doKeyEvent(12, 1);
                doKeyEvent(12, 0); doKeyEvent(50, 0); return TRUE;
            case 0x24:			// $
                doKeyEvent(50, 1); doKeyEvent(13, 1);
                doKeyEvent(13, 0); doKeyEvent(50, 0); return TRUE;
            case 0x25:		    // %
                doKeyEvent(50, 1); doKeyEvent(14, 1);
                doKeyEvent(14, 0); doKeyEvent(50, 0); return TRUE;
            case 0x5E:			// ^
                doKeyEvent(50, 1); doKeyEvent(15, 1);
                doKeyEvent(15, 0); doKeyEvent(50, 0); return TRUE;
            case 0x26:			// &
                doKeyEvent(50, 1); doKeyEvent(16, 1);
                doKeyEvent(16, 0); doKeyEvent(50, 0); return TRUE;
            case 0x2A:			// *
                doKeyEvent(50, 1); doKeyEvent(17, 1);
                doKeyEvent(17, 0); doKeyEvent(50, 0); return TRUE;
            case 0x28:			// (
                doKeyEvent(50, 1); doKeyEvent(18, 1);
                doKeyEvent(18, 0); doKeyEvent(50, 0); return TRUE;
            case 0x29:			// )
                doKeyEvent(50, 1); doKeyEvent(19, 1);
                doKeyEvent(19, 0); doKeyEvent(50, 0); return TRUE;
            case 0x2C:			// ,
            case 0xF7:          // , caps + syb
                doKeyEvent(59, 1); doKeyEvent(59, 0); return TRUE;
            case 0x2E:			// .
            case 0xF8:			// . caps + syb
                doKeyEvent(60, 1); doKeyEvent(60, 0); return TRUE;
            case 0x2D:			// -
            case 0xF0:			// - caps + syb
                doKeyEvent(20, 1); doKeyEvent(20, 0); return TRUE;
            case 0x3D:			// =
            case 0xF1:			// = caps + syb
                doKeyEvent(21, 1); doKeyEvent(21, 0); return TRUE;
            case 0x5C:		    // "\"
            case 0xF4:			// "\" caps + syb
                doKeyEvent(51, 1); doKeyEvent(51, 0); return TRUE;
            case 0x5B:			// [
            //case 0xF2:			// [ caps + syb
                doKeyEvent(34, 1); doKeyEvent(34, 0); return TRUE;
            case 0x5D:			// ]
            case 0xF3:			// ] caps + syb
                doKeyEvent(35, 1); doKeyEvent(35, 0); return TRUE;
            case 0x3B:			// ;
           // case 0xF5:			// : caps + syb
                doKeyEvent(47, 1); doKeyEvent(47, 0); return TRUE;
            case 0x27:			// '单引号
           // case 0xF6:			// '单引号 caps + syb
                doKeyEvent(48, 1); doKeyEvent(48, 0); return TRUE;
            case 0x2F:			// /
            case 0xF9:			// / caps + syb
                doKeyEvent(61, 1); doKeyEvent(61, 0); return TRUE;
            case 0x60:			// `ESC上标点
            case 0xFA:			// `ESC上标点 caps + syb
                doKeyEvent(49, 1); doKeyEvent(49, 0); return TRUE;
            case 0x3C:			// <
                doKeyEvent(50, 1); doKeyEvent(59, 1);
                doKeyEvent(59, 0); doKeyEvent(50, 0); return TRUE;
            case 0x3E:			// >
                doKeyEvent(50, 1); doKeyEvent(60, 1);
                doKeyEvent(60, 0); doKeyEvent(50, 0); return TRUE;
            case 0x5F:			// _
                doKeyEvent(50, 1); doKeyEvent(20, 1);
                doKeyEvent(20, 0); doKeyEvent(50, 0); return TRUE;
            case 0x2B:		    // +
                doKeyEvent(50, 1); doKeyEvent(21, 1);
                doKeyEvent(21, 0); doKeyEvent(50, 0); return TRUE;
            case 0x7C:			// |
                doKeyEvent(50, 1); doKeyEvent(51, 1);
                doKeyEvent(51, 0); doKeyEvent(50, 0); return TRUE;
            case 0x7B:			// {
            case 0xF2:			// [ caps + syb
                doKeyEvent(50, 1); doKeyEvent(34, 1);
                doKeyEvent(34, 0); doKeyEvent(50, 0); return TRUE;
            case 0x7D:			// }
                doKeyEvent(50, 1); doKeyEvent(35, 1);
                doKeyEvent(35, 0); doKeyEvent(50, 0); return TRUE;
            case 0x3A:			// :
            case 0xF5:
                doKeyEvent(50, 1); doKeyEvent(47, 1);
                doKeyEvent(47, 0); doKeyEvent(50, 0); return TRUE;
            case 0x22:			// "
            case 0xF6:			// '单引号 caps + syb
                doKeyEvent(50, 1); doKeyEvent(48, 1);
                doKeyEvent(48, 0); doKeyEvent(50, 0); return TRUE;
            case 0x3F:			// ?
                doKeyEvent(50, 1); doKeyEvent(61, 1);
                doKeyEvent(61, 0); doKeyEvent(50, 0); return TRUE;
            case 0x7E:			// ~
                doKeyEvent(50, 1); doKeyEvent(49, 1);
                doKeyEvent(49, 0); doKeyEvent(50, 0); return TRUE;

                // Add func key
            case 0x08:			// BackSpace
                doKeyEvent(22, 1); doKeyEvent(22, 0); return TRUE;
            case 0xA0:			// Delete
                doKeyEvent(119, 1); doKeyEvent(119, 0); return TRUE;
            case 0x20:			// space
                doKeyEvent(65, 1); doKeyEvent(65, 0); return TRUE;
            case 0x0D:			// enter
                doKeyEvent(36, 1); doKeyEvent(36, 0); return TRUE;
            case 0x09:			// tab
                doKeyEvent(23, 1); doKeyEvent(23, 0); return TRUE;
            case 0x18:			// up
                doKeyEvent(111, 1); doKeyEvent(111, 0); return TRUE;
            case 0x19:			// down
                doKeyEvent(116, 1); doKeyEvent(116, 0); return TRUE;
            case 0x1A:			// right
                doKeyEvent(114, 1); doKeyEvent(114, 0); return TRUE;
            case 0x1B:			// left
                doKeyEvent(113, 1); doKeyEvent(113, 0); return TRUE;
            case 0xAD:			// malesymbol
                doKeysymEvent(GDK_malesymbol, 1); doKeysymEvent(GDK_malesymbol, 0); return TRUE;
            case 0xAC:			// femalesymbol
                doKeysymEvent(GDK_femalesymbol, 1); doKeysymEvent(GDK_femalesymbol, 0); return TRUE;
#if defined(EMP_313)
            case 0xE7:			// page up
                doKeyEvent(112, 1); doKeyEvent(112, 0); return TRUE;
            case 0xE0:			// page down
                doKeyEvent(117, 1); doKeyEvent(117, 0); return TRUE;
#else
            case 0xE0:			// page up
                doKeyEvent(112, 1); doKeyEvent(112, 0); return TRUE;
            case 0xE7:			// page down
                doKeyEvent(117, 1); doKeyEvent(117, 0); return TRUE;
#endif
        }

    }
    else if(RU == language)
    {

 switch (key) {
            case 0x21:			// !
                doKeyEvent(50, 1); doKeyEvent(10, 1);
                doKeyEvent(10, 0); doKeyEvent(50, 0); return TRUE;
            case 0x40:			// @
                doKeyEvent(50, 1); doKeyEvent(11, 1);
                doKeyEvent(11, 0); doKeyEvent(50, 0); return TRUE;
            case 0x23:			// #
                doKeyEvent(50, 1); doKeyEvent(12, 1);
                doKeyEvent(12, 0); doKeyEvent(50, 0); return TRUE;
            case 0x24:			// $
                doKeyEvent(50, 1); doKeyEvent(13, 1);
                doKeyEvent(13, 0); doKeyEvent(50, 0); return TRUE;
            case 0x25:		    // %
                doKeyEvent(50, 1); doKeyEvent(14, 1);
                doKeyEvent(14, 0); doKeyEvent(50, 0); return TRUE;
            case 0x5E:			// ^
                doKeyEvent(50, 1); doKeyEvent(15, 1);
                doKeyEvent(15, 0); doKeyEvent(50, 0); return TRUE;
            case 0x26:			// &
                doKeyEvent(50, 1); doKeyEvent(16, 1);
                doKeyEvent(16, 0); doKeyEvent(50, 0); return TRUE;
            case 0x2A:			// *
                doKeyEvent(50, 1); doKeyEvent(17, 1);
                doKeyEvent(17, 0); doKeyEvent(50, 0); return TRUE;
            case 0x28:			// (
                doKeyEvent(50, 1); doKeyEvent(18, 1);
                doKeyEvent(18, 0); doKeyEvent(50, 0); return TRUE;
            case 0x29:			// )
                doKeyEvent(50, 1); doKeyEvent(19, 1);
                doKeyEvent(19, 0); doKeyEvent(50, 0); return TRUE;
            case 0x2C:			// ,
           // case 0xF7:          // , caps + syb
                doKeyEvent(59, 1); doKeyEvent(59, 0); return TRUE;
            case 0x2E:			// .
            //case 0xF8:			// . caps + syb
                doKeyEvent(60, 1); doKeyEvent(60, 0); return TRUE;
            case 0x2D:			// -
            case 0xF0:			// - caps + syb
                doKeyEvent(20, 1); doKeyEvent(20, 0); return TRUE;
            case 0x3D:			// =
            case 0xF1:			// = caps + syb
                doKeyEvent(21, 1); doKeyEvent(21, 0); return TRUE;
            case 0x5C:		    // "\"
            case 0xF4:			// "\" caps + syb
                doKeyEvent(51, 1); doKeyEvent(51, 0); return TRUE;
            case 0x5B:			// [
          //  case 0xF2:			// [ caps + syb
                doKeyEvent(34, 1); doKeyEvent(34, 0); return TRUE;
            case 0x5D:			// ]
          //  case 0xF3:			// ] caps + syb
                doKeyEvent(35, 1); doKeyEvent(35, 0); return TRUE;
            case 0x3B:			// ;
          //  case 0xF5:			// : caps + syb
                doKeyEvent(47, 1); doKeyEvent(47, 0); return TRUE;
            case 0x27:			// '单引号
          //  case 0xF6:			// '单引号 caps + syb
                doKeyEvent(48, 1); doKeyEvent(48, 0); return TRUE;
            case 0x2F:			// /
            case 0xF9:			// / caps + syb
                doKeyEvent(61, 1); doKeyEvent(61, 0); return TRUE;
            case 0x60:			// `ESC上标点
            case 0xFA:			// `ESC上标点 caps + syb
                doKeyEvent(49, 1); doKeyEvent(49, 0); return TRUE;
            case 0x3C:			// <
            case 0xF7:          // , caps + syb
                doKeyEvent(50, 1); doKeyEvent(59, 1);
                doKeyEvent(59, 0); doKeyEvent(50, 0); return TRUE;
            case 0x3E:			// >
            case 0xF8:			// . caps + syb
                  doKeyEvent(50, 1); doKeyEvent(60, 1);
                doKeyEvent(60, 0); doKeyEvent(50, 0); return TRUE;
            case 0x5F:			// _
                doKeyEvent(50, 1); doKeyEvent(20, 1);
                doKeyEvent(20, 0); doKeyEvent(50, 0); return TRUE;
            case 0x2B:		    // +
                doKeyEvent(50, 1); doKeyEvent(21, 1);
                doKeyEvent(21, 0); doKeyEvent(50, 0); return TRUE;
            case 0x7C:			// |
                doKeyEvent(50, 1); doKeyEvent(51, 1);
                doKeyEvent(51, 0); doKeyEvent(50, 0); return TRUE;
            case 0x7B:			// {
            case 0xF2:			// [ caps + syb
                doKeyEvent(50, 1); doKeyEvent(34, 1);
                doKeyEvent(34, 0); doKeyEvent(50, 0); return TRUE;
            case 0x7D:			// }
             case 0xF3:			// ] caps + syb
                doKeyEvent(50, 1); doKeyEvent(35, 1);
                doKeyEvent(35, 0); doKeyEvent(50, 0); return TRUE;
            case 0x3A:			// :
              case 0xF5:			// : caps + syb
                doKeyEvent(50, 1); doKeyEvent(47, 1);
                doKeyEvent(47, 0); doKeyEvent(50, 0); return TRUE;
            case 0x22:			// "
            case 0xF6:			// '单引号 caps + syb
                doKeyEvent(50, 1); doKeyEvent(48, 1);
                doKeyEvent(48, 0); doKeyEvent(50, 0); return TRUE;
            case 0x3F:			// ?
                doKeyEvent(50, 1); doKeyEvent(61, 1);
                doKeyEvent(61, 0); doKeyEvent(50, 0); return TRUE;
            case 0x7E:			// ~
                doKeyEvent(50, 1); doKeyEvent(49, 1);
                doKeyEvent(49, 0); doKeyEvent(50, 0); return TRUE;

                // Add func key
            case 0x08:			// BackSpace
                doKeyEvent(22, 1); doKeyEvent(22, 0); return TRUE;
            case 0xA0:			// Delete
                doKeyEvent(119, 1); doKeyEvent(119, 0); return TRUE;
            case 0x20:			// space
                doKeyEvent(65, 1); doKeyEvent(65, 0); return TRUE;
            case 0x0D:			// enter
                doKeyEvent(36, 1); doKeyEvent(36, 0); return TRUE;
            case 0x09:			// tab
                doKeyEvent(23, 1); doKeyEvent(23, 0); return TRUE;
            case 0x18:			// up
                doKeyEvent(111, 1); doKeyEvent(111, 0); return TRUE;
            case 0x19:			// down
                doKeyEvent(116, 1); doKeyEvent(116, 0); return TRUE;
            case 0x1A:			// right
                doKeyEvent(114, 1); doKeyEvent(114, 0); return TRUE;
            case 0x1B:			// left
                doKeyEvent(113, 1); doKeyEvent(113, 0); return TRUE;
            case 0xAD:			// malesymbol
                doKeysymEvent(GDK_malesymbol, 1); doKeysymEvent(GDK_malesymbol, 0); return TRUE;
            case 0xAC:			// femalesymbol
                doKeysymEvent(GDK_femalesymbol, 1); doKeysymEvent(GDK_femalesymbol, 0); return TRUE;
#if defined(EMP_313)
            case 0xE7:			// page up
                doKeyEvent(112, 1); doKeyEvent(112, 0); return TRUE;
            case 0xE0:			// page down
                doKeyEvent(117, 1); doKeyEvent(117, 0); return TRUE;
#else
            case 0xE0:			// page up
                doKeyEvent(112, 1); doKeyEvent(112, 0); return TRUE;
            case 0xE7:			// page down
                doKeyEvent(117, 1); doKeyEvent(117, 0); return TRUE;
#endif
        }

    }
    else
    {

 switch (key) {
            case 0x21:			// !
                doKeyEvent(50, 1); doKeyEvent(10, 1);
                doKeyEvent(10, 0); doKeyEvent(50, 0); return TRUE;
            case 0x40:			// @
                doKeyEvent(50, 1); doKeyEvent(11, 1);
                doKeyEvent(11, 0); doKeyEvent(50, 0); return TRUE;
            case 0x23:			// #
                doKeyEvent(50, 1); doKeyEvent(12, 1);
                doKeyEvent(12, 0); doKeyEvent(50, 0); return TRUE;
            case 0x24:			// $
                doKeyEvent(50, 1); doKeyEvent(13, 1);
                doKeyEvent(13, 0); doKeyEvent(50, 0); return TRUE;
            case 0x25:		    // %
                doKeyEvent(50, 1); doKeyEvent(14, 1);
                doKeyEvent(14, 0); doKeyEvent(50, 0); return TRUE;
            case 0x5E:			// ^
                doKeyEvent(50, 1); doKeyEvent(15, 1);
                doKeyEvent(15, 0); doKeyEvent(50, 0); return TRUE;
            case 0x26:			// &
                doKeyEvent(50, 1); doKeyEvent(16, 1);
                doKeyEvent(16, 0); doKeyEvent(50, 0); return TRUE;
            case 0x2A:			// *
                doKeyEvent(50, 1); doKeyEvent(17, 1);
                doKeyEvent(17, 0); doKeyEvent(50, 0); return TRUE;
            case 0x28:			// (
                doKeyEvent(50, 1); doKeyEvent(18, 1);
                doKeyEvent(18, 0); doKeyEvent(50, 0); return TRUE;
            case 0x29:			// )
                doKeyEvent(50, 1); doKeyEvent(19, 1);
                doKeyEvent(19, 0); doKeyEvent(50, 0); return TRUE;
            case 0x2C:			// ,
            case 0xF7:          // , caps + syb
                doKeyEvent(59, 1); doKeyEvent(59, 0); return TRUE;
            case 0x2E:			// .
            case 0xF8:			// . caps + syb
                doKeyEvent(60, 1); doKeyEvent(60, 0); return TRUE;
            case 0x2D:			// -
            case 0xF0:			// - caps + syb
                doKeyEvent(20, 1); doKeyEvent(20, 0); return TRUE;
            case 0x3D:			// =
            case 0xF1:			// = caps + syb
                doKeyEvent(21, 1); doKeyEvent(21, 0); return TRUE;
            case 0x5C:		    // "\"
            case 0xF4:			// "\" caps + syb
                doKeyEvent(51, 1); doKeyEvent(51, 0); return TRUE;
            case 0x5B:			// [
            case 0xF2:			// [ caps + syb
                doKeyEvent(34, 1); doKeyEvent(34, 0); return TRUE;
            case 0x5D:			// ]
            case 0xF3:			// ] caps + syb
                doKeyEvent(35, 1); doKeyEvent(35, 0); return TRUE;
            case 0x3B:			// ;
            case 0xF5:			// ; caps + syb
                doKeyEvent(47, 1); doKeyEvent(47, 0); return TRUE;
            case 0x27:			// '单引号
            case 0xF6:			// '单引号 caps + syb
                doKeyEvent(48, 1); doKeyEvent(48, 0); return TRUE;
            case 0x2F:			// /
            case 0xF9:			// / caps + syb
                doKeyEvent(61, 1); doKeyEvent(61, 0); return TRUE;
            case 0x60:			// `ESC上标点
            case 0xFA:			// `ESC上标点 caps + syb
                doKeyEvent(49, 1); doKeyEvent(49, 0); return TRUE;
            case 0x3C:			// <
                doKeyEvent(50, 1); doKeyEvent(59, 1);
                doKeyEvent(59, 0); doKeyEvent(50, 0); return TRUE;
            case 0x3E:			// >
                doKeyEvent(50, 1); doKeyEvent(60, 1);
                doKeyEvent(60, 0); doKeyEvent(50, 0); return TRUE;
            case 0x5F:			// _
                doKeyEvent(50, 1); doKeyEvent(20, 1);
                doKeyEvent(20, 0); doKeyEvent(50, 0); return TRUE;
            case 0x2B:		    // +
                doKeyEvent(50, 1); doKeyEvent(21, 1);
                doKeyEvent(21, 0); doKeyEvent(50, 0); return TRUE;
            case 0x7C:			// |
                doKeyEvent(50, 1); doKeyEvent(51, 1);
                doKeyEvent(51, 0); doKeyEvent(50, 0); return TRUE;
            case 0x7B:			// {
                doKeyEvent(50, 1); doKeyEvent(34, 1);
                doKeyEvent(34, 0); doKeyEvent(50, 0); return TRUE;
            case 0x7D:			// }
                doKeyEvent(50, 1); doKeyEvent(35, 1);
                doKeyEvent(35, 0); doKeyEvent(50, 0); return TRUE;
            case 0x3A:			// :
                doKeyEvent(50, 1); doKeyEvent(47, 1);
                doKeyEvent(47, 0); doKeyEvent(50, 0); return TRUE;
            case 0x22:			// "
                doKeyEvent(50, 1); doKeyEvent(48, 1);
                doKeyEvent(48, 0); doKeyEvent(50, 0); return TRUE;
            case 0x3F:			// ?
                doKeyEvent(50, 1); doKeyEvent(61, 1);
                doKeyEvent(61, 0); doKeyEvent(50, 0); return TRUE;
            case 0x7E:			// ~
                doKeyEvent(50, 1); doKeyEvent(49, 1);
                doKeyEvent(49, 0); doKeyEvent(50, 0); return TRUE;

                // Add func key
            case 0x08:			// BackSpace
                doKeyEvent(22, 1); doKeyEvent(22, 0); return TRUE;
            case 0xA0:			// Delete
                doKeyEvent(119, 1); doKeyEvent(119, 0); return TRUE;
            case 0x20:			// space
                doKeyEvent(65, 1); doKeyEvent(65, 0); return TRUE;
            case 0x0D:			// enter
                doKeyEvent(36, 1); doKeyEvent(36, 0); return TRUE;
            case 0x09:			// tab
                doKeyEvent(23, 1); doKeyEvent(23, 0); return TRUE;
            case 0x18:			// up
                doKeyEvent(111, 1); doKeyEvent(111, 0); return TRUE;
            case 0x19:			// down
                doKeyEvent(116, 1); doKeyEvent(116, 0); return TRUE;
            case 0x1A:			// right
                doKeyEvent(114, 1); doKeyEvent(114, 0); return TRUE;
            case 0x1B:			// left
                doKeyEvent(113, 1); doKeyEvent(113, 0); return TRUE;
            case 0xAD:			// malesymbol
                doKeysymEvent(GDK_malesymbol, 1); doKeysymEvent(GDK_malesymbol, 0); return TRUE;
            case 0xAC:			// femalesymbol
                doKeysymEvent(GDK_femalesymbol, 1); doKeysymEvent(GDK_femalesymbol, 0); return TRUE;
#if defined(EMP_313)
            case 0xE7:			// page up
                doKeyEvent(112, 1); doKeyEvent(112, 0); return TRUE;
            case 0xE0:			// page down
                doKeyEvent(117, 1); doKeyEvent(117, 0); return TRUE;
#else
            case 0xE0:			// page up
                doKeyEvent(112, 1); doKeyEvent(112, 0); return TRUE;
            case 0xE7:			// page down
                doKeyEvent(117, 1); doKeyEvent(117, 0); return TRUE;
#endif
        }

    }
    return FALSE;
}

void FakeEscKey(void)
{
    doKeyEvent(9, 1); doKeyEvent(9, 0);
}

GdkColor* MeasureColorConvert(int colorIndex)
{
    if (colorIndex == 0)
        return g_red;
	else if (colorIndex == 1)
		return g_yellow;
	else if (colorIndex == 2)
		return g_green;
	else if (colorIndex == 3)
		return g_blue;
	else if (colorIndex == 4)
		return g_white;
    else if(colorIndex == 5)
        return g_black;
	else
		return NULL;
}

int CountLines(string& text)
{
	int count = 1;
	string::size_type pos = 0;
	while ((pos = text.find_first_of("\n", pos)) != string::npos) {
		++pos;
		++count;
	}
	return count;
}

// Remove leading and trailing whitespace
void trim(string& str)
{
	static const char whitespace[] = " \n\t\v\r\f";
	str.erase(0, str.find_first_not_of(whitespace));
	str.erase(str.find_last_not_of(whitespace) + 1);
}

// 返回Combobox的index值, 失败返回-1
int GetComboBoxIndex(GtkComboBox *combobox, string name)
{
	GtkTreeIter iter;
	GtkTreeModel* model = gtk_combo_box_get_model(GTK_COMBO_BOX (combobox));
	char *new_name = 0;
	string str_name;
	if (gtk_tree_model_get_iter_first (model, &iter)) {
		gtk_tree_model_get(model, &iter, 0, &new_name, -1);
		str_name = new_name;
		delete new_name;
		new_name = 0;
		while (str_name != name) {
			if (gtk_tree_model_iter_next(model, &iter)) {
				gtk_tree_model_get(model, &iter, 0, &new_name, -1);
				str_name = new_name;
				delete new_name;
				new_name = 0;
			} else {
				PRINTF("Do not exist in combobox\n");
				return -1;
			}
		}
		GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
		char *path_str = gtk_tree_path_to_string(path);
		gtk_tree_path_free(path);
		int ret = atoi(path_str);
		delete path_str;
		return ret;
	} else {
		return -1;
	}
}

bool IsNum(string str)
{
	std::istringstream iss(str);
	float i;
	iss>> i;
	return iss.rdstate() == std::ios_base::eofbit;
}

PangoFontDescription* AdaptStringFont(const char *font_family, const char *font_style, int font_size, PangoLayout *layout, const char *str, const int max_width)
{
	char font_des[128];
	int width = 0;
	int height = 0;
	int size = font_size;
	PangoFontDescription *font = NULL;
	do {
		sprintf(font_des, "%s, %s, %d", font_family, font_style, size);
		font = pango_font_description_from_string(font_des);
		pango_layout_set_font_description(layout, font);
		pango_layout_set_alignment(layout, PANGO_ALIGN_LEFT);
		pango_layout_get_pixel_size(layout, &width, &height);
		--size;
	} while (width > max_width);
	return font;
}

void SetTheme(const char *rc_path)
{
	gtk_rc_parse(rc_path);
	gtk_rc_reparse_all();
	gtk_rc_reset_styles(gtk_settings_get_default());
}

#define XK_PUBLISHING
#include <X11/keysymdef.h>

void XRemapKey(Display *display, int old_keycode, KeySym new_keysym)
{
	if(old_keycode > 0)
		XChangeKeyboardMapping(display, old_keycode, 1, &new_keysym, 1);
}

void ChangeKeymap()
{
	Display *display = getXDisplay();

	int min_keycode, max_keycode;
	XDisplayKeycodes(display, &min_keycode, &max_keycode);
	PRINTF("The range of keycode is %d - %d\n", min_keycode, max_keycode);
	XRemapKey(display, max_keycode, XK_femalesymbol);
	XRemapKey(display, max_keycode-1, XK_malesymbol);

#if 0	//test code
	for(int i=min_keycode; i<=max_keycode; i++)
	{
		KeySym ks1 = XKeycodeToKeysym(display, i, 0);
		KeySym ks2 = XKeycodeToKeysym(display, i, 1);
		char* kstr1 = XKeysymToString(ks1);
		char* kstr2 = XKeysymToString(ks2);
		PRINTF("keycode=%d, keysym0=0x%x, string1=\"%s\", keysym1=0x%x, string1=\"%s\"\n", i, (int)ks1, kstr1, (int)ks2, kstr2);
	}
#endif

	XFlush(display);
	XCloseDisplay(display);
}