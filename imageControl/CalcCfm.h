#ifndef CALC_CFM_H
#define CALC_CFM_H

#include "FpgaCfm.h"
#include "Colormap.h"
#include <opencv/cxcore.hpp>  
using namespace cv;

class CalcCfm
{
	public:
		struct CalcPara
		{
			double soundSpeed;
			int freq;
		};
		enum {COLOR_LENGTH = COLOR_GRADE, COLOR_WIDTH = 16, COLOR_DEPTH = COLOR_NUM, VAR_DEPTH = 7}; 

		CalcCfm();
		~CalcCfm();

		void SetCalcPara(CalcPara* calc) {m_calcPara = calc;}

		void CalcBox(int lineBegin, int lineEnd, int dotBegin, int dotEnd);
		void CalcGain(int gain);
		void CalcTurbo(int level);
		int CalcMaxPeriod(int prf, int clusterSize);
		void CalcInvert(bool on);
		void CalcColorGrayCfm(int colorCode, int baseline, int turbo, bool invert, unsigned char colorMap[COLOR_LENGTH * COLOR_WIDTH][COLOR_DEPTH]);
		void CalcColorGrayCfmVar(int colorCode, int baseline, int turbo, bool invert, unsigned char colorMap[COLOR_LENGTH * COLOR_WIDTH][COLOR_DEPTH]);
		void CalcColorGrayPdi(int colorCode, int baseline, int turbo, bool invert, unsigned char colorMap[COLOR_LENGTH * COLOR_WIDTH][COLOR_DEPTH]);
		void CalcPulseNum(int num);
		void CalcWallFilter(unsigned short *filter, int size);
		void CalcWallFilter(int wallFilterIndex, int packSize);
		void CalcPacketSize(int pktSize);
		void CalcLog();
		void CalcBaseline();
		void CalcNoise();
		void CalcSmooth();
		void CalcGainGate(int gate);
		void CalcScale(int data);
		double CalcMaxVel(int prf, int colorFreq, double soundSpeed);
		int CalcClusterSize(int prf, int depth, double soundSpeed, int mbp, int &tDepth);
		// emit delay
		// receive delay
		
		void CalcSwitchPdi(bool dir);
        void CalcLowFilter(float fccur);

	private:
		CalcPara* m_calcPara;
		FpgaCfm m_fpga;

		void Demod();
		void SmatchFilter();
        void LowFilter();
		void CalcProjectionInitialMatrix(Mat matFilter, int order, double* a, double* b, int packSize);

        static const unsigned char m_filterLowPass[30720];
        static const int FREQ_SECTION = 64;
#ifdef EMP_430
        static const int FOC_LOW_TIME = 60;
#else
        static const int FOC_LOW_TIME = 20;
#endif

};
#endif
