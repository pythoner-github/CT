#include <stdlib.h>
#include <algorithm>
#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <dirent.h>
#include <errno.h>

#include "MenuReview.h"
#include "ImgMan.h"
#include "ImageArea.h"
#include "TopArea.h"
#include "ViewMain.h"
#include "Def.h"
#include "gui_func.h"
#include "gui_global.h"
#include "PatientInfo.h"
#include "CreateAvi.h"
#include "Replay.h"
#include "FreezeMode.h"
#include "KnobNone.h"
#include "KnobLoadSnap.h"
#include "KnobReplay.h"
#include "MeasureMan.h"
#include "MeasureFactory.h"
#include "HintArea.h"
#include "ViewDialog.h"
#include "PeripheralMan.h"
#include "KeyFunc.h"
#include "UpdateBodyMark.h"
#include "ScanMode.h"
#include "Arrow.h"
#include "DCMMan.h"
#include "SysOptions.h"
#include "ImgMan.h"
#include "VideoMan.h"
#include "AviEncDecH264.h"
//#define SNAP_W	100
//#define SNAP_H	80

MenuReview g_menuReview;
static char LabelReviewType[][25] = { N_("Show Cine"), N_("Show Snap") };

// global
gboolean CallbackSlidePlay(gpointer data)
{
	g_menuReview.ShowSnapOneByOne();
	return TRUE;
}

gboolean DelayLoadVideoToMem(gpointer data)
{
	MenuReview *tmp = (MenuReview *)data;
	tmp->LoadVideoToReplay();
	return FALSE;
}

gboolean DelayLoadSnapToReview(gpointer data)
{
	MenuReview *tmp = (MenuReview *)data;
	tmp->LoadSnapToReview();
	return FALSE;
}

gboolean DelayPlayVideoDemo(gpointer data)
{
    FreezeMode::GetInstance()->EnterAutoReplayView();
	return FALSE;
}

// class
const int MenuReview::SLIDE_SPEED[MAX_SLIDE_SPEED] =
{
	500, 1000, 1500, 2000, 3000
};

MenuReview::MenuReview(void)
{
	m_ptrImgMan = ImgMan::GetInstance();
	m_ptrImgArea= ImageArea::GetInstance();
	m_ptrTopArea= TopArea::GetInstance();

    SysOptions so;
	m_imageFormat = so.GetImageFormat();
	m_videoFormat = so.GetCineFormat();


	m_fixed = NULL;
	m_labelPage = NULL;
	m_btnLeft = NULL;
	m_btnRight = NULL;
	m_vecName.clear();
	m_vecNameSel.clear();
	m_numTotal = 0;
	m_pageCur = 1;
	m_idShowed = -1;
	m_dir = NULL;
	m_reviewType = REVIEW_SNAP;
	m_limit = 0;
	m_timer = 0;
	m_slidePlay = FALSE;
	m_slideSpeed = 1;

	m_folderID  = 0;
	//m_path = (char*)STORE_PATH;
	m_cur_btn = 0;//m_btn[0];
    m_timer2 = 0;
}

void MenuReview::Hide(void)
{
	if(m_dir)
		g_dir_close(m_dir);
	m_dir = NULL;

    gtk_widget_hide_all(m_fixed);
	ExitSlide();
#ifdef EMP_355
	ExitSlideDemo();
#endif
	SetLimit(0);
	KeyReview::HideMenuReview();

    m_vecName.clear();
    m_vecNameSel.clear();
}

void MenuReview::Show(void)
{
	gtk_widget_show_all(m_fixed);
    if(ViewSuperuser::GetInstance()->GetDemoStatus()) //init demo para
    {
    	m_vecName.clear();
    	m_vecNameSel.clear();
    	m_numTotal = 0;
    	m_pageCur = 1;
    	m_idShowed = -1;
    	m_dir = NULL;
    	m_reviewType = REVIEW_SNAP;
    	m_limit = 0;
    	m_timer = 0;
    	m_slidePlay = FALSE;
    	m_slideSpeed = 1;
    	m_folderID  = 0;
    }
    else
    {
        m_idShowed = -1;
        m_vecNameSel.clear();
    }

	if(m_reviewType == REVIEW_SNAP)
	{
		KnobLoadSnapCreate();
		InitSnapInfo();
        if(ViewSuperuser::GetInstance()->GetDemoStatus()) //demo
        {
            SlidePlayDemo();
        }
	}
	else if(m_reviewType == REVIEW_VIDEO)
	{
		KnobNoneCreate();
		LoadVideoInfo();

        if(ViewSuperuser::GetInstance()->GetDemoStatus()) //demo
        {
            if (m_vecName.size() > 0)
            {
                m_idShowed = 0;
                UpdateSelectedColor();
        		FreezeMode::GetInstance()->ExitAutoReplay();
        		HintArea::GetInstance()->UpdateHint(_("Loading..."), 0);
        		g_timeout_add(100, DelayLoadVideoToMem, this);
        	    g_timeout_add(200, DelayPlayVideoDemo, this);
            }
        }
	}

    if(ViewSuperuser::GetInstance()->GetDemoStatus()) //demo
	{
		gtk_widget_set_sensitive(m_btnSend, false);
		gtk_widget_set_sensitive(m_btnSelAll, false);
		gtk_widget_set_sensitive(m_btnUnSel, false);
		gtk_widget_set_sensitive(m_btnDel, false);
	}
    else
    {
		gtk_widget_set_sensitive(m_btnSend, true);
		gtk_widget_set_sensitive(m_btnSelAll, true);
		gtk_widget_set_sensitive(m_btnUnSel, true);
		gtk_widget_set_sensitive(m_btnDel, true);
    }

	if(m_limit == 1)
	{
		gtk_widget_hide(m_btnSend);
		gtk_widget_hide(m_btnSelAll);
		gtk_widget_hide(m_btnUnSel);
		gtk_widget_hide(m_btnDel);
	}
}

void MenuReview::UpdatePreviewList(void)
{
	if(m_reviewType == REVIEW_SNAP)
    {
    	InitSnapInfo();
    }
	else if(m_reviewType == REVIEW_VIDEO)
    {
    	LoadVideoInfo();
    }
}

void MenuReview::UpdateLabel(void)
{
	gtk_widget_set_tooltip_text(m_btnChgShow, _(LabelReviewType[0]));
	gtk_widget_set_tooltip_text(m_btnSelAll, _("Select All"));
	gtk_widget_set_tooltip_text(m_btnUnSel, _("Deselect"));
	gtk_widget_set_tooltip_text(m_btnSend, _("Export to USB storage"));
	gtk_widget_set_tooltip_text(m_btnDel, _("Delete"));
}

