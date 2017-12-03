#include <stdlib.h>
#include <string.h>
#include "FpgaGeneral.h"
#include "UsbControl.h"
#include "PcieControl.h"
#include "CalcTime.h"


///> /////////////////////////////////[private variable]////////////////////////////
CFpgaGeneral* CFpgaGeneral::m_ptrInstance = NULL;

///> /////////////////////////////////[public func]////////////////////////////

CFpgaGeneral* CFpgaGeneral::GetInstance()
{
	if(m_ptrInstance == NULL)
	{
		m_ptrInstance = new CFpgaGeneral;
	}

	return m_ptrInstance;
}

CFpgaGeneral::CFpgaGeneral()
{
    flag = 0;
}

CFpgaGeneral::~CFpgaGeneral()
{
    if(m_ptrInstance != NULL)
        delete m_ptrInstance;
}

void  CFpgaGeneral::AD9273ControlInit(void) //9273初始化
{
    CalcTime  ct;
    ct.Begin();

	SpiControlSetting(0x0f, 0x00, 0x18);		//chip_port_config
	Update();
	
	ResetControl();
	Update();

	SpiControlSetting(0x0f, 0x00, 0x18);		//chip_port_config
	Update();
	
	RunChip();
	Update();
	
	SetIndex();
	Update();

    if (ADREFTYPE == 1)
	    SpiControlSetting(0x0f, 0x18, 0x40);		//FLEX_VREF  //外部参考
    else
        SpiControlSetting(0x0f, 0x18, 0x00);		//FLEX_VREF	//内部参考	0x00---0.625V		0x03---1.0V 		0x01---0.75V		0x10---0.875V
																
	Update();

	//SpiControlSetting(0x0f, 0x11, 0x0E);		  //FLEX_GAIN     //PGA Gain(Bit3 Bit2: 00---21dB     01---24dB   10---27dB   11---30dB), 
	SpiControlSetting(0x0f, 0x11, 0x0A);		  //FLEX_GAIN     //PGA Gain(Bit3 Bit2: 00---21dB     01---24dB   10---27dB   11---30dB), 
	                                //              //LNA Gain(Bit1 Bit0: 00---15.6dB   01---17.9dB 10---21.3dB)   
                                    //              0x0E
    Update();

	SpiControlSetting(0x0f, 0x0f, 0x00);    //0x80);		//FLEX_CHANNEL_INPUT
	Update();

	SpiControlSetting(0x0f, 0x2B, 0x40);     //0x41);		//FLEX_FILTER
	Update();

	//SpiControlSetting(0x0f, 0x14, 0x01);		//OUTPUT_MODE	twos complement
	SpiControlSetting(0x0f, 0x14, 0x01);		//OUTPUT_MODE   binary offset
	Update();

	SpiControlSetting(0x0f, 0x0d, 0x00);		//TEST_IO	off
	Update();

	SpiControlSetting(0x0f, 0x10, 0x20);		//FLEX_OFFSET
	Update();

	SpiControlSetting(0x0f, 0x12, 0x08);		//BIAS_CURRENT
	Update();

	SpiControlSetting(0x0f, 0x15, 0x00);		//OUTPUT_ADJUST
	Update();

	//SpiControlSetting(0x0f, 0x16, 0x0C);		//OUTPUT_PHASE
	SpiControlSetting(0x0f, 0x16, 0x0A);		//OUTPUT_PHASE
	Update();

	//SpiControlSetting(0x0f, 0x19, 0xAA);		//USER_PATT1_LSB
	//Update();
	//SpiControlSetting(0x0f, 0x1A, 0xAA);		//USER_PATT1_MSB
	//Update();
	//SpiControlSetting(0x0f, 0x1B, 0x55);		//USER_PATT2_LSB
	//Update();
	//SpiControlSetting(0x0f, 0x1C, 0x55);		//USER_PATT2_MSB
	//Update();
    SpiControlSetting(0x0f, 0x19, 0x3F);		//USER_PATT1_LSB
    Update();
    SpiControlSetting(0x0f, 0x1A, 0x00);		//USER_PATT1_MSB
    Update();
    SpiControlSetting(0x0f, 0x1B, 0xC0);		//USER_PATT2_LSB
    Update();
    SpiControlSetting(0x0f, 0x1C, 0x0F);		//USER_PATT2_MSB
    Update();

	SpiControlSetting(0x0f, 0x21, 0x00);		//SERIAL_CONTROL
	Update();

	SpiControlSetting(0x0f, 0x22, 0x00);		//SERIAL_CH_STAT
	Update();

	SpiControlSetting(0x0f, 0x2C, 0x00);		//ANALOG_INPUT
	Update();

	SpiControlSetting(0x0f, 0x2D, 0x00);		//CROSS_POINT_SWITCH
	Update();

	AD9273_TestChNo(0, 1 ); //设置偏置电流

    //AD9273_TestIO(TRUE);
    ct.End();
	
}

