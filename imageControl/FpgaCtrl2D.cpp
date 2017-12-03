/**
* 2009, ���ڶ��յ��Ӽ������޹�˾
* 
* @file:Fpga_control.cpp
* @brief:fpga send data
*
* version:V1.0
* date:2009-5-8 
* @author:QiuJiuming
*/

#include "PcieControl.h"
#include "FpgaCtrl2D.h"
#include "UsbControl.h"
#include <iostream>

using namespace std;

struct FpgaCtrl2D::FpgaAddrVariable2D FpgaCtrl2D::fpga;

#ifdef EMP_460 //G40
	AbsUltraInterface* FpgaCtrl2D::m_ptrInterface = PcieControl::GetInstance();
#else
    AbsUltraInterface* FpgaCtrl2D::m_ptrInterface = EzUsb::GetInstance();
#endif

/**
* @brief send focus position FOCUS_A FOCUS_B FOCUS_C FOCUS_D
*/
int FpgaCtrl2D::SendFocusPosition(INT32U focusBegin[4], INT32U focusEnd[4])
{
	INT32U addr[4];
#ifdef EMP_355
	INT32U data[5];
#else
	INT32U data[4];
#endif
	INT32 ret;

	PRINTF("fpga send focus position begin\n");

	addr[0] = FOC_A;
	addr[1] = FOC_B;
	addr[2] = FOC_C;
	addr[3] = FOC_D;

	fpga.m_focus1 = ((focusEnd[0] & 0x3ff) << 16) + (focusBegin[0] & 0x3ff);	//0xffff
	fpga.m_focus2 = ((focusEnd[1] & 0x3ff) << 16) + (focusBegin[1] & 0x3ff);	
	fpga.m_focus3 = ((focusEnd[2] & 0x3ff) << 16) + (focusBegin[2] & 0x3ff);	
	fpga.m_focus4 = ((focusEnd[3] & 0x3ff) << 16) + (focusBegin[3] & 0x3ff);	
#ifdef EMP_355
	data[0] = focusBegin[0] & 0x3ff;
    data[1] = focusEnd[0] & 0x3ff;
	data[2] = focusEnd[1] & 0x3ff;
    data[3] = focusEnd[2] & 0x3ff;
	data[4] = focusEnd[3] & 0x3ff;
#else
	data[0] = fpga.m_focus1;
	data[1] = fpga.m_focus2;
	data[2] = fpga.m_focus3;
	data[3] = fpga.m_focus4;
#endif

	PRINTF("0: focus1 addr--data: 0x%x--0x%x\n", addr[0], data[0]);
	PRINTF("0: focus2 addr--data: 0x%x--0x%x\n", addr[1], data[1]);
	PRINTF("0: focus3 addr--data: 0x%x--0x%x\n", addr[2], data[2]);
	PRINTF("0: focus4 addr--data: 0x%x--0x%x\n", addr[3], data[3]);
#ifdef EMP_355
    //focus 1
    INT8U focusIndex = 0;
	ret = m_ptrInterface->WriteOneDataToFpga(addr[0], focusIndex);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	ret = m_ptrInterface->WriteOneDataToFpga(addr[1], data[0]);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
    focusIndex = 1;
	ret = m_ptrInterface->WriteOneDataToFpga(addr[0], focusIndex);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	ret = m_ptrInterface->WriteOneDataToFpga(addr[1], data[1]);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
    //focus 2
    focusIndex = 2;
	ret = m_ptrInterface->WriteOneDataToFpga(addr[0], focusIndex);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	ret = m_ptrInterface->WriteOneDataToFpga(addr[1], data[1]);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
    focusIndex = 3;
	ret = m_ptrInterface->WriteOneDataToFpga(addr[0], focusIndex);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	ret = m_ptrInterface->WriteOneDataToFpga(addr[1], data[2]);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
    //focus 3
    focusIndex = 4;
	ret = m_ptrInterface->WriteOneDataToFpga(addr[0], focusIndex);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	ret = m_ptrInterface->WriteOneDataToFpga(addr[1], data[2]);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
    focusIndex = 5;
	ret = m_ptrInterface->WriteOneDataToFpga(addr[0], focusIndex);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	ret = m_ptrInterface->WriteOneDataToFpga(addr[1], data[3]);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
    //focus 4
    focusIndex = 6;
	ret = m_ptrInterface->WriteOneDataToFpga(addr[0], focusIndex);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	ret = m_ptrInterface->WriteOneDataToFpga(addr[1], data[3]);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
    focusIndex = 7;
	ret = m_ptrInterface->WriteOneDataToFpga(addr[0], focusIndex);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	ret = m_ptrInterface->WriteOneDataToFpga(addr[1], data[4]);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
#else
	ret = m_ptrInterface->WriteOneDataToFpga(addr[0], data[0]);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	
	ret = m_ptrInterface->WriteOneDataToFpga(addr[1], data[1]);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	ret = m_ptrInterface->WriteOneDataToFpga(addr[2], data[2]);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	ret = m_ptrInterface->WriteOneDataToFpga(addr[3], data[3]);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
#endif
	return (SUCCESS);	
}

/**
* @brief send delay to fpga
* @param delay[]: delay of emit focus
* @param size: number of delay data
* @param focus_sum: focus sum
* 
*/
int FpgaCtrl2D::SendEmitDelayEven(INT16U delay[], INT32U size, INT8U focusNumber, INT32U offset)
{
    INT32U clear;
	if (focusNumber > 3)
	{
		PRINTF("fpga send emit delay: focus sum is out of range(0-3)\n");
		return (ERR_OP);
	}
    
    clear = focusNumber * offset;
    return EmitDelay(delay, size, clear);
}

int FpgaCtrl2D::SendEmitDelayOdd(INT16U delay[], INT32U size, INT8U focusNumber, INT32U offset)
{
    INT32U clear;
	if (focusNumber > 3)
	{
		PRINTF("fpga send emit delay: focus sum is out of range(0-3)\n");
		return (ERR_OP);
	}
    
    clear = 512 + focusNumber * offset;
    return EmitDelay(delay, size, clear);
}

int FpgaCtrl2D::SendEmitDelayEvenSpaceCompound1(INT16U delay[], INT32U size, INT8U focusNumber, INT32U offset)
{
    INT32U clear;
	if (focusNumber > 3)
	{
		PRINTF("fpga send emit delay: focus sum is out of range(0-3)\n");
		return (ERR_OP);
	}
    
    clear = 1024 + focusNumber * offset;
    return EmitDelay(delay, size, clear);
}

int FpgaCtrl2D::SendEmitDelayOddSpaceCompound1(INT16U delay[], INT32U size, INT8U focusNumber, INT32U offset)
{
    INT32U clear;
	if (focusNumber > 3)
	{
		PRINTF("fpga send emit delay: focus sum is out of range(0-3)\n");
		return (ERR_OP);
	}
    
    clear = 512 + 1024 + focusNumber * offset;
    return EmitDelay(delay, size, clear);
}

int FpgaCtrl2D::SendEmitDelayEvenSpaceCompound2(INT16U delay[], INT32U size, INT8U focusNumber, INT32U offset)
{
    INT32U clear;
	if (focusNumber > 3)
	{
		PRINTF("fpga send emit delay: focus sum is out of range(0-3)\n");
		return (ERR_OP);
	}
    
    clear = 2048 + focusNumber * offset;
    return EmitDelay(delay, size, clear);
}

int FpgaCtrl2D::SendEmitDelayOddSpaceCompound2(INT16U delay[], INT32U size, INT8U focusNumber, INT32U offset)
{
    INT32U clear;
	if (focusNumber > 3)
	{
		PRINTF("fpga send emit delay: focus sum is out of range(0-3)\n");
		return (ERR_OP);
	}
    
    clear = 512 + 2048 + focusNumber * offset;
    return EmitDelay(delay, size, clear);
}

int FpgaCtrl2D::SendEmitDelayEvenCFM(INT16U delay[], INT32U size, INT32U offset, INT8U phaseProbe)
{
	INT32U clear;

    if (phaseProbe == 1)
    {
        clear = 1 * offset;
    }
    else
    {
        clear = 256; 
    }

    return EmitDelay(delay, size, clear);
}

int FpgaCtrl2D::SendEmitDelayOddCFM(INT16U delay[], INT32U size, INT32U offset, INT8U phaseProbe)
{
	INT32U clear;

    if (phaseProbe == 1)
    {
        clear = 1 * offset;
    }
    else
    {
        clear = 512 + 256; 
    }

    return EmitDelay(delay, size, clear);
}

int FpgaCtrl2D::SendEmitDelayEvenPW(INT16U delay[], INT32U size, INT32U offset, INT8U phaseProbe)
{
	INT32U clear;

    if (phaseProbe == 1)
    {
        clear = 1 * offset;
    }
    else
    {
        clear = 320; 
    }

    return EmitDelay(delay, size, clear);
}

int FpgaCtrl2D::SendEmitDelayOddPW(INT16U delay[], INT32U size, INT32U offset, INT8U phaseProbe)
{
	INT32U clear;

    if (phaseProbe == 1)
    {
        clear = 1 * offset;
    }
    else
    {
        clear = 512 + 320; 
    }

    return EmitDelay(delay, size, clear);
}

