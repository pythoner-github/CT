#ifndef IMAGING2D_H
#define IMAGING2D_H

#include "ProbeMan.h"
#include "ExamItem.h"
#include "Calc2D.h"
#include "AbsUpdate2D.h"
#include "Def.h"
#include "DSCTypes.h"
#include "DSC.h"
#include <base/CalcTime.h>
#include "ManRegister.h"

/*
 * @brief class Img2D control the imging of 2D and M mode
 */
class Img2D
{
	public:
		///>
		~Img2D();

		static Img2D* GetInstance();

		static const int MAX_FOCUS = 4;// = Calc2D::FOC_MAX;
		static const int MAX_LINE_DENSITY = 2;
		static const string LINE_DENSITY_DISPLAY[MAX_LINE_DENSITY];
		static const int MAX_MBP_INDEX = 3;
		static const int MBP[MAX_MBP_INDEX];
		static const int FPGAMBP2D[MAX_LINE_DENSITY][MAX_MBP_INDEX];
		static const int FPGAMBPPw[MAX_LINE_DENSITY][MAX_MBP_INDEX];
		static const int FPGAMBPColor[MAX_LINE_DENSITY][MAX_MBP_INDEX];
		static const int MAX_POWER_INDEX = 10;
		static const int POWER_DATA[MAX_POWER_INDEX];
		static const int MAX_STEER = 7;
		static const int STEER_ANGLE[MAX_STEER];
		static const int FOC_POS_INDEX_MAX[MAX_FOCUS];
		static const int MAX_SCALE_INDEX = 24;
		static const int IMG_SCALE[MAX_SCALE_INDEX];
		static const int MAX_AGC = 4;
		static const int MAX_EDGE = 8;
		static const int DYNAMIC_DATA_D[Calc2D::MAX_DYNAMIC_INDEX]; //display
		static const int MAX_TSI = 4;
		static const string TSI_DISPLAY[MAX_TSI];
        static const int MAX_ANGLE = 8;
        static const int MAX_SPACE_COMPOUND = 4;
        static const int SPACE_COMPOUND_ANGLE[MAX_SPACE_COMPOUND];
        static const int MAX_SCAN_LINES = 4;
        static const int SCAN_LINES_PHASE_PROBE[MAX_SCAN_LINES];

		CalcTime m_ctGain2D, m_ctGainM;
		int m_tGain2D, m_tGainM;
		static const float INTERVAL = 50000.0;

        CalcTime m_ctDepth;
        int m_tDepth;
        int m_stepDepth;


        char m_str[20];
		///> general member
		void SetCalc2D(Calc2D* calc);
		void SetDsc(CDSC* dsc) { m_ptrDsc = dsc;}

