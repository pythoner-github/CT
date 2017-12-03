
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include "Init.h"
#include "Def.h"
#include "SysLog.h"
#include "VersionConfig.h"
#include "Img2D.h"
#include "Format2D.h"
#include "ScanMode.h"
#include "Replay.h"
#include "ProbeMan.h"
#include "ProbeSocket.h"
#include "FreezeMode.h"
#include "ExamItem.h"
#include <gtk/gtk.h>
#include "gui_global.h"
#include "gui_func.h"
#include "ViewMain.h"
#include "ViewReport.h"
#include "ImageArea.h"
#include "ProbeSelect.h"
#include "ImgProc2D.h"
#include <stdlib.h>
#include "Database.h"
#include "MeasureMan.h"
#include "MeaCalcFun.h"
#include "UserSelect.h"
#include "DCMMan.h"
#include "PeripheralMan.h"
#include "./sysMan/UpgradeMan.h"
#include "ScreenSaver.h"
#include "./sysMan/SysGeneralSetting.h"
#include "SysCalculateSetting.h"
#include "SysMeasurementSetting.h"
#include "../sysMan/SysOptions.h"
#include "Authorize.h"
#include "ViewSystem.h"
#include "gui_func.h"
#include "FpgaGeneral.h"
#include "DCMRegister.h"
#include  "UsbControl.h"

pthread_t pidInitTable;
gboolean ReportInit = FALSE;
bool g_authorizationOn = false;
bool g_init = true;
bool g_calcPwStatus = false; // false: 未进行包络计算； true: 包络计算完成

//for test
using std::endl;

Init::Init()
{

}

