#include "imageProc/FpgaReceive.h"
#include "imageProc/Format2D.h"
#include <BDSC.h>
#include "imageControl/Img2D.h"
#include "imageProc/DscMan.h"
#include "imageControl/ImgPw.h"
//for test
#include "base/CalcTime.h"
#include "imageProc/ModeStatus.h"
#include "imageControl/D4FuncMan.h"
#include "keyboard/MultiFuncMode.h"
#include "imageControl/QuickAdjustmentPw.h"

#ifdef EMP_460 //G40
	PcieControl* FpgaReceive::m_ptrUsb = PcieControl::GetInstance();
#else
	EzUsb* FpgaReceive::m_ptrUsb = EzUsb::GetInstance();
#endif

CDSC* FpgaReceive::m_ptrDsc = NULL;
ScanMode* FpgaReceive::m_ptrScanMode = ScanMode::GetInstance();
extern int tempoffsound;
extern bool updateoffsound;

#ifdef EMP_355
int g_fps = 0;
#endif
/**
* @brief service routine only used to receive data from usb device
*/
void FpgaReceive::UsbServiceRoutine(void *arg)
{
	DscMan* ptrDscMan = DscMan::GetInstance();
	const int pktSize = 512;
#ifdef EMP_355
	const int pktPerBlockMax = 16;
    int pktPerBlock = 16;
#else
	const int pktPerBlockMax = 16;
    int pktPerBlock = 16;
#endif
	int ret;
	int pkt;
	unsigned char *ptrSrc = NULL;
	unsigned char buffer[pktPerBlockMax* pktSize];
	int len, i;
	int j;

	len = pktSize * pktPerBlockMax;
	ptrSrc = buffer;
	memset(ptrSrc, 0, len);

	//test begin
	ScanMode* ptrMode = ScanMode::GetInstance();
	ScanMode::EScanMode mode;
	DSCCONTROLATTRIBUTES* m_ptrDscPara = DscMan::GetInstance()->GetDscPara();
	int count = 0;
	int simultData = 0;
	CalcTime ct;
	InitDataCfm();
	Img2D *ptr2D = Img2D::GetInstance();
	ImgPw *ptrPw = ImgPw::GetInstance();
	int scanRange[2];
	unsigned char boxRange[2];
	scanRange[0] = 0;
	scanRange[1] = 127;
	unsigned char BitsBuf[pktSize];
	unsigned char* pBits = BitsBuf;
	int rmColor = 0;//12;
    ScanMode::EScanMode fpgaMode;
    // test end
		//AbsUpdatePw* m_ptrUpdate;
	while (1)
	{
#if 1
//#define DEBUG_VM
//#ifndef DEBUG_VM
// 接收USB的真实数据
		if (ModeStatus::IsUnFreezeMode())
		{
begin:
			//read 16/ package data to ptrSrc
            fpgaMode = m_ptrScanMode->GetFpgaScanMode();
            if ((fpgaMode == ScanMode::PW) || (fpgaMode == ScanMode::CW))
                pktPerBlock = m_ptrDscPara->dcaPWSpeed;
            else
                pktPerBlock = pktPerBlockMax;
            len = pktPerBlock * pktSize;

			ret = m_ptrUsb->ReadBufFromFpga(len, ptrSrc);
			if (ret < 0)
			{
				goto begin;
			}

			//copy data to mem
			unsigned char *ptrTemp;
            static int n = 0;

			for (pkt = 0; pkt < pktPerBlock; pkt++)
            {
                int offset; //offset in block
                offset = pkt * pktSize;
                ptrTemp = ptrSrc + offset;
#if 0

                    if ((!ModeStatus::IsCWImgMode()) || (!ImgPw::GetInstance()->GetCwImgCtrl()))
                        n = 0;

                    if (( ModeStatus::IsCWImgMode()) && (ImgPw::GetInstance()->GetCwImgCtrl()))
                    {
                        if(ptrTemp[16]==4)
                        {
                            if(n<80)
                                memset(ptrTemp+24, 0, (pktSize-24));
                            else
                            {
                                if(tempoffsound)
                                {
                                    m_ptrCalc->CalcSoundStatus(1);
                                    tempoffsound=0;
                                }
                            }
                            n++;
                        }
                    }
#endif

                    PRINTF("---flag=%d autoOn=%d\n", ptrTemp[16], QuickAdjustmentPw::m_onQAPw);
                    if (ptrTemp[16] == 2) //第17位标识是模式
                    {
#ifdef EMP_430
                        memset(ptrTemp+450, 0, 52);
#endif
                        //memset(ptrTemp+450, 0, 62);
#if 0
                        for(i =8; i< 20;i++)
                        {
                            printf("%d ",ptrTemp[i]);
                        }
                        printf("\n\n");
#endif
                    }
                    else if (ptrTemp[16] == 1)
                    {
                        memcpy(pBits, ptrTemp, 24);
                        memcpy(pBits+24, ptrTemp+24+rmColor, pktSize-24-rmColor);
                        memset(pBits+pktSize-rmColor, ptrTemp[pktSize-1], rmColor);
                        ptrTemp = pBits;
                    }

#if 0

                printf("------------output 0-25-----------");
                printf("\n");
                for(i =0; i< 26;i++)
                {
                    printf("%d ",ptrTemp[i]);
                }
                printf("\n");
#endif

#ifdef EMP_355
                boxRange[1] = m_ptrDscPara->dcaCFMScanLEnd;
                scanRange[1] = m_ptrDscPara->dcaCurScanEnd;

                if (fpgaMode == ScanMode::D2 || fpgaMode == ScanMode::M || fpgaMode == ScanMode::PW_SIMULT)
                {
                    if (ptrTemp[16] == 2 && ptrTemp[8] == scanRange[1])
                        g_fps++;
                }
                else if (fpgaMode == ScanMode::CFM || fpgaMode == ScanMode::PDI || fpgaMode == ScanMode::PWCFM_SIMULT || fpgaMode == ScanMode::PWPDI_SIMULT)
                {
                    if (ptrTemp[16] == 1 && ptrTemp[8] == boxRange[1])
                    {
                        g_fps++;
                    }
                }
#endif
                if (D4FuncMan::GetInstance()->Get4DMode())
                {
                    D4FuncMan::GetInstance()->GetDataFromUsb(ptrTemp);
                }
                else
                {
#ifdef EMP_355
                    if (QuickAdjustmentPw::m_onQAPw)
                    {
                        if (ptrTemp[16] == 4)
                        {
                            PRINTF("send data to QAPw\n");
                            QuickAdjustmentPw::GetInstance()->SendDataToQAPw(ptrTemp);
                        }
                    }
//                    else
//                    {
//                        // 正在接收pw数据时，切换到了其他模式，需关闭QAPw，初始化各变量
//                        QuickAdjustmentPw::GetInstance()->SetParaChangeMode();
//                    }

                    // 由于控件调节后，pw数据会出现异常，将初始更新的pw数据不发给DSC
                    QuickAdjustmentPw::GetInstance()->SetParaChangeKnob(ptrTemp[16]);

					mode = ptrMode->GetScanMode();
					if(!((QuickAdjustmentPw::m_shutInitialPw)&&((mode == ScanMode::PW) || (mode == ScanMode::PW_SIMULT) || (mode == ScanMode::PWCFM) || (mode == ScanMode::PWPDI) || (mode == ScanMode::PWCFM_SIMULT) || (mode == ScanMode::PWPDI_SIMULT))))
					{
						ptrDscMan->SendDataToDsc(ptrTemp);
					}
#else
                    ptrDscMan->SendDataToDsc(ptrTemp);
#endif
                }
            }
        }
        else
        {
            usleep(1000);
        }
#endif

		//随机自动产生渐变的数据 for PW 2D CFM test
#if 0
		if (ModeStatus::IsUnFreezeMode())
		{
			//2D
			ptr2D->GetScanRange(scanRange);
			for(i = scanRange[0]; i<= scanRange[1]+1; i++)
			{
				Data2D(pBits, i, simultData);
				ptrDscMan->SendDataToDsc(pBits);
			}

			//CFM
			mode = ptrMode->GetScanMode();
			int cur = ptrMode->GetPwCurImg();
			if ((mode == ScanMode::CFM) || (mode == ScanMode::PWCFM_INIT) || (mode == ScanMode::PDI) || (mode == ScanMode::PWPDI_INIT)
					|| ((mode == ScanMode::PWPDI) && (cur == 1)) || ((mode == ScanMode::PWCFM) && (cur == 1))
					|| (mode == ScanMode::CFM_VS_2D) || (mode == ScanMode::PDI_VS_2D)
					)
			{
				ImgCfm::GetInstance()->GetBoxRange(boxRange);
				for (i = boxRange[0]; i <= boxRange[1]; i ++)
				{
					DataCfm(pBits, i);
					ptrDscMan->SendDataToDsc(pBits);
				}
			}
			//M
			if (mode == ScanMode::M)
			{
				DataM(pBits, simultData);
				ptrDscMan->SendDataToDsc(pBits);
			}
			//PW
			if (((mode == ScanMode::PW) || ((mode == ScanMode::PWCFM) && (cur == 2)) || ((mode == ScanMode::PWPDI) && (cur == 2)))
			    || ((mode == ScanMode::CW) || ((mode == ScanMode::CWCFM) && (cur == 2)) || ((mode == ScanMode::CWPDI) && (cur == 2))))
			{
				DataPw(pBits, simultData);
				ptrDscMan->SendDataToDsc(pBits);
			}

			if (simultData < 255)
				simultData += 1;
			else
				simultData = 0;
		}

#endif

		// 根据usb的传输速度自动产生渐变的数据 pw
#if 0
		if (ModeStatus::IsUnFreezeMode())
		{
begin:
            //read 16 package data to ptrSrc
            if (m_ptrScanMode->GetFpgaScanMode() == PW)
                pktPerBlock = m_ptrDscPara->dcaPWSpeed;
            else
                pktPerBlock = pktPerBlockMax;
            len = pktPerBlock * pktSize;

#if 0
            ret = m_ptrUsb->ReadBufFromFpga(len, ptrSrc);
            if (ret < 0)
            {
                PRINTF("bulkin error!\n");
                goto begin;
            }
#endif

            //copy data to mem
            for (pkt = 0; pkt < pktPerBlock; pkt++)
            {
                mode = ptrMode->GetScanMode();
                if (mode == ScanMode::PW)
                    DataPw(pBits, count);
                else
                    Data2D(pBits, count);
                if (count < 255)
                    count ++;
                else
                    count = 0;

                //			if(m_ptrDsc != NULL)
                {
                    ptrDscMan->GetWriteLock();
                    m_ptrDsc->GetUSBDatas(pBits);//数据由何好处理，一次发送512，分16次发完一包
                    ptrDscMan->ReadWriteUnlock();
                }

            }
        }
#endif
    }
    free(ptrSrc);
    delete []pBits;
}