void CFpgaGeneral::AD5805ControlInit() //5805 init
{
//soft reset  S_RST   0X00----0X0001
    SpiControlSetting(0xff,0x00, 0x0001); 
      
    //init 
    //0X01----0X0010
    SpiControlSetting(0xff,0x01, 0x0010);
     
    //0XD1----0X0140
    SpiControlSetting(0xff,0xd1, 0x0140);
     
    //0XDA----0X0001
    SpiControlSetting(0xff,0xda, 0x0001);
     
    //0XE1----0X0020
    SpiControlSetting(0xff,0xe1, 0x0020);
     
    //0X02----0X0080
    SpiControlSetting(0xff,0x02, 0x0080);
     
    //0X01----0X0000
    SpiControlSetting(0xff,0x01, 0x0000);

    //vca_reset RES_VCA 0X03----0X0020
    SpiControlSetting(0xff,0x03,0x0020);

    //vca_unreset RES_VCA 0X03----0X0000
    SpiControlSetting(0xff,0x03, 0x0000);
     
    //vca_regester1  set   VCA_SDATA[15:0]   
    //0X16----16'B0000_0000_0010_0011
    //BIE15-8:USE FOR CW DOPPLER MODE, SET AS 0
    //BIT7-6:PGA GAIN, 00=20DB,01=25DB,10=27DB;11=30DB
    //BIT5:MODE,1=TGC MODE,0=CW DOPPLER MODE
    //BIT4:CL, 0= CLAMPS THE OUTPUT SIGNAL AT APPROSIMATELY -1.4DB BELOW THE FULL-SCALE OF 2Vpp(DEFAULT),1=CLAMP TRANSPARENT(DISABLE) 
    //BIE3:LOW-PASS FILTER BANDWIDTH,0=15MHz,1=10MHz
    //BIE2:PWR,1=POWER-DOWN MODE ENABLE,0=PD DISABLE(DEFAULT)
    //SpiControlSetting(0xff,0x16, 0xfff3);
    //16'b0000_0000_1110_0011
    //SpiControlSetting(0xff,0x16, 0x00e3);
    //SpiControlSetting(0xff,0x16, 0x00a3);		//from new g30
    SpiControlSetting(0xff,0x16, 0x00e3);

    //vca register2 set  VCA_SDATA[31:16]   USE FOR CW MODE
    //OX17----0X0000
    //SpiControlSetting(0xff,0x17, 0xffff);
    SpiControlSetting(0xff,0x17, 0x0000);
     


    //vca register3 set VCA_SDATA[39-32]  USE FOR CW MODE, SET AS O
    //0X18----0X0000
   // SpiControlSetting(0xff,0x18, 0x00ff);
    SpiControlSetting(0xff,0x18, 0x0000);
     

    //POWER DOWN MODE SET  
    //0X0F----0X0000   16'B0000_0***_****_****
    //BIT15-11  USELESS, SET AS 0
    //BIT10-8: PDN_PIN_CFG, 100=USE FOR HARDWARE PARTIAL POWERDOWN MODE,
    //000=USE FOR HARDWARE COMPLETE POWERDOWN MODE
    //010=USE FOR SOFTWARE COMPLETE POWERDOWN MODE
    //001=USE FOR SOFTWARE PARTIAL POWERDOWN MODE
    //BIT7-0: CHANNEL 8-1 SELECT, 1=SELECT, THEN THE CHANNEL IN POWERDOWN MODE, 0=DON'T SELECT
    SpiControlSetting(0xff,0x0f, 0x0000);
     


    //lvds current drive   0x11----0x0000
	SpiControlSetting(0xff,0x11, 0x0000);		//from new g30
    //SpiControlSetting(0xff,0x11, 0x0600);
     

    
    //enable internal termination  0x12----0x0000
    SpiControlSetting(0xff,0x12, 0x0000);	//from new g30
    //SpiControlSetting(0xff,0x12, 0x4700); 

    //channel-specific, low-frequency noise suppression mode enable  0x14----0x0000
   // SpiControlSetting(0xff,0x14, 0x00ff);
    SpiControlSetting(0xff,0x14, 0x0000);
     
    

    //test mode register  BITS_CUSTOM1[9:0]    16'B1100_1000_1111
    //0X26----16'B00_1000_1111_0000_00
    SpiControlSetting(0xff,0x26, 0x23c0);
    //SpiControlSetting(0xff,0x26, 0x5540);
     
    
    //test mode register BITS_CUSTOM2[9:0]   16'B1100_1000_1111
    //0X27----16'B00_1000_1111_0000_00
    SpiControlSetting(0xff,0x27,0x23c0);
    //SpiControlSetting(0xff,0x27,0xAA80);
     
    
    //test mode1    SET 16'B0000_0000_0000_1111
    //0x25----16'b0000_0000_0***_****
    //bit1-0: BITS_CUSTOM1[11:10], SET AS 11
    //BIT3-2: BITS_CUSTOM2[11:10],SET AS 11
    //BIT6-4: 000=TEST MODE OFF, 100=EN_RAMP,010=DUAL_CUSTOM_PAT,001=SINGLE_CUSTOM_PAT
    SpiControlSetting(0xff,0x25, 0x000f);	//正常工作
	//SpiControlSetting(0xff,0x25, 0x001f);
	//SpiControlSetting(0xff,0x25, 0x0029);
     
    
    //TEST MODE2    SET 16'B0000_0000_0000_0000
    //0X45----16'B0000_0000_0000_00**
    //BIT1-0:00=TEST MODE OFF, 01= PAT_DESKEW,test data = 12'b0101_0101_0101,
    //10=PAT_SYNC,test data = 12'b1111_1100_0000
    //SpiControlSetting(0xff,0x45, 0x0000);
     
    
    //program gain channel 1
    //0x2a----16'b****_****_****_****; 
    //bit15-12: channel 1
    //bit11-8:channel 2
    //bit7-4:channel 3
    //bit3-0:channel 4
    //0000=0 db(default), 0001=1db,0010=2db,0011=3db,0100=4db,0101=5db,0110=6db,0111=7db,1000=8db,1001=9db,1010=10db,1011=11db,1100=12db, other=do not use
#ifdef EMP_355
	SpiControlSetting(0xff,0x2a,0x8888);	//from new g30
#else    
 	SpiControlSetting(0xff,0x2a, 0x0000);
#endif
    //SpiControlSetting(0xff,0x2a, 0x6666);
	//SpiControlSetting(0xff,0x2a,0x8888);	//from new g30
   // SpiControlSetting(0xff,0x2a,0xcccc);
   //SpiControlSetting(0xff,0x2a,0x6666);

     


    //program gain channel 2
    //0x2b----16'b****_****_****_****
    //bit15-12: channel 5
    //bit11-8:channel 6
    //bit7-4:channel 7
    //bit3-0:channel 8
    //0000=0 db(default), 0001=1db,0010=2db,0011=3db,0100=4db,0101=5db,0110=6db,0111=7db,1000=8db,1001=9db,1010=10db,1011=11db,1100=12db, other=do not use
#ifdef EMP_355
    SpiControlSetting(0xff,0x2b,0x8888);	//from new g30
#else   
   SpiControlSetting(0xff,0x2b, 0x0000);
#endif
   // SpiControlSetting(0xff,0x2b, 0x6666);
   //  SpiControlSetting(0xff,0x2b,0x8888);	//from new g30
   // SpiControlSetting(0xff,0x2b,0xcccc);

    //other setting
    //0x42---- 16'b1000_0000_1**0_**0* set 16'b1000_0000_1100_0000
    //bit0: DIFF_CLK, 1= diff clk on, 0=single-end clock(default)
    //bit2: EN_DCC, 1= ENABLES THE DUTY-CYCLE CPRRECTOPM CIRCUIT,0=OFF(DEFAULT)
    //BIT3: EXT_REF_VCM, 0=external reference drives REFT AND REFB(default), 1=drives the external reference mode through the vcm pin
    //bit6-5:PHASE_DDR[1:0], 10=90 DEGREE(DEFAULT)
    //16'b1000_0000_1100_0101
//    SpiControlSetting(0xff,0x42,0x8085);
//    SpiControlSetting(0xff,0x42,0x80a5);
    
    //SpiControlSetting(0xff,0x0142,0x80c5);//chip1
     
   // SpiControlSetting(0xff,0x0242,0x80c5);//80c5  > 80e5 //chip2
     
    //SpiControlSetting(0xff,0x0342,0x80c5);//chip3
     
   // SpiControlSetting(0xff,0x0442,0x80c5);//chip4
    
//  SpiControlSetting(0xff,0x42,0x80c5); //chip1-2-3-4
//
//    SpiControlSetting(0xff,0x42,0x80e5);
    //16'b1000_0000_1100_1101----exe_ref mode of afe5805 is useless
    // SpiControlSetting(0xff,0x42, 0x80cd);
#ifdef EMP_355
      SpiControlSetting(0xff, 0x42, 0x80c5);
#else
 	  SpiControlSetting(0x03, 0x42, 0x8085 | (2<<5));	//F11B, ab 片 相位2
      SpiControlSetting(0x0C, 0x42, 0x8085 | (1<<5));	//F11B, cd 片 相位1
#endif
   
    //other setting
    //0x46----16'b10*0_0010_000*_**00  set 16'b1000_0010_0000_0100
    //bit13,bit4:control whether the LCLK rising or falling edge comes in the middle of the data window when operating in SDR output mode.11=falling,01=rising(default)
    //bit4:EN_SDR, enables sdr output mode,0=disable,ddr output(default),1=enable
    //bit3:MSB_FIRST,1=MSB_FIRST,0=LSB_FIRST(default)

    //bie2:BTC_MODE,0=straight offset binary(default),1=binary two's complement
     SpiControlSetting(0xff,0x46,0x820c);
    // SpiControlSetting(0xff,0x46,0x8208);
	// SpiControlSetting(0xff,0x0F,0x0200); 

}

