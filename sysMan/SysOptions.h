#ifndef __SYSOPTIONS_H__
#define __SYSOPTIONS_H__

#include "../base/IniFile.h"

class SysOptions
{
public:
	
    SysOptions();
    ~SysOptions();

    // @param get biopsyLine from gui and save it  0-on 1-off
    int GetBiopsyLine(void);
    //param 0-on 1-off
    int GetCenterLine(void);
    // param 0-TIC 1-TIB 2-TIS
    int GetTI(void);
    // param 0-on 1-off
    int GetKeyboardSound(void);
    // param 0-bmp 1-jpeg 2-emp
    int GetImageFormat(void); 
    // param 0-harddisk 1-udisk
    int GetDisplayFormatM(void);
    // 0-total 1-ud_11 2-ud_21 3-ud_12 4-lr_11
    int GetDisplayFormatPW(void);
    // 0-total 1-ud_11 2-ud_21 3-ud_12 4-lr_11
    int GetImageMedia(void);
    // param 0-manual 1-auto
    int GetImageAutoName(void);
    //param 0-avi 1-cine
    int GetCineFormat(void);
    // param 0-harddisk 1-udisk
    int GetCineMedia(void);
    // param 0-manual 1-auto
    int GetCineAutoName(void);
	// speed: 0-high 1-middle 2-low
	int GetMouseSpeed(void);

    void SetBiopsyLine(int biopsyLine);
    void SetCenterLine(int centerLine);
    void SetTI(int TI);
    void SetKeyboardSound(int keyboardSound);
    void SetImageFormat(int imageFormat); 
    void SetDisplayFormatM(int dispalyFormat);
    void SetDisplayFormatPW(int dispalyFormat);
    // 0-total 1-ud_11 2-ud_21 3-ud_12 4-lr_11
    void SetImageMedia(int imageMedia);
    void SetImageAutoName(int imageAutoName);
    void SetCineFormat(int cineFormat);
    void SetCineMedia(int cineMedia);
    void SetCineAutoName(int cineAutoName);
	void SetMouseSpeed(int mouseSpeed);
    void SyncFile(void);
    void DefaultFactory(void);
private:
    IniFile *ptrIni;
};

#endif
