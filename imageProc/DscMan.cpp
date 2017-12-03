
#include "DscMan.h"
#include "BDSC.h"
#include "BBDSC.h"
#include "B4DSC.h"
#include "MDSC.h"
#include "PWDSC.h"
#include "CFMDSC.h"
#include "CFMBBDSC.h"
#include "CFMB4DSC.h"
#include "PWCFMDSC.h"
#include "CFMVS2DDSC.h"
#include "CMMDSC.h"
#include "CFMDSC_Rate.h"
#include "CFMBBDSC_Rate.h"
#include "CFMB4DSC_Rate.h"
#include "CFMVS2DDSC_Rate.h"
#include "PWCFMDSC_Rate.h"
#include "CFMDSC_R.h"
#include "CFMBBDSC_R.h"
#include "CFMB4DSC_R.h"
#include "CFMVS2DDSC_R.h"
#include "PWCFMDSC_R.h"
#include "EFOVDSC.h"
#include "Img2D.h"
#include "Zoom.h"
#include "ImgCfm.h"

DscMan* DscMan::m_ptrInstance = NULL;

DscMan::DscMan()
{
	m_ptrDsc = NULL;
	InitDscPara();

	// init pthrad lock
	if (pthread_mutex_init(&m_pthreadLock, NULL) == 0)
	{
		PRINTF("pthread mutex lock create ok\n");
		m_lockIsCreate = TRUE;	
	}
	else
		m_lockIsCreate = FALSE;	
#ifdef EMP_355
	pthread_mutex_init(&m_pthreadLock1, NULL);
	pthread_mutex_init(&m_pthreadLock2, NULL);
#endif
	// replay resource lock
	if (pthread_mutex_init(&m_replayLock, NULL) == 0)
	{
		PRINTF("replay mutex lock create ok\n");
		m_replayLockIsCreate = TRUE;	
	}
	else
		m_replayLockIsCreate= FALSE;	

#ifdef EMP_355
    m_receive = new CReceive(&m_ptrDsc, m_pthreadLock, m_pthreadLock1);
#endif
}

DscMan::~DscMan()
{
	if (m_ptrInstance != NULL)
		delete m_ptrInstance;

	if (m_lockIsCreate)
		pthread_mutex_destroy(&m_pthreadLock);
#ifdef EMP_355
    pthread_mutex_destroy(&m_pthreadLock1);
    pthread_mutex_destroy(&m_pthreadLock2);
#endif
	if (m_replayLockIsCreate)
		pthread_mutex_destroy(&m_replayLock);

#ifdef EMP_355
    delete m_receive;
#endif
}

DscMan* DscMan::GetInstance()
{
	if (m_ptrInstance == NULL)
		m_ptrInstance = new DscMan;

	return m_ptrInstance;
}

void DscMan::SendDataToDsc(const unsigned char *pData)
{
	if((m_ptrInstance != NULL) && (m_ptrDsc != NULL))
	{
#ifndef EMP_355 
		GetWriteLock();
		m_ptrDsc->GetUSBDatas(pData);//数据由何好处理，一次发送512，分16次发完一包
		ReadWriteUnlock();
#else
		pthread_mutex_lock(&m_pthreadLock2);
        m_receive->ReceiveData(pData);
		pthread_mutex_unlock(&m_pthreadLock2);
#endif
	}	
}