//◀ ◁ ▶ ▷
GtkWidget* MenuReview::Create(void)
{
	int i, j;

    m_fixed = gtk_fixed_new ();
    gtk_widget_set_size_request (m_fixed, MENU_AREA_W, 730);

	GtkWidget *table = gtk_table_new (MAX_ROWS, (int)((float)MAX_SNAP/MAX_ROWS+0.5), TRUE);
    gtk_widget_set_size_request (table, MENU_AREA_W, 510); //H=MAX_ROWS*SNAP_H+(MAX_ROWS+1)*5
    gtk_fixed_put (GTK_FIXED (m_fixed), table, 0, 20);
	gtk_widget_show(table);

	int k = (int)((float)MAX_SNAP/MAX_ROWS+0.5);
	for(i=0; i<MAX_ROWS; i++)
	{
		for(j=0; j<k; j++)
		{
			sprintf(m_id[j+i*k], "%d", j+i*k);
			m_vbox[j+i*k] = CreateItem(&m_btn[j+i*k], &m_chkbtn[j+i*k], m_id[j+i*k]);
			gtk_table_attach_defaults(GTK_TABLE(table), m_vbox[j+i*k], j, j+1, i, i+1);
		}
	}

	m_hboxOper = gtk_hbox_new (TRUE, 0);
    gtk_fixed_put (GTK_FIXED (m_fixed), m_hboxOper, 0, 540);
    gtk_widget_set_size_request (m_hboxOper, MENU_AREA_W, 30);

#if 0
	GtkWidget *hbox_button2 = gtk_hbox_new (TRUE, 0);
    gtk_fixed_put (GTK_FIXED (m_fixed), hbox_button2, 0, 60);
    gtk_widget_set_size_request (hbox_button2, MENU_AREA_W, 30);
#endif

	GtkWidget *imgChgShow = gtk_image_new_from_stock(GTK_STOCK_REFRESH, GTK_ICON_SIZE_LARGE_TOOLBAR);
	m_btnChgShow = create_button_icon_only(imgChgShow, 0, 0, g_deep);
	gtk_button_set_relief(GTK_BUTTON(m_btnChgShow), GTK_RELIEF_NONE);
	gtk_widget_set_tooltip_text(m_btnChgShow, _(LabelReviewType[0]));
	gtk_box_pack_start (GTK_BOX (m_hboxOper), m_btnChgShow, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(m_btnChgShow), "clicked", G_CALLBACK(HandleChgShowClicked), this);

	GtkWidget *imgSelAll = gtk_image_new_from_stock(GTK_STOCK_APPLY, GTK_ICON_SIZE_LARGE_TOOLBAR);
	m_btnSelAll = create_button_icon_only(imgSelAll, 0, 0, g_deep);
	gtk_button_set_relief(GTK_BUTTON(m_btnSelAll), GTK_RELIEF_NONE);
	gtk_widget_set_tooltip_text(m_btnSelAll, _("Select All"));
	gtk_box_pack_start (GTK_BOX (m_hboxOper), m_btnSelAll, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(m_btnSelAll), "clicked", G_CALLBACK(HandleSelAllClicked), this);

	GtkWidget *imgDeselect = gtk_image_new_from_stock(GTK_STOCK_CLOSE, GTK_ICON_SIZE_LARGE_TOOLBAR);
	m_btnUnSel = create_button_icon_only(imgDeselect, 0, 0, g_deep);
	gtk_button_set_relief(GTK_BUTTON(m_btnUnSel), GTK_RELIEF_NONE);
	gtk_widget_set_tooltip_text(m_btnUnSel, _("Deselect"));
	gtk_box_pack_start (GTK_BOX (m_hboxOper), m_btnUnSel, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(m_btnUnSel), "clicked", G_CALLBACK(HandleUnSelectClicked), this);

	GtkWidget *imgSend = gtk_image_new_from_stock(GTK_STOCK_SAVE, GTK_ICON_SIZE_LARGE_TOOLBAR);
	m_btnSend = create_button_icon_only(imgSend, 0, 0, g_deep);
	gtk_button_set_relief(GTK_BUTTON(m_btnSend), GTK_RELIEF_NONE);
	gtk_widget_set_tooltip_text(m_btnSend, _("Export to USB storage"));
    gtk_box_pack_start (GTK_BOX (m_hboxOper), m_btnSend, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(m_btnSend), "clicked", G_CALLBACK(HandleSendClicked), this);

	GtkWidget *imgDel = gtk_image_new_from_stock(GTK_STOCK_DELETE, GTK_ICON_SIZE_LARGE_TOOLBAR);
	m_btnDel = create_button_icon_only(imgDel, 0, 0, g_deep);
	gtk_button_set_relief(GTK_BUTTON(m_btnDel), GTK_RELIEF_NONE);
	gtk_widget_set_tooltip_text(m_btnDel, _("Delete"));
	gtk_box_pack_start (GTK_BOX (m_hboxOper), m_btnDel, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(m_btnDel), "clicked", G_CALLBACK(HandleDelSelClicked), this);

    GtkWidget *hbox_scroll = gtk_hbox_new (TRUE, 0);
    gtk_fixed_put (GTK_FIXED (m_fixed), hbox_scroll, 0, 570);
    gtk_widget_set_size_request (hbox_scroll, MENU_AREA_W, 30);

//	GtkWidget *labelLeft = create_label("◀", 0, 0, g_white, NULL);
	GtkWidget *imgPrevious = gtk_image_new_from_stock(GTK_STOCK_MEDIA_PREVIOUS, GTK_ICON_SIZE_LARGE_TOOLBAR);
//	m_btnLeft = create_button(labelLeft, 60, 0, g_deep);
	m_btnLeft = create_button_icon_only(imgPrevious, 0, 0, g_deep);
	gtk_button_set_relief(GTK_BUTTON(m_btnLeft), GTK_RELIEF_NONE);
    gtk_box_pack_start (GTK_BOX (hbox_scroll), m_btnLeft, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(m_btnLeft), "clicked", G_CALLBACK(HandlePageUpClicked), this);
	gtk_widget_modify_bg(m_btnLeft, GTK_STATE_INSENSITIVE, g_deep);

    m_labelPage = gtk_label_new ("[ 0 / 0 ]");
    gtk_box_pack_start (GTK_BOX (hbox_scroll), m_labelPage, TRUE, TRUE, 0);

//	GtkWidget *labelRight = create_label("▶", 0, 0, g_white, NULL);
	GtkWidget *imgNext = gtk_image_new_from_stock(GTK_STOCK_MEDIA_NEXT, GTK_ICON_SIZE_LARGE_TOOLBAR);
//	m_btnRight = create_button(labelRight, 60, 0, g_deep);
	m_btnRight = create_button_icon_only(imgNext, 0, 0, g_deep);
	gtk_button_set_relief(GTK_BUTTON(m_btnRight), GTK_RELIEF_NONE);
    gtk_box_pack_start (GTK_BOX (hbox_scroll), m_btnRight, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(m_btnRight), "clicked", G_CALLBACK(HandlePageDownClicked), this);
	gtk_widget_modify_bg(m_btnRight, GTK_STATE_INSENSITIVE, g_deep);

	// init
	InitKnobReview();

    return m_fixed;
}

GtkWidget* MenuReview::CreateItem(GtkWidget **button, GtkWidget **checkbutton, char *id)
{
    GtkWidget *hbox = gtk_hbox_new (FALSE, 0);

    *button = gtk_button_new ();
    gtk_widget_set_size_request (*button, SNAP_W+10, -1);
    gtk_box_pack_start (GTK_BOX (hbox), *button, FALSE, FALSE, 15);
	g_signal_connect(G_OBJECT(*button), "clicked", G_CALLBACK(HandlePreviewClicked), this);
//	gtk_button_set_relief (GTK_BUTTON (*button), GTK_RELIEF_NONE);
	gtk_button_set_focus_on_click(GTK_BUTTON(*button), FALSE);
	g_object_set_data(G_OBJECT(*button), "id", id);

    *checkbutton = gtk_check_button_new_with_label ("");
	gtk_widget_set_size_request (*checkbutton, 40, 30);
	gtk_button_set_focus_on_click(GTK_BUTTON(*checkbutton), FALSE);
    gtk_box_pack_start (GTK_BOX (hbox), *checkbutton, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(*checkbutton), "clicked", G_CALLBACK(HandleChkBtnClicked), this);
	g_object_set_data(G_OBJECT(*checkbutton), "id", id);

    return hbox;
}

