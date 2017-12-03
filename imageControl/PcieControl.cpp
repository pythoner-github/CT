#include "PcieControl.h"
#include <errno.h>

PcieControl* PcieControl::m_ptrInstance = NULL;

PcieControl::~PcieControl()
{
	if (m_ptrInstance != NULL)
	{
		m_ptrInstance->CloseDevice(); //close device

		delete m_ptrInstance;
	}
}
PcieControl* PcieControl::GetInstance()
{
	if (m_ptrInstance == NULL)
	{
		m_ptrInstance = new PcieControl;
		m_ptrInstance->OpenDevice(); // open device
	}

	return m_ptrInstance;
}

/**
* @brief init system when system starting, open device(dma, i2c, io, fpga, framebuffer), set x start position of image and set display control by fpga.
*/
int PcieControl::OpenDevice(void)
{
	int value;

	m_fdPcieControl = open(DEVICE_PCIE_NAME, O_RDWR);
	if (m_fdPcieControl < 0)
	{
		PRINTF("can not open device PcieControl\n");
		return (ERR_OP);
	}
	PRINTF("open device PcieControl: sucess to open /dev/pcie_fpga\n");

	PRINTF("open device PcieControl: begin send data to fpga\n");
	value = 0x03;
	//ReqIo(CMD_SEND_FPGA, CMD_SEND_FPGA, 4, (unsigned char *)&value);

	return (SUCCESS);
}

/**
* @brief close device(dma, i2c, io, framebuffer)when system exit
*
*/
void PcieControl::CloseDevice(void)
{
	if (m_fdPcieControl)
	{
		close(m_fdPcieControl);
		m_fdPcieControl = 0;
		PRINTF("device PcieControl is close\n");
	}
}

/**
 * @brief send command to usb to control io
 */
int PcieControl::ReqIo(unsigned char request, int addr, int len, unsigned char *buf)
{
	return (SUCCESS);
	int ret;

	FpgaControl control;
	control.par_addr = addr;
	control.par_type = BITS32;
	control.par_len = len;
	control.par_data = (char *)buf;
	
	ret = ioctl(m_fdPcieControl, SEND_PAR, &control);
	if (ret < 0)
	{
		PRINTF("VenReq: ioctl error\n");
		return (ERR_SYSCALL);
	}

	return (SUCCESS);
}

int PcieControl::ReqProbe(unsigned char request, int value, int len, unsigned char *buf)
{
	return (SUCCESS);
}

/**
* @brief write probe parameter to usb device
*/
int PcieControl::WriteBufToProbe(unsigned char interfaces, unsigned int len, unsigned char *buf)
{
	return (SUCCESS);
}

/**
* @brief receive data from usb
*/
int PcieControl::ReadBufFromFpga(int len, unsigned char *buf)
{
	int ret;

	ret = read(m_fdPcieControl, buf, len);
	if (ret < 0)
	{
		PRINTF("bulk in: read error\n");
		PRINTF("Error is: %s\n", strerror(errno));
		return (ERR_SYSCALL);
	}
	
	return (SUCCESS);
}

/**
* @brief send one data to fpga to control image
* @param addr fpga address
* @param data data to send to fpga
* @return success or error
*/
int PcieControl::WriteOneDataToFpga(INT32U addr, INT32U data)
{
	unsigned char dataType;
	unsigned char paraLen;
	unsigned char fpgaAddr;
	unsigned int *buf;
	INT32 ret;

	dataType = BITS32;
	paraLen = 1;	
	fpgaAddr = (unsigned char)addr;
	buf = &data;

	ret = BulkOut(dataType, paraLen, fpgaAddr, (unsigned char *)buf);
	if (ret != SUCCESS)
	{
		PRINTF("fpga send one data error\n");
		return (ERR_OP);
	}
	
	return (SUCCESS);
}

/**
* @brief send buf to fpga to control image
* @param addr fpga address
* @param len data length
* @param type bits type
* @param send_buf[]  buf to send to fpga 
*/
int PcieControl::WriteBufToFpga(INT32U addr, INT32U len, INT8U type, INT8U *sendBuf)
{
	INT32 lenRemain;
	INT32 lenCur;
	INT32 lenPara;
	INT32 dataPkt = 4096;
	INT32 ret;

	PRINTF("fpga send buf: type = %d\n", type);
	PRINTF("fpga send buf: len = %d\n", len);
	
	lenRemain = type * len;
	lenCur = 0;
	
	while(lenRemain)
	{
		if (lenRemain > dataPkt)
			lenPara = dataPkt;
		else
			lenPara = lenRemain;
		
		ret = BulkOut(type, (lenPara / type), addr, (sendBuf + lenCur));
		if (ret != SUCCESS)
		{
			PRINTF("fpga send one data error\n");
			return (ERR_OP);
		}

		lenRemain -= lenPara;
		lenCur += lenPara;
		//PRINTF("fpga send buf: len_cur = %d\n", len_cur);
	}

	return (SUCCESS);
}

///>private
/**
* @brief write one package (512bytes) to usb device
*/
int PcieControl::BulkOut(unsigned char dataType, unsigned int dataLen, unsigned int fpgaAddr, unsigned char *buf)
{
	INT32 ret;
	FpgaControl control;
	control.par_addr = fpgaAddr * 8 + 64;
	control.par_type = dataType;
	control.par_len = dataType * dataLen;
	control.par_data = (char *)buf;
	
	ret = ioctl(m_fdPcieControl, SEND_PAR, &control);
	if (ret < 0)
	{
		PRINTF("VenReq: ioctl error\n");
		return (ERR_SYSCALL);
	}

	return (SUCCESS);
}


