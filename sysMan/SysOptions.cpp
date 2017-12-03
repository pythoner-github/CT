#include "../include/Def.h"
#include "SysOptions.h"

SysOptions::SysOptions()
{
    char path[256];
    sprintf(path, "%s/%s", CFG_RES_PATH, SYS_SETTING_FILE);
    ptrIni = new IniFile(path);
}

SysOptions::~SysOptions()
{
    if (ptrIni != NULL) {
	delete ptrIni;
	ptrIni = NULL;
    }
}

int SysOptions::GetBiopsyLine(void)
{
    return ptrIni->ReadInt("Options", "BiopsyLine");
}

int SysOptions::GetCenterLine(void)
{
    return ptrIni->ReadInt("Options", "CenterLine");
}

int SysOptions::GetTI(void)
{
    return ptrIni->ReadInt("Options", "TI");
}

int SysOptions::GetKeyboardSound(void)
{
    return ptrIni->ReadInt("Options", "KeyboardSound");
}

int SysOptions::GetImageFormat(void)
{
    return ptrIni->ReadInt("Options", "ImageFormat");
}

int SysOptions::GetDisplayFormatM(void)
{
    return ptrIni->ReadInt("Options", "DisplayFormatM");
}

int SysOptions::GetDisplayFormatPW(void)
{
    return ptrIni->ReadInt("Options", "DisplayFormatPW");
}

int SysOptions::GetImageMedia(void)
{
    return ptrIni->ReadInt("Options", "ImageMedia");
}

int SysOptions::GetImageAutoName(void)
{
    return ptrIni->ReadInt("Options", "ImageAutoName");
}

int SysOptions::GetCineFormat(void)
{
    return ptrIni->ReadInt("Options", "CineFormat");
}

int SysOptions::GetCineMedia(void)
{
    return ptrIni->ReadInt("Options", "CineMedia");
}

int SysOptions::GetCineAutoName(void)
{
    return ptrIni->ReadInt("Options", "CineAutoName");
}

int SysOptions::GetMouseSpeed(void)
{
    return ptrIni->ReadInt("Options", "MouseSpeed");
}

void SysOptions::SetBiopsyLine(int biopsyLine)
{
    ptrIni->WriteInt("Options", "BiopsyLine", biopsyLine);
}

void SysOptions::SetCenterLine(int centerLine)
{
    ptrIni->WriteInt("Options", "CenterLine", centerLine);
}

void SysOptions::SetTI(int TI)
{
    ptrIni->WriteInt("Options", "TI", TI);
}

void SysOptions::SetKeyboardSound(int keyboardSound)
{
    ptrIni->WriteInt("Options", "KeyboardSound", keyboardSound);
}

void SysOptions::SetImageFormat(int imageFormat)
{
    ptrIni->WriteInt("Options", "ImageFormat", imageFormat);
}

void SysOptions::SetDisplayFormatM(int displayFormat)
{
    ptrIni->WriteInt("Options", "DisplayFormatM", displayFormat);
}

void SysOptions::SetDisplayFormatPW(int displayFormat)
{
    ptrIni->WriteInt("Options", "DisplayFormatPW", displayFormat);
}

void SysOptions::SetImageMedia(int imageMedia)
{
    ptrIni->WriteInt("Options", "ImageMedia", imageMedia);
}

void SysOptions::SetImageAutoName(int imageAutoName)
{
    ptrIni->WriteInt("Options", "ImageAutoName", imageAutoName);
}

void SysOptions::SetCineFormat(int cineFormat)
{
    ptrIni->WriteInt("Options", "CineFormat", cineFormat);
}

void SysOptions::SetCineMedia(int cineMedia)
{
    ptrIni->WriteInt("Options", "CineMedia", cineMedia);
}

void SysOptions::SetCineAutoName(int cineAutoName)
{
    ptrIni->WriteInt("Options", "CineAutoName", cineAutoName);
}

void SysOptions::SetMouseSpeed(int mouseSpeed)
{
    ptrIni->WriteInt("Options", "MouseSpeed", mouseSpeed);
}

void SysOptions::SyncFile(void)
{
    ptrIni->SyncConfigFile();
}

void SysOptions::DefaultFactory(void)
{
    int biopsyLine=1;
    int centerLine=1;
    int TI=2;
    int keyboardSound=0;
    int imageFormat=0;
    int imageMedia=0;
    int imageAutoName=1;
    int cineFormat=0;
    int cineMedia=0;
    int cineAutoName=1;
    int displayFormat=1;
	int mouseSpeed = 1;

    ptrIni->WriteInt("Options", "BiopsyLine", biopsyLine);
    ptrIni->WriteInt("Options", "CenterLine", centerLine);
    ptrIni->WriteInt("Options", "TI", TI);
    ptrIni->WriteInt("Options", "KeyboardSound", keyboardSound);
    ptrIni->WriteInt("Options", "ImageFormat", imageFormat);
    ptrIni->WriteInt("Options", "ImageMedia", imageMedia);
    ptrIni->WriteInt("Options", "ImageAutoName", imageAutoName);
    ptrIni->WriteInt("Options", "CineFormat", cineFormat);
    ptrIni->WriteInt("Options", "CineMedia", cineMedia);
    ptrIni->WriteInt("Options", "CineAutoName", cineAutoName);
    ptrIni->WriteInt("Options", "DisplayFormatPW", displayFormat);
    displayFormat=4;
    ptrIni->WriteInt("Options", "DisplayFormatM", displayFormat);
    ptrIni->WriteInt("Options", "MouseSpeed", mouseSpeed);

}