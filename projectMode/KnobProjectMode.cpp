#include <gtk/gtk.h>
#include <string.h>
#include "display/gui_func.h"
#include "display/gui_global.h"
#include "imageProc/GlobalClassMan.h"
#include "projectMode/KnobProjectMode.h"
#include "display/KnobMenu.h"
#include "projectMode/ProjectCalc2D.h"
#ifdef EMP_PROJECT

#if 1
EKnobReturn D2ChgDemodSection(EKnobOper oper)
{
    return ProjectCalc2D::GetInstance()->ChangeDemodSection(oper);
}

EKnobReturn D2PressChgDemodParaPrintf()
{
    return ProjectCalc2D::GetInstance()->ChangeDemodParaPrintf();
}

EKnobReturn D2ChgBandPassFc(EKnobOper oper)
{
    return ProjectCalc2D::GetInstance()->ChangeBandPassFc(oper);
}

EKnobReturn D2ChgTxFNum(EKnobOper oper)
{
    return ProjectCalc2D::GetInstance()->ChangeTxFNum(oper);
}

EKnobReturn D2ChgWpScale(EKnobOper oper)
{
    return ProjectCalc2D::GetInstance()->ChangeWpScale(oper);
}

EKnobReturn D2Chg2DPulse(EKnobOper oper)
{
    return ProjectCalc2D::GetInstance()->Change2DPulse(oper);
}

EKnobReturn D2ChgPwPulse(EKnobOper oper)
{
    return ProjectCalc2D::GetInstance()->ChangePwPulse(oper);
}

EKnobReturn D2ChgCfmPulse(EKnobOper oper)
{
    return ProjectCalc2D::GetInstance()->ChangeCfmPulse(oper);
}

EKnobReturn D2ChgProbeR(EKnobOper oper)
{
    return ProjectCalc2D::GetInstance()->ChangeProbeR(oper);
}

EKnobReturn D2ChgProbeAngle(EKnobOper oper)
{
    return ProjectCalc2D::GetInstance()->ChangeProbeAngle(oper);
}

#if 0
EKnobReturn D2ChgBPWSimult(EKnobOper oper)
{
    //ProjectCalc2D::GetInstance()->ChangeBandPassW(oper);
    return ProjectCalc2D::GetInstance()->ChangeBandPassFc(oper);
}

EKnobReturn D2ChgBandPassWindowFunc(EKnobOper oper)
{
    return ProjectCalc2D::GetInstance()->ChangeBandPassWindowFunc(oper);
}

EKnobReturn D2ChgLowPassWindowFunc(EKnobOper oper)
{
    return ProjectCalc2D::GetInstance()->ChangeLowPassWindowFunc(oper);
}

EKnobReturn D2ChgEmission(EKnobOper oper)
{
    return ProjectCalc2D::GetInstance()->ChangeEmissionPara(oper);
}
#endif
#endif

KnobProjectMode::KnobItem KnobProjectMode::KnobProjectMenu[KNOB_MENU_SIZE] =  //KnobProjectMode::KNOB_NUM*3
{
    {_("Demod Section"), "", FALSE, MIN, D2ChgDemodSection, D2PressChgDemodParaPrintf},
    {_("BandPass FC"), "", FALSE, MIN, D2ChgBandPassFc, NULL},
    {_("TxFNum"), "", FALSE, MIN, D2ChgTxFNum, NULL},
    {_("WpScale"), "", FALSE, MIN, D2ChgWpScale, NULL},
    {_("2D Pulse"), "", FALSE, MIN, D2Chg2DPulse, NULL},
    {_("PW Pulse"), "", FALSE, MIN, D2ChgPwPulse, NULL},
    {_("CFM Pulse"), "", FALSE, MIN, D2ChgCfmPulse, NULL},
    {_("Probe Radio"), "", FALSE, MIN, D2ChgProbeR, NULL},
    {_("Probe Angle"), "", FALSE, MIN, D2ChgProbeAngle, NULL},
//    {_("BPW Simult"), "", FALSE, MIN, D2ChgBPWSimult, NULL},
//    {_("BandPass WinFunc"), "", FALSE, MIN, D2ChgBandPassWindowFunc, NULL},
  //  {_("LowPass WinFunc"), "", FALSE, MIN, D2ChgLowPassWindowFunc, NULL},
   // {_("Emission"), "", FALSE, MIN, D2ChgEmission, NULL},
    //{"", "", FALSE, ERROR, NULL, NULL},
    {"", "", FALSE, ERROR, NULL, NULL},
};