/**
* @brief send tgc to fpga
* @param tgc[]: data of tgc
* @param size: number of tgc
* 
*/
int FpgaCtrl2D::SendTgc2D(INT8U tgc[], INT32U size)
{
	INT32U clearAddr = 0;

	TgcCombine(clearAddr, tgc, size);
	return (SUCCESS);	
}
int FpgaCtrl2D::SendTgcPw(INT8U tgc[], INT32U size)
{
	INT32U clearAddr = 256;

	TgcCombine(clearAddr, tgc, size);
	return (SUCCESS);	
}
int FpgaCtrl2D::SendTgcCfm(INT8U tgc[], INT32U size)
{
	INT32U clearAddr = 512;

	TgcCombine(clearAddr, tgc, size);
	return (SUCCESS);	
}

int FpgaCtrl2D::SendTgcDigital2D(INT16U tgc[], INT32U size)
{
    INT32U clearAddr = 3;
    return DemodCombineBuf(tgc, size, clearAddr);
}
int FpgaCtrl2D::SendTgcDigitalCycle2D(INT16U cycle)
{
    INT32U clearAddr = 4;
    INT16U data = cycle;
    return DemodCombineBuf(&data, 1, clearAddr);
}
int FpgaCtrl2D::SendTgcDigitalPw(INT16U tgc[], INT32U size)
{
    INT32U clearAddr = 7;
    return DemodCombineBuf(tgc, size, clearAddr);
}
int FpgaCtrl2D::SendTgcDigitalCyclePw(INT16U cycle)
{
    INT32U clearAddr = 8;
    INT16U data = cycle;
    return DemodCombineBuf(&data, 1, clearAddr);
}

int FpgaCtrl2D::SendTgcDigitalCfm(INT16U tgc[], INT32U size)
{
    INT32U clearAddr = 5;
    return DemodCombineBuf(tgc, size, clearAddr);
}
int FpgaCtrl2D::SendTgcDigitalCycleCfm(INT16U cycle)
{
    INT32U clearAddr = 6;
    INT16U data = cycle;
    return DemodCombineBuf(&data, 1, clearAddr);
}

/**
* @brief send scanrange
* @param scan_begin: begin of scan line  begin-8bits(8-15)
* @param scan_end: end of scan line  end-8bits(0-7)
* 
*/
int FpgaCtrl2D::SendScanrange(INT32U scanBegin, INT32U scanEnd)
{
	INT32U addr;
	INT32U data;
	INT8U begin, end;
	INT32 ret;
	
	addr = SCAN_RANGE;

	begin = scanBegin & 0xff;
	end = scanEnd & 0xff;
	fpga.m_scanRange = (fpga.m_scanRange & (~(0xffff << 0))) | (begin << 8) | (end << 0);	
	data = fpga.m_scanRange;

	PRINTF("fpga send scanrange left,addr--data: 0x%x--0x%x  \n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	
	return (SUCCESS);	
}

/*
 * @param an: pointer to parameter an of BW reveive focus of the four emit 
 * @param size: number of para
 */
int FpgaCtrl2D::SendReceiveDelayAn(INT16 *an, INT32U size)
{
	INT32U clear = 0;
    return ReceiveDelayAn(an, size, clear);

#if 0
    INT32U addr;
	INT32U data;	
	INT32 ret;
	
	//clear all data first
	addr = FOCUS_ADDR;
	fpga.m_focusAddr = 0;
	data = fpga.m_focusAddr;

	PRINTF("fpga send black-white receive delay An,clear to zero addr--data: 0x%x--0x%x\n", addr, data); 
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

#if 0
    int i;
    PRINTF("+++++++++++++++++++++++++++++++++++an\n");
    for (i = 0; i < 64; i ++)
    {
        printf("%d  ", *((unsigned int*)an+i));
    }
    printf("\n");
#endif
	//write data to fpga
    addr = FOCUS_An;
	PRINTF("fpga send black-white receive delay An: addr = %d, size = %d\n", addr, size);
	ret = m_ptrInterface->WriteBufToFpga(addr, size, BITS16, (INT8U*)an);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);	
#endif

}

int FpgaCtrl2D::SendReceiveDelayAnSpaceCompound1(INT16 *an, INT32U size)
{
	INT32U clear = 1024;
    return ReceiveDelayAn(an, size, clear);
}
int FpgaCtrl2D::SendReceiveDelayAnSpaceCompound2(INT16 *an, INT32U size)
{
	INT32U clear = 2048;
    return ReceiveDelayAn(an, size, clear);
}

/*
 * @param an: pointer to parameter bn of BW reveive focus of the four emit 
 * @param size: number of para
 */
int FpgaCtrl2D::SendReceiveDelayBn(INT16 *bn, INT32U size)
{
	INT32U clear = 0;
    return ReceiveDelayBn(bn, size, clear);
}

int FpgaCtrl2D::SendReceiveDelayBnSpaceCompound1(INT16 *bn, INT32U size)
{
	INT32U clear = 1024;
    return ReceiveDelayBn(bn, size, clear);
}

int FpgaCtrl2D::SendReceiveDelayBnSpaceCompound2(INT16 *bn, INT32U size)
{
	INT32U clear = 2048;
    return ReceiveDelayBn(bn, size, clear);
}
/**
* @param an: pointer to parameter an of color reveive focus of the four emit 
* @param size: number of para
*   
*/
int FpgaCtrl2D::SendReceiveDelayColorAn(INT16 *an, INT32U size)
{
	INT32U clear = 512;
    return ReceiveDelayAn(an, size, clear);

#if 0
	INT32U addr;
	INT32U data;	
	INT32 ret;
	
	//clear all data first
	addr = FOCUS_ADDR;
	fpga.m_focusAddr = 512;
	data = fpga.m_focusAddr;

	PRINTF("fpga send color receive delay An,clear to zero, addr--data: 0x%x--0x%x\n", addr, data); 
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	//write data to fpga
    addr = FOCUS_An;
	PRINTF("fpga send color receive delay: addr = %d, size = %d\n", addr, size);
	ret = m_ptrInterface->WriteBufToFpga(addr, size, BITS16, (INT8U*)an);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);	
#endif
}

/**
* @param an: pointer to parameter bn of color reveive focus of the four emit 
* @param size: number of para
*   
*/
int FpgaCtrl2D::SendReceiveDelayColorBn(INT16 *bn, INT32U size)
{
	INT32U clear = 512;
    return ReceiveDelayBn(bn, size, clear);
}


/**
* @param aperture: pinter to aperture data of reveive focus
* @param size: number of aperture
*  
*/
int FpgaCtrl2D::SendReceiveAperture(INT8U *aperture, INT32U size)
{
	INT32U clear = 4096*0;
    return ReceiveAperture(aperture, size, clear);	
}

int FpgaCtrl2D::SendReceiveApertureSpaceCompound1(INT8U *aperture, INT32U size)
{
	INT32U clear = 4096*1;
    return ReceiveAperture(aperture, size, clear);	
}

int FpgaCtrl2D::SendReceiveApertureSpaceCompound2(INT8U *aperture, INT32U size)
{
	INT32U clear = 4096*2;
    return ReceiveAperture(aperture, size, clear);	
}

int FpgaCtrl2D::SendReceiveApertureU16(INT16 *aperture, INT32U size)
{
	INT32U clear = 4096*0;
    return ReceiveApertureU16(aperture, size, clear);	
}

int FpgaCtrl2D::SendReceiveApertureSpaceCompound1U16(INT16 *aperture, INT32U size)
{
	INT32U clear = 4096*1;
    return ReceiveApertureU16(aperture, size, clear);	
}

int FpgaCtrl2D::SendReceiveApertureSpaceCompound2U16(INT16 *aperture, INT32U size)
{
	INT32U clear = 4096*2;
    return ReceiveApertureU16(aperture, size, clear);	
}
/**
* @param aperture: pinter to aperture data of reveive focus color
* @param size: number of aperture
*  
*/
int FpgaCtrl2D::SendReceiveApertureColor(INT8U *aperture, INT32U size)
{
	INT32U clear = 4096*3;
    return ReceiveAperture(aperture, size, clear);	
}

int FpgaCtrl2D::SendReceiveApertureColorU16(INT16 *aperture, INT32U size)
{
	INT32U clear = 4096*3;
    return ReceiveApertureU16(aperture, size, clear);	
}

/**
* @param parameter--8bits(8--15)
*   
*/
int FpgaCtrl2D::SendDemodCombine(INT16U data)
{
	INT32U clearAddr = 2;
    INT16U value = data;
    return DemodCombineBuf(&value, 1, clearAddr);
}

int FpgaCtrl2D::SendDemodCombineBuf(INT16U* data, INT32U size)
{
	INT32U clearAddr = 1;
    return DemodCombineBuf(data, size, clearAddr);
}

int FpgaCtrl2D::SendDemodBufFreqCompound(INT16U* data, INT32U size, INT32U order)
{
	INT32U clearAddr;

	if (order == 1)
		clearAddr = 9;
	else
		clearAddr = 10;
    
    return DemodCombineBuf(data, size, clearAddr);
}

