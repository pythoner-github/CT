/*
 * 2009, 深圳恩普电子技术有限公司
 *
 * @file: Format2D.cpp
 * @brief: control 2d diplay format and current image
 *
 * version: V1.0
 * date: 2009-5-27
 * @author: zhanglei
 */

#include "Format2D.h"
#include "Def.h"
#include "ModeStatus.h"
#include "GlobalClassMan.h"
#include "KeyFunc.h"
#include "ImgProc2D.h"
#include "../keyboard/KeyFunc.h"

Format2D* Format2D::m_ptrInstance = NULL;

Format2D* Format2D::GetInstance()
{
	if (m_ptrInstance == NULL)
		m_ptrInstance = new Format2D;

	return m_ptrInstance;
}

Format2D::Format2D()
{
	GlobalClassMan* ptrGcm = GlobalClassMan::GetInstance();
	m_ptrUpdate = ptrGcm->GetUpdate2D();

	m_ptrDsc = DscMan::GetInstance();
	m_ptrDscPara = m_ptrDsc->GetDscPara();

	m_ptrImg = Img2D::GetInstance();
	m_ptrReplay = Replay::GetInstance();

	m_format = B;
	m_formatSnap = B;
	m_curB = 0;
	m_curRealB = m_curB;
	m_curRealB_bak = m_curRealB;
}
Format2D::~Format2D()
{
	if (m_ptrInstance != NULL)
		delete m_ptrInstance;
}

/*
 * @brief change 2D format to "format"
 *
 * @para format format that will be changed to
 *
 * @retval current B
 */
int Format2D::ChangeFormat(enum EFormat2D format)
{
    PRINTF("--------change format B begin\n");
	m_format = format;

    // exit zoom
    MultiFuncFactory::EMultiFunc type = MultiFuncFactory::GetInstance()->GetMultiFuncType();
    if ((type == MultiFuncFactory::LOCAL_ZOOM) || (type == MultiFuncFactory::GLOBAL_ZOOM) || (type == MultiFuncFactory::PIP_ZOOM))
    {
            MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::NONE);
    }

    // parepare
    Update2D::SetCineRemoveImg(-1);
    m_ptrImg->InitSeperateScale(m_curRealB); // Init scale when current real B image is changed
    ImgProc2D::GetInstance()->SetRotate(0, MIN);

	m_curB = 0;
    m_curRealB = 0;

	///> send to dsc
	m_ptrDscPara->dcaBBFlag = m_curB;
	m_ptrDscPara->dcaB4Flag = m_curB;

#if 1
	if (m_format == B)
	{
		m_ptrDsc->CreateDscObj(DscMan::B);

		///> replay region switch
		m_ptrReplay->SetAreaNum(1);
	}
	else if (m_format == BB)
	{
		m_ptrDsc->CreateDscObj(DscMan::BB);

		///> replay region switch
		m_ptrReplay->SetAreaNum(2);
	}
	else if (m_format == B4)
	{
		m_ptrDsc->CreateDscObj(DscMan::B4);

		///> replay region switch
		m_ptrReplay->SetAreaNum(4);
	}
#endif
	///> update view
	m_ptrUpdate->ChangeFormat2D(m_format);
    Update2D::SetCineRemoveImg(3);

	// unfreeze
	if (!ModeStatus::IsUnFreezeMode())
	{
		FreezeMode::GetInstance()->PressUnFreeze();
	}
	else
	{
		KeyClearScreen kcs;
		kcs.ModeExecute();
	}
    PRINTF("--------change format B end\n");

	return m_curB;
}


/*
 * @brief switch current image in BB format
 * @para left[in] true-left, false-right
 *
 * @retval current image, range(0-1)
 */
bool Format2D::SwitchBB(bool left, int &current)
{
    // is unfreeze?
    bool unfreeze = FALSE;
    if (ModeStatus::IsUnFreezeMode())
        unfreeze = TRUE;
    if (((m_curB == 0) && (left)) || ((m_curB == 1) && (!left)))
    {
        current = m_curB;
        return FALSE;
    }

 // 在冻结下，若选中区域没有图像，则返回   zjx
    if(!unfreeze)
    {
        if(m_ptrReplay->ReviewNoImgBB(m_curB))
            return FALSE;
    }

    switch (m_curB) {
        case 0:// left
            m_curB = 1;
            //scale
            if (unfreeze) {
                m_ptrImg->ChangeSeperateScale(1, 0);
                m_curRealB = m_curB;
            }
            break;

    case 1:
        m_curB = 0;

        //scale
        if (unfreeze) {
            m_ptrImg->ChangeSeperateScale(0, 1);
            m_curRealB = m_curB;
        }
        break;

    default:
        break;
    }

    //send dsc
    m_ptrDsc->GetWriteLock();
    m_ptrDscPara->dcaBBFlag = m_curB;

    ///> replay region
    DscMan::GetInstance()->GetDsc()->UpdateBBFlag();
    m_ptrDsc->ReadWriteUnlock();
    m_ptrReplay->SwitchArea(m_curB, unfreeze);
    if (!unfreeze)
    {
        m_ptrReplay->PrepareForReplay();
        m_ptrReplay->ReviewOneImg();
    }

    ///> update
    m_ptrUpdate->ChangeCurrentImgBB(m_curB);

    current = m_curB;
    return TRUE;
}