void KnobProjectMode::SetPrint(int seq, bool value)
{
    KnobProjectMenu[seq].isPrint = value;
}

bool KnobProjectMode::GetPrint(int seq)
{
    return KnobProjectMenu[seq].isPrint;
}

KnobProjectMode* KnobProjectMode::m_ptrInstance = NULL;
KnobProjectMode* KnobProjectMode::GetInstance()
{
    if (m_ptrInstance == NULL)
    m_ptrInstance  = new KnobProjectMode;
    return m_ptrInstance;
}

KnobProjectMode::KnobProjectMode(void)
{
    m_tableKnob = 0;
    m_knobType = D2;
    m_isShow = false;
}

KnobProjectMode::~KnobProjectMode()
{
    if (m_ptrInstance != NULL)
    {
        delete m_ptrInstance;
        m_ptrInstance = NULL;
    }
}
void KnobProjectMode::SetItem(KnobItem *item, unsigned int total_item, EKnobType type)
{
    m_KnobItem = item;
    if ((total_item % KNOB_NUM) == 0)
        m_MaxLevel = (total_item/ KNOB_NUM) - 1;
    else
        m_MaxLevel = total_item/ KNOB_NUM;
    m_CurLevel = 0;
    m_knobType = type;
    m_isShow = true;
    Update();
}

KnobProjectMode::KnobItem* KnobProjectMode::GetItem(void)
{
    return m_KnobItem;
}

