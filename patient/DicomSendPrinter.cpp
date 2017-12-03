#include "patient/DicomSendPrinter.h"

#include "sysMan/SysGeneralSetting.h"
#include "keyboard/KeyValueOpr.h"
#include "patient/ImgMan.h"
#include "patient/Database.h"
#include "display/ViewDialog.h"
#include "patient/ViewArchive.h"
#include "ViewMain.h"

//#include "cxtypes.h"
#include <opencv/cxcore.h>
#include "sysMan/DicomPrint.h"
#include "sysMan/ViewSystem.h"
#include "sysMan/DicomServiceSetting.h"

#define SCALE_WIDTH  1

#define SCALE_HEIGHT 1

#define AREA_WIDTH 844
#define AREA_HEIGHT 660

DicomSendPrinter *DicomSendPrinter::m_pInstance = NULL;
DicomSendPrinter::DicomSendPrinter()
{
    m_index = 0;
}
DicomSendPrinter::~DicomSendPrinter()
{

}

DicomSendPrinter *DicomSendPrinter::GetInstance()
{
    if(NULL == m_pInstance)
    {
        m_pInstance = new DicomSendPrinter();
    }
    return m_pInstance;
}

GtkWidget* DicomSendPrinter::CreateItem(GtkWidget **button, GtkWidget **checkButton, char *id)
{
    GtkWidget *vbox;
    //	GtkWidget *hbox;

    vbox = gtk_vbox_new (FALSE, 0);

    *button = gtk_button_new ();
    gtk_box_pack_start (GTK_BOX (vbox), *button, FALSE, FALSE, 0);
    gtk_widget_set_size_request (*button, (SNAP_W+10)*SCALE_WIDTH, -1);
    g_signal_connect(G_OBJECT(*button), "clicked", G_CALLBACK(HandleSnapImgClicked), this);
    gtk_button_set_relief (GTK_BUTTON (*button), GTK_RELIEF_NONE);
    g_object_set_data(G_OBJECT(*button), "id", id);

    return vbox;
}

void DicomSendPrinter::InitParameter(vector<string> examID)
{

    m_fixedContent = NULL;
    if(!m_isUdisk)
        m_vecExamID = examID;
    else
    {
        m_vecExamID.clear();
        m_vecExamID.push_back("0");
    }
    m_curPat = 0;
    m_vecFileName.clear();
    m_vecFileSel.clear();
    m_curImgPage = 0;
    m_maxImgPage = 0;
}

void DicomSendPrinter::getPositionSize(areaImgInfo_t positionsize,FilmSize filmsize,ORIENTATION orientation)
{
    float size;

    size = 	(float)positionsize.width/(float)positionsize.height;

    PRINTF("size = %f\n",(float)positionsize.width/(float)positionsize.height);

    if(size > getMultiple(filmsize,orientation))
    {
        m_positionsize.height= positionsize.height;
        m_positionsize.width = positionsize.height*getMultiple(filmsize,orientation);

        m_positionsize.x = positionsize.x + (positionsize.width - m_positionsize.width)/2;
        m_positionsize.y = positionsize.y;
        PRINTF("x0 = %d,y0 = %d\n",positionsize.x,positionsize.y);
        PRINTF("x = %d,y = %d\n",m_positionsize.x,m_positionsize.y);
    }
    else
    {
        m_positionsize.width= positionsize.width;
        m_positionsize.height= (float)positionsize.width/getMultiple(filmsize,orientation);
        m_positionsize.x = positionsize.x;
        m_positionsize.y = positionsize.y+(positionsize.height - m_positionsize.height)/2;
        PRINTF("x0 = %d,y0 = %d\n",positionsize.x,positionsize.y);
        PRINTF("x = %d,y = %d\n",m_positionsize.x,m_positionsize.y);
    }
}

GtkWidget* DicomSendPrinter::GetWindow(void)
{
    if (GTK_IS_WIDGET(m_window))
        return m_window;
    else

        return NULL;
}

float DicomSendPrinter::getMultiple(FilmSize filesize,ORIENTATION orientation)
{
    float buf1[9]={8.0/10.0,10.0/12.0,10.0/14.0,11.0/14.0,14.0/14.0,14.0/17.0,24.0/24.0,24.0/30.0,210.0/297.0};
    float buf2[9]={10.0/8.0,12.0/10.0,14.0/10.0,14.0/11.0,14.0/14.0,17.0/14.0,24.0/24.0,30.0/24.0,297.0/210.0};
    float *buf = NULL;
    if(orientation == PORTRAIT)
    {
        buf = buf1;
    }
    else
    {
        buf = buf2;
    }

    PRINTF("filesize = %f\n",buf[filesize]);
    return buf[filesize];

}

void DicomSendPrinter::BtnExitClicked()
{
    //ExitSlide();
    //DestroyWindow();

    Replay::GetInstance()->SetDefaultFrameFunc();

    // destroywindow
    if(GTK_IS_WIDGET(m_window))
    {
        DestroyDrawData();
        gtk_widget_destroy(m_window);
        g_keyInterface.Pop();
        m_window = NULL;
	}

}

void DicomSendPrinter::handleLeftButtonEvent(int x,int y)
{
    int size  = 0;
    int i = 0;

    size = m_rectangleInfo.size();

    m_index = x/m_rectangleInfo[0].width+(y/m_rectangleInfo[0].height)*m_multiple;

    PRINTF("index = %d\n",m_index);

    if(size != 0)
    {

        for(i = 0;i <size;i++)
        {
            SetRectangleColor(i,g_deepGray);
        }
        SetRectangleColor(m_index,g_green);
        gtk_widget_queue_draw(m_drawArea);
    }
}

bool DicomSendPrinter::dealMousePress(GtkWidget *widget,GdkEventButton *event)
{
    switch(event->button)
    {

        case 1:
            PRINTF("Left Buttion\n");
            DicomSendPrinter::GetInstance()->handleLeftButtonEvent((int)event->x,(int)event->y);
            break;
        case 2:
            PRINTF("Middle Buton\n");

            break;
        case 3:
            PRINTF("Right Button\n");

            break;

    }
    PRINTF("press_x = %f,press_y = %f\n",event->x,event->y);
    return TRUE;
}