//在2B冻结下，当当前区域不等于冻结前最后区域时，解冻后进行区域切换zjx
int Format2D::SwitchBBForFreeze(int lastRealB)
{
    m_curB = lastRealB;
    m_ptrImg->ChangeSeperateScale(m_curB, m_curRealB);
    m_curRealB = m_curB;

    ///> update
    m_ptrUpdate->ChangeCurrentImgBB(m_curB);
    return TRUE;
}

int Format2D::SwitchBB(void)
{
    // is unfreeze?
    bool unfreeze = FALSE;
    if (ModeStatus::IsUnFreezeMode())
        unfreeze = TRUE;

 // 在冻结下，若选中区域没有图像，则返回   zjx
    if(!unfreeze)
    {
        if(m_ptrReplay->ReviewNoImgBB(m_curB))
            return FALSE;
    }

    switch (m_curB) {
        case 0:// left
            m_curB = 1;

            //scale
            if (unfreeze) {
                m_ptrImg->ChangeSeperateScale(1, 0);
                m_curRealB = m_curB;
            }
            break;

    case 1:
        m_curB = 0;

        //scale
        if (unfreeze) {
            m_ptrImg->ChangeSeperateScale(0, 1);
            m_curRealB = m_curB;
        }
        break;

    default:
        break;
    }

    //send dsc
    m_ptrDsc->GetWriteLock();
    m_ptrDscPara->dcaBBFlag = m_curB;

    ///> replay region
    DscMan::GetInstance()->GetDsc()->UpdateBBFlag();
    m_ptrDsc->ReadWriteUnlock();
    m_ptrReplay->SwitchArea(m_curB, unfreeze);
    if (!unfreeze)
    {
        m_ptrReplay->PrepareForReplay();
        m_ptrReplay->ReviewOneImg();
    }

    ///> update
    m_ptrUpdate->ChangeCurrentImgBB(m_curB);

    return TRUE;
}


//在4B冻结下，当当前区域不等于冻结前最后区域时，解冻后进行区域切换zjx
int Format2D::SwitchB4ForFreeze(int lastRealB)
{
    m_curB = lastRealB;
    m_ptrImg->ChangeSeperateScale(m_curB, m_curRealB);
    m_curRealB_bak = m_curRealB;
    m_curRealB = m_curB;

    m_ptrUpdate->ChangeCurrentImg4B(m_curB);

    return TRUE;
}

/*
 * @brief switch current image in 4B format
 *
 * @retval current image, range(0-3)
 */
int Format2D::SwitchB4()
{
	// is unfreeze?
	bool unfreeze = FALSE;
	if (ModeStatus::IsUnFreezeMode())
	{
		unfreeze = TRUE;
	}

 // 在冻结下，若选中区域没有图像，则显示下一个   zjx
    if(!unfreeze)
    {
        for(int i = m_curB; i<4; i++)
        {
            if(m_ptrReplay->ReviewNoImgB4(i))
            {
                if(i == 3)
                    i= 0;
            }
            else
            {
                m_curB = i;
                break;
            }
        }
    }

    switch (m_curB)
    {
        case 0:
            m_curB = 1;

            if (unfreeze)
            {
                m_ptrImg->ChangeSeperateScale(1, 0);
                m_curRealB = m_curB;
            }
			break;

		case 1:
			m_curB = 2;

			if (unfreeze)
            {
				m_ptrImg->ChangeSeperateScale(2, 1);
                m_curRealB = m_curB;
            }
			break;

		case 2:
			m_curB = 3;
			if (unfreeze)
            {
				m_ptrImg->ChangeSeperateScale(3, 2);
                m_curRealB = m_curB;
            }
			break;

		case 3:
			m_curB = 0;

			if (unfreeze)
            {
				m_ptrImg->ChangeSeperateScale(0, 3);
                m_curRealB = m_curB;
            }
			break;

		default:
			break;
	}

    //send to dsc
    m_ptrDsc->GetWriteLock();
    m_ptrDscPara->dcaB4Flag = m_curB;

    DscMan::GetInstance()->GetDsc()->UpdateB4Flag();
    m_ptrDsc->ReadWriteUnlock();

	///> replay region
	m_ptrReplay->SwitchArea(m_curB, unfreeze);
    if (!unfreeze)
    {
        m_ptrReplay->PrepareForReplay();
        m_ptrReplay->ReviewOneImg();
    }

	///> update
	m_ptrUpdate->ChangeCurrentImg4B(m_curB);

	return m_curB;
}

Format2D::EFormat2D Format2D::GetFormat()
{
    if (ScanMode::GetInstance()->IsSpecialMeasureStatus())
        return m_formatSnap;
    else
        return m_format;
}