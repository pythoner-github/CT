
#include "ImgProcCfm.h"
#include "ImgProc2D.h"
#include "DscMan.h"
#include "GlobalClassMan.h"
#include "display/HintArea.h"
#include "display/TopArea.h"

//const int ImgProcCfm::THRESHOLD[MAX_THRESHOLD] = {255, 239, 223, 207, 191, 175, 159, 143, 127, 111, 95, 79, 63, 47, 31, 15, 0};
const int ImgProcCfm::THRESHOLD[MAX_THRESHOLD] = {0, 15, 31, 47, 63, 79, 95, 111, 127, 143, 159, 175, 191, 207, 223, 239, 255};
const int ImgProcCfm::PERSIST[MAX_PERSIST] = {64,56,48,40,32,28,24,16};
ImgProcCfm* ImgProcCfm::m_ptrInstance = NULL;

ImgProcCfm::ImgProcCfm()
{
	m_threshold = 0;
	m_thresholdBak = 0;
	m_persist = 0;	
	m_reject = 0;
	m_smooth = 0;
	m_artifact = 0;
	m_backgroundHide = FALSE;
	m_colorHide = FALSE;
	m_cfmCompare = FALSE;

	m_ptrDscPara = DscMan::GetInstance()->GetDscPara();
	m_ptrUpdate = GlobalClassMan::GetInstance()->GetUpdateCfm();
}

ImgProcCfm::~ImgProcCfm()
{
	if (m_ptrInstance != NULL)
		delete m_ptrInstance;
}

ImgProcCfm* ImgProcCfm::GetInstance()
{
	if (m_ptrInstance == NULL)
		m_ptrInstance = new ImgProcCfm;

	return m_ptrInstance;
}

void ImgProcCfm::InitOptimize(ExamItem::ParaColor* color)
{
	EKnobReturn ret;

    //reject
	m_reject = color->reject;
	if(m_reject == (MAX_REJECT-1))
		ret = MAX;
	else if(m_reject == 0)
		ret = MIN;
	else
		ret = OK;
	SetReject(m_reject, ret);

	//smooth
	m_smooth = color->smooth;
	if(m_smooth == (MAX_SMOOTH-1))
		ret = MAX;
	else if(m_smooth == 0)
		ret = MIN;
	else
		ret = OK;
	SetSmooth(m_smooth, ret);

	//persist
	m_persist = color->persist;
	if(m_persist == (MAX_PERSIST-1))
		ret = MAX;
	else if(m_persist == 0)
		ret = MIN;
	else
		ret = OK;
	SetPersist(m_persist, ret);

    //artifact
    m_artifact = color->artifact;
	if(m_artifact == (MAX_ARTIFACT - 1))
		ret = MAX;
	else if(m_artifact == 0)
		ret = MIN;
	else
		ret = OK;
#if 0
    ProbeSocket::ProbePara para;
    ProbeMan::GetInstance()->GetCurProbe(para);
    string s;
    TopArea::GetInstance()->GetCheckPart(s);
    if(strcmp(para.model, "35C50K") == 0)
    {
        if(strcmp(s.c_str(), _("Kidney Ureter")) == 0)
        {
            m_artifact = 0;
        }
    }
    else if(strcmp(para.model, "75L40K") == 0)
    {
        if(strcmp(s.c_str(), _("Carotid")) == 0)
        {
            m_artifact = 0;
        }
    }
#endif
    SetArtifact(m_artifact, ret);
}

void ImgProcCfm::Init(ExamItem::ParaColor* color)
{

    InitOptimize(color);

	//threshold
	//m_threshold = 0;
	m_threshold = MAX_THRESHOLD - 1;
	SetThreshold(m_threshold, MAX);
	m_colorHide = FALSE;
  
	//background hide
	m_backgroundHide = FALSE;
	m_ptrUpdate->BackgroundHide(m_backgroundHide);
	
	//2d+cfm
	m_cfmCompare = FALSE;
	m_ptrUpdate->CfmCompare(FALSE);
}

void ImgProcCfm::GetCurPara(ExamItem::ParaItem* ptrParaItem)
{
	ptrParaItem->color.reject = m_reject;
	ptrParaItem->color.smooth = m_smooth;
	ptrParaItem->color.persist = m_persist;
	ptrParaItem->color.artifact = m_artifact;
}

/*
 * @brief only used when chroma is off
 */
void ImgProcCfm::ChangeColorHide()
{
    if (m_colorHide)
    {
        m_colorHide = FALSE;
    }
    else
    {
        m_colorHide = TRUE;
    }
    ColorHide(m_colorHide);
}

void ImgProcCfm::SetThresholdOff()
{
	ColorHide(FALSE);
}