void DicomSendPrinter::CreateWindow(vector<string> examID,Format format,FilmSize filmsize,ORIENTATION orientation,GdkColor *bgColor)
{
    GtkWidget *fixed;

    GtkWidget *framePatSel;
    GtkWidget *fixedPat;
    GtkWidget *hboxPatSel;
    GtkWidget *btnLastPat;
    GtkWidget *btnNextPat;
    GtkWidget *hboxPatInfo;
    GtkWidget *vseparatorPat;
    GtkWidget *vboxPatStat;
    GtkWidget *vseparator2;
    GtkWidget *labelPatSel;
    GtkWidget *alignment;
    GtkWidget *image;
    GtkWidget * PrintSetting;
    GtkWidget *btnClear;
    areaImgInfo_t areaImgInfo;
    int i = 0;
    int j = 0;
    int size = 0;

    GtkWidget *fixedContent;
    GtkWidget *tableImg;
    GtkWidget *hboxImgSel;

    GtkWidget *btnExit;
    GtkWidget *btnPrint;
    areaImgInfo_t positionsize;
    DCMPRINTIMAGEINFO imagInfo;

    m_format = format;
    m_filmsize = filmsize;

    if(examID.size() == 0)
        m_isUdisk = TRUE;
    else
        m_isUdisk = FALSE;

    InitParameter(examID);
    // main window
    m_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (m_window, SCREEN_WIDTH, SCREEN_HEIGHT);
    gtk_window_set_modal (GTK_WINDOW (m_window), TRUE);
    gtk_window_set_title (GTK_WINDOW (m_window), _("Image manage"));
    gtk_window_set_position (GTK_WINDOW (m_window), GTK_WIN_POS_CENTER_ALWAYS);
    gtk_window_set_resizable (GTK_WINDOW (m_window), FALSE);
    gtk_window_set_decorated (GTK_WINDOW (m_window), FALSE);
    gtk_window_set_transient_for (GTK_WINDOW (m_window), GTK_WINDOW(GetWindow()));
    fixed = gtk_fixed_new ();
    gtk_container_add (GTK_CONTAINER (m_window), fixed);

    // patient selection region
    framePatSel = gtk_frame_new (NULL);
    gtk_fixed_put(GTK_FIXED (fixed), framePatSel, 174*SCALE_WIDTH, 5*SCALE_HEIGHT);
    gtk_widget_set_size_request (framePatSel, 830*SCALE_WIDTH, 60*SCALE_HEIGHT);
    gtk_frame_set_shadow_type (GTK_FRAME (framePatSel), GTK_SHADOW_IN);

    alignment = gtk_alignment_new (0.1, 0.5, 0.9, 1);
    gtk_container_add(GTK_CONTAINER (framePatSel), alignment);

    fixedPat = gtk_fixed_new ();
    gtk_container_add(GTK_CONTAINER (alignment), fixedPat);

    hboxPatSel = gtk_hbox_new (TRUE, 0);
    gtk_fixed_put(GTK_FIXED (fixedPat), hboxPatSel, 0*SCALE_WIDTH, 0*SCALE_HEIGHT);
    gtk_widget_set_size_request (hboxPatSel, 70*SCALE_WIDTH, 35*SCALE_HEIGHT);

    btnLastPat = gtk_button_new ();
    gtk_box_pack_start (GTK_BOX (hboxPatSel), btnLastPat, FALSE, FALSE, 0);
    g_signal_connect(G_OBJECT(btnLastPat), "clicked", G_CALLBACK(HandleBtnLastPatClicked), this);

    image = gtk_image_new_from_stock ("gtk-go-back", GTK_ICON_SIZE_BUTTON);
    gtk_container_add(GTK_CONTAINER (btnLastPat), image);

    btnNextPat = gtk_button_new ();
    gtk_box_pack_start (GTK_BOX (hboxPatSel), btnNextPat, FALSE, FALSE, 0);
    g_signal_connect(G_OBJECT(btnNextPat), "clicked", G_CALLBACK(HandleBtnNextPatClicked), this);

    image = gtk_image_new_from_stock ("gtk-go-forward", GTK_ICON_SIZE_BUTTON);
    gtk_container_add (GTK_CONTAINER (btnNextPat), image);

    hboxPatInfo = gtk_hbox_new (TRUE, 0);
    gtk_fixed_put(GTK_FIXED (fixedPat), hboxPatInfo, 145*SCALE_WIDTH, 0*SCALE_HEIGHT);
    //gtk_widget_set_size_request (hboxPatInfo, 650*SCALE_WIDTH, 35*SCALE_HEIGHT);
    gtk_widget_set_size_request (hboxPatInfo, 650*SCALE_WIDTH+20, 35*SCALE_HEIGHT);//2016.06.03--hy

    m_labName = gtk_label_new ("");
    gtk_box_pack_start (GTK_BOX (hboxPatInfo), m_labName, FALSE, TRUE, 0);
    gtk_misc_set_alignment (GTK_MISC (m_labName), 0, 0.5);

#ifdef VET

    m_labOwner = gtk_label_new ("");
    gtk_box_pack_start (GTK_BOX (hboxPatInfo), m_labOwner, FALSE, TRUE, 0);
    gtk_misc_set_alignment (GTK_MISC (m_labOwner), 0, 0.5);

    m_labSpecies = gtk_label_new ("");
    gtk_box_pack_start (GTK_BOX (hboxPatInfo), m_labSpecies, FALSE, TRUE, 0);
    gtk_misc_set_alignment (GTK_MISC (m_labSpecies), 0, 0.5);
#endif

    m_labSex = gtk_label_new ("");
    gtk_box_pack_start (GTK_BOX (hboxPatInfo), m_labSex, FALSE, TRUE, 0);
    gtk_misc_set_alignment (GTK_MISC (m_labSex), 0, 0.5);

    m_labBirthday = gtk_label_new ("");
    gtk_box_pack_start (GTK_BOX (hboxPatInfo), m_labBirthday, FALSE, TRUE, 0);
    gtk_misc_set_alignment (GTK_MISC (m_labBirthday), 0, 0.5);

    m_labExamNum = gtk_label_new ("");
    gtk_box_pack_start (GTK_BOX (hboxPatInfo), m_labExamNum, FALSE,FALSE, 0);
    gtk_misc_set_alignment (GTK_MISC (m_labExamNum), 0, 0.5);

    vseparatorPat = gtk_vseparator_new ();
    gtk_fixed_put(GTK_FIXED (fixedPat), vseparatorPat, 65*SCALE_WIDTH, 0*SCALE_HEIGHT);
    gtk_widget_set_size_request (vseparatorPat, 20*SCALE_WIDTH, 35*SCALE_HEIGHT);

    vboxPatStat = gtk_vbox_new (TRUE, 0);
    gtk_fixed_put(GTK_FIXED (fixedPat), vboxPatStat, 80*SCALE_WIDTH, 0*SCALE_HEIGHT);
    gtk_widget_set_size_request (vboxPatStat, 55*SCALE_WIDTH, 35*SCALE_HEIGHT);

    m_labStatInfo = gtk_label_new ("");
    gtk_box_pack_start (GTK_BOX (vboxPatStat), m_labStatInfo, FALSE, FALSE, 0);

    vseparator2 = gtk_vseparator_new ();
    gtk_fixed_put(GTK_FIXED (fixedPat), vseparator2, 130*SCALE_WIDTH, 0*SCALE_HEIGHT);
    gtk_widget_set_size_request (vseparator2, 20*SCALE_WIDTH, 35*SCALE_HEIGHT);
#ifdef VET
    labelPatSel =gtk_label_new (_("<b>Animal</b>"));
#else
    labelPatSel =gtk_label_new (_("<b>Patient</b>"));
#endif
    //labelPatSel = gtk_label_new (_("<b>Patient</b>"));
    gtk_frame_set_label_widget (GTK_FRAME (framePatSel), labelPatSel);
    gtk_label_set_use_markup (GTK_LABEL (labelPatSel), TRUE);

    PrintSetting= gtk_button_new_with_mnemonic(_("Print Config"));
    gtk_fixed_put(GTK_FIXED(fixed), PrintSetting, 20*SCALE_WIDTH, 20*SCALE_HEIGHT);
    gtk_widget_set_size_request(PrintSetting, 130*SCALE_WIDTH, 30*SCALE_HEIGHT);
    g_signal_connect(G_OBJECT(PrintSetting), "clicked", G_CALLBACK(HandlePrintSettingClicked), this);

    //m_fixedContent = CreateImgArea(fixed);

    fixedContent = gtk_fixed_new ();
    gtk_fixed_put (GTK_FIXED (fixed), fixedContent, 20*SCALE_WIDTH, 70*SCALE_HEIGHT);
    gtk_widget_set_size_request (fixedContent, 1004*SCALE_WIDTH, 693*SCALE_HEIGHT);

    // create snap table
    GtkWidget* framePreview;
    framePreview = gtk_frame_new(NULL);
    gtk_fixed_put(GTK_FIXED(fixedContent), framePreview, 0*SCALE_WIDTH, 0*SCALE_HEIGHT);
    gtk_widget_set_size_request(framePreview, 145*SCALE_WIDTH, 688*SCALE_HEIGHT);
    gtk_frame_set_shadow_type(GTK_FRAME(framePreview), GTK_SHADOW_IN);

    GtkWidget *fixedImg = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(framePreview), fixedImg);

    tableImg = gtk_table_new (SNAP_ROW, SNAP_COL, TRUE);
    gtk_fixed_put(GTK_FIXED(fixedImg), tableImg, 10*SCALE_WIDTH, 0*SCALE_HEIGHT);
    gtk_widget_set_size_request(tableImg, 135*SCALE_WIDTH, 615*SCALE_HEIGHT);
    gtk_container_set_border_width(GTK_CONTAINER(tableImg), 1);

    for(i = 0; i < SNAP_ROW; i ++)
    {
        for(j = 0; j < SNAP_COL; j ++)
        {
            sprintf(m_id[i*SNAP_COL+j], "%d", i*SNAP_COL+j);
            m_vbox[i*SNAP_COL+j] = CreateItem (&m_btnSnap[i*SNAP_COL+j], &m_checkBtnSnap[i*SNAP_COL+j], m_id[i*SNAP_COL+j]);
            gtk_table_attach_defaults(GTK_TABLE(tableImg), m_vbox[i*SNAP_COL+j], j, j+1, i, i+1);
        }
    }
    hboxImgSel = gtk_hbox_new (TRUE, 0);
    gtk_fixed_put(GTK_FIXED(fixedImg), hboxImgSel, 0*SCALE_WIDTH, 653*SCALE_HEIGHT);
    gtk_widget_set_size_request (hboxImgSel, 140*SCALE_WIDTH, 30*SCALE_HEIGHT);
    m_btnLeft = gtk_button_new_with_mnemonic ("<");
    gtk_box_pack_start (GTK_BOX (hboxImgSel),m_btnLeft, FALSE, TRUE, 0);
    g_signal_connect(G_OBJECT(m_btnLeft), "clicked", G_CALLBACK(HandleBtnLastImgPageClicked), this);

    m_labImgPage = gtk_label_new ("5/16");
    gtk_box_pack_start (GTK_BOX (hboxImgSel), m_labImgPage, FALSE, FALSE, 0);

    m_btnRight = gtk_button_new_with_mnemonic (">");
    gtk_box_pack_start (GTK_BOX (hboxImgSel), m_btnRight, FALSE, TRUE, 0);
    g_signal_connect(G_OBJECT(m_btnRight), "clicked", G_CALLBACK(HandleBtnNextImgPageClicked), this);

    // display large image of original size
    m_drawArea = gtk_drawing_area_new (); //800*640
    //gtk_fixed_put (GTK_FIXED (fixedContent), m_drawArea, 155*SCALE_WIDTH, 0*SCALE_HEIGHT);
    //gtk_widget_set_size_request (m_drawArea, (IMG_WIDTH-10)*SCALE_WIDTH, IMG_HEIGHT*SCALE_HEIGHT);

    positionsize.width = (IMG_WIDTH-10)*SCALE_WIDTH;
    positionsize.height = IMG_HEIGHT*SCALE_HEIGHT;
    positionsize.x = 155*SCALE_WIDTH;
    positionsize.y = 0*SCALE_HEIGHT;

    getPositionSize(positionsize,m_filmsize,orientation);
    gtk_fixed_put (GTK_FIXED (fixedContent), m_drawArea, m_positionsize.x, m_positionsize.y);
    gtk_widget_set_size_request (m_drawArea, m_positionsize.width, m_positionsize.height);
    gtk_widget_modify_bg(m_drawArea, GTK_STATE_NORMAL, bgColor);
    //g_signal_connect(G_OBJECT(m_drawArea), "configure_event", G_CALLBACK(HandleDrawAreaConfigure), this);
    g_signal_connect(G_OBJECT(m_drawArea), "expose_event", G_CALLBACK(HandleDrawAreaExpose), this);

    gtk_widget_add_events(m_drawArea,GDK_BUTTON_PRESS_MASK|GDK_BUTTON_MOTION_MASK);
    g_signal_connect(m_drawArea,"button-press-event",G_CALLBACK(DicomSendPrinter::dealMousePress),NULL);

    m_hboxFunc = gtk_hbox_new(TRUE, 10);
    gtk_fixed_put(GTK_FIXED(fixedContent), m_hboxFunc, (734- 120)*SCALE_WIDTH, 643*SCALE_HEIGHT);
    //	gtk_widget_set_size_request(m_hboxFunc, 380, 45);
    //	gtk_widget_set_size_request(m_hboxFunc, (-1)*SCALE_WIDTH, 45*SCALE_HEIGHT);
    gtk_widget_set_size_request(m_hboxFunc, -1, 45*SCALE_HEIGHT);

	image = gtk_image_new_from_stock("gtk-clear", GTK_ICON_SIZE_BUTTON);
    btnClear= gtk_label_new_with_mnemonic (_("Clear"));
    btnClear = create_button_icon(btnClear, image);
    gtk_box_pack_start(GTK_BOX (m_hboxFunc), btnClear, FALSE, TRUE, 0);
    g_signal_connect(G_OBJECT(btnClear), "clicked", G_CALLBACK(HandleBtnClearClicked), this);

    //m_btnShow= gtk_button_new_with_label(_("Print"));
    //gtk_box_pack_start (GTK_BOX (m_hboxFunc), m_btnShow, FALSE, TRUE, 0);
    image = gtk_image_new_from_stock("gtk-print", GTK_ICON_SIZE_BUTTON);
    btnPrint= gtk_label_new_with_mnemonic (_("Print"));
    btnPrint = create_button_icon(btnPrint, image);
    gtk_box_pack_start(GTK_BOX (m_hboxFunc), btnPrint, FALSE, TRUE, 0);
    g_signal_connect(G_OBJECT(btnPrint), "clicked", G_CALLBACK(HandleBtnPrintClicked), this);

    //btnExit= gtk_button_new_with_label(_("Exit"));
    //gtk_box_pack_start (GTK_BOX (m_hboxFunc), btnExit, FALSE, TRUE, 0);
    image = gtk_image_new_from_stock("gtk-quit", GTK_ICON_SIZE_BUTTON);
    btnExit = gtk_label_new_with_mnemonic (_("Exit"));
    btnExit = create_button_icon(btnExit, image);
    gtk_box_pack_start(GTK_BOX (m_hboxFunc), btnExit, FALSE, TRUE, 0);
    g_signal_connect(G_OBJECT(btnExit), "clicked", G_CALLBACK(HandleBtnExitClicked), this);

    SysGeneralSetting sysGeneralSetting;

    // show
    g_keyInterface.Push(this);
    gtk_widget_show_all(m_window);

    gtk_widget_show_all(m_fixedContent);

    gtk_fixed_move(GTK_FIXED(m_fixedContent), m_hboxFunc, 734*SCALE_WIDTH, 643*SCALE_HEIGHT);

    UpdateSnap();

    SetSystemCursorToCenter();
    //init
    //m_curPat = 0;
    PRINTF("==============================%s,%d,%d\n",m_vecExamID[m_curPat].c_str(),m_curPat,m_vecExamID.size());
    UpdateCurPat(m_vecExamID[m_curPat], m_curPat, m_vecExamID.size(), false);

    getMultiple(IN10XIN12,orientation);

    HandleAllFormatArea(m_format,m_positionsize,m_filmsize);

    size = m_rectangleInfo.size();
    if(size != 0)
    {
        for(i = 0;i < size;i++)
        {
            InitImageInfo(i);
        }
    }
    SetRectangleColor(m_index,g_green);
    m_total = getImageTotal(m_format);
    m_ImageInfo.clear();
    for(i = 0;i < m_total;i++)
    {
        imagInfo.imageFilePath = "";
        imagInfo.index = i;
        m_ImageInfo.push_back(imagInfo);
    }

    switch(m_errno)
    {
        case NODEFUALT :
            {
                ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::INFO, _("please set the default print service in system setting!"), NULL);
            }
            break;

        case FAIL_TO_GET:
            {
                ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::INFO, _("Fail to get configuration infomation!"), NULL);
            }
            break;

    }
}

