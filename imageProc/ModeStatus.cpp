
#include "ModeStatus.h"
///> judge

bool ModeStatus::IsD2Mode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	

	if (mode == ScanMode::D2) 
		return TRUE;
	else
		return FALSE;
}
bool ModeStatus::IsMMode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	
	if ((mode == ScanMode::M) || (mode == ScanMode::M_INIT))
		return TRUE;
	else
		return FALSE;
}

bool ModeStatus::IsSpectrumMode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	

	if ((mode == ScanMode::PW_INIT) || (mode == ScanMode::PW) || (mode == ScanMode::PW_SIMULT)
            || (mode == ScanMode::CW_INIT) || (mode == ScanMode::CW))
		return TRUE;
	else
		return FALSE;
}
bool ModeStatus::IsColorMode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	
	if ((mode == ScanMode::CFM) || (mode == ScanMode::PDI) || (mode == ScanMode::CFM_VS_2D) || (mode == ScanMode::PDI_VS_2D))
		return TRUE;
	else
		return FALSE;
}
bool ModeStatus::IsPureColorMode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	
	if ((mode == ScanMode::CFM) || (mode == ScanMode::PDI))
		return TRUE;
	else
		return FALSE;
}
bool ModeStatus::IsSpectrumColorMode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	
	if ((mode == ScanMode::PWCFM_INIT) || (mode == ScanMode::PWCFM) || (mode == ScanMode::PWCFM_SIMULT) 
            || (mode == ScanMode::PWPDI_INIT) || (mode == ScanMode::PWPDI) || (mode == ScanMode::PWPDI_SIMULT) 
            || (mode == ScanMode::CWCFM_INIT) || (mode == ScanMode::CWCFM) || (mode == ScanMode::CWPDI_INIT) || (mode == ScanMode::CWPDI))
		return TRUE;
	else
		return FALSE;
}

bool ModeStatus::IsColorVS2DMode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	
	if ((mode == ScanMode::CFM_VS_2D))
		return TRUE;
	else
		return FALSE;
}
bool ModeStatus::IsPwMode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	

	if ((mode == ScanMode::PW) || (mode == ScanMode::PW_INIT) || (mode == ScanMode::PW_SIMULT) 
			|| (mode == ScanMode::PWCFM_INIT) || (mode == ScanMode::PWCFM) || (mode == ScanMode::PWCFM_SIMULT) 
			|| (mode == ScanMode::PWPDI_INIT) || (mode == ScanMode::PWPDI) || (mode == ScanMode::PWPDI_SIMULT))
		return TRUE;
	else
		return FALSE;
}
bool ModeStatus::IsCFMMode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	

	if ((mode == ScanMode::CFM) || (mode == ScanMode::PWCFM_INIT) || (mode == ScanMode::PWCFM) || (mode == ScanMode::PWCFM_SIMULT)
            || (mode == ScanMode::CWCFM) || (mode == ScanMode::CWCFM_INIT))
		return TRUE;
	else
		return FALSE;
}

bool ModeStatus::IsPDIMode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	

	if ((mode == ScanMode::PDI) || (mode == ScanMode::PWPDI_INIT) || (mode == ScanMode::PWPDI) || (mode == ScanMode::PWPDI_SIMULT))
		return TRUE;
	else
		return FALSE;
}

bool ModeStatus::IsSpectrumSimult()
{
    ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	

	if (mode == ScanMode::PW_SIMULT)
		return TRUE;
	else
		return FALSE;
}
bool ModeStatus::IsSpectrumColorSimult()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	

	if ((mode == ScanMode::PWCFM_SIMULT) || (mode == ScanMode::PWPDI_SIMULT))
		return TRUE;
	else
		return FALSE;
}

bool ModeStatus::IsPWMode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	

	if ((mode == ScanMode::PW_INIT) || (mode == ScanMode::PW) || (mode == ScanMode::PW_SIMULT))
		return TRUE;
	else
		return FALSE;
}
bool ModeStatus::IsPWColorMode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	
	if ((mode == ScanMode::PWCFM_INIT) || (mode == ScanMode::PWCFM) || (mode == ScanMode::PWCFM_SIMULT) 
            || (mode == ScanMode::PWPDI_INIT) || (mode == ScanMode::PWPDI) || (mode == ScanMode::PWPDI_SIMULT)) 
		return TRUE;
	else
		return FALSE;
}
bool ModeStatus::IsCWMode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	

	if ((mode == ScanMode::CW_INIT) || (mode == ScanMode::CW))
		return TRUE;
	else
		return FALSE;
}    
bool ModeStatus::IsCWColorMode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	
	if ((mode == ScanMode::CWCFM_INIT) || (mode == ScanMode::CWCFM) || (mode == ScanMode::CWPDI_INIT) || (mode == ScanMode::CWPDI)) 
		return TRUE;
	else
		return FALSE;
}

