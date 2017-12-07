#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "Def.h"
#include "display/gui_global.h"
#include "display/gui_func.h"
#include "base/IniFile.h"
#include "patient/FileMan.h"
#include "periDevice/PeripheralMan.h"
#include "sysMan/UpgradeMan.h"
#include "sysMan/ViewSystem.h"
#include "display/ViewDialog.h"

#define FILE_MAX_NUM 50
#define FILENAME_MAX_LEN 255

UpgradeMan* UpgradeMan::m_ptrInstance = NULL;

const string UpgradeMan::SECTION = "Upgrade";
const string UpgradeMan::m_info[] = { N_("Sure to upgrade?"),
                                      N_("Upgrading, please wait..."),
                                      N_("Upgrade success,please reboot!"),
                                      N_("Upgrade failed: USB device not found!"),
                                      N_("Upgrade failed: The system has not been restarted  after last upgrade.\nPlease restart the system and try again!"),
                                      N_("Upgrade failed: Cannot mount USB device!"),
                                      N_("Upgrade failed: Cannot copy upgrade file!"),
                                      N_("Upgrade failed: Upgrade file not found!"),
                                      N_("Upgrade failed: Dest directory not found!"),
                                      N_("Upgrade failed!") };

UpgradeMan* UpgradeMan::GetInstance()
{
    if (m_ptrInstance == NULL)
        m_ptrInstance = new UpgradeMan;
    return m_ptrInstance;
}

void UpgradeMan::NewIniFile()
{
    char path[256];
    sprintf(path, "%s/%s", CFG_RES_PATH, UPGRADE_INI_PATH);
    ptrIni = new IniFile(path);
}

UpgradeMan::UpgradeMan()
{
    NewIniFile();
}

UpgradeMan::~UpgradeMan()
{
     if (ptrIni != NULL) {
         delete ptrIni;
         ptrIni = NULL;
     }
     if (m_ptrInstance != NULL)
         delete m_ptrInstance;
}

/**
* @return machineType
*/
void UpgradeMan::GetMachineType(string& machinetype)
{
    machinetype = ptrIni->ReadString(SECTION.c_str(), "MachineType");
}

/**
* @return softVersion
*/
void UpgradeMan::GetSoftVersion(string& softversion)
{
    softversion = ptrIni->ReadString(SECTION.c_str(), "SoftVersion");
}

/**
* @return fpgaVersion to gui to display
*/
void UpgradeMan::GetFpgaVersion(string& fpgaversion)
{
    fpgaversion = ptrIni->ReadString(SECTION.c_str(),"FpgaVersion");
}

void UpgradeMan::GetRebootState(gint *state)
{
    *state = ptrIni->ReadInt(SECTION.c_str(), "Upgrade-Reboot");
}

void UpgradeMan::GetOrderState(gint *state)
{
    *state = ptrIni->ReadInt(SECTION.c_str(), "Upgrade-Order");
}

void UpgradeMan::SetRebootState(const char *val)
{
    ptrIni->WriteString(SECTION.c_str(), "Upgrade-Reboot", val);
    ptrIni->SyncConfigFile();
}

void UpgradeMan::SetOrderState(const char *val)
{
    ptrIni->WriteString(SECTION.c_str(), "Upgrade-Order", val);
    ptrIni->SyncConfigFile();
}

