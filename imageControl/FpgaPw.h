#ifndef __FPGAPW_H__
#define __FPGAPW_H__	

#include "DefHal.h"
#include "AbsUltraInterface.h"

class FpgaPw
{
	public:

	int SendNoise(INT16 data);
	
	int SendDyn(INT16 data);

    int SendGainCw(INT16 dataCw);

	int SendBaseLine(INT16 data);
	
	int SendLogRank(INT16 data);
	
	int SendWfFc(INT32U *filter, INT32U size);
	
	int SendSampleBegin(INT16 data);
	
	int SendSampleEnd(INT16 data);
	
	int SendScanLine(INT16 data);

	int SendSpectrumSpeed(INT16 data);

	int SendSoundVolume(INT16 index);
	
	int SendSoundFilter(INT16U *filter, INT32U size);

	int SendSpectrumInvert(INT32U num);

	int SendMaxPeriod(INT32U maxPeriod);

	int SendHPRF(bool on);

	int SendMBP(INT16U index);
	
	int SendSoundIntpCoef(INT16U *value, INT32U size);

	int SendSoundIntpNum(INT32U value);

	int SendSoundStatus(INT16 data);

	int SendHPRFEmitNum(INT16 data);
	
	int SendPwSample(INT32 sample);
	
	int SendPwRGBSampleDots(INT32 sample);
	
	int SendPwRGBSampleCoef(INT16U *table, INT32U size);

	int SendPwSoundStopDot(INT32 dots);
	
	int SendPwImgCtrl(bool on);

	int SendPwImgCtrlPara(INT32U offLineNum);

	int SendCwImgCtrl(bool on);
	private:

	//PW
	enum
	{
		FPGA_DATA=4,
		TGC_ADDR=5,
		TGC_DATA=7,

        CW_COMBINE_ADDR=28,
        CW_COMBINE_DATA=29,
		HPRF_MBP=30,
		PW_COMBINED_ADDR=31,
		PW_COMBINED_DATA=32,
		PW_SOUND_DATA=36,
		MAXPERIOD_ADDR=37,
		MAXPERIOD_DATA=39,
		SOUND_INTPCOEF_DATA=41,
		WALL_FILTER_DATA=42,
		PW_RGB_SAMPLE_COEF=44,
	};
	
	struct FpgaAddrVariablePw 
	{
		INT32U m_pwCombinedAddr;
		INT32U m_pwCombinedData;
		INT32U m_pwSoundFilterAddr;
		INT32U m_pwMaxperiod;
		INT32U m_pwHPRF;
	};
	static struct FpgaAddrVariablePw fpgaPw;
	static AbsUltraInterface* m_ptrInterface;
	
	int SendPwCombined(INT16U clearAddr, INT16U pwData);
	int SendCwCombined(INT16U clearAddr, INT16U cwData);
};


#endif