void  CFpgaGeneral::RunChip(void)
{
	SpiControlSetting(0x0f, 0x08, 0x00);
}

void  CFpgaGeneral::ResetControl(void)
{
	SpiControlSetting(0x0f, 0x08, 0x03);
}

void  CFpgaGeneral::PowerOff(void)
{
	SpiControlSetting(0x0f, 0x08, 0x01);
}

void  CFpgaGeneral::Ready(void) //准备就绪
{
	SpiControlSetting(0x0f, 0x08, 0x02);
}

void  CFpgaGeneral::Update(void)
{
	SpiControlSetting(0x0f, 0xff, 0x01);
}

void  CFpgaGeneral::SetIndex(void)
{
	SpiControlSetting(0x0f, 0x04, 0x0f);	
	SpiControlSetting(0x0f, 0x05, 0x0f);
	
}

void  CFpgaGeneral::AD9273_TestIO(bool TYPE)
{
	if(TYPE==true)
	{	
	//	SpiControlSetting(0x0f, 0x0d, 0x00);		//TEST_IO	on		Middle scale, 100000 000000
	//	SpiControlSetting(0x0f, 0x0d, 0x49);		//TEST_IO	on		1010 1010 1010
	//	SpiControlSetting(0x0f, 0x0d, 0x4A);		//TEST_IO	on		0000 0011 1111
	//	SpiControlSetting(0x0f, 0x0d, 0x4B);		//TEST_IO	on		1000 0000 0000
		SpiControlSetting(0x0f, 0x0d, 0x4C);		//TEST_IO	on		1010 0011 0011
	//	SpiControlSetting(0x0f, 0x0d, 0x48);		//TEST_IO	on		USER input
		Update();
	}
	else
	{
		SpiControlSetting(0x0f, 0x0d, 0x00);		//TEST_IO	off
		Update();
	}	
}