CDSC* DscMan::CreateDscObj(EDSCObj type)
{
	// lock for multi-thread
#ifdef EMP_355
	pthread_mutex_lock(&m_pthreadLock2);
#endif
	GetWriteLock();

	// destroy the last object
	if (m_ptrDsc != NULL)
	{
		m_ptrDsc->Destroy();
		m_ptrDsc =  NULL;
	}

	// create new object
	UPDATEFRAME m_ptrFuncUpdateFrame = ImageArea::DrawImgData;
	//UPDATEFRAME m_ptrFuncUpdateFrame = ImageArea::DrawImgData(3);
	switch(type)
	{
		case B:
#ifdef EMP_355
            m_receive->SetScanMode(CReceive::MODE_2D);
#endif
			m_ptrDsc = CBDSC::Create(m_dscPara, m_ptrFuncUpdateFrame);
			break;

		case BB:
#ifdef EMP_355
            m_receive->SetScanMode(CReceive::MODE_2D);
#endif
			m_ptrDsc = CBBDSC::Create(m_dscPara, m_ptrFuncUpdateFrame);
			break;

		case B4:
#ifdef EMP_355
            m_receive->SetScanMode(CReceive::MODE_2D);
#endif
			m_ptrDsc = CB4DSC::Create(m_dscPara, m_ptrFuncUpdateFrame);
			break;

		case M:
#ifdef EMP_355
            m_receive->SetScanMode(CReceive::MODE_M);
#endif
			m_ptrDsc = CMDSC::Create(m_dscPara, m_ptrFuncUpdateFrame);
			break;

		case PW:
#ifdef EMP_355
            m_receive->SetScanMode(CReceive::MODE_PW);
#endif
			m_ptrDsc = CPWDSC::Create(m_dscPara, m_ptrFuncUpdateFrame);
			break;

		case CFMB:
#ifdef EMP_355
            m_receive->SetScanMode(CReceive::MODE_CFM);
#endif
			m_ptrDsc = CCFMDSC_R::Create(m_dscPara, m_ptrFuncUpdateFrame);
			break;

		case CFMBB:
#ifdef EMP_355
            m_receive->SetScanMode(CReceive::MODE_CFM);
#endif
			m_ptrDsc = CCFMBBDSC_R::Create(m_dscPara, m_ptrFuncUpdateFrame);
			break;

		case CFMB4:
#ifdef EMP_355
            m_receive->SetScanMode(CReceive::MODE_CFM);
#endif
			m_ptrDsc = CCFMB4DSC_R::Create(m_dscPara, m_ptrFuncUpdateFrame);
			break;
		
		case PWCFM:
#ifdef EMP_355
            m_receive->SetScanMode(CReceive::MODE_PWCFM);
#endif
			m_ptrDsc = CPWCFMDSC_R::Create(m_dscPara, m_ptrFuncUpdateFrame);
			break;

		case CFMVS2D:
#ifdef EMP_355
            m_receive->SetScanMode(CReceive::MODE_CFM);
#endif		
			m_ptrDsc = CCFMVS2DDSC_R::Create(m_dscPara, m_ptrFuncUpdateFrame);
			break;

		case MANATOMIC:
			POINT p1, p2;
			p1.x = 100;
			p1.y = 100;
			p2.x = 200;
			p2.y = 200;
#ifdef EMP_355
            m_receive->SetScanMode(CReceive::MODE_2D);
#endif
			m_ptrDsc = CCMMDSC::Create(m_dscPara, m_ptrFuncUpdateFrame, p1, p2);
			break;

		case EFOV:
#ifdef EMP_355
            m_receive->SetScanMode(CReceive::MODE_2D);
#endif
			m_ptrDsc = CEFOVDSC::Create(m_dscPara, m_ptrFuncUpdateFrame);
			break;
		
		default:
			break;
	}

	m_fpgaReceive.SetDsc(m_ptrDsc);
	Img2D::GetInstance()->SetDsc(m_ptrDsc);
	Zoom::GetInstance()->SetDsc(m_ptrDsc);
    //Zoom::GetInstance()->ReSendLocalZoomInfo();
	ImgCfm::GetInstance()->SetDsc(m_ptrDsc);

	// unlock
	ReadWriteUnlock();
#ifdef EMP_355
		pthread_mutex_unlock(&m_pthreadLock2);
#endif

	return m_ptrDsc;
}