bool ModeStatus::IsNormalMode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	

	if ((mode == ScanMode::CFM_VS_2D) || (mode == ScanMode::PDI_VS_2D)|| (mode == ScanMode::ANATOMIC_M))
		return FALSE;
	else
		return TRUE;
}

bool ModeStatus::IsAnatomicMode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	

	if (mode == ScanMode::ANATOMIC_M)
		return TRUE;
	else
		return FALSE;
}

bool ModeStatus::IsCFMCompareMode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	

	if (mode == ScanMode::CFM_VS_2D)
		return TRUE;
	else
		return FALSE;
}

bool ModeStatus::IsPDICompareMode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	

	if (mode == ScanMode::PDI_VS_2D)
		return TRUE;
	else
		return FALSE;
}


bool ModeStatus::IsSimultMode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	
    
    if ((mode == ScanMode::PW_SIMULT) || (mode == ScanMode::PWCFM_SIMULT) || (mode == ScanMode::PWPDI_SIMULT))
        return TRUE;
    else
        return FALSE;
}

bool ModeStatus::IsFreezeMode()
{
	FreezeMode::EFreezeMode freeze = FreezeMode::GetInstance()->GetFreezeMode();

	if ((freeze == FreezeMode::FREEZE) || (freeze == FreezeMode::REPLAY))
		return TRUE;
	else
		return FALSE;
}
bool ModeStatus::IsUnFreezeMode()
{
	FreezeMode::EFreezeMode freeze = FreezeMode::GetInstance()->GetFreezeMode();

	if (freeze == FreezeMode::UNFREEZE)
		return TRUE;
	else
		return FALSE;
}
bool ModeStatus::IsPureFreezeMode()
{
	FreezeMode::EFreezeMode freeze = FreezeMode::GetInstance()->GetFreezeMode();

	if (freeze == FreezeMode::FREEZE)
		return TRUE;
	else
		return FALSE;
}
bool ModeStatus::IsAutoReplayMode()
{
    FreezeMode::EFreezeMode replay = FreezeMode::GetInstance()->GetFreezeMode();
	if (replay == FreezeMode::AUTOREPLAY)
		return TRUE;
	else
		return FALSE;
}
bool ModeStatus::IsReplayMode()
{
    FreezeMode::EFreezeMode replay = FreezeMode::GetInstance()->GetFreezeMode();
	if (replay == FreezeMode::REPLAY)
		return TRUE;
	else
		return FALSE;
}

bool ModeStatus::IsD2ImgMode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	
	if ((mode == ScanMode::D2) || (mode == ScanMode::M_INIT) || (mode == ScanMode::PW_INIT) || (mode == ScanMode::CW_INIT))
			return TRUE;
	else
		return FALSE;	
}
bool ModeStatus::IsMImgMode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	
	if (mode == ScanMode::M) 
			return TRUE;
	else
		return FALSE;	
}
bool ModeStatus::IsSpectrumImgMode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	

	if ((mode == ScanMode::PW) || (mode == ScanMode::CW) || (mode == ScanMode::PW_SIMULT))
		return TRUE;
	else
		return FALSE;	
}
bool ModeStatus::IsCwImgMode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	

	if (mode == ScanMode::CW)
		return TRUE;
	else
		return FALSE;	
}
bool ModeStatus::IsColorImgMode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	
	if ((mode == ScanMode::CFM) || (mode == ScanMode::PDI) || (mode == ScanMode::PWCFM_INIT) || (mode == ScanMode::PWPDI_INIT) 
            || (mode == ScanMode::CFM_VS_2D) || (mode == ScanMode::PDI_VS_2D))
		return TRUE;
	else
		return FALSE;	
}
bool ModeStatus::IsSpectrumColorImgMode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	
	if ((mode == ScanMode::PWCFM) || (mode == ScanMode::PWCFM_SIMULT) || (mode == ScanMode::PWPDI) || (mode == ScanMode::PWPDI_SIMULT)
	    ||(mode == ScanMode::CWCFM) || (mode == ScanMode::CWPDI))
		return TRUE;
	else
		return FALSE;
}

#if 0
bool ModeStatus::IsPWPDIImgMode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	
	if (mode == ScanMode::PWPDI) 
		return TRUE;
	else
		return FALSE;
}
#endif

