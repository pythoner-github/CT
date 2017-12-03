/*
 * 2009, 深圳恩普电子技术有限公司
 *
 * @file: FormatCfm.cpp
 * @brief: control cfm/pdi diplay format and current image
 *
 * version: V1.0
 * date: 2009-5-27
 * @author: zhanglei
 */

#include "FormatCfm.h"
#include "Format2D.h"
#include "GlobalClassMan.h"

FormatCfm* FormatCfm::m_ptrInstance = NULL;

FormatCfm* FormatCfm::GetInstance()
{
	if (m_ptrInstance == NULL)
		m_ptrInstance = new FormatCfm;

	return m_ptrInstance;
}

FormatCfm::FormatCfm()
{
	GlobalClassMan* ptrGcm = GlobalClassMan::GetInstance();
	m_ptrUpdate = ptrGcm->GetUpdateCfm();

	m_ptrDsc = DscMan::GetInstance();
	m_ptrDscPara = m_ptrDsc->GetDscPara();

	m_ptrReplay = Replay::GetInstance();
	m_ptrImg = ImgCfm::GetInstance();

	m_format = B;
	m_formatSnap = B;
	m_curB = 0;
}
FormatCfm::~FormatCfm()
{
	if (m_ptrInstance != NULL)
		delete m_ptrInstance;
}

/*
 * @brief change cfm format to "format", when it changd 2D format will be changed too.
 *
 * @para format format that will be changed to
 */
void FormatCfm::ChangeFormat(enum EFormatCfm format)
{
	m_format = format;

	m_curB = Format2D::GetInstance()->ChangeFormat((Format2D::EFormat2D)format);

	if (m_format == B)
	{
		m_ptrDsc->CreateDscObj(DscMan::CFMB);
	}
	else if (m_format == BB)
	{
		m_ptrDsc->CreateDscObj(DscMan::CFMBB);
	}
	else if (m_format == B4)
	{
		m_ptrDsc->CreateDscObj(DscMan::CFMB4);
	}

	m_ptrImg->SetBoxStatus(false);

	///> update view
	m_ptrUpdate->ChangeFormatCfm(m_format);
}


//在2B冻结下，切换区域，当当前不等于冻结前最后区域时解冻后要切换重绘zjx
bool FormatCfm::SwitchBBForFreeze(int lastRealB)
{
    m_curB = lastRealB;

   if (Format2D::GetInstance()->SwitchBBForFreeze(lastRealB))
    {
        m_ptrUpdate->ChangeCurrentImgBB(lastRealB);
        return TRUE;
    }

    return FALSE;
}



/*
 * @brief switch current image in BB format, 2D current image of BB will be changed tof
 */
bool FormatCfm::SwitchBB(bool left, int &current)
{
  //对冻结和非冻结下切换进行不同处理 zjx
    if (ModeStatus::IsUnFreezeMode())
    {
        m_ptrImg->SetBoxStatus(false);
        if (Format2D::GetInstance()->SwitchBB(left, current))
        {
            m_ptrUpdate->ChangeCurrentImgBB(m_curB);
            return TRUE;
        }
    }
    else
    {
        Format2D::GetInstance()->SwitchBB(left, current);
    }
    return FALSE;
}

int FormatCfm::SwitchBB(void)
{
  //对冻结和非冻结下切换进行不同处理 zjx
    if (ModeStatus::IsUnFreezeMode())
    {
        m_ptrImg->SetBoxStatus(false);
        m_curB = Format2D::GetInstance()->SwitchBB();

        ///> update
        m_ptrUpdate->ChangeCurrentImgBB(m_curB);
    }
    else
    {
        m_curB = Format2D::GetInstance()->SwitchBB();
    }
    return m_curB;
}

//在4B冻结下，切换区域，当当前不等于冻结前最后区域时解冻后要切换重绘zjx
int FormatCfm::SwitchB4ForFreeze(int curB, int lastRealB)
{
    m_curB = curB;

    Format2D::GetInstance()->SwitchB4ForFreeze(m_curB);
    m_ptrUpdate->ChangeCurrentImg4BForFreeze(lastRealB);

    return FALSE;
}


int FormatCfm::SwitchB4()
{
    //对冻结和非冻结下切换进行不同处理 zjx
    if (ModeStatus::IsUnFreezeMode())
    {
        m_ptrImg->SetBoxStatus(false);
        m_curB = Format2D::GetInstance()->SwitchB4();

        ///> update
        m_ptrUpdate->ChangeCurrentImg4B(m_curB);
    }
    else
    {
        m_curB = Format2D::GetInstance()->SwitchB4();
    }
	return m_curB;

}

FormatCfm::EFormatCfm FormatCfm::GetFormat()
{
    if (ScanMode::GetInstance()->IsSpecialMeasureStatus())
        return m_formatSnap;
    else
        return m_format;
}