/**
* @param speed  8bits(8--15)
*
*/
int FpgaCtrl2D::SendBmSpeed(INT32U speed)
{
	INT32U addr;
	INT32U data;
	INT32U temp;
	INT32 ret;
	
	addr = BM_LINE;

	temp = speed;
	fpga.m_bmLine = (fpga.m_bmLine & (~(0xff << 8))) | (temp << 8);	
	data = fpga.m_bmLine;

	PRINTF("==============fpga send bm speed, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);	
}

/**
* @brief black-white 60mHz 460点
* @param sample--16bits(0-15)
*/
int FpgaCtrl2D::SendSample48m(INT32U sample)
{
	INT32U addr;
	INT32U data;
	
	addr = 5;
	data = sample;

	PRINTF("fpga send sample black-white addr = %d, data = %d\n", addr, data);
	return SendSampleCombined(addr, data); 
}


/** 
* @brief 60m 64点arperture BW
* @param sample  16bits(0-15)
*/
int FpgaCtrl2D::SendSampleApertureBW(INT32U sample)
{
	INT32U addr;
	INT32U data;
	
	addr = 8;
	data = sample;

	PRINTF("fpga send sample aperture BW addr = %d, data = %d\n", addr, data);
	return SendSampleCombined(addr, data); 
}

/** 
* @brief 60m 64点arperture color
* @param sample  16bits(0-15)
*/
int FpgaCtrl2D::SendSampleApertureColor(INT32U sample)
{
	INT32U addr;
	INT32U data;
	
	addr = 9;
	data = sample;

	PRINTF("fpga send sample aperture color addr = %d, data = %d\n", addr, data);
	return SendSampleCombined(addr, data); 
}

/*
* @param cycle 8bits(0-7) 
* 
*/
int FpgaCtrl2D::SendPulseCycle(INT32U cycle)
{
	INT32U clear = 1;
    return PulseCycle(cycle, clear);
}

int FpgaCtrl2D::SendPulseWidthFreqCompound(INT32U width, INT32U order)
{
	INT32U clear = 0;
	if (order == 1)
		clear = 6;
	else
		clear = 8;
	INT32U size = 1;
    return PulseWidthNew(width, clear);
}

int FpgaCtrl2D::SendPulseCycleFreqCompound(INT32U cycle, INT32U order)
{
	INT32U clear = 0;
	if (order == 1)
		clear = 5;
	else
		clear = 7;
    return PulseCycle(cycle, clear);
}

/**
* @param cycle 8bits(0-7) 
* 
*/
int FpgaCtrl2D::SendColorPulseCycle(INT32U cycle)
{
	INT32U clear = 3;
    return PulseCycle(cycle, clear);
}

/**
* @param with  8bits(0-7) 
* black-white weighting value of emit
*/
int FpgaCtrl2D::SendPulseWidth(INT8U *value, INT32U size)
{
	INT32U clear = 0;
    return PulseWidth(value, size, clear);
}
int FpgaCtrl2D::SendPulseWidthFreqCompound(INT8U *value, INT32U size, INT32U order)
{
	INT32U clear = 0;
	if (order == 1)
		clear = 128;
	else
		clear = 192;
    return PulseWidth(value, size, clear);
}
/**
* @param width  8bits(0-7) 
* color weighting value of emit 
*/
int FpgaCtrl2D::SendColorPulseWidth(INT8U *value, INT32U size)
{
	INT32U clear = 64;
    return PulseWidth(value, size, clear);
}

/** 
* @param sum  8bits(8-15)--real focus sum
* 
*/
int FpgaCtrl2D::SendFocusSum(INT8U sum) 
{
	INT32U addr;
	INT32U data;
	INT32U temp;
	INT32 ret;
	
	addr = PROBE_EMIT1;

	if ( (sum > 4) || (sum == 0))
	{
		PRINTF("input is out of range, focus sum (1-4)\n");
	}
	sum = sum - 1;
	//temp = sum & (0x3 << 0);
	temp = sum & (0xff << 0);
	//fpga.probe_emit1 = (fpga.probe_emit1 & (~(0x3 << 6))) | (temp << 6);	
	fpga.m_probeEmit1 = (fpga.m_probeEmit1 & (~(0xff << 8))) | (temp << 8);	
	data = fpga.m_probeEmit1;

	PRINTF("fpga send focus sum, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);	
}

/** 
* @param max_period  16bits(0-15) focus pulse 协议上为2bits
*/
int FpgaCtrl2D::SendMaxPeriod(INT32U maxPeriod)
{
	INT32U addr;
	INT32U data;
	INT32U temp;
	INT32 ret;
	
	// clear
	addr = MAXPERIOD_ADDR;
	data = 0;
	PRINTF("fpga clear max_period_addr , addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	
	// send data
	addr = PROBE_EMIT2;
	temp = maxPeriod & (0xffff << 0);
	fpga.m_probeEmit2 = (fpga.m_probeEmit2 & (~(0xffff << 0))) | (temp << 0);	
	data = fpga.m_probeEmit2;

	PRINTF("fpga send max period, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);	
}
int FpgaCtrl2D::SendTgcMaxPeriod(INT32U maxPeriod)
{
	INT32U addr;
	INT32U data;
	INT32U temp;
	INT32 ret;
	
	// clear
	addr = MAXPERIOD_ADDR;
	data = 4;
	PRINTF("fpga clear tgc max_period_addr , addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	
	// send data
	addr = PROBE_EMIT2;
	temp = maxPeriod & (0xffff << 0);
	fpga.m_probeEmit2 = (fpga.m_probeEmit2 & (~(0xffff << 0))) | (temp << 0);	
	data = fpga.m_probeEmit2;

	PRINTF("fpga send tgc max period, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);	
}

/**
* @param level  8bits(0-7) level of dynamic range
* 
*/
int FpgaCtrl2D::SendDynamicRange(INT8U level)
{	
	INT32U addr;
	INT32U data;
	INT32U temp;
	INT32 ret;
	addr = IMAGE_CTL;

	temp = level & (0xff << 0);
	fpga.m_imageCtl = (fpga.m_imageCtl & (~(0xff << 0))) | (temp << 0);	
	data = fpga.m_imageCtl;

	PRINTF("fpga send dynamic rage, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);	
}

/**
* @param line  8bits(0-7) bm line
* 
*/
int FpgaCtrl2D::SendBmLine(INT32U line)
{
	INT32U addr;
	INT32U data;
	INT32U temp;
	INT32 ret;
	
	addr = BM_LINE;

	temp = line & (0xff << 0);
	fpga.m_bmLine = (fpga.m_bmLine & (~(0xff << 0))) | (temp << 0);	
	data = fpga.m_bmLine;

	PRINTF("fpga send bm line, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);
}

/** 
* @param filter[]: filter data 16bits
* @param size: number of data for one probe one scale
*     
*/
int FpgaCtrl2D::SendFilter(INT16U *filter, INT32U size) 
{
	INT32U offset = 64*32*0;
	return DynamicFilter(filter, size, offset);
}

int FpgaCtrl2D::SendFilterCompoundFreq1(INT16U *filter, INT32U size)
{
	INT32U offset = 64*32*1;
	return DynamicFilter(filter, size, offset);
}
int FpgaCtrl2D::SendFilterCompoundFreq2(INT16U *filter, INT32U size)
{
	INT32U offset = 64*32*2;
	return DynamicFilter(filter, size, offset);
}
int FpgaCtrl2D::SendFilterColor(INT16U *filter, INT32U size) 
{
	INT32U offset = 64*32*3;
	return DynamicFilter(filter, size, offset);
}

/**
 * @brief: tell fpga to close power
 */
int FpgaCtrl2D::SendPowerOff(void)
{
	INT32U addr;
	INT32U data;
	INT32 ret;

	addr = RUN_EMIT;
	data = 0xdddd;

	PRINTF("fpga send poweroff, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);
}

/**
* @brief tell fpga to begin emit
* 8bits(0~7)
*/
int FpgaCtrl2D::SendBeginEmit(void)
{
	INT32U addr;
	INT32U data;
	INT32U temp;
	INT32 ret;

	addr = RUN_EMIT;

	temp = 0xaa;
	fpga.m_runEmit = (fpga.m_runEmit & (~(0xff << 0))) | (temp << 0);
	data = fpga.m_runEmit;

	PRINTF("fpga send begin emit, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);
}

int FpgaCtrl2D::SendWorkModel(INT32U wkModel)
{
	INT32U addr;
	INT32U data;
	INT32U temp;
	INT32 ret;

	addr = WORK_MODEL_T;

	temp = wkModel & (0x1ff << 0); //wk_model = 10001
	fpga.m_wkModel = (fpga.m_wkModel & (~(0xf << 0))) | (temp << 0);
	data = fpga.m_wkModel;

	PRINTF("fpga send work model, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);
}

/**
* @brief send packet size
* @param size 5bits (4-8)
*/
int FpgaCtrl2D::SendPacketSize(INT8U size)
{
	INT32U addr;
	INT32U data;
	INT32U temp;
	INT32 ret;

	addr = WORK_MODEL_T;

	temp = size & (0xff << 0); 
	fpga.m_wkModel = (fpga.m_wkModel & (~(0x1f << 4))) | (temp << 4);
	data = fpga.m_wkModel;

	PRINTF("fpga send packet size, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);
}
/**
* @brief send log table data        
*/
int FpgaCtrl2D::SendLogAddrData(INT32U size, INT16U *logData)
{
	INT32U addr;
	INT32U data;
	INT32 ret;
	
	// clear all log addr
	addr = LOG_ADDR_T;	
	fpga.m_logAddrData = 0;	
	data = fpga.m_logAddrData;

	PRINTF("fpga send logarithm, clear addr: 0x%x\n", addr);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
/*
	//send logarithm datas to fpga
	int i;
	int size;
	unsigned char logData[32768];

	for(i=0;i<32768;i++)
	{
		if(i < (8192 * 2))
		  logData[i] = (char)((255/value)*(log(i/((8192*2-1)/(pow(10, value)-1))+1)/log(10)));
		else
		  logData[i] = 255;
	}
	size = 32768 / 2;
*/
	PRINTF("fpga send log data size = %d\n", size);
	addr = LOG_DATA_T;
	PRINTF("fpga send logarithm, data addr: 0x%x\n", addr);

	ret = m_ptrInterface->WriteBufToFpga(addr, size, BITS16, (INT8U*)logData);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);	
}

/**
* @brief  PH control label          
* @param phLabel 1bits 1:相控阵  0：非相控阵
*/
int FpgaCtrl2D::SendPhaseControl(INT32U phLabel)
{
	INT32U addr;
	INT32U data;
	INT32U temp;
	INT32 ret;

	addr = PHASE_PULSE_T;

	temp = phLabel & (0x01 << 0); 
	fpga.m_num = (fpga.m_num & (~(0x01 << 0))) | (temp << 0);
	data = fpga.m_num;

	PRINTF("fpga send phase control, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);
}

/**
* @brief  pulse num 
* @param num 3bits (3-5)
*/
int FpgaCtrl2D::Send2DPulseNum(INT32U num)
{
	INT32U addr;
	INT32U data;
	INT32U temp;
	INT32 ret;

	addr = PHASE_PULSE_T;

	temp = num & (0x7 << 0);
	fpga.m_num = (fpga.m_num & (~(0x7 << 3))) | (temp << 3);
	data = fpga.m_num;

	PRINTF("fpga send phase control, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);
}

int FpgaCtrl2D::SendPWPulseNum(INT32U num)
{
	INT32U addr;
	INT32U data;
	INT32U temp;
	INT32 ret;

	addr = PHASE_PULSE_T;

	temp = num & (0x1f << 0); 
	fpga.m_num = (fpga.m_num & (~(0x1f << 11))) | (temp << 11);
	data = fpga.m_num;

	PRINTF("fpga send pw pulse num, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);
}

int FpgaCtrl2D::SendCFMPulseNum(INT32U num)
{
	INT32U addr;
	INT32U data;
	INT32U temp;
	INT32 ret;

	addr = PHASE_PULSE_T;

	temp = num & (0x1f << 0); 
	fpga.m_num = (fpga.m_num & (~(0x1f << 6))) | (temp << 6);
	data = fpga.m_num;

	PRINTF("fpga send cfm pulse num, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);
}


/**
* @brief send line density status   
* @param line_density 0: low line desity 1: high  8bits(0-7)
*/
int FpgaCtrl2D::SendLineDensity(INT32U lineDensity)
{
	INT32U clear = 0;
    return FunctonSwitch(lineDensity, clear);
}

int FpgaCtrl2D::SendReverseTHI(INT32U on)
{
	INT32U clear = 1;
    return FunctonSwitch(on, clear);
}
int FpgaCtrl2D::SendSpaceCompound(INT32U on)
{
	INT32U clear = 2;
    return FunctonSwitch(on, clear);
}
int FpgaCtrl2D::SendFreqCompund(INT32U on)
{
	INT32U clear = 3;
    return FunctonSwitch(on, clear);
}

/**
* @brief send high filter datas              
* 16 16bits
*/
int FpgaCtrl2D::SendMatchFilter(INT16U *filter,INT32U size) 
{
	INT32U clear = 64*32*0;
    PRINTF("match filter: size = %d\n", size);
    return MatchFilter(filter, size, clear);
}
int FpgaCtrl2D::SendMatchFilterFreqCompound1(INT16U *filter,INT32U size) 
{
	INT32U clear = 64*32*1;
    PRINTF("match filter1: size = %d\n", size);
    return MatchFilter(filter, size, clear);
}
int FpgaCtrl2D::SendMatchFilterFreqCompound2(INT16U *filter,INT32U size) 
{
	INT32U clear = 64*32*2;
    PRINTF("match filter2: size = %d\n", size);
    return MatchFilter(filter, size, clear);
}
int FpgaCtrl2D::SendMatchFilterPw(INT16U *filter,INT32U size) 
{
    PRINTF("match filter color: size = %d\n", size);
	INT32U clear = 64*32*3;
    return MatchFilter(filter, size, clear);
}

int FpgaCtrl2D::SendValid2DImage(int start2D, int startColor)
{
	INT32U addr;
	INT32U data;
	INT32 ret;

	// clear all filter data
	// set the start time to receive Bw data
	addr = HFILTER_ADDR_T;	
	fpga.m_highPassFilter = 1;	
	data = fpga.m_highPassFilter;

	PRINTF("fpga send filter, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	//erite data to fpga
	// set the start time to receive Bw data
	addr = HFILTER_DATA_T;	
	data = start2D;
	PRINTF("fpga send filter, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	
	// set the start time to receive IQ data
	addr = HFILTER_ADDR_T;	
	fpga.m_highPassFilter = 2;	
	data = fpga.m_highPassFilter;

	PRINTF("fpga send filter, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	//erite data to fpga
	//set the start time to receive IQ data
	addr = HFILTER_DATA_T;	
	data = startColor;//-28;
	PRINTF("fpga send filter, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return SUCCESS;
}
int FpgaCtrl2D::SendPhaseFocStart(INT16 time)
{
    return PhaseCombine(0, time);
}
int FpgaCtrl2D::SendPhaseWrOffset(INT16 offset)
{
    return PhaseCombine(1, offset);
}
int FpgaCtrl2D::SendPhaseStartK(INT16 start)
{
    return PhaseCombine(2, start);
}

int FpgaCtrl2D::SendPhaseFocStartColor(INT16 time)
{
    return PhaseCombine(3, time);
}
int FpgaCtrl2D::SendPhaseWrOffsetColor(INT16 offset)
{
    return PhaseCombine(4, offset);
}
int FpgaCtrl2D::SendPhaseStartKColor(INT16 start)
{
    return PhaseCombine(5, start);
}

int FpgaCtrl2D::SendSpaceCompound1FocStart(INT16 time)
{
    return PhaseCombine(6, time);
}
int FpgaCtrl2D::SendSpaceCompound1WrOffset(INT16 offset)
{
    return PhaseCombine(7, offset);
}
int FpgaCtrl2D::SendSpaceCompound1StartK(INT16 start)
{
    return PhaseCombine(8, start);
}

int FpgaCtrl2D::SendSpaceCompound2FocStart(INT16 time)
{
    return PhaseCombine(9, time);
}
int FpgaCtrl2D::SendSpaceCompound2WrOffset(INT16 offset)
{
    return PhaseCombine(10, offset);
}
int FpgaCtrl2D::SendSpaceCompound2StartK(INT16 start)
{
    return PhaseCombine(11, start);
}
#if 0
/** 
* @param demod[]: demod data 16bits
* @param size: number of data 
*     
*/
int FpgaCfm::SendCfmDemod(INT16U *demod, INT32U size) 
{
	INT32U addr;
	INT32U data;
	INT32 ret;
	
	// clear all demod data
	addr = CFM_DEMOD_ADDR;	
	fpgaCfm.m_cfmDemodAddr = 0;	
	data = fpgaCfm.m_cfmDemodAddr;

	PRINTF("1: fpga send demod, addr--data: 0x%x--0x%x\n", addr, data);
	ret = FpgaCtrl2D.WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	//erite data to fpga
	INT8U cfmSend[32];
	PRINTF("fpga send demod, size = %d\n", size);
	addr = CFM_DEMOD_DATA;	

	switch(size)
	{
	case 1:
		memcpy(cfmSend, demod, 32);
		break;
	case 2:
		memcpy(cfmSend, demod, 32);
		break;
	case 3:
		memcpy(cfmSend, demod, 32);
		break;
	case 4:
		memcpy(cfmSend, demod, 32);
		break;
	case 5:
		memcpy(cfmSend, demod, 32);
		break;
	case 6:
		memcpy(cfmSend, demod, 32);
		break;
	case 7:
		memcpy(cfmSend, demod, 32);
		break;
	case 8:
		memcpy(cfmSend, demod, 32);
		break;
	case 9:
		memcpy(cfmSend, demod, 32);
		break;
	}
	ret = FpgaCtrl2D.WriteBufToFpga(addr, 16, BITS16, (INT8U*)cfmSend);

/*	//send data to fpga
	PRINTF("1: fpga send demod, size = %d\n", size);
	addr = CFM_DEMOD_DATA;	
	
	ret = fpga_send_buf(addr, size, BITS16, (INT8U*)demod);
*/
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
int FpgaCfm::SendCfmFilterP(INT16U *filter, INT32U size) 
{
	INT32U addr;
	INT32U data;
	INT32 ret;
	
	// clear all filter data
	addr = CFM_FILTER_ADDR_P;	
	fpgaCfm.m_cfmFilterAddrP = 0;	
	data = fpgaCfm.m_cfmFilterAddrP;

	PRINTF("1: fpga send filter_p, addr--data: 0x%x--0x%x\n", addr, data);
	ret = FpgaCtrl2D.WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	//erite data to fpga
	INT8U cfmSend[32];
	PRINTF("fpga send filter_p, size = %d\n", size);
	addr = CFM_FILTER_DATA_P;	

	switch(size)
	{
	case 1:
		memcpy(cfmSend, filter, 32);
		break;
	case 2:
		memcpy(cfmSend, filter, 32);
		break;
	case 3:
		memcpy(cfmSend, filter, 32);
		break;
	case 4:
		memcpy(cfmSend, filter, 32);
		break;
	case 5:
		memcpy(cfmSend, filter, 32);
		break;
	case 6:
		memcpy(cfmSend, filter, 32);
		break;
	case 7:SendVPAngleSpeed
		memcpy(cfmSend, filter, 32);
		break;
	case 8:
		memcpy(cfmSend, filter, 32);
		break;
	case 9:
		memcpy(cfmSend, filter, 32);
		break;
	}
	ret = FpgaCtrl2D.WriteBufToFpga(addr, 16, BITS16, (INT8U*)cfmSend);

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
int FpgaCfm::SendCfmFilterD(INT16U *filter, INT32U size) 
{
	INT32U addr;
	INT32U data;
	INT32 ret;
	
	// clear all filter data
	addr = CFM_FILTER_ADDR_D;	
	fpgaCfm.m_cfmFilterAddrD = 0;	
	data = fpgaCfm.m_cfmFilterAddrD;

	PRINTF("1: fpga send filter_d, addr--data: 0x%x--0x%x\n", addr, data);
	ret = FpgaCtrl2D.WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	//erite data to fpga
	INT8U cfmSend[32];
	PRINTF("fpga send filter_d, size = %d\n", size);
	addr = CFM_FILTER_DATA_D;	

	switch(size)
	{
	case 1:
		memcpy(cfmSend, filter, 32);
		break;
	case 2:
		memcpy(cfmSend, filter, 32);
		break;
	case 3:
		memcpy(cfmSend, filter, 32);
		break;
	case 4:
		memcpy(cfmSend, filter, 32);
		break;
	case 5:
		memcpy(cfmSend, filter, 32);
		break;
	case 6:
		memcpy(cfmSend, filter, 32);
		break;
	case 7:
		memcpy(cfmSend, filter, 32);
		break;
	case 8:
		memcpy(cfmSend, filter, 32);
		break;
	case 9:
		memcpy(cfmSend, filter, 32);
		break;
	}
	ret = FpgaCtrl2D.WriteBufToFpga(addr, 16, BITS16, (INT8U*)cfmSend);

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
int FpgaCfm::SendCfmFilterB(INT16U *filter, INT32U size) 
{
	INT32U addr;
	INT32U data;
	INT32 ret;
	
	// clear all filter data
	addr = CFM_FILTER_ADDR_B;	
	fpgaCfm.m_cfmFilterAddrB = 0;	
	data = fpgaCfm.m_cfmFilterAddrB;

	PRINTF("1: fpga send filter_b, addr--data: 0x%x--0x%x\n", addr, data);
	ret = FpgaCtrl2D.WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	//erite data to fpga
	INT8U cfmSend[32];
	PRINTF("fpga send filter_b, size = %d\n", size);
	addr = CFM_FILTER_DATA_B;	

	switch(size)
	{
	case 1:
		memcpy(cfmSend, filter, 32);
		break;
	case 2:
		memcpy(cfmSend, filter, 32);
		break;
	case 3:
		memcpy(cfmSend, filter, 32);
		break;
	case 4:
		memcpy(cfmSend, filter, 32);
		break;
	case 5:
		memcpy(cfmSend, filter, 32);
		break;
	case 6:
		memcpy(cfmSend, filter, 32);
		break;
	case 7:
		memcpy(cfmSend, filter, 32);
		break;
	case 8:
		memcpy(cfmSend, filter, 32);
		break;
	case 9:
		memcpy(cfmSend, filter, 32);
		break;
	}
	ret = FpgaCtrl2D.WriteBufToFpga(addr, 16, BITS16, (INT8U*)cfmSend);

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
int FpgaCfm::SendCfmLog(INT16U *log, INT32U size) 
{
	INT32U addr;
	INT32U data;
	INT32 ret;
	
	// clear all log data
	addr = CFM_LOG_ADDR;	
	fpgaCfm.m_cfmLog = 0;	
	data = fpgaCfm.m_cfmLog;

	PRINTF("1: fpga send log, addr--data: 0x%x--0x%x\n", addr, data);
	ret = FpgaCtrl2D.WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	//send data to fpga
	PRINTF("1: fpga send log, size = %d\n", size);
	addr = CFM_LOG_DATA;	
	
	ret = FpgaCtrl2D.WriteBufToFpga(addr, size, BITS16, (INT8U*)log);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);	
}

/** 
* @param clear_addr: clear addr 0-10
* @param cfm_data: data to send
*     
*/
int FpgaCfm::SendCfmCombined(INT16U clearAddr, INT16U cfmData) 
{
	INT32U addr;
	INT32U data;
	INT32 ret;
	
	addr = CFM_COMBINED_ADDR;	
	fpgaCfm.m_cfmCombinedAddr= clearAddr;	
	data = fpgaCfm.m_cfmCombinedAddr;

	PRINTF("1: fpga send cfm_combined, addr--data: 0x%x--0x%x\n", addr, data);
	ret = FpgaCtrl2D.WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	//erite data to fpga
	PRINTF("1: fpga send cfm_combined, data = %d\n", cfmData);
	addr = CFM_COMBINED_DATA;	

	ret = FpgaCtrl2D.WriteOneDataToFpga(addr, cfmData);
	//ret = WriteBufToFpga(addr, 16, BITS16, (INT8U*)cfm_data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);	
}

/** 
* @param clear_addr: clear addr 0-7
* @param cfm_data: data to send
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

	PRINTF("1: fpga send pw_combined, addr--data: 0x%x--0x%x\n", addr, data);
	ret = FpgaCtrl2D.WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	//erite data to fpga
    PRINTF("1: fpga send pw_combined, data = %d\n", pwData);
	addr = PW_COMBINED_DATA;	

	ret = FpgaCtrl2D.WriteOneDataToFpga(addr, pwData);
	//ret = WriteBufToFpga(addr, 16, BITS16, (INT8U*)cfm_data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);	
}
#endif

int FpgaCtrl2D::SendSampleCombined(INT16U clearAddr, INT16U sampleData)
{
	INT32U addr;
	INT32U data;
	INT32 ret;

	addr = FPGA_ADDR;	
	data = clearAddr;

	PRINTF("1: fpga send sample_combined clear, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	//erite data to fpga
	addr = SAMPLE_48;	
	data = sampleData;
	PRINTF("1: fpga send sample combine data, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);	
}
// only for test
/**
* @param width  8bits(0-7) 
* 
*/
int FpgaCtrl2D::SendPulseWidthBak(INT32U width)
{
	INT32U addr;
	INT32U data;
	INT32U temp;
	INT32 ret;

	// clear
	addr = FPGA_ADDR;
	data = 2;
	PRINTF("fpga send clear pulse width, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	
	addr = PROBE_EMIT1;
	temp = width & (0xff << 0);
	fpga.m_probeEmit1 = (fpga.m_probeEmit1 & (~(0xff << 0))) | (temp << 0);	
	data = fpga.m_probeEmit1;

	PRINTF("fpga send pulse width, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
#if 0
	// clear only for test
	addr = FPGA_ADDR;
	data = 5;
	PRINTF("fpga send clear pulse width, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
#endif

	return (SUCCESS);	
}
/**
* @param width  8bits(0-7) 
* 
*/
int FpgaCtrl2D::SendColorPulseWidthBak(INT32U width)
{
	INT32U addr;
	INT32U data;
	INT32U temp;
	INT32 ret;

	// clear
	addr = FPGA_ADDR;
	data = 4;
	PRINTF("fpga send clear color pulse width, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	
	addr = PROBE_EMIT1;
	temp = width & (0xff << 0);
	fpga.m_probeEmit1 = (fpga.m_probeEmit1 & (~(0xff << 0))) | (temp << 0);	
	data = fpga.m_probeEmit1;

	PRINTF("fpga send color pulse width, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

#if 0
	// clear only for test
	addr = FPGA_ADDR;
	data = 5;
	PRINTF("fpga send clear pulse width, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
#endif

	return (SUCCESS);	
}

int FpgaCtrl2D::SendSpaceCompoundAdderLine(INT16 line)
{
	INT16U clear;
	clear = 0;
	return CompoundImageCombine(clear, line);
}
int FpgaCtrl2D::SendSpaceCompound1AdderLine(INT16 line)
{
	INT16U clear;
	clear = 1;
	return CompoundImageCombine(clear, line);
}	
int FpgaCtrl2D::SendSpaceCompound2AdderLine(INT16 line)
{
	INT16U clear;
	clear = 2;
	return CompoundImageCombine(clear, line);
}
int FpgaCtrl2D::SendSpaceCompoundAdderDot(INT16 dot)
{
	INT16U clear;
	clear = 3;
	return CompoundImageCombine(clear, dot);
}	
int FpgaCtrl2D::SendSpaceCompound1AdderDot(INT16 dot)
{
	INT16U clear;
	clear = 4;
	return CompoundImageCombine(clear, dot);
}	
int FpgaCtrl2D::SendSpaceCompound2AdderDot(INT16 dot)
{
	INT16U clear;
	clear = 5;
	return CompoundImageCombine(clear, dot);
}	
int FpgaCtrl2D::SendSpaceCompoundStartSamp(INT16 start)
{
	INT16U clear;
	clear = 6;
	return CompoundImageCombine(clear, start);
}	
int FpgaCtrl2D::SendSpaceCompound1StartSamp(INT16 start)
{
	INT16U clear;
	clear = 7;
	return CompoundImageCombine(clear, start);
}		
int FpgaCtrl2D::SendSpaceCompound2StartSamp(INT16 start)
{
	INT16U clear;
	clear = 8;
	return CompoundImageCombine(clear, start);
}		

int FpgaCtrl2D::SendSpaceCompoundAdder(INT16U *value, INT32U size)
{
 	INT32U addr;
	INT32U data;
	INT32 ret;

	// clear
	addr = COMPOUND_ADDR;
	data = 14;
	PRINTF("fpga send clear compound adder, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	
	addr = COMPOUND_DATA;
	ret = m_ptrInterface->WriteBufToFpga(addr, size, BITS16, (INT8U*)value);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);   
}

int FpgaCtrl2D::SendSpaceCompoundCoef(INT16 value)
{
	INT16U clear;
	INT16U data;
	
	clear = 9;
	data = value & 0x1f;
	return CompoundImageCombine(clear, data);
}	
int FpgaCtrl2D::SendSpaceCompound1Coef(INT16 value)
{
	INT16U clear;
	INT16U data;
	
	clear = 10;
	data = value & 0x1f;
	return CompoundImageCombine(clear, data);
}	
int FpgaCtrl2D::SendSpaceCompound2Coef(INT16 value)
{
	INT16U clear;
	INT16U data;
	
	clear = 11;
	data = value & 0x1f;
	return CompoundImageCombine(clear, data);
}	
int FpgaCtrl2D::SendSpaceCompoundBase(INT16 value)
{
	INT16U clear;
	
	clear = 12;
	return CompoundImageCombine(clear, value);
}	
int FpgaCtrl2D::SendFreqCompoundCoef(INT16U* value, INT32U size)
{
	INT16U clear;
	
	clear = 13;
	return CompoundImageCombineBuf(clear, value, size);
}

#if 1
/**
 * @ 容积探头帧采样计数器   8bits(0--7)
 * @ 容积探头slices         8bits(8--15)
 * @ add by LWF 2012.3.6
 */
int FpgaCtrl2D::SendVPSamplesCnt(INT32U cnt)
{
    INT32U addr;
    INT32U data;
    INT32U temp;
    INT32 ret;

    addr = VPSAMPLECNT;

    temp = cnt;

    fpga.m_vpsamplescnt = (temp & 0xffff);//(fpga.m_vpsamplescnt & (~(0xff << 0))) | (temp << 0);	
    data = fpga.m_vpsamplescnt;

    //printf("==============fpga send volume probe sample counter, addr--data: 0x%x--0x%x\n", addr, data);
    ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
    if (ret != SUCCESS)
    {
        return (ERR_OP);
    }

    return (SUCCESS);	
}

/**
 * @ 
 * @ 
 * @ add by LWF 2012.7.30
 */
int FpgaCtrl2D::SendVPAngleSpeed(INT32U cnt)
{
    INT32U addr;
    INT32U data;
    INT32U temp;
    INT32 ret;

    addr = VPPARA;

    temp = cnt;

    fpga.m_vp_para = (temp & 0xffffffff);
    data = fpga.m_vp_para;

    //printf("==============fpga send volume probe para angle and speed, addr--data: 0x%x--0x%x\n", addr, data);
    ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
    if (ret != SUCCESS)
    {
        return (ERR_OP);
    }

    return (SUCCESS);	
}
#endif

int FpgaCtrl2D::SendCWReceive(INT8U* value, INT32U size)
{
    int i;
    //INT16U ph[16/4];
    INT16U ph[24/4];

    ASSERT(size == 24); //16); 
    //for (i = 0; i < 4; i += 1)
    for (i = 0; i < 6; i ++ )
    {
        ph[i] = ((*(value+i*4+3) & 0xf) << 12) + ((*(value+i*4+2) & 0xf) << 8) + ((*(value+i*4+1) & 0xf) << 4) + (*(value+i*4) & 0xf);
        CwCombine(i, ph[i]);
    }
}

int FpgaCtrl2D::SendExtendImaging(INT32U on)
{
	INT32U addr;
	INT32U data;
	INT32U temp;
	INT32 ret;

	addr = PHASE_PULSE_T;

	temp = on & (0x1 << 0); 
	fpga.m_num = (fpga.m_num & (~(0x1 << 2))) | (temp << 2);
	data = fpga.m_num;

	PRINTF("fpga send extend imaging, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);

}
int FpgaCtrl2D::SendExtendImagingSample(INT16U* value, INT32U size)
{
	INT32U addr;
	INT32U data;
	INT32 ret;
	
    //clear all data first
    addr = FPGA_ADDR;
    data = 10;

    PRINTF("fpga send send extend sample: fpga_addr addr--data: 0x%x--0x%x\n", addr, data); 
    ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
    if (ret != SUCCESS)
    {
        return (ERR_OP);
    }

#if 0
	INT32 i;
    INT32 j;
	for(i=10;i<50;i++)
    {
        printf("line[%d]:\n", i);
        for (j = 0; j < 64; j ++)
        {
            printf("%d  ", delay[i*64+j]); 
        }
        printf("\n");
    }
#endif
	addr = SAMPLE_48;
	ret = m_ptrInterface->WriteBufToFpga(addr, size, BITS16, (INT8U*)value);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	
	return (SUCCESS);	

}


int FpgaCtrl2D::TgcCombine(INT32 clearAddr, INT8U tgc[], INT32U size)
{
	INT32U addr;
	INT32U data;
	INT32 ret;
	
	//clear all data first
	addr = FPGA_ADDR;
	data = clearAddr;

	PRINTF("fpga send tgc, addr--data: 0x%x--0x%x\n", addr, data); 
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	//write data to fpga
	PRINTF("fpga send tgc:size = %d\n", size);

	addr = TGC_DATA_T;
	//ret = fpga_send_buf(addr, size, BITS32, (INT8U*)tgc);
	ret = m_ptrInterface->WriteBufToFpga(addr, size, BITS8, (INT8U*)tgc);
    // for (int i = 0; i < 8; i++)
      // printf("ADDR=%d,tgc[%d] =%d\n",addr,i,tgc[i]);

    if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);	
}
#if 0
int FpgaCtrl2D::TgcCombine(INT32 clearAddr, INT8U tgc[], INT32U size)
{
	INT32U addr;
	INT32U data;
	INT32 ret;
	
	//clear all data first
	addr = FPGA_ADDR;
	data = clearAddr;

	PRINTF("fpga send tgc, addr--data: 0x%x--0x%x\n", addr, data); 
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	//write data to fpga
	PRINTF("fpga send tgc:size = %d\n", size);

	addr = TGC_DATA_T;
	//ret = fpga_send_buf(addr, size, BITS32, (INT8U*)tgc);
	ret = m_ptrInterface->WriteBufToFpga(addr, size, BITS8, (INT8U*)tgc);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);	
}
#endif
int FpgaCtrl2D::PhaseCombine(INT16U clearAddr, INT16U para)
{
	INT32U addr;
	INT32U data;
	INT32 ret;

	addr = PHASE_ADDR;	
	data = clearAddr;

	PRINTF("1: fpga send phase_combined clear, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	//write data to fpga
	addr = PHASE_DATA;	
	data = para;
	PRINTF("1: fpga send phase combine data, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);	
}

int FpgaCtrl2D::EmitDelay(INT16U delay[], INT32U size, INT32U clear)
{
	INT32U addr;
	INT32U data;
	INT32 ret;
	
    //clear all data first
    addr = FPGA_ADDR;
    fpga.m_fpgaAddr = clear; 
    data = fpga.m_fpgaAddr;

    PRINTF("fpga send send delay: fpga_addr addr--data: 0x%x--0x%x\n", addr, data); 
    ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
    if (ret != SUCCESS)
    {
        return (ERR_OP);
    }

	//erite data to fpga
	PRINTF("fpga send send emit delay: size = %d\n", size); 
#if 0
	INT32 i;
    INT32 j;
	for(i=10;i<50;i++)
    {
        printf("line[%d]:\n", i);
        for (j = 0; j < 64; j ++)
        {
            printf("%d  ", delay[i*64+j]); 
        }
        printf("\n");
    }
#endif
	addr = FPGA_DATA;
	ret = m_ptrInterface->WriteBufToFpga(addr, size, BITS16, (INT8U*)delay);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	
	PRINTF("fpga send send delay end...\n"); 
	
	return (SUCCESS);	
}

int FpgaCtrl2D::PulseWidthNew(INT32U width, INT32U clear)
{
	INT32U addr;
	INT32U data;
	INT32U temp;
	INT32 ret;

	// clear
	addr = FPGA_ADDR;
	data = clear;
	PRINTF("fpga send clear pulse cycle, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	
	addr = PROBE_EMIT1;
	temp = width & (0xff << 0);
	fpga.m_probeEmit1 = (fpga.m_probeEmit1 & (~(0xff << 0))) | (temp << 0);	
	data = fpga.m_probeEmit1;

	PRINTF("fpga send pulse cycle, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);	
}

int FpgaCtrl2D::PulseCycle(INT32U cycle, INT32U clear)
{
	INT32U addr;
	INT32U data;
	INT32U temp;
	INT32 ret;

	// clear
	addr = FPGA_ADDR;
	data = clear;
	PRINTF("fpga send clear pulse cycle, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	
	addr = PROBE_EMIT1;
	temp = cycle & (0xff << 0);
	fpga.m_probeEmit1 = (fpga.m_probeEmit1 & (~(0xff << 0))) | (temp << 0);	
	data = fpga.m_probeEmit1;

	PRINTF("fpga send pulse cycle, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);	
}

int FpgaCtrl2D::PulseWidth(INT8U *value, INT32U size, INT32U clear)
{
	INT32U addr;
	INT32U data;
	INT32 ret;

	// clear
	addr = FPGA_ADDR;
	data = clear;
	PRINTF("fpga send clear pulse width, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	addr = WEIGHTING_EMIT;
	PRINTF("fpga send wighting value of emit, addr--size: 0x%x--%d\n", addr, size);
	ret = m_ptrInterface->WriteBufToFpga(addr, size, BITS8, (INT8U*)value);	
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);	
}

int FpgaCtrl2D::ReceiveDelayAn(INT16 *an, INT32U size, INT32U clear)
{
	INT32U addr;
	INT32U data;	
	INT32 ret;
	
	//clear all data first
	addr = FOCUS_ADDR;
	fpga.m_focusAddr = clear;
	data = fpga.m_focusAddr;

	PRINTF("fpga send black-white receive delay An,clear to zero addr--data: 0x%x--0x%x\n", addr, data); 
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

#if 0
    int i;
    PRINTF("+++++++++++++++++++++++++++++++++++an\n");
    for (i = 0; i < 64; i ++)
    {
        printf("%d  ", *((unsigned int*)an+i));
    }
    printf("\n");
#endif
	//write data to fpga
    addr = FOCUS_An;
	PRINTF("fpga send black-white receive delay An: addr = %d, size = %d\n", addr, size);
	ret = m_ptrInterface->WriteBufToFpga(addr, size, BITS16, (INT8U*)an);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);	

}
int FpgaCtrl2D::ReceiveDelayBn(INT16 *bn, INT32U size, INT32U clear)
{
	INT32U addr;
	INT32U data;	
	INT32 ret;
	
	//clear all data first
	addr = FOCUS_ADDR;
	fpga.m_focusAddr = clear;
	data = fpga.m_focusAddr;

	PRINTF("fpga send receive delay Bn,clear to zero addr--data: 0x%x--0x%x\n", addr, data); 
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	//write data to fpga
    addr = FOCUS_Bn;
	PRINTF("fpga send receive delay Bn: addr = %d, size = %d\n", addr, size);
#if 0
    INT32U i, j;
    for (i = 10; i < 60; i++)
    {
        printf("line[%d]:\n", i);
        for (j = 0; j < 64; j ++)
        {
            printf("%d  ", bn[i*64+j]);
        }
        printf("\n");
    }
#endif
	ret = m_ptrInterface->WriteBufToFpga(addr, size, BITS16, (INT8U*)bn);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);	
}

int FpgaCtrl2D::CompoundImageCombine(INT16U clear, INT16U para)
{
	INT32U addr;
	INT32U data;
	INT32U temp;
	INT32 ret;

	// clear
	addr = COMPOUND_ADDR;
	data = clear;
	PRINTF("fpga send clear compound combine, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	
	addr = COMPOUND_DATA;
	temp = para & (0xffff << 0);
	fpga.m_compound = (fpga.m_compound & (~(0xffff << 0))) | (temp << 0);	
	data = fpga.m_compound;

	PRINTF("fpga send compound combine data, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);	
}

int FpgaCtrl2D::CompoundImageCombineBuf(INT16U clear, INT16U* value, INT32U size)
{
	INT32U addr;
	INT32 ret;
    INT16U data;

	// clear
	addr = COMPOUND_ADDR;
	data = clear;
	PRINTF("fpga send clear compound combine, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	
	addr = COMPOUND_DATA;

	PRINTF("fpga send compound combine data buf, addr--len: 0x%x--0x%x\n", addr, size);
	ret = m_ptrInterface->WriteBufToFpga(addr, size, 2, (INT8U*)value);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);	
}

int FpgaCtrl2D::FunctonSwitch(INT32U value, INT32U clear)
{
	INT32U addr;
	INT32U data;
	INT32U temp;
	INT32 ret;

	addr = FUNCTION_SWITCH;

	temp = value & 0x1; 
	fpga.m_functionSwitch = (fpga.m_functionSwitch & (~(0x1 << clear))) | (temp << clear);
	data = fpga.m_functionSwitch;

	PRINTF("fpga send function switch, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);
}

int FpgaCtrl2D::MatchFilter(INT16U *filter, INT32U size, INT32U clear) 
{
	INT32U addr;
	INT32U data;
	INT32 ret;
	
	// clear all filter data
	addr = MATCH_FILTER_ADDR;	
	data = clear;

	PRINTF("fpga send match filter clear, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	//erite data to fpga
	PRINTF("fpga send match filter, size = %d\n", size);
	addr = MATCH_FILTER_DATA;	
	ret = m_ptrInterface->WriteBufToFpga(addr, size, BITS16, (INT8U*)filter);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	return (SUCCESS);	
}

int FpgaCtrl2D::DynamicFilter(INT16U *filter, INT32U size, INT32U offset) 
{
	INT32U addr;
	INT32U data;
	INT32 ret;
	
	// clear all filter data
	addr = FILTER_ADDR;	
	fpga.m_filterAddr = offset;	
	data = fpga.m_filterAddr;

	PRINTF("fpga send dynamic filter, addr--data: 0x%x--0x%x\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	//erite data to fpga
	PRINTF("fpga send filter, size = %d \n", size);
	addr = FILTER_DATA;	

#if 0
	int i;
	for (i = 0; i < 20; i ++)
	{
		PRINTF("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^SEND FILTER: %d = 0x%x\n", i, *(filter + i));
	}
#endif
	ret = m_ptrInterface->WriteBufToFpga(addr, size, BITS16, (INT8U*)filter);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);	
}

int FpgaCtrl2D::ReceiveAperture(INT8U *aperture, INT32U size, INT32U clear)
{
	INT32U addr;
	INT32U data;
	INT32 ret;
	
	//clear all data first
	addr = FOCUS_ADDR;
	fpga.m_focusAddr = clear;
	data = fpga.m_focusAddr;

	PRINTF("fpga send receive aperture, clear,addr--data: 0x%x--0x%x\n", addr, data); 
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	//write data to fpga
	addr = APERTURE_DATA;
	PRINTF("fpga send receive aperture: size = %d\n", size);
#if 0
    PRINTF("--------------------------fpga send receive aperture:\n");
	INT32 i;
	for(i=0;i<size;i++)
    {
        printf("%d  ", aperture[i]);
    }
    printf("\n");
#endif
	ret = m_ptrInterface->WriteBufToFpga(addr, size, BITS8, aperture);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);	
}

int FpgaCtrl2D::ReceiveApertureU16(INT16 *aperture, INT32U size, INT32U clear)
{
	INT32U addr;
	INT32U data;
	INT32 ret;
	
	//clear all data first
	addr = FOCUS_ADDR;
	fpga.m_focusAddr = clear;
	data = fpga.m_focusAddr;

	PRINTF("fpga send receive aperture, clear,addr--data: 0x%x--0x%x\n", addr, data); 
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	//write data to fpga
	addr = APERTURE_DATA;
	PRINTF("fpga send receive aperture: size = %d\n", size);
#if 0
    PRINTF("--------------------------fpga send receive aperture:\n");
	INT32 i;
	for(i=0;i<size;i++)
    {
        printf("%d  ", aperture[i]);
    }
    printf("\n");
#endif
	ret = m_ptrInterface->WriteBufToFpga(addr, size, BITS16, (INT8U*)aperture);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);	
}

int FpgaCtrl2D::DemodCombineBuf(INT16U* data, INT32U size, INT32U clear)
{
	INT32U addr;
	INT32 ret;
	INT16U clearAddr = clear;
	
	addr = DEMOD_COMBINED_ADDR;	
	PRINTF("1: fpga clear demod_combined, addr--data: 0x%x--0x%x\n", addr, clearAddr);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, clearAddr);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	//erite data to fpga
	addr = DEMOD_COMBINED_DATA;	
	PRINTF("1: fpga send demod_combined, addr--data = %d--%d\n", addr, data[0]);
    PRINTF("--- data0 = %d, size = %d, clear = %d\n", data[0], size, clear);

    if (size == 1)
        ret = m_ptrInterface->WriteOneDataToFpga(addr, data[0]);
    else
        ret = m_ptrInterface->WriteBufToFpga(addr, size, BITS16, (INT8U*)data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);	
}

int FpgaCtrl2D::CwCombine(INT16U clear, INT16U para)
{
	INT32U addr;
	INT32U data;
	INT32 ret;

	// clear
	addr = CW_RECEIVE_ADDR;
	data = clear;
    if (data == 4)
        data = 5;
    else if (data == 5)
        data = 6;
	PRINTF("fpga send clear cw receive combine, addr--data: %d--%d\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	
	addr = CW_RECEIVE_DATA;
	data = para & (0xffff << 0);
	PRINTF("fpga send cw receive combine data, addr--data: %d--%d\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);	
}

/** 
 * @para 0: NTSC 1: PAL
 */
int FpgaCtrl2D::VideoStandard(INT16U para)
{
	INT32U addr;
	INT32U data;
	INT32 ret;

	// clear
	addr = 58;
	data = 5;
	printf("fpga send clear cw receive combine, addr--data: %d--%d\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	
	addr = 59;
	data = para & (0xffff << 0);
	printf("fpga send cw receive combine data, addr--data: %d--%d\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{

		return (ERR_OP);
	}

	return (SUCCESS);	
}


/**
 *  @para 0: master 1: host
 */
int FpgaCtrl2D::VGADisplay(INT16U para)
{
    INT32U addr;
    INT32U data;
	INT32 ret;

	// clear
	addr = 58;
	data = 6;
	PRINTF("fpga send clear cw receive combine, addr--data: %d--%d\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	
	addr = 59;
	data = para & (0xffff << 0);
	PRINTF("fpga send cw receive combine data, addr--data: %d--%d\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	return (SUCCESS);	

}

#ifdef EMP_PROJECT
int FpgaCtrl2D::SendWaveData(INT8U *emitWave, INT32U pulseWidth, INT32U pulseNum, INT16U size)
{
	INT32U addr;
	INT32U data;
	INT32 ret;

    INT32U clear;
    clear = 8;
    data = pulseNum;
    ret = EmissionPulseNum(clear, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

    clear = 13;
    data = pulseWidth;
    ret = EmissionPulseWidth(clear, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

    clear = 13;
    ret = EmissionPulseData(clear, emitWave, size);
    if (ret != SUCCESS)
    {
        return (ERR_OP);
    }

    return ret;
}

int FpgaCtrl2D::Send2DPulseNumFreq0(INT32U pulseNum)
{
	INT32U clear;
	
	clear = 8;
	return EmissionPulseNum(clear, pulseNum);
}

int FpgaCtrl2D::Send2DPulseNumFreq1(INT32U pulseNum)
{
	INT32U clear;
	
	clear = 9;
	return EmissionPulseNum(clear, pulseNum);
}

int FpgaCtrl2D::Send2DPulseNumFreq2(INT32U pulseNum)
{
	INT32U clear;
	
	clear = 10;
	return EmissionPulseNum(clear, pulseNum);
}

int FpgaCtrl2D::SendCfmPulseNum(INT32U pulseNum)
{
	INT32U clear;
	
	clear = 11;
	return EmissionPulseNum(clear, pulseNum);
}

int FpgaCtrl2D::SendPwPulseNum(INT32U pulseNum)
{
	INT32U clear;
	
	clear = 12;
	return EmissionPulseNum(clear, pulseNum);
}

int FpgaCtrl2D::Send2DPulseWidthFreq0(INT32U pulseWidth)
{
	INT32U clear;
	
	clear = 13;
	return EmissionPulseWidth(clear, pulseWidth);
}

int FpgaCtrl2D::Send2DPulseWidthFreq1(INT32U pulseWidth)
{
	INT32U clear;
	
	clear = 14;
	return EmissionPulseWidth(clear, pulseWidth);
}

int FpgaCtrl2D::Send2DPulseWidthFreq2(INT32U pulseWidth)
{
	INT32U clear;
	
	clear = 15;
	return EmissionPulseWidth(clear, pulseWidth);
}

int FpgaCtrl2D::SendCfmPulseWidth(INT32U pulseWidth)
{
	INT32U clear;
	
	clear = 16;
	return EmissionPulseWidth(clear, pulseWidth);
}

int FpgaCtrl2D::SendPwPulseWidth(INT32U pulseWidth)
{
	INT32U clear;
	
	clear = 17;
	return EmissionPulseWidth(clear, pulseWidth);
}

int FpgaCtrl2D::SendCwPulseWidth(INT32U pulseWidth)
{
	INT32U clear;
	
	clear = 18;
	return EmissionPulseWidth(clear, pulseWidth);
}

int FpgaCtrl2D::SendEmissionPulseDataFreq0(INT8U *data, INT32U size)
{
	INT32U clear;
	
	clear = 20;
	return EmissionPulseData(clear, data, size);
}

int FpgaCtrl2D::SendEmissionPulseDataFreq1(INT8U *data, INT32U size)
{
	INT32U clear;
	
	clear = 21;
	return EmissionPulseData(clear, data, size);
}

int FpgaCtrl2D::SendEmissionPulseDataFreq2(INT8U *data, INT32U size)
{
	INT32U clear;
	
	clear = 22;
	return EmissionPulseData(clear, data, size);
}

int FpgaCtrl2D::SendLogCoef(INT32U coef)
{
    INT32U addr;
    INT32U data;
    INT32 ret;

	//clear all data first
	addr = LOG_ADDR_T;
    data = 0;

	printf("fpga send clear log coef para, addr--data: %d--%d\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	//write  number to fpga
	addr = LOG_DATA_T;
    data = coef & 0xffff;
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	printf("fpga send log coef para, addr--data: %d--%d\n", addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

    return ret;
}

int FpgaCtrl2D::SendDynamicRangeCoef(INT32U coef)
{
    INT32U addr;
    INT32U data;
    INT32 ret;

	//clear all data first
	addr = LOG_ADDR_T;
    data = 1;

	printf("fpga send clear dynamic range coef para, addr--data: %d--%d\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
#if 1
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
#endif
	//write  number to fpga
	addr = LOG_DATA_T;
    data = coef & 0xffff;
	printf("fpga send dynamic range coef para, addr--data: %d--%d\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

    return ret;
}

int FpgaCtrl2D::SendTgc(INT16U *tgc, INT32U size)
{
	INT32U clear;

    clear = 2;
    return(TgcSequence(clear, tgc, size));
}

int FpgaCtrl2D::SendFreqCompoundTgc1(INT16U *tgc, INT32U size)
{
	INT32U clear;

    clear = 3;
    return(TgcSequence(clear, tgc, size));
}

int FpgaCtrl2D::SendFreqCompoundTgc2(INT16U *tgc, INT32U size)
{
	INT32U clear;

    clear = 3;
    return(TgcSequence(clear, tgc, size));
}


///>private 2016.05.20
int FpgaCtrl2D::EmissionPulseNum(INT32U clear, INT32U pulseNum)
{
	INT32U addr;
	INT32U data;
	INT32 ret;
	
	//clear all data first
	addr = FPGA_ADDR;
    data = clear;

	printf("fpga send clear wave para, addr--data: %d--%d\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	//write pulse number to fpga
	addr = PROBE_EMIT1;
    data = pulseNum;
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	printf("fpga send pulse number data, addr--data: %d--%d\n", addr, pulseNum);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
    }
    return ret;
}

int FpgaCtrl2D::EmissionPulseWidth(INT32U clear, INT32U pulseWidth)
{
    INT32U addr;
	INT32U data;
	INT32 ret;

	addr = FPGA_ADDR;
    data = clear;

	printf("fpga send clear wave para, addr--data: %d--%d\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	//write pulse width to fpga
	addr = PROBE_EMIT1;
    data = pulseWidth;
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	printf("fpga send pulse width data, addr--data: %d--%d\n", addr, pulseWidth);

	if (ret != SUCCESS)
	{
		return (ERR_OP);
    }
    return ret;
}

int FpgaCtrl2D::EmissionPulseData(INT32U clear, INT8U *waveData, INT32U size)
{
    INT32U addr;
    INT32U data;
    INT32 ret;

	addr = FPGA_ADDR;
    data = clear;

	printf("fpga send clear wave para, addr--data: %d--%d\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
	//write wave data to fpga
	addr = PROBE_EMIT1;
	ret = m_ptrInterface->WriteBufToFpga(addr, size, BITS8, waveData);
	printf("fpga send wave data, addr: %d data: \n", addr);

#if 1
	INT32 i;
	for(i=0;i<size;i++)
    {
        printf("%d,  ", waveData[i]);
    }
    printf("\n");
#endif
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
    return ret;
}

int FpgaCtrl2D::TgcSequence(INT32U clear, INT16U *tgc, INT32U size)
{
    INT32U addr;
    INT32U data;
    INT32 ret;

	addr = LOG_ADDR_T;
    data = clear;

	printf("fpga send clear tgc, addr--data: %d--%d\n", addr, data);
	ret = m_ptrInterface->WriteOneDataToFpga(addr, data);
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}

	//write tgc data to fpga
	addr = LOG_DATA_T;
	ret = m_ptrInterface->WriteBufToFpga(addr, size, BITS16, (INT8U*)tgc);
	printf("fpga send wave data, addr: %d data: \n", addr);

#if 0
	INT32 i;
	for(i=0;i<size;i++)
    {
        printf("%d,  ", tgc[i]);
    }
    printf("\n");
#endif
	if (ret != SUCCESS)
	{
		return (ERR_OP);
	}
    return ret;

}
#endif