int UpgradeMan::SureToUpgrade()
{
    int reboot;
    char tmp[5];

    GetRebootState(&reboot);
    if(!reboot)
    {
        return E_REBOOT;
    }

    PeripheralMan* ptrPeripheralMan = PeripheralMan::GetInstance();

    if(!ptrPeripheralMan->CheckUsbStorageState())
    {
        PRINTF("No UsbStorage Plug-in\n");
        return E_NO_USB;
    }

    if(!ptrPeripheralMan->MountUsbStorage())
        return E_MOUNT;

    //check all of the dest file in the list
    PRINTF("\n%s: Begin to check the file in the list\n", __FUNCTION__);
    int i;
    string value;
    char *listPath = g_build_path(G_DIR_SEPARATOR_S, UPGRADE_SRC_PATH, UPGRADE_LIST, NULL);
    if (!g_file_test(listPath, G_FILE_TEST_EXISTS))
    {
        g_free(listPath);
        return E_NO_SRC;
    }

    int order;
    char *destAppPath;

    GetOrderState(&order);
    if(order==0)
        destAppPath = g_build_path(G_DIR_SEPARATOR_S, UPGRADE_DEST_PATH, "1", NULL);
    else if(order==1)
        destAppPath = g_build_path(G_DIR_SEPARATOR_S, UPGRADE_DEST_PATH, "0", NULL);
    else
    {
        g_free(listPath);
        return E_OTHER;
    }

    if(g_file_test(destAppPath, G_FILE_TEST_IS_DIR) < 0)
    {
        perror("Check dest dir error:");
        g_free(listPath);
        g_free(destAppPath);
        return E_NO_DEST;
    }

    FileMan fm;
    char *src, *dest;
    vector<string> vecSrc, vecDest;
    vecSrc.clear();
    vecDest.clear();
    IniFile ini(listPath);
    for(i=1; i<=FILE_MAX_NUM; i++)
    {
        sprintf(tmp, "%d", i);
        value = ini.ReadString("Upgrade-List", tmp);
        PRINTF("Read Upgrade List : %s\n", value.c_str());
        if (strcmp(value.c_str(), "") != 0)
        {
            if(strcmp(value.c_str(), APP_NAME)==0)
                dest = g_build_filename(destAppPath, value.c_str(), NULL);
            else
                dest = g_build_filename(UPGRADE_DEST_PATH, value.c_str(), NULL);
            PRINTF("dest file = %s\n", dest);

            src = g_build_filename(UPGRADE_SRC_PATH, value.c_str(), NULL);
            PRINTF("src = %s\n", src);
#if  0
            if(access(src, F_OK) < 0)
            {
                perror("Check source file error:");
                g_free(src);
                g_free(dest);
                g_free(destAppPath);
                return E_NO_SRC;
            }
#endif
            if(g_file_test(src, G_FILE_TEST_EXISTS))
            {
                if(g_file_test(src, G_FILE_TEST_IS_DIR))
                {
                    if(!fm.CheckDir(dest))
                    {
                        g_free(src);
                        g_free(dest);
                        g_free(destAppPath);
                        return E_NO_DEST;
                    }
                }
                else if(g_file_test(src, G_FILE_TEST_IS_REGULAR))
                {
                    vecSrc.push_back(src);
                    vecDest.push_back(dest);
                    g_free(src);
                    g_free(dest);
                }
            }
            else
            {
                perror("Check source file error:");
                g_free(src);
                g_free(dest);
                g_free(destAppPath);
                return E_NO_SRC;
            }
        }
        else
        {
            break;
        }
    }
    g_free(destAppPath);
    g_free(listPath);

    //copy dest to src
    PRINTF("\n%s: Begin to copy file to dest\n", __FUNCTION__);
    int retval = UpgradeFile(vecSrc, vecDest);
    if(retval != SUCCESS)
        return retval;
    else
        return SUCCESS;
}

int UpgradeMan::UpgradeFile(vector<string> src, vector<string> dest)
{
    vector<string>::iterator  iteSrc, iteDest;
    PRINTF("%s: source vector size = %d, dest vector size = %d\n", __FUNCTION__, src.size(), dest.size());
    if(src.size() != dest.size() || src.size() == 0 || dest.size() == 0)
        return E_OTHER;

    int order;
    GetOrderState(&order);
    FileMan fm;
    for(iteSrc=src.begin(), iteDest=dest.begin(); iteSrc<src.end(), iteDest<dest.end(); iteSrc++, iteDest++)
    {
        PRINTF("%s: Copy %s To %s\n", __FUNCTION__, (*iteSrc).c_str(), (*iteDest).c_str());
        if(fm.CopyFile((*iteSrc).c_str(), (*iteDest).c_str()) < 0)
        {
            return E_COPY;
        }

        gchar *basename = g_path_get_basename((*iteDest).c_str());
        gchar *name2 = g_path_get_basename(UPGRADE_INI_PATH);

        if(strcmp(basename, APP_NAME)==0 || strcmp(basename, name2)==0)
        {
            if(strcmp(basename, name2)==0)
            {
                //delete old ini file
                if (ptrIni != NULL) {
                    delete ptrIni;
                    ptrIni = NULL;
                }
                NewIniFile(); //reload new ini file
            }
            char tmp[5];
            if(order==0)
                sprintf(tmp, "1");
            else if(order==1)
                sprintf(tmp, "0");
            SetOrderState(tmp);
            SetRebootState("0");
        }
        g_free(basename);
    }

    return SUCCESS;
}

