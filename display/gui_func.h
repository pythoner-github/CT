#ifndef GUI_FUNC_H
#define GUI_FUNC_H

#include <gtk/gtk.h>
#include <string>
#include "Def.h"
#include <vector>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>
Display * getXDisplay(void);

using std::string;

GdkColor* new_color(guint16 red, guint16 green, guint16 blue);
void free_color(GdkColor *color);
bool init_colors(void);
void free_init_colors(void);

void CountUTF8(const void* pBuffer, long size, int *ascii, int *utf8);
void GetCurrentDate(int &Year, int &Month, int &Day);
void GetCurrentDateTime(int &Year, int &Month, int &Day, int &Hour, int &Minute, int &Second);
void GetEntryTextForDB(GtkWidget *entry, std::string& text);
bool CalcAge(const int year, const int month, const int day, int& age, int& age_unit);
void ChangeDateFormatToString(int iYear, int iMon, int iDay, std::string &sYear, std::string &sMon, std::string &sDay);
void ChangeTimeFormatToString(int iHour, int iMin, int iSec, std::string &sHour, std::string &sMin, std::string &sSec);
void GenerateDataString(std::string year, std::string mon, std::string day, std::string &date);
void QuichSearchDataString(std::string year, std::string mon, std::string day, std::string &date);
void GetDateString(int iYear, int iMon, int iDay, std::string &date);
void GetTimeString(int iHour, int iMin, int iSec, std::string &time);
void ValidDateFormat(std::string &sYear, std::string &sMon, std::string &sDay);
void GetAge(int year, int mon, int day, std::string &age); //age 090Y/M/D
void GenAgeStr(int age, int ageUnit, std::string &strAge); //age 090Y/M/D
void GetSexString(int iSex, std::string &sSex);
void GetTypeStr(string &src, string &des);
int CalcLength(POINT p1, POINT p2);
void DotToCommaLocaleNumeric(char *Comma, unsigned int size);
void CommaToDotLocaleNumeric(char *Dot, unsigned int size);

#ifdef VET
void GetAnimalSpeciesString(int index, std::string &species);
#endif

GdkColor* MeasureColorConvert(int colorIndex);
GtkWidget* create_label(const char *str, guint width, guint height, const GdkColor* const color, PangoFontDescription *font);
GtkWidget* create_button(GtkWidget *label, guint width, guint height, const GdkColor* const color);
GtkWidget* create_button_icon(GtkWidget *label, GtkWidget *icon_image);
GtkWidget* create_button_icon_only(GtkWidget *icon_image, guint width, guint height, const GdkColor *color);
GtkWidget* create_combobox(guint width, guint height, const char *layouttype, GtkTreeModel *treemodel);
GtkWidget* create_combo_box_with_text(const int width, const int height, const std::vector<std::string> text);
GtkWidget* create_pixmap(const gchar *filename);
PangoLayout * create_pango_text(GtkWidget *widget, 
				const char *text, 
				int width, 
				PangoFontDescription *font,
				const GdkColor* const color);

void doKeyEvent(const unsigned int keycode, int is_press);
void doBtnEvent(const unsigned int button, int is_press);
void doMotionEvent(int x, int y);
void SetSystemCursor(int x, int y);
void ResetIfOutOfRange(void);
void SetSystemCursorToCenter();

void fakeXMotionEvent(int offsetX, int offsetY);
void fakeXMotionEventFullRange(int offsetX, int offsetY);
void fakeXMotionEventImage(int &x0, int &y0, int offsetX, int offsetY);
void fakeXMotionEventMenu(int &x0, int &y0, int offsetX, int offsetY);
void fakeXMotionEventScreen(int &x0, int &y0, int offsetX, int offsetY);
void SetMenuBiopsyCursorYRange(int y0,int y1);//2016.08.25
void fakeXMotionEventMenuBiopsy(int offsetX, int offsetY);//2016.08.24

void FakeEscKey(void); 
gboolean FakeMouseButton(unsigned char key);
gboolean FakeNumKey(unsigned char key);
gboolean FakeAlphabet(unsigned char key);
gboolean FakePunctuation(unsigned char key);

int CountLines(std::string& text);
void trim(std::string& str);
int GetComboBoxIndex(GtkComboBox *combobox, string name);
bool IsNum(std::string str);
PangoFontDescription* AdaptStringFont(const char *font_family, const char *font_style, int font_size, PangoLayout *layout, const char *str, const int max_width);

template <typename T> T max_3(const T &a, const T &b, const T &c)
{ return a > b ? (a > c ? a : c) : (b > c ? b : c); }

template <typename T> T min_3(const T &a, const T &b, const T &c)
{ return a < b ? (a < c ? a : c) : (b < c ? b : c); }

void SetTheme(const char *rc_path);

void XRemapKey(Display *display, int old_keycode, KeySym new_keysym);
void ChangeKeymap();

#endif