void DicomSendPrinter::KeyEvent(unsigned char keyValue)
{
    FakeXEvent::KeyEvent(keyValue);

    switch(keyValue)
    {
        case KEY_ESC:
            g_timeout_add(100,DicomSendPrinter::ExitWindow , this);
            FakeEscKey();
            break;

        default:
            break;
    }
}

gboolean DicomSendPrinter::ExitWindow(gpointer data)
{
    DicomSendPrinter::GetInstance()->BtnExitClicked();

    return FALSE;
}

void DicomSendPrinter::ConvertToSystemSetting()
{
    Replay::GetInstance()->SetDefaultFrameFunc();

    // destroywindow
    if(GTK_IS_WIDGET(m_window))
    {
        DestroyDrawData();
        gtk_widget_destroy(m_window);
        g_keyInterface.Pop();
        m_window = NULL;
    }
    //ViewArchive::GetInstance()->UpdateImg();
    // exit Dicom Send

    ViewArchive::GetInstance()->DestroyWindow();

    ViewMain::GetInstance()->KeyEvent(KEY_SYSTEM);
    ViewSystem::GetInstance()->ChangeNoteBookPage(9);
    ViewSystem::GetInstance()->ChangeDicomNoteBookPage(2);
    DicomServiceSetting::GetInstance()->ChangeServiceNoteBook(5);
    DicomPrint::GetInstance()->init_print_setting();
}