void CFpgaGeneral::AD9273_Test(unsigned char regaddr,unsigned char data)
{
	SpiControlSetting(0x0f, regaddr, data);
	Update();
}

//void  CFpgaGeneral::AdjustOutputPhase(U32 ChipCs, U32 PhaseValue)
void  CFpgaGeneral::AdjustOutputPhase(unsigned char chipcs, unsigned char phasevalue)
{
	SpiControlSetting(chipcs, 0x16, phasevalue);	
	Update();
}

//	ChipCs[0]---1:选中第一片ADC进行SPI控制
//	ChipCs[1]---1:选中第二片ADC进行SPI控制
//	ChipCs[2]---1:选中第三片ADC进行SPI控制
//	ChipCs[3]---1:选中第四片ADC进行SPI控制
void  CFpgaGeneral::SpiControlSetting(unsigned int chipcs, unsigned int regaddr,unsigned int data)
{
    INT32U value;
    value = (chipcs <<8) + regaddr;
#ifdef EMP_460
	PcieControl::GetInstance()->WriteOneDataToFpga(AdcCtrlAddr, value);
#else
	EzUsb::GetInstance()->WriteOneDataToFpga(AdcCtrlAddr, value);
#endif

    value = data;
#ifdef EMP_460
	PcieControl::GetInstance()->WriteOneDataToFpga(AdcCtrlData, value);
#else
	EzUsb::GetInstance()->WriteOneDataToFpga(AdcCtrlData, value);
#endif
}

