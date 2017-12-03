#ifndef MULTI_FUNC_BODYMARK_H
#define MULTI_FUNC_BODYMARK_H

#include <gtk/gtk.h>
#include "KnobEvent.h"
#include "FakeXEvent.h"
#include "AbsMultiFunc.h"
#include "BodyMark.h"

class MultiFuncBodyMark: public FakeXEvent, public AbsMultiFunc
{
	public:
		MultiFuncBodyMark();
		~MultiFuncBodyMark();

		static void SetNewBodyMark(GdkPixbuf* pixbuf);
		static EKnobReturn SetBodyMarkSize(EKnobOper opr);
		static EKnobReturn PressSetBodyMarkSize(void);
		static EKnobReturn SetProbeMarkColor(EKnobOper opr);
		static EKnobReturn PressSetProbeMarkColor(void);
		static EKnobReturn PressSetHideBodyMark(EKnobOper opr);
		static EKnobReturn PressSetHideBodyMark(void);
		// event
		void KeyEvent(unsigned char keyValue);
		void KnobEvent(unsigned char keyValue, unsigned char offset);
		void MouseEvent(char offsetX, char offsetY);
		void KnobBodyMarkCreate();

	private:
		//enum {BDMK_W = 60, BDMK_H = 75};
		enum BODYMARK_OPR {IDEL, ADD, MOVE_PROBE, MOVE_BODY};

		static MultiFuncBodyMark* m_ptrThis;
		POINT m_bodyPos[4]; // pos of left-up corner of bodymark
		POINT m_probePos[4]; // pos of center of probe mark
		POINT m_probePosOffset; // offset of probe mark to body mark(left-up corner)
		GdkRectangle m_bodyPosRange; // range of bodymark's pos
		BodyMark* m_ptrBodyMark;
		BODYMARK_OPR m_bodyOpr;
		int m_timer;
		//bool m_firstDraw[4];
		int m_active; // which bodymark is active,  equal to current B; must <= 4

		// opr
		POINT GetPointProbe(int offsetX, int offsetY);
		POINT GetPointBody(int offsetX, int offsetY);
		void Value(EKnobOper opr);
		void EndMoveProbeMark();
		void EndMoveBodyMark();
		void UndoMoveBodyMark();
		void Esc();
		void Hide(); // hide bodymark
		bool Clicked(); 
		bool DClicked(); 

		void AddNewBodyMark(GdkPixbuf* pixbuf);
		void BeginMoveProbeMark();
		void GetBodyMarkSize(int &w, int &h);
		void ReviseProbePos(double scale);

		static EKnobReturn GetKnobRetBodyMarkSize(BodyMark::SIZE size);
		static EKnobReturn GetKnobRetProbeMarkColor(BodyMark::COLOR color);

		static gboolean HandleClicked(gpointer data) { return m_ptrThis->Clicked(); }
};
#endif
