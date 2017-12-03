// -*- c++ -*-
#ifndef _VIEW_PRINT_PREVIEW_H_
#define _VIEW_PRINT_PREVIEW_H_

#include "FakeXEvent.h"
#include <string>
	//GtkWidget *GetMainWindow(void);
class ViewPrintPreview : public FakeXEvent
{
public:
    ~ViewPrintPreview();
    static ViewPrintPreview* GetInstance();
    void CreateWindow();
	void CreateWindow(const char* path, GtkWidget *parent);
    void CreateWindow2(void);
	GtkWidget *GetMainWindow(void);
    int GetReportPage(void) { return report_page_count; }
	void CreatePages(char *path);
    void BtnPrintClicked(GtkButton *button);
    void ClickedPrintButton(GtkButton *button);
     void CreatePagesForRetrieve(char *path,string indication,string comments,vector<string> imageFileName);
    void PrintArchiveReport(const char* path);
    void QuickPrintScreen(const char *path);

private:
    ViewPrintPreview();
    void KeyEvent(unsigned char keyValue);
    static ViewPrintPreview* m_ptrInstance;
    GtkWidget *m_radiobutton_auto;
    GtkWidget *m_radiobutton_orignial;
    GtkWidget *m_scrolledwindow_auto;
    GtkWidget *m_scrolledwindow_orignial;
    
    GtkWidget *m_window;
    string m_imgPath;
    GtkWidget *m_image;
    GdkPixmap *m_pixmapPrintImage;
    PangoFontDescription *m_baseFont;
    PangoFontDescription *m_headFont;
    PangoFontDescription *m_commentTmpFont; 

    #define MAX_PAGE        (30)
    
#ifdef REPORTPICS_6
    #define MAX_IMAGE       (6)
#else
    #define MAX_IMAGE       (2)
#endif

    //#define IMAGE_SCALE		(0.7)
    #define IMG_SCALE_WIDTH		(440)//(IMG_W*IMAGE_SCALE)
    //#define IMG_SCALE_HEIGHT	(IMG_H*IMAGE_SCALE)


    
    GdkGC *pagegc;
    GdkPixmap *pagepixmap;
    GtkWidget *drawingarea;
    PangoLayout *pagelayout;
        
    GtkWidget *report_image[30];
    
    int report_page_count;
    bool page_num;
    char *m_path;
	void InitReportPagePath(char *path);
    char *GetReportPageName(int page);
    int SavePageByPixmap(const char *path);
    void AddTextToPageByPixmap(int &x, int &y, char *text, PangoFontDescription *font);
    void AddImageToPageByPixmap(int &x, int &y, GdkPixbuf *pixbuf);
   // void ClickedPrintButton(GtkButton *button);
    void ExposeImage(GtkWidget *widget, GdkEventExpose *event);
    static gboolean HandleExposeImage(GtkWidget *widget, GdkEventExpose *event, ViewPrintPreview *data){ data->ExposeImage(widget, event); return FALSE; }
    static void HandleClickPrintButton(GtkButton *button, ViewPrintPreview *data) { data->ClickedPrintButton(button); }
    
    //struct print_re_item  Item; 

    void SeekPrinter(char *device);
    void Printing(void);

// signal handle
   // void BtnPrintClicked(GtkButton *button);
    void BtnExitClicked(GtkButton *button);
    void BtnSettingClicked(GtkButton *button);

    //2012.10.10 lhm
    void BtnPrintWinClicked(GtkButton *button);
    void BtnExitWinClicked(GtkButton *button);
    void BtnSettingWinClicked(GtkButton *button);
    void PrintAreaConfigure(GtkWidget *widget, GdkEventConfigure *event);
    void PrintAreaExpose(GtkWidget *widget, GdkEventExpose *event);
    void Item_To_Get();
    void Draw_To_Report(struct print_re_item Item, int lenOfline, int numOfimage);
    int SaveReport_To_Print(const char *path);


    // signal connect
    static gboolean HandleDeleteEvent(GtkWidget *widget, GdkEvent *event, ViewPrintPreview *data)
	{ data->BtnExitClicked(NULL); return FALSE; }
 
    static gboolean HandleDeleteWinEvent(GtkWidget *widget, GdkEvent *event, ViewPrintPreview *data)
	{ data->BtnExitWinClicked(NULL); return FALSE; }

    static void on_button_print_clicked(GtkButton *button, ViewPrintPreview *data) { data->BtnPrintClicked(button); }
    static void on_button_exit_clicked(GtkButton *button, ViewPrintPreview *data) { data->BtnExitClicked(button); }
    static void on_button_setting_clicked(GtkButton *button, ViewPrintPreview *data) { data->BtnSettingClicked(button); }

//2012.10.10 lihuamei
    static gboolean HandlePrintPreviewAreaConfigure(GtkWidget *widget, GdkEventConfigure *event, ViewPrintPreview *data)
    { data->PrintAreaConfigure(widget, event); return FALSE; }

    static gboolean HandlePrintPreviewAreaExpose(GtkWidget *widget, GdkEventExpose *event, ViewPrintPreview *data)
    { data->PrintAreaExpose(widget, event); return FALSE; }

    static void button_print_clicked(GtkButton *button, ViewPrintPreview *data) { data->BtnPrintWinClicked(button); }
    static void button_exit_clicked(GtkButton *button, ViewPrintPreview *data) { data->BtnExitWinClicked(button); }
    static void button_setting_clicked(GtkButton *button, ViewPrintPreview *data) { data->BtnSettingWinClicked(button); }
    static void on_auto_radio_button_toggled(GtkToggleButton *togglebutton, ViewPrintPreview *data) { data->AutoRadioToggled(togglebutton); }
    static void on_orignial_radio_button_toggled(GtkToggleButton *togglebutton, ViewPrintPreview *data) { data->OrignialRadioToggled(togglebutton); }
    void AutoRadioToggled(GtkToggleButton *togglebutton);
    void OrignialRadioToggled(GtkToggleButton *togglebutton);

    GtkWidget* CreateImageWindow(GtkWidget *frame, int viewmode);
    GtkWidget* CreateReportWindow(GtkWidget *frame, int viewmode);
    
};
#endif
