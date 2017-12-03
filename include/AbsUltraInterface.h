#ifndef __ABS_ULTRA_INTERFACE_H__
#define __ABS_ULTRA_INTERFACE_H__

/**
* @brief abstrace ultrasound interface, control the comunication of io, probe, and fpga
*
*/
class AbsUltraInterface
{

	public:
		~AbsUltraInterface() {}

		/**
		* @brief init system when system starting, open device(dma, i2c, io, fpga, framebuffer), set x start position of image and set display control by fpga.
		*/
		virtual int OpenDevice(void) {return -1;}

		/**
		* @brief close device(dma, i2c, io, framebuffer)when system exit
		*/
		virtual void CloseDevice(void) {}

		/**
		* @brief send command to device to control io
		*/
		virtual int ReqIo(unsigned char request, int value, int len, unsigned char *buf) = 0;

		/**
		* @brief send command to probe device
		*/
		virtual int ReqProbe(unsigned char request, int value, int len, unsigned char *buf) = 0;

		/**
		* @brief write data to probe device
		*/
		virtual int WriteBufToProbe(unsigned char interfaces, unsigned int len, unsigned char *buf) = 0;

		/**
		* @brief receive data from fpga device
		* @para len[in] number of data to be read
		* @para buf[out] data read from device
		*/
		virtual int ReadBufFromFpga(int len, unsigned char *buf) = 0;

		/**
		* @brief send one data to fpga to control image
		* @param addr fpga address
		* @param data data to send to fpga
		* @return success or error
		*/
		virtual int WriteOneDataToFpga(INT32U addr, INT32U data) = 0;

		/**
		* @brief send buf to fpga to control image
		* @param addr fpga address
		* @param len data length
		* @param type bits type
		* @param send_buf[]  buf to send to fpga
		*/
		virtual int WriteBufToFpga(INT32U addr, INT32U len, INT8U type, INT8U *sendBuf) = 0;

};

#endif //_ABS_ULTRA_INTERFACE_H___
