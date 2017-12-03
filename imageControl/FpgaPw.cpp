#include "PcieControl.h"
#include "FpgaPw.h"
#include "Def.h"
#include "UsbControl.h"

struct FpgaPw::FpgaAddrVariablePw FpgaPw::fpgaPw;
#ifdef EMP_460
    AbsUltraInterface* FpgaPw::m_ptrInterface = PcieControl::GetInstance();
#else
    AbsUltraInterface* FpgaPw::m_ptrInterface = EzUsb::GetInstance();
#endif

/**
* @brief noise threshold 
* @data range:0~60  initial value: 30
*/
int FpgaPw::SendNoise(INT16 data)
{
	INT16U clearAddr;

	clearAddr=0;
	PRINTF("0: fpga send pw_combined noise threshold data = %d\n", data);
	SendPwCombined(clearAddr,data);	

	return (SUCCESS);
}
/**
 * @brief gainCw
 * @
 */
int FpgaPw::SendGainCw(INT16 data)
{
    INT16U clearAddr;
    
    clearAddr=20;
    PRINTF("20:fpga send pw_combined GainCw data =%d\n",data);
    SendPwCombined(clearAddr,data);

    return (SUCCESS);

}


/**
* @brief dyn
* @data range:0~120  initial value: 60
*/
int FpgaPw::SendDyn(INT16 data)
{
	INT16U clearAddr;

	clearAddr=1;
	PRINTF("1: fpga send pw_combined Dyn data = %d\n", data);
	SendPwCombined(clearAddr,data);	

	return (SUCCESS);
}

/**
* @brief base
* @data range -64~64(0-128) initial value:0
*/
int FpgaPw::SendBaseLine(INT16 data)
{
	INT16U clearAddr;

	clearAddr=2;
	PRINTF("2: fpga send pw_combined baseline =%d \n", data);
	SendPwCombined(clearAddr,data);	

	return (SUCCESS);
}

/**
* @brief logrank
* @param data  initial value is 1179
*/
int FpgaPw::SendLogRank(INT16 data)
{
	INT16U clearAddr;

	clearAddr=3;
	PRINTF("3: fpga send pw_combined logrank = %d\n", data);
	SendPwCombined(clearAddr,data);	

	return (SUCCESS);
}

/**
* @brief wf-fc
*/
int FpgaPw::SendWfFc(INT32U *filter, INT32U size)
{
	INT32U addr;
	INT32U data;
	INT32 ret;
	INT16U clearAddr;

	clearAddr=4;
	data = 0;
	PRINTF("fpga send pw_combined clear wall filter addr = %d, data = %d\n", clearAddr, data);
	SendPwCombined(clearAddr,data);	

#if 0
	unsigned int i;
	for(i = 0; i < size * 2; i++)
	{
		PRINTF(" wF: filter[%d] = %d\n", i, *((unsigned char*)filter+i));
	}
#endif

	//erite data to fpga
	PRINTF("1: fpga send pw_combined, size = %d\n", size);
	addr = WALL_FILTER_DATA;	
	ret = m_ptrInterface->WriteBufToFpga(addr, 14, BITS16, (INT8U*)filter);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	return (SUCCESS);	
}

/**
* @brief pwbegin
* @param data 0-200
*/
int FpgaPw::SendSampleBegin(INT16 data)
{
	INT16U clearAddr;

	clearAddr=5;

	PRINTF("fpga send pw_combined sample begin = %d\n", data);
	SendPwCombined(clearAddr,data);	

	return (SUCCESS);
}

/**
* @brief pwend
* @param data 0-220
*/
int FpgaPw::SendSampleEnd(INT16 data)
{
	INT16U clearAddr;

	clearAddr=6;

	PRINTF("fpga send pw_combined sample end = %d\n", data);
	SendPwCombined(clearAddr,data);	

	return (SUCCESS);
}

/**
* @brief pwscan
* @param data 0-255  initial value: 128
*/
int FpgaPw::SendScanLine(INT16 data)
{
	INT16U clearAddr;

	clearAddr=7;

	data = data / 2 * 2;

	PRINTF("fpga send pw_combined scan line = %d\n", data);
	SendPwCombined(clearAddr,data);	

	return (SUCCESS);
}

/**
* @brief spectrum
* @param data 0,1,2,3  initial value: 0
*/
int FpgaPw::SendSpectrumSpeed(INT16 data)
{
	INT16U clearAddr;

	clearAddr=8;

	PRINTF("------------------------------fpga send pw_combined spetrum speed = %d\n", data);
	SendPwCombined(clearAddr,data);	

	return (SUCCESS);
}

