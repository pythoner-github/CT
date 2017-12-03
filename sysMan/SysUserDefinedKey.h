#ifndef __SYSUSERDEFINEDKEY_H__
#define __SYSUSERDEFINEDKEY_H__

#include "../base/IniFile.h"

class SysUserDefinedKey
{
public:
	
    SysUserDefinedKey();
    ~SysUserDefinedKey();
#if 0  
    void SetFuncGain(int func);
    int GetFuncGain(void);

    void SetFuncValue(int func);
    int GetFuncValue(void);
#endif

    void SetFuncP1(int func);
    int GetFuncP1(void);

    void SetFuncP2(int func);
    int GetFuncP2(void);

    void SetFuncP3(int func);
    int GetFuncP3(void);
    void SyncFile(void);
private:
    IniFile *ptrIni;
};

#endif
