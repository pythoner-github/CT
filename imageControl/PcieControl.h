#ifndef __PCIECONTROL_H__
#define __PCIECONTROL_H__

#include "DefHal.h"
#include "AbsUltraInterface.h"

/**
* @brief it's basic func, control pcie device 
*
*/
class PcieControl: public AbsUltraInterface
{
	public:
		~PcieControl();
		
		/*
		 * @brief get the instance of PcieControl.
		 */
		static PcieControl* GetInstance();

		/**
		* @brief init system when system starting, open device(dma, i2c, io, fpga, framebuffer), set x start position of image and set display control by fpga.
		*/
		int OpenDevice(void);

		/**
		* @brief close device(dma, i2c, io, framebuffer)when system exit
		*/
		void CloseDevice(void);
		
		/**
		* @brief send request command to device to control io
		*/
		int ReqIo(unsigned char request, int addr, int len, unsigned char *buf);

		/**
		* @brief send request command to probe device 
		*/
		int ReqProbe(unsigned char request, int value, int len, unsigned char *buf);

		/**
		* @brief write data to probe device
		*/
		int WriteBufToProbe(unsigned char interfaces, unsigned int len, unsigned char *buf);

		/**
		* @brief receive data from fpga device 
		* @para len[in] number of data to be read
		* @para buf[out] data read from device
		*/
		int ReadBufFromFpga(int len, unsigned char *buf);
	
		/**
		* @brief send one data to fpga to control image
		* @param addr fpga address
		* @param data data to send to fpga
		* @return success or error
		*/
		int WriteOneDataToFpga(INT32U addr, INT32U data);
		
		/**
		* @brief send buf to fpga to control image
		* @param addr fpga address
		* @param len data length
		* @param type bits type
		* @param send_buf[]  buf to send to fpga 
		*/
		int WriteBufToFpga(INT32U addr, INT32U len, INT8U type, INT8U *sendBuf);
	private:
		PcieControl() {}

		int m_fdPcieControl;
		typedef struct{
			unsigned int par_addr;
			unsigned int par_type;
			unsigned int par_len;
			char *par_data;
		} FpgaControl;

		static PcieControl* m_ptrInstance;

		/**
		* @brief write one package (512bytes) to usb device
		*/
		int BulkOut(unsigned char dataType, unsigned int paraLen, unsigned int fpgaAddr, unsigned char *buf);		

		#define IMAGE_MAGIC 'k'
		#define START_IMG_UPLOAD  _IO(IMAGE_MAGIC,1)
		#define STOP_IMG_UPLOAD   _IO(IMAGE_MAGIC,2)
		#define SEND_PAR          _IOW(IMAGE_MAGIC,3,FpgaControl)
		#define RECEIVE_PAR       _IOR(IMAGE_MAGIC,4,FpgaControl)
		#define DEVICE_PCIE_NAME	"/dev/pcie_fpga"
};

#endif //__PCIE_CONTROL_H__