void MenuReview::UpdateSelectedColor()
{
	for(int i=0; i<MAX_SNAP; i++)
	{
		gtk_widget_modify_bg(m_btn[i], GTK_STATE_NORMAL, g_deepGray);
		gtk_widget_modify_bg(m_btn[i], GTK_STATE_PRELIGHT, g_deepGray);
	}

	if((m_typeShowed != m_reviewType))
	{
		return;
	}
	if(m_idShowed/MAX_SNAP == m_pageCur-1)
	{
		gtk_widget_modify_bg(m_btn[m_idShowed%MAX_SNAP], GTK_STATE_NORMAL, g_white);
		gtk_widget_modify_bg(m_btn[m_idShowed%MAX_SNAP], GTK_STATE_PRELIGHT, g_white);
	}
}

void MenuReview::LoadVideoToReplay(void)
{
	Replay::GetInstance()->EnterReadCine();
	LoadVideoToMem(m_vecName[m_idShowed].c_str());
	Replay::GetInstance()->PrepareForReplay();
    if(ViewSuperuser::GetInstance()->GetDemoStatus()) //init demo para
    {
        Replay::GetInstance()->ViewOneImg();
    }
    else
    {
    	Replay::GetInstance()->ReviewOneImg();
    }

	FreezeMode::GetInstance()->EnterReplayMode();
	HintArea::GetInstance()->ClearHint();
}

void MenuReview::LoadSnapToReview(void)
{
    PRINTF("%s: load snap file %s\n", __FUNCTION__, m_vecName[m_idShowed].c_str());
    LoadSnapToScreen(m_vecName[m_idShowed].c_str());
}


void MenuReview::SetPatientID(unsigned int id)
{
	m_folderID = id;
	PRINTF("---------menureview: id = %d\n", id);
}
void MenuReview::SetImgPath(const char *path)
{
	m_path = (char*)path;
}

/*
 *  隐藏Tooltips，仅用于存储图片时使用
 *  当Tooltip显示时存储图片会导致图像数据被Tooltip覆盖，因此在存储图片时需要先隐藏Tooltip，存储完成后需要重新设置Tooltip
 *  由于存储图片后会调用UpdatePreviewList刷新列表，同时会重设Tooltip，但全选、删除等按钮未重设，因此需要在完成存储后调用UpdateLabel进行重设
 */
void MenuReview::HideTooltips()
{
	for(int i=0; i<MAX_SNAP; i++)
	{
		gtk_widget_set_tooltip_text(m_btn[i], NULL);
	}

	gtk_widget_set_has_tooltip(m_btnChgShow, NULL);
	gtk_widget_set_has_tooltip(m_btnSelAll, NULL);
	gtk_widget_set_has_tooltip(m_btnUnSel, NULL);
	gtk_widget_set_has_tooltip(m_btnSend, NULL);
	gtk_widget_set_has_tooltip(m_btnDel, NULL);
}

void MenuReview::UpdateSlideSpeed(int speed, EKnobReturn ret, bool update)
{
	char buf[10];
	sprintf(buf, "%d", speed);
	SyncKnobReview(REVIEW_SLIDE_SPEED, buf, ret, update);
}

void MenuReview::UpdateSlidePlay(bool on, EKnobReturn ret, bool update)
{
	const char * toggle[2] =
	{
		N_("OFF"),
		N_("ON")
	};

	SyncKnobReview(REVIEW_SLIDE_PLAY, toggle[(int)on], ret, update);
}

void MenuReview::UpdateSnapIndex(int index, bool update)
{
    EKnobReturn ret;
    if(index == (m_vecName.size() - 1))
        ret = MAX;
    else if(index == 0)
        ret = MIN;
    else
        ret = OK;
	char buf[10];
	sprintf(buf, "%d", (index+1));
	SyncKnobReview(REVIEW_NEXT, buf, ret, update);
}

void MenuReview::InitKnobReview()
{
	EKnobReturn ret = OK;

	if (m_slidePlay)
		ret = MAX;
	else
		ret = MIN;
	UpdateSlidePlay(m_slidePlay, ret, FALSE);

	if (m_slideSpeed == 0)
		ret = MIN;
	else if (m_slideSpeed == (MAX_SLIDE_SPEED - 1))
		ret = MAX;
	else
		ret = OK;
	UpdateSlideSpeed(m_slideSpeed, ret, FALSE);
}

void MenuReview::ExitSlide(void)
{
	// close timer
	if (m_timer > 0)
	{
		if (!g_source_remove(m_timer))
        {
            PRINTF("remove timer error\n");
        }
		m_timer = 0;
	}
	// off slide play
	if (m_slidePlay)
	{
		m_slidePlay = FALSE;
		UpdateSlidePlay(m_slidePlay, MIN, TRUE);
	}

	gtk_widget_show(m_btnLeft);
	gtk_widget_show(m_btnRight);

	gtk_widget_show_all(m_hboxOper);
	if(m_limit == 1)
	{
		gtk_widget_hide(m_btnSend);
		gtk_widget_hide(m_btnSelAll);
		gtk_widget_hide(m_btnUnSel);
		gtk_widget_hide(m_btnDel);
	}
}

void MenuReview::ExitSlideDemo(void)
{
	// close timer
	if (m_timer2 > 0)
	{
		if (!g_source_remove(m_timer2))
        {
            PRINTF("remove timer error\n");
        }
		m_timer2 = 0;
	}
	// off slide play
	if (m_slidePlay)
	{
		m_slidePlay = FALSE;
		UpdateSlidePlay(m_slidePlay, MIN, TRUE);
	}

	gtk_widget_show(m_btnLeft);
	gtk_widget_show(m_btnRight);

	gtk_widget_show_all(m_hboxOper);
	if(m_limit == 1)
	{
		gtk_widget_hide(m_btnSend);
		gtk_widget_hide(m_btnSelAll);
		gtk_widget_hide(m_btnUnSel);
		gtk_widget_hide(m_btnDel);
	}
}

