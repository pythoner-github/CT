
#include "MultiFuncFactory.h"
#include "MultiFuncMode.h"
#include "MultiFuncValue.h"
#include "MultiFuncBodyMark.h"
#include  "ViewMain.h"
#include  "MultiFuncArrow.h"

MultiFuncFactory* MultiFuncFactory::m_ptrInstance = NULL;
AbsMultiFunc* MultiFuncFactory::m_ptrMulti = NULL;

MultiFuncFactory::MultiFuncFactory()
{
	m_type = NONE;
	m_typePre = NONE;
}

MultiFuncFactory::~MultiFuncFactory()
{
	if (m_ptrInstance != NULL)
		delete m_ptrInstance;
}

MultiFuncFactory* MultiFuncFactory::GetInstance()
{
	if (m_ptrInstance == NULL)
		m_ptrInstance = new MultiFuncFactory;

	return  m_ptrInstance;
}

AbsMultiFunc* MultiFuncFactory::Create(EMultiFunc type)
{
	ModeStatus ms;
    m_typePre = type;

	if (m_ptrMulti != NULL)
	{
		delete m_ptrMulti;
		m_ptrMulti = NULL;
	}

	m_type = type;
	switch(type)
	{
		case NONE:
			InvisibleCursor(FALSE, FALSE);
			m_ptrMulti = new MultiFuncNone;
			break;

		case FOCUS:
			InvisibleCursor(TRUE);
			m_ptrMulti = new MultiFuncFocus;
			break;

        case DEPTH:
			InvisibleCursor(TRUE);
			m_ptrMulti = new MultiFuncDepth;
			break;

		case LOCAL_ZOOM:
			InvisibleCursor(TRUE);
			m_ptrMulti = new MultiFuncLocalZoom;
			break;

		case GLOBAL_ZOOM:
			InvisibleCursor(TRUE);
			m_ptrMulti = new MultiFuncGlobalZoom;
			break;

		case PIP_ZOOM:
			InvisibleCursor(TRUE);
			m_ptrMulti = new MultiFuncPIPZoom;
			break;
		
		case MEASURE:
			InvisibleCursor(TRUE);
			m_ptrMulti = new MultiFuncMeasure;
			break;
	
        case CALC:
			InvisibleCursor(TRUE);
			//m_ptrMulti = new MultiFuncCalc;
			break;

		case TEXT:
			InvisibleCursor(FALSE);
			m_ptrMulti = new MultiFuncText;
			break;

		case ARROW:
			InvisibleCursor(TRUE);
			m_ptrMulti = new MultiFuncArrow;
			break;

		case BODYMARK:
			InvisibleCursor(FALSE);
			m_ptrMulti = new MultiFuncBodyMark;
			break;

		case GRAY_TRANS:
			InvisibleCursor(TRUE);
			m_ptrMulti = new MultiFuncGrayTrans;
			break;

        case GRAY_REJECT:
            InvisibleCursor(TRUE);
            m_ptrMulti = new MultiFuncGrayReject;
            break;

        case FREQ:
            InvisibleCursor(TRUE);
            m_ptrMulti = new MultiFuncFreq;
            break;

        case CHROMA:
            InvisibleCursor(TRUE);
            m_ptrMulti = new MultiFuncChroma;
            break;

		case M_INIT:
			InvisibleCursor(TRUE);
			m_ptrMulti = new MultiFuncMInit;
			break;

		case M:
			InvisibleCursor(TRUE);
			m_ptrMulti = new MultiFuncM;
			break;

		case PW_INIT:
			InvisibleCursor(TRUE);
			m_ptrMulti = new MultiFuncPwInit;
			break;

		case PW:
			InvisibleCursor(TRUE);
			m_ptrMulti = new MultiFuncPw;
			break;

		case CFM:
			InvisibleCursor(TRUE);
			m_ptrMulti = new MultiFuncCfm;
			break;

		case PWCFM_INIT:
			InvisibleCursor(TRUE);
			m_ptrMulti = new MultiFuncPwCfmInit;
			break;

		case PWCFM:
			InvisibleCursor(TRUE);
			m_ptrMulti = new MultiFuncPwCfm;
			break;
	
		case PWPDI_INIT:
			InvisibleCursor(TRUE);
			m_ptrMulti = new MultiFuncPwPdiInit;
			break;

		case PWPDI:
			InvisibleCursor(TRUE);
			m_ptrMulti = new MultiFuncPwPdi;
			break;

		case ANATOMIC_M:
			InvisibleCursor(TRUE);
			m_ptrMulti = new MultiFuncAnatomicM;
			break;

		case REPLAY:
			InvisibleCursor(TRUE);
			m_ptrMulti = new MultiFuncReplay;
			break;

		case EFOV:
			InvisibleCursor(TRUE);
			m_ptrMulti = new MultiFuncEFOV;
			break;

        case CW_INIT:
			InvisibleCursor(TRUE);
			m_ptrMulti = new MultiFuncCwInit;
			break;

		case CW:
			InvisibleCursor(TRUE);
			m_ptrMulti = new MultiFuncCw;
			break;

        case CWCFM_INIT:
			InvisibleCursor(TRUE);
			m_ptrMulti = new MultiFuncCwCfmInit;
			break;

		case CWCFM:
			InvisibleCursor(TRUE);
			m_ptrMulti = new MultiFuncCwCfm;
			break;
	
		case CWPDI_INIT:
			InvisibleCursor(TRUE);
			m_ptrMulti = new MultiFuncCwPdiInit;
			break;

		case CWPDI:
			InvisibleCursor(TRUE);
			m_ptrMulti = new MultiFuncCwPdi;
			break;
		case BIOPSY_VERIFY:
			InvisibleCursor(TRUE);
		//	InvisibleCursor(false);
			m_ptrMulti=new MultiFuncBiopsyVerify;
			break;
		case BIOPSY:
			InvisibleCursor(FALSE,FALSE);
			m_ptrMulti=new MultiFuncBiopsy;
			break;
		default:
			break;
	}

	return m_ptrMulti;
}

