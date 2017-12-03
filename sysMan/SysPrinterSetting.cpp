#include <string>
#include "SysPrinterSetting.h"
#include "../base/IniFile.h"
#include "../include/Def.h"
#include "ViewPrintSetting.h"

SysPrinterSetting::SysPrinterSetting()
{
    char path[256];
    sprintf(path, "%s/%s", CFG_RES_PATH, SYS_SETTING_FILE);
    ptrIni = new IniFile(path);
}

SysPrinterSetting::~SysPrinterSetting()
{
    if (ptrIni != NULL) {
	delete ptrIni;
	ptrIni = NULL;
    }
}

int SysPrinterSetting::GetCopies(void)
{
    return ptrIni->ReadInt("PrinterSetting", "Copies");
}

int SysPrinterSetting::GetGamma(void)
{
    return ptrIni->ReadInt("PrinterSetting", "Gamma");
}

int SysPrinterSetting::GetBrightness(void)
{
    return ptrIni->ReadInt("PrinterSetting", "Brightness");
}

int SysPrinterSetting::GetMediaSize(void)
{
    return ptrIni->ReadInt("PrinterSetting", "MediaSize");
}

int SysPrinterSetting::GetLandscape(void)
{
	return ptrIni->ReadInt("PrinterSetting", "Landscape");
}

int SysPrinterSetting::GetPageSize(void)
{
	return ptrIni->ReadInt("PrinterSetting", "PageSize");
}

int SysPrinterSetting::GetSonyGamma(void)
{
	return ptrIni->ReadInt("PrinterSetting", "SonyGamma");
}

void SysPrinterSetting::SetCopies(int copies)
{
    ptrIni->WriteInt("PrinterSetting", "Copies", copies);
}

void SysPrinterSetting::SetGamma(int gamma)
{
    ptrIni->WriteInt("PrinterSetting", "Gamma", gamma);
}

void SysPrinterSetting::SetBrightness(int brightness)
{
    ptrIni->WriteInt("PrinterSetting", "Brightness", brightness);
}

void SysPrinterSetting::SetMediaSize(int media_size)
{
    ptrIni->WriteInt("PrinterSetting", "MediaSize", media_size);
}

void SysPrinterSetting::SetLandscape(int landscape_ang)
{
    ptrIni->WriteInt("PrinterSetting", "Landscape", landscape_ang);
}

void SysPrinterSetting::SetPageSize(int pagesize)
{
    ptrIni->WriteInt("PrinterSetting", "PageSize", pagesize);
}

void SysPrinterSetting::SetSonyGamma(int sonygamma)
{
    ptrIni->WriteInt("PrinterSetting", "SonyGamma", sonygamma);
}

void SysPrinterSetting::SyncFile(void)
{
    ptrIni->SyncConfigFile();
}

void SysPrinterSetting::DefaultFactory(void)
{
	if(ViewPrintSetting::GetInstance()->SonyURI()){
		printf("/////////////// 01\n");
		ptrIni->WriteInt("PrinterSetting", "Copies", 1);
		ptrIni->WriteInt("PrinterSetting", "Landscape", 0);
		ptrIni->WriteInt("PrinterSetting", "PageSize", 0);
		ptrIni->WriteInt("PrinterSetting", "SonyGamma", 0);
    }
    else{
        ptrIni->WriteInt("PrinterSetting", "Copies", 1);
		ptrIni->WriteInt("PrinterSetting", "Gamma", 1000);
		ptrIni->WriteInt("PrinterSetting", "Brightness", 100);
		ptrIni->WriteInt("PrinterSetting", "MediaSzie", 0);
    }
}