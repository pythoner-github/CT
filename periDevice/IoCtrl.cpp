#include "IoCtrl.h"
#include "UsbControl.h"

static const int DATA_ADDR_COM = 58;
static const int SOCKET_ADDR_COM = 59;
AbsUltraInterface* IoCtrl::m_ptrInterface = EzUsb::GetInstance();

IoCtrl::IoCtrl()
{
}

IoCtrl::~IoCtrl()
{
}

/**
* @brief on or off the high voltage
* @param onOff  0-off 1-on
*/
int IoCtrl::HighVoltage(INT8U onOff)
{
	int ret = ERROR;
	unsigned char request;
	int value = 0x00;

	if (onOff > 1)
	{
		PRINTF("input is out of range(0-1)\n");
		 return (ERR_RANGE);
	}

	request = CMD_SET_HV;
	if (onOff == 1) //hv on
	{
		PRINTF("HIGH voltage on\n");
		value = 0x00;
	}
	else if (onOff == 0) //hv off
	{
		PRINTF("HIGH voltage off\n");
		value = 0x01;
	}
	ret = m_ptrInterface->ReqIo(request, value, 0, NULL);

	if (ret != SUCCESS)
	{
		PRINTF("io high voltage error\n");
		return (ERR_OP);
	}
	return (SUCCESS);
}

/**
* @brief switch the interface of probe 
* @param addr addr of interface
*/
int IoCtrl::SetInterface(INT8U addr)
{
	int ret = ERROR;
	unsigned char request;
	int value = addr;
	
	request = CMD_SET_INTERFACE;

	PRINTF("io set interface \n");
#if 1
	ret = m_ptrInterface->ReqIo(request, value, 0, NULL);
	if (ret != SUCCESS)
	{
		PRINTF("io set interface error\n");
		return (ERR_OP);
	}
#endif

    INT32U data;
    data = 3;
    ret = m_ptrInterface->WriteOneDataToFpga(DATA_ADDR_COM, data);
	if (ret != SUCCESS)
	{
		PRINTF("write to fpga error\n");
		return (ERR_OP);
	}

    ret = m_ptrInterface->WriteOneDataToFpga(SOCKET_ADDR_COM, value);
    if (ret != SUCCESS)
    {
        PRINTF("write to fpga error\n");
        return (ERR_OP);
    }


	return (SUCCESS);
}

/**
* @brief freeze the image
*/
int IoCtrl::Freeze(void)
{
	int ret = ERROR;
	unsigned char request;
	int value;

	request = CMD_SET_FREEZE;
	value = 1;
	ret = m_ptrInterface->ReqIo(request, value, 0, NULL);
	if (ret != SUCCESS)
	{
		PRINTF("io freeze error\n");
		return (ERR_OP);
	}
	return (SUCCESS);
}

/**
* @brief unfreeze the image
*/
int IoCtrl::Unfreeze(void)
{
	int ret = ERROR;
	unsigned char request;
	int value;

	request = CMD_SET_FREEZE;
	value = 0;
	ret = m_ptrInterface->ReqIo(request, value, 0, NULL);
	if (ret != SUCCESS)
	{
		PRINTF("io Unfreeze error\n");
		return (ERR_OP);
	}
	return (SUCCESS);
}

/**
* @brief start to write probe
*/
int IoCtrl::BeginWriteProbe(void)
{
	int ret = ERROR;
	unsigned char request;
	int value;

	request = CMD_BEGIN_WRITE;
	value = 0x0a;
	ret = m_ptrInterface->ReqIo(request, value, 0, NULL);
	if (ret != SUCCESS)
	{
		PRINTF("io begin to write probe error\n");
		return (ERR_OP);
	}
	return (SUCCESS);
}

int IoCtrl::EndWriteProbe(void)
{
	int ret = ERROR;
	unsigned char request;
	int value;

	request = CMD_SEND_FPGA;
	value = 0x03;
	ret = m_ptrInterface->ReqIo(request, value, 0, NULL);
	if (ret != SUCCESS)
	{
		PRINTF("io begin to write probe error\n");
		return (ERR_OP);
	}
	return (SUCCESS);
}
/**
* @brief read one probe(on pointed interface) parameter 
* @param readCmd command to read probe info
* @param rec_buf array to store parameter readed
* @param len data length ; len must < 256
* @return length of byte data readed or error
*/
int IoCtrl::ReadOneProbe(int readCmd, unsigned char recBuf[], unsigned  int len)
{
	int ret = ERROR;
	unsigned char offset;
	unsigned char request;
	unsigned char *buf;
	const int endPointSize = 64;
    unsigned char buf_temp[endPointSize];
	int count = 1;
	int value;

	request = CMD_READ_PROBE;
	value = readCmd;
	buf = recBuf;

	for(offset=0; offset<len; offset += endPointSize)
	{
		count = 1;
		while(count > 0)
		{
			// read 
			ret = m_ptrInterface->ReqProbe(request, endPointSize*256+value,endPointSize, buf_temp);
			if (ret != SUCCESS)
			{
			    PRINTF("io read probe error\n");
			}
			else // if valid
			{
				PRINTF("Begin print usb data\n");
				if ((int)(len - offset) >= endPointSize)
					memcpy(buf+offset, buf_temp, endPointSize);
				else 
					memcpy(buf+offset, buf_temp, len - offset);
				break;
			}
			count --;
		}
	}
	return (SUCCESS);
}

/**
* @brief tell usb device transfer data to pc
*/
int IoCtrl::BeginReadImage(void)
{
	int ret = ERROR;
	unsigned char request;
	int value;

	request = CMD_BEGIN_READ;
	value = 0;
	PRINTF("io begin read image \n");
	ret = m_ptrInterface->ReqIo(request, value, 0, NULL);
	if (ret != SUCCESS)
	{
		PRINTF("io begin receive data error\n");
		return (ERR_OP);
	}
	
	return (SUCCESS);
}

/**
* @brief write probe parameter   note:after write probe ,must write ReqIo(0xba,0x03) to 
	make sure the bulkout is correct
* @param interfaces: it should be 1,2,3.....
* @param probe_buff:probe parameter
*/
int IoCtrl::ProbeWrite(INT8U interfaces, INT8U *probeBuff, INT32U len)
{
    //FPGA接收到该指令，进入写探头状态
	BeginWriteProbe();
	usleep(1000000);

	m_ptrInterface->WriteBufToProbe(interfaces, len, probeBuff);
	
	usleep(10000);

    //恢复给fpga发送参数状态
    EndWriteProbe();

	return (SUCCESS);
}

