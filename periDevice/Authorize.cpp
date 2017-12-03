#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "hasp_api.h"
#include "Authorize.h"
#include "gui_func.h"
#include "./sysMan/SysGeneralSetting.h"
#include "LicenseDialog.h"
#include "gui_global.h"
#include "./sysMan/ViewSystem.h"
//using namespace std;
/*
unsigned char vendor_code[] =
"UcsDtBGaBxZSDHTqDUC3Vbhc/+sHrWrq6m433ljWIKoKyUIml8AUWOkRFoU7xmobWQ3YVI6N3tiXeMot"
"dBah3Vdicyv1EGD4543IWePjjspsurMdyTtc+Jb9EXBnL4WfevTYdsez+giHxKvt+hw79/PgFYlTWogp"
"ziX0iicsM1QuC8CxmF6BXS3RpL+ulgoKO6Go5ph/OceGW2uvoWZYBifffMj2UbYtZrXAzAVHDyFBoUyE"
"wIQyz1lBzsyuxUfgqirYhgfDBT5yDlyw/Nv9at3fMikbuVasrMdHgDv9Z2mV4uyklFZT9NL7rBVMvlmx"
"+BuKeR+mMZiczbM/i6m6/AO1nCXhfSjkQwrctusH8CDzrogR0Y1eBNwjalrYhI7ZRerhHdsV8xmjN7Su"
"2cIO29E8CxDs1xdTOT991ploiqvacc8fz68XDk0ZV0xAOGEFjjxWFlMQoBPDHA2Q8jwUAiNneuK8KBdx"
"Ta64fuum2YudiPx0Ldfpcfz3a0XGLPnq3EBwwJQTteESBpJ2FKJTyDmlAA8kX1pDC+lCNYPOufZO+PlX"
"NM1GVet6zd9dweWtv3WZBR4LFpgJbjCk2LvjN2Pu+VAYYaYgUl1y32naTgpl9w72i+C+tyNgrG2ZRPAi"
"fzkoUSdjNC4u8WeYxmwDlNzVoLoJjE+IlgO7MDSsZjUZKrmIKhbDQhpuetRqyX7PGrRA+LZr6hw9KbCN"
"fSond5cfF7KxMeg/eVZ6yUDnt6ol+RUDCn72PFJtO4NXfVoqiMK5dAwoCybmNhxgqY1m9pmYI5n3pGks"
"PQr8QjTyU1UtGUOVd3tEOWwM+nEcqeynBg5janQj9bl7ja4B6PXon5QfbsBzUdb7Bw/26l8wq/kJqOP0"
"2UzRzjXBBs6fK18e2aTogTFsh/xr05mBmDUxRE2GG+iKtNGbCDoUNwE30cA=";*/
unsigned char vendor_code[] =
"EZacqHdXLIPt9TYLoDwoGaJu3ROzGSi/Y+yTT7zVmjqQKnq2GWaaArzAQliYwuRLz2hosWZyCIRFDOUg"
"5JcwMquKbdK1DxxbnhFIp75cwWFvcz4wVo6y5DM4BA8dLoB6IbvrZzGfohatkxCrDh9H7T8aIZWpTrDK"
"nIiDWlSEjXg6Vpswr2M1qUKZpony+SeRWnypFJfd1zhL7NwKVXQUYC18SE0lQOaDftxoAEVN2z2SzZ/u"
"GgCzqrsIAn0ejyUD40lrE/Tp/+qFyJHxte63749L+AUslRFr726BfT318Zx/iPOAyDHMuzICoHeiReEP"
"Tx7Mi4vLQUNp61n1EYaHIAkoWYWH5cXdXAehjNFsohDasJ9BWbwODRPOf51rPrwaeKXt9hWvSiBuErT9"
"P45TFGAjR4LZAYHQP7zgsWzkEFWoBlUpjlXS+9uLCOXSl1e7f2Ea9z23vhtUk/xyGeGoU//IG6O29qcG"
"fSFLU50BBapwCmt2MjqkXzbNEO7FPDkvaKPtzcyWQnCar8Z183D1zLQEeadUi9CT2jl2dOMZ2safhm4F"
"Rs0CGZDJ2YuTWzUNxc+0YZxh4HpWuK8GZRlpGUT17TzpUydvgR9tB4s33KL5i+OxitO5WLbk+fvrdw96"
"sS/HlE7Wo4epTeb5TMlzJcoA2+4LyGKiq0v3dD+CubyjpQXlO7YQbIFMVRZ8LC/IWdMM45obiJ9SRAqt"
"s4m42hF6HibDHExWYq0gk8NPNGm6PhEkFVFUvsJ/vbakGEueCU8OpPriGcrurWpj5u7liochhRfxA8GQ"
"3Wtw6Akiu6xqq4a33R0iDtw4B84yhmT/JqMLp2wjcDB/DTcdk2mdVpSU0jdiTBPZRp38xpNYuyBg4Spl"
"sDJGmpIK2BVy8ICilZrWzlgX+nydz3Lq4uUdS6ww74azVksmdstM0J/BV00=";

