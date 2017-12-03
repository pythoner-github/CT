#include "UsbControl.h"
#include <errno.h>

EzUsb* EzUsb::m_ptrInstance = NULL;

EzUsb::~EzUsb()
{
	if (m_ptrInstance != NULL)
	{
		m_ptrInstance->CloseDevice(); //close device

		delete m_ptrInstance;
	}
}
EzUsb* EzUsb::GetInstance()
{
	if (m_ptrInstance == NULL)
	{
		m_ptrInstance = new EzUsb;
		m_ptrInstance->OpenDevice(); // open device
	}

	return m_ptrInstance;
}

/**
* @brief init system when system starting, open device(dma, i2c, io, fpga, framebuffer), set x start position of image and set display control by fpga.
*/
int EzUsb::OpenDevice(void)
{
	int value;

	m_fdEzUsb = open(DEVICE_EZUSB_NAME, O_RDWR);
	if (m_fdEzUsb < 0)
	{
	    PRINTF("can not open device EzUsb\n");
		return (ERR_OP);
	}

	PRINTF("open device EzUsb: sucess to open /dev/osrfx2\n");

	PRINTF("open device EzUsb: begin send data to fpga\n");
	value = 0x03;
	ReqIo(CMD_SEND_FPGA, value, 0, NULL);

	return (SUCCESS);
}

/**
* @brief close device(dma, i2c, io, framebuffer)when system exit
*
*/
void EzUsb::CloseDevice(void)
{
	if (m_fdEzUsb)
	{
		close(m_fdEzUsb);
		m_fdEzUsb = 0;
		PRINTF("device EzUsb is close\n");
	}
}

/**
 * @brief send command to usb to control io
 */
int EzUsb::ReqIo(unsigned char request, int value, int len, unsigned char *buf)
{
	int ret;
	struct ezUsbControl control;

	control.request = request;
	control.value = value;
	control.len = len;
	control.buf = buf;
	
	ret = ioctl(m_fdEzUsb, EZUSB_IOC_REQUEST, &control);
	if (ret < 0)
	{
		PRINTF("VenReq: ioctl error\n");
		return (ERR_SYSCALL);
	}

	return (SUCCESS);
}

int EzUsb::ReqProbe(unsigned char request, int value, int len, unsigned char *buf)
{
	int ret;
	struct ezUsbControlProbe probe;

	probe.request = request;
	probe.value = (unsigned short)value;
	probe.len = len;
	probe.buf = buf;

	ret = ioctl(m_fdEzUsb, EZUSB_IOC_READPROBE, &probe);
	if (ret < 0)
	{
		PRINTF("VenReq: ioctl error\n");
		return (ERR_SYSCALL);
	}

	return (SUCCESS);
}

/**
* @brief write probe parameter to usb device
*/
int EzUsb::WriteBufToProbe(unsigned char interfaces, unsigned int len, unsigned char *buf)
{
	int ret;
	unsigned char dataLen;
	unsigned char *ptrBuf;

	dataLen = len + 2;
	ptrBuf = (unsigned char*)malloc(dataLen);
	memset(ptrBuf, 0, dataLen);

	*(ptrBuf + 0) = interfaces; 
	*(ptrBuf + 1) = (unsigned char)len;
	memcpy(ptrBuf + 2, buf, len);

	ret = write(m_fdEzUsb, ptrBuf, dataLen);
	if (ret < 0)
	{
		free(ptrBuf);
        perror("write probe error");
		PRINTF("bulk out probe:write error\n");
		return (ERR_SYSCALL);
	}
	
	free(ptrBuf);
	return (SUCCESS);
}

/**
* @brief receive data from usb
*/
int EzUsb::ReadBufFromFpga(int len, unsigned char *buf)
{
	int ret;

	ret = read(m_fdEzUsb,buf,len);
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
int EzUsb::WriteOneDataToFpga(INT32U addr, INT32U data)
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
int EzUsb::WriteBufToFpga(INT32U addr, INT32U len, INT8U type, INT8U *sendBuf)
{
	INT32 lenRemain;
	INT32 lenCur;
	INT32 lenPara;
	INT32 dataPkt;
	INT32 ret;

	if (type == BITS8)
	{	
		dataPkt = 255; //becuse bulkout's para len is "char" type
	}
	else
	{	
		dataPkt = 500;
	}

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
int EzUsb::BulkOut(unsigned char dataType, unsigned char dataLen, unsigned char fpgaAddr, unsigned char *buf)

{
	int ret;
	int len;
	unsigned char *ptrBuf;

	len = dataType * dataLen + 3;
	ptrBuf = (unsigned char*)malloc(len);
	memset(ptrBuf, 0, len);

	*(ptrBuf + 0) = dataLen;
	*(ptrBuf + 1) = dataType;
	*(ptrBuf + 2) = fpgaAddr;

	memcpy(ptrBuf + 3, buf, dataType * dataLen);
    errno = 0;
	ret = write(m_fdEzUsb, ptrBuf, len);
    
	if (ret < 0)
	{
		free(ptrBuf);
		PRINTF("bulk out:write error fpagAddr=%d\n", fpgaAddr);
		return (ERR_SYSCALL);
	}

	free(ptrBuf);
	return (SUCCESS);
}