/**
 * @brief get the ptrDsc and save it
 */
void FpgaReceive::SetDsc(CDSC *ptrDsc)
{
    m_ptrDsc = ptrDsc;
}

void FpgaReceive::Data2D(unsigned char *data, int line, int imgData)
{
    int j;

    memset(data, imgData, 512);
    //memset(data, line, 512);
    unsigned char yBits[8] = {2,255,2,255,2,255,2,255};
    memcpy(data+16, yBits, 8);
    memset(data+8, line, 8);

#if 0
    for(j = 0; j < 32; j++)
        PRINTF("%d  ", data[j]);
    PRINTF("\n");
#endif

}
void FpgaReceive::DataPw(unsigned char *data, int line)
{
    int j;

    memset(data, line, 512);
    unsigned char yBits[8] = {4,255,4,255,4,255,4,255};
    memcpy(data+16, yBits, 8);
    memset(data+8, line, 8);

#if 0
    for(j = 0; j < 152; j++)
        PRINTF("%d  ", data[j]);
    PRINTF("\n");
#endif
}
void FpgaReceive::DataCfm(unsigned char *data, int line)
{
    int j;
    const int MAX_SEC = 160;

    memset(data, 128, 512);
    unsigned char yBits[8] = {1,255,1,255,1,255,1,255};
    memcpy(data+16, yBits, 8);
    memset(data+8, line, 4);
    memset(data+12, 0, 4);
    memcpy(data+24,m_cfmData, 160*3);

#if 0
    for(j = 0; j < 32; j++)
        PRINTF("%d  ", data[j]);
    PRINTF("\n");
#endif
}
void FpgaReceive::DataM(unsigned char *data, int line)
{
    int j;

    memset(data, line, 512);
    unsigned char yBits[8] = {3,255,3,255,3,255,3,255};
    memcpy(data+16, yBits, 8);
    //memset(data+8, line, 8);
    for(int z=0; z<480; z++)
    {
        if(z < 255)
            data[24+z] = z;
        else
            data[24+z] = 255;
    }

#if 0
    for(j = 0; j < 32; j++)
        PRINTF("%d  ", data[j]);
    PRINTF("\n");
#endif
}
void FpgaReceive::InitDataCfm()
{
    int i;

    for (i = 0; i < 160; i ++)
    {
        m_cfmData[i * 3 + 0] = 128;
        m_cfmData[i * 3 + 1] = 128;
        m_cfmData[i * 3 + 2] = 128;
    }
}