void  CFpgaGeneral::AD9273_TestChNo(U32 ChipNo, U32 ChNoInChip)
{
    int data = ChipNo*8 + ChNoInChip;
#ifdef EMP_460
	PcieControl::GetInstance()->WriteOneDataToFpga(TESTLVDSCHADDR, data);;
#else
	EzUsb::GetInstance()->WriteOneDataToFpga(TESTLVDSCHADDR, data);;
#endif
#if 0
	FPGAConData[0] = ChipNo*8 + ChNoInChip;
    unsigned char fpgaAddr;
    fpgaAddr = (unsigned char)TESTLVDSCHADDR;

	unsigned char dataType;
	unsigned char paraLen;
    unsigned int data = 0;
	dataType = 1;
	paraLen = 1;	
    unsigned int *buf;
	buf = &data;

	//EzUsb::GetInstance()->BulkOut(2, 1, AdcCtrlAddr, 0);
	EzUsb::GetInstance()->BulkOut(dataType, paraLen, fpgaAddr, (unsigned char *)buf);


    //EzUsb::GetInstance()->BulkOut(1, 1, TESTLVDSCHADDR, 0);
    //EzUsb::GetInstance()->BulkOut(1, 1, fpgaAddr, NULL);
#endif
}

//	探头座选择(继电器切换)
//	
//	SocketNo---0：选第1个探头座；1：选第2个探头座
//	
void  CFpgaGeneral::ProbeSocketSel(U32 SocketNo)
{
 
	//venreq(0xBA,0x03);

//	#define COMAddrFPGAAddrA	58
//	#define COMDataFPGAAddrA    59
#if 0
	FPGAConData[0] = 3;				//探头座继电器切换 组合地址
	FPGAConData[0+1] = 0;//3>>8;		//

	EzUsb::GetInstance()->BulkOut(2, 1, COMAddrFPGAAddrA, 0);

	FPGAConData[0] = SocketNo;		//探头座继电器切换 组合数据
	FPGAConData[0+1] = SocketNo>>8;	//

	EzUsb::GetInstance()->BulkOut(2, 1, COMDataFPGAAddrA, 0);

    sleep(1);		//delay 1ms
#endif
}

unsigned char* CFpgaGeneral::venreq(unsigned char VendorReq,unsigned short ReqValue)
{
#if 0
//	if(VendorReq==0xB9)  //每次冻结解冻，将OimageBegin置零
//		 OimageBegin=0;
	bResult = FALSE;
	nBytes = 0;
	VENDOR_OR_CLASS_REQUEST_CONTROL	myRequest;
    short bufferSize = 64; 
	memset(VENbuffertemp, 0, bufferSize);
	
    // Open the driver
    if (bOpenDriver (&hDevice, pcDriverName) != TRUE)
	{
		hDevice = NULL;
		return NULL;
    }
    myRequest.request = VendorReq;
    myRequest.value = ReqValue;
    myRequest.index = 0XBEEF;
    myRequest.direction = 1;

    myRequest.requestType=2; // vendor specific request type (2)
    myRequest.recepient=0; // recepient is device (0)
				
	if (hDevice != NULL) 
	{
		bResult = DeviceIoControl (hDevice,
			IOCTL_EZUSB_VENDOR_OR_CLASS_REQUEST,
			&myRequest,
			sizeof(VENDOR_OR_CLASS_REQUEST_CONTROL),
			VENbuffertemp,
			bufferSize,
			(unsigned long *)&nBytes,
			NULL);
		CloseHandle (hDevice);
		hDevice=NULL;
     }/* if valid driver handle */       
#endif
	 return VENbuffertemp;
}