void Init::SystemInit(int argc, char *argv[])
{
    // keyboard control
    KeyboardOversee((void *)&g_keyInterface);

	/*if(g_authorizationOn)
	{
		CAuthorize::GetInstance()->Excute(argc, argv);
	}*/

	KeyboardRequestTGC();

	// keyboard sound
    SysOptions *sysOpt = new SysOptions;
    int keySound = sysOpt->GetKeyboardSound();
    if (keySound)
        KeyboardSound(false);
    else
        KeyboardSound(true);

	int mouseSpeed = sysOpt->GetMouseSpeed();
	g_keyInterface.SetMouseSpeed(mouseSpeed);
    delete sysOpt;

	///> GUI i18n
	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	bind_textdomain_codeset(PACKAGE, "UTF-8");
	textdomain(PACKAGE);

	SysGeneralSetting *sysGeneral = new SysGeneralSetting;
	int index_lang = sysGeneral->GetLanguage();
	int index_video_mode = sysGeneral->GetVideoMode();
	int tvout_format = sysGeneral->GetVideoFormat();

    //printf ("-----index_lang: %d\n", index_lang);
	delete sysGeneral;
    char str_cmd[256];
	if (ZH == index_lang)
	{
	    setenv("LANG", "zh_CN.UTF-8", 1);
	    setenv("LANGUAGE", "zh_CN:zh", 1);
        sprintf(str_cmd, "setxkbmap -layout %s", "us");
        _system_(str_cmd);
        //system("setxkbmap -layout us");
	}
    else if (RU == index_lang)
    {
        setenv("LANG", "ru_RU.UTF-8", 1);
	    setenv("LANGUAGE", "ru_RU:ru", 1);
        //system("setxkbmap -layout ru");
        
        sprintf(str_cmd, "setxkbmap -layout %s", "ru");
        _system_(str_cmd);
    }
    else if (PL == index_lang)
    {
        setenv("LANG", "pl_PL.UTF-8", 1);
	    setenv("LANGUAGE", "pl_PL:pl", 1);
        //system("setxkbmap -layout pl qwertz");
        sprintf(str_cmd, "setxkbmap -layout %s", "pl qwertz");
        _system_(str_cmd);
    }
    else if (ES == index_lang)
    {
        setenv("LANG", "es_ES.UTF-8", 1);
        setenv("LANGUAGE", "es_ES:es", 1);
        //system("setxkbmap -layout es");
        sprintf(str_cmd, "setxkbmap -layout %s", "es");
        _system_(str_cmd);
    }

    else if (FR == index_lang)
    {
        setenv("LANG", "fr_FR.UTF-8", 1);
	    setenv("LANGUAGE", "fr_FR:fr", 1);
        //system("setxkbmap -layout fr");
        sprintf(str_cmd, "setxkbmap -layout %s", "fr");
        _system_(str_cmd);
    }
    else if (DE == index_lang)
    {
        setenv("LANG", "de_DE.UTF-8", 1);
	    setenv("LANGUAGE", "de_DE:de", 1);
        //system("setxkbmap -layout de");
        sprintf(str_cmd, "setxkbmap -layout %s", "de");
        _system_(str_cmd);
    }
	else
	{
	    setenv("LANG", "en_US.UTF-8", 1);
	    setenv("LANGUAGE", "en_US:en", 1);
        //system("setxkbmap -layout us");
        sprintf(str_cmd, "setxkbmap -layout %s", "us");
        _system_(str_cmd);
	}
   // setlocale(LC_NUMERIC, "en_US.UTF-8");

#if 0
#if not defined (EMP3D)
	string command;
	if (tvout_format == 0)
		command = "--mode 1024x768 --pos 0x0";
	else if (tvout_format == 1)
		command = "--mode 848x660 --pos 180x0";
	else
		command = "--mode 848x560 --pos 180x100";

	if (index_video_mode) {
		//command = "xrandr --output TV --set TV_FORMAT NTSC-M " + command;
		command = "xrandr --output TV1 --set mode NTSC-M " + command;
	} else {
		//command = "xrandr --output TV --set TV_FORMAT PAL " + command;
		command = "xrandr --output TV1 --set mode PAL " + command;
	}
	_system_(command.c_str());
#endif
#endif

	SysCalculateSetting *sysCalc = new SysCalculateSetting;
	SysMeasurementSetting *sysMeasure = new SysMeasurementSetting;
	sysCalc->UpdateOBSetting();
	sysMeasure->UpdateTraceItemSetting();
	delete sysCalc;
	delete sysMeasure;

	// Database
	Database db;
	if (!db.CreateTable()) {
	    return ;
	}
	db.OpenDB();

	///> upgrade manage
	UpgradeMan::GetInstance()->SetRebootState("1");
#if (defined (EMP_430) || defined(EMP_355))
    //5805 AD control para
    CFpgaGeneral::GetInstance()->AD5805ControlInit();
#else
    //9273 AD control para
    CFpgaGeneral::GetInstance()->AD9273ControlInit();
#endif

}

void* ThreadReportTable(void* argv)
{
	PRINTF("Enter thread to init report table data!\n");
	ViewReport::GetInstance()->InitTableData();
	PRINTF("Leave thread to init report table data!\n");
	ReportInit = TRUE;
	return NULL;
}

void Init::ParaInit()
{
	// replay
	Replay::GetInstance()->Init();

	// Report Table
	if( pthread_create(&pidInitTable, NULL, ThreadReportTable, NULL) != 0)
	{
		PRINTF("Failed to create report table init thread!\n");
	}

	// Measure
	MeaCalcFun::InitCustomTable();
	MeasureMan::GetInstance()->ClearAllValue();

	//clear record
	g_patientInfo.ClearRecord();

	///> watch Volume device
	PRINTF("begin add watch for peripheral device\n");
	PeripheralMan::GetInstance()->AddWatch();
	PeripheralMan::GetInstance()->SwitchPrinterDriver();

	///> watch network cable
	PRINTF("begin add watch network cable\n");
	PeripheralMan::GetInstance()->AddWatchNetCable();

	///> screen saver
	PRINTF("begin screen saver timer\n");
	SysGeneralSetting sgs;
	ScreenSaver::GetInstance()->SetPeriod(sgs.GetScreenProtect()*60);
	
	//ProbeMan::GetInstance()->WriteProbeManual();
	//ExamItem item;
	//item.GenerateDefaultImgOptimize();

	//creat dicom manual object
	CDCMMan::Create(DCMDB_DIR);
    CDCMRegister::Create(DCMRES_DIR);
}