bool review_pic =false;
void MenuReview::PreviewClicked(GtkButton *button)
{
    review_pic = true;

	m_cur_btn = GTK_WIDGET(button);
//	PRINTF("%s\n", __FUNCTION__);
	if(!m_slidePlay)
		ExitSlide();

	if(g_ptrAbsMeasure)
		delete g_ptrAbsMeasure;
	g_ptrAbsMeasure = NULL;

	/* 此处仅需要清除数据，不要进行刷新操作，由LoadSnapToScreen决定是否需要刷新 */

	MeasureMan::GetInstance()->DeleteAllForClearScreen(FALSE);	//update operation must be after the LoadFrm() function
    ImageArea::GetInstance()->ClearScreen(FALSE);	//update operation must be after the LoadFrm() function
	CUpdateBodyMark cub;
	cub.HideBodyMark();

    int id = (m_pageCur-1) * MAX_SNAP + atoi((char*)(g_object_get_data(G_OBJECT(button), "id")));
    m_idShowed = id;
	m_typeShowed = m_reviewType;
	UpdateSelectedColor();
    UpdateSnapIndex(m_idShowed, TRUE);

	if(m_reviewType == REVIEW_SNAP)
	{
		FreezeMode::GetInstance()->ExitAutoReplay();
        Replay::GetInstance()->EnterReadSnap();
        if (m_timer2 > 0)
        {
            if (!g_source_remove(m_timer2))
            {
                PRINTF("remove timer error\n");
            }
            m_timer2 = 0;
        }

        m_timer2 = g_timeout_add(100, DelayLoadSnapToReview, this);

		//LoadSnapToScreen(m_vecName[m_idShowed].c_str());
		//PRINTF("%s: load snap file %s\n", __FUNCTION__, m_vecName[m_idShowed].c_str());
	}
	else if(m_reviewType == REVIEW_VIDEO)
	{
		FreezeMode::GetInstance()->ExitAutoReplay();
		HintArea::GetInstance()->UpdateHint(_("Loading..."), 0);
		g_timeout_add(100, DelayLoadVideoToMem, this);
        if(ViewSuperuser::GetInstance()->GetDemoStatus())
        {
		    g_timeout_add(200, DelayPlayVideoDemo, this);
        }
	}
}

void MenuReview::PlayNextVideoDemo(void)
{
	if(m_idShowed+1 >= m_numTotal)
	{
		m_idShowed = -1;
		m_pageCur = 1;
		SetPageValue(m_pageCur, (int)((float)m_numTotal/MAX_SNAP+0.9999));
		HideAllButton();
		UpdatePreview();
	}
	NextSnap();
}

void MenuReview::LoadSnapToScreen(const char* path)
{
	ImgMan::ImgItem item;
	int width, height;

	if(ImgMan::GetInstance()->CheckFileName(path) != 1)
	{
		PRINTF("%s: Failed to load snap to screen!\n", __FUNCTION__);
		return;
	}

    item.pixbuf = NULL;
    if(ViewSuperuser::GetInstance()->GetDemoStatus())
    {
        item.pixbuf = gdk_pixbuf_new_from_file(path, NULL);
        if(item.pixbuf == NULL)
            return;
    }
    else
    {
    	if(ImgMan::GetInstance()->ReadSnap(path, &item) != 0)
    	{
    		return;
    	}
    }

	//设置图像参数
	ScanMode::GetInstance()->EnterSpecialMeasure(item.para);

	//clear arrow and measure data
	CArrow::DeleteAllForFreeze();
	MeasureMan::GetInstance()->DeleteAllForClearScreen(FALSE); //此处不可进行刷新操作

    if(!ViewSuperuser::GetInstance()->GetDemoStatus())
    {
    	char frm_path[256];
    	FileMan fm;
    	fm.GetFrmFilePath(path, frm_path);
    	m_readFrm = frm_path;
        PRINTF("----frm path %s\n", frm_path);
    }

//	pixels = gdk_pixbuf_get_pixels(item.pixbuf);
//	rowstride = gdk_pixbuf_get_rowstride(item.pixbuf);
//	n_channels = gdk_pixbuf_get_n_channels(item.pixbuf);
//	p =	pixels + IMAGE_Y * rowstride + IMAGE_X * n_channels;
	width = gdk_pixbuf_get_width(item.pixbuf);
	height = gdk_pixbuf_get_height(item.pixbuf);

	//默认显示待字符的图像，存在Frm图像先加载好，待进入测量后通过刷新将Frm图像刷新出来，从而去掉字符信息
	m_ptrTopArea->DrawSnap(item.pixbuf, 0, 0, width, TOP_AREA_H);
	m_ptrImgArea->DrawSnap(item.pixbuf, 0, IMG_AREA_Y, width, height-IMG_AREA_Y);
    if(!ViewSuperuser::GetInstance()->GetDemoStatus())
    {
       if(m_limit != 2)
        {
            PRINTF("---m_limit is not equal 2\n");
    		ImageArea::GetInstance()->LoadFrm();
        }
    }

	g_object_unref(item.pixbuf);
}

void MenuReview::LoadVideoToMem(const char* path)
{
	int i;
	VideoMan::VideoItem item;
    if (m_ptrVideoMan->ReadVideoInfo(path, &item) != 0)
		return;
	free(item.data);

    if (item.format == VideoMan::AVI)
    {
        Replay::GetInstance()->SetReplayFps(item.frame_rate);
        ReadAviData(item.path, NULL,  item.frames, item.width, item.height,VideoMan::SaveReplay);
    }
    else
    {

        FILE* fs = fopen(item.path, "rb");
        if(fs==NULL)
        {
            perror("fopen error!");
            return;
        }

        Replay::GetInstance()->SetReplayFps(item.frame_rate);

        int frame_size = item.width * item.height * 3;
        guchar buf[frame_size];
        for(i=0; i<item.frames; i++)
        {
            if(!m_ptrVideoMan->ReadVideoData(fs, buf, item.format, i, item.width, item.height))
            {
                PRINTF("ReadVideoData Error!\n");
                break;
            }

            Image::AutoTracePara para;
            Image image((unsigned int*)buf, para);
            Replay::GetInstance()->AddOneCineImg(image);
        }

        fclose(fs);
    }
}

void MenuReview::ChkBtnClicked(GtkButton *button)
{
	int id = (m_pageCur-1) * MAX_SNAP + atoi((char*)(g_object_get_data(G_OBJECT(button), "id")));

	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button)))
	{
		vector<string>::iterator iter = find(m_vecNameSel.begin(), m_vecNameSel.end(), m_vecName[id]);
		if(iter == m_vecNameSel.end())	//not found and add it to the vector
			m_vecNameSel.push_back(m_vecName[id]);
	}
	else
	{
		vector<string>::iterator iter = find(m_vecNameSel.begin(), m_vecNameSel.end(), m_vecName[id]);
		if(iter != m_vecNameSel.end())
			m_vecNameSel.erase(iter);	//found and erase it from the vector
	//	else
	//		m_vecNameSel.clear();
	}
}

void MenuReview::ChangeShowClicked(GtkButton *button)
{
	ExitSlide();
	m_idShowed = -1;

	if(m_reviewType == REVIEW_SNAP)
	{
		gtk_widget_set_tooltip_text(GTK_WIDGET(button), _(LabelReviewType[1]));
		m_reviewType = REVIEW_VIDEO;
		LoadVideoInfo();
		if(m_idShowed >= 0 && m_typeShowed == REVIEW_VIDEO)
			KnobReplayCreate();
		else
		{
            if (ModeStatus::IsAutoReplayMode())
				KnobCineCreate();
			else
				KnobNoneCreate();
		}
        if ((m_vecName.size() > 0) && (ViewSuperuser::GetInstance()->GetDemoStatus())) //demo
        {
            m_idShowed = 0;
            UpdateSelectedColor();
            HintArea::GetInstance()->UpdateHint(_("Loading..."), 0);
            g_timeout_add(150, DelayLoadVideoToMem, this);
            g_timeout_add(200, DelayPlayVideoDemo, this);
            KnobReplayCreate();
        }

	}
	else if(m_reviewType == REVIEW_VIDEO)
	{
		gtk_widget_set_tooltip_text(GTK_WIDGET(button), _(LabelReviewType[0]));
		m_reviewType = REVIEW_SNAP;
		KnobLoadSnapCreate();
		InitSnapInfo();
        if(ViewSuperuser::GetInstance()->GetDemoStatus()) //demo
        {
            SlidePlayDemo();
        }
	}
	m_vecNameSel.clear();
}