GtkWidget * KnobProjectMode::Create(void)
{
    m_tableKnob = gtk_table_new(1, 22, TRUE);
    gtk_widget_set_usize(m_tableKnob, WIDTH_KNOB, HEIGHT_KNOB);

    m_labelLeft = create_label("≪", 0, 22, g_lightGray, NULL);
    gtk_widget_modify_fg(m_labelLeft, GTK_STATE_PRELIGHT, g_lightGray);
    gtk_widget_modify_fg(m_labelLeft, GTK_STATE_ACTIVE, g_lightGray);
    GtkWidget *btn_left = create_button(m_labelLeft, 0, 22, g_deep);
    gtk_table_attach_defaults(GTK_TABLE(m_tableKnob), btn_left, 0, 1, 0, 1);
    GTK_WIDGET_UNSET_FLAGS (btn_left, GTK_CAN_FOCUS);
    gtk_widget_modify_bg(btn_left, GTK_STATE_PRELIGHT, g_deep);
    gtk_widget_modify_bg(btn_left, GTK_STATE_ACTIVE, g_deep);
    gtk_widget_modify_bg(btn_left, GTK_STATE_INSENSITIVE, g_deepGray);

    m_labelKnob[0] = create_label("knob1", 0, 22, g_lightGray, NULL);
    gtk_label_set_justify(GTK_LABEL(m_labelKnob[0]), GTK_JUSTIFY_CENTER);
    gtk_widget_modify_fg(m_labelKnob[0], GTK_STATE_PRELIGHT, g_lightGray);
    gtk_widget_modify_fg(m_labelKnob[0], GTK_STATE_ACTIVE, g_lightGray);
    m_btnKnob[0] = create_button(m_labelKnob[0], 0, 22, g_deep);
    gtk_table_attach_defaults(GTK_TABLE(m_tableKnob), m_btnKnob[0], 1, 5, 0, 1);
    GTK_WIDGET_UNSET_FLAGS (m_btnKnob[0], GTK_CAN_FOCUS);
    gtk_widget_modify_bg(m_btnKnob[0], GTK_STATE_PRELIGHT, g_deep);
    gtk_widget_modify_bg(m_btnKnob[0], GTK_STATE_ACTIVE, g_deep);
    gtk_widget_modify_bg(m_btnKnob[0], GTK_STATE_INSENSITIVE, g_deepGray);

    m_labelKnob[1] = create_label("knob2", 0, 22, g_lightGray, NULL);
    gtk_widget_modify_fg(m_labelKnob[1], GTK_STATE_PRELIGHT, g_lightGray);
    gtk_widget_modify_fg(m_labelKnob[1], GTK_STATE_ACTIVE, g_lightGray);
    gtk_label_set_justify(GTK_LABEL(m_labelKnob[1]), GTK_JUSTIFY_CENTER);
    m_btnKnob[1] = create_button(m_labelKnob[1], 0, 22, g_deep);
    gtk_table_attach_defaults(GTK_TABLE(m_tableKnob), m_btnKnob[1], 5, 9, 0, 1);
    GTK_WIDGET_UNSET_FLAGS (m_btnKnob[1], GTK_CAN_FOCUS);
    gtk_widget_modify_bg(m_btnKnob[1], GTK_STATE_PRELIGHT, g_deep);
    gtk_widget_modify_bg(m_btnKnob[1], GTK_STATE_ACTIVE, g_deep);
    gtk_widget_modify_bg(m_btnKnob[1], GTK_STATE_INSENSITIVE, g_deepGray);

    m_labelKnob[2] = create_label("knob3", 0, 22, g_lightGray, NULL);
    gtk_widget_modify_fg(m_labelKnob[2], GTK_STATE_PRELIGHT, g_lightGray);
    gtk_widget_modify_fg(m_labelKnob[2], GTK_STATE_ACTIVE, g_lightGray);
    gtk_label_set_justify(GTK_LABEL(m_labelKnob[2]), GTK_JUSTIFY_CENTER);
    m_btnKnob[2] = create_button(m_labelKnob[2], 0, 22, g_deep);
    gtk_table_attach_defaults(GTK_TABLE(m_tableKnob), m_btnKnob[2], 9, 13, 0, 1);
    GTK_WIDGET_UNSET_FLAGS (m_btnKnob[2], GTK_CAN_FOCUS);
    gtk_widget_modify_bg(m_btnKnob[2], GTK_STATE_PRELIGHT, g_deep);
    gtk_widget_modify_bg(m_btnKnob[2], GTK_STATE_ACTIVE, g_deep);
    gtk_widget_modify_bg(m_btnKnob[2], GTK_STATE_INSENSITIVE, g_deepGray);

    m_labelKnob[3] = create_label("knob4", 0, 22, g_lightGray, NULL);
    gtk_widget_modify_fg(m_labelKnob[3], GTK_STATE_PRELIGHT, g_lightGray);
    gtk_widget_modify_fg(m_labelKnob[3], GTK_STATE_ACTIVE, g_lightGray);
    gtk_label_set_justify(GTK_LABEL(m_labelKnob[3]), GTK_JUSTIFY_CENTER);
    m_btnKnob[3] = create_button(m_labelKnob[3], 0, 22, g_deep);
    gtk_table_attach_defaults(GTK_TABLE(m_tableKnob), m_btnKnob[3], 13, 17, 0, 1);
    GTK_WIDGET_UNSET_FLAGS (m_btnKnob[3], GTK_CAN_FOCUS);
    gtk_widget_modify_bg(m_btnKnob[3], GTK_STATE_PRELIGHT, g_deep);
    gtk_widget_modify_bg(m_btnKnob[3], GTK_STATE_ACTIVE, g_deep);
    gtk_widget_modify_bg(m_btnKnob[3], GTK_STATE_INSENSITIVE, g_deepGray);

    m_labelKnob[4] = create_label("knob5", 0, 22, g_lightGray, NULL);
    gtk_widget_modify_fg(m_labelKnob[4], GTK_STATE_PRELIGHT, g_lightGray);
    gtk_widget_modify_fg(m_labelKnob[4], GTK_STATE_ACTIVE, g_lightGray);
    gtk_label_set_justify(GTK_LABEL(m_labelKnob[4]), GTK_JUSTIFY_CENTER);
    m_btnKnob[4] = create_button(m_labelKnob[4], 0, 22, g_deep);
    gtk_table_attach_defaults(GTK_TABLE(m_tableKnob), m_btnKnob[4], 17, 21, 0, 1);
    GTK_WIDGET_UNSET_FLAGS (m_btnKnob[4], GTK_CAN_FOCUS);
    gtk_widget_modify_bg(m_btnKnob[4], GTK_STATE_PRELIGHT, g_deep);
    gtk_widget_modify_bg(m_btnKnob[4], GTK_STATE_ACTIVE, g_deep);
    gtk_widget_modify_bg(m_btnKnob[4], GTK_STATE_INSENSITIVE, g_deepGray);

    m_labelRight = create_label("≫", 0, 22, g_lightGray, NULL);
    gtk_widget_modify_fg(m_labelRight, GTK_STATE_PRELIGHT, g_lightGray);
    gtk_widget_modify_fg(m_labelRight, GTK_STATE_ACTIVE, g_lightGray);
    GtkWidget *btn_right = create_button(m_labelRight, 0, 22, g_deep);
    gtk_table_attach_defaults(GTK_TABLE(m_tableKnob), btn_right, 21, 22, 0, 1);
    GTK_WIDGET_UNSET_FLAGS (btn_right, GTK_CAN_FOCUS);
    gtk_widget_modify_bg(btn_right, GTK_STATE_PRELIGHT, g_deep);
    gtk_widget_modify_bg(btn_right, GTK_STATE_ACTIVE, g_deep);
    gtk_widget_modify_bg(btn_right, GTK_STATE_INSENSITIVE, g_deepGray);

    SetItem(KnobProjectMenu, sizeof(KnobProjectMenu)/sizeof(KnobProjectMenu[0]), TEST);
    return m_tableKnob;
}

