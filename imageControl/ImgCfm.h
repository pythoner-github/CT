#ifndef IMG_CFM_H
#define IMG_CFM_H

#include "CalcCfm.h"
#include "AbsUpdateCfm.h"
#include "AbsUpdate2D.h"
#include "AbsUpdatePw.h"
#include "Img2D.h"
#include "ImgPw.h"
#include "FpgaCfm.h"
#include "ProbeMan.h"
#include "ExamItem.h"
#include "DscMan.h"
#include <base/CalcTime.h>

class ImgCfm
{
	public:
		enum EFormatCfm{B, BB, B4}; ///< 2d display format, "B"=one B, "BB"=two B, "B4"=four B,
		enum EColorMode{ COLOR_CFM, COLOR_PDI, COLOR_PDI_DIR};

		static const int MAX_PRF_INDEX = 13;
		static const int CFM_PRF[MAX_PRF_INDEX]; // unit:hz
		static const int MAX_PKTSIZE_INDEX = 10; // max pktsize index 
		static const int MAX_WALLFILTER_INDEX_ALL = 16; //max number of wall filter in wall filter lib(can be selected)
		static const int MAX_WALLFILTER_INDEX = 13; // max wall filter index 
		static const float WALLFILTER_DISPLAY[MAX_PKTSIZE_INDEX][MAX_WALLFILTER_INDEX_ALL];
		static const int WALL_FILTER_SELECT_IDNEX[MAX_PKTSIZE_INDEX][8];
		static const int MAX_LINE_DENSITY = 2;
		static const string LINE_DENSITY_DISPLAY[MAX_LINE_DENSITY];
		static const int MAX_TURB = 2;
		static const float INTERVAL = 50000.0;

		~ImgCfm();

		static ImgCfm* GetInstance();
		static int GetWFDisplay(int prfIndex, int pktIndex, int wfIndex);
		
		void SetImg2D(Img2D* img) { m_ptrImg2D = img; }
		void SetCalcCfm(CalcCfm* calc);
		void SetDsc(CDSC* dsc) {m_ptrDsc = dsc;}

		void InitProbeOptimize(ProbeSocket::ProbePara* ptrPara, ExamItem::ParaItem* ptrParaItem);		
		void InitProbe(ProbeSocket::ProbePara* ptrPara, ExamItem::ParaItem* ptrParaItem);		
		void GetCurPara(ExamItem::ParaItem* ptrParaItem);
		void ChangeDopplerFreq();

		// box
		void BCWidth(void);
		void ReSendBox(void);
		void ResetBox(bool draw);
		void SetBoxStatus(bool changeSize);
		bool GetBoxStatus(void);
		bool ChangeBoxSize(int offsetX, int offsetY, bool lrOverturn, bool udOverturn);
		bool ChangeBoxPos(int offsetX, int offsetY);
        void GetBoxInfo(int &lineBegin, int &lineEnd, int &dotBegin, int &dotEnd);
        void SetBoxInfo(int lineBegin, int lineEnd, int dotBegin, int dotEnd);
        void SetBalanceStatus(bool balance) { m_balance = balance; }

		// func
		void ChangeMBP(bool drawBox);
		void ChangeTgcCfm(int tgcY[8]);
		EKnobReturn ChangeGainCfm(EKnobOper oper);
		EKnobReturn ChangeScale(EKnobOper oper);
		EKnobReturn ChangeScaleSimult(EKnobOper oper);
		EKnobReturn ChangeBaseline(EKnobOper oper);
		EKnobReturn ChangeWallFilter(EKnobOper oper);
		EKnobReturn ChangeLineDensity(EKnobOper oper);
		EKnobReturn ChangeSensitive(EKnobOper oper);
		EKnobReturn ChangeTurbo(EKnobOper oper);
		EKnobReturn ChangeColorMap(EKnobOper oper);
		EKnobReturn ChangeColorMapCfm(EKnobOper oper);
		EKnobReturn ChangeColorMapPdi(EKnobOper oper);
		EKnobReturn ChangeColorMapPdiDir(EKnobOper oper);
		EKnobReturn ChangeColorFocus(EKnobOper oper);
        EKnobReturn ChangeFlowOpt(EKnobOper oper);
        EKnobReturn ChangeInvert(EKnobOper oper);
        void ScanLinesSetting(int scanline);
        EKnobReturn ChangeScanLines(EKnobOper oper);