static GCancellable* cancellable = NULL;

static void progress_callback(goffset current, goffset total, gpointer data)
{
#if 1
	if(g_cancellable_is_cancelled(cancellable))
		return;

	double prac = (double)current/total;
//	PRINTF("prac = %f\n", prac);
	if(prac >= 0 && prac <= 1.0)
	{
		gdk_threads_enter();
		ViewDialog::GetInstance()->SetProgressBar(prac);
		while(gtk_events_pending())
			gtk_main_iteration();
		gdk_threads_leave();
	}
	else
		PRINTF("fraction out of range!\n");
#endif
}

#if 0
static gchar *strSrc, *strDest;

void* ThreadCopy (void* argv)
{
	CalcTime ct;
	ct.Begin();

	PRINTF("copy %s to %s\n", strSrc, strDest);

	FileMan fm;
	if(fm.CopyFile(strSrc, strDest)<0)
	{
		PRINTF("ShellCopyFile failed!\n");
	}
	g_free(strSrc);
	g_free(strDest);

	PeripheralMan::GetInstance()->UmountUsbStorage();
	ct.End();
}
#endif

static gboolean SendToFlash(gpointer data)
{
	int cond = 0;
	int count = 1;
	int total = 0;
	gchar str_info[256], result[256];
	gchar *curPath = NULL;
	vector<string> vecName;
	PeripheralMan *ptr = PeripheralMan::GetInstance();

	if(!ptr->CheckUsbStorageState())
	{
		ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()),
				ViewDialog::ERROR,
				_("No USB storage found!"),
				NULL);
		return FALSE;
	}
	else
	{
		if(!ptr->MountUsbStorage())
		{
			ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()),
					ViewDialog::ERROR,
					_("Failed to mount USB storage!"),
					NULL);
			return FALSE;
		}
	}

	gchar *path = g_build_path(G_DIR_SEPARATOR_S, UDISK_DATA_PATH, "0", NULL);
	GFile *dir = g_file_new_for_path(path);
	g_free(path);
	GError *err_mkdir = NULL;
	if(!g_file_make_directory_with_parents(dir, NULL, &err_mkdir))
	{
		if(err_mkdir->code!=G_IO_ERROR_EXISTS)
		{
			PRINTF("g_file_make_directory %s error: %s\n", g_file_get_path(dir), err_mkdir->message);
			ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()),
					ViewDialog::ERROR,
					_("Failed to export to USB storage!\nError: Failed to create directory."),
					NULL);
			g_object_unref(dir);
			return FALSE;
		}
	}
	g_object_unref(dir);

	vecName = g_menuReview.GetSelName();
	total = vecName.size();
	cancellable = g_cancellable_new();

	int n = 2;

	FileMan fm;
	vector<string>::iterator ite = vecName.begin();
	while (ite < vecName.end() && !cond) {
		for(int i=0; i<n; i++)
		{
			gchar name[256];
			switch(i)
			{
				case 0:	//BMP,JPG,EMP
					sprintf(name , "%s", (*ite).c_str());
					break;
				case 1:	//INI
					fm.GetIniFilePath((*ite).c_str(), name);
					break;
				default:
					sprintf(name , "%s", (*ite).c_str());
					break;
			}

			sprintf(str_info, "%s %s   %d/%d\n%s", _("Exporting"), name, count, total, _("Please wait..."));
			ViewDialog::GetInstance()->SetText(str_info);
			ViewDialog::GetInstance()->SetProgressBar(0);

			gchar *basename = g_path_get_basename(name);
			gchar *destPath = g_build_filename(UDISK_DATA_PATH, "0", basename, NULL);
			PRINTF("copy %s to %s\n", name, destPath);
			curPath = g_strdup(destPath);

			GFile *src = g_file_new_for_path(name);
			GFile *dest = g_file_new_for_path(destPath);

			g_free(basename);
			g_free(destPath);

			GError *err = NULL;
			int ret = g_file_copy(src, dest, G_FILE_COPY_OVERWRITE, cancellable, progress_callback, NULL, &err);
			g_object_unref(src);
			g_object_unref(dest);
			if(!ret)
			{
				PRINTF("g_file_copy error: %s\n", err->message);
				switch(err->code)
				{
					case G_IO_ERROR_NO_SPACE:
						sprintf(result, _("Failed to export to USB storage!\nError: No space left on storage."));
						break;
					case G_IO_ERROR_CANCELLED:
						sprintf(result, _("Failed to export to USB storage!\nError: Operation was cancelled."));
						break;
					default:
						sprintf(result, _("Failed to export to USB storage!"));
						break;
				}
				cond = -1;
				g_error_free(err);
				break; //break for loop
			}
			g_free(curPath);
		}
		if(cond == -1)
			break; //break while loop
		count++;
		ite++;
	}

	ViewDialog::GetInstance()->Destroy();

	if(!cond)
	{
		gchar *path = g_path_get_basename(UDISK_DATA_PATH);
		gchar *storePath = g_build_filename(path, "0", NULL);
		sprintf(result, "%s %s", _("Success to export to USB storage.\nPath: "), storePath);
		g_free(path);
		g_free(storePath);
	}
	else
	{
		if(g_file_test(curPath, G_FILE_TEST_IS_REGULAR))
		{
			if(g_unlink(curPath) < 0)
				PRINTF("delete file %s error: %s\n", curPath, strerror(errno));
		}
		g_free(curPath);
	}

	ptr->UmountUsbStorage();

	ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()),
			ViewDialog::INFO,
			result,
			NULL);

	return FALSE;
}

static int CancelCopyToFlash(gpointer data)
{
	PRINTF("Cancel copy!\n");
	g_cancellable_cancel(cancellable);
	return 0;
}

void MenuReview::SendClicked(GtkButton *button)
{
	int size = m_vecNameSel.size();

	if(size == 0)
	{
		ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()),
				ViewDialog::ERROR,
				_("Please select the wanted image/cine from the list to be exported!"),
				NULL);
		return;
	}

	g_timeout_add(1000, SendToFlash, NULL);

	ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()),
			ViewDialog::PRG_CANCEL,
			_("Please wait..."),
			CancelCopyToFlash);
}

void MenuReview::UnSelectClicked(GtkButton *button)
{
	int i, j;
	int size = m_vecName.size();
	int nth = (m_pageCur-1) * MAX_SNAP;

	for(i=nth, j=0; i<size; i++, j++)
	{
		if(j >= MAX_SNAP)
			break;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_chkbtn[i-nth]), FALSE);
	}
	m_vecNameSel.clear();
}

void MenuReview::SelAllClicked(GtkButton *button)
{
	int i, j;
	int size = m_vecName.size();
	int nth = (m_pageCur-1) * MAX_SNAP;

	for(i=nth, j=0; i<size; i++, j++)
	{
		if(j >= MAX_SNAP)
			break;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_chkbtn[i-nth]), TRUE);
	}
	m_vecNameSel = m_vecName;
}

