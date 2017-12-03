#ifndef __IOCTRL_H__
#define __IOCTRL_H__

#include "DefHal.h"
#include "AbsUltraInterface.h"

class IoCtrl
{
	public:
		IoCtrl();

		~IoCtrl();

		/**
		* @brief on or off the high voltage
		* 
		* @param on_off  0-off 1-on
		*/
		int HighVoltage(INT8U onOff);

		/**
		* @brief switch the interface of probe 
		* @param addr addr of interface
		*/
		int SetInterface(INT8U addr);

		/**
		* @brief freeze the image
		*/
		int Freeze(void);

		/**
		* @brief unfreeze the image
		*/
		int Unfreeze(void);

		/**
		* @brief read one probe(on pointed interface) parameter 
		* @param readCmd command to read probe info
		* @param rec_buf array to store parameter readed
		* @param len data length
		* @return length of byte data readed or error
		*/
		int ReadOneProbe(int readCmd, unsigned char recBuf[], unsigned  int len);

		/**
		* @brief start to write probe
		*/
		int BeginWriteProbe(void);

        /**
         * @brief fpga start to receive para from pc
         * @brief stop writing probe
         */
        int EndWriteProbe(void);

		/**
		* @brief tell usb device transfer data to pc
		*/
		int BeginReadImage(void);

		/**
		* @brief write probe parameter 
		*/
		int ProbeWrite(INT8U interfaces, INT8U *probeBuf, INT32U len);

	private:
		
		static AbsUltraInterface* m_ptrInterface;
};


#endif 