int DicomSendPrinter::getImageTotal(Format format)
{
    int buf[10] = {1,2,4,6,9,12,15,16,20,24};
    return buf[format];
}

void DicomSendPrinter::InitRectangle()
{
    RectangleInfo_t rectangleinfo;
    GdkGC *gc = gdk_gc_new(m_drawArea->window);

    gdk_gc_set_line_attributes(gc, 1, GDK_LINE_SOLID, GDK_CAP_NOT_LAST, GDK_JOIN_BEVEL);

    rectangleinfo.gc = gc;
    rectangleinfo.x = m_rectanglePositionSize.x;
    rectangleinfo.y = m_rectanglePositionSize.y;
    if(rectangleinfo.x +m_rectanglePositionSize.width >= m_positionsize.width)
    {
        rectangleinfo.width = m_rectanglePositionSize.width - 1;
    }
    else
    {
        rectangleinfo.width = m_rectanglePositionSize.width;
    }

    if(rectangleinfo.y +m_rectanglePositionSize.height >= m_positionsize.height)
    {
        rectangleinfo.height = m_rectanglePositionSize.height - 1;
    }
    else
    {
        rectangleinfo.height = m_rectanglePositionSize.height;
    }

    rectangleinfo.g_color = g_deepGray;
    m_rectangleInfo.push_back(rectangleinfo);

    //g_object_unref(gc);
}