///> global func
void MultiFuncUndo()
{
	ModeStatus ms;
	MultiFuncFactory::EMultiFunc type;	
	//----------2016.08.22---------------------------//
	if(MenuArea::GetInstance()->GetMenuType()==MenuArea::BIOPSYVERIFY)
	{
		type=MultiFuncFactory::BIOPSY_VERIFY;
		MultiFuncFactory::GetInstance()->Create(type);
		return;
	}

	//----------------------------------------------------//
	if ((ms.GetScanMode() == ScanMode::EFOV) && !(Replay::GetInstance()->GetReadSnapStatus()))
	{
		type = MultiFuncFactory::EFOV;
		MultiFuncFactory::GetInstance()->Create(type);
		return;
	}

	if (ms.GetFreezeMode() == FreezeMode::UNFREEZE)
	{
		switch(ms.GetScanMode())
		{
			case ScanMode::M_INIT:
				type = MultiFuncFactory::M_INIT;
				break;

			case ScanMode::M:
				type = MultiFuncFactory::M;
				break;

			case ScanMode::PW_INIT:
				type = MultiFuncFactory::PW_INIT;
				break;

			case ScanMode::PW:
			case ScanMode::PW_SIMULT:
				type = MultiFuncFactory::PW;
				break;

			case ScanMode::CFM:
			case ScanMode::PDI:
			case ScanMode::CFM_VS_2D:
			case ScanMode::PDI_VS_2D:
				type = MultiFuncFactory::CFM;
				break;

			case ScanMode::PWCFM_INIT:
				type = MultiFuncFactory::PWCFM_INIT;
				break;

			case ScanMode::PWCFM:
			case ScanMode::PWCFM_SIMULT:
				type = MultiFuncFactory::PWCFM;
				break;

			case ScanMode::PWPDI_INIT:
				type = MultiFuncFactory::PWPDI_INIT;
				break;

			case ScanMode::PWPDI:
			case ScanMode::PWPDI_SIMULT:
				type = MultiFuncFactory::PWPDI;
				break;

			case ScanMode::ANATOMIC_M:
				type = MultiFuncFactory::ANATOMIC_M;
				break;

            case ScanMode::CW_INIT:
				type = MultiFuncFactory::CW_INIT;
				break;

			case ScanMode::CW:
				type = MultiFuncFactory::CW;
				break;

            case ScanMode::CWCFM_INIT:
				type = MultiFuncFactory::CWCFM_INIT;
				break;

			case ScanMode::CWCFM:
				type = MultiFuncFactory::CWCFM;
				break;

			case ScanMode::CWPDI_INIT:
				type = MultiFuncFactory::CWPDI_INIT;
				break;

			case ScanMode::CWPDI:
				type = MultiFuncFactory::CWPDI;
				break;

			default:
				type = MultiFuncFactory::NONE;
				break;
		}
	}
	else 
	{
		if (Replay::GetInstance()->GetReadSnapStatus())
			type = MultiFuncFactory::NONE;
		else
			type = MultiFuncFactory::REPLAY;
	}
	MultiFuncFactory::GetInstance()->Create(type);
}


