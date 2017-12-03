#ifndef _SYSBIOPSYSETTING_H_
#define _SYSBIOPSYSETTING_H_

#include "IniFile.h"

class SysBiopsySetting
{
public:
    SysBiopsySetting();
    ~SysBiopsySetting();

    int GetBiopsyX_R50(void);
    int GetBiopsyAngle_R50(void);
    int GetBiopsyX_R40(void);
    int GetBiopsyY_R40(void);
    int GetBiopsyX_R20(void);
    int GetBiopsyY_R20(void);
    int GetBiopsyX_T10(void);
    int GetBiopsyAngle_T10(void);
    int GetBiopsyX_T13(void);
    int GetBiopsyY_T13(void);
    int GetBiopsyX_L40(void);
    //int GetBiopsyY_L40(void);
    int GetBiopsyAngle_L40(void);

    void SetBiopsyX_T10(int BiopsyX);
    void SetBiopsyAngle_T10(int Angle);
    void SetBiopsyX_R50(int BiopsyX);
    void SetBiopsyAngle_R50(int Angle);
    void SetBiopsyX_R40(int BiopsyX);
    void SetBiopsyY_R40(int BiopsyY);
    void SetBiopsyX_R20(int BiopsyX);
    void SetBiopsyY_R20(int BiopsyY);
    void SetBiopsyX_T13(int BiopsyX);
    void SetBiopsyY_T13(int BiopsyY);
    void SetBiopsyX_L40(int BiopsyX);
    // void SetBiopsyY_L40(int BiopsyY);
    void SetBiopsyAngle_L40(int Angle);

    void SyncFile(void);
private:
    IniFile *ptrIni;
};

#endif /* _SYSBIOPSYSETTING_H_ */