		//operation of 2D mode
		void InitProbe2DOptimize(ProbeSocket::ProbePara* ptrPara, ExamItem::ParaItem* ptrParaItem);
		void InitProbe2D(ProbeSocket::ProbePara* ptrPara, ExamItem::ParaItem* ptrParaItem);
		void InitProbeMOptimize(ProbeSocket::ProbePara* ptrPara, ExamItem::ParaItem* ptrParaItem);
		void InitProbeM(ProbeSocket::ProbePara* ptrPara, ExamItem::ParaItem* ptrParaItem);
		void GetCurPara(ExamItem::ParaItem* ptrParaItem);
        bool GetHarminocStatus(void){return m_harmonic;}
        int CalcFPS();
        void EnableEmit();
        EKnobReturn ChangeCompoundSpace(void);
        EKnobReturn ChangeCompoundFreq(void);
        EKnobReturn ChangeMBP(EKnobOper oper);
       EKnobReturn ChangeFreq(EKnobOper oper);
        EKnobReturn ChangeDepth(EKnobOper oper);
        void ChangeTgc2D(int tgcY[8]);
        void ChangeGain2D(int gain);
#ifdef TRANSDUCER
     void ChangeTransducer(int transducer);
#endif
        EKnobReturn ChangeGain2D(EKnobOper oper);
        EKnobReturn ChangeFocSum(EKnobOper oper);
		bool SetFocSum(int sum);
		EKnobReturn ChangeFocPos(EKnobOper oper);
		bool SetFocPos(int posIndex);
        void GetDisplayScanAngle(float& scanangle);
		EKnobReturn SetScanAngle(int index);
        EKnobReturn ChangeScanAngle(EKnobOper oper);
        EKnobReturn ChangeScanLines(EKnobOper oper);
		EKnobReturn ChangeDynamicRange(EKnobOper oper);
		EKnobReturn ChangeLineDensity(EKnobOper oper);
        //void SetLineDensity(int index); // index[0,1]
        void SetLineDensity(int index, bool update=true); // index[0,1]
        void RecoverLineDensity(); // index[0,1]
		EKnobReturn ChangeEdgeEnhance(EKnobOper oper);
		EKnobReturn ChangeSoundPower(EKnobOper oper);
		EKnobReturn ChangeHarmonic(EKnobOper oper);
        EKnobReturn ChangeHarmonicStatus(EKnobOper oper);
        EKnobReturn ChangeD2HarmonicFreq(EKnobOper oper);
        //void SetMBP(int mbpIndex);
	    void SetMBP(int mbpIndexi, bool update=true);
	    void RecoverMBP();
		int ChangeMBP();
        EKnobReturn ChangeTSI(EKnobOper oper);
        EKnobReturn ChangeAGC(EKnobOper oper);///< not realize now
        void ChangeSteer();
        EKnobReturn ChangeSteer(EKnobOper oper);
        int GetSteer();
        void SetSteer(int& index);
        EKnobReturn ChangeTpView(EKnobOper oper);
        EKnobReturn ChangeEFVI(EKnobOper oper);


		///>operation of M mode
		void ResetMLine();
		void ClearMLine();
		void ReDrawMLine();
        void ChangeTgcM(int tgcY[8]);
        void ChangeGainM(int gain);
        EKnobReturn ChangeGainM(EKnobOper oper);
        void ReSendGainM(void);
        void ReSendGain2D(void);
        bool ChangeMLine(int offsetX, int offsetY);
		EKnobReturn ChangeMSpeed(EKnobOper oper);
		EKnobReturn ChangeCompoundSpace(EKnobOper oper);
		EKnobReturn ChangeCompoundFreq(EKnobOper oper);
        //EKnobReturn SetCompoundSpace(int index);
        EKnobReturn SetCompoundSpace(int index, bool draw=true);
        EKnobReturn RecoverCompoundSpace();
        //EKnobReturn SetCompoundFreq(bool on);
        EKnobReturn SetCompoundFreq(bool on, bool draw=true);
        EKnobReturn RecoverCompoundFreq();
        bool IsCompoundSpaceOn(void);
        bool IsCompoundFreqOn(void);
		void Change2DTis();
        EKnobReturn ChangePolarity(EKnobOper oper);

		///> call by other class
		void SetCfmTis(int dopFreq, int prfIndex, int boxLineBegin, int boxLineEnd);
		void SetPwTis(int dopFreq, int prfIndex);
        void SetCwLine(int line);

		///> call by other class
		void GetScanRange(int scanRange[2]);
		void GetDisplayScanRange(int scanRange[2]);
		int GetOutputPower(void);
		int GetLDensity(void); ///< LD index[0,1], 0-low, 1-high
		int GetMBP(void);//< MBP=[1,2,4]
        int GetMBPIndex();
		int GetMBPFpga2D(void);
		int GetMBPFpgaColor(void);
		double GetSoundSpeed(void); // km/s
		int GetImgStartDots();
		int GetScanLines();
        int GetScanLineIndex() { return m_indexScanLines;} ///<index of scan lines
		double GetWidthBetweenLinesL();
		int GetDepthIndex();
		int GetDepth();
        int GetFocSum(void);
        bool GetHarmonicStatus(void);
        bool GetEFVIStatus(void);
        bool GetTpViewStatus(void);
        int  GetTpViewIndex(void) { return m_tpViewIndex;}
        void ExitTpView(void);
        void ExitEFVI(void);