#define SCOPE_VIEW_MAXSIZE (1024 * 128)
CAuthorize *m_ptrInstance = NULL;

CAuthorize *CAuthorize::GetInstance()
{
	if (m_ptrInstance == NULL)
		m_ptrInstance = new CAuthorize;

	return m_ptrInstance;
}

CAuthorize::CAuthorize() : m_timeoutIdUkey(0)
{
}

CAuthorize::~CAuthorize() 
{
}

void CAuthorize::Excute(int argc, char *argv[])
{
	SysGeneralSetting sysGS;
    int index_fid = sysGS.GetFID();
	//int status = CheckAuthorize(FEATURE);
    int status = CheckAuthorize(index_fid);
	PRINTF("status=%d\n", status);
    if (status < 0)
    {
        setlocale(LC_ALL, "");
        bindtextdomain(PACKAGE, LOCALEDIR);
        bind_textdomain_codeset(PACKAGE, "UTF-8");
		textdomain(PACKAGE);

		SysGeneralSetting *sysGeneral = new SysGeneralSetting;
		int index_lang = sysGeneral->GetLanguage();
		delete sysGeneral;

		if (ZH == index_lang)
		{
			setenv("LANG", "zh_CN.UTF-8", 1);
			setenv("LANGUAGE", "zh_CN:zh", 1);
		}
		else if (RU == index_lang)
		{
			setenv("LANG", "ru_RU.UTF-8", 1);
			setenv("LANGUAGE", "ru_RU:ru", 1);
		}
		else if (PL == index_lang)
		{
			setenv("LANG", "pl_PL.UTF-8", 1);
			setenv("LANGUAGE", "pl_PL:pl", 1);
		}
        else if (ES == index_lang)
		{
			setenv("LANG", "es_ES.UTF-8", 1);
			setenv("LANGUAGE", "es_ES:ES", 1);
		}
	
		else if (FR == index_lang)
		{
			setenv("LANG", "fr_FR.UTF-8", 1);
			setenv("LANGUAGE", "fr_FR:fr", 1);
        }
		else if (DE == index_lang)
		{
			setenv("LANG", "de_DE.UTF-8", 1);
			setenv("LANGUAGE", "de_DE:de", 1);
		}
		else
		{
			setenv("LANG", "en_US.UTF-8", 1);
			setenv("LANGUAGE", "en_US:en", 1);
		}

		if(!g_thread_supported())
			g_thread_init(NULL);
		gdk_threads_init();
		gtk_init(&argc, &argv);

		SetTheme(RC_PATH);
		if (status == -1)
			CLicenseDialog::GetInstance()->Create(CLicenseDialog::NODEVICE, true);
		else if (status == -2)
			CLicenseDialog::GetInstance()->Create(CLicenseDialog::EXPIRE, true);
		else
			CLicenseDialog::GetInstance()->Create(CLicenseDialog::NOAUTHORIZE, true);

        gdk_threads_enter();
        gtk_main();
        gdk_threads_leave();
    }

	AddTimeout();

}

void CAuthorize::AddTimeout()
{
	if (!m_timeoutIdUkey)
		m_timeoutIdUkey = g_timeout_add(600000, CallbackUKey, this);
}

