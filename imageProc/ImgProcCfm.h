#ifndef IMG_PROC_CFM_H
#define IMG_PROC_CFM_H

#include "DSCTypes.h"
#include "AbsUpdateCfm.h"
#include "ExamItem.h"

class ImgProcCfm
{
	public:
		~ImgProcCfm();

		static const int MAX_PERSIST = 8;
		static const int MAX_REJECT = 6;
		static const int MAX_SMOOTH = 8;

		static ImgProcCfm* GetInstance();
		void InitOptimize(ExamItem::ParaColor* color);
		void Init(ExamItem::ParaColor* color);
		void GetCurPara(ExamItem::ParaItem* ptrParaItem);

		void ChangeColorHide();
		EKnobReturn ChangeBackgroundHide(bool on);
		EKnobReturn ChangeThreshold(EKnobOper oper);
		EKnobReturn ChangePersist(EKnobOper oper);
		EKnobReturn ChangeReject(EKnobOper oper);
		EKnobReturn ChangeSmooth(EKnobOper oper);
		EKnobReturn ChangeArtifact(EKnobOper oper); //自适应伪差去除技术

		void SetThresholdOff();

	private:
		ImgProcCfm();

		static ImgProcCfm* m_ptrInstance;

		DSCCONTROLATTRIBUTES* m_ptrDscPara;
		AbsUpdateCfm* m_ptrUpdate;

		static const int MAX_THRESHOLD = 17;
		static const int THRESHOLD[MAX_THRESHOLD];
		static const int PERSIST[MAX_PERSIST];
		static const int MAX_ARTIFACT = 7;//4

		int m_threshold;
		int m_thresholdBak;
		int m_persist;
		int m_reject;
		int m_smooth;
		int m_artifact;
		bool m_backgroundHide;
		bool m_colorHide;
		bool m_cfmCompare;

		void ColorHide(bool on);
		void SetThreshold(int index, EKnobReturn ret);
		void SetPersist(int index, EKnobReturn ret);
		void SetReject(int index, EKnobReturn ret);
		void SetSmooth(int index, EKnobReturn ret);
		void SetArtifact(int index, EKnobReturn ret);
};
#endif