EKnobReturn ImgProcCfm::ChangeBackgroundHide(bool on)
{
    DscMan* ptrDscMan = DscMan::GetInstance();
    ptrDscMan->GetWriteLock();
	m_ptrDscPara->dcaCFMIPAttrs.ipaHideBackground = on;
    CDSC* ptrDsc = DscMan::GetInstance()->GetDsc();
    if (ptrDsc != NULL)
        ptrDsc->Update();
    ptrDscMan->ReadWriteUnlock();
	return OK;
}
EKnobReturn ImgProcCfm::ChangeThreshold(EKnobOper oper)
{
	if (!ImgProc2D::GetInstance()->GetChromaStatus())
	{
		if ((oper == ADD) && (m_threshold < (MAX_THRESHOLD - 1)))
			m_threshold ++;
		else if ((oper == SUB) && (m_threshold > 0))
			m_threshold --;
		else
			return ERROR;

		EKnobReturn ret;
		if(m_threshold == (MAX_THRESHOLD-1))
			ret = MAX;
		else if(m_threshold == 0)
			ret = MIN;
		else
			ret = OK;

		// threshold
		SetThreshold(m_threshold, ret);

		// color hide
		if(m_threshold == 0)
			m_colorHide = TRUE;
		else
			m_colorHide = FALSE;
		return ret;
	}
    else
    {
		HintArea::GetInstance()->UpdateHint(_("[Threshold]: Invalid when chroma is on."), 1);
    }

	return ERROR;
}
EKnobReturn ImgProcCfm::ChangePersist(EKnobOper oper)
{
	if ((oper == ADD) && (m_persist < (MAX_PERSIST - 1)))
		m_persist ++;
	else if ((oper == SUB) && (m_persist > 0))
		m_persist --;
	else
		return ERROR;

	EKnobReturn ret;
	if(m_persist == (MAX_PERSIST-1))
		ret = MAX;
	else if(m_persist == 0)
		ret = MIN;
	else
		ret = OK;

	SetPersist(m_persist, ret);

	return ret;
}
EKnobReturn ImgProcCfm::ChangeReject(EKnobOper oper)
{
	if ((oper == ADD) && (m_reject < (MAX_REJECT - 1)))
		m_reject ++;
	else if ((oper == SUB) && (m_reject > 0))
		m_reject --;
	else
		return ERROR;

	EKnobReturn ret;
	if(m_reject == (MAX_REJECT-1))
		ret = MAX;
	else if(m_reject == 0)
		ret = MIN;
	else
		ret = OK;
	
	SetReject(m_reject, ret);

	return ret;	
}
EKnobReturn ImgProcCfm::ChangeSmooth(EKnobOper oper)
{
	if ((oper == ADD) && (m_smooth < (MAX_SMOOTH - 1)))
		m_smooth ++;
	else if ((oper == SUB) && (m_smooth > 0))
		m_smooth --;
	else
		return ERROR;

	EKnobReturn ret;
	if(m_smooth == (MAX_SMOOTH-1))
		ret = MAX;
	else if(m_smooth == 0)
		ret = MIN;
	else
		ret = OK;

	SetSmooth(m_smooth, ret);

	return ret;	
}
EKnobReturn ImgProcCfm::ChangeArtifact(EKnobOper oper)
{
	if ((oper == ADD) && (m_artifact < (MAX_ARTIFACT - 1)))
		m_artifact ++;
	else if ((oper == SUB) && (m_artifact > 0))
		m_artifact --;
	else
		return ERROR;

	EKnobReturn ret;
	if(m_artifact == (MAX_ARTIFACT-1))
	{
		ret = MAX;
	}
	else if(m_artifact == 0)
	{
		ret = MIN;
	}
	else
	{
		ret = OK;
	}

	SetArtifact(m_artifact, ret);

	return ret;	
}

///> private
void ImgProcCfm::ColorHide(bool on)
{
	EKnobReturn ret = OK;
	if (on)
	{
		m_threshold = 0;
		ret = MIN;
	}
	else
	{
		m_threshold = MAX_THRESHOLD - 1;
		ret = MAX;
	}

	SetThreshold(m_threshold, ret);
}
void ImgProcCfm::SetThreshold(int index, EKnobReturn ret)
{
	m_ptrDscPara->dcaCFMIPAttrs.ipaThreshold = THRESHOLD[index];
	m_ptrUpdate->Threshold(THRESHOLD[index], ret);
}
void ImgProcCfm::SetPersist(int index, EKnobReturn ret)
{
	m_ptrDscPara->dcaCFMIPAttrs.ipaFrameAver = PERSIST[index];
	m_ptrUpdate->Persist(index, ret);
}
void ImgProcCfm::SetReject(int index, EKnobReturn ret)
{
	m_ptrUpdate->Reject(index, ret);
}
void ImgProcCfm::SetSmooth(int index, EKnobReturn ret)
{
	m_ptrDscPara->dcaCFMIPAttrs.ipaSmooth = index;
	m_ptrUpdate->Smooth(index, ret);
}
void ImgProcCfm::SetArtifact(int index, EKnobReturn ret)
{
	m_ptrDscPara->dcaCFMIPAttrs.ipaArtifact = index;
	m_ptrUpdate->Artifact(index, ret);
}