void DicomSendPrinter::HandleBtnPrintClicked(GtkButton *button, DicomSendPrinter* data)
{
    EDCMReturnStatus ret;
    GtkWidget *window;
    vector<DCMPRINTIMAGEINFO> ImageInfo;
    int size;
    int i = 0;

    window = DicomSendPrinter::GetInstance()->m_window;

    size = DicomSendPrinter::GetInstance()->m_total;

    for(i = 0 ;i < size;i++)
    {
        if(DicomSendPrinter::GetInstance()->m_ImageInfo[i].imageFilePath.size() != 0)
        {
            ImageInfo = DicomSendPrinter::GetInstance()->m_ImageInfo;

            break;
        }

    }

    ret = CDCMMan::GetMe()->PrintFilm(ImageInfo,AREA_WIDTH,AREA_HEIGHT);
    if(ret == DCMNODEFALUTSERVER)
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(window), ViewDialog::INFO, _("No default print server!"), NULL);

    }
    else if(ret == DCMCONNECTIONFAILURE)
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(window), ViewDialog::INFO, _("Connect print server failure!"), NULL);
    }
    else if(ret == DCMNOPRINTIMAGE)
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(window), ViewDialog::INFO, _("No print image!"), NULL);
    }
    else if(ret == DCMPRINTFAILURE)
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(window), ViewDialog::INFO, _("Print failure!"), NULL);
    }
    else
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(window), ViewDialog::INFO, _("Print success!"), NULL);

    }
}

void DicomSendPrinter::SetRectangleColor(int index,GdkColor* g_color)
{
    m_rectangleInfo[index].g_color = g_color;
}

void DicomSendPrinter::DrawRectangle(int index)
{
    gdk_gc_set_foreground(m_rectangleInfo[index].gc, m_rectangleInfo[index].g_color);
    gdk_draw_rectangle(m_drawArea->window, m_rectangleInfo[index].gc, FALSE, m_rectangleInfo[index].x, m_rectangleInfo[index].y, m_rectangleInfo[index].width, m_rectangleInfo[index].height);
}

void DicomSendPrinter::GetSnapsName(string examID, vector<string> &vec)
{
    unsigned int i;
    char *path;

    if(!m_isUdisk)
    {
        ImgMan::GetInstance()->LoadSnap(atoi(examID.c_str()), STORE_PATH, &vec);
        // get full path file name
        for (i = 0; i < vec.size(); i ++)
        {
            path = g_build_filename(STORE_PATH, examID.c_str(), vec[i].c_str(), NULL);
            vec[i] = path;
            g_free(path);
        }
    }
    else
    {
        struct dirent *ent;
        DIR *dir = opendir(SLIDE_PATH);
        if(!dir)
        {
            perror("opendir error");
            return;
        }

        vector<string> vec_tmp;
        while( (ent = readdir(dir)) != NULL)
        {
            if(ent->d_name[0]=='.')
                continue;
            //			if(ent->d_type==DT_DIR)
            gchar *absPath = g_build_path(G_DIR_SEPARATOR_S, SLIDE_PATH, ent->d_name, NULL);
            if(g_file_test(absPath, G_FILE_TEST_IS_DIR))
            {
                //get file name by directory name
                vec_tmp.clear();

                if(strcmp(ent->d_name, "3D")==0) // for 3D4D image
                {
                    if(ImgMan::GetInstance()->LoadSnap(absPath, &vec_tmp) != 0)
                    {
                        g_free(absPath);
                        continue;
                    }
                    //get full path file name
                    for (i = 0; i < vec_tmp.size(); i ++)
                    {
                        path = g_build_filename(SLIDE_PATH, ent->d_name, vec_tmp[i].c_str(), NULL);
                        vec_tmp[i] = path;
                        g_free(path);
                    }
                }
                else // for normal image
                {
                    //if(ImgMan::GetInstance()->LoadSnap(atoi(ent->d_name), SLIDE_PATH, &vec_tmp) != 0)
                    if(ImgMan::GetInstance()->LoadSnap(absPath, &vec_tmp) !=0)
                    {
                        g_free(absPath);
                        continue;
                    }
                    //get full path file name
                    for (i = 0; i < vec_tmp.size(); i ++)
                    {
                        path = g_build_filename(SLIDE_PATH, ent->d_name, vec_tmp[i].c_str(), NULL);
                        vec_tmp[i] = path;
                        g_free(path);
                    }
                }
                //add all file name to dest vector
                for(vector<string>::iterator it=vec_tmp.begin(); it!=vec_tmp.end(); ++it)
                    vec.push_back(*it);
            }
            g_free(absPath);
        }
        closedir(dir);
    }
}

void DicomSendPrinter::UpdateSnapColor()
{
    for(int i=0; i<(SNAP_COL * SNAP_ROW); i++)
    {
        gtk_widget_modify_bg(m_btnSnap[i], GTK_STATE_NORMAL, g_deepGray);
        gtk_widget_modify_bg(m_btnSnap[i], GTK_STATE_PRELIGHT, g_deepGray);
    }

    if(m_idShowed/(SNAP_COL * SNAP_ROW) == m_curImgPage)
    {
        gtk_widget_modify_bg(m_btnSnap[m_idShowed%(SNAP_COL * SNAP_ROW)], GTK_STATE_NORMAL, g_white);
        gtk_widget_modify_bg(m_btnSnap[m_idShowed%(SNAP_COL * SNAP_ROW)], GTK_STATE_PRELIGHT, g_white);
    }
}