void MenuReview::DelSelClicked(GtkButton *button)
{
	int i;
	int size = m_vecNameSel.size();
	vector<string>::iterator iter;

	//delete all file in selecting vector
	for(i=0; i<size; i++)
    {
        m_ptrImgMan->DeleteSnap(m_vecNameSel[i].c_str());
        string strName = m_vecNameSel[i];
        int length = strlen(strName.c_str());
        int index = strName.find_last_of("/");
        string nameTmp = strName.substr(index+1,length-index-1);
        CDCMMan::GetMe()->DeleteImageBeforeEndStudy(nameTmp);
    }

    UpdatePreviewList();

	//delete all from vector stored filename selected
	m_vecNameSel.clear();

	m_idShowed = -1;
	UpdateSelectedColor();
}

void MenuReview::PageUpClicked(GtkButton *button)
{
    if (m_vecName.size() < 1)
    {
        HintArea::GetInstance()->UpdateHint(_("No Image to play."), 2);
        return;
    }

    m_pageCur --;
    SetPageValue(m_pageCur, (int)((float)m_numTotal/MAX_SNAP+0.9999));
    HideAllButton();
    UpdatePreview();
    UpdateSelectedColor();
}

void MenuReview::PageDownClicked(GtkButton *button)
{
    if (m_vecName.size() < 1)
    {
        HintArea::GetInstance()->UpdateHint(_("No Image to play."), 2);
        return;
    }

    m_pageCur ++;
    SetPageValue(m_pageCur, (int)((float)m_numTotal/MAX_SNAP+0.9999));
    HideAllButton();
	UpdatePreview();
	UpdateSelectedColor();
}

void MenuReview::PreviewNextOrPrevious(GtkButton *button)
{
    review_pic = true;

	m_cur_btn = GTK_WIDGET(button);
//	PRINTF("%s\n", __FUNCTION__);
	if(!m_slidePlay)
		ExitSlide();

	if(g_ptrAbsMeasure)
		delete g_ptrAbsMeasure;
	g_ptrAbsMeasure = NULL;

	/* 此处仅需要清除数据，不要进行刷新操作，由LoadSnapToScreen决定是否需要刷新 */
	ImageArea::GetInstance()->ClearScreen(FALSE);	//update operation must be after the LoadFrm() function
	MeasureMan::GetInstance()->DeleteAllForClearScreen(FALSE);	//update operation must be after the LoadFrm() function

    m_typeShowed = m_reviewType;
	UpdateSelectedColor();
    UpdateSnapIndex(m_idShowed, TRUE);

	if(m_reviewType == REVIEW_SNAP)
	{
		FreezeMode::GetInstance()->ExitAutoReplay();
        Replay::GetInstance()->EnterReadSnap();
        if (m_timer2 > 0)
        {
            if (!g_source_remove(m_timer2))
            {
                PRINTF("remove timer error\n");
            }
            m_timer2 = 0;
        }

        m_timer2 = g_timeout_add(100, DelayLoadSnapToReview, this);

		//LoadSnapToScreen(m_vecName[m_idShowed].c_str());
		//PRINTF("%s: load snap file %s\n", __FUNCTION__, m_vecName[m_idShowed].c_str());
	}
	else if(m_reviewType == REVIEW_VIDEO)
	{
		FreezeMode::GetInstance()->ExitAutoReplay();
		HintArea::GetInstance()->UpdateHint(_("Loading..."), 0);
		g_timeout_add(100, DelayLoadVideoToMem, this);
        if(ViewSuperuser::GetInstance()->GetDemoStatus())
        {
		    g_timeout_add(200, DelayPlayVideoDemo, this);
        }
	}

}


EKnobReturn MenuReview::SlidePlay(EKnobOper oper)
{
	EKnobReturn ret = OK;

    if(m_reviewType != REVIEW_SNAP)
        return ERROR;

    if (m_vecName.size() < 1)
    {
        HintArea::GetInstance()->UpdateHint(_("No Image to play."), 2);
        return ERROR;
    }
    if(oper == ROTATE)
    {
        if(!m_slidePlay)
            m_slidePlay = TRUE;
        else
            m_slidePlay = FALSE;
    }
    else if (oper == ADD)
    {
        if (!m_slidePlay)
            m_slidePlay = TRUE;
        else
            return MAX;
    }
    else if (oper == SUB)
    {
        if (m_slidePlay)
            m_slidePlay = FALSE;
        else
            return MIN;
    }
    else
    {
        return ERROR;
    }

	// on or off
    if (m_slidePlay)
        ret = MAX;
    else
        ret = MIN;
	UpdateSlidePlay(m_slidePlay, ret, TRUE);
	if (m_slidePlay)
	{
		ret = MAX;
        if (m_timer > 0)
        {
            if (!g_source_remove(m_timer))
            {
                PRINTF("remove timer error\n");
            }
            m_timer = 0;
        }

        Replay::GetInstance()->EnterReadSnap();
        m_timer = g_timeout_add(SLIDE_SPEED[m_slideSpeed], CallbackSlidePlay, NULL);
        gtk_widget_hide(m_btnLeft);
        gtk_widget_hide(m_btnRight);
        gtk_widget_hide_all(m_hboxOper);
    }
    else
    {
		ret = MIN;
		ExitSlide();
#if 0
		if (m_timer > 0)
		{
			g_source_remove(m_timer);
			m_timer = 0;
		}
#endif
	}

	return ret;
}


EKnobReturn MenuReview::SlidePlayDemo(void)
{
    if (m_vecName.size() < 1)
    {
        HintArea::GetInstance()->UpdateHint(_("No Image to play."), 2);
        return ERROR;
    }

    m_slidePlay = TRUE;
	UpdateSlidePlay(m_slidePlay, MAX, TRUE);
    UpdateSlideSpeed(m_slideSpeed, OK, TRUE);
    if(m_timer > 0)g_source_remove(m_timer);
    m_timer = g_timeout_add(SLIDE_SPEED[m_slideSpeed], CallbackSlidePlay, NULL);
    gtk_widget_hide(m_btnLeft);
    gtk_widget_hide(m_btnRight);
	return MAX;
}


EKnobReturn MenuReview::SlideSpeed(EKnobOper oper)
{
	EKnobReturn ret = OK;

	if(m_reviewType != REVIEW_SNAP)
		return ERROR;
  if (oper == ROTATE)
	{
		if(m_slideSpeed < (MAX_SLIDE_SPEED -1))
		{
			m_slideSpeed ++;
		}
		else
            m_slideSpeed = 0;
	}
	else if (oper == ADD)
	{
		if (m_slideSpeed < (MAX_SLIDE_SPEED - 1))
			m_slideSpeed ++;
		else
			return MAX;
	}
	else if (oper == SUB)
	{
		if (m_slideSpeed > 0)
			m_slideSpeed --;
		else
			return MIN;
	}
	else
	{
		return ERROR;
	}

	if (m_slideSpeed == (MAX_SLIDE_SPEED - 1))
		ret = MAX;
	else if (m_slideSpeed == 0)
		ret = MIN;
	else
		ret = OK;

	UpdateSlideSpeed(m_slideSpeed, ret, TRUE);

	if(m_slidePlay)
	{
		if (m_timer > 0)
		{
			if (!g_source_remove(m_timer))
			{
				PRINTF("remove timer error\n");
			}
			m_timer = 0;
		}

		m_timer = g_timeout_add(SLIDE_SPEED[m_slideSpeed], CallbackSlidePlay, NULL);
	}

	return ret;
}
void MenuReview::ShowSnapOneByOne(void)
{
	if(m_idShowed+1 >= m_numTotal)
	{
		m_idShowed = -1;
		m_pageCur = 1;
		SetPageValue(m_pageCur, (int)((float)m_numTotal/MAX_SNAP+0.9999));
		HideAllButton();
		UpdatePreview();
	}
	NextSnap();
}