//private
void DscMan::InitDscPara()
{
	// 2D
	m_dscPara.dcaProbeWidth = 7200;
	m_dscPara.dcaCurMaxScanLines = 256;
	m_dscPara.dcaCurScanStart = 0;
	m_dscPara.dcaCurScanEnd = 255;
	m_dscPara.dcaSamplePoints = 200;//440;
	m_dscPara.dcaScale = 0.54;
	m_dscPara.dcaSteer = 0;//degree    
	m_dscPara.dcaTpView = 0;
	m_dscPara.dcaProbeR = 50;
	m_dscPara.dcaProbeType = 'C';
	m_dscPara.dcaB4Flag = 0;
	m_dscPara.dcaBBFlag = 0;
	m_dscPara.dca2DIPAttrs.ipaPolarityOverturn = 0;
	m_dscPara.dca2DIPAttrs.ipaUDOverturn = 0;
	m_dscPara.dca2DIPAttrs.ipaLROverturn = 0;
	m_dscPara.dca2DIPAttrs.ipaRotate = 0;
	m_dscPara.dca2DIPAttrs.ipaFrameAver = 32;//32;
	m_dscPara.dca2DIPAttrs.ipaLineAver = 64;//48;
	m_dscPara.dca2DIPAttrs.ipaPIPZoom = 0;
	m_dscPara.dca2DIPAttrs.ipaGlobalZoom = 0;
	m_dscPara.dca2DIPAttrs.ipaLocalZoom = 0;
	m_dscPara.dca2DIPAttrs.ipaEdgeEhn = 0;
	m_dscPara.dca2DIPAttrs.ipaSmooth = 0;
	m_dscPara.dca2DIPAttrs.ipaReject = 0;
	m_dscPara.dca2DIPAttrs.ipaImgEhn = 0;

	int i;
	for(i = 0; i < 256; i ++)
	{
		m_dscPara.dcaPalette[i].rgbtRed = i;
		m_dscPara.dcaPalette[i].rgbtGreen = i;
		m_dscPara.dcaPalette[i].rgbtBlue = i;
	}

	// CFM
	m_dscPara.dcaCFMScanLStart = 50;		
	m_dscPara.dcaCFMScanLEnd = 100;		
	m_dscPara.dcaCFMSampPtStart = 10;		
	m_dscPara.dcaCFMSampPtEnd = 159;	
	m_dscPara.dcaCFMLDensity = 1;
	m_dscPara.dcaCFMSteer = 0;
	m_dscPara.dcaCFMBaseline = 128;
	m_dscPara.dcaCFMIsDirection = 1;
	for (i = 0; i < 256; i ++)
	{
		m_dscPara.dcaCFMPalette[i].rgbtRed = i;
		m_dscPara.dcaCFMPalette[i].rgbtGreen = i;
		m_dscPara.dcaCFMPalette[i].rgbtBlue = i;
	}
	m_dscPara.dcaCFMIPAttrs.ipaThreshold = 0;	
	m_dscPara.dcaCFMIPAttrs.ipaFrameAver = 32;	
	m_dscPara.dcaCFMIPAttrs.ipaHideBackground = false;	
	m_dscPara.dcaCFMIPAttrs.ipaArtifact = 1;

	// PW
	m_dscPara.dcaPWSamplePoints = 256;//128; ///< pw orginal data size
	m_dscPara.dcaPWStartPosAxisY = 0;	
	m_dscPara.dcaPWSpeed = 2;//8;		
	m_dscPara.dcaPWFlag = 0;		
	m_dscPara.dcaPWDisplayFormat = 1;		
	m_dscPara.dcaPWIPAttrs.ipaUDOverturn = FALSE;		
	m_dscPara.dcaPWIPAttrs.ipaTrace.status = FALSE;
	m_dscPara.dcaPWIPAttrs.ipaTrace.type = 0;
	m_dscPara.dcaPWIPAttrs.ipaTrace.direction = 0;
	m_dscPara.dcaPWIPAttrs.ipaTrace.threshold = 0;
	m_dscPara.dcaPWIPAttrs.ipaTrace.smooth = 0;
	m_dscPara.dcaPWIPAttrs.ipaTrace.baseline = 1;
	m_dscPara.dcaPWIPAttrs.ipaTrace.fs = 10;

	// M
	m_dscPara.dcaMSpeed = 2;		
	m_dscPara.dcaMDisplayFormat = 0;	
	m_dscPara.dcaMIPAttrs.ipaSmooth = 0;	
	m_dscPara.dcaMIPAttrs.ipaEnhance = 0;	
}