void KnobProjectMode::PageUp(void)
{
    if (m_CurLevel > 0)
    --m_CurLevel;
    Update();

    ProjectCalc2D::GetInstance()->ShowCurPageParaValue(m_CurLevel);
}

void KnobProjectMode::PageDown(void)
{
    if (m_CurLevel < m_MaxLevel)
    ++m_CurLevel;
    Update();

    ProjectCalc2D::GetInstance()->ShowCurPageParaValue(m_CurLevel);
}

void KnobProjectMode::Refresh(void)
{
    if(!m_isShow) return;

    FormatCfm::EFormatCfm formatCfm = FormatCfm::GetInstance()->GetFormat();
    Format2D::EFormat2D format2D = Format2D::GetInstance()->GetFormat();

    // samples
    DSCCONTROLATTRIBUTES* m_ptrDscPara = DscMan::GetInstance()->GetDscPara();
    bool cfmIsDirection = m_ptrDscPara->dcaCFMIsDirection;

    gtk_widget_queue_draw(m_labelLeft);
    gtk_widget_queue_draw(m_labelRight);
    char type = GlobalClassMan::GetInstance()->GetProbeType();
    for (int i = 0; i < KNOB_NUM; i ++)
    {
        gtk_widget_queue_draw(m_labelKnob[i]);
        gtk_widget_modify_fg(m_labelKnob[i], GTK_STATE_NORMAL, g_lightGray);
#if 0
        if((type == 'L') || (type == 'l'))
        {
            if(strcmp(m_KnobItem[m_CurLevel * KNOB_NUM + i].name, "Scan Angle") == 0)
                gtk_widget_modify_fg(m_labelKnob[i], GTK_STATE_NORMAL, g_deepGray);
            if(strcmp(m_KnobItem[m_CurLevel * KNOB_NUM + i].name, "EFVI") == 0)
                gtk_widget_modify_fg(m_labelKnob[i], GTK_STATE_NORMAL, g_deepGray);
        }
        else
        {
            if(strcmp( m_KnobItem[m_CurLevel * KNOB_NUM + i].name, "Steer") == 0)
                gtk_widget_modify_fg(m_labelKnob[i], GTK_STATE_NORMAL, g_deepGray);
            if(strcmp(m_KnobItem[m_CurLevel * KNOB_NUM + i].name, "TP-View") == 0)
                gtk_widget_modify_fg(m_labelKnob[i], GTK_STATE_NORMAL, g_deepGray);
        }

        if((type != 'P') && (type != 'p'))
        {
            if(strcmp( m_KnobItem[m_CurLevel * KNOB_NUM + i].name, "Scan Line") == 0)
                gtk_widget_modify_fg(m_labelKnob[i], GTK_STATE_NORMAL, g_deepGray);
        }

        if(!cfmIsDirection && (ScanMode::GetInstance()->GetScanMode() != ScanMode::PWPDI))
        {
            if(strcmp( m_KnobItem[m_CurLevel * KNOB_NUM + i].name, "Baseline") == 0)
                gtk_widget_modify_fg(m_labelKnob[i], GTK_STATE_NORMAL, g_deepGray);
        }

        if((ScanMode::GetInstance()->GetScanMode() == ScanMode::CFM)
             ||(ScanMode::GetInstance()->GetScanMode() == ScanMode::PDI)
             ||(ScanMode::GetInstance()->GetScanMode() == ScanMode::PDI_VS_2D))
        {
            if(strcmp( m_KnobItem[m_CurLevel * KNOB_NUM + i].name, "Simult") == 0)
                gtk_widget_modify_fg(m_labelKnob[i], GTK_STATE_NORMAL, g_deepGray);
        }
#endif
    }
}

