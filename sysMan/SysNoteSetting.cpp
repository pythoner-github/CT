#include "../include/Def.h"
#include "SysNoteSetting.h"

SysNoteSetting::SysNoteSetting()
{
    char path[256];
    sprintf(path, "%s/%s", CFG_RES_PATH, SYS_SETTING_FILE);
    ptrIni = new IniFile(path);
}

SysNoteSetting::~SysNoteSetting()
{
    if (ptrIni != NULL) {
	delete ptrIni;
	ptrIni = NULL;
    }
}

int SysNoteSetting::GetFontSize(void)
{
    return ptrIni->ReadInt("NoteSetting", "FontSize");
}

int SysNoteSetting::GetBodyMarkSize(void)
{
    return ptrIni->ReadInt("NoteSetting", "BodyMarkSize");
}

int SysNoteSetting::GetFontColor(void)
{
    return ptrIni->ReadInt("NoteSetting", "FontColor");
}

int SysNoteSetting::GetBodyMarkColor(void)
{
    return ptrIni->ReadInt("NoteSetting", "BodyMarkColor");
}

void SysNoteSetting::SetFontSize(int fontSize)
{
    ptrIni->WriteInt("NoteSetting", "FontSize", fontSize);
}

void SysNoteSetting::SetBodyMarkSize(int bodyMarkSize)
{
    ptrIni->WriteInt("NoteSetting", "BodyMarkSize", bodyMarkSize);
}

void SysNoteSetting::SetFontColor(int fontColor)
{
    ptrIni->WriteInt("NoteSetting", "FontColor", fontColor);
}

void SysNoteSetting::SetBodyMarkColor(int bodyMarkColor)
{
    ptrIni->WriteInt("NoteSetting", "BodyMarkColor", bodyMarkColor);
}

void SysNoteSetting::SyncFile(void)
{
    ptrIni->SyncConfigFile();
}

void SysNoteSetting::DefaultFactory(void)
{
    int fontSize = 1;
    int bodyMarkSize = 1;
    int fontColor = 1;
    int bodyMarkColor = 1;

    ptrIni->WriteInt("NoteSetting", "FontSize", fontSize);
    ptrIni->WriteInt("NoteSetting", "BodyMarkSize", bodyMarkSize);
    ptrIni->WriteInt("NoteSetting", "FontColor", fontColor);
    ptrIni->WriteInt("NoteSetting", "BodyMarkColor", bodyMarkColor);
}



