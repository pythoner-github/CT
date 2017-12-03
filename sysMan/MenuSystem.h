#ifndef _MENU_SYSTEM_H_
#define _MENU_SYSTEM_H_


#define WIDTH_SYSTEM_MENU  220
#define HEIGHT_SYSTEM_MENU  635

class MenuSystem {
public:
    MenuSystem();
    ~MenuSystem() {}
    GtkWidget * Create(void);
    void UpdateLabel(void);
    void Show(void);
    void Hide(void);
private:
    GtkWidget *m_table;
    GtkWidget *m_labelGeneral;
    GtkWidget *m_labelBiopsy;

//signal handle
    void BtnGeneral(GtkButton *button);
    void BtnBiopsy(GtkButton *button);
//signal connect
    static void HandleBtnGeneral(GtkButton *button, MenuSystem *data) { data->BtnGeneral(button); }
    static void HandleBtnBiopsy(GtkButton *button, MenuSystem *data) { data->BtnBiopsy(button); }
};

extern MenuSystem g_menuSystem;
#endif