/**
* @brief sound
* @param data 100-1000  initial value: 1000
*/
int FpgaPw::SendSoundVolume(INT16 index)
{	
	INT32U addr;
	INT32U data;
	INT32 ret;
	
	addr = HPRF_MBP;
	fpgaPw.m_pwHPRF = (fpgaPw.m_pwHPRF & (~(0x3F << 0))) | (index << 0);
	data = fpgaPw.m_pwHPRF;

	PRINTF("fpga send pw hprf-soundVolume, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);	
}

/**
* @brief  pw sound filter 
* @param filter[]  16bits (0-15)   
* @param size   initial value 0
*/
int FpgaPw::SendSoundFilter(INT16U *filter, INT32U size)
{
	INT32U addr;
	INT32U data;
	INT32 ret;
	
	// clear all filter data
	addr = PW_COMBINED_ADDR;	
	fpgaPw.m_pwSoundFilterAddr = 15; 
	data = fpgaPw.m_pwSoundFilterAddr;
	PRINTF("fpga send pw sound filter, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	
	//erite data to fpga
	PRINTF("fpga send pw sound filter, size = %d\n", size);
#if 0
	unsigned int i;
	for(i = 0; i < size; i++)
	{
		PRINTF("filter[%d] = %d\n", i, filter[i]);
	}
#endif
	addr = PW_SOUND_DATA;	
	
	ret = m_ptrInterface->WriteBufToFpga(addr, size, BITS16, (INT8U*)filter);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	return (SUCCESS);	
}

/**
* @brief  pw spectrum overturn 
* @param invert 1bits (1)   0: normal  1:invert
*/
int FpgaPw::SendSpectrumInvert(INT32U invert)
{
	INT16U clearAddr;

	clearAddr=12;

	PRINTF("fpga send pw_combined invert= %d\n", invert);
	SendPwCombined(clearAddr,invert);	

	return (SUCCESS);
}

int FpgaPw::SendMaxPeriod(INT32U maxPeriod)
{
	INT32U addr;
	INT32U data;
	INT32U temp;
	INT32 ret;
	
	addr = MAXPERIOD_ADDR;
	data = 2;
	PRINTF("fpga send pw max_period_addr , addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	addr = MAXPERIOD_DATA;
	temp = maxPeriod & (0xffff << 0);
	fpgaPw.m_pwMaxperiod = (fpgaPw.m_pwMaxperiod & (~(0xffff << 0))) | (temp << 0);	
	data = fpgaPw.m_pwMaxperiod;

	PRINTF("fpga send pw max_period_data, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);	
}

int FpgaPw::SendHPRF(bool on)
{
	INT32U addr;
	INT32U data;
	INT32 ret;
	
	addr = HPRF_MBP;
	if (on)
		data = 1;
	else
		data = 0;

	fpgaPw.m_pwHPRF = (fpgaPw.m_pwHPRF & (~(0x1 << 6))) | (data << 6);
	data = fpgaPw.m_pwHPRF;

	PRINTF("fpga send pw hprf, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);	
}

/**
* @brief send mbp
* @param index the value of pw mbp, HPRF_MBP(8~10bit)
*/
int FpgaPw::SendMBP(INT16U index)
{
	INT32U addr;
	INT32U data;
	INT32 ret;
	
	addr = HPRF_MBP;
	fpgaPw.m_pwHPRF = (fpgaPw.m_pwHPRF & (~(0x7 << 8))) | (index << 8);
	data = fpgaPw.m_pwHPRF;

	PRINTF("fpga send pw hprf-mbp, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);	

}

/**
* @brief  pw sound interpolation coeff 
* @param data[]  16bits (0-15)   
* @param size   initial value 0
*/
int FpgaPw::SendSoundIntpCoef(INT16U *value, INT32U size)
{
	INT32U addr;
	INT32U data;
	INT32 ret;
	INT16U clearAddr;

	clearAddr=9;
	data = 0;
	PRINTF("fpga send pw sound interpolation coeff, addr--data: 0x%x--0x%x\n", clearAddr, data);
	SendPwCombined(clearAddr,data);	
	
	//erite data to fpga
	PRINTF("fpga send pw sound interpolation coeff, size = %d\n", size);
#if 0
	unsigned int i;
	for(i = 0; i < size; i++)
	{
		PRINTF("value[%d] = %d\n", i, value[i]);
	}
#endif
	addr = SOUND_INTPCOEF_DATA;	
	
	ret = m_ptrInterface->WriteBufToFpga(addr, size, BITS16, (INT8U*)value);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	return (SUCCESS);	
}

/**
* @brief send sound interpolation num
* @param value the value of sound interpolation num(16bit)
*/
int FpgaPw::SendSoundIntpNum(INT32U value)
{
	INT32U data;
	INT16U clearAddr;

	clearAddr=13;
	data = (value << 0);
	PRINTF("fpga send sound interpolation num, addr--data: 0x%x--0x%x\n", clearAddr, data);
	SendPwCombined(clearAddr,data);	

	return (SUCCESS);	
}


/**
* @brief sound status
* @data range:0:sound off, 1:sound on  initial value: 0
*/
int FpgaPw::SendSoundStatus(INT16 data)
{
	INT16U clearAddr;

	clearAddr=10;
	PRINTF("0: fpga send pw_combined sound status data = %d\n", data);
	SendPwCombined(clearAddr,data);	

	return (SUCCESS);
}


/**
* @brief HPRF emit num.
* @data range:PRF:0; HPRF:emitNum-1, base on 0.
*/
int FpgaPw::SendHPRFEmitNum(INT16 data)
{
	INT16U clearAddr;

	clearAddr=11;
	PRINTF("1: fpga send pw_combined HPRF emit num data = %d\n", data);
	SendPwCombined(clearAddr,data);	

	return (SUCCESS);
}

/*
 * @brief send pw sample rate to fpga 60Mhz 460dots
 */
int FpgaPw::SendPwSample(INT32 sample)
{
	INT32U addr;
	INT32U data;
	INT32 ret;

	// clear
	addr = TGC_ADDR;
	data = 6;
	PRINTF("fpga send clear pw sample, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	
	addr = FPGA_DATA;
	data = sample;

	PRINTF("fpga send pw sample, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);
}

int FpgaPw::SendPwRGBSampleDots(INT32 sample)
{
	INT16U clearAddr;
	INT32U data;

	clearAddr=14;
	data = sample;
	PRINTF("0: fpga send pw_combined rgb sample dots data = %d\n", data);
	SendPwCombined(clearAddr,data);	

	return (SUCCESS);

}

int FpgaPw::SendPwRGBSampleCoef(INT16U *table, INT32U size)
{
	INT32U addr;
	INT32U data;
	INT32 ret;
	INT16U clearAddr;

	clearAddr=15;
	data = 0;
	PRINTF("fpga send pw rgb sample coeff, addr--data: 0x%x--0x%x\n", clearAddr, data);
	SendPwCombined(clearAddr,data);	
	
	//send data to fpga
	PRINTF("fpga send pw rgb sample coeff, size = %d\n", size);
#if 0
	unsigned int i;
	for(i = 0; i < size; i++)
	{
		PRINTF("table[%d] = %d\n", i, table[i]);
	}
#endif
	addr = PW_RGB_SAMPLE_COEF;	
	
	ret = m_ptrInterface->WriteBufToFpga(addr, size, BITS16, (INT8U*)table);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	return (SUCCESS);	
}
int FpgaPw::SendPwSoundStopDot(INT32 dots)
{
	INT16U clearAddr;
	INT32U data;

	clearAddr=16;
	data = dots;
	PRINTF("0: -------fpga send pw_combined sound stop dots data = %d\n", data);
	SendPwCombined(clearAddr,data);	

	return (SUCCESS);
}
	
int FpgaPw::SendPwImgCtrl(bool on)
{
	INT16U clearAddr;
	INT32U data;

	clearAddr=17;
	data = (int)on;
	PRINTF("0: -------fpga send pw_combined pw img display ctrl = %d\n", data);
	SendPwCombined(clearAddr,data);	

	return (SUCCESS);
}

int FpgaPw::SendPwImgCtrlPara(INT32U offLineNum)
{
	INT16U clearAddr;
	INT32U data;

	clearAddr=18;
	data = offLineNum;
	PRINTF("0: -------fpga send pw_combined pw img ctrl line off = %d\n", data);
	SendPwCombined(clearAddr,data);	
	
	return (SUCCESS);

}

int FpgaPw::SendCwImgCtrl(bool on)
{
	INT16U clearAddr;
	INT32U data;

	clearAddr=4;
	data = (int)on;
	PRINTF("0: -------fpga send cw_combined cw img display ctrl = %d\n", data);
	SendCwCombined(clearAddr,data);	

	return (SUCCESS);
}

///>private
/** 
* @param clear_addr: clear addr 0-9
* @param pw data: data to send
*     
*/
int FpgaPw::SendPwCombined(INT16U clearAddr, INT16U pwData) 
{
	INT32U addr;
	INT32U data;
	INT32 ret;
	
	addr = PW_COMBINED_ADDR;	
	fpgaPw.m_pwCombinedAddr= clearAddr;	
	data = fpgaPw.m_pwCombinedAddr;

	PRINTF("1: fpga send pw_combined clear, addr--data: %d--%d\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	//write data to fpga
	addr = PW_COMBINED_DATA;	
    data = pwData;
	PRINTF("1: fpga send pw_combined send data, addr--data: %d--%d\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);	
}

/** 
* @param clear_addr: clear addr 0-9
* @param cwData: data to send
*     
*/
int FpgaPw::SendCwCombined(INT16U clearAddr, INT16U cwData) 
{
	INT32U addr;
	INT32U data;
	INT32 ret;
	
	addr = CW_COMBINE_ADDR;	
	data = clearAddr;

	PRINTF("1: fpga send cw_combined clear, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	//write data to fpga
	addr = CW_COMBINE_DATA;	
    data = cwData;
	PRINTF("1: fpga send cw_combined send data, addr--data: 0x%x--0x%x\n", addr, data);

	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);	
}




