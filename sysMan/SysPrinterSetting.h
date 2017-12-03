#ifndef _SYSPRINTERSETTING_H_
#define _SYSPRINTERSETTING_H_

#include "IniFile.h"

class SysPrinterSetting
{
public:
	
    SysPrinterSetting();
    ~SysPrinterSetting();

    int GetCopies(void);
    int GetGamma(void);
    int GetBrightness(void);
    int GetMediaSize(void);
    int GetLandscape(void);
    int GetPageSize(void);
    int GetSonyGamma(void);

    void SetCopies(int copies);
    void SetGamma(int gamma);
    void SetBrightness(int brightness);
    void SetMediaSize(int media_size);
    void SetLandscape(int landscape_ang);
    void SetPageSize(int pagesize);
    void SetSonyGamma(int sonygamma);

    void SyncFile(void);
    void DefaultFactory(void);
private:
    IniFile *ptrIni;
};


#endif /* _SYSPRINTERSETTING_H_ */