void DicomSendPrinter::UpdateCurPat(string examID, int curPat, int totalPat, bool cur)
{
    gchar buf[256];
    //printf("examID = %s, curPat = %d, totalPat = %d\n", examID.c_str(), curPat, totalPat);

    if(!m_isUdisk)
    {
        // get all patient info from database, according to examID
        Database::NewPatSearchResult result;
        Database db;
        db.CommonSearch(examID, result);

        // update curent patient information
#ifdef VET
        sprintf(buf, "%s: %s  ", _("Name"), result.animal_name.c_str());
        gtk_label_set_text(GTK_LABEL(m_labName), buf);
        gtk_label_set_ellipsize(GTK_LABEL(m_labName),PANGO_ELLIPSIZE_END);

        //	float result[MEA_TIMES_MULTI];
        sprintf(buf, "%s: %s  ", _("Owner"), result.owner_name.c_str());
        gtk_label_set_text(GTK_LABEL(m_labOwner), buf);

        sprintf(buf, "%s: %s  ", _("Species"), result.species.c_str());
        gtk_label_set_text(GTK_LABEL(m_labSpecies), buf);
#else
        sprintf(buf, "%s: %s", _("Name"), result.name.c_str());
        gtk_label_set_text(GTK_LABEL(m_labName), buf);
        gtk_label_set_ellipsize(GTK_LABEL(m_labName),PANGO_ELLIPSIZE_END);
#endif
        sprintf(buf, "%s: %s", _("Gender"), result.gender.c_str());
        gtk_label_set_text(GTK_LABEL(m_labSex), buf);
#ifdef VET
        sprintf(buf, "%s: %s  ", _("Bir."), result.birthDate.c_str());
        gtk_label_set_text(GTK_LABEL(m_labBirthday), buf);
#else
        sprintf(buf, "%s: %s", _("Date of Birth"), result.birthDate.c_str());
        gtk_label_set_text(GTK_LABEL(m_labBirthday), buf);
#endif
        sprintf(buf, "%s: %s", _("Exam ID"), result.examID.c_str());
        gtk_label_set_text(GTK_LABEL(m_labExamNum), buf);
#if 0
        sprintf(buf, "ExamType: %s", result.examType.c_str());
        gtk_label_set_text(GTK_LABEL(m_labExamType), buf);
#endif
    }

    // update statistic info
    sprintf(buf, "%d/%d", curPat+1, totalPat);
    gtk_label_set_text(GTK_LABEL(m_labStatInfo), buf);

    // update image info
    m_vecFileName.clear();

    GetSnapsName(examID, m_vecFileName);
    m_numTotal = m_vecFileName.size();

    sort(m_vecFileName.begin(), m_vecFileName.end(), FileMan::SortByMTime);

    // update select status, unselect nothing
    m_vecFileSel.clear();
    unsigned int i;
    for (i = 0; i < m_vecFileName.size(); i ++)
    {
        m_vecFileSel.push_back(FALSE);
    }

    // update image area(display snap)
    int size = m_vecFileName.size();
    int temp = size / (SNAP_COL * SNAP_ROW);
    m_maxImgPage = (size % (SNAP_COL * SNAP_ROW)) == 0? temp : (temp+1);
    if(!cur)
        m_curImgPage =  0;

    UpdateSnap();
    UpdateSnapColor();
}

vector<string> DicomSendPrinter::GetSelExamID(void)
{
    vector<string> vec;
    vec.clear();
    return vec;
}

void DicomSendPrinter::BtnDisplayClicked(GtkMenuItem *menuitem)
{
    Format format = ONEXONE;
    FilmSize filmsize = IN10XIN14;
    ORIENTATION orientation = PORTRAIT;
    GdkColor *bgColor = g_black;
    string defaultDevice;
    DCMPRINTCONFIGELEMENT element;
    m_errno = NORMAL;
    vector<string> vecPatExamID = ViewArchive::GetInstance()->GetSelExamID();

    // display image management dialog

    if (vecPatExamID.size() == 0)
        //if( !(vecPatExamID.size() == 0))//test--2016.04.27
    {

        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()), ViewDialog::INFO, _("No record is selected!"), NULL);

    }
    else
    {
        defaultDevice =  CDCMMan::GetMe()->GetDefaultPrintServiceDevice();
        PRINTF("defaultDevice = %s defaultDevice.size() = %d\n",defaultDevice.c_str(),defaultDevice.size());
        if(defaultDevice.size() == 0)
        {
            m_errno = NODEFUALT;
        }

        else
        {

            if( FALSE == (CDCMMan::GetMe()->GetPrintServiceConfiguration((char *)defaultDevice.c_str(),element)))
            {
                m_errno = FAIL_TO_GET;
            }
            else
            {
                format = (Format)element.pcFormat;
                filmsize = (FilmSize)element.pcFilmSize;
                orientation = (ORIENTATION)element.pcOrientation;
            }
        }
        //Format format[] = {ONEXONE,ONEXTWO,TWOXTWO,TWOXTHREE,THREEXTHREE,THREEXFOUR,THREEXFIVE,FOURXFOUR,FOURXFIVE,FOURXSIX};
        for(int i = 0;i < vecPatExamID.size();i++)
            printf("vecPatExamID = %s\n",vecPatExamID[i].c_str());
        PRINTF("format = %d\n",format);
        PRINTF("filmsize = %d\n",filmsize);
        PRINTF("orientation = %d\n",orientation);
        PRINTF("bgColor = %d\n",bgColor);
        CreateWindow(vecPatExamID,format,filmsize,orientation,bgColor);
        //CreateWindow(vecPatExamID,FOURXSIX,IN10XIN12);

        PRINTF("exit ViewArchive.cpp : displayClicked\n");
    }
}

void DicomSendPrinter::DestroyDrawData()
{
    int i = 0;
    int size = 0;
    m_index = 0;
    size = m_rectangleInfo.size();

    for(i = 0;i < size;i++)
    {
        g_object_unref(m_rectangleInfo[i].gc);
        m_rectangleInfo[i].gc = NULL;

    }
    m_rectangleInfo.clear();
    size = m_drawImgInfo.size();
    for(i = 0;i < size;i++)
    {
        g_object_unref(m_drawImgInfo[i].imgPixBuf);
        m_drawImgInfo[i].imgPixBuf = NULL;
    }
    m_drawImgInfo.clear();
}

void DicomSendPrinter::DestroyImgData()
{
    int i = 0;
    int size = 0;
    m_index = 0;

    size = m_rectangleInfo.size();
    for(i = 0;i < m_total;i++)
    {
        m_ImageInfo[i].imageFilePath = "";
        m_ImageInfo[i].index = i;
    }

    if(size != 0)
    {

        for(i = 0;i <size;i++)
        {
            SetRectangleColor(i,g_deepGray);
        }
        SetRectangleColor(m_index,g_green);
        gtk_widget_queue_draw(m_drawArea);
    }

    size = m_drawImgInfo.size();
    for(i = 0;i < size;i++)
    {
        g_object_unref(m_drawImgInfo[i].imgPixBuf);
        m_drawImgInfo[i].imgPixBuf = NULL;
    }
    gtk_widget_queue_draw(m_drawArea);

}

