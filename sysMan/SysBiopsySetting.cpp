#include "SysBiopsySetting.h"
#include "../base/IniFile.h"
#include "../include/Def.h"

SysBiopsySetting::SysBiopsySetting()
{
    char path[256];
    sprintf(path, "%s/%s", CFG_RES_PATH, SYS_SETTING_FILE);
    ptrIni = new IniFile(path);
}

SysBiopsySetting::~SysBiopsySetting()
{
    if (ptrIni != NULL) {
	delete ptrIni;
	ptrIni = NULL;
    }
}
#if 1
int SysBiopsySetting::GetBiopsyX_R50(void)
{
#if 0
    int tempx = 0;
    tempx = ptrIni->ReadInt("BiopsySetting", "BiopsyX_R50");
#endif
    return ptrIni->ReadInt("BiopsySetting", "BiopsyX_R50");
}
int SysBiopsySetting::GetBiopsyAngle_R50(void)
{
#if 0
    printf("read the R50 Angle\n");
    int temp1(0), temp2(0);
    temp1 = ptrIni->ReadInt("BiopsySetting", "BiopsyAngle_T10");
    temp2 = ptrIni->ReadInt("BiopsySetting", "BiopsyAngle");
    printf("read the T10 Angle = %d\n",temp1);
    printf("read the R50 Angle = %d\n",temp2);
    return temp2;
#endif

     return ptrIni->ReadInt("BiopsySetting", "BiopsyAngle");
}
int SysBiopsySetting::GetBiopsyX_T10(void)
{
    return ptrIni->ReadInt("BiopsySetting", "BiopsyX_T10");
}
int SysBiopsySetting::GetBiopsyAngle_T10(void)
{
    return ptrIni->ReadInt("BiopsySetting", "BiopsyAngle_T10");
}

int SysBiopsySetting::GetBiopsyX_L40(void)
{
    return ptrIni->ReadInt("BiopsySetting", "BiopsyX_L40");
}
int SysBiopsySetting::GetBiopsyAngle_L40(void)
{
    return ptrIni->ReadInt("BiopsySetting", "BiopsyAngle_L40");
}
#endif

int SysBiopsySetting::GetBiopsyX_R40(void)
{
    return ptrIni->ReadInt("BiopsySetting", "BiopsyX_R40");
}
int SysBiopsySetting::GetBiopsyY_R40(void)
{
    return ptrIni->ReadInt("BiopsySetting", "BiopsyY_R40");
}

int SysBiopsySetting::GetBiopsyX_R20(void)
{
    return ptrIni->ReadInt("BiopsySetting", "BiopsyX_R20");
}
int SysBiopsySetting::GetBiopsyY_R20(void)
{
    return ptrIni->ReadInt("BiopsySetting", "BiopsyY_R20");
}

int SysBiopsySetting::GetBiopsyX_T13(void)
{
    return ptrIni->ReadInt("BiopsySetting", "BiopsyX_T13");
}
int SysBiopsySetting::GetBiopsyY_T13(void)
{
    return ptrIni->ReadInt("BiopsySetting", "BiopsyY_T13");
}
/*
int SysBiopsySetting::GetBiopsyX_L40(void)
{
    return ptrIni->ReadInt("BiopsySetting", "BiopsyX_L40");
}
int SysBiopsySetting::GetBiopsyY_L40(void)
{
    return ptrIni->ReadInt("BiopsySetting", "BiopsyY_L40");
}
*/

///////////////////////////////////////////////////////////
//set the config file para
//////////////////////////////////////////////////////////
//
void SysBiopsySetting::SetBiopsyX_R50(int BiopsyX)
{
    ptrIni->WriteInt("BiopsySetting", "BiopsyX_R50", BiopsyX);
}
void SysBiopsySetting::SetBiopsyAngle_R50(int Angle)
{
    ptrIni->WriteInt("BiopsySetting", "BiopsyAngle", Angle);
}

void SysBiopsySetting::SetBiopsyX_T10(int BiopsyX)
{
    ptrIni->WriteInt("BiopsySetting", "BiopsyX_T10", BiopsyX);
}
void SysBiopsySetting::SetBiopsyAngle_T10(int Angle)
{
    ptrIni->WriteInt("BiopsySetting", "BiopsyAngle_T10", Angle);
}

void SysBiopsySetting::SetBiopsyX_R40(int BiopsyX)
{
    ptrIni->WriteInt("BiopsySetting", "BiopsyX_R40", BiopsyX);
}
void SysBiopsySetting::SetBiopsyY_R40(int BiopsyY)
{
    ptrIni->WriteInt("BiopsySetting", "BiopsyY_R40", BiopsyY);
}

void SysBiopsySetting::SetBiopsyX_R20(int BiopsyX)
{
    ptrIni->WriteInt("BiopsySetting", "BiopsyX_R20", BiopsyX);
}
void SysBiopsySetting::SetBiopsyY_R20(int BiopsyY)
{
    ptrIni->WriteInt("BiopsySetting", "BiopsyY_R20", BiopsyY);
}

void SysBiopsySetting::SetBiopsyX_T13(int BiopsyX)
{
    ptrIni->WriteInt("BiopsySetting", "BiopsyX_T13", BiopsyX);
}
void SysBiopsySetting::SetBiopsyY_T13(int BiopsyY)
{
    ptrIni->WriteInt("BiopsySetting", "BiopsyY_T13", BiopsyY);
}

void SysBiopsySetting::SetBiopsyX_L40(int BiopsyX)
{
    ptrIni->WriteInt("BiopsySetting", "BiopsyX_L40", BiopsyX);
}
void SysBiopsySetting::SetBiopsyAngle_L40(int Angle)
{
    ptrIni->WriteInt("BiopsySetting", "BiopsyAngle_L40", Angle);
}

void SysBiopsySetting::SyncFile(void)
{
    ptrIni->SyncConfigFile();
}