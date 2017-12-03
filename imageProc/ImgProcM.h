#ifndef __IMGPROCM_H__
#define __IMGPROCM_H__

#include "DSCTypes.h"
#include "DSC.h"
#include "AbsUpdate2D.h"
#include "ExamItem.h"
#include "../display/ImageAreaDraw.h"
#include "../display/ImageAreaPara.h"

/**
* @brief processing M image (just for the image which processed with DSC )
*/
class ImgProcM
{
	public:
		///>
		~ImgProcM();

		static ImgProcM* GetInstance();
		void InitOptimize(ExamItem::Para2D* d2);
		void Init(ExamItem::Para2D* d2);

		///> change the para for GUI
		enum EKnobReturn ChangeEnhanceM(enum EKnobOper oper);
		enum EKnobReturn ChangeMSmooth(enum EKnobOper oper);

		void AnatomicMInit(void);
		void ChangeAnatomicLine(int offsetX, int offsetY);
		void MoveAnatomicLine(int offsetX, int offsetY);
		void AnatomicLineInit(void);
		void GetAnatomicInitLine(POINT &begin, POINT &end);

		void ChangeTimeMark(bool on);
	
	private:
		ImgProcM();

		///> extern member
		static ImgProcM* m_ptrInstance;

		DSCCONTROLATTRIBUTES* m_ptrDscPara;
		AbsUpdate2D* m_ptrUpdate;
		ImageAreaDraw* m_ptrImgDraw;
    ImageAreaPara *m_ptrImgPara;
		CDSC* m_ptrDsc;

		///> static const
		static const int MAX_ENHANCE_M = 4;
		static const int MAX_ANATOMIC_M = 36; ///< 10 per level
		static const int MAX_MSMOOTH = 8;

		int m_EnhanceM;	///< 0~3
		unsigned char m_MSmooth;	///< 0~7
		bool m_LineDraw;
		POINT m_LineBegin; //start point of anatomic line
		POINT m_LineEnd; // end point of anatomic line
		bool m_TimeMark;

		///> set para to DSC of M mode
		void SetEnhanceM(int data, EKnobReturn ret);
		void SetMSmooth(int data, EKnobReturn ret);
		void SetAnatomicLine(POINT begin, POINT end);
		void ClearAnatomicLine(POINT begin, POINT end);
		void SetTimeMark(bool on);
};

#endif //__IMGPROCM_H__
