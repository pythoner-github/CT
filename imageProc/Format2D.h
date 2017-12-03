#ifndef FORMAT_2D_H
#define FORMAT_2D_H

#include "AbsUpdate2D.h"
#include "Img2D.h"
#include "Replay.h"
#include "DSCTypes.h"
#include "DscMan.h"

/*
 * @brief change 2d display format or switch current image in BB/4B format
 */
class Format2D
{
	public:
		///> new type
		enum EFormat2D{B, BB, B4}; ///< 2d display format, "B"=one B, "BB"=two B, "B4"=four B,

		static Format2D* GetInstance();
		~Format2D();

		int ChangeFormat(enum EFormat2D format);
		bool SwitchBB(bool left, int &current);
		int SwitchBB(void);
		int SwitchBBForFreeze(int lastRealB);
		int SwitchB4();
		int SwitchB4ForFreeze(int lastRealB);

		EFormat2D GetFormat();
		int GetCurrentB() { return m_curB; }
		int GetLastRealB() { return m_curRealB; }

        // special measure
        void SetFormatForSnap(EFormat2D format) { m_formatSnap = format; }
	
	private:
		Format2D();
		
		static Format2D* m_ptrInstance;

		DscMan* m_ptrDsc;
		DSCCONTROLATTRIBUTES* m_ptrDscPara;

		AbsUpdate2D* m_ptrUpdate;
		Replay* m_ptrReplay;
		Img2D* m_ptrImg;

		EFormat2D m_format; ///< current 2d format
		int m_curB; ///< curent B image. BB(0-left, 1-right), 2B(0-upleft, 1-upright, 2-downleft, 3-downright)
        int m_curRealB; ///< current real B image BB(0-left, 1-right), 2B(0-upleft, 1-upright, 2-downleft, 3-downright)
        int m_curRealB_bak; ///< current real B image BB(0-left, 1-right), 2B(0-upleft, 1-upright, 2-downleft, 3-downright)
        EFormat2D m_formatSnap;
};

#endif