		double GetScale2D(); ///<scale of 2D no matter in what format, and you can also get 2D scale in each place 2D image appers(for example PW mode) unit: mm/dot
		void GetAllScale2D(double scale[4]);
		double GetScaleMDepth(); //unit of scaleDepth: mm/dots;
		double GetScaleMTime(); //unit of scaleTime: second/line
		double GetScaleAnatomicMTime(); //unit of scaleTime: second/line
		double GetScale2DInImgHDot(); ///<未经格式变化、未经局部放大，以IMAGE_H点采样，当前深度下的原始比率尺。
		void CalcEmitDelayPw(float focPos);
		void CalcEmitDelayCfm(float focPos);
		void CalcReceiveDelayColor(void);
		void SetSteerColor(int angle);
        void CalcTgc(int gain, int tgcY[8], int section);
        void CalcTgcDigital(int gain, int section, int maxValue);
        void ExitSimult();

		void ChangeSeperateScale(int destIndex, int srcIndex);
        void InitSeperateScale(int srcIndex);
		void UpdateSeperateScaleWhenUnfreeze(void);

		void Enter2D();
		void EnterM();
		void CalcSampleAgain(void);

		void EnterEFOV();
		void ExitEFOV();
		double GetScaleEFOV();

        ///> call for snap's measure
        void SetScaleForSpecialMeasure(double scale2DAll[4], double scale2D, double scaleMDepth, double scaleMTime);

        ///> call for measure when local zoom
        void SetScale2DZoomMeasure(double scale);
        void RestoreScale2DZoomMasure(void);
		void ResetFocPosWhenZoom(void);
        void GetScanAngle(int& ScanAngle);
		char ReviseProbeType(char type);

        int GetCompoundSpaceIndex(void) { return m_spaceCompoundIndexBak; }
        bool GetCompoundFreqCtrl(void) { return m_freqCompoundCtrlBak; } //m_freqCompoundCtrl; }



        ///>
        void UpdateAutoOptimize(bool on);
        void ModeCwCtrl(bool on);

        ///> single aperture
        void EnterSingleAperture(bool on);
        char GetReviseProbeType(void);

        //cw for testing
        void SendChannelCW(int num);

        //status of thi
        void SetStatusTHI(bool status) { m_statusTHI = status; }
        bool GetStatusTHI() { return m_statusTHI; }
        void SetScanLines(int index);

        //test
        void ChangeFreqDynamicFilter_test(EKnobOper oper, int segment);
        void ChangeFreqBPFilter_test(EKnobOper oper, int segment);
        void ChangeBPFilter(EKnobOper oper);
        void ChangeDynamicDemodFd(EKnobOper oper, int segment);
        void ChangeHarmonicFreq(EKnobOper oper);
        int GetHarmonicFreqIndex() { return  (m_harmonicFreqIndex); }
        int GetFreqIndex() { return m_freqIndex; }

#ifdef EMP_PROJECT
 	 int GetFreq();
        int GetHarmonicFreq();
        void ChangeFreqForProject(int freq_index);
        void ChangeHTIForProject(int harmonic_index);
#endif

        void UpdateGain();       //用来解决调节M增益后，进入CFM时，增益错误的问题
        void EnhanceBiopsy(bool on);
        void Depth(int scaleIndex);

		int GetScanAngleIndex()
		{
			return m_scanAngleIndex;
		}
#ifdef EMP_355
	int GetDepthDots() { return m_calcPara.depthDots; }
#endif

        private:
        Img2D();

		///> static const-2D
		///> focus pos(percent of depth = *FocRange[*][*]/FocusDepth)
		static const int FOC_DEPTH;///< value to calc focus position percent.
		static const unsigned char ONE_FOC_POS[18][1];
		static const unsigned char TWO_FOC_POS[11][2];
		static const unsigned char THREE_FOC_POS[8][3];
		static const unsigned char FOUR_FOC_POS[6][4];
#ifdef EMP_355
		static const int MAX_GAIN_2D = 140;
		static const int MAX_GAIN_M = 140;
#else
		static const int MAX_GAIN_2D = 255;
		static const int MAX_GAIN_M = 255;
#endif
		static const double TSI_SPEED[MAX_TSI]; //m/s