void DicomSendPrinter::InitImageInfo(int index)
{
    float size;

    DrawImgInfo_t drawImgInfo;

    size = 	(float)m_rectangleInfo[index].width/(float)m_rectangleInfo[index].height;

    PRINTF("size = %f\n",(float)m_rectangleInfo[index].width/(float)m_rectangleInfo[index].height);

    if(size > (float)AREA_WIDTH/(float)AREA_HEIGHT)
    {
        drawImgInfo.height= m_rectangleInfo[index].height;
        drawImgInfo.width = m_rectangleInfo[index].height*((float)AREA_WIDTH/(float)AREA_HEIGHT);
        drawImgInfo.x = m_rectangleInfo[index].x + (m_rectangleInfo[index].width - drawImgInfo.width)/2;
        drawImgInfo.y = m_rectangleInfo[index].y;
    }
    else
    {
        drawImgInfo.width= m_rectangleInfo[index].width;
        drawImgInfo.height= m_rectangleInfo[index].width*((float)AREA_HEIGHT/(float)AREA_WIDTH);
        drawImgInfo.x = m_rectangleInfo[index].x;
        drawImgInfo.y = m_rectangleInfo[index].y+(m_rectangleInfo[index].height - drawImgInfo.height)/2;
    }
    drawImgInfo.imgPixBuf = NULL;
    m_drawImgInfo.push_back(drawImgInfo);
}

void DicomSendPrinter::DisplayImage(string fileName,int index)
{
    int i = 0;
    int size = 0;

    PRINTF("begin display image: fileName = %s\n", fileName.c_str());
    ImgMan::ImgItem imgItem;

    ImgMan::GetInstance()->ReadSnap(fileName.c_str(), &imgItem);
    if(m_drawImgInfo[index].imgPixBuf != NULL)
    {
        g_object_unref(m_drawImgInfo[index].imgPixBuf);
        m_drawImgInfo[index].imgPixBuf = NULL;
    }
    m_drawImgInfo[index].imgPixBuf = gdk_pixbuf_scale_simple(imgItem.pixbuf,m_drawImgInfo[index].width, m_drawImgInfo[index].height, GDK_INTERP_BILINEAR);

    g_object_unref(imgItem.pixbuf);
    gtk_widget_queue_draw(m_drawArea);
}

void DicomSendPrinter::HideAllBtn(void)
{
    int i;
    for (i = 0; i < SNAP_ROW * SNAP_COL; i ++)
    {
        gtk_widget_hide(m_vbox[i]);
    }
}

void DicomSendPrinter::UpdateImg(void)
{
    int i;
    int sumOfPage = SNAP_ROW * SNAP_COL;
    int size = m_vecFileName.size();
    ImgMan::ImgItem imgItem;
    GtkWidget *image;
    GdkPixbuf *pixbuf = NULL;
    gchar buf[20];

    // display current pageinfo
    if (size == 0)
        sprintf(buf, "%d/%d", m_curImgPage, m_maxImgPage);
    else
        sprintf(buf, "%d/%d", m_curImgPage+1, m_maxImgPage);
    gtk_label_set_text(GTK_LABEL(m_labImgPage), buf);

    HideAllBtn();

    // no image return
    if (size == 0)
        return;

    // display snap in current page
    for (i = 0; i < sumOfPage; i ++)
    {
        if ((m_curImgPage * sumOfPage + i) > (size-1))
            break;
        ImgMan::GetInstance()->ReadSnap(m_vecFileName[m_curImgPage*sumOfPage+i].c_str(), &imgItem);

        pixbuf = gdk_pixbuf_scale_simple(imgItem.pixbuf, SNAP_W*SCALE_WIDTH, SNAP_H*SCALE_HEIGHT, GDK_INTERP_BILINEAR);
        g_object_unref(imgItem.pixbuf);
        if (pixbuf != NULL)
        {
            image = gtk_image_new_from_pixbuf(pixbuf);
            gtk_button_set_image(GTK_BUTTON(m_btnSnap[i]), image);
            gtk_widget_show(m_vbox[i]);
            gchar *basename = g_path_get_basename(m_vecFileName[m_curImgPage*sumOfPage+i].c_str());
            gtk_widget_set_tooltip_text(m_vbox[i], basename);
            g_free(basename);

            g_object_unref(pixbuf);
        }
    }

#if 0
    // display check button
    for (i = 0; i < sumOfPage; i ++)
    {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkBtnSnap[i]), m_vecFileSel[m_curImgPage*sumOfPage +i]);
    }
#endif
}

void DicomSendPrinter::UpdateSnap(void)
{
    UpdateImg();
}
void DicomSendPrinter::HideAllFixed(void)
{
    if(GTK_IS_WIDGET(m_fixedContent))
        gtk_widget_hide_all(m_fixedContent);
}

void DicomSendPrinter::BtnSnapImgClicked(GtkButton* button)
{
    int id = atoi((char*)g_object_get_data(G_OBJECT(button), "id"));
    DCMPRINTIMAGEINFO imagInfo;

    int i = 0;
    int size = 0;

    BOOL isExsist = FALSE;
    //get button pressed
    id += m_curImgPage * (SNAP_COL * SNAP_ROW);
    m_idShowed = id;//
    // display large image
    PRINTF("!!!!name = %s\n",m_vecFileName[id].c_str());

    m_ImageInfo[m_index].index = m_index;
    m_ImageInfo[m_index].imageFilePath = m_vecFileName[id];

    DisplayImage(m_vecFileName[id],m_index);
    UpdateSnapColor();
}

void DicomSendPrinter::BtnLastPatClicked(GtkButton* button)
{
    int size = m_vecExamID.size();
    if (size == 0)
    {
        PRINTF("No patient is exist\n");
        return;
    }

    if (m_curPat <= 0)
    {
        PRINTF("this is the first one\n");
        return;
    }
    DestroyImgData();
    m_curPat --;
    UpdateCurPat(m_vecExamID[m_curPat], m_curPat, size, false);
}

void DicomSendPrinter::BtnNextPatClicked(GtkButton* button)
{
    int size = m_vecExamID.size();
    if (size == 0)
    {
        PRINTF("No patient is exist\n");
        return;
    }

    PRINTF("Exam ID vector's size is %d\n", size);
    if (m_curPat >= (size-1))
    {
        PRINTF("this is the last one\n");
        return;
    }
    DestroyImgData();
    m_curPat ++;
    UpdateCurPat(m_vecExamID[m_curPat], m_curPat, size, false);
}

void DicomSendPrinter::BtnLastImgPageClicked(GtkButton* button)
{
    int size;

    size = m_vecFileName.size();
    if (size == 0)
    {
        PRINTF("no image stored for this patient\n");
        return;
    }

    if (m_curImgPage > 0)
        m_curImgPage --;

    UpdateSnap();
    UpdateSnapColor();
}

