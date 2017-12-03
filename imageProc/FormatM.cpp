/*
 * 2009, 深圳恩普电子技术有限公司
 *
 * @file: FormatM.cpp
 * @brief: control M diplay format and current image
 *
 * version: V1.0
 * date: 2009-5-27
 * @author: zhanglei
 */

#include "FormatM.h"
#include "MDSC.h"
#include "GlobalClassMan.h"
#include "KeyFunc.h"
#include "KeyFunc.h"
#include "ImgProcM.h"
#include "Zoom.h"
#include "SysOptions.h"

FormatM* FormatM::m_ptrInstance = NULL;

FormatM* FormatM::GetInstance()
{
	if (m_ptrInstance == NULL)
		m_ptrInstance = new FormatM;

	return m_ptrInstance;
}

FormatM::FormatM()
{
	GlobalClassMan* ptrGcm = GlobalClassMan::GetInstance();
	m_ptrUpdate = ptrGcm->GetUpdate2D();

	m_ptrDsc = DscMan::GetInstance();
	m_ptrDscPara = m_ptrDsc->GetDscPara(); 

	m_ptrImg = Img2D::GetInstance();
	m_ptrReplay = Replay::GetInstance();

	m_format = M_TOTAL;
	m_formatSnap = M_TOTAL;
}
FormatM::~FormatM()
{
	if (m_ptrInstance != NULL)
		delete m_ptrInstance;
}


void FormatM::ChangeFormat(enum EFormatM format)
{
	bool isAnatomicMode = FALSE;

	m_ptrDsc->GetWriteLock();	

	// if anatomic M, init anatomic line
	if (ScanMode::GetInstance()->GetScanMode() == ScanMode::ANATOMIC_M)
	{
		isAnatomicMode = TRUE;
		if (format == M_TOTAL)
			return;
	}
	m_format = format;

    Update2D::SetCineRemoveImg(-1);

	// reset anatomic line
	if (isAnatomicMode == TRUE)
		ImgProcM::GetInstance()->AnatomicLineInit();

	//send to dsc
	switch(m_format)
	{
		case M_TOTAL:
			m_ptrDscPara->dcaMDisplayFormat = 0;
			break;
			
		case BM11_UD:
			m_ptrDscPara->dcaMDisplayFormat = 1;
			break;
		
		case BM21_UD:
			m_ptrDscPara->dcaMDisplayFormat = 3;
			break;
			
		case BM12_UD:
			m_ptrDscPara->dcaMDisplayFormat = 2;
			break;
		
		case BM11_LR:
			m_ptrDscPara->dcaMDisplayFormat = 4;
			break;
				
		default:
			m_ptrDscPara->dcaMDisplayFormat = 0;
			break;		
	}
	m_ptrDsc->GetDsc()->UpdateMDisplayFormat();
	m_ptrDsc->ReadWriteUnlock();	

	// set replay area number
	m_ptrReplay->SetAreaNum(1);
    Update2D::SetCineRemoveImg(3);

	//update view
	KeyClearScreen kcs;
	kcs.ModeExecute();

    // change pointer focus
    if (isAnatomicMode)
        MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::ANATOMIC_M);
    else
        MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::M);
}

FormatM::EFormatM FormatM::GetFormat()
{
    if (ScanMode::GetInstance()->IsSpecialMeasureStatus())
        return m_formatSnap;
    else
        return m_format;
}
/*
 * @brief get B image's height in dot, according to current display format
 * @note only used in M mode
 */
int FormatM::GetBImgHeightDot(void)
{
	int height = IMAGE_H;

	switch (m_format)
	{
		case M_TOTAL:
			height = 0;
			break;

		case BM11_UD:
			height = height / 2;
			break;

		case BM21_UD:
			height = height * 2 / 3;
			break;

		case BM12_UD:
			height = height * 1 / 3;
			break;

		case BM11_LR:
			height = height;
			break;
	}

	return height;
}

/*
 * @biref get center point 's coordinate of B image, according to current display format
 * @note only used in M mode
 */
POINT FormatM::GetBCenterPoint(void)
{
	POINT p;

	switch(m_format)
	{
		case M_TOTAL:
			p.x = IMAGE_X + IMAGE_W / 2;
			p.y = IMAGE_Y + IMAGE_H / 2;
			break;

		case BM11_UD:
			p.x = IMAGE_X + IMAGE_W / 4;
			p.y = IMAGE_Y + IMAGE_H / 8;
			break;

		case BM21_UD:
			p.x = IMAGE_X + IMAGE_W / 4;
			p.y = IMAGE_Y + IMAGE_H / 3;
			break;

		case BM12_UD:
			p.x = IMAGE_X + IMAGE_W / 3;
			p.y = IMAGE_Y + IMAGE_H / 10;
			break;

		case BM11_LR:
			p.x = IMAGE_W / 4;
			p.y = IMAGE_H / 2;
			break;		

		default:
			p.x = IMAGE_X + IMAGE_W / 2;
			p.y = IMAGE_Y + IMAGE_H / 2;
			break;
	}

	return p;
}

