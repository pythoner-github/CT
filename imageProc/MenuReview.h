// -*- c++ -*-
#ifndef MENU_REVIEW_H
#define MENU_REVIEW_H

#include <string>
#include <vector>
#include <ImgMan.h>
#include <VideoMan.h>
#include <ImageArea.h>
#include <TopArea.h>
#include <MenuArea.h>
#include <ViewSuperuser.h>


using std::string;
using std::vector;

class MenuReview
{
public:

	enum ReviewType { REVIEW_SNAP, REVIEW_VIDEO };

	static const int MAX_SNAP = 6;//8; //must greater-than or equal-to the number of rows
	static const int MAX_ROWS = 6;//4;
	static const int SNAP_W = 100;
	static const int SNAP_H = 80;
    
	vector<string> m_vecName;
	vector<string> m_vecNameSel;

    MenuReview();
    ~MenuReview() {}

    GtkWidget* Create(void);

    void Hide(void);
    void Show(void);
	void UpdatePreviewList(void);
	void UpdateLabel(void);

    EKnobReturn ChangeSnap(EKnobOper oper);
    void NextSnap();
	void PreviousSnap();
    void PlayNextVideoDemo(void);
    
	const char* GetReadFrm(){ return m_readFrm.c_str(); }
    void LoadVideoToReplay(void);
    void LoadSnapToReview(void);

	void ExitSlide(void);
    void ExitSlideDemo(void);
	EKnobReturn SlidePlay(EKnobOper oper);
	EKnobReturn SlidePlayDemo(void);
	EKnobReturn SlideSpeed(EKnobOper oper);
	void ShowSnapOneByOne(void);
	vector<string> GetSelName() { return m_vecNameSel; }
	ReviewType GetCurType() { return m_reviewType; }
	void SetCurType(ReviewType type) { m_reviewType = type; }
	void SetLimit(guchar limit) { m_limit = limit; }
	guchar GetLimit() { return m_limit; }
	void SetPatientID(unsigned int id);
	void SetImgPath(const char *path);
	void HideTooltips();
	void UpdatePreview(void);
	void UpdateSelectedColor(void);
	void SetPageValue(guint page, guint total_page);

	gint m_pageCur;
	gint m_numTotal;

    bool OnlyOneVideoDemo(void){return ((m_reviewType == REVIEW_VIDEO)&&(1 == m_numTotal));}
    GtkWidget* CurrentPreview() { return m_cur_btn; }
	void PreviewClicked(GtkButton *button);

private:

	string m_readFrm;	//abs path

	unsigned int m_folderID;
	const char *m_path;

	ImgMan *m_ptrImgMan;
	VideoMan *m_ptrVideoMan;
	ImageArea *m_ptrImgArea;
	TopArea *m_ptrTopArea;

    GtkWidget *m_cur_btn;
    GtkWidget *m_fixed;
	GtkWidget *m_hboxOper;
    GtkWidget *m_btnChgShow;
    GtkWidget *m_btnSend;
    GtkWidget *m_btnSelAll;
    GtkWidget *m_btnUnSel;
    GtkWidget *m_btnDel;
    GtkWidget *m_labelPage;
    GtkWidget *m_btnLeft;
    GtkWidget *m_btnRight;
    GtkWidget *m_vbox[MAX_SNAP];
    GtkWidget *m_btn[MAX_SNAP];
    GtkWidget *m_chkbtn[MAX_SNAP];
	char m_id[MAX_SNAP][4];
	ReviewType m_reviewType;
	gint m_idShowed;
	ReviewType m_typeShowed;
	GDir *m_dir;
	int m_imageFormat;
	int m_videoFormat;
	// slide
	/* m_limit=0: enable measure and delete 
	 * m_limit=1: enable measure but disable delete (show image from ViewArchiveImgMan-harddisk)
	 * m_limit=2: disable measure but enable delete (show image form ViewArchiveImgMan-udisk)
	 */
	guchar m_limit;		
	gint m_timer;
    gint m_timer2;
	gboolean m_slidePlay; 
	gint m_slideSpeed; //level
	static const int MAX_SLIDE_SPEED = 5;
	static const int SLIDE_SPEED[MAX_SLIDE_SPEED]; //ms

	GtkWidget* CreateItem(GtkWidget **button, GtkWidget **checkbutton, char *id);

	void HideAllButton(void);
	void InitSnapInfo(void);
	void LoadVideoInfo(void);
	void LoadSnapToScreen(const char* path);
	void LoadVideoToMem(const char* path);
    void UpdateSlideSpeed(int speed, EKnobReturn ret, bool update);
    void UpdateSlidePlay(bool on, EKnobReturn ret, bool update);
    void UpdateSnapIndex(int index, bool update);
	void InitKnobReview(void);

	void ChkBtnClicked(GtkButton *button);
	void ChangeShowClicked(GtkButton *button);
	void SendClicked(GtkButton *button);
	void UnSelectClicked(GtkButton *button);
	void SelAllClicked(GtkButton *button);
	void DelSelClicked(GtkButton *button);
	void PageUpClicked(GtkButton *button);
	void PageDownClicked(GtkButton *button);
    void PreviewNextOrPrevious(GtkButton *button);

    unsigned char LoadSnapDemo(unsigned int no, const char* filepath, vector<string> *vec);

	static void HandlePreviewClicked(GtkButton *button, MenuReview *data) 
     { 
        if(ViewSuperuser::GetInstance()->GetDemoStatus() && (data->m_reviewType == REVIEW_SNAP))
        {
            //do nothing
        }
        else
        {
            data->ExitSlide(); 
            data->PreviewClicked(button); 
        }
     }
	static void HandleChkBtnClicked(GtkButton *button, MenuReview *data) { data->ChkBtnClicked(button); }
	static void HandleChgShowClicked(GtkButton *button, MenuReview *data) { data->ChangeShowClicked(button); }
	static void HandleSendClicked(GtkButton *button, MenuReview *data) { data->SendClicked(button); }
	static void HandleUnSelectClicked(GtkButton *button, MenuReview *data) { data->UnSelectClicked(button); }
	static void HandleSelAllClicked(GtkButton *button, MenuReview *data) { data->SelAllClicked(button); }
	static void HandleDelSelClicked(GtkButton *button, MenuReview *data) { data->DelSelClicked(button); }
	static void HandlePageUpClicked(GtkButton *button, MenuReview *data) { data->PageUpClicked(button); }
	static void HandlePageDownClicked(GtkButton *button, MenuReview *data) { data->PageDownClicked(button); }
};

extern MenuReview g_menuReview;

#endif
