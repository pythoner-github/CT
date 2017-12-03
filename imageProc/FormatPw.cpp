/*
 * 2009, 深圳恩普电子技术有限公司
 *
 * @file: FormatPw.cpp
 * @brief: control Pw diplay format and current image
 *
 * version: V1.0
 * date: 2009-6-2
 * @author: zhanglei
 */

#include "FormatPw.h"
#include "Replay.h"
#include "GlobalClassMan.h"
#include "KeyFunc.h"
#include "FreezeMode.h"
#include "GlobalClassMan.h"
#include "Zoom.h"
#include "SysOptions.h"

FormatPw* FormatPw::m_ptrInstance = NULL;

FormatPw* FormatPw::GetInstance()
{
	if (m_ptrInstance == NULL)
		m_ptrInstance = new FormatPw;

	return m_ptrInstance;
}

FormatPw::FormatPw()
{
	GlobalClassMan* ptrGcm = GlobalClassMan::GetInstance();
	m_ptrUpdate = ptrGcm->GetUpdatePw();

	m_ptrDsc = DscMan::GetInstance();
	m_ptrDscPara = m_ptrDsc->GetDscPara();
	
	m_ptrImg = ImgPw::GetInstance();
	m_ptrReplay = Replay::GetInstance();

	m_format = P_TOTAL;
	m_formatSnap = P_TOTAL;
	m_pwDots = IMG_H / 2;
}
FormatPw::~FormatPw()
{
	if (m_ptrInstance != NULL)
		delete m_ptrInstance;
}

void FormatPw::ChangeFormat(enum EFormatPw format)
{
    Update2D::SetCineRemoveImg(-1);

	m_ptrDsc->GetWriteLock();
	m_format = format;

	switch(m_format)
	{
		case P_TOTAL:
			m_ptrDscPara->dcaPWDisplayFormat = 0;
			// set replay area number
			m_ptrReplay->SetAreaNum(1);
			break;
			
		case BP11_UD:
			// set replay area number
			m_ptrDscPara->dcaPWDisplayFormat = 1;
			m_ptrReplay->SetAreaNum(2);
			break;
		
		case BP21_UD:
			// set replay area number
			m_ptrDscPara->dcaPWDisplayFormat = 3;
			m_ptrReplay->SetAreaNum(2);
			break;
			
		case BP12_UD:
			// set replay area number
			m_ptrDscPara->dcaPWDisplayFormat = 2;
			m_ptrReplay->SetAreaNum(2);
			break;
		
		case BP11_LR:
			// set replay area number
			m_ptrDscPara->dcaPWDisplayFormat = 4;
			m_ptrReplay->SetAreaNum(2);
			break;
				
		default:
			m_ptrDscPara->dcaPWDisplayFormat = 1;
			// set replay area number
			m_ptrReplay->SetAreaNum(1);
			break;
   }
	
    // send image dots to dsc and fpga
	m_pwDots = m_ptrDsc->GetDsc()->GetPWHeight();
	int dots = m_pwDots;
	if (dots > 256)
		dots = 256;
	m_ptrDscPara->dcaPWSamplePoints = dots;
	ImgPw::GetInstance()->SetRGBSampleDots(dots);

	// update
	if (ModeStatus::IsUnFreezeMode())
		m_ptrDsc->GetDsc()->ClearPWLine();

	m_ptrDsc->GetDsc()->UpdatePWDisplayFormat();
	m_ptrDsc->ReadWriteUnlock();

    Update2D::SetCineRemoveImg(3);

	//update view
	KeyClearScreen kcs;
	kcs.ModeExecute();

    // change pointer focus
   MultiFuncUndo();
#if 0
	if (ModeStatus::IsUnFreezeMode())
	{
		if (ModeStatus::IsSpectrumImgMode())
			MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::PW);
		else if (ModeStatus::IsPWCFMImgMode())
			MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::PWCFM);
        else
			MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::PWPDI);
	}
#endif
}


FormatPw::EFormatPw FormatPw::GetFormat()
{
    if (ScanMode::GetInstance()->IsSpecialMeasureStatus())
        return m_formatSnap;
    else
        return m_format;
}

