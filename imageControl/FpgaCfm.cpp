#include "PcieControl.h"
#include "FpgaCfm.h"
#include "UsbControl.h"

struct FpgaCfm::FpgaAddrVariableCfm FpgaCfm::fpgaCfm;

#ifdef EMP_460 //G40
	AbsUltraInterface* FpgaCfm::m_ptrInterface = PcieControl::GetInstance();
#else
    AbsUltraInterface* FpgaCfm::m_ptrInterface = EzUsb::GetInstance();
#endif


int FpgaCfm::SendVariable(INT16U data) 
{
	INT32U addr;
	INT32 ret;
	
	// clear all demod data
	addr = CFM_VARIABLE;	

	PRINTF("fpga send cfm turbo, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);		
}

/** 
* @param demod[]: demod data 16bits
* @param size: number of data 
*     
*/
int FpgaCfm::SendDemod(INT16U *demod, INT32U size) 
{
	INT32U addr;
	INT32U data;
	INT32 ret;
	
	// clear all demod data
	addr = CFM_DEMOD_ADDR;	
	fpgaCfm.m_cfmDemodAddr = 0;	
	data = fpgaCfm.m_cfmDemodAddr;

	PRINTF("fpga send cfm demod, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	//erite data to fpga
	PRINTF("fpga send cfm demod, size = %d\n", size);
	addr = CFM_DEMOD_DATA;	

	ret = m_ptrInterface->WriteBufToFpga(addr, size, BITS16, (INT8U*)demod);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	return (SUCCESS);
}

/** 
* @param filter[]: filter data 16bits
* @param size: number of data 
*     
*/
int FpgaCfm::SendMatchFilter(INT16U *filter, INT32U size) 
{
	INT32U addr;
	INT32U data;
	INT32 ret;
	
	// clear all filter data
	addr = CFM_COMBINED_ADDR;	
	data = 0;

	PRINTF("fpga send cfm match filter, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	//erite data to fpga
	PRINTF("fpga send cfm match filter, size = %d\n", size);
	addr = CFM_MATCH_FILTER;	

	ret = m_ptrInterface->WriteBufToFpga(addr, size, BITS16, (INT8U*)filter);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	return (SUCCESS);	
}

int FpgaCfm::SendLowFilter(INT16U *filter, INT32U size)
{
#if 0
	INT32U addr;
	INT32U data;
	INT32 ret;
	
	// clear all filter data
	addr = CFM_COMBINED_ADDR;	
	data = 0;

	PRINTF("fpga send cfm low filter, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	//erite data to fpga
	PRINTF("fpga send cfm low filter, size = %d\n", size);
	addr = CFM_LOW_FILTER;	

	ret = m_ptrInterface->WriteBufToFpga(addr, size, BITS16, (INT8U*)filter);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	return (SUCCESS);		
#endif
    INT32U offset = 64*32*3;
    return DynamicFilter(filter, size, offset);
}
/** 
* @param filter[]: filter data 16bits
* @param size: number of data 
*     
*/
int FpgaCfm::SendWallFilter(INT16U *filter, INT32U size) 
{
	INT32U addr;
	INT32U data;
	INT32 ret;
	signed short wfdata[26*32];

	INT32 i;
	for(i=0; i<26*32; i++)
	{
		wfdata[i] = *(filter+i);
#if 0
		PRINTF("%d,  ", wfdata[i]);
		if(i%32==31)
			PRINTF("\n");
#endif
	}
	
	// clear all filter data
	addr = CFM_COMBINED_ADDR;	
	data = 0;

	PRINTF("fpga send cfm wall filter, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	//erite data to fpga
	PRINTF("fpga send cfm wall filter, size = %d\n", size);
	addr = CFM_WALL_FILTER;	

	ret = m_ptrInterface->WriteBufToFpga(addr, size, BITS16, (INT8U*)wfdata);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	return (SUCCESS);
}

/** 
* @param log[]: filter data 16bits
* @param size: number of data 
*     
*/
int FpgaCfm::SendLog(INT16U *log, INT32U size) 
{
	INT32U addr;
	INT32U data;
	INT32 ret;
	
	// clear all filter data
	addr = CFM_COMBINED_ADDR;	
	data = 0;

	PRINTF("fpga send cfm log, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	//erite data to fpga
	PRINTF("fpga send cfm log, size = %d\n", size);
	addr = CFM_LOG;	

	ret = m_ptrInterface->WriteBufToFpga(addr, size, BITS16, (INT8U*)log);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	return (SUCCESS);	
}

/**
* @brief tissue gate
*/
int FpgaCfm::SendTissueGate(INT16U data)
{
	INT16U clearAddr;

	clearAddr=0;

	SendCombined(clearAddr,data);
	
	return(SUCCESS);
}

/**
* @brief noise Gate 
*/
int FpgaCfm::SendNoiseGate(INT16U data)
{
	INT16U clearAddr;

	clearAddr=1;

	SendCombined(clearAddr,data);
	
	return(SUCCESS);
}

/**
* @brief speed
*/
int FpgaCfm::SendSpeedGate(INT16U data)
{
	INT16U clearAddr;

	clearAddr=2;

	SendCombined(clearAddr,data);
	
	return(SUCCESS);
}

/**
* @brief variance Gate
*/
int FpgaCfm::SendVarianceGate(INT16U data)
{	
	INT16U clearAddr;

	clearAddr=11;

	SendCombined(clearAddr,data);
	
	return(SUCCESS);

}

/**
* @brief sample
*/
int FpgaCfm::SendSampleFactor(INT16U data)
{
	INT16U clearAddr;

	clearAddr=3;

	SendCombined(clearAddr,data);
	
	return(SUCCESS);
}

/**
* @brief gain gate 
*/
int FpgaCfm::SendGainGate(INT16U data)
{
	INT16U clearAddr;

	clearAddr=12;

	SendCombined(clearAddr,data);
	
	return(SUCCESS);
}

/**
* @brief flat
*/
int FpgaCfm::SendSmooth(INT16U data)
{
	INT16U clearAddr;

	clearAddr=4;

	SendCombined(clearAddr,data);
	
	return(SUCCESS);
}

/**
* @brief colorline
*/
int FpgaCfm::SendBaseline(INT16U data)
{
	INT16U clearAddr;

	clearAddr=5;

	SendCombined(clearAddr,data);
	
	return(SUCCESS);
}

/**
* @brief scanbegin  
* @param data begin from 0 and it's Initial value is 100
*/
int FpgaCfm::SendScanBegin(INT16U data)
{
	INT16U clearAddr;

	clearAddr=6;

	SendCombined(clearAddr,data);
	
	return(SUCCESS);
}

/**
* @brief scanend
* @param data begin from 0 and it's initial value is 149
*/
int FpgaCfm::SendScanEnd(INT16U data)
{
	INT16U clearAddr;

	clearAddr=7;

	SendCombined(clearAddr,data);
	
	return(SUCCESS);
}

/**
* @brief samplebegin
* @param data begin from 0 and it's initial value is 50
*/
int FpgaCfm::SendSampleBegin(INT16U data)
{
	INT16U clearAddr;

	clearAddr=8;

	SendCombined(clearAddr,data);
	
	return(SUCCESS);
}
/**
* @brief sampleend
* @param data begin from 0 and it's initial value is 210
*/
int FpgaCfm::SendSampleEnd(INT16U data)
{
	INT16U clearAddr;

	clearAddr=9;

	SendCombined(clearAddr,data);
	
	return(SUCCESS);
}

/**
* @brief samplepart
* @param  data begin from 0 and it's initial value is 0
*/
int FpgaCfm::SendSampleSection(INT16U data)
{
	INT16U clearAddr;

	clearAddr=10;

	SendCombined(clearAddr,data);

	return(SUCCESS);
}

/**
* @brief  pw color bands 
*/
int FpgaCfm::SendColorBands(INT16* colorBands, int size)
{
	INT32U addr;
	INT32U data;
	INT32 ret;
	
	// clear all filter data
	addr = CFM_COMBINED_ADDR;	
	data = 0;

	PRINTF("fpga send cfm color band, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	//erite data to fpga
	PRINTF("fpga send cfm color bands, size = %d\n", size);
	addr = CFM_COLOR_BAND;	

	ret = m_ptrInterface->WriteBufToFpga(addr, size, BITS8, (INT8U*)colorBands);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	return (SUCCESS);		
}

int FpgaCfm::SendPdiDirMode(bool on)
{
	INT16U clearAddr;
	INT16U data = (INT16U)on;

	clearAddr=13;

	SendCombined(clearAddr,data);

	return (SUCCESS);		
}

int FpgaCfm::SendMaxPeriod(INT16U maxPeriod)
{	
	INT32U addr;
	INT32U data;
	INT32U temp;
	INT32 ret;

	// clear
	addr = MAXPERIOD_ADDR;
	data = 1;
	PRINTF("fpga clear max_period_addr , addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	
	// send data
	addr = PROBE_EMIT2;
	temp = maxPeriod & (0xffff << 0);
	fpgaCfm.m_cfmMaxPeriod = (fpgaCfm.m_cfmMaxPeriod & (~(0xffff << 0))) | (temp << 0);	
	data = fpgaCfm.m_cfmMaxPeriod;

	PRINTF("fpga send max period, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);		
}

int FpgaCfm::SendClusterSize(INT16U cluster)
{	
	INT32U addr;
	INT32U data;
	INT32U temp;
	INT32 ret;

	addr = MAXPERIOD_ADDR;
	data = 3;
	PRINTF("fpga clear cluster, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	
	// send data
	addr = PROBE_EMIT2;
	temp = cluster & (0xffff << 0);
	fpgaCfm.m_cfmMaxPeriod = (fpgaCfm.m_cfmMaxPeriod & (~(0xffff << 0))) | (temp << 0);	
	data = fpgaCfm.m_cfmMaxPeriod;

	PRINTF("fpga send cluster, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);		
}

///>private
/** 
* @param clear_addr: clear addr 0-10
* @param cfm_data: data to send
*     
*/
int FpgaCfm::SendCombined(INT16U clearAddr, INT16U cfmData) 
{
	INT32U addr;
	INT32U data;
	INT32 ret;
	
	addr = CFM_COMBINED_ADDR;	
	data = clearAddr;

	PRINTF("1: fpga send cfm_combined, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	//erite data to fpga
	PRINTF("1: fpga send cfm_combined, data = %d\n", cfmData);
	addr = CFM_COMBINED_DATA;	

	ret = m_ptrInterface->WriteOneDataToFpga(addr, cfmData);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);	
}

//send low filter 
int FpgaCfm::DynamicFilter(INT16U *filter, INT32U size, INT32U offset) 
{
	INT32U addr;
	INT32U data;
	INT32 ret;
	
	// clear all filter data
	addr = FILTER_ADDR;	
	data = offset;

	PRINTF("fpga send low filter, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	//erite data to fpga
	PRINTF("fpga send filter, size = %d \n", size);
	addr = FILTER_DATA;	

	ret = m_ptrInterface->WriteBufToFpga(addr, size, BITS16, (INT8U*)filter);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);	
}