bool ModeStatus::IsPWImgMode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	
	if ((mode == ScanMode::PW) || (mode == ScanMode::PWCFM) || (mode == ScanMode::PWCFM_SIMULT) || (mode == ScanMode::PWPDI) 
		|| (mode == ScanMode::PWPDI_SIMULT) || (mode == ScanMode::PW_SIMULT))
		return TRUE;
	else
		return FALSE;
}

bool ModeStatus::IsCWImgMode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	

	if ((mode == ScanMode::CW) || (mode == ScanMode::CWCFM) || (mode == ScanMode::CWPDI)) 
		return TRUE;
	else
		return FALSE;	
}    
bool ModeStatus::IsPWColorImgMode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	
	if ((mode == ScanMode::PWCFM) || (mode == ScanMode::PWCFM_SIMULT) || (mode == ScanMode::PWPDI) || (mode == ScanMode::PWPDI_SIMULT))
		return TRUE;
	else
		return FALSE;
}    
bool ModeStatus::IsCWColorImgMode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	
	if ((mode == ScanMode::CWCFM) || (mode == ScanMode::CWPDI))
		return TRUE;
	else
		return FALSE;
}    

bool ModeStatus::IsPWCFMImgMode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	
	if (mode == ScanMode::PWCFM) 
		return TRUE;
	else
		return FALSE;
}

bool ModeStatus::IsPWPDIImgMode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	
	if (mode == ScanMode::PWPDI) 
		return TRUE;
	else
		return FALSE;
}

bool ModeStatus::IsCWCFMImgMode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	
	if (mode == ScanMode::CWCFM) 
		return TRUE;
	else
		return FALSE;
}    
bool ModeStatus::IsCWPDIImgMode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	
	if (mode == ScanMode::CWPDI) 
		return TRUE;
	else
		return FALSE;
}
bool ModeStatus::IsColorPDIMode()
{
    ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	
    if ((mode == ScanMode::CFM) || (mode == ScanMode::PDI))
        return TRUE;
    else 
        return FALSE;
}
bool ModeStatus::IsCFMImgMode()
{
	if (IsCFMMode())
		return TRUE;
	else
		return FALSE;
}
bool ModeStatus::IsPDIImgMode()
{
	if (IsPDIMode())
		return TRUE;
	else
		return FALSE;
}

bool ModeStatus::IsNormalSpectrumImgMode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	

	if ((mode == ScanMode::PW) || (mode == ScanMode::CW))
		return TRUE;
	else
		return FALSE;	
}

bool ModeStatus::IsNormalSpectrumColorImgMode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	
	if ((mode == ScanMode::PWCFM) || (mode == ScanMode::PWPDI))
		return TRUE;
	else
		return FALSE;
}

bool ModeStatus::IsEFOVMode()
{
    ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();
    if (mode == ScanMode::EFOV)
        return true;
    else
        return false;
}

bool ModeStatus::IsMeasure2DMode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	
    ModeStatus ms;
    int pwImg = ms.GetPwCurImg();

    if (((mode == ScanMode::PW) || (mode == ScanMode::CW) || (mode == ScanMode::PWCFM) || (mode == ScanMode::PWPDI))
            && (pwImg == 2))
        return FALSE;
    else if ((mode == ScanMode::M) || (mode == ScanMode::ANATOMIC_M))
        return FALSE;
    else
        return TRUE;

}
bool ModeStatus::IsMeasureMMode() 
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	

    if ((mode == ScanMode::M) || (mode == ScanMode::ANATOMIC_M))
        return TRUE;
    else
        return FALSE;
}
bool ModeStatus::IsMeasureDMode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	

    ModeStatus ms;
    int pwImg = ms.GetPwCurImg();

    if (((mode == ScanMode::PW) || (mode == ScanMode::CW) || (mode == ScanMode::PWCFM) || (mode == ScanMode::PWPDI)|| (mode == ScanMode::CWCFM) || (mode == ScanMode::CWPDI))
            && (pwImg == 2))
        return TRUE;
    else
        return FALSE;
}
bool ModeStatus::IsAllSpectrumImgMode()
{
	ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();	

    if (((mode == ScanMode::PW) || (mode == ScanMode::CW) || (mode == ScanMode::PWCFM) || (mode == ScanMode::PWPDI)|| (mode == ScanMode::CWCFM) || (mode == ScanMode::CWPDI) || (mode == ScanMode::PW_SIMULT) || (mode == ScanMode::PWCFM_SIMULT)
            ||(mode == ScanMode::PWPDI_SIMULT)) 
            )
        return TRUE;
    else
        return FALSE;
}
