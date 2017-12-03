#ifndef __FPGACFM_H__
#define __FPGACFM_H__

#include "DefHal.h"
#include "AbsUltraInterface.h"

class FpgaCfm
{
	public:

	/** 
	* @param demod[]: demod data 16bits
	* @param size: number of data 
	*/
	int SendVariable(INT16U data);
	
	int SendDemod(INT16U *demod, INT32U size);

	int SendMatchFilter(INT16U *filter, INT32U size);
	
	int SendLowFilter(INT16U *filter, INT32U size);

	int SendWallFilter(INT16U *filter, INT32U size);

	int SendLog(INT16U *log, INT32U size); 

	int SendTissueGate(INT16U data);

	int SendNoiseGate(INT16U data);

	int SendSpeedGate(INT16U data);

	int SendVarianceGate(INT16U data);

	int SendSampleFactor(INT16U data);
	
	int SendGainGate(INT16U data);

	int SendSmooth(INT16U data);

	int SendBaseline(INT16U data);

	int SendScanBegin(INT16U data);

	int SendScanEnd(INT16U data);

	int SendSampleBegin(INT16U data);

	int SendSampleEnd(INT16U data);

	int SendSampleSection(INT16U data);

	int SendColorBands(INT16* colorBands, int size);

	int SendPdiDirMode(bool on);
	
	int SendMaxPeriod(INT16U maxPeriod);
	
	int SendClusterSize(INT16U cluster);
	

	private:

	enum
	{
        //CFM
        FILTER_ADDR=19, //   
        FILTER_DATA=20, //   
        CFM_DEMOD_ADDR=21,
        CFM_DEMOD_DATA=22,
		CFM_VARIABLE=23,
		CFM_MATCH_FILTER=24,
		CFM_LOW_FILTER=25,
		CFM_WALL_FILTER=26,
		CFM_LOG=27,
		CFM_COMBINED_ADDR=33,
		CFM_COMBINED_DATA=34,
		CFM_COLOR_BAND=35,

		MAXPERIOD_ADDR = 37,
		PROBE_EMIT2 = 39
	};

	struct FpgaAddrVariableCfm 
	{
		 INT32U m_cfmDemodAddr;
		 INT32U m_cfmLog;
		 INT32U m_cfmCombinedAddr;
		 INT32U m_cfmCombinedData;
		 INT32U m_cfmMaxPeriod;
	};

	static struct FpgaAddrVariableCfm fpgaCfm;
	static AbsUltraInterface* m_ptrInterface;

	/** 
	* @param clear_addr: clear addr 0-10
	* @param cfm_data: data to send
	*/
	int SendCombined(INT16U clearAddr, INT16U cfmData);

    int DynamicFilter(INT16U *filter, INT32U size, INT32U offset) ;
};


#endif
