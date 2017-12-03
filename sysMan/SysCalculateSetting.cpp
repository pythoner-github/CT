#include <stdio.h>
#include <string>
#include "../include/Def.h"
#include "MeaCalcFun.h"
#include "SysCalculateSetting.h"

OBSetting g_obSetting;

SysCalculateSetting::SysCalculateSetting()
{
    char path[256];
    sprintf(path, "%s/%s", CFG_RES_PATH, SYS_SETTING_FILE);
    ptrIni = new IniFile(path);
}

SysCalculateSetting::~SysCalculateSetting()
{
    if (ptrIni != NULL) {
	delete ptrIni;
	ptrIni = NULL;
    }
}

const int GetCurEfwMethod(void)
{
    int method = 0;

    method = g_obSetting.efw;
    method += EFW_START;
    return method;
}

const int GetGWMethod(const int item)
{
    int method = 0;

    switch(item) {
    case OB_CER:
	method = g_obSetting.cer;
	method += CER_START - 1;
	break;

    case OB_HL:
	method = g_obSetting.hl;
	method += HL_START - 1;
	break;

    case OB_BPD:
	method = g_obSetting.bpd;
	method += BPD_START - 1;
	break;

    case OB_FL:
	method = g_obSetting.fl;
	method += FL_START - 1;
	break;

    case OB_CRL:
	method = g_obSetting.crl;
	method += CRL_START - 1;
	break;

    case OB_GS:
	method = g_obSetting.gs;
	method += GS_START - 1;
	break;

    case OB_AC:
	method = g_obSetting.ac;
	method += AC_START - 1;
	break;

    case OB_HC:
	method = g_obSetting.hc;
	method += HC_START - 1;
	break;

    case OB_TAD:
	method = g_obSetting.tad;
	method += TAD_START - 1;
	break;

    case OB_APAD:
	method = g_obSetting.apad;
	method += APAD_START - 1;
	break;

    case OB_THD:
	method = g_obSetting.thd;
	method += THD_START - 1;
	break;

    case OB_OFD:
	method = g_obSetting.ofd;
	method += OFD_START - 1;
	break;

    case OB_OOD:
	//	method = GetOodMethod();
	//	method += OOD_START - 1;
	method = OOD_START - 1;
	break;

    case OB_TIBIA:
	//	method = GetTibiaMethod();
	//	method += TIBIA_START - 1;
	method = TIBIA_START - 1;
	break;

    case OB_TTD:
	//	method = GetTtdMethod();
	//	method += TTD_START - 1;
	method = TTD_START - 1;
	break;

    case OB_ULNA:
	//	method = GetUlnaMethod();
	//	method += ULNA_START - 1;
	method = ULNA_START - 1;
	break;
    case OB_FTA:
		method = FTA_START - 1;
	break;


    default:
	break;
    }

    return(method);
}

int SysCalculateSetting::GetBSAMethod(void)
{
    return ptrIni->ReadInt("CalculateSetting", "BSA");
}

int SysCalculateSetting::GetCerMethod(void)
{
    return ptrIni->ReadInt("CalculateSetting", "Cer");
}
int SysCalculateSetting::GetHlMethod(void)
{
    return ptrIni->ReadInt("CalculateSetting", "Hl");
}
int SysCalculateSetting::GetBpdMethod(void)
{
    return ptrIni->ReadInt("CalculateSetting", "Bpd");
}
int SysCalculateSetting::GetFlMethod(void)
{
    return ptrIni->ReadInt("CalculateSetting", "Fl");
}
int SysCalculateSetting::GetCrlMethod(void)
{
    return ptrIni->ReadInt("CalculateSetting", "Crl");
}
int SysCalculateSetting::GetGsMethod(void)
{
    return ptrIni->ReadInt("CalculateSetting", "Gs");
}
int SysCalculateSetting::GetAcMethod(void)
{
    return ptrIni->ReadInt("CalculateSetting", "Ac");
}
int SysCalculateSetting::GetHcMethod(void)
{
    return ptrIni->ReadInt("CalculateSetting", "Hc");
}
int SysCalculateSetting::GetTadMethod(void)
{
    return ptrIni->ReadInt("CalculateSetting", "Tad");
}
int SysCalculateSetting::GetApadMethod(void)
{
    return ptrIni->ReadInt("CalculateSetting", "Apad");
}
int SysCalculateSetting::GetThdMethod(void)
{
    return ptrIni->ReadInt("CalculateSetting", "Thd");
}
int SysCalculateSetting::GetOfdMethod(void)
{
    return ptrIni->ReadInt("CalculateSetting", "Ofd");
}
int SysCalculateSetting::GetEfwMethod(void)
{
    return ptrIni->ReadInt("CalculateSetting", "Efw");
}