#if 0
int UpgradeMan::SureToUpgrade()
{
    int reboot;
    char tmp[5];

    GetRebootState(&reboot);
    if(!reboot)
    {
        return E_REBOOT;
    }

    PeripheralMan* ptrPeripheralMan = PeripheralMan::GetInstance();

    if(!ptrPeripheralMan->CheckUsbStorageState())
    {
        PRINTF("No UsbStorage Plug-in\n");
        return E_NO_USB;
    }

    if(!ptrPeripheralMan->MountUsbStorage())
        return E_MOUNT;

    int i;
    string value;
    char *list_path = g_build_path(G_DIR_SEPARATOR_S, UPGRADE_SRC_PATH, UPGRADE_LIST, NULL);
    if (!g_file_test(list_path, G_FILE_TEST_EXISTS))
        return E_NO_SRC;
    IniFile ini(list_path);
    for(i=1; i<=FILE_MAX_NUM; i++)
    {
        sprintf(tmp, "%d", i);
        value = ini.ReadString("Upgrade-List", tmp);
        PRINTF("Read Upgrade List : %s\n", value.c_str());
        if (strcmp(value.c_str(), "") != 0)
        {
            UpgradeFile(value.c_str());
        }
        else
        {
            break;
        }
    }

    g_free(list_path);

    return SUCCESS;
}

int UpgradeMan::UpgradeFile(const char *file)
{
    int order;
    char *src, *dest;
    char *destPath;

    if(strcmp(file, APP_NAME)==0)
    {
        GetOrderState(&order);

        if(order==0)
            destPath = g_build_path(G_DIR_SEPARATOR_S, UPGRADE_DEST_PATH, "1", NULL);
        else if(order==1)
            destPath = g_build_path(G_DIR_SEPARATOR_S, UPGRADE_DEST_PATH, "0", NULL);
    }
    else
    {
        destPath = g_build_path(G_DIR_SEPARATOR_S, UPGRADE_DEST_PATH, NULL);
    }

    if(access(destPath, F_OK) < 0)
    {
        perror("Check dest dir error:");
        g_free(destPath);
        return E_NO_DEST;
    }

    dest = g_build_filename(destPath, file, NULL);
    g_free(destPath);
    PRINTF("dest file = %s\n", dest);

    src = g_build_filename(UPGRADE_SRC_PATH, file, NULL);
    PRINTF("src = %s\n", src);
    if(access(src, F_OK) < 0)
    {
        perror("Check source file error:");
        g_free(src);
        g_free(dest);
        return E_NO_SRC;
    }

    //copyfile_file_io(src, dest);
    FileMan fm;
    fm.CopyFile(src, dest);
    g_free(src);
    g_free(dest);

    if(strcmp(file, APP_NAME)==0)
    {
        char tmp[5];
        if(order==0)
            sprintf(tmp, "1");
        else if(order==1)
            sprintf(tmp, "0");
        SetOrderState(tmp);
        SetRebootState("0");
    }

    PRINTF("Upgrade file %s success!\n", file);
    return SUCCESS;
}
#endif

gboolean TimeoutUpgrade(gpointer data)
{
    int val;
    val = UpgradeMan::GetInstance()->SureToUpgrade();
    PeripheralMan::GetInstance()->UmountUsbStorage();

//  ViewDialog::GetInstance()->Destroy();
    ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()),
            ViewDialog::INFO,
            _(UpgradeMan::m_info[val].c_str()),
            NULL);
    return FALSE;
}

int HandleOKUpgrade(gpointer data)
{
    ViewHintDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()),
                                              _(UpgradeMan::m_info[UpgradeMan::PROGRESS].c_str()));

    g_timeout_add(500, TimeoutUpgrade, NULL);

    return 1;
}

void UpgradeMan::Upgrade()
{
    ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()),
            ViewDialog::QUESTION,
            _(m_info[QUESTION].c_str()),
            HandleOKUpgrade);
}