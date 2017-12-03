#include "../include/Def.h"
#include "SysUserDefinedKey.h"

SysUserDefinedKey::SysUserDefinedKey()
{
    char path[256];
    sprintf(path, "%s/%s", CFG_RES_PATH, SYS_SETTING_FILE);
    ptrIni = new IniFile(path);	
}

SysUserDefinedKey::~SysUserDefinedKey()
{
    if (ptrIni != NULL) {
	delete ptrIni;
	ptrIni = NULL;
    }
}
#if 0
void SysUserDefinedKey::SetFuncGain(int func)
{
    ptrIni->WriteInt("UserDefinedKey", "Gain", func);
}

int SysUserDefinedKey::GetFuncGain(void)
{
    return ptrIni->ReadInt("UserDefinedKey", "Gain");
}

void SysUserDefinedKey::SetFuncValue(int func)
{
    ptrIni->WriteInt("UserDefinedKey", "Value", func);
}

int SysUserDefinedKey::GetFuncValue(void)
{
    return ptrIni->ReadInt("UserDefinedKey", "Value");
}
#endif
void SysUserDefinedKey::SetFuncP1(int func)
{
    ptrIni->WriteInt("UserDefinedKey", "P1", func);
}

int SysUserDefinedKey::GetFuncP1(void)
{
    return ptrIni->ReadInt("UserDefinedKey", "P1");
}

void SysUserDefinedKey::SetFuncP2(int func)
{
    ptrIni->WriteInt("UserDefinedKey", "P2", func);
}

int SysUserDefinedKey::GetFuncP2(void)
{
    return ptrIni->ReadInt("UserDefinedKey", "P2");
}

void SysUserDefinedKey::SetFuncP3(int func)
{
    ptrIni->WriteInt("UserDefinedKey", "P3", func);
}

int SysUserDefinedKey::GetFuncP3(void)
{
    return ptrIni->ReadInt("UserDefinedKey", "P3");
}

void SysUserDefinedKey::SyncFile(void)
{
    ptrIni->SyncConfigFile();
}
