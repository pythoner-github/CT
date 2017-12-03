#ifndef __FPGARECEIVE_H__
#define __FPGARECEIVE_H__

#include "DefHal.h"
#include "UsbControl.h"
#include "AbsFpgaReceive.h"
#include "DSC.h"
#include "ImgPw.h"
#include "PcieControl.h"

class ScanMode;//declare class scanmode, 前置声明，用于定义指针。
class FpgaReceive:public AbsFpgaReceive
{
	public:


	/**
	* @brief service routine only used to receive data from usb device
	*/
	void UsbServiceRoutine(void *arg);
	/**
	* @brief get the ptrDsc and save it
	*/
	void SetDsc(CDSC *ptrDsc);

	private:
#ifdef EMP_460		
	static PcieControl* m_ptrUsb;
#else
	static EzUsb* m_ptrUsb;
#endif
	static CDSC* m_ptrDsc;
    static ScanMode* m_ptrScanMode;

	//for test
	void Data2D(unsigned char *data, int line, int imgData = 128);
	void DataPw(unsigned char *data, int line);
	void DataCfm(unsigned char *data, int line);
	void DataM(unsigned char *data, int line);
	unsigned char m_cfmData[160*3];
	void InitDataCfm();
    
	CalcPw* m_ptrCalc;

};

#endif