void SysCalculateSetting::UpdateOBSetting(void)
{
    g_obSetting.cer = GetCerMethod();
    g_obSetting.hl = GetHlMethod();
    g_obSetting.bpd = GetBpdMethod();
    g_obSetting.fl = GetFlMethod();
    g_obSetting.crl = GetCrlMethod();
    g_obSetting.gs = GetGsMethod();
    g_obSetting.ac = GetAcMethod();
    g_obSetting.hc = GetHcMethod();
    g_obSetting.tad = GetTadMethod();
    g_obSetting.apad = GetApadMethod();
    g_obSetting.thd = GetThdMethod();
    g_obSetting.ofd = GetOfdMethod();
    g_obSetting.efw = GetEfwMethod();
}

void SysCalculateSetting::SetBSAMethod(int BSA)
{
    ptrIni->WriteInt("CalculateSetting", "BSA", BSA);
}

void SysCalculateSetting::SetCerMethod(int cer)
{
    ptrIni->WriteInt("CalculateSetting", "Cer", cer);
}
void SysCalculateSetting::SetHlMethod(int hl)
{
    ptrIni->WriteInt("CalculateSetting", "Hl", hl);
}
void SysCalculateSetting::SetBpdMethod(int bpd)
{
    ptrIni->WriteInt("CalculateSetting", "Bpd", bpd);
}

void SysCalculateSetting::SetFlMethod(int fl)
{
    ptrIni->WriteInt("CalculateSetting", "Fl", fl);
}

void SysCalculateSetting::SetCrlMethod(int crl)
{
    ptrIni->WriteInt("CalculateSetting", "Crl", crl);
}

void SysCalculateSetting::SetGsMethod(int gs)
{
    ptrIni->WriteInt("CalculateSetting", "Gs", gs);
}

void SysCalculateSetting::SetAcMethod(int ac)
{
    ptrIni->WriteInt("CalculateSetting", "Ac", ac);
}

void SysCalculateSetting::SetHcMethod(int hc)
{
    ptrIni->WriteInt("CalculateSetting", "Hc", hc);
}

void SysCalculateSetting::SetTadMethod(int tad)
{
    ptrIni->WriteInt("CalculateSetting", "Tad", tad);
}

void SysCalculateSetting::SetApadMethod(int apad)
{
    ptrIni->WriteInt("CalculateSetting", "Apad", apad);
}

void SysCalculateSetting::SetThdMethod(int thd)
{
    ptrIni->WriteInt("CalculateSetting", "Thd", thd);
}

void SysCalculateSetting::SetOfdMethod(int ofd)
{
    ptrIni->WriteInt("CalculateSetting", "Ofd", ofd);
}

void SysCalculateSetting::SetEfwMethod(int efw)
{
    ptrIni->WriteInt("CalculateSetting", "Efw", efw);
}

void SysCalculateSetting::SyncFile(void)
{
    ptrIni->SyncConfigFile();
}

void SysCalculateSetting::DefaultFactory(void)
{
    int BSA=0;
    int Cer=0;
    int Hl=0;
    int Bpd=0;
    int Fl=0;
    int Crl=0;
    int Gs=0;
    int Ac=0;
    int Hc=0;
    int Tad=0;
    int Apad=0;
    int Thd=0;
    int Ofd=0;
    int Efw=0;

    ptrIni->WriteInt("CalculateSetting", "BSA", BSA);
    ptrIni->WriteInt("CalculateSetting", "Cer", Cer);
    ptrIni->WriteInt("CalculateSetting", "Hl", Hl);
    ptrIni->WriteInt("CalculateSetting", "Bpd", Bpd);
    ptrIni->WriteInt("CalculateSetting", "Fl", Fl);
    ptrIni->WriteInt("CalculateSetting", "Crl", Crl);
    ptrIni->WriteInt("CalculateSetting", "Gs", Gs);
    ptrIni->WriteInt("CalculateSetting", "Ac", Ac);
    ptrIni->WriteInt("CalculateSetting", "Hc", Hc);
    ptrIni->WriteInt("CalculateSetting", "Tad", Tad);
    ptrIni->WriteInt("CalculateSetting", "Apad", Apad);
    ptrIni->WriteInt("CalculateSetting", "Thd", Thd);
    ptrIni->WriteInt("CalculateSetting", "Ofd", Ofd);
    ptrIni->WriteInt("CalculateSetting", "Efw", Efw);
}