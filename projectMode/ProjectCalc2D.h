#ifndef __PROJECT_CALC2D_H__
#define __PROJECT_CALC2D_H__
#ifdef EMP_PROJECT
#include "Calc2D.h"
#include "DSCTypes.h"
#include "DSC.h"

class ProjectCalc2D
{
	public:
		~ProjectCalc2D();
		static ProjectCalc2D* GetInstance(void);

		void SetProjectCalc2D(Calc2D* calc);
		void SetDsc(CDSC* dsc) {m_ptrDsc = dsc;}
        void RefreshProjectCalc(const char* model, int freqIndex, bool harmonic);
        void InitKnobPara(void);
        void RefreshKnobPara(void);

		// test-----optimization
		EKnobReturn ChangeDemodSection(EKnobOper oper);
	    EKnobReturn ChangeBandPassFc(EKnobOper oper);
		EKnobReturn ChangeTxFNum(EKnobOper oper);
		EKnobReturn ChangeWpScale(EKnobOper oper);
		EKnobReturn Change2DPulse(EKnobOper oper);
		EKnobReturn ChangePwPulse(EKnobOper oper);
		EKnobReturn ChangeCfmPulse(EKnobOper oper);
		EKnobReturn ChangeProbeR(EKnobOper oper);
		EKnobReturn ChangeProbeAngle(EKnobOper oper);
#if 0
        EKnobReturn ChangeLowPassWindowFunc(EKnobOper oper);
		EKnobReturn ChangeBPWSimult(EKnobOper oper);

		EKnobReturn ChangeBandPassWindowFunc(EKnobOper oper);
        EKnobReturn ChangeEmissionPara(EKnobOper oper);
#endif
        EKnobReturn ChangeDemodParaPrintf();

		void SaveCalcPara2D(char* name);
		void ReadCalcPara2D(const char* name);
		void ShowCurPageParaValue(int row);

	private:
		ProjectCalc2D();
		static ProjectCalc2D* m_ptrInstance;

		Calc2D* m_ptrCalc;
		Calc2D::CalcPara m_calcPara;
		Calc2D::ProjectCalcPara m_projectCalcPara;

		DSCCONTROLATTRIBUTES* m_ptrDscPara;
		CDSC* m_ptrDsc;
		static FpgaCtrl2D m_fpga;
        int m_probeIndex;
        int m_freqIndex;
        bool m_harmonic;
        int m_emitIndex;

		static const int MAX_DEMOD_SECTION = 6;
		static const float MAX_BAND_PASS_W1 = 200.0;
        static const float MAX_TX_F_NUM = 200.0;
        static const float MAX_WP_SCALE = 200.0;
        static const int MAX_2D_PLUSE= 20;
        static const int MAX_PW_PLUSE= 20;
        static const int MAX_CFM_PLUSE= 20;
        static const int MAX_PROBE_R= 20000;
        static const int MAX_PROBE_ANGLE= 20000;

        static const float EPSINON ;

        ///>2016.06.07
        void UpdateDemodSection(int section, EKnobReturn ret);
        void UpdateBandPassW(float data, EKnobReturn ret);
        void UpdateTxFNum(float data, EKnobReturn ret);
        void UpdateWpScale(float data, EKnobReturn ret);
        void Update2DPulse(float data, EKnobReturn ret);
        void UpdatePwPulse(float data, EKnobReturn ret);
        void UpdateCfmPulse(float data, EKnobReturn ret);
        void UpdateProbeR(float data, EKnobReturn ret);
        void UpdateProbeAngle(float data, EKnobReturn ret);
};
#endif
#endif