// -*- c++ -*-
#ifndef __MENU_AREA_H__
#define __MENU_AREA_H__

#define MENU_AREA_H 705 //ModLayout 705 PreLayout 688
#define MENU_AREA_W 180 //ModLayout 180 PreLayout 220

#include <gtk/gtk.h>

class MenuArea
{
public:
    ~MenuArea(){ if (m_ptrInstance != NULL) delete m_ptrInstance; }
    static MenuArea* GetInstance();

// enum EMenuType {MAIN, D2, M, PW, CW, CFM, PDI, PWCFM, CWCFM, MEASURE, BDMK, REVIEW, CALC, NOTE, BIOPSY, SYSTEM, EFOV};

#ifdef EMP_PROJECT
    enum EMenuType {MAIN, D2, M, PW, CW, CFM, PDI, PWCFM, CWCFM, MEASURE, BDMK, REVIEW, CALC, NOTE, BIOPSY, SYSTEM, EFOV,PROJECTMODE, BIOPSYBRACKET,BIOPSYVERIFY};
#else
  enum EMenuType {MAIN, D2, M, PW, CW, CFM, PDI, PWCFM, CWCFM, MEASURE, BDMK, REVIEW, CALC, NOTE, BIOPSY, SYSTEM, EFOV,BIOPSYBRACKET,BIOPSYVERIFY};
#endif
void ShowEFOVPrepare(void);
    void ShowEFOVCapture(void);
    void ShowEFOVView(void);
    void ShowEFOVReview(void);

    EMenuType GetMenuType(void);
    bool GetMenuVisible() { return m_menuVisbible; }
    void SwitchMenu(EMenuType type);
    EMenuType GetNotebookType(void);

    GtkWidget* Create(void);
	void ShowMenu(void);
	void HideMenu(void);
    void UpdateLabel(void);
    void UpdateSubMenu(void);
    void ShowMainMenu(void);
    void Show2DMenu(void);
    void ShowMMenu(void);
    void ShowPWMenu(void);
    void ShowCWMenu(void);
    void ShowCFMMenu(void);
    void ShowCWCFMMenu(bool currentCw);
    void ShowPWCFMMenu(bool currentPw);
    void ShowMeasureMenu(void);
    void ShowBDMKMenu(void);
    void ShowReviewMenu(void);
    void ShowCalcMenu(void);
    void ShowNoteMenu(void);
    void ShowBiopsyMenu(void);
    void ShowSystemMenu(void);

#ifdef EMP_PROJECT
    void ShowProjectModeMenu(void);
    void HideProjectModeMenu(void);
#endif

    void GetMenuIndex(int &index);
	void ShowBioBracketMenu(void);
	void ShowBioVerifyMenu(void);
private:
    MenuArea();
    static MenuArea *m_ptrInstance;

	void HideCurMenuChild(void);

	EMenuType m_menuType;

#ifdef EMP_PROJECT
	EMenuType m_menuTypePre;
#endif

   bool m_menuVisbible;

    GtkWidget *m_tableMenu;
    GtkWidget *m_fixedMenu;
    GtkWidget *m_labelSub;
    GtkWidget *m_btnExit;

    GtkWidget *m_noteBook;
    GtkWidget *m_label2D;
    GtkWidget *m_labelM;
    GtkWidget *m_labelPW;
    GtkWidget *m_labelCW;
    GtkWidget *m_labelCFM;
    GtkWidget *m_labelEFOV;
    bool m_in2DOnly;
//signal handle
    void MainMenuClk(GtkButton *button);
    void NotebookChanged(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num);
//signal connect
    static void HandleNotebookChanged(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, MenuArea *data)
	{ data->NotebookChanged(notebook, page, page_num); }

    void HideAllOtherMenu(void);
};

extern void MenuShowUndo();

#endif