gboolean CAuthorize::CallbackUKey(gpointer data)
{
	CAuthorize *pClass = (CAuthorize *)data;
	
    SysGeneralSetting sysGS;
    int index_fid = sysGS.GetFID();
    
    int status = pClass->CheckAuthorize(index_fid);

    //int status = pClass->CheckAuthorize(FEATURE);
    if (status < 0)
    {
		if (status == -1)
			CLicenseDialog::GetInstance()->Create(CLicenseDialog::NODEVICE, false);
		else if (status == -2)
			CLicenseDialog::GetInstance()->Create(CLicenseDialog::EXPIRE, false);
		else
			CLicenseDialog::GetInstance()->Create(CLicenseDialog::NOAUTHORIZE, false);

		if (pClass->m_timeoutIdUkey)
		{
			g_source_remove(pClass->m_timeoutIdUkey);
			pClass->m_timeoutIdUkey = 0;
		}
	}

	return TRUE;
}

/*
 * 函数功能：验证key的有效性
 * 参数：
 *    feature：功能ID
 *  返回值:
 *    0:授权   -1:未找到设备   -2：过期  -3：未授权
 */
int CAuthorize::CheckAuthorize(unsigned int feature)
{
    hasp_handle_t handle = HASP_INVALID_HANDLE_VALUE;
    hasp_status_t status;

    // 与锁建立会话
    status = hasp_login(feature, (hasp_vendor_code_t *)vendor_code, &handle);

	switch (status)
	{
		case HASP_STATUS_OK:
			hasp_logout(handle);
			PRINTF("UKEY: HASP_STATUS_OK\n");
			return 0;
		case HASP_HASP_NOT_FOUND:
			PRINTF("UKEY: HASP_HASP_NOT_FOUND\n");
			return -1;
		case HASP_FEATURE_EXPIRED:
			PRINTF("UKEY: HASP_FEATURE_EXPIRED\n");
			return -2;
		case HASP_FEATURE_NOT_FOUND:
			PRINTF("UKEY: HASP_FEATURE_NOT_FOUND\n");
			return -3;
		case HASP_FEATURE_TYPE_NOT_IMPL:
			PRINTF("UKEY: HASP_FEATURE_TYPE_NOT_IMPL\n");
			return -3;
		case HASP_TMOF:
			PRINTF("UKEY: HASP_TMOF\n");
			return -3;
		case HASP_INSUF_MEM:
			PRINTF("UKEY: HASP_INSUF_MEM\n");
			return -3;
		case HASP_INV_VCODE:
			PRINTF("UKEY: HASP_INV_VCODE\n");
			return -3;
		case HASP_NO_DRIVER:
			PRINTF("UKEY: HASP_NO_DRIVER\n");
			return -3;
		case HASP_NO_VLIB:
			PRINTF("UKEY: HASP_NO_VLIB\n");
			return -3;
		case HASP_OLD_DRIVER:
			PRINTF("UKEY: HASP_OLD_DRIVER\n");
			return -3;
		case HASP_UNKNOWN_VCODE:
			PRINTF("UKEY: HASP_UNKNOWN_VCODE\n");
			return -3;
		case HASP_TOO_MANY_USERS:
			PRINTF("UKEY: HASP_TOO_MANY_USERS\n");
			return -3;
		case HASP_OLD_LM:
			PRINTF("UKEY: HASP_OLD_LM\n");
			return -3;
		case HASP_DEVICE_ERR:
			PRINTF("UKEY: HASP_DEVICE_ERR\n");
			return -3;
		case HASP_TIME_ERR:
			PRINTF("UKEY: HASP_TIME_ERR\n");
			return -3;
		case HASP_HARDWARE_MODIFIED:
			PRINTF("UKEY: HASP_HARDWARE_MODIFIED\n");
			return -3;
		case HASP_TS_DETECTED:
			PRINTF("UKEY: HASP_TS_DETECTED\n");
			return -3;
		case HASP_LOCAL_COMM_ERR:
			PRINTF("UKEY: HASP_LOCAL_COMM_ERR\n");
			return -3;
		case HASP_REMOTE_COMM_ERR:
			PRINTF("UKEY: HASP_REMOTE_COMM_ERR\n");
			return -3;
		case HASP_OLD_VLIB:
			PRINTF("UKEY: HASP_OLD_VLIB\n");
			return -3;
		case HASP_CLONE_DETECTED:
			PRINTF("UKEY: HASP_CLONE_DETECTED\n");
			return -3;
		default:
			break;
	}

	return -3;
}

