#ifndef __USBCONTROL_H__
#define __USBCONTROL_H__

#include "DefHal.h"
#include "AbsUltraInterface.h"

/**
* @brief it's basic func, control usb device 
*
*/
class EzUsb: public AbsUltraInterface
{
	public:
		~EzUsb();
		
		/*
		 * @brief get the instance of EzUsb.
		 */
		static EzUsb* GetInstance();

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
		int ReqIo(unsigned char request, int value, int len, unsigned char *buf);

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
		int GetCurEzUsbId() { return m_fdEzUsb;}

    private:
		EzUsb() {}
		
		int m_fdEzUsb;
		struct ezUsbControl
		{
			unsigned char request;///<command
			int value; ///<value to set
			int len; ///< len of date returned in buf; 
			unsigned char * buf;
		};
		struct ezUsbControlProbe
		{
			unsigned char request;///<command
			unsigned short value; ///<value to set
			int len; ///< len of date returned in buf; 
			unsigned char * buf;
		};

		static EzUsb* m_ptrInstance;

		/**
		* @brief write one package (512bytes) to usb device
		*/
		int BulkOut(unsigned char dataType, unsigned char paraLen, unsigned char fpgaAddr, unsigned char *buf);		

		#define OUR_MAGIC	'o'
		#define EZUSB_IOC_MAXNR	3
		#define EZUSB_IOC_REQUEST	_IOW(OUR_MAGIC, 1, struct ezUsbControl)
		#define EZUSB_IOC_READPROBE	_IOW(OUR_MAGIC, 2, struct ezUsbControlProbe)
		#define DEVICE_EZUSB_NAME	"/dev/osrfx2_emp"

		// cmd
		#define CMD_SEND_FPGA 0xBA
		#define	CMD_READ_PROBE 0xb4
		#define	CMD_SET_HV 0xb6
		#define	CMD_SET_INTERFACE 0xb7
		#define	CMD_SET_FREEZE 0xb9
		#define	CMD_BEGIN_READ 0xc0	
		#define	CMD_BEGIN_WRITE  0xBA

};

#endif //__USB_CONTROL_H__

