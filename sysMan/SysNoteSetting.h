#ifndef __SYSNOTESETTING_H__
#define __SYSNOTESETTING_H__

#include "../base/IniFile.h"

class SysNoteSetting
{
public:
	
    SysNoteSetting();
    ~SysNoteSetting();

    // param fontSize: 0-big 1-normal 2-small 
    int GetFontSize(void);
    void SetFontSize(int fontSize);

    // param bodyMark: 0-big 1-mid 2-small
    int GetBodyMarkSize(void);
    void SetBodyMarkSize(int bodyMark);

    // param fontColor: 
    int GetFontColor(void);
    void SetFontColor(int fontColor);
    
    // param bodyMarkColor: 0-white, 1-gray, 2-red, 3-green, 4-yellow, 5-blue
    int GetBodyMarkColor(void);
    void SetBodyMarkColor(int bodyMarkColor);
    void SyncFile(void);
    void DefaultFactory(void);

private:
    IniFile *ptrIni;
};

#endif


