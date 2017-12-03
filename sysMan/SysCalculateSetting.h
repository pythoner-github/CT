#ifndef __SYSCALCULATESETTING_H__
#define __SYSCALCULATESETTING_H__

#include "../base/IniFile.h"

struct OBSetting{
    int cer;
    int hl;
    int bpd;
    int fl;
    int crl;
    int gs;
    int ac;
    int hc;
    int tad;
    int apad;
    int thd;
    int ofd;
    int efw;
};
const int GetGWMethod(const int item);
const int GetCurEfwMethod(void);

extern OBSetting g_obSetting;

class SysCalculateSetting
{
public:
	
    SysCalculateSetting();
    ~SysCalculateSetting();

    int GetBSAMethod(void);
    int GetCerMethod(void);
    int GetHlMethod(void);
    int GetBpdMethod(void);
    int GetFlMethod(void);
    int GetCrlMethod(void);
    int GetGsMethod(void);
    int GetAcMethod(void);
    int GetHcMethod(void);
    int GetTadMethod(void);
    int GetApadMethod(void);
    int GetThdMethod(void);
    int GetOfdMethod(void);
    int GetEfwMethod(void);
    void UpdateOBSetting(void);

    void SetBSAMethod(int BSA);
    // @param cer 0-goldstein 1-custom
    void SetCerMethod(int cer);
    // @param hl 0-jeanty 1-custom
    void SetHlMethod(int hl);
    // @param bpd 0-hadlock 1-merz 2-lasser 3-rempen 4-tokyo 5-custom
    void SetBpdMethod(int bpd);
    // @param fl 0-hadlock 1-merz 2-jeanty 3-tokyo 4-custom
    void SetFlMethod(int fl);
    // @param crl 0-hadlock 2-robinson 3-hansmenn 4-lasser 5-tokyo 6-custom
    void SetCrlMethod(int crl);
    // @param gs 0-hellman 1-rempen 2-tokyo 3-custom
    void SetGsMethod(int gs);
    // @param ac 0-hadlock 1-merz 2-lasser 3-tokyo 4-custom
    void SetAcMethod(int ac);
    // @param hc 0-hadlock 1-merz 2-lasser 3-custom
    void SetHcMethod(int hc);
    // @param tad 0-merz 1-custom
    void SetTadMethod(int tad);
    // @param apad 0-merz 1-custom
    void SetApadMethod(int apad);
    // @param thd 0-hansmenn 1-custom
    void SetThdMethod(int thd);
    // @param ofd 0-korean 1-custom
    void SetOfdMethod(int ofd);
    // @param efw 0-hadlock1 2-hadlock2 3-hadlock3 4-hadlock4 5-shepard 6hansmenn 7-tokyo
    void SetEfwMethod(int efw);
    void SyncFile(void);
    void DefaultFactory(void);
private:
    IniFile *ptrIni;
};

#endif