		///> static const-M
        static const int MAX_M_SPEED = 8;
        static const int MAX_HARMONIC_FREQ = 50;


         ///> extended imaging
        static const int MAX_TP_VIEW = 2;

		///> extern member
		static Img2D* m_ptrInstance;

        Calc2D* m_ptrCalc;
        Calc2D::CalcPara m_calcPara;
        AbsUpdate2D* m_ptrUpdate;
        DSCCONTROLATTRIBUTES* m_ptrDscPara;

        //ImageAreaDraw *m_ptrImgDraw;

		CDSC* m_ptrDsc;
        int m_transducer;
		///> 2D mode
		int m_tgc[8]; ///< 8 tgc
		int m_gain2D; /// 2D total gain
		int m_lines; ///< max scan lines of current probe
        int m_indexScanLines; ///<index of scan lines
        unsigned char m_type;
		int m_freqRange[2]; ///< range of freq is between [0]-[1], realFreq*20
		vector<ProbeSocket::FreqPair> m_vecFreqRange; ///< selectable freq pari(emit-recevie), realFreq*20
		int m_freqIndex; ///< index of freq in m_vecFreqRange
		int m_depthMax; ///< max depth if current probe
		ProbeSocket::FreqPair m_freq; ///< curr pair of freqency*20 //Mhz
		int m_depth; ///< curr depth //mm
		int m_imgScaleIndex; ///< index of image scale
		int m_focSum; ///< sum of focus (1~FOC_SUM)
		int m_focPosIndex; ///< curr index postion of focus
		int m_focPos[MAX_FOCUS]; //unit:mm
		int m_scanAngleIndex; /// < level of scan angle, range(0~MAX_ANGE-1)
		int m_dynamicRangeIndex; ///< level of dynamic,range(0~MAX_DYNAMIC_INDEX-1) dB
		int m_lineDensityIndex; ///< level of line density, range(0~MAX_LINE_DENSITY-1) 0:low, 1:high
		int m_lineDensityIndexBak; ///< level of line density, range(0~MAX_LINE_DENSITY-1) 0:low, 1:high
		int m_edgeEnhanceIndex; ///< level of edge enhance, range(0~MAX_EDGE-1)
        int m_edgeEnhanceIndexBak;
		int m_soundPowerIndex; ///< level of edge sound power, range(0~MAX_POWER_INDEX-1)
		bool m_harmonic; ///< harmonic control, TRUE = on, FALSE = off
        int m_harmonicFreqIndex; ///<index of harmonic freq
		vector<int> m_vecHarFreqRange; ///< selectable harmonic freq, realFreq*20
        int m_tsiIndex; ///< index of tissue synchronization imaging, 常规／肌肉／液体／脂肪
		int m_mbpIndex; ///< index of multi-beam parallel imaging, 1/2/4
		int m_mbpIndexBak; ///< index of multi-beam parallel imaging, 1/2/4
		int m_agcIndex; ///< level of agc
		int m_steerIndex; ///< level of steer
        int m_steerIndexbak; ///< save level of steer from config
        bool m_statusTHI;
		bool m_polarity;

		///> M mode
		int m_gainM; ///< M total gain
		int m_mLine; ///< number of m sample line(M cursor)
		int m_mSpeedIndex; ///< level of m speed

		double m_scale2D[4]; ///< scale of each B, in BB format only [0]-[1] is valid. Note: [0] is used in single B of 2D and M mode , so you can thought
		double m_scale2D_bak[4]; ///< scale of each B, in BB format only [0]-[1] is valid. Note: [0] is used in single B of 2D and M mode , so you can thought
								///< M mode always in single B format

        ///> scale for snap
        double m_scale2DAllSnap[4];
        double m_scale2DSnap;
        double m_scaleMDepthSnap;
        double m_scaleMTimeSnap;

        //efov
        unsigned char m_frameAverEfovBak;
        unsigned char m_lineAverEfovBak;
		int m_mouseSpeedEfovBak;

