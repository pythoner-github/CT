#ifndef IMG_PROC_PW_H
#define IMG_PROC_PW_H

#include "Def.h"
#include "DSCTypes.h"
#include "AbsUpdatePw.h"
#include "ExamItem.h"

class ImgProcPw
{
	public:
		~ImgProcPw();

		static const int MAX_TIME_RES = 4;

		static ImgProcPw* GetInstance();
		void InitOptimize(ExamItem::ParaSpectrum* pw);
		void Init(ExamItem::ParaSpectrum* pw);
		void GetCurPara(ExamItem::ParaItem* ptrParaItem);

		EKnobReturn ChangeTimeSmooth(EKnobOper oper);
		void ChangeTimeMark(bool on);
		
		bool GetTraceStatus();
		bool GetDscTraceStatus();
        void SetDscTraceStatus(bool status);
		vector<POINT> GetTraceData(void);
		vector<POINT> GetTraceCalcData(void);
		void GetTraceCycle(int &begin, int &end);
		void SetTraceStatus(bool status);
		void SetTraceType(unsigned char type);
		void SetTraceDirection(unsigned char direction);
		void SetTraceThreshold(unsigned char threshold);
		void SetTraceSmooth(unsigned char smooth);
		void SetTraceBaseline(char baseline);
		void SetTraceFs(char fs);
        void SetAutoCalc(bool on);
        bool GetAutoCalc(void);
	private:
		ImgProcPw();
		static ImgProcPw* m_ptrInstance;

		DSCCONTROLATTRIBUTES* m_ptrDscPara;
		AbsUpdatePw* m_ptrUpdate;

		int m_timeSmooth;
		bool m_TimeMark;
        bool m_autoCalc;
        bool m_drawTraceStatus;

		void SetTimeSmooth(int data, EKnobReturn ret);
		void SetTimeMark(bool on);

};

inline void ImgProcPw::SetAutoCalc(bool on)
{
    m_autoCalc = on;
}
inline bool ImgProcPw::GetAutoCalc(void)
{
    return m_autoCalc;
}

#endif
