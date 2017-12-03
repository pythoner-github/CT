#ifndef _MENUEFOV_H_
#define _MENUEFOV_H_

class MenuEFOV
{
public:
    MenuEFOV();
    ~MenuEFOV() {}
    void Hide(void);
    void Show(void);
    void ShowView(void);
    void ShowPrepare(void);
    void ShowCapture(void);
    void ShowReview(void);

    GtkWidget *Create(void);
    
    void UpdateLabel(void);
private:
    GtkWidget *m_table;
    GtkWidget *m_fixedPrepare;
    GtkWidget *m_fixedCapture;
    GtkWidget *m_labelPress;
    GtkWidget *m_labelPress2;
    GtkWidget *m_labelPress3;
    GtkWidget *m_labelPrepareHint;
    GtkWidget *m_labelCaptureHint;
    GtkWidget *m_labelCaptureHint2;

//signal handle
    // void BtnReview(GtkButton *button);
    // void BtnZoom(GtkButton *button);
    // void BtnRotate(GtkButton *button);
//signal connect
    // static void HandleBtnReview(GtkButton *button, MenuEFOV *data) { data->BtnReview(button); }
    // static void HandleBtnZoom(GtkButton *button, MenuEFOV *data) { data->BtnZoom(button); }
    // static void HandleBtnRotate(GtkButton *button, MenuEFOV *data) { data->BtnRotate(button); }
};

extern MenuEFOV g_menuEFOV;

#endif /* _MENUEFOV_H_ */