void KnobProjectMode::Display(KnobItem item, char *buf)
{
    if (strcmp(item.name, "") == 0)
    {
        sprintf(buf, " ");
        return ;
    }
    if (item.status == MIN)
    {
        if (strcmp(item.value, ""))
            sprintf(buf, "%s\n◁ %s ▶",  _(item.name),  _(item.value));
        else
            sprintf(buf, "%s\n◁ %s ▶", _(item.name),  item.value);
    }
    else if (item.status == MAX)
    {
        if (strcmp(item.value, ""))
            sprintf(buf, "%s\n◀ %s ▷", _(item.name),  _(item.value));
        else
            sprintf(buf, "%s\n◀ %s ▷", _(item.name),  item.value);
    }
    else if (item.status == OK)
    {
        if (strcmp(item.value, ""))
            sprintf(buf, "%s\n◀ %s ▶", _(item.name),  _(item.value));
        else
            sprintf(buf, "%s\n◀ %s ▶", _(item.name),  item.value);
    }
    else if (item.status == PRESS)
    {
        if (strcmp(item.value, ""))
            sprintf(buf, "%s\n【 %s 】", _(item.name),  _(item.value));
        else
            sprintf(buf, "%s\n【 %s 】", _(item.name),  item.value);
    }
    else if (item.status == ERROR)
    {
        if (strcmp(item.value, ""))
            sprintf(buf, "<span color='#505050'>%s\n %s </span>",_(item.name), _(item.value));
        else
            sprintf(buf, "<span color='#505050'>%s\n %s </span>", _(item.name),  item.value);
    }
}

void KnobProjectMode::Update(void)
{
    char buf[80];
    for (int i = 0; i < KNOB_NUM; i ++)
    {
        Display(m_KnobItem[m_CurLevel * KNOB_NUM + i], buf);
        gtk_label_set_markup(GTK_LABEL(m_labelKnob[i]), buf);
    }

    if (m_CurLevel == 0 && m_MaxLevel == 0)
    {
        gtk_label_set_text(GTK_LABEL(m_labelLeft), "");
        if(GetKnobStatus())
        {
            gtk_label_set_text(GTK_LABEL(m_labelRight), " \n ▶");
        }
        else
        {
            gtk_label_set_text(GTK_LABEL(m_labelRight), "");
        }
    }
    else if (m_CurLevel == 0 && m_MaxLevel > 0)

    {
        gtk_label_set_text(GTK_LABEL(m_labelLeft), "");
        if(GetKnobStatus())
        {
            gtk_label_set_text(GTK_LABEL(m_labelRight), ">>\n ▶");
        }
        else
        {
            gtk_label_set_text(GTK_LABEL(m_labelRight), ">>");
        }
    }
    else if (m_CurLevel > 0 && m_CurLevel == m_MaxLevel)
    {
        gtk_label_set_text(GTK_LABEL(m_labelLeft), "<<");
        if(GetKnobStatus())
        {
            gtk_label_set_text(GTK_LABEL(m_labelRight), " \n ▶");
        }
        else
        {
            gtk_label_set_text(GTK_LABEL(m_labelRight), "");
        }
    }
    else if (m_CurLevel > 0 && m_CurLevel < m_MaxLevel)
    {
        gtk_label_set_text(GTK_LABEL(m_labelLeft), "<<");
        if(GetKnobStatus())
        {
            gtk_label_set_text(GTK_LABEL(m_labelRight), ">>\n ▶");
        }
        else
        {
            gtk_label_set_text(GTK_LABEL(m_labelRight), ">>");
        }
    }

    Refresh();
}

void KnobProjectMode::SetValue(int index, const char* value, EKnobReturn status)
{
    sprintf(m_KnobItem[index].value, "%s", value);
    m_KnobItem[index].status = status;
    Update();
}