void MenuReview::SetPageValue(guint page, guint total_page)
{
	char str[20];

	sprintf(str, "[ %d / %d ]", page, (total_page==0)?1:total_page);
	gtk_label_set_text(GTK_LABEL(m_labelPage), str);

	if(page <= 1 && total_page <= 1)
	{
		gtk_widget_set_sensitive (m_btnLeft, FALSE);
		gtk_widget_set_sensitive (m_btnRight, FALSE);
	}
	else if(page == 1 && total_page > 1)
	{
		gtk_widget_set_sensitive (m_btnLeft, FALSE);
		gtk_widget_set_sensitive (m_btnRight, TRUE);
	}
	else if(page > 1 && total_page > page)
	{
		gtk_widget_set_sensitive (m_btnLeft, TRUE);
		gtk_widget_set_sensitive (m_btnRight, TRUE);
	}
	else if(page > 1 && total_page == page)
	{
		gtk_widget_set_sensitive (m_btnLeft, TRUE);
		gtk_widget_set_sensitive (m_btnRight, FALSE);
	}
}

void MenuReview::HideAllButton()
{
	int i;
	for(i=0; i<MAX_SNAP; i++)
	{
		if(GTK_WIDGET_VISIBLE(m_vbox[i]))
			gtk_widget_hide(m_vbox[i]);
	}
}

void MenuReview::UpdatePreview()
{
	int i, j;
	int nth, size;
	char tmp[10];
	GtkWidget *image;
	GdkPixbuf *pixbuf = NULL;
	ImgMan *ptrIM = ImgMan::GetInstance();
	VideoMan *ptrVM = VideoMan::GetInstance();
	ImgMan::ImgItem img_item;
	VideoMan::VideoItem video_item;

	size = m_vecName.size();
	nth = (m_pageCur-1) * MAX_SNAP;
	for(i=nth, j=0; i<size; i++, j++)
	{
		if(j >= MAX_SNAP)
			break;
        img_item.pixbuf = NULL;
		if(m_reviewType == REVIEW_SNAP)
		{
            if(ViewSuperuser::GetInstance()->GetDemoStatus())
            {
                img_item.pixbuf = gdk_pixbuf_new_from_file(m_vecName[i].c_str(), NULL);
                if(!img_item.pixbuf)continue;
            }
            else
            {
    			if(ptrIM->ReadSnap(m_vecName[i].c_str(), &img_item) != 0)
    			{
    				continue;
    			}
            }

			pixbuf = gdk_pixbuf_scale_simple(img_item.pixbuf, SNAP_W, SNAP_H, GDK_INTERP_BILINEAR);
			g_object_unref(img_item.pixbuf);
		}
		else if(m_reviewType == REVIEW_VIDEO)
		{
            if(ptrVM->ReadVideoInfo(m_vecName[i].c_str(), &video_item) != 0)
            {
                continue;
            }

			GdkPixbuf *pb = gdk_pixbuf_new_from_data(video_item.data, GDK_COLORSPACE_RGB,
					false, 8, video_item.width, video_item.height, video_item.width*3, NULL, NULL);
			pixbuf = gdk_pixbuf_scale_simple(pb, SNAP_W, SNAP_H, GDK_INTERP_BILINEAR);
			g_object_unref(pb);
            free(video_item.data);
            //g_free(video_item.data);
		}

		if(pixbuf)
		{
			image = gtk_image_new();
			gtk_widget_show(image);
			gtk_image_set_from_pixbuf(GTK_IMAGE(image), pixbuf);
			g_object_unref(pixbuf);
		}
		else
		{
			PRINTF("gdk_pixbuf_scale_simple error!\n");
			continue;
		}
		gtk_button_set_image(GTK_BUTTON(m_btn[j]), image);

        if(!ViewSuperuser::GetInstance()->GetDemoStatus())
        {
    	//	gtk_widget_set_tooltip_text(m_btn[j], m_vecName[i].c_str());
    		gchar *basename = g_path_get_basename(m_vecName[i].c_str());
    		gtk_widget_set_tooltip_text(m_btn[j], basename);
    		g_free(basename);
        }
		sprintf(tmp, "%d", i+1);
		gtk_label_set_text(GTK_LABEL(gtk_bin_get_child(GTK_BIN(m_chkbtn[i-nth]))), tmp);

		vector<string>::iterator iter = find(m_vecNameSel.begin(), m_vecNameSel.end(), m_vecName[i]);
		if(iter != m_vecNameSel.end())
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_chkbtn[i-nth]), 1);
		else
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_chkbtn[i-nth]), 0);

		gtk_widget_show(m_vbox[j]);
	}
}



/*
 * @brief load all image filename to vector by patient's id
 *
 * @param no: patient id
 * @param filepath: file path (eg: STORE_PATH)
 * @param vec(out): vector which to store the filename (only file name, path is not included)
 *
 * @retval 0: success
 *		   1: can not open the dir
 *		   2: the path is empty
 */
unsigned char MenuReview::LoadSnapDemo(unsigned int no, const char* filepath, vector<string> *vec)
{
	char *path;
	char tmp[10];
	string str;
	const char *name;
	GDir *dir;
	GError *err = NULL;

	sprintf(tmp, "%d", no);
	path = g_build_filename(filepath, tmp, NULL);
	dir = g_dir_open(path, 0, &err);
	g_free(path);
	if(!dir)
	{
		PRINTF("%s: g_dir_open with error: %s\n", __FUNCTION__, err->message);
		return 1;
	}

	name = g_dir_read_name(dir);
	if(!name)
	{
		PRINTF("%s: the dir is null!\n", __FUNCTION__);
		g_dir_close(dir);
		return 2;
	}
    FileMan fm;
	while(name != NULL)
	{
		//PRINTF("name is %s\n", name);
		if(!fm.CompareSuffix(name, "bmp") || !fm.CompareSuffix(name, "emp") || !fm.CompareSuffix(name, "jpg"))
        {
			str = name;
			vec->push_back(str);
        }

		name = g_dir_read_name(dir);
	}

	g_dir_close(dir);
	return 0;
}

