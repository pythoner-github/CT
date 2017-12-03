// -*- c++ -*-
#ifndef _KNOB_MENU_H_
#define _KNOB_MENU_H_

#define WIDTH_KNOB_MENU 844 //ModLayout 844 PreLayout 804
#define HEIGHT_KNOB_MENU 60 //ModLayout 60 PreLayout 80

#include <gtk/gtk.h>
#include "Def.h"
#include "ManRegister.h"

class KnobMenu
{
public:
    ~KnobMenu(){ if (m_ptrInstance != NULL) delete m_ptrInstance; }
	
	typedef EKnobReturn (*pKnobFunc)(EKnobOper);
	typedef EKnobReturn (*pKnobFuncPress)(void);
	struct KnobItem{
        char *name;
        char value[20];
		EKnobReturn status;
		pKnobFunc pf;
		pKnobFuncPress pfPress;
	};

    enum EKnobType {NONE, D2, M, PW, CFM, REPLAY, SNAP, ARROW, BDMK, ANATOMIC_M, EFOV};

    EKnobType GetKnobType(void) { return m_knobType; }
    static KnobMenu* GetInstance();

    GtkWidget * Create(void);
    void SetItem(KnobItem *item, unsigned int total_item, EKnobType type);
    KnobItem* GetItem(void);
    void SetValue(int index, const char* value, EKnobReturn status);
    void Update(void); ///< update Knob menu
    void PageUp(void);
    void PageDown(void);

    void Knob1_Screw(int cw);
    void Knob2_Screw(int cw);
    void Knob3_Screw(int cw);
    void Knob4_Screw(int cw);
    void Knob5_Screw(int cw);
	
	void Knob1_Press(void);
    void Knob2_Press(void);
    void Knob3_Press(void);
    void Knob4_Press(void);
    void Knob5_Press(void);
    void Knob6_Press(void);


private:
    KnobMenu();

    static KnobMenu* m_ptrInstance;
#if (defined (EMP_322) || defined(EMP_313))
    static const int KNOB_NUM = 6;
#else
  static const int KNOB_NUM = 5;
#endif
    EKnobType m_knobType;
    void Refresh(void);
    void Display(KnobItem item, char *buf, const char *tmp);
    //void Display(KnobItem item, char *buf);

    GtkWidget *m_tableKnob;

    GtkWidget *m_labelLeft;
    GtkWidget *m_labelRight;

    GtkWidget *m_labelKnob[KNOB_NUM];
    GtkWidget *m_btnKnob[KNOB_NUM];
    unsigned int m_CurLevel;
    unsigned int m_MaxLevel;
    KnobItem *m_KnobItem;

//signal handle
//     void BtnLeftClicked(GtkButton *button);
//     void BtnRightClicked(GtkButton *button);
//signal connect
//     static void HandleBtnLeft(GtkButton *button, KnobMenu *data) { data->BtnLeftClicked(button); }
//     static void HandleBtnRight(GtkButton *button, KnobMenu *data) { data->BtnRightClicked(button); }

//for test signal
//     void KnobPress_1(GtkWidget *widget, GdkEventButton *event);
//     void KnobPress_2(GtkWidget *widget, GdkEventButton *event);
//     void KnobPress_3(GtkWidget *widget, GdkEventButton *event);
//     void KnobPress_4(GtkWidget *widget, GdkEventButton *event);
//     void KnobPress_5(GtkWidget *widget, GdkEventButton *event);

//     static gboolean HandleKnobPress_1(GtkWidget *widget, GdkEventButton *event, KnobMenu *data)
// 	{ data->KnobPress_1(widget, event); return FALSE; }
//     static gboolean HandleKnobPress_2(GtkWidget *widget, GdkEventButton *event, KnobMenu *data)
// 	{ data->KnobPress_2(widget, event); return FALSE; }
//     static gboolean HandleKnobPress_3(GtkWidget *widget, GdkEventButton *event, KnobMenu *data)
// 	{ data->KnobPress_3(widget, event); return FALSE; }
//     static gboolean HandleKnobPress_4(GtkWidget *widget, GdkEventButton *event, KnobMenu *data)
// 	{ data->KnobPress_4(widget, event); return FALSE; }
//     static gboolean HandleKnobPress_5(GtkWidget *widget, GdkEventButton *event, KnobMenu *data)
// 	{ data->KnobPress_5(widget, event); return FALSE; }

};


#endif