void KnobProjectMode::Knob1_Screw(int cw)
{
    int index = 0;
    index = KNOB_NUM * m_CurLevel + 0;

    if (m_KnobItem[index].pf == NULL || m_KnobItem[index].status == ERROR)
    return ;

    if (cw == 1)
    {
        (*(m_KnobItem[index].pf))(ADD);
    }
    else if (cw == 0)
    {
        (*(m_KnobItem[index].pf))(SUB);
    }
}

void KnobProjectMode::Knob2_Screw(int cw)
{
    int index = 0;
    index = KNOB_NUM * m_CurLevel + 1;

    if (m_KnobItem[index].pf == NULL || m_KnobItem[index].status == ERROR)
    return ;

    if (cw == 1) {
    (*(m_KnobItem[index].pf))(ADD);
    }
    else if (cw == 0) {
    (*(m_KnobItem[index].pf))(SUB);
    }
}
void KnobProjectMode::Knob3_Screw(int cw)
{
    int index = 0;
    index = KNOB_NUM * m_CurLevel + 2;

    if (m_KnobItem[index].pf == NULL || m_KnobItem[index].status == ERROR)
    return ;

    if (cw == 1) {
    (*(m_KnobItem[index].pf))(ADD);
    }
    else if (cw == 0) {
    (*(m_KnobItem[index].pf))(SUB);
    }

}
void KnobProjectMode::Knob4_Screw(int cw)
{
    int index = 0;
    index = KNOB_NUM * m_CurLevel + 3;

    if (m_KnobItem[index].pf == NULL || m_KnobItem[index].status == ERROR)
    return ;

    if (cw == 1) {
    (*(m_KnobItem[index].pf))(ADD);
    }
    else if (cw == 0) {
    (*(m_KnobItem[index].pf))(SUB);
    }

}
void KnobProjectMode::Knob5_Screw(int cw)
{
    int index = 0;
    index = KNOB_NUM * m_CurLevel + 4;

    if (m_KnobItem[index].pf == NULL || m_KnobItem[index].status == ERROR)
    return ;

    if (cw == 1) {
    (*(m_KnobItem[index].pf))(ADD);
    }
    else if (cw == 0) {
    (*(m_KnobItem[index].pf))(SUB);
    }
}

void KnobProjectMode::Knob1_Press(void)
{
    int index = 0;
    index = KNOB_NUM * m_CurLevel + 0;

    if (m_KnobItem[index].pfPress == NULL || m_KnobItem[index].status == ERROR)
    return ;

    (*(m_KnobItem[index].pfPress))();
}

void KnobProjectMode::Knob2_Press(void)
{
    int index = 0;
    index = KNOB_NUM * m_CurLevel + 1;

    if (m_KnobItem[index].pfPress == NULL || m_KnobItem[index].status == ERROR)
    return ;

    (*(m_KnobItem[index].pfPress))();

}
void KnobProjectMode::Knob3_Press(void)
{
    int index = 0;
    index = KNOB_NUM * m_CurLevel + 2;

    if (m_KnobItem[index].pfPress == NULL || m_KnobItem[index].status == ERROR)
    return ;

    (*(m_KnobItem[index].pfPress))();

}
void KnobProjectMode::Knob4_Press(void)
{
    int index = 0;
    index = KNOB_NUM * m_CurLevel + 3;

    if (m_KnobItem[index].pfPress == NULL || m_KnobItem[index].status == ERROR)
    return ;

    (*(m_KnobItem[index].pfPress))();
}
void KnobProjectMode::Knob5_Press(void)
{
    int index = 0;
    index = KNOB_NUM * m_CurLevel + 4;

    if (m_KnobItem[index].pfPress == NULL || m_KnobItem[index].status == ERROR)
    return ;

    (*(m_KnobItem[index].pfPress))();
}

void KnobProjectMode::ShowKnob(void)
{
    m_enable = true;
    if(!m_isShow) m_isShow = true;
    Update();
    gtk_widget_show_all(m_tableKnob);
}

void KnobProjectMode::EnableKnob(bool enable)
{
    m_enable = enable;
    Update();
}

void KnobProjectMode::HideKnob(void)
{
    m_enable = false;
    if(m_isShow) m_isShow = false;
    Update();
    gtk_widget_hide_all(m_tableKnob);
}

unsigned int KnobProjectMode::GetCurPageLevel()
{
    return m_CurLevel;
}
#endif