void Init::ProbeCheck()
{
	FreezeMode* ptrFreeze = FreezeMode::GetInstance();
	ptrFreeze->Freeze(); 
   
    //send 0 to fpga 
    INT32U addr;
    INT32U temp;
    addr = 55;
    temp = 0x0;
    EzUsb::GetInstance()->WriteOneDataToFpga(addr, temp);
    ProbeSelect ps;
    ps.ProbeRead();

    //select user
    char path[256];
    sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini(path);
    ExamItem exam;
    const char *userName;
    userName = exam.ReadDefaultUserSelect(&ini).c_str();

   if ((strcmp(userName, "System Default") != 0) && (strcmp(userName, "Умолчан системы") != 0) && 
            (strcmp(userName, "系统默认") != 0) && (strcmp(userName, "Domyślne Systemu") != 0)  &&
            (strcmp(userName, "Par défaut du sys.") != 0) && (strcmp(userName, "Systemvorgabe") != 0) && (strcmp(userName, "Sistema por defecto") !=0))
   {
       sprintf(user_configure, "%s%s%s", "userconfig/",userName, ".ini");
   }
   else
   {
#ifdef VET
       sprintf(user_configure, "%s", "VetItemPara.ini");

#else
        sprintf(user_configure, "%s", "ItemPara.ini");
#endif
    }

    string itemName;
    bool flag = false;
    ps.GetDefaultItemNameandFlag(itemName, flag);
    //printf("====================Probe %d is exist indexItem=%d\n", (indexSocket), indexItem);
    int indexSocket = 0; ///< start from 0
    ExamItem::EItem indexItem; ///< start form 0
    ps.GetDefaultValue(indexSocket, indexItem, flag);

    // probe init
    if(flag)
        ps.UserItemOfProbeInit(indexSocket, indexItem, itemName.c_str());
    else
        ps.ProbeInit(indexSocket, indexItem);

    //退出刚开机状态
    g_init = false;

    // enable emit 
    // doing in the ProbeInit func
    //Img2D::GetInstance()->EnableEmit();

    ///> tell usb begin read image	
    IoCtrl io;
    io.BeginReadImage();	
}

void Init::WriteLogHead()
{
    class SysLog* sysLog = SysLog::Instance();

    class VersionConfig* version = VersionConfig::Instance();

    ///> get work-mode of software
    version->Init();
    if (version->GetProduct() == VersionConfig::TROLLEY)
        (*sysLog) << "software work in trolley mode." << endl;
    else if (version->GetProduct() == VersionConfig::PORTABLE)
        (*sysLog) << "software work in portable mode." << endl;

	///> get realse configure version of software
	if (version->GetRealse() == VersionConfig::EMP_CHI)
		(*sysLog) << "software is config to chinese language of emperor." << endl;
	else if (version->GetRealse() == VersionConfig::EMP_EN)
		(*sysLog) << "software is config to english language of emperor." << endl;
}

void Init::ExitSystem()
{
	//destory dicom manual object
	CDCMMan::Destroy();
    CDCMRegister::Destroy();

	// remove device info
	PeripheralMan::GetInstance()->RemoveWatch();

	// destroy global color
	free_init_colors();
#ifdef EMP_3410
    CManRegister::GetInstance()->Destroy();
#endif
}
///> private
void Init::CreatMainWindow()
{
    ViewMain::GetInstance()->Create();
    // ViewMeasureResult::GetInstance()->Create();
    // ViewMeasureResult::GetInstance()->Show();
}