void MenuReview::InitSnapInfo(void)
{
	unsigned int i;
	char *path, sid[10];
	string str;

	HideAllButton();
	m_vecName.clear();

	if(m_limit!=2)
	{
        if(ViewSuperuser::GetInstance()->GetDemoStatus())
        {
    		LoadSnapDemo(m_folderID, m_path, &m_vecName);
	    }
        else
        {
    		m_ptrImgMan->LoadSnap(m_folderID, m_path, &m_vecName);
        }

		for (i = 0; i < m_vecName.size(); i ++)
		{
			sprintf(sid, "%d", m_folderID);
			path = g_build_filename(m_path, sid, m_vecName[i].c_str(), NULL);
			m_vecName[i] = path;
			g_free(path);
		}
	}
	else	//Load all from SLIDE_PATH
	{
		struct dirent *ent;
		DIR *dir = opendir(SLIDE_PATH);
		if(dir)
		{
			vector<string> vec_tmp;
			while( (ent = readdir(dir)) != NULL)
			{
				PRINTF("readdir: %s\n", ent->d_name);
				if(ent->d_name[0]=='.')
					continue;
				//	if(ent->d_type==DT_DIR)
				gchar *absPath = g_build_path(G_DIR_SEPARATOR_S, SLIDE_PATH, ent->d_name, NULL);
				if(g_file_test(absPath, G_FILE_TEST_IS_DIR))
				{
					//get file name by directory name
					vec_tmp.clear();
#if 0
                    if (strcmp(ent->d_name, "3D") == 0) //LL 2013-06-21
                    {
                        ImgMan::GetInstance()->LoadSnap(absPath, &vec_tmp);
                    }
                    else
                    {
					    ImgMan::GetInstance()->LoadSnap(atoi(ent->d_name), SLIDE_PATH, &vec_tmp);
                    }
#endif
                    ImgMan::GetInstance()->LoadSnap(absPath, &vec_tmp);

                    //get full path file name to dest vector
					for (i = 0; i < vec_tmp.size(); i++)
					{
						path = g_build_filename(SLIDE_PATH, ent->d_name, vec_tmp[i].c_str(), NULL);
						m_vecName.push_back(path);
						g_free(path);
					}
				}
				g_free(absPath);
			}
			closedir(dir);
		}
		else
			perror("Failed to open slide path: ");
	}

	//sort list by filename
	sort(m_vecName.begin(), m_vecName.end(), FileMan::SortByMTime);
#if 1
	m_pageCur = 1;
	m_numTotal = m_vecName.size();
	SetPageValue(m_pageCur, (int)((float)m_numTotal/MAX_SNAP+0.9999));
	UpdatePreview();
	UpdateSelectedColor();
    UpdateSnapIndex(m_idShowed, TRUE);
#endif
}

void MenuReview::LoadVideoInfo()
{
	unsigned int i;
	char *path, sid[10];
	string str;

	HideAllButton();
	m_vecName.clear();

	if(m_limit!=2)
	{
		m_ptrVideoMan->LoadVideo(m_folderID, m_path, &m_vecName);
		for (i = 0; i < m_vecName.size(); i ++)
		{
			sprintf(sid, "%d", m_folderID);
			path = g_build_filename(m_path, sid, m_vecName[i].c_str(), NULL);
			m_vecName[i] = path;
			g_free(path);
		}
	}
	else	//Load all from SLIDE_PATH
	{
		struct dirent *ent;
		DIR *dir = opendir(SLIDE_PATH);
		vector<string> vec_tmp;

		while( (ent = readdir(dir)) != NULL)
		{
			if(ent->d_name[0]=='.')
				continue;
		//	if(ent->d_type==DT_DIR)
			gchar *absPath = g_build_path(G_DIR_SEPARATOR_S, SLIDE_PATH, ent->d_name, NULL);
			if(g_file_test(absPath, G_FILE_TEST_IS_DIR))
			{
				//get file name by directory name
				vec_tmp.clear();
				VideoMan::GetInstance()->LoadVideo(atoi(ent->d_name), SLIDE_PATH, &vec_tmp);
				//get full path file name
				for (i = 0; i < vec_tmp.size(); i ++)
				{
					path = g_build_filename(SLIDE_PATH, ent->d_name, vec_tmp[i].c_str(), NULL);
					m_vecName.push_back(path);
					g_free(path);
				}
			}
			g_free(absPath);
		}
		closedir(dir);
	}

	//sort list by filename
	sort(m_vecName.begin(), m_vecName.end(), FileMan::SortByMTime);

	m_pageCur = 1;
	m_numTotal = m_vecName.size();
	SetPageValue(m_pageCur, (int)((float)m_numTotal/MAX_SNAP+0.9999));
	UpdatePreview();
	UpdateSelectedColor();
}

EKnobReturn MenuReview::ChangeSnap(EKnobOper oper)
{
    EKnobReturn ret = ERROR;
    if (m_vecName.size() < 1)
    {
        HintArea::GetInstance()->UpdateHint(_("No Image to play."), 2);
        return ret;
    }

    if(oper == ADD)
    {
        if(m_idShowed < m_numTotal - 1)
            m_idShowed++;
        else
            return MAX;
    }
    else if(oper == SUB)
    {
        if(m_idShowed > 0)
            m_idShowed--;
        else
            return MIN;
    }
    else if(oper == ROTATE)
    {
        if(m_idShowed < m_numTotal - 1)
            m_idShowed++;
        else
            m_idShowed = 0;
    }

    if(oper == SUB)
    {
        if((m_idShowed+1)%MAX_SNAP == 0)
        {
            m_pageCur = (m_idShowed+1) / MAX_SNAP;
            SetPageValue(m_pageCur, (int)((float)m_numTotal/MAX_SNAP+0.9999));
            HideAllButton();
            UpdatePreview();
        }
    }
    else
    {
        if(m_idShowed%MAX_SNAP == 0)
        {
            m_pageCur = (m_idShowed+1) / MAX_SNAP + 1;
            SetPageValue(m_pageCur, (int)((float)m_numTotal/MAX_SNAP+0.9999));
            HideAllButton();
            UpdatePreview();
        }
    }
    PreviewNextOrPrevious(GTK_BUTTON(m_btn[(m_idShowed)%MAX_SNAP]));

    return ret;
}

void MenuReview::NextSnap()
{
    if (m_vecName.size() < 1)
    {
        HintArea::GetInstance()->UpdateHint(_("No Image to play."), 2);
        return;
    }

    if(m_idShowed+1 < m_numTotal)
	{
		if(m_idShowed%MAX_SNAP == MAX_SNAP-1)
		{
			m_pageCur ++;
			SetPageValue(m_pageCur, (int)((float)m_numTotal/MAX_SNAP+0.9999));
			HideAllButton();
			UpdatePreview();
		}
    	PreviewClicked(GTK_BUTTON(m_btn[(m_idShowed+1)%MAX_SNAP]));
	}
    else
    {
		m_pageCur = 1;
		SetPageValue(m_pageCur, (int)((float)m_numTotal/MAX_SNAP+0.9999));
		HideAllButton();
		UpdatePreview();
    	PreviewClicked(GTK_BUTTON(m_btn[0]));
    }

}

void MenuReview::PreviousSnap()
{
    if (m_vecName.size() < 1)
    {
        HintArea::GetInstance()->UpdateHint(_("No Image to play."), 2);
        return;
    }

    if(m_idShowed-1 >= 0 )
	{
		if(m_idShowed%MAX_SNAP==0)
		{
			m_pageCur --;
			SetPageValue(m_pageCur, (int)((float)m_numTotal/MAX_SNAP+0.9999));
		//	SetPageValue(m_pageCur, (m_numTotal/MAX_SNAP)+1);

			HideAllButton();
			UpdatePreview();
		}
		PreviewClicked(GTK_BUTTON(m_btn[(m_idShowed-1)%MAX_SNAP]));
	}
}