        void ChangeDepthBefore();
		void ChangeDepth();
		void ChangeSteer();
		void ChangeTSI(); ///< must call after ChangeTSI of Img2D
        void ChangeCfmTis();
		void ChangeSoundSpeed();
        void ClearBox();
        void ChangeCfmBoxLine(int scanline);
        void ReDrawBox();
		void PRFSimult3(int prf, EKnobReturn ret);
		void ExitSimult3();

		void SetColorMode(EColorMode mode);

//        void ChangeTgcCfm(int tgcY[]);

        // switch mode
		void EnterCfm(void);
		void EnterPdi(void);
		void SwitchPdiAndDir(bool enterPdiDir);
		//////////////////////  test  ///////////////////////////////////
		EKnobReturn ChangeTissue(EKnobOper oper);
		EKnobReturn ChangeNoise(EKnobOper oper);
		EKnobReturn ChangeSpeed(EKnobOper oper);
		EKnobReturn ChangeVariance(EKnobOper oper);
		EKnobReturn ChangeGainGate(EKnobOper oper);
		int GetBoxRange(unsigned char range[2]);
        int GetFrameRemove(void);
        void SubFrameRemove(void);
        bool WhenBoxIsOutRange(bool draw);
        int GetPktSize(void);
        void SetScanLines();

	private:
		static const int PRF_DISPLAY[MAX_PRF_INDEX];
		static const int WALL_FILTER_SIZE = MAX_WALLFILTER_INDEX_ALL * MAX_PKTSIZE_INDEX * 26 * 32 ;
		static const short WALL_FILTER[WALL_FILTER_SIZE];		
		static const int PACKSIZE[MAX_PKTSIZE_INDEX];
		static const int MAX_BASELINE_INDEX = 8;
		static const int MAX_COLOR_MAP_CFM = 6;
		static const int COLOR_MAP_CFM[MAX_COLOR_MAP_CFM];
		static const int MAX_COLOR_MAP_PDI = 6;
		static const int COLOR_MAP_PDI[MAX_COLOR_MAP_PDI];
		static const int MAX_COLOR_MAP_PDIDIR = 6;
		static const int COLOR_MAP_PDIDIR[MAX_COLOR_MAP_PDIDIR];
#if defined (EMP_430)
		static const int MAX_GAIN_CFM =  300; //500; // linear = 256; //300; //500;
#elif defined(EMP_355)
		static const int MAX_GAIN_CFM =  400;
#else
		static const int MAX_GAIN_CFM = 1084 ;
#endif
		static const int MAX_FLOW_OPT = 8;
		static const int FLOW_OPT[MAX_FLOW_OPT][5]; 
		static const int FLOW_OPT_PDI[MAX_FLOW_OPT][5]; 
        static const int MAX_CFM_FOC_RATIO = 3;
        static const float CFM_FOC_RATIO[MAX_CFM_FOC_RATIO];
		//temp
		static const double BOX_CORRECT_DIST = 1.7;	//0.8; //mm
        static const int BASELINE_VALUE = 127;
        static const float CFM_FOCUS[3];

		ImgCfm();