void DicomSendPrinter::BtnNextImgPageClicked(GtkButton* button)
{
    int size;
    size = m_vecFileName.size();

    if (size == 0)
    {
        PRINTF("no image stored for this patient\n");
        return;

    }

    if (m_curImgPage < (m_maxImgPage - 1))
        m_curImgPage ++;

    UpdateSnap();
    UpdateSnapColor();
}

void DicomSendPrinter::HandleAllFormatArea(Format format,areaImgInfo_t positionSize,FilmSize filmsize)
{
    int i = 0;
    int remainder = 0;
    int multiple = 0;
    switch(format)
    {
        case ONEXONE:
            {
                m_rectanglePositionSize.width = positionSize.width;
                m_rectanglePositionSize.height =positionSize.height;
                m_rectanglePositionSize.x = 0;
                m_rectanglePositionSize.y = 0;
                m_multiple = 1;
                InitRectangle();
            }
            break;

        case ONEXTWO:
            {
                m_multiple = 2;
                for(i = 0;i < 2;i++)
                {

                    m_rectanglePositionSize.width = positionSize.width/2;

                    m_rectanglePositionSize.height = positionSize.height;
                    m_rectanglePositionSize.x = 0+i*positionSize.width/2;
                    m_rectanglePositionSize.y = 0;

                    InitRectangle();
                }
            }
            break;

        case TWOXTWO:
            {
                m_multiple = 2;
                for(i = 0;i < 2*2;i++)
                {
                    remainder = i%2;
                    multiple = i/2;
                    PRINTF("width = %d\n",positionSize.width);
                    PRINTF("height = %d\n",positionSize.height);

                    m_rectanglePositionSize.width = positionSize.width/2;
                    m_rectanglePositionSize.height = positionSize.height/2;

                    m_rectanglePositionSize.x = 0+remainder*positionSize.width/2;
                    m_rectanglePositionSize.y = 0+ multiple*positionSize.height/2;
                    InitRectangle();
                }
            }
            break;

        case TWOXTHREE:
            {
                m_multiple = 3;
                for(i = 0;i < 2*3;i++)
                {
                    remainder = i%3;
                    multiple = i/3;
                    m_rectanglePositionSize.width = positionSize.width/3;
                    m_rectanglePositionSize.height = positionSize.height/2;

                    m_rectanglePositionSize.x = 0+remainder*positionSize.width/3;
                    m_rectanglePositionSize.y = 0+ multiple*positionSize.height/2;
                    InitRectangle();
                }
            }
            break;

        case THREEXTHREE:
            {
                m_multiple = 3;
                for(i = 0;i < 3*3;i++)
                {
                    remainder = i%3;
                    multiple = i/3;
                    m_rectanglePositionSize.width = positionSize.width/3;
                    m_rectanglePositionSize.height = positionSize.height/3;

                    m_rectanglePositionSize.x = 0+remainder*positionSize.width/3;
                    m_rectanglePositionSize.y = 0+ multiple*positionSize.height/3;
                    InitRectangle();
                }
            }
            break;

        case THREEXFOUR:
            {
                m_multiple = 4;
                for(i = 0;i < 3*4;i++)
                {
                    remainder = i%4;
                    multiple = i/4;
                    m_rectanglePositionSize.width = positionSize.width/4;
                    m_rectanglePositionSize.height = positionSize.height/3;

                    m_rectanglePositionSize.x = 0+remainder*positionSize.width/4;
                    m_rectanglePositionSize.y = 0+ multiple*positionSize.height/3;
                    InitRectangle();
                }
            }
            break;

        case THREEXFIVE:
            {
                m_multiple = 5;
                for(i = 0;i < 3*5;i++)
                {
                    remainder = i%5;
                    multiple = i/5;
                    m_rectanglePositionSize.width = positionSize.width/5;
                    m_rectanglePositionSize.height = positionSize.height/3;

                    m_rectanglePositionSize.x = 0+remainder*positionSize.width/5;
                    m_rectanglePositionSize.y = 0+ multiple*positionSize.height/3;
                    InitRectangle();
                }
            }
            break;

        case FOURXFOUR:
            {
                m_multiple = 4;
                for(i = 0;i < 4*4;i++)
                {
                    remainder = i%4;
                    multiple = i/4;
                    m_rectanglePositionSize.width = positionSize.width/4;
                    m_rectanglePositionSize.height = positionSize.height/4;

                    m_rectanglePositionSize.x = 0+remainder*positionSize.width/4;
                    m_rectanglePositionSize.y = 0+ multiple*positionSize.height/4;
                    InitRectangle();
                }
            }
            break;

        case FOURXFIVE:
            {
                m_multiple = 5;
                for(i = 0;i < 4*5;i++)
                {
                    remainder = i%5;
                    multiple = i/5;
                    m_rectanglePositionSize.width = positionSize.width/5;
                    m_rectanglePositionSize.height = positionSize.height/4;

                    m_rectanglePositionSize.x = 0+remainder*positionSize.width/5;
                    m_rectanglePositionSize.y = 0+ multiple*positionSize.height/4;
                    InitRectangle();
                }
            }
            break;

        case FOURXSIX:
            {
                m_multiple = 6;
                for(i = 0;i < 4*6;i++)
                {
                    remainder = i%6;
                    multiple = i/6;
                    m_rectanglePositionSize.width = positionSize.width/6;
                    m_rectanglePositionSize.height = positionSize.height/4;

                    m_rectanglePositionSize.x = 0+remainder*positionSize.width/6;
                    m_rectanglePositionSize.y = 0+ multiple*positionSize.height/4;
                    InitRectangle();
                }
            }
            break;
    }
    return;
}

void DicomSendPrinter::DrawAreaExpose(GtkWidget *widget, GdkEventExpose *event)
{
    int size = 0;
    int i = 0;

    size = m_drawImgInfo.size();
    if(size != 0)
    {
        for(i = 0;i < size;i++)
        {
            if (m_drawImgInfo[i].imgPixBuf != NULL)
            {
                gdk_draw_pixbuf(widget->window,
                        widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
                        m_drawImgInfo[i].imgPixBuf,
                        0, 0,
                        m_drawImgInfo[i].x, m_drawImgInfo[i].y,
                        m_drawImgInfo[i].width, m_drawImgInfo[i].height,
                        GDK_RGB_DITHER_NORMAL,
                        0, 0);
            }
        }
    }

    size = m_rectangleInfo.size();
    if(size != 0)
    {
        for(i = 0;i < size;i++)
        {
            DrawRectangle(i);
        }
        DrawRectangle(m_index);
    }
}