        // extended imaging
        int m_tpViewIndex; ///< used in linear probe
        bool m_efviCtrl; ///< used in convex probe


        // measure when zoom
        bool m_zoomMeasureCtrl; ///<是否进入局部放大的测量状态
        double m_zoomScale; ///<局部放大的倍率
        double m_scale2DZoom[4]; ///<局部放大后的比例尺
        double m_scale2DZoom_bak[4]; ///<局部放大后的比例尺
        int m_focIndexRange[2];

        // compound
        int m_freqIndexPre;
        int m_freqIndexNext;
        int m_spaceCompoundIndex;
        int m_spaceCompoundIndexBak;
        int m_spaceCompoundIndexBakBak;
        bool m_freqCompoundCtrl;
        bool m_freqCompoundCtrlBak;
        bool m_freqCompoundCtrlBakBak;
        int m_freqCompoundIndex;

        //thi
        int m_thiFreqSum;
        int m_thiFreq[ProbeSocket::MAX_HARMONIC_FREQ];
        int m_numSum;
        int m_numSumIndex;
        static int m_data;
        int m_curProbeIndex;
        int m_curHarmonicFreqIndex;

        static float m_fc01;
        static float m_fc02;
        static float m_fc03;
        static float m_fc04;
        static float m_fc05;
        static float m_fc1;
        static float m_fc2;
        static float m_fc3;
        static float m_fc4;
        static float m_fc5;
        static int m_freqEmit;


		///> fun
		void InitProbe2DOptimizePara(ProbeSocket::ProbePara* ptrPara, ExamItem::ParaItem* ptrParaItem);
		void InitProbeMOptimizePara(ProbeSocket::ProbePara* ptrPara, ExamItem::ParaItem* ptrParaItem);
		void SetDepthDots(int dots);
		void InitProbeCalcM();
		void Freq(ProbeSocket::FreqPair freq, EKnobReturn ret);
        void HarmonicFreq(ProbeSocket::FreqPair freq, EKnobReturn ret);
        void Steer(int angle, EKnobReturn ret);
		void GetFocPos();
		void ResetFocPos(int focSum, int &focPosIndex);
		void MLine(int line);
		void SetScale2D(double scale);
		void Tsi(int index, EKnobReturn ret);
		//void LineDensity(int index, EKnobReturn ret);
		void LineDensity(int index, EKnobReturn ret, bool update=true);
		void PrintImgPara();
		void PrintCalcPara();
		void SetDscScanLine();
        EKnobReturn DisplayScanAngle(void);
		void Set2DTis(int scanAngleIndex, float focPos, int freq, int power, int depthIndex);
	//	char ReviseProbeType(char type);
		void CompoundSpaceCtrl(bool on);
		void CompoundFreqCtrl(bool on);
        void CalcFilter(void);
        bool CalcFocPulse(void);
        void GetBMLineRange(int scanRange[2]);
        void GetFocPosIndexRange(int &indexB, int &indexE);
        EKnobReturn TpViewPara(int index);
        EKnobReturn EFVIViewPara(bool on);
        EKnobReturn TpView(int index);
        EKnobReturn EFVIView(bool on);
        void SetPolarity(bool on, EKnobReturn status);
        void UpdateDsc(void);
#ifdef EMP_PROJECT
        Calc2D::ProjectCalcPara m_projectCalcPara;
 #endif
        // for 4d tgc
    public:
        int GetImgScaleFor4D();
        int GetDepthMaxFor4D();
        int GetGainFor4D();
        int* GetTGCFor4D();
        void SetGainFrom4D(int gain);
        void SetTGCFrom4D(int *tgc);
		void Set4DDepth(int depth_scale);
		void InitProbeCalc2D();
        //test
        void DefaultFreqBandPassFilter(int probeindex, int thi);
        void DefaultFreqDynamicFilter(int probeindex, int thi);
        void DefaultIndexBandPassFilter(int probeindex, int thi);
        void DefaultDynamicDemodFd(int probeindex, int thi);
        void DefaultIndexBandPassFilterBaseFreq(int probeindex, int freqindex);
};


#endif