		void InitProbeCalc();
		void Box(int lineBegin, int lineEnd, int dotBegin, int dotEnd, bool draw);
		void MaxBox();
		void ExitMaxBox();
		void GainCfm(int data);
		EKnobReturn PRF(int index);
		void WhenPRFChanged(int index);
		void Baseline(int index, EKnobReturn ret);
		void VelRange(int prfIndex, int baselineIndex);
		void ReviseBoxLine(int &lineBegin, int &lineEnd);
		void LDensity(bool draw = TRUE);
		void PktSize(int index, EKnobReturn ret);
		void ColorMapCfm(int index, int baselineIndex, int turb, bool invert, EKnobReturn ret);
		void ColorMapPdi(int index, int baselineIndex, int turb, bool invert, EKnobReturn ret);
		void ColorMapPdiDir(int index, int baselineIndex, int turb, bool invert, EKnobReturn ret);
		void ColorMap();
		void Turbo(int index, EKnobReturn ret);
		void LineDensity(int index, EKnobReturn ret);
		void FocPos(int focIndex);
		void SetTis(int prfIndex, int boxLineBegin, int boxLineEnd);
		EKnobReturn FlowOpt(int index);
		bool ClusterSizeNormal(int prf, int depthEndDot);
		void ClusterSize(int prf);
		int GetRealBaseline(int baselineIndex);
		void GetBoxLineRange(int &begin, int &end);
		void GetBoxDotRange(int &begin, int &end);
		void Steer(void);
		EKnobReturn WallFilter(int indexWallFilter, int indexPktsize);		
		int GetPRFIndex(int prf); //unit:hz
		EKnobReturn UpdatePRFRangeStatus(int index, int maxIndex);
        void BalancePwInfo(void);
		int GetFpgaMBP(void);
		void SetVariance(int cfmGain, int variance);
		void BoxDotCorrect(int dotBegin, int dotEnd);
		int GetFlowOptValue(int flowIndex, int itemIndex);
		int GetCfmFocus(float focusPos);
		void StopColorFlow();
		void OpenColorFlow();
		static gboolean TimeOutOpenColorFlow(gpointer data) {ImgCfm::GetInstance()->OpenColorFlow(); return FALSE;}

		static ImgCfm* m_ptrInstance;
		static FpgaCfm m_fpga;
		Img2D* m_ptrImg2D;
		ImgPw* m_ptrImgPw;
		AbsUpdateCfm* m_ptrUpdate;
		AbsUpdatePw* m_ptrUpdatePw;
		DSCCONTROLATTRIBUTES* m_ptrDscPara;
		CDSC *m_ptrDsc;

        Calc2D* m_ptrCalc2d;
        AbsUpdate2D* m_ptrUpdate2d;
		CalcCfm* m_ptrCalc;
		CalcCfm::CalcPara m_calcPara;
		CalcTime m_ctGainCfm;
		EColorMode m_colorMode;	///< color image mode

		int m_tgc[8]; ///< 8 tgc
		int m_gainCfm;
		int m_PRFIndex; ///< index of pulse repeat frequency, zero-based.
		int m_baselineIndex; ///< baseline's position
		int m_wallFilterIndex; ///< level of wall filter, zero-based.
		int m_lineDensity; ///< color flow line density
		int m_pktsizeIndex;	///< index of packet size, zero-based. color sensitive
		int m_turbIndex; ///< level of trubo
		int m_colorMapCfm; ///< level of cfm map
		int m_colorMapPdi; ///< level of pdi map
		int m_colorMapPdiDir; ///< level of direction pdiDirmap
		int m_boxLine[2]; ///< range of line in box 
		int m_boxDot[2]; ///< range of dot in box
		int m_boxDotBefore[2]; ///< range of dot in box before correct
		bool m_boxStatus; ///< if or not change box size
		int m_tissueGate;	///< tissue threshold
		int m_noiseGate;	///< noise threshold
		int m_speedGate;	///< speed threshold
		int m_varianceGate;	///< variance threshold
		int m_gainGate;
		bool m_invert; ///< color invert
		int m_flowOptIndex;
		int m_clusterSize;
		int m_maxPRFIndex;
		int m_tGainCfm;
        double m_scale2DBefore;
        bool m_balance;
        bool m_BCWidth;
		int m_boxLineBak[2]; ///< bak for BCwidth
		int m_boxDotBak[2]; ///< bak for BCwidth
        int m_focIndex; ///< focus index of cfm
        int m_frameRemove;

        ///> focus
        int m_cfmFocusIndex;
        ///>low filter
        float m_fcLowFilter;
        int m_scanLineBak;

		int m_timerOpenColorFlow;
		unsigned char m_colorPersistBak;
		unsigned char m_colorThresholdBak;
       
        ///>test
    public:
        void ChangeLowFilterFc(EKnobOper oper);
        void UpdateHint();

};

#endif
