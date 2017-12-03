#ifndef __KNOB_PROJECTMODE_H__
#define __KNOB_PROJECTMODE_H__
#ifdef EMP_PROJECT
#include "Def.h"
#include "HintArea.h"

class KnobProjectMode
{
    public:
        ~KnobProjectMode();

        enum EKnobType {NONE, D2, M, PW, CFM, REPLAY, SNAP, ARROW, BDMK, ANATOMIC_M, EFOV, TEST};
        enum CONST{ WIDTH_KNOB=844, HEIGHT_KNOB=46, KNOB_NUM=5};

    	typedef EKnobReturn (*pKnobFunc)(EKnobOper);
    	typedef EKnobReturn (*pKnobFuncPress)(void);
    	struct KnobItem
        {
		char *name;
		char value[20];
		bool isPrint;
		EKnobReturn status;
		pKnobFunc pf;
		pKnobFuncPress pfPress;
    	};

        static KnobProjectMode* GetInstance(void);
        EKnobType GetKnobType(void) { return m_knobType; }
        KnobItem* GetItem(void);
        GtkWidget* Create(void);
        bool GetKnobStatus(void){ return (m_enable&&m_isShow);}
        void EnableKnob(bool enable);
        void ShowKnob(void);
        void HideKnob(void);
        void SetItem(KnobItem *item, unsigned int total_item, EKnobType type);
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

	static const int KNOB_MENU_SIZE = 10;
	void SetPrint(int seq, bool value);
	bool GetPrint(int seq);
	unsigned int GetCurPageLevel();

    private:
        KnobProjectMode();
        void Refresh(void);
        void Display(KnobItem item, char *buf);

        GtkWidget *m_tableKnob;
        GtkWidget *m_labelLeft;
        GtkWidget *m_labelRight;
        GtkWidget *m_labelKnob[KNOB_NUM];
        GtkWidget *m_btnKnob[KNOB_NUM];
        unsigned int m_CurLevel;
        unsigned int m_MaxLevel;
        KnobItem *m_KnobItem;
        EKnobType m_knobType;
        static KnobProjectMode* m_ptrInstance;
        static KnobItem KnobProjectMenu[KNOB_MENU_SIZE];
        bool m_isShow;
        bool m_enable;

};
#endif
#endif //__KNOB_PROJECTMODE_H__
