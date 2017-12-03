/*
 * 2009, 深圳恩普电子技术有限公司
 *
 * @file: ExamItem.cpp
 * @brief: manage parameter of exam item and auto-optimizing image
 * 
 * version: V1.0
 * date: 2009-6-7
 * @author: zhanglei
 */
#include "ModeStatus.h"
#include <stdio.h>
#include <string.h>
#include "ExamItem.h"
#include "../base/IniFile.h"
#include "../include/Def.h"
#include "ViewProbe.h"
#include "ViewSystem.h"
#include <locale.h>
#include <libintl.h>
#include "ImgPw.h"
#include "FileMan.h"

#include <errno.h>
//#include "ScanMode.h"

// 0: "AdultAbdo": 成人腹部
// 1: "AdultLiver": 成人肝胆脾
// 2: "KidAbdo": 小儿腹部
// 3: "AdultCardio": 成人心脏
// 4: "KidCardio": 小儿心脏
// 5: "MammaryGlands": 乳腺
// 6: "Thyroid": 甲状腺
// 7: "EyeBall": 眼球
// 8: "SmallPart": 浅表器官
// 9: "Gyn": 妇科
// 10: "EarlyPreg": 早孕
// 11: "MiddleLaterPreg": 中晚孕
// 12: "FetusCardio": 胎儿心脏
// 13: "KidneyUreter": 双肾输尿管
// 14: "BladderProstate": 膀胱前列腺
// 15: "Carotid": 颈动脉
// 16: "Jugular": 颈静脉
// 17: "PeripheryArtery": 外周动脉 
// 18: "PeripheryVein": 外周静脉
// 19: "HipJoint": 髋关节
// 20: "Meniscus": 半月板
// 21: "JointCavity": 关节腔
// 22: "Spine": 脊柱
// 23: "TCD": 经颅多普勒
// 24: "MSK": 肌健
// 25: "User1": 用户自定义1
// 26: "User2": 用户自定义2
// 27: "User3": 用户自定义3
// 28: "User4": 用户自定义4


#ifdef VET
#ifdef EMP_355
const string ExamItem::ITEM_LIB[NUM_ITEM] = 
{
	"Dog","Cat","Bovine","Equine","Swine","Sheep"
};
#else
const string ExamItem::ITEM_LIB[NUM_ITEM] = 
{
	"User1","User2","User3","User4","User5","User6","User7","User8","User9","User10"
};
#endif
#else
const string ExamItem::ITEM_LIB[NUM_ITEM] = 
{
	"AdultAbdo","AdultLiver","KidAbdo", "AdultCardio", "KidCardio", "MammaryGlands", 
	//"Thyroid", "EyeBall", "SmallPart", "Gyn", "EarlyPreg", "MiddleLaterPreg", 
	"Thyroid", "EyeBall", "Testicle", "Gyn", "EarlyPreg", "MiddleLaterPreg", 
	"FetusCardio", "KidneyUreter", "BladderProstate", "Carotid", "Jugular", "PeripheryArtery", 
	"PeripheryVein", "HipJoint", "Meniscus", "JointCavity", "Spine", "MSK", "TCD",  "User1", "User2", "User3", "User4"
};
#endif
const string ExamItem::KEY_COMMON = "Common";
///> public func
ExamItem::ExamItem()
{
	m_probeIndex = 0;
#ifdef VET
	m_itemIndex = (EItem)0;
#else
	m_itemIndex = ABDO_ADULT;
#endif
	InitItemPara(&m_paraItem);
	InitItemPara(&m_paraOptimize);

	int i;
	for (i = 0; i < NUM_PROBE; i ++)
	{
		m_vecItemIndex[i].clear();
	}

    InitItemOfProbe();
    m_genFirstItem = "Carotid";
}

ExamItem::~ExamItem()
{
}

/*
 * @brief get item list of designated probe
 *
 * @para probeModel[in] designamted probe model, in
 * @para itemList[out] list of item index in ITEM_LIB returned, out
 */
void ExamItem::GetItemListOfProbe(char* probeModel, vector<enum EItem> *ptrItemList)
{
    int probeIndex = GetProbeIndex(probeModel);
    *ptrItemList = m_vecItemIndex[probeIndex];
     // vector<ExamPara> vecExamItem;
    PRINTF("SIZE of item list = %d\n", ptrItemList->size());
}
/*
 * @brief get user item list of designated probe
 *
 * @para probeModel[in] designamted probe model, in
 * @para itemList[out] list of user item index in ITEM_LIB returned, out
 */
void ExamItem::GetUserItemListOfProbe(char* probeModel, vector<string> &ItemList)
{
    m_vecUserItemName.clear();
    // GetUserItemInfo(probeModel);
    string genfirstitem="Adult Abdomen";
    GetUserItemInfo(probeModel, genfirstitem);
    ItemList = m_vecUserItemName;
}

/*
 * @brief get user item info 
 *
 * @para probeModel[in] designamted probe model, in
 */
void ExamItem::GetUserItemInfo(char* probeModel, string &genfirstitem)
{
    char probelist[256];
    char useritem[256];
    char department[256];
    char genFirstItem[256];
    char src_group[256];
    char userselect[256];
    char path[256];
    sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini(path);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini);

    
    vector<string> useritemgroup;
    vector<string> vecExamItem;
    ExamItem examitem;
    useritemgroup = GetDefaultUserGroup();

    int group_length(0);
    group_length = useritemgroup.size();
    int num = 0;
    string str_user_item_name =  ViewProbe::GetInstance()->GetItemNameUserDef();
    for (int i= 0 ; i <  group_length; i++)
    {
        sprintf(src_group ,"%s", useritemgroup[i].c_str());
        examitem.GetDefaultUserItem(src_group, userselect, probelist, useritem, department, genFirstItem);
        ExamPara exampara;
        exampara.dept_name=department;
        exampara.name = useritem;
        exampara.index = ExamItem::USERNAME;
        if(strcmp(username.c_str(), userselect) == 0)
        {
            if (strcmp(probeModel, probelist)==0)
            {
                m_vecUserItemName.push_back(exampara.name);

                if(strcmp(str_user_item_name.c_str(), useritem) == 0)
                {
                    m_genFirstItem = genFirstItem;
                    genfirstitem = m_genFirstItem;
                    break;
                }
            }
        }
        PRINTF("----------------probemodel = %s\n", probeModel);
    }
}


void ExamItem::GetInitUserItemInfo(char* probeModel, string inituseritem, string &geninitfirstitem)
{
    char probelist[256];
    char useritem[256];
    char department[256];
    char genFirstItem[256];
    char src_group[256];
    char userselect[256];
    char path[256];
    sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini(path);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini);

    
    vector<string> useritemgroup;
    vector<string> vecExamItem;
    ExamItem examitem;
    useritemgroup = examitem.GetDefaultUserGroup();

    int group_length(0);
    group_length = useritemgroup.size();
    int num = 0;
    string str_user_item_name =  inituseritem;
    for (int i= 0 ; i <  group_length; i++)
    {
        sprintf(src_group ,"%s", useritemgroup[i].c_str());
        examitem.GetDefaultUserItem(src_group, userselect, probelist, useritem, department, genFirstItem);
        ExamPara exampara;
        exampara.dept_name=department;
        exampara.name = useritem;
        exampara.index = ExamItem::USERNAME;
        PRINTF("probeMODL=%s, probelist=%s\n",probeModel, probelist);
        if(strcmp(username.c_str(), userselect) == 0)
        {
            if (strcmp(probeModel, probelist)==0)
            {
                m_vecUserItemName.push_back(exampara.name);
                if(strcmp(str_user_item_name.c_str(), useritem) == 0)
                {
                    m_genFirstItem = genFirstItem;
                    geninitfirstitem = m_genFirstItem;
                    break;
                }
            }
        }
        PRINTF("----------------probemodel = %s\n", probeModel);
    }
}

/*
 * @brief set probe "probeModel"'s default item
 * 
 * @para probeModel probe model
 * @retval value of default item
 */
enum ExamItem::EItem ExamItem::GetDefaultItem(char* probeModel)
{
    //char path[256];
    //sprintf(path, "%s/%s", CFG_RES_PATH, STORE_ITEM_PATH);
    //IniFile ini(path);
    //int item = ReadSelectedProbeItem(probeModel, &ini);
    //m_itemIndex = ExamItem::EItem (item);
    //PRINTF("----m_itemIndex=%d\n", m_itemIndex);

    m_probeIndex = GetProbeIndex(probeModel);
    m_itemIndex = m_vecItemIndex[m_probeIndex][0];
   
    //SetItemOfProbe(probeModel,m_itemIndex);

	return m_itemIndex;
}

void ExamItem::SetUserItemOfProbe(char* probeModel, enum EItem itemIndex, const char* item)
{
	m_probeIndex = GetProbeIndex(probeModel);
	m_itemIndex = itemIndex;

	// read current item para value to m_paraItem
	char path[256];
#ifdef VET
 	if (strcmp(user_configure, "VetItemPara.ini") == 0)    
        sprintf(path, "%s%s", CFG_RES_PATH, EXAM_FILE);
 else
        sprintf(path, "%s%s%s", CFG_RES_PATH, EXAM_FILE_DIR, user_configure);

#else
    //const gchar *name = ViewProbe::GetInstance()->GetUserName();
    if (strcmp(user_configure, "ItemPara.ini") == 0)    
        sprintf(path, "%s%s", CFG_RES_PATH, EXAM_FILE);
    else
        sprintf(path, "%s%s%s", CFG_RES_PATH, EXAM_FILE_DIR, user_configure);
#endif
    IniFile ini(path);
	ReadConfigCommon(&m_paraItem, KEY_COMMON + "-" + item, &ini);
	ReadConfigOther(&m_paraItem, PROBE_LIST[m_probeIndex] + "-" + item, &ini);
}
/*
 * @brief set current probe and current item of probe, and get para value according current probe and item, save to m_paraItem
 */
void ExamItem::SetItemOfProbe(char* probeModel, enum EItem itemIndex)
{
    m_probeIndex = GetProbeIndex(probeModel);
    m_itemIndex = itemIndex;

    // read current item para value to m_paraItem
    char path[256];

#ifdef VET
    
    if (strcmp(user_configure, "VetItemPara.ini") == 0)    
        sprintf(path, "%s%s", CFG_RES_PATH, EXAM_FILE);
    else
        sprintf(path, "%s%s%s", CFG_RES_PATH, EXAM_FILE_DIR, user_configure);

#else
    if (strcmp(user_configure, "ItemPara.ini") == 0)    
        sprintf(path, "%s%s", CFG_RES_PATH, EXAM_FILE);
    else
        sprintf(path, "%s%s%s", CFG_RES_PATH, EXAM_FILE_DIR, user_configure);
#endif
    IniFile ini(path);
    ReadConfigCommon(&m_paraItem, KEY_COMMON + "-" + ITEM_LIB[m_itemIndex], &ini);
    ReadConfigOther(&m_paraItem, PROBE_LIST[m_probeIndex] + "-" + ITEM_LIB[m_itemIndex], &ini);
}
/*
 * @brief get current selected item's para
 *
 * @para ptrPara item's para value saved in it
 */
void ExamItem::GetCurrentItemPara(ParaItem &ptrPara)
{
	ptrPara = m_paraItem;
}

/*
 * @brief generate default config file, para value in it should be revise according to real condition, only used when the first time when "ItemPara.ini" is generate 
 * note: only used for our programer
 */
void ExamItem::GenerateDefaultExamItem()
{
	int i;
	int j;

	InitItemPara(&m_paraItem);
	InitItemOfProbe();
	
	char path[256];
	sprintf(path, "%s/%s", CFG_RES_PATH, DEFAULT_EXAM_FILE);
	IniFile ini(path);
	// write common para in each item
	for (j = 0; j < NUM_ITEM; j ++)
	{
		WriteConfigCommon(&m_paraItem, KEY_COMMON + "-" + ITEM_LIB[j], &ini);
	}

	// write different para in each item and each probe
	//一个探头对应一组检查项目，见m_vecItemIndex[]， 最多MAX_ITEM（16）个。
	int size;
	int itemIndex;
	for (j = 0; j < NUM_PROBE; j ++)
	{
		size = m_vecItemIndex[j].size(); // get the size of item in probe PROBE_LIST[j];
		for (i = 0; i < size; i ++)
		{
			itemIndex = m_vecItemIndex[j][i];
			WriteConfigOther(&m_paraItem, PROBE_LIST[j] + "-" + ITEM_LIB[itemIndex], &ini);
		}
	}
}

/*
 * @brief read image optimizing para of designated probe 
 *
 * @para probeModel designated probe model  
 */
void ExamItem::GetImgOptimize(char* probeModel, ParaItem &para)
{
	char path[256];
	sprintf(path, "%s/%s", CFG_RES_PATH, OPTIMIZE_FILE);
	IniFile ini(path);

	int probeIndex = GetProbeIndex(probeModel);
	ReadConfigCommon(&m_paraOptimize, PROBE_LIST[probeIndex], &ini);
	ReadConfigOther(&m_paraOptimize, PROBE_LIST[probeIndex], &ini);

	para = m_paraOptimize;
}


/*
 * @brief read parameter of examItem in config file, according to probeIndex and itemIndex
 * @para probeIndex [in] index of probe in PROBE_LIST
 * @para itemIndex [in] index of exam item in ITEM_LIB
 * @para paraItem[out] pointer saved all parameter read from ItemPara.ini file 
 */
void ExamItem::ReadExamItemPara(int probeIndex, int itemIndex, ParaItem* paraItem, char *itemName)
{
    char path[256];
    const gchar *name = ViewSystem::GetInstance()->GetUserName();
   if ((strcmp(name, "System Default") != 0) && (strcmp(name, "Умолчан системы") != 0) && 
            (strcmp(name, "系统默认") != 0) && (strcmp(name, "Domyślne Systemu") != 0)  &&
            (strcmp(name, "Par défaut du sys.") != 0) && (strcmp(name, "Systemvorgabe") != 0)&& (strcmp(name, "Sistema por defecto") !=0))
    {
        sprintf(path, "%s%s%s", CFG_RES_PATH, EXAM_FILE_DIR, user_configure);
      //        sprintf(path, "%s%s%s%s%s", CFG_RES_PATH, EXAM_FILE_DIR, name, ".ini");
    }
    else
    {
        sprintf(path, "%s%s", CFG_RES_PATH, EXAM_FILE);
    }

    IniFile ini(path);
    if(probeIndex>=0)
    {
        if(itemIndex == ExamItem::USERNAME)
        {
            ReadConfigCommon(paraItem, KEY_COMMON + "-" + itemName, &ini);
            ReadConfigOther(paraItem, PROBE_LIST[probeIndex] + "-" + itemName, &ini);
        }
        else
        {
            ReadConfigCommon(paraItem, KEY_COMMON + "-" + ITEM_LIB[itemIndex], &ini);
            ReadConfigOther(paraItem, PROBE_LIST[probeIndex] + "-" + ITEM_LIB[itemIndex], &ini);
        }
    }
}
/*
 * @brief write parameter of examItem to config file, according to probeIndex and itemIndex
 * @para probeIndex[in] index of probe in PROBE_LIST
 * @para itemIndex[in] index of exam item in ITEM_LIB
 * @para paraItem[out] pointer saved all parameter read from ItemPara.ini file 
 */
void ExamItem::WriteExamItemPara(int probeIndex, int itemIndex, ParaItem* paraItem,char *itemName)
{
    FileMan fm;
    char path[256];
    char path_other[256]; //只是用来到处默认用户时候的参数
    const gchar *name = ViewSystem::GetInstance()->GetUserName();
    if ((strcmp(name, "System Default") != 0) && (strcmp(name, "Умолчан системы") != 0) && 
            (strcmp(name, "系统默认") != 0) && (strcmp(name, "Domyślne Systemu") != 0)  &&
            (strcmp(name, "Par défaut du sys.") != 0) && (strcmp(name, "Systemvorgabe") != 0)&& (strcmp(name, "Sistema por defecto") !=0))

    {
        sprintf(path, "%s%s%s%s%s", CFG_RES_PATH, EXAM_FILE_DIR,"userconfig/", name, ".ini");
    }
    else
    {
        sprintf(path, "%s%s", CFG_RES_PATH, EXAM_FILE);
        sprintf(path_other, "%s%s", CFG_RES_PATH, EXAM_FILE_OTHER);
        FILE *fd;
        if( (fd = fopen(path_other, "r")) == NULL)
        {
            PRINTF("open default itempara file error\n");
            fm.CopyFile(path, path_other);
        }
        fclose(fd);
        IniFile ini_other(path_other);
        if(itemIndex==ExamItem::USERNAME)
        {
            WriteConfigCommon(paraItem, KEY_COMMON + "-" + itemName, &ini_other);
            WriteConfigOther(paraItem, PROBE_LIST[probeIndex] + "-" + itemName, &ini_other);
        }
        else
        {
            WriteConfigCommon(paraItem, KEY_COMMON + "-" + ITEM_LIB[itemIndex], &ini_other);
            WriteConfigOther(paraItem, PROBE_LIST[probeIndex] + "-" + ITEM_LIB[itemIndex], &ini_other);
        }
    }
    IniFile ini(path);
    if(itemIndex==ExamItem::USERNAME)
    {
        WriteConfigCommon(paraItem, KEY_COMMON + "-" + itemName, &ini);
        WriteConfigOther(paraItem, PROBE_LIST[probeIndex] + "-" + itemName, &ini);
    }
    else
    {
        WriteConfigCommon(paraItem, KEY_COMMON + "-" + ITEM_LIB[itemIndex], &ini);
        WriteConfigOther(paraItem, PROBE_LIST[probeIndex] + "-" + ITEM_LIB[itemIndex], &ini);
    }
}

vector <string> ExamItem::GetUserGroup(void)
{
    char path[256];
    sprintf(path, "%s%s", CFG_RES_PATH, EXAM_ITEM_FILE);
    IniFile ini(path);
    IniFile *ptrIni= &ini;
    return ptrIni->GetGroupName();
}

vector <string> ExamItem::GetCurrentUserGroup(void)
{
    char path[256];
   const gchar *username = ViewSystem::GetInstance()->GetUserName();
    char name[256];
    strcpy(name,username);
    if(strcmp(name,_("System Default"))==0)
    {
        strcpy(name, "SystemDefault");
    }
    sprintf(path, "%s%s%s%s", CFG_RES_PATH, EXAM_ITEM_FILE,name, ".ini");

    FILE *fp = NULL;
    if((fp = fopen(path, "r")) == NULL)
    {
        PRINTF("errno_str=%s\n", strerror(errno));
        vector<string> tmp;
        tmp.clear(); 
        return tmp;
    }
    else
    {
        IniFile ini(path);
        IniFile *ptrIni= &ini;

        fclose(fp);
        return ptrIni->GetGroupName();
    }
}

vector <string> ExamItem::GetCurrentProbeUserGroup(char *username)
{
    char name[256];
    char path[256];
    if(strcmp(username,"System Default")==0)
    {
        strcpy(name, "SystemDefault");
    }
    else
        strcpy(name, username);
    sprintf(path, "%s%s%s%s", CFG_RES_PATH, EXAM_ITEM_FILE, name, ".ini");
    int fd; 
    FILE *fp;
    if((fp = fopen(path, "r")) == NULL)
    {
        vector<string> tmp;
        tmp.clear(); 
        return tmp;
    }
    else
    {
        IniFile ini(path);
        IniFile *ptrIni= &ini;
        fclose(fp);
        return ptrIni->GetGroupName();
    }
}


vector <string> ExamItem::GetDefaultUserGroup(void)
{
    char path[256];
    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    if(strcmp(username.c_str(),"System Default")==0)
    {
        username="SystemDefault";
    }
    sprintf(path, "%s%s%s%s", CFG_RES_PATH, EXAM_ITEM_FILE, username.c_str(), ".ini");
    int fd; 
    FILE *fp;
    if((fp = fopen(path, "r")) == NULL)
    {
        vector<string> tmp;
        tmp.clear(); 
        return tmp;
    }
    else
    {
        IniFile ini(path);
        IniFile *ptrIni= &ini;
        fclose(fp);
        return ptrIni->GetGroupName();
    }
}

 void ExamItem::GetDefaultUserDefinedItem(const char * group, char *userselect, char *username, char *probelist_value, char *useritem_value, char * department_value, char *genFirstItem)
{
    char name[256];
    if(strcmp(username,"System Default")==0)
    {
        strcpy(name, "SystemDefault");
    }
    else
        strcpy(name, username);

    char path[256];

    sprintf(path, "%s%s%s%s", CFG_RES_PATH, EXAM_ITEM_FILE, name, ".ini");
    int fd; 
    FILE *fp;
    if((fp = fopen(path, "r")) == NULL)
    {
        printf("file is error\n");
    }
    else
    {
        IniFile ini(path);
        IniFile *ptrIni= &ini;

        strcpy(userselect,(char*)ptrIni->ReadString(group, "UserSelect").c_str());
        strcpy(probelist_value,(char*)ptrIni->ReadString(group, "ProbeType").c_str());
        strcpy(useritem_value,(char*)ptrIni->ReadString(group, "ExamItem").c_str());
#ifndef VET
        strcpy(department_value,(char*)ptrIni->ReadString(group, "Department").c_str());
        strcpy(genFirstItem,(char*)ptrIni->ReadString(group, "GenFirstItem").c_str());
#endif
        PRINTF("first item:%s\n", genFirstItem);
    }
    fclose(fp);
}



 void ExamItem::GetDefaultUserItem(const char * group, char *userselect, char *probelist_value, char *useritem_value, char * department_value, char *genFirstItem)
{
     char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    if(strcmp(username.c_str(),"System Default")==0)
    {
        username="SystemDefault";
    }

    char path[256];

    sprintf(path, "%s%s%s%s", CFG_RES_PATH, EXAM_ITEM_FILE,username.c_str(), ".ini");
    int fd; 
    FILE *fp;
    if((fp = fopen(path, "r")) == NULL)
    {
       printf("file is error\n");
    }
    else
    {
        IniFile ini(path);
        IniFile *ptrIni= &ini;

        strcpy(userselect,(char*)ptrIni->ReadString(group, "UserSelect").c_str());
        strcpy(probelist_value,(char*)ptrIni->ReadString(group, "ProbeType").c_str());
        strcpy(useritem_value,(char*)ptrIni->ReadString(group, "ExamItem").c_str());
#ifndef VET
        strcpy(department_value,(char*)ptrIni->ReadString(group, "Department").c_str());
        strcpy(genFirstItem,(char*)ptrIni->ReadString(group, "GenFirstItem").c_str());
#endif
        PRINTF("first item:%s\n", genFirstItem);
    }
    fclose(fp);
}


 void ExamItem::GetUserItem(const char * group, char *userselect, char *probelist_value, char *useritem_value, char * department_value, char *genFirstItem)
{
      char path[256];
   const gchar *username = ViewSystem::GetInstance()->GetUserName();
    char name[256];
    strcpy(name,username);
    if(strcmp(name,_("System Default"))==0)
    {
        strcpy(name, "SystemDefault");
    }
    sprintf(path, "%s%s%s%s", CFG_RES_PATH, EXAM_ITEM_FILE,name, ".ini");
    
    IniFile ini(path);
    IniFile *ptrIni= &ini;

    strcpy(userselect,(char*)ptrIni->ReadString(group, "UserSelect").c_str());
    strcpy(probelist_value,(char*)ptrIni->ReadString(group, "ProbeType").c_str());
    strcpy(useritem_value,(char*)ptrIni->ReadString(group, "ExamItem").c_str());
#ifndef VET
    strcpy(department_value,(char*)ptrIni->ReadString(group, "Department").c_str());
    strcpy(genFirstItem,(char*)ptrIni->ReadString(group, "GenFirstItem").c_str());
#endif
    PRINTF("first item:%s\n", genFirstItem);
}

 void ExamItem::DeleteUserItem(const char * group)
{
    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    if(strcmp(username.c_str(),"System Default")==0)
    {
        username="SystemDefault";   
    }

    char path[256];
    sprintf(path, "%s%s%s%s", CFG_RES_PATH, EXAM_ITEM_FILE,username.c_str(), ".ini");
    int fd; 
    FILE *fp;
    if((fp = fopen(path, "r")) == NULL)
    {
        printf("error");
    }
    else
    {
        IniFile ini(path);
        IniFile *ptrIni= &ini;
        ptrIni->RemoveGroup(group);
        ptrIni->SyncConfigFile();

    }
    fclose(fp);
}

void ExamItem::DeleteNewItemForCalcFile(int probeIndex, const char *old_string, const char *str)
{
    const gchar *username = ViewSystem::GetInstance()->GetUserName();
    char path[256];
    if(strcmp(username, _("System Default")) == 0)
    {
        sprintf(path, "%s%s", CFG_RES_PATH, CALC_FILE);
    }
    else
    {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username, ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni= &ini;

    ptrIni->RemoveGroup(old_string);

    ptrIni->SyncConfigFile();
}

void ExamItem::DeleteNewItemForMeasureFile(int probeIndex, const char *old_string, const char *str)
{
    const gchar *username = ViewSystem::GetInstance()->GetUserName();
    char path[256];
    if(strcmp(username, _("System Default")) == 0)
    {
        sprintf(path, "%s%s", CFG_RES_PATH, MEASURE_FILE);
    }
    else
    {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, MEASURE_PATH, username, ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni= &ini;

    ptrIni->RemoveGroup(old_string);

    ptrIni->SyncConfigFile();
}


void ExamItem::DeleteNewItemForCommentFile(int probeIndex, const char *old_string, const char *str)
{
    const gchar *username = ViewSystem::GetInstance()->GetUserName();
    char path[256];
    if(strcmp(username, _("System Default")) == 0)
    {
        sprintf(path, "%s%s", CFG_RES_PATH, COMMENT_FILE);
    }
    else
    {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, COMMENT_PATH, username, ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni= &ini;

    ptrIni->RemoveGroup((PROBE_LIST[probeIndex] + "-" + old_string).c_str());

    ptrIni->SyncConfigFile();
}


void ExamItem::WriteNewItemToCommentFile(int probeIndex, char *new_string, const char *str)
{
    const gchar *username = ViewSystem::GetInstance()->GetUserName();
   printf("username = %s\n", username);
    char path[256];
    if(strcmp(username, _("System Default")) == 0)
    {
        sprintf(path, "%s%s", CFG_RES_PATH, COMMENT_FILE);
    }
    else
    {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, COMMENT_PATH, username, ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni= &ini;

    
 /*   char path[256];
    sprintf(path, "%s%s", CFG_RES_PATH, COMMENT_FILE);
    IniFile ini(path);
    IniFile *ptrIni= &ini;
   */ ptrIni->WriteInt((PROBE_LIST[probeIndex] + "-" + new_string).c_str(), "Number", 0);
#ifndef VET
    ptrIni->WriteString((PROBE_LIST[probeIndex] + "-" + new_string).c_str(), "Department", str);
#endif
    ptrIni->SyncConfigFile();

}

void ExamItem::WriteNewItemToCalcFile(int probeIndex, char *new_string, const char *str)
{
    const gchar *username = ViewSystem::GetInstance()->GetUserName();
    char path[256];
    if(strcmp(username, _("System Default")) == 0)
    {
        sprintf(path, "%s%s", CFG_RES_PATH, CALC_FILE);
    }
    else
    {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username, ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni= &ini;
    ptrIni->WriteInt(new_string, "Number", 0);
    ptrIni->SyncConfigFile();

}
void ExamItem::WriteNewItemToMeasureFile(int probeIndex, char *new_string, const char *str)
{
    const gchar *username = ViewSystem::GetInstance()->GetUserName();
    char path[256];
    if(strcmp(username, _("System Default")) == 0)
    {
        sprintf(path, "%s%s", CFG_RES_PATH, MEASURE_FILE);
    }
    else
    {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, MEASURE_PATH, username, ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni= &ini;
    ptrIni->WriteInt(new_string, "Number", 0);
    ptrIni->SyncConfigFile();

}


void ExamItem::DeleteNewItemFile(int probeIndex, const char *old_string, const char *str)
{
  char path[256];
    const gchar *username = ViewSystem::GetInstance()->GetUserName();
    char name[256];
    strcpy(name,username);
    if(strcmp(name,_("System Default"))==0)
    {
        strcpy(name, "SystemDefault");
    }
    sprintf(path, "%s%s%s%s", CFG_RES_PATH, EXAM_ITEM_FILE,name, ".ini");
    
    IniFile ini(path);

    DeleteNewExamItem(str, PROBE_LIST[probeIndex] + "-" + old_string, &ini);
}
void ExamItem::DeleteItemParaFile(int probeIndex, const char *old_string, const char *str)
{
    FileMan fm;
    char path[256];
    char path_other[256]; //只是用来到处默认用户时候的参数
    const gchar *name = ViewSystem::GetInstance()->GetUserName();
    if ((strcmp(name, "System Default") != 0) && (strcmp(name, "Умолчан системы") != 0) && 
            (strcmp(name, "系统默认") != 0) && (strcmp(name, "Domyślne Systemu") != 0)  &&
            (strcmp(name, "Par défaut du sys.") != 0) && (strcmp(name, "Systemvorgabe") != 0)&& (strcmp(name, "Sistema por defecto") !=0))
    {
        sprintf(path, "%s%s%s", CFG_RES_PATH, EXAM_FILE_DIR, user_configure);
       //    sprintf(path, "%s%s%s%s%s", CFG_RES_PATH, EXAM_FILE_DIR, name, ".ini");
    }
    else
    {
        sprintf(path, "%s%s", CFG_RES_PATH, EXAM_FILE);

        sprintf(path_other, "%s%s", CFG_RES_PATH, EXAM_FILE_OTHER);
        FILE *fd;
        if( (fd = fopen(path_other, "r")) == NULL)
        {
            PRINTF("open default itempara file error\n");
            fm.CopyFile(path, path_other);
        }
        fclose(fd);
        IniFile ini_other(path_other);
        DeleteNewExamItem(str, KEY_COMMON + "-" + old_string, &ini_other);
        DeleteNewExamItem(str, PROBE_LIST[probeIndex] + "-" + old_string, &ini_other);
   }
    IniFile ini(path);

    DeleteNewExamItem(str, KEY_COMMON + "-" + old_string, &ini);
    DeleteNewExamItem(str, PROBE_LIST[probeIndex] + "-" + old_string, &ini);

    /*char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, DEFAULT_EXAM_FILE);
    IniFile ini1(path1);

    DeleteNewExamItem(str, KEY_COMMON  + "-" + old_string, &ini1);
    DeleteNewExamItem(str, PROBE_LIST[probeIndex] + "-" + old_string, &ini1);
*/
}

void ExamItem::WriteDefinedItemPara(int probeIndex, char *new_string,const char *str,const char *str_index)
{
    char path[256];
    const gchar *name = ViewSystem::GetInstance()->GetUserName();
    if ((strcmp(name, "System Default") != 0) && (strcmp(name, "Умолчан системы") != 0) && 
            (strcmp(name, "系统默认") != 0) && (strcmp(name, "Domyślne Systemu") != 0)  &&
            (strcmp(name, "Par défaut du sys.") != 0) && (strcmp(name, "Systemvorgabe") != 0)&& (strcmp(name, "Sistema por defecto") !=0))

    {
      //  sprintf(path1, "%s%s%s%s%s", CFG_RES_PATH, EXAM_FILE_DIR, name, ".ini");
        sprintf(path, "%s%s%s", CFG_RES_PATH, EXAM_FILE_DIR, user_configure);
    }
    else
    {
        sprintf(path, "%s%s", CFG_RES_PATH, EXAM_FILE);
    }

    IniFile ini(path);
    WriteDefinedExamItemPara(str, PROBE_LIST[probeIndex] + "-" + new_string, &ini,PROBE_LIST[probeIndex],new_string,str_index);
}



void ExamItem::WriteDefaultDefinedItemPara(int probeIndex, char *new_string,const char *str, const char *str_index)
{
    char path[256];
    sprintf(path, "%s/%s", CFG_RES_PATH, DEFAULT_EXAM_FILE);
    IniFile ini(path);

    WriteDefaultDefinedExamItemPara(str, PROBE_LIST[probeIndex] + "-" + new_string, &ini,PROBE_LIST[probeIndex],new_string,str_index);
}

void ExamItem::WriteNewItemFile(int probeIndex, char *new_string,const char *str, const char *str_index)
{
    const gchar *username = ViewSystem::GetInstance()->GetUserName();
    char nameuser[256];
    strcpy(nameuser, username);
    if(strcmp(nameuser,_("System Default"))==0)
    {
        strcpy(nameuser, "SystemDefault");  
    }

    char path[256];
    sprintf(path, "%s%s%s%s", CFG_RES_PATH, EXAM_ITEM_FILE, nameuser, ".ini");
    
    int fd;
    fd = open(path, O_RDWR |O_CREAT, 0666);  
    close(fd);

    IniFile ini(path);

    WriteNewExamItem(str, PROBE_LIST[probeIndex] + "-" + new_string, &ini,PROBE_LIST[probeIndex],new_string, str_index);

    char path1[256];
    char path_other[256];
    const gchar *name = ViewSystem::GetInstance()->GetUserName();
    if ((strcmp(name, "System Default") != 0) && (strcmp(name, "Умолчан системы") != 0) && 
            (strcmp(name, "系统默认") != 0) && (strcmp(name, "Domyślne Systemu") != 0)  &&
            (strcmp(name, "Par défaut du sys.") != 0) && (strcmp(name, "Systemvorgabe") != 0)&& (strcmp(name, "Sistema por defecto") !=0))

    {
        sprintf(path1, "%s%s%s", CFG_RES_PATH, EXAM_FILE_DIR, user_configure);
    }
    else
    {
        sprintf(path1, "%s%s", CFG_RES_PATH, EXAM_FILE);
        sprintf(path_other, "%s%s", CFG_RES_PATH, EXAM_FILE_OTHER);
        IniFile ini1(path_other);
        WriteNewExamItem(str, PROBE_LIST[probeIndex] + "-" + new_string, &ini1, PROBE_LIST[probeIndex],new_string, str_index);
    }

    IniFile ini1(path1);

    WriteNewExamItem(str, PROBE_LIST[probeIndex] + "-" + new_string, &ini1, PROBE_LIST[probeIndex],new_string, str_index);

}

/*
 * @brief read parameter of factory defalut examItem in config file, according to probeIndex and itemIndex
 * @para probeIndex [in] index of probe in PROBE_LIST
 * @para itemIndex [in] index of exam item in ITEM_LIB
 * @para paraItem[out] pointer saved all parameter read from ItemPara.ini file 
 */
void ExamItem::ReadDefaultExamItem(int probeIndex, int itemIndex, ParaItem* paraItem, char *itemName)
{
    char path[256];
    sprintf(path, "%s/%s", CFG_RES_PATH, DEFAULT_EXAM_FILE);
    IniFile ini(path);
    if(itemIndex==ExamItem::USERNAME)
    {
        ReadConfigCommon(paraItem, KEY_COMMON + "-" + itemName, &ini);
        ReadConfigOther(paraItem, PROBE_LIST[probeIndex] + "-" + itemName, &ini);
    }
    else
    {
        ReadConfigCommon(paraItem, KEY_COMMON + "-" + ITEM_LIB[itemIndex], &ini);
        ReadConfigOther(paraItem, PROBE_LIST[probeIndex] + "-" + ITEM_LIB[itemIndex], &ini);
    }
}


/*
 * @brief generate all probe's default image optimizing para, only used when the first time "OptimizePara.ini" file is generate 
 */
void ExamItem::GenerateDefaultImgOptimize()
{
	char path[256];
	sprintf(path, "%s/%s", CFG_RES_PATH, DEFAULT_OPTIMIZE_FILE);
	IniFile ini(path);
	InitItemPara(&m_paraOptimize);

	int i;
	for (i = 0; i < NUM_PROBE; i ++)
	{
		WriteConfigCommon(&m_paraOptimize, PROBE_LIST[i], &ini);
		WriteConfigOther(&m_paraOptimize, PROBE_LIST[i], &ini);
	}
}


///> private func
/*
 * @brief get the probe index that match probeModel, if no probe match it, return an index of similar probe
 *
 * @retval index of probe in PROBE_LIST[]
 */
int ExamItem::GetProbeIndex(char* probeModel)
{

	int i; 
	for (i = 0; i < NUM_PROBE; i ++)
	{
        PRINTF("list model = %s\n", PROBE_LIST[i].c_str());
        if (strcmp(probeModel, PROBE_LIST[i].c_str()) == 0)
        {
            PRINTF("item %d is match\n", i);
            break;
        }
	}

	 PRINTF("get probe of index = %d\n", i);
    int index = i;
    if (index >= NUM_PROBE) //not found
	{
        index = 0;
	}

    return index;
}
/*
 * @brief init value of m_paraItem
 *
 * @para paraItem variable to be set value
 */
void ExamItem::InitItemPara(ParaItem* paraItem)
{
	// common
	paraItem->common.MBP = 0;
	paraItem->common.powerIndex = 9;
	paraItem->common.textType = 0;
	paraItem->common.bodymarkType = 0;
	//2D
	paraItem->d2.freqIndex = 2;
	paraItem->d2.imgScale = 1;
	paraItem->d2.gain2D = 100;
	paraItem->d2.focSum = 1;
	paraItem->d2.focPosIndex = 10;
	paraItem->d2.scanAngle = 0;
	paraItem->d2.dynamicRange = 0;
	paraItem->d2.lineDensity = 1;
	paraItem->d2.steerIndex = 0;
	paraItem->d2.AGC = 0;
	paraItem->d2.edgeEnhance = 0;
	paraItem->d2.harmonic = FALSE;
	paraItem->d2.TSI = 0;
	paraItem->d2.chroma = 0;
	paraItem->d2.leftRight = FALSE;
	paraItem->d2.upDown = FALSE;
	paraItem->d2.polarity = FALSE;
	paraItem->d2.rotate = 0;
	paraItem->d2.frameAver = 3;
	paraItem->d2.lineAver = 1;
	paraItem->d2.smooth = 1;
	paraItem->d2.noiseReject = 0;
	paraItem->d2.imgEhn = 2;
	paraItem->d2.gainM = 100;
	
	//spectrum
	paraItem->spectrum.freq = 0; //< index of color freq
	
    if(ModeStatus::IsCWImgMode())
    {
        paraItem->spectrum.gain = 50;
    }
    else if(ModeStatus::IsPWImgMode())
    {
        paraItem->spectrum.gain = 60;
    }
    paraItem->spectrum.dynamicRange = 60;
	paraItem->spectrum.PRF = 1;
	paraItem->spectrum.wallFilter = 0;
	paraItem->spectrum.invert = FALSE;
	paraItem->spectrum.timeSmooth = 0;
	paraItem->spectrum.correctAngle = 0;
	paraItem->spectrum.SV = 5;

	//color
	paraItem->color.gain = 70;
	paraItem->color.PRFIndex = 1;
	paraItem->color.wallFilter = 0;
	paraItem->color.lineDensity = 0;
	paraItem->color.sensitive = 0;
	paraItem->color.turb = 0;
	paraItem->color.invert = FALSE;
	paraItem->color.reject = 0;
	paraItem->color.smooth = 0;
	paraItem->color.persist = 0;
}

/*
 * @brief init items in each probe(in PROBE_LIST), set the item index to m_vecItemIndex
 *
 */
void ExamItem::InitItemOfProbe()
{
#ifdef VET
#ifdef EMP_355
    ///35C50L
	m_vecItemIndex[0].push_back(DOG);
	m_vecItemIndex[0].push_back(CAT);
	m_vecItemIndex[0].push_back(BOVINE);
	m_vecItemIndex[0].push_back(EQUINE);
	m_vecItemIndex[0].push_back(SWINE);
	m_vecItemIndex[0].push_back(SHEEP);
    ///70L40J
#if 0
	m_vecItemIndex[1].push_back(CAT);
	m_vecItemIndex[1].push_back(DOG);
	m_vecItemIndex[1].push_back(EQUINE);
#endif
	m_vecItemIndex[1].push_back(DOG);
	m_vecItemIndex[1].push_back(CAT);
	m_vecItemIndex[1].push_back(BOVINE);
	m_vecItemIndex[1].push_back(EQUINE);
	m_vecItemIndex[1].push_back(SWINE);
	m_vecItemIndex[1].push_back(SHEEP);
    ///75L40J
	m_vecItemIndex[2].push_back(DOG);
	m_vecItemIndex[2].push_back(CAT);
	m_vecItemIndex[2].push_back(BOVINE);
	m_vecItemIndex[2].push_back(EQUINE);
	m_vecItemIndex[2].push_back(SWINE);
	m_vecItemIndex[2].push_back(SHEEP);
    ///90L40J
	m_vecItemIndex[3].push_back(DOG);
	m_vecItemIndex[3].push_back(CAT);
	m_vecItemIndex[3].push_back(BOVINE);
	m_vecItemIndex[3].push_back(EQUINE);
	m_vecItemIndex[3].push_back(SWINE);
	m_vecItemIndex[3].push_back(SHEEP);
    ///65C10L
#if 0
	m_vecItemIndex[4].push_back(SWINE);
	m_vecItemIndex[4].push_back(BOVINE);
	m_vecItemIndex[4].push_back(SHEEP);
#endif
	m_vecItemIndex[4].push_back(DOG);
	m_vecItemIndex[4].push_back(CAT);
	m_vecItemIndex[4].push_back(BOVINE);
	m_vecItemIndex[4].push_back(EQUINE);
	m_vecItemIndex[4].push_back(SWINE);
	m_vecItemIndex[4].push_back(SHEEP);

    ///35C20I
#if 0
	m_vecItemIndex[5].push_back(CAT);
	m_vecItemIndex[5].push_back(DOG);
	m_vecItemIndex[5].push_back(EQUINE);
#endif
	m_vecItemIndex[5].push_back(DOG);
	m_vecItemIndex[5].push_back(CAT);
	m_vecItemIndex[5].push_back(BOVINE);
	m_vecItemIndex[5].push_back(EQUINE);
	m_vecItemIndex[5].push_back(SWINE);
	m_vecItemIndex[5].push_back(SHEEP);
    ///65C15E
	m_vecItemIndex[6].push_back(DOG);
	m_vecItemIndex[6].push_back(CAT);
	m_vecItemIndex[6].push_back(BOVINE);
	m_vecItemIndex[6].push_back(EQUINE);
	m_vecItemIndex[6].push_back(SWINE);
	m_vecItemIndex[6].push_back(SHEEP);
    ///30P16B
	m_vecItemIndex[7].push_back(DOG);
	m_vecItemIndex[7].push_back(CAT);
	m_vecItemIndex[7].push_back(BOVINE);
	m_vecItemIndex[7].push_back(EQUINE);
	m_vecItemIndex[7].push_back(SWINE);
	m_vecItemIndex[7].push_back(SHEEP);
    ///10L25J
	m_vecItemIndex[8].push_back(DOG);
	m_vecItemIndex[8].push_back(CAT);
	m_vecItemIndex[8].push_back(BOVINE);
	m_vecItemIndex[8].push_back(EQUINE);
	m_vecItemIndex[8].push_back(SWINE);
	m_vecItemIndex[8].push_back(SHEEP);
    ///65C10I
	m_vecItemIndex[9].push_back(DOG);
	m_vecItemIndex[9].push_back(CAT);
	m_vecItemIndex[9].push_back(BOVINE);
	m_vecItemIndex[9].push_back(EQUINE);
	m_vecItemIndex[9].push_back(SWINE);
	m_vecItemIndex[9].push_back(SHEEP);
    ///55L60G
	m_vecItemIndex[10].push_back(DOG);
	m_vecItemIndex[10].push_back(CAT);
	m_vecItemIndex[10].push_back(BOVINE);
	m_vecItemIndex[10].push_back(EQUINE);
	m_vecItemIndex[10].push_back(SWINE);
	m_vecItemIndex[10].push_back(SHEEP);
#else
	m_vecItemIndex[0].push_back(USER1);
	m_vecItemIndex[0].push_back(USER2);
	m_vecItemIndex[0].push_back(USER3);
	m_vecItemIndex[0].push_back(USER4);
	m_vecItemIndex[0].push_back(USER5);
	m_vecItemIndex[0].push_back(USER6);
	m_vecItemIndex[0].push_back(USER7);
	m_vecItemIndex[0].push_back(USER8);
	m_vecItemIndex[0].push_back(USER9);
	m_vecItemIndex[0].push_back(USER10);
	// probe 1 70L40J 8
	m_vecItemIndex[1].push_back(USER1);
	m_vecItemIndex[1].push_back(USER2);
	m_vecItemIndex[1].push_back(USER3);
	m_vecItemIndex[1].push_back(USER4);
	m_vecItemIndex[1].push_back(USER5);
	m_vecItemIndex[1].push_back(USER6);
	m_vecItemIndex[1].push_back(USER7);
	m_vecItemIndex[1].push_back(USER8);
	m_vecItemIndex[1].push_back(USER9);
	m_vecItemIndex[1].push_back(USER10);
    // probe 2 70L60J 8
    m_vecItemIndex[2].push_back(USER1);
    m_vecItemIndex[2].push_back(USER2);
    m_vecItemIndex[2].push_back(USER3);
    m_vecItemIndex[2].push_back(USER4);
    m_vecItemIndex[2].push_back(USER5);
    m_vecItemIndex[2].push_back(USER6);
    m_vecItemIndex[2].push_back(USER7);
    m_vecItemIndex[2].push_back(USER8);
    m_vecItemIndex[2].push_back(USER9);
    m_vecItemIndex[2].push_back(USER10);

    // probe 2 70L60J 8
    m_vecItemIndex[3].push_back(USER1);
    m_vecItemIndex[3].push_back(USER2);
    m_vecItemIndex[3].push_back(USER3);
    m_vecItemIndex[3].push_back(USER4);
    m_vecItemIndex[3].push_back(USER5);
    m_vecItemIndex[3].push_back(USER6);
    m_vecItemIndex[3].push_back(USER7);
    m_vecItemIndex[3].push_back(USER8);
    m_vecItemIndex[3].push_back(USER9);
    m_vecItemIndex[3].push_back(USER10);
// probe 2 70L60J 8
	m_vecItemIndex[4].push_back(USER1);
	m_vecItemIndex[4].push_back(USER2);
	m_vecItemIndex[4].push_back(USER3);
	m_vecItemIndex[4].push_back(USER4);
	m_vecItemIndex[4].push_back(USER5);
	m_vecItemIndex[4].push_back(USER6);
	m_vecItemIndex[4].push_back(USER7);
	m_vecItemIndex[4].push_back(USER8);
	m_vecItemIndex[4].push_back(USER9);
	m_vecItemIndex[4].push_back(USER10);

	// probe 2 70L60J 8
	m_vecItemIndex[5].push_back(USER1);
	m_vecItemIndex[5].push_back(USER2);
	m_vecItemIndex[5].push_back(USER3);
	m_vecItemIndex[5].push_back(USER4);
	m_vecItemIndex[5].push_back(USER5);
	m_vecItemIndex[5].push_back(USER6);
	m_vecItemIndex[5].push_back(USER7);
	m_vecItemIndex[5].push_back(USER8);
	m_vecItemIndex[5].push_back(USER9);
	m_vecItemIndex[5].push_back(USER10);

	m_vecItemIndex[6].push_back(USER1);
	m_vecItemIndex[6].push_back(USER2);
	m_vecItemIndex[6].push_back(USER3);
	m_vecItemIndex[6].push_back(USER4);
	m_vecItemIndex[6].push_back(USER5);
	m_vecItemIndex[6].push_back(USER6);
	m_vecItemIndex[6].push_back(USER7);
	m_vecItemIndex[6].push_back(USER8);
	m_vecItemIndex[6].push_back(USER9);
	m_vecItemIndex[6].push_back(USER10);


	m_vecItemIndex[7].push_back(USER1);
	m_vecItemIndex[7].push_back(USER2);
	m_vecItemIndex[7].push_back(USER3);
	m_vecItemIndex[7].push_back(USER4);
	m_vecItemIndex[7].push_back(USER5);
	m_vecItemIndex[7].push_back(USER6);
	m_vecItemIndex[7].push_back(USER7);
	m_vecItemIndex[7].push_back(USER8);
	m_vecItemIndex[7].push_back(USER9);
	m_vecItemIndex[7].push_back(USER10);


	m_vecItemIndex[8].push_back(USER1);
	m_vecItemIndex[8].push_back(USER2);
	m_vecItemIndex[8].push_back(USER3);
	m_vecItemIndex[8].push_back(USER4);
	m_vecItemIndex[8].push_back(USER5);
	m_vecItemIndex[8].push_back(USER6);
	m_vecItemIndex[8].push_back(USER7);
	m_vecItemIndex[8].push_back(USER8);
	m_vecItemIndex[8].push_back(USER9);
	m_vecItemIndex[8].push_back(USER10);

	m_vecItemIndex[9].push_back(USER1);
	m_vecItemIndex[9].push_back(USER2);
	m_vecItemIndex[9].push_back(USER3);
	m_vecItemIndex[9].push_back(USER4);
	m_vecItemIndex[9].push_back(USER5);
	m_vecItemIndex[9].push_back(USER6);
	m_vecItemIndex[9].push_back(USER7);
	m_vecItemIndex[9].push_back(USER8);
	m_vecItemIndex[9].push_back(USER9);
	m_vecItemIndex[9].push_back(USER10);
#endif
#else
	//probe 0 35C505 13
	m_vecItemIndex[0].push_back(ABDO_ADULT);
	m_vecItemIndex[0].push_back(ABDO_LIVER);
	m_vecItemIndex[0].push_back(ABDO_KID);
	m_vecItemIndex[0].push_back(GYN);
	m_vecItemIndex[0].push_back(EARLY_PREG);
	m_vecItemIndex[0].push_back(LATER_PREG);
	m_vecItemIndex[0].push_back(CAR_FETUS);
	m_vecItemIndex[0].push_back(KIDNEY);
	m_vecItemIndex[0].push_back(BLADDER);
	m_vecItemIndex[0].push_back(HIP_JOINT);
	m_vecItemIndex[0].push_back(MENISCUS);
	m_vecItemIndex[0].push_back(JOINT_CAVITY);
	m_vecItemIndex[0].push_back(SPINE);

	// probe 1 75L40K 8
	m_vecItemIndex[1].push_back(GLANDS);
	m_vecItemIndex[1].push_back(THYROID);
	m_vecItemIndex[1].push_back(EYE);
	m_vecItemIndex[1].push_back(SMALL_PART);
	m_vecItemIndex[1].push_back(HIP_JOINT);
	m_vecItemIndex[1].push_back(MENISCUS);
	m_vecItemIndex[1].push_back(JOINT_CAVITY);
	m_vecItemIndex[1].push_back(SPINE);
#if (defined(EMP_340) || defined(EMP_430) || defined(EMP_360) || (defined(EMP_161)) || defined(EMP_440))
	m_vecItemIndex[1].push_back(CAROTID);
	m_vecItemIndex[1].push_back(JUGULAR);
	m_vecItemIndex[1].push_back(PERI_ARTERY);
	m_vecItemIndex[1].push_back(PERI_VEIN);
#elif (defined(EMP_355)) //70L40J
	m_vecItemIndex[1].push_back(CAROTID);
	m_vecItemIndex[1].push_back(JUGULAR);
	m_vecItemIndex[1].push_back(PERI_ARTERY);
	m_vecItemIndex[1].push_back(PERI_VEIN);
#endif
	m_vecItemIndex[1].push_back(MSK);

	// probe 2 65C10K 2
#if defined(EMP_322)
	m_vecItemIndex[2].push_back(GYN);
#elif (defined EMP_355)
   //75L40J
    m_vecItemIndex[2].push_back(GLANDS);
	m_vecItemIndex[2].push_back(THYROID);
	m_vecItemIndex[2].push_back(EYE);
	m_vecItemIndex[2].push_back(SMALL_PART);
	m_vecItemIndex[2].push_back(HIP_JOINT);
	m_vecItemIndex[2].push_back(MENISCUS);
	m_vecItemIndex[2].push_back(JOINT_CAVITY);
	m_vecItemIndex[2].push_back(SPINE);
	m_vecItemIndex[2].push_back(CAROTID);
	m_vecItemIndex[2].push_back(JUGULAR);
	m_vecItemIndex[2].push_back(PERI_ARTERY);
	m_vecItemIndex[2].push_back(PERI_VEIN);
	m_vecItemIndex[2].push_back(MSK);
    ///90L40J
    m_vecItemIndex[3].push_back(GLANDS);
	m_vecItemIndex[3].push_back(THYROID);
	m_vecItemIndex[3].push_back(EYE);
	m_vecItemIndex[3].push_back(SMALL_PART);
	m_vecItemIndex[3].push_back(HIP_JOINT);
	m_vecItemIndex[3].push_back(MENISCUS);
	m_vecItemIndex[3].push_back(JOINT_CAVITY);
	m_vecItemIndex[3].push_back(SPINE);
	m_vecItemIndex[3].push_back(CAROTID);
	m_vecItemIndex[3].push_back(JUGULAR);
	m_vecItemIndex[3].push_back(PERI_ARTERY);
	m_vecItemIndex[3].push_back(PERI_VEIN);
#else
    m_vecItemIndex[2].push_back(GYN);
    m_vecItemIndex[2].push_back(EARLY_PREG);
    m_vecItemIndex[2].push_back(LATER_PREG);
    m_vecItemIndex[2].push_back(CAR_FETUS);
    m_vecItemIndex[2].push_back(KIDNEY);
    m_vecItemIndex[2].push_back(BLADDER);
#endif
#ifdef EMP_355
	m_vecItemIndex[3].push_back(MSK);
#endif
#if defined(EMP_430)
    //probe 3 65C15D
    m_vecItemIndex[3].push_back(ABDO_KID);
    m_vecItemIndex[3].push_back(CAR_ADULT);
    m_vecItemIndex[3].push_back(CAR_KID);
#elif (defined EMP_355)
    //90L40J
    m_vecItemIndex[4].push_back(GLANDS);
	m_vecItemIndex[4].push_back(THYROID);
	m_vecItemIndex[4].push_back(EYE);
	m_vecItemIndex[4].push_back(SMALL_PART);
	m_vecItemIndex[4].push_back(HIP_JOINT);
	m_vecItemIndex[4].push_back(MENISCUS);
	m_vecItemIndex[4].push_back(JOINT_CAVITY);
	m_vecItemIndex[4].push_back(SPINE);
	m_vecItemIndex[4].push_back(CAROTID);
	m_vecItemIndex[4].push_back(JUGULAR);
	m_vecItemIndex[4].push_back(PERI_ARTERY);
	m_vecItemIndex[4].push_back(PERI_VEIN);
	m_vecItemIndex[4].push_back(MSK);
#else
	// probe 3 35C20H 3
	m_vecItemIndex[3].push_back(ABDO_KID);
	m_vecItemIndex[3].push_back(CAR_ADULT);
	m_vecItemIndex[3].push_back(CAR_KID);
#endif

#if (defined(EMP_161))
    //30P16A
    if(NUM_PROBE > 4)
    {
        m_vecItemIndex[4].push_back(CAR_ADULT);
        m_vecItemIndex[4].push_back(CAR_KID);
        m_vecItemIndex[4].push_back(CAR_FETUS);
    }
#elif (defined EMP_430)
    {
        m_vecItemIndex[4].push_back(ABDO_ADULT);
    }
#elif (defined EMP_355)
    m_vecItemIndex[5].push_back(GYN);
    m_vecItemIndex[5].push_back(EARLY_PREG);
    m_vecItemIndex[5].push_back(LATER_PREG);
    m_vecItemIndex[5].push_back(CAR_FETUS);
    m_vecItemIndex[5].push_back(KIDNEY);
    m_vecItemIndex[5].push_back(BLADDER);
#else
    // probe 4 65C15D
    if(NUM_PROBE > 4)
    {
        m_vecItemIndex[4].push_back(ABDO_KID);
        m_vecItemIndex[4].push_back(CAR_ADULT);
        m_vecItemIndex[4].push_back(CAR_KID);
    }
#endif
 
#if (defined(EMP_313) || defined(EMP_322))
    // probe 5 90L40J
    if (NUM_PROBE > 5)
    {
        m_vecItemIndex[5].push_back(THYROID);
        m_vecItemIndex[5].push_back(MSK);
    }

// probe 6 35D40J
   if (NUM_PROBE > 6)
   {
       m_vecItemIndex[6].push_back(ABDO_ADULT);
   }

   if (NUM_PROBE > 7)
   {
       m_vecItemIndex[7].push_back(CAR_ADULT);
       m_vecItemIndex[7].push_back(CAR_KID);
       m_vecItemIndex[7].push_back(CAR_FETUS);
   }

#elif defined(EMP_360)
    // probe 5 30P16A
    if (NUM_PROBE > 5)
    {
        m_vecItemIndex[5].push_back(CAR_ADULT);
        m_vecItemIndex[5].push_back(CAR_KID);
        m_vecItemIndex[5].push_back(CAR_FETUS);
    }

   //probe 6 35D40J
   if (NUM_PROBE > 6)
   {
       m_vecItemIndex[6].push_back(ABDO_ADULT);
       m_vecItemIndex[6].push_back(EARLY_PREG);
       m_vecItemIndex[6].push_back(LATER_PREG);
   }

#elif defined(EMP_355)
   ///probe6 35C20I
    if (NUM_PROBE > 5)
    {
        m_vecItemIndex[6].push_back(ABDO_KID);
        m_vecItemIndex[6].push_back(CAR_ADULT);
        m_vecItemIndex[6].push_back(CAR_KID);
    }

    ///probe7 65C15E
   if (NUM_PROBE > 6)
   {
       m_vecItemIndex[7].push_back(ABDO_KID);
       m_vecItemIndex[7].push_back(CAR_ADULT);
       m_vecItemIndex[7].push_back(CAR_KID);
   }
   //probe8 30P16B
   if(NUM_PROBE > 7)
   {
       m_vecItemIndex[8].push_back(CAR_ADULT);
       m_vecItemIndex[8].push_back(CAR_KID);
       m_vecItemIndex[8].push_back(CAR_FETUS);
   }
   ///probe9 10L25K
   if(NUM_PROBE > 8)
   {
       m_vecItemIndex[9].push_back(GLANDS);
       m_vecItemIndex[9].push_back(THYROID);
       m_vecItemIndex[9].push_back(EYE);
       m_vecItemIndex[9].push_back(SMALL_PART);
       m_vecItemIndex[9].push_back(CAROTID);
       m_vecItemIndex[9].push_back(JUGULAR);
       m_vecItemIndex[9].push_back(PERI_ARTERY);
       m_vecItemIndex[9].push_back(PERI_VEIN);
       m_vecItemIndex[9].push_back(HIP_JOINT);
       m_vecItemIndex[9].push_back(MENISCUS);
       m_vecItemIndex[9].push_back(JOINT_CAVITY);
       m_vecItemIndex[9].push_back(SPINE);
       m_vecItemIndex[9].push_back(MSK);
   }
   ///probe10 60C10I
   if(NUM_PROBE > 9)
   {
       m_vecItemIndex[10].push_back(GYN);
   }

#else //340
   // probe 5 30P16A
   if (NUM_PROBE > 5)
   {
        m_vecItemIndex[5].push_back(CAR_ADULT);
        m_vecItemIndex[5].push_back(CAR_KID);
        m_vecItemIndex[5].push_back(CAR_FETUS);
    }

    //probe 6 90L25K
    if (NUM_PROBE > 6)
    {
        m_vecItemIndex[6].push_back(GLANDS);
        m_vecItemIndex[6].push_back(THYROID);
        m_vecItemIndex[6].push_back(EYE);
        m_vecItemIndex[6].push_back(SMALL_PART);
        m_vecItemIndex[6].push_back(CAROTID);
        m_vecItemIndex[6].push_back(JUGULAR);
        m_vecItemIndex[6].push_back(PERI_ARTERY);
        m_vecItemIndex[6].push_back(PERI_VEIN);
        m_vecItemIndex[6].push_back(HIP_JOINT);
        m_vecItemIndex[6].push_back(MENISCUS);
        m_vecItemIndex[6].push_back(JOINT_CAVITY);
        m_vecItemIndex[6].push_back(SPINE);
        m_vecItemIndex[6].push_back(MSK);
    }

    //probe 7 10L25J 
    if (NUM_PROBE > 7)
    {
        m_vecItemIndex[7].push_back(GLANDS);
        m_vecItemIndex[7].push_back(THYROID);
        m_vecItemIndex[7].push_back(EYE);
        m_vecItemIndex[7].push_back(SMALL_PART);
		m_vecItemIndex[7].push_back(CAROTID);
		m_vecItemIndex[7].push_back(JUGULAR);
		m_vecItemIndex[7].push_back(PERI_ARTERY);
		m_vecItemIndex[7].push_back(PERI_VEIN);
		m_vecItemIndex[7].push_back(HIP_JOINT);
		m_vecItemIndex[7].push_back(MENISCUS);
		m_vecItemIndex[7].push_back(JOINT_CAVITY);
		m_vecItemIndex[7].push_back(SPINE);
        m_vecItemIndex[7].push_back(MSK);
    }

	//probe 8 35D40J
	if (NUM_PROBE > 8)
	{
		m_vecItemIndex[8].push_back(ABDO_ADULT);
       	m_vecItemIndex[8].push_back(EARLY_PREG);
	    m_vecItemIndex[8].push_back(LATER_PREG);
    }

	//probe 9 65C10H
	if (NUM_PROBE > 9)
	{
		m_vecItemIndex[9].push_back(GYN);
	}
#endif
#endif
}

/*
 * @brief write common para in each item to config file
 *
 * @para paraItem it's common para are to be written
 * @para section item name
 */
void ExamItem::WriteConfigCommon(ParaItem* paraItem, string section, IniFile* ptrIni)
{
    const char* ptrSection = section.c_str();
    // common
    ptrIni->WriteInt(ptrSection, "Common-MBP", paraItem->common.MBP);
    ptrIni->WriteInt(ptrSection, "Common-PowerIndex", paraItem->common.powerIndex);
    int texType = 0, bodymarkType = 0;
    ptrIni->WriteInt(ptrSection, "Common-TextType", texType);
    ptrIni->WriteInt(ptrSection, "Common-BodymarkType", bodymarkType);
    //ptrIni->WriteInt(ptrSection, "Common-TextType", paraItem->common.textType);
    //ptrIni->WriteInt(ptrSection, "Common-BodymarkType", paraItem->common.bodymarkType);
    ptrIni->SyncConfigFile();
}

void ExamItem::TransUserSelectForEng(char *name, char *username)
{
    string system_default =_("System Default");
   if (strcmp(name, system_default.c_str())==0)
    {
        strcpy(username, "System Default");
    }
    else
    {
        strcpy(username, name);
    }
}

void ExamItem::TransItemNameEng(const char *str_index,char str_name[256])
{
    
    string adbo ="Adult Abdomen";
    string gyn ="Gynecology";
    string earlypreg ="Early Pregnancy";
    string kidney ="Kidney Ureter";
    string hipjoint ="Hip Joint";
    string mammary ="Mammary Glands";
    string kidabdo ="Kid Abdomen";
    string adultcar ="Adult Cardio";
    string fetus ="Fetal Cardio";
    string thyroid ="Thyroid";
    string carotid ="Carotid";

    string liver ="Adult Liver";
    string laterpreg ="Middle-late Pregnancy";
    string ureter ="Kidney Ureter";
    string bladder ="Bladder Prostate";
    string joint ="Joint Cavity";
    string eye ="Eye Ball"; 
    string small ="Small Part";
    string perart ="Periphery Artery";
    string pervein ="Periphery Vein";
    string kidcar ="Kid Cardio";
    string msk ="MSK";
    
    if (strcmp(str_index, liver.c_str())==0)
    {
        strcpy(str_name, "AdultLiver");
    }
    else if (strcmp(str_index, laterpreg.c_str())==0)
    {
        strcpy(str_name, "MiddleLaterPreg");
    }
    else if (strcmp(str_index, ureter.c_str())==0)
    {
        strcpy(str_name, "KidneyUreter");
    }
    else if (strcmp(str_index, bladder.c_str())==0)
    {
        strcpy(str_name, "BladderProstate");
    }
    else if (strcmp(str_index, joint.c_str())==0)
    {
        strcpy(str_name, "JointCavity");
    }
    else if (strcmp(str_index, eye.c_str())==0)
    {
        strcpy(str_name, "EyeBall");
    }
    else if (strcmp(str_index, small.c_str())==0)
    {
        strcpy(str_name, "SmallPart");
    }
    else if (strcmp(str_index, perart.c_str())==0)
    {
        strcpy(str_name, "PeripheryArtery");
    }
    else if (strcmp(str_index, pervein.c_str())==0)
    {
        strcpy(str_name, "PeripheryVein");

    }
    else if (strcmp(str_index, kidcar.c_str())==0)
    {
       strcpy(str_name, "KidCardio");

    }
    else if (strcmp(str_index, adbo.c_str())==0)
    {
        strcpy(str_name, "AdultAbdo");    
    }
    else if (strcmp(str_index, gyn.c_str())==0)
    {
        strcpy(str_name, "Gyn");
    }
    else if (strcmp(str_index,earlypreg.c_str())==0)
    {
        strcpy(str_name, "EarlyPreg");
    }
    else if (strcmp(str_index, kidney.c_str())==0)
    {
        strcpy(str_name, "KidneyUreter");
    }
    else if (strcmp(str_index,hipjoint.c_str())==0)
    {
        strcpy(str_name, "HipJoint");
    }
    else if (strcmp(str_index,mammary.c_str())==0)
    {
        strcpy(str_name, "MammaryGlands");
    }
    else if (strcmp(str_index,kidabdo.c_str())==0)
    {
        strcpy(str_name, "KidAbdo");
    }
    else if (strcmp(str_index,adultcar.c_str())==0)
    {
        strcpy(str_name, "AdultCardio");
    }
    else if (strcmp(str_index,fetus.c_str())==0)
    {
        strcpy(str_name, "FetusCardio");

    }
    else if (strcmp(str_index,thyroid.c_str())==0)
    {
        strcpy(str_name, "Thyroid");

    }
    else if (strcmp(str_index,carotid.c_str())==0)
    {
        strcpy(str_name, "Carotid");

    }
    else if (strcmp(str_index,msk.c_str())==0)
    {
        strcpy(str_name, "MSK");
    }
    else
    {
        strcpy(str_name,str_index);
    }
}

void ExamItem::TransItemName(const char *str_index,char str_name[256])
{
    
#ifndef VET
    string adbo =_("Adult Abdomen");
    string gyn =_("Gynecology");
    string earlypreg =_("Early Pregnancy");
    string kidney =_("Kidney Ureter");
    string hipjoint =_("Hip Joint");
    string mammary =_("Mammary Glands");
    string kidabdo =_("Kid Abdomen");
    string adultcar =_("Adult Cardio");
    string fetus =_("Fetal Cardio");
    string thyroid =_("Thyroid");
    string carotid =_("Carotid");

    string liver =_("Adult Liver");
    string laterpreg =_("Middle-late Pregnancy");
    string ureter =_("Kidney Ureter");
    string bladder =_("Bladder Prostate");
    string joint =_("Joint Cavity");
    string eye =_("Eye Ball"); 
    //string small =_("Small Part");
    string small =_("Testicle");
    string perart =_("Periphery Artery");
    string pervein =_("Periphery Vein");
    string kidcar =_("Kid Cardio");
    string spine=_("Spine");
    string tcd=_("TCD");
    string meniscus=_("Meniscus");
    string jugular=_("Jugular");
    string msk=_("MSK");

    if (strcmp(str_index, liver.c_str())==0)
    {
        strcpy(str_name, "AdultLiver");    
    }
    else if (strcmp(str_index, laterpreg.c_str())==0)
    {
        strcpy(str_name, "MiddleLaterPreg");
    }
    else if (strcmp(str_index, ureter.c_str())==0)
    {
        strcpy(str_name, "KidneyUreter");
    }
    else if (strcmp(str_index, bladder.c_str())==0)
    {
        strcpy(str_name, "BladderProstate");
    }
    else if (strcmp(str_index, joint.c_str())==0)
    {
        strcpy(str_name, "JointCavity");
    }
    else if (strcmp(str_index, eye.c_str())==0)
    {
        strcpy(str_name, "EyeBall");
    }
    else if (strcmp(str_index, small.c_str())==0)
    {
        //strcpy(str_name, "SmallPart");
        strcpy(str_name, "Testicle");
    }
    else if (strcmp(str_index, perart.c_str())==0)
    {
        strcpy(str_name, "PeripheryArtery");
    }
    else if (strcmp(str_index, pervein.c_str())==0)
    {
        strcpy(str_name, "PeripheryVein");
    }
    else if (strcmp(str_index, kidcar.c_str())==0)
    {
       strcpy(str_name, "KidCardio");
    }
    else if (strcmp(str_index, adbo.c_str())==0)
    {
        strcpy(str_name, "AdultAbdo");
    }
    else if (strcmp(str_index, gyn.c_str())==0)
    {
        strcpy(str_name, "Gyn");
    }
    else if (strcmp(str_index,earlypreg.c_str())==0)
    {
        strcpy(str_name, "EarlyPreg");
    }
    else if (strcmp(str_index, kidney.c_str())==0)
    {
        strcpy(str_name, "KidneyUreter");
    }
    else if (strcmp(str_index,hipjoint.c_str())==0)
    {
        strcpy(str_name, "HipJoint");
    }
    else if (strcmp(str_index,mammary.c_str())==0)
    {
        strcpy(str_name, "MammaryGlands");
    }
    else if (strcmp(str_index,kidabdo.c_str())==0)
    {
        strcpy(str_name, "KidAbdo");
    }
    else if (strcmp(str_index,adultcar.c_str())==0)
    {
        strcpy(str_name, "AdultCardio");
    }
    else if (strcmp(str_index,fetus.c_str())==0)
    {
        strcpy(str_name, "FetusCardio");
    }
    else if (strcmp(str_index,thyroid.c_str())==0)
    {
        strcpy(str_name, "Thyroid");
    }
    else if (strcmp(str_index,carotid.c_str())==0)
    {
        strcpy(str_name, "Carotid");
    }
    else if (strcmp(str_index, spine.c_str())==0)
    {
        strcpy(str_name, "Spine");
    }
    else if(strcmp(str_index, tcd.c_str())==0)
    {
        strcpy(str_name, "TCD");
    }
    else if(strcmp(str_index, meniscus.c_str())==0)
    {
        strcpy(str_name, "Meniscus");
    }
    else if(strcmp(str_index, jugular.c_str())==0)
    {
        strcpy(str_name, "Jugular");
    }
    else if(strcmp(str_index, msk.c_str())==0)
    {
        strcpy(str_name, "MSK");
    }

    else
    {
        strcpy(str_name,str_index);
    }
#else
#ifdef EMP_355
    string user1 =_("Dog");
    string user2 =_("Cat");
    string user3 =_("Bovine");
    string user4 =_("Equine");
    string user5 =_("Swine");
    string user6 =_("Sheep");
    if (strcmp(str_index, user1.c_str())==0)
    {
        strcpy(str_name, "Dog");    
    }
    else if (strcmp(str_index, user2.c_str())==0)
    {
        strcpy(str_name, "Cat");    
    }
    else if (strcmp(str_index, user3.c_str())==0)
    {
        strcpy(str_name, "Bovine");    
    }
    else if (strcmp(str_index, user4.c_str())==0)
    {
        strcpy(str_name, "Equine");    
    }
    else if (strcmp(str_index, user5.c_str())==0)
    {
        strcpy(str_name, "Swine");    
    }
    else if (strcmp(str_index, user6.c_str())==0)
    {
        strcpy(str_name, "Sheep");    
    }
else
    {
        strcpy(str_name,str_index);
    }

#else
    string user1 =_("User1");
    string user2 =_("User2");
    string user3 =_("User3");
    string user4 =_("User4");
    string user5 =_("User5");
    string user6 =_("User6");
    string user7 =_("User7");
    string user8 =_("User8");
    string user9 =_("User9");
    string user10 =_("User10");
    if (strcmp(str_index, user1.c_str())==0)
    {
        strcpy(str_name, "User1");    
    }
    else if (strcmp(str_index, user2.c_str())==0)
    {
        strcpy(str_name, "User2");    
    }
    else if (strcmp(str_index, user3.c_str())==0)
    {
        strcpy(str_name, "User3");    
    }
    else if (strcmp(str_index, user4.c_str())==0)
    {
        strcpy(str_name, "User4");    
    }
    else if (strcmp(str_index, user5.c_str())==0)
    {
        strcpy(str_name, "User5");    
    }
    else if (strcmp(str_index, user6.c_str())==0)
    {
        strcpy(str_name, "User6");    
    }
    else if (strcmp(str_index, user7.c_str())==0)
    {
        strcpy(str_name, "User7");    
    }
    else if (strcmp(str_index, user8.c_str())==0)
    {
        strcpy(str_name, "User8");    
    }
    else if (strcmp(str_index, user9.c_str())==0)
    {
        strcpy(str_name, "User9");    
    }
    else if (strcmp(str_index, user10.c_str())==0)
    {
        strcpy(str_name, "User10");    
    }
else
    {
        strcpy(str_name,str_index);
    }
#endif
#endif
}
void ExamItem::WriteDefinedExamItemPara(const char *department, string section, IniFile* ptrIni, string probelist, char *new_string,const char *str_index)
{
     char str_name[256];
    TransItemNameEng(str_index, str_name); 
    //printf("--------------str_naem=%s\n",str_name);
    ParaItem paradefinedItem;
    char path[256];
    ParaItem* paraItem;
    sprintf(path, "%s%s", CFG_RES_PATH, DEFAULT_EXAM_FILE);
    IniFile Ini(path);

    ReadConfigCommon(&paradefinedItem, KEY_COMMON + "-" + str_name, &Ini);
    ReadConfigOther(&paradefinedItem, probelist + "-" + str_name, &Ini);
    paradefinedItem.d2.rotate = Ini.ReadInt((probelist + "-" + str_name).c_str(), "D2-Rotate");

    char path1[256];
    const gchar *name = ViewSystem::GetInstance()->GetUserName();
    if ((strcmp(name, "System Default") != 0) && (strcmp(name, "Умолчан системы") != 0) && 
            (strcmp(name, "系统默认") != 0) && (strcmp(name, "Domyślne Systemu") != 0)  &&
            (strcmp(name, "Par défaut du sys.") != 0) && (strcmp(name, "Systemvorgabe") != 0)&& (strcmp(name, "Sistema por defecto") !=0))

    {
        sprintf(path1, "%s%s%s", CFG_RES_PATH, EXAM_FILE_DIR, user_configure);
    }
    else
    {
        sprintf(path1, "%s%s", CFG_RES_PATH, EXAM_FILE);
    }

    IniFile ini(path1);
    WriteConfigCommon(&paradefinedItem, KEY_COMMON + "-" + new_string, &ini);
    WriteConfigOther(&paradefinedItem, probelist + "-" + new_string, &ini);
    ini.WriteInt((probelist + "-" + new_string).c_str(), "Spectrum-DynamicRange", paradefinedItem.spectrum.dynamicRange);
    ini.WriteInt((probelist + "-" + new_string).c_str(), "Spectrum-SampleVolume", paradefinedItem.spectrum.SV);
    ini.WriteInt((probelist + "-" + new_string).c_str(), "D2-Rotate", paradefinedItem.d2.rotate);
    ini.SyncConfigFile();
}


void ExamItem::WriteDefaultDefinedExamItemPara(const char *department, string section, IniFile* ptrIni, string probelist,char *new_string,const char *str_index)
{
    char str_name[256];
    TransItemNameEng(str_index, str_name); 

    char path[256];
    ParaItem paraItem;
    sprintf(path, "%s%s", CFG_RES_PATH, EXAM_FILE);
    IniFile Ini(path);

    ReadConfigCommon(&paraItem, KEY_COMMON + "-" + str_name, &Ini);
    ReadConfigOther(&paraItem, probelist + "-" + str_name, &Ini);
    paraItem.d2.rotate = Ini.ReadInt((probelist + "-" + str_name).c_str(), "D2-Rotate");

    WriteConfigCommon(&paraItem, KEY_COMMON + "-" + new_string, ptrIni);
    WriteConfigOther(&paraItem, probelist + "-" + new_string, ptrIni);
    Ini.WriteInt((probelist + "-" + new_string).c_str(), "Spectrum-DynamicRange", paraItem.spectrum.dynamicRange);
    Ini.WriteInt((probelist + "-" + new_string).c_str(), "Spectrum-SampleVolume", paraItem.spectrum.SV);
    Ini.WriteInt((probelist + "-" + new_string).c_str(), "D2-Rotate", paraItem.d2.rotate);
    Ini.SyncConfigFile();
}


void ExamItem::WriteNewExamItem(const char *department, string section, IniFile* ptrIni, string probelist, const char *new_string,const char *str_index)
{
    const gchar *username = ViewSystem::GetInstance()->GetUserName();
    char name[256];
    strcpy(name , username);
    char userselect[256];;
    ExamItem exam;
    exam.TransUserSelectForEng(name, userselect);
    const char* ptrProbelist = probelist.c_str();
    const char* ptrSection = section.c_str();

    ptrIni->WriteString(ptrSection, "UserSelect",userselect);
    ptrIni->WriteString(ptrSection, "ProbeType",ptrProbelist);
    ptrIni->WriteString(ptrSection, "ExamItem", new_string);
#ifndef VET
    ptrIni->WriteString(ptrSection, "Department", department);
    ptrIni->WriteString(ptrSection, "GenFirstItem", str_index);
#endif
    ptrIni->SyncConfigFile();
}

void ExamItem::DeleteNewExamItem(const char *department, string section, IniFile* ptrIni)
{
    const char* ptrSection = section.c_str();
    printf("%s\n",ptrSection);
    ptrIni->RemoveGroup(ptrSection);

	ptrIni->SyncConfigFile();
}

/*
 * @brief write 2d/cfm/pw para in each item and each probe to config file
 *
 * @para paraItem it's paras except common para are to be written
 * @para section probe name + item name
 */
void ExamItem::WriteConfigOther(ParaItem* paraItem, string section, IniFile* ptrIni)
{
	const char* ptrSection = section.c_str();
	//2d
	ptrIni->WriteInt(ptrSection, "D2-DepthScale", paraItem->d2.imgScale);
	ptrIni->WriteInt(ptrSection, "D2-Gain2D", paraItem->d2.gain2D);
	ptrIni->WriteInt(ptrSection, "D2-FreqIndex", paraItem->d2.freqIndex);
	ptrIni->WriteInt(ptrSection, "D2-ScanAngle", paraItem->d2.scanAngle);
	ptrIni->WriteInt(ptrSection, "D2-SpaceCompoundIndex", paraItem->d2.spaceCompoundIndex);
	ptrIni->WriteInt(ptrSection, "D2-FreqCompoundIndex", paraItem->d2.freqCompoundIndex);
	ptrIni->WriteInt(ptrSection, "D2-Chroma", paraItem->d2.chroma);
	ptrIni->WriteInt(ptrSection, "D2-DynamicRange", paraItem->d2.dynamicRange);
	ptrIni->WriteInt(ptrSection, "D2-Harmonic", paraItem->d2.harmonic);
	ptrIni->WriteInt(ptrSection, "D2-Steer", paraItem->d2.steerIndex);
	ptrIni->WriteInt(ptrSection, "D2-TSI", paraItem->d2.TSI);
	ptrIni->WriteInt(ptrSection, "D2-LineDensity", paraItem->d2.lineDensity);
	ptrIni->WriteInt(ptrSection, "D2-EdgeEnhance", paraItem->d2.edgeEnhance);
	ptrIni->WriteInt(ptrSection, "D2-FocSum", paraItem->d2.focSum);
	ptrIni->WriteInt(ptrSection, "D2-FocPosIndex", paraItem->d2.focPosIndex);
	ptrIni->WriteInt(ptrSection, "D2-AGC", paraItem->d2.AGC);
	ptrIni->WriteInt(ptrSection, "D2-GainM", paraItem->d2.gainM);
	ptrIni->WriteInt(ptrSection, "D2-ThiFreqIndex", paraItem->d2.thiFreqIndex);
	ptrIni->WriteInt(ptrSection, "D2-LeftRight", paraItem->d2.leftRight);
	ptrIni->WriteInt(ptrSection, "D2-UpDown", paraItem->d2.upDown);
	ptrIni->WriteInt(ptrSection, "D2-Polarity", paraItem->d2.polarity);
	ptrIni->WriteInt(ptrSection, "D2-NoiseReject", paraItem->d2.noiseReject);
	//ptrIni->WriteInt(ptrSection, "D2-Rotate", paraItem->d2.rotate);
	ptrIni->WriteInt(ptrSection, "D2-FrameAver", paraItem->d2.frameAver);
	ptrIni->WriteInt(ptrSection, "D2-LineAver", paraItem->d2.lineAver);
	ptrIni->WriteInt(ptrSection, "D2-Smooth", paraItem->d2.smooth);
	ptrIni->WriteInt(ptrSection, "D2-Gamma", paraItem->d2.gamma);
	ptrIni->WriteInt(ptrSection, "D2-ImageEhn", paraItem->d2.imgEhn);
	ptrIni->WriteInt(ptrSection, "D2-GrayTransIndex", paraItem->d2.grayTransIndex);
	ptrIni->WriteInt(ptrSection, "D2-Scanline", paraItem->d2.scanline);
#if not defined(EMP_322)
#if not defined(EMP_313)
	// spectrum
	ptrIni->WriteInt(ptrSection, "Spectrum-Freq", paraItem->spectrum.freq);
    ptrIni->WriteInt(ptrSection, "Spectrum-Gain", paraItem->spectrum.gain);
    ptrIni->WriteInt(ptrSection, "Spectrum-DynamicRange", paraItem->spectrum.dynamicRange);
	ptrIni->WriteInt(ptrSection, "Spectrum-PRF", paraItem->spectrum.PRF);
	ptrIni->WriteInt(ptrSection, "Spectrum-WallFilter", paraItem->spectrum.wallFilter);
	ptrIni->WriteInt(ptrSection, "Spectrum-Invert", paraItem->spectrum.invert);
	ptrIni->WriteInt(ptrSection, "Spectrum-TimeSmooth", paraItem->spectrum.timeSmooth); //time resolution
	ptrIni->WriteInt(ptrSection, "Spectrum-CorrectAngle", paraItem->spectrum.correctAngle);
	ptrIni->WriteInt(ptrSection, "Spectrum-SampleVolume", paraItem->spectrum.SV);
	ptrIni->WriteInt(ptrSection, "Spectrum-Baseline", paraItem->spectrum.baseline);
	ptrIni->WriteInt(ptrSection, "Spectrum-SpectrumSpeed", paraItem->spectrum.speed);
	ptrIni->WriteInt(ptrSection, "Spectrum-SoundVolume", paraItem->spectrum.soundVolume);
	ptrIni->WriteInt(ptrSection, "Spectrum-NoiseThread", paraItem->spectrum.noiseThread);

	// color
	ptrIni->WriteInt(ptrSection, "Color-Gain", paraItem->color.gain);
	ptrIni->WriteInt(ptrSection, "Color-PRF", paraItem->color.PRFIndex);
	ptrIni->WriteInt(ptrSection, "Color-WallFilter", paraItem->color.wallFilter);
	ptrIni->WriteInt(ptrSection, "Color-LineDensity", paraItem->color.lineDensity);
	ptrIni->WriteInt(ptrSection, "Color-Sensitive", paraItem->color.sensitive);
	ptrIni->WriteInt(ptrSection, "Color-Onflow", paraItem->color.turb);
	ptrIni->WriteInt(ptrSection, "Color-Invert", paraItem->color.invert);
	ptrIni->WriteInt(ptrSection, "Color-Reject", paraItem->color.reject);
	ptrIni->WriteInt(ptrSection, "Color-Smooth", paraItem->color.smooth);
	ptrIni->WriteInt(ptrSection, "Color-Persist", paraItem->color.persist);
	ptrIni->WriteInt(ptrSection, "Color-FlowOpt", paraItem->color.flowOpt);
	ptrIni->WriteInt(ptrSection, "Color-Baseline", paraItem->color.baseline);
	ptrIni->WriteInt(ptrSection, "Color-Steer", paraItem->color.steer);
	//ptrIni->WriteInt(ptrSection, "Color-ColorMap", paraItem->color.colormap);
	ptrIni->WriteInt(ptrSection, "Color-Artifact", paraItem->color.artifact);
#endif
#endif
	ptrIni->SyncConfigFile();
}

void ExamItem::ReadConfigCommon(ParaItem* paraItem, string section, IniFile* ptrIni)
{
    const char* ptrSection = section.c_str();
//    printf("ptrsection=%s\n",ptrSection);
    // common
    paraItem->common.MBP = ptrIni->ReadInt(ptrSection, "Common-MBP");
    paraItem->common.powerIndex = ptrIni->ReadInt(ptrSection, "Common-PowerIndex");
    paraItem->common.textType = ptrIni->ReadInt(ptrSection, "Common-TextType");
    paraItem->common.bodymarkType = ptrIni->ReadInt(ptrSection, "Common-BodymarkType");
}

void ExamItem::ReadConfigOther(ParaItem* paraItem, string section, IniFile* ptrIni)
{
	const char* ptrSection = section.c_str();

    PRINTF("==========================item: section is %s\n", ptrSection);
	//2d
	paraItem->d2.freqIndex = ptrIni->ReadInt(ptrSection, "D2-FreqIndex");
	paraItem->d2.imgScale = ptrIni->ReadInt(ptrSection, "D2-DepthScale"); //自动优化时未使用此参数
	paraItem->d2.gain2D = ptrIni->ReadInt(ptrSection, "D2-Gain2D");
	paraItem->d2.focSum = ptrIni->ReadInt(ptrSection, "D2-FocSum");
	paraItem->d2.focPosIndex = ptrIni->ReadInt(ptrSection, "D2-FocPosIndex");
	paraItem->d2.scanAngle = ptrIni->ReadInt(ptrSection, "D2-ScanAngle");
	paraItem->d2.dynamicRange = ptrIni->ReadInt(ptrSection, "D2-DynamicRange");
	paraItem->d2.lineDensity = ptrIni->ReadInt(ptrSection, "D2-LineDensity");
	paraItem->d2.steerIndex = ptrIni->ReadInt(ptrSection, "D2-Steer");
	paraItem->d2.AGC = ptrIni->ReadInt(ptrSection, "D2-AGC");
	paraItem->d2.edgeEnhance = ptrIni->ReadInt(ptrSection, "D2-EdgeEnhance");
	paraItem->d2.chroma = ptrIni->ReadInt(ptrSection, "D2-Chroma");
	paraItem->d2.leftRight = ptrIni->ReadInt(ptrSection, "D2-LeftRight");
	paraItem->d2.upDown = ptrIni->ReadInt(ptrSection, "D2-UpDown");
	paraItem->d2.polarity = ptrIni->ReadInt(ptrSection, "D2-Polarity");
	//paraItem->d2.rotate = ptrIni->ReadInt(ptrSection, "D2-Rotate");
	paraItem->d2.frameAver = ptrIni->ReadInt(ptrSection, "D2-FrameAver");
	paraItem->d2.lineAver = ptrIni->ReadInt(ptrSection, "D2-LineAver");
	paraItem->d2.smooth = ptrIni->ReadInt(ptrSection, "D2-Smooth");
	paraItem->d2.gamma = ptrIni->ReadInt(ptrSection, "D2-Gamma");
	paraItem->d2.noiseReject = ptrIni->ReadInt(ptrSection, "D2-NoiseReject");
	paraItem->d2.harmonic = ptrIni->ReadInt(ptrSection, "D2-Harmonic");
	paraItem->d2.TSI = ptrIni->ReadInt(ptrSection, "D2-TSI");
	paraItem->d2.imgEhn = ptrIni->ReadInt(ptrSection, "D2-ImageEhn");
	paraItem->d2.gainM = ptrIni->ReadInt(ptrSection, "D2-GainM");
    paraItem->d2.grayTransIndex = ptrIni->ReadInt(ptrSection, "D2-GrayTransIndex");
    paraItem->d2.spaceCompoundIndex = ptrIni->ReadInt(ptrSection, "D2-SpaceCompoundIndex");
    paraItem->d2.freqCompoundIndex = ptrIni->ReadInt(ptrSection, "D2-FreqCompoundIndex");
    paraItem->d2.thiFreqIndex = ptrIni->ReadInt(ptrSection, "D2-ThiFreqIndex");
    paraItem->d2.scanline = ptrIni->ReadInt(ptrSection, "D2-Scanline");
#if not defined(EMP_322)
#if not defined(EMP_313)
	// spectrum
    paraItem->spectrum.freq = ptrIni->ReadInt(ptrSection, "Spectrum-Freq");
    paraItem->spectrum.gain = ptrIni->ReadInt(ptrSection, "Spectrum-Gain");
    paraItem->spectrum.dynamicRange = ptrIni->ReadInt(ptrSection, "Spectrum-DynamicRange");
    paraItem->spectrum.PRF= ptrIni->ReadInt(ptrSection, "Spectrum-PRF");
	paraItem->spectrum.wallFilter = ptrIni->ReadInt(ptrSection, "Spectrum-WallFilter");
	paraItem->spectrum.invert = ptrIni->ReadInt(ptrSection, "Spectrum-Invert");
	paraItem->spectrum.timeSmooth = ptrIni->ReadInt(ptrSection, "Spectrum-TimeSmooth");
	paraItem->spectrum.correctAngle = ptrIni->ReadInt(ptrSection, "Spectrum-CorrectAngle");
	paraItem->spectrum.SV = ptrIni->ReadInt(ptrSection, "Spectrum-SampleVolume");
	paraItem->spectrum.baseline = ptrIni->ReadInt(ptrSection, "Spectrum-Baseline");
	paraItem->spectrum.speed = ptrIni->ReadInt(ptrSection, "Spectrum-SpectrumSpeed");
	paraItem->spectrum.noiseThread = ptrIni->ReadInt(ptrSection, "Spectrum-NoiseThread");
	paraItem->spectrum.soundVolume = ptrIni->ReadInt(ptrSection, "Spectrum-SoundVolume");

	// color
	paraItem->color.gain = ptrIni->ReadInt(ptrSection, "Color-Gain");
	paraItem->color.PRFIndex = ptrIni->ReadInt(ptrSection, "Color-PRF");
	paraItem->color.wallFilter = ptrIni->ReadInt(ptrSection, "Color-WallFilter");
	paraItem->color.lineDensity = ptrIni->ReadInt(ptrSection, "Color-LineDensity");
	paraItem->color.sensitive = ptrIni->ReadInt(ptrSection, "Color-Sensitive");
	paraItem->color.turb = ptrIni->ReadInt(ptrSection, "Color-Onflow");
	paraItem->color.invert = ptrIni->ReadInt(ptrSection, "Color-Invert");
	paraItem->color.reject = ptrIni->ReadInt(ptrSection, "Color-Reject");
	paraItem->color.smooth = ptrIni->ReadInt(ptrSection, "Color-Smooth");
	paraItem->color.persist = ptrIni->ReadInt(ptrSection, "Color-Persist");
	paraItem->color.artifact = ptrIni->ReadInt(ptrSection, "Color-Artifact");
	paraItem->color.flowOpt = ptrIni->ReadInt(ptrSection, "Color-FlowOpt");
	paraItem->color.baseline = ptrIni->ReadInt(ptrSection, "Color-Baseline");
	paraItem->color.steer = ptrIni->ReadInt(ptrSection, "Color-Steer");
	paraItem->color.colormap = ptrIni->ReadInt(ptrSection, "Color-ColorMap");
#endif
#endif
}


void ExamItem::WriteSelectedProbeItem(char *probeModel, IniFile* ptrIni, int item)
{
    // write item to file
    ptrIni->WriteInt(probeModel, "SelectProbeItem", item);
    ptrIni->SyncConfigFile();
}

int ExamItem::ReadSelectedProbeItem(char *probeModel, IniFile* ptrIni)
{
	// read item from file
	return (ptrIni->ReadInt(probeModel, "SelectProbeItem"));
}
void ExamItem::WriteDefaultProbe(const char *probeModel, IniFile* ptrIni)
{
	ptrIni->WriteString("ProbeModel", "ProbeModel", probeModel);
    ptrIni->SyncConfigFile();
}

/**
 * read the last probe before close machine, and switch to this probe when open machine
 */

void ExamItem::WriteDefaultUserIndex(IniFile* ptrIni, int index)
{
    ptrIni->WriteInt("UserIndex", "DefaultUserIndex", index);
    ptrIni->SyncConfigFile();
}

int ExamItem::ReadDefaultUserIndex(IniFile* ptrIni)
{
	return (ptrIni->ReadInt("UserIndex", "DefaultUserIndex"));
}


void ExamItem::WriteDefaultUserSelect(IniFile* ptrIni, const char *username)
{
    ptrIni->WriteString("UserSelect", "DefaultUser", username);
    ptrIni->SyncConfigFile();
}

std::string ExamItem::ReadDefaultUserSelect(IniFile* ptrIni)
{
	return (ptrIni->ReadString("UserSelect", "DefaultUser"));
}

string ExamItem::ReadDefaultProbe(IniFile* ptrIni)
{
	// read default probe from file
	return (ptrIni->ReadString("ProbeModel", "ProbeModel"));
}


void ExamItem::WriteDefaultProbeItem(IniFile* ptrIni, int item)
{
    ptrIni->WriteInt("Item", "DefaultItem", item);
    ptrIni->SyncConfigFile();
}

int ExamItem::ReadDefaultProbeDefaultItem(IniFile* ptrIni)
{
	return (ptrIni->ReadInt("Item", "DefaultItem"));
}

void ExamItem::WriteDefaultProbeItemName(IniFile* ptrIni, const char *itemName)
{
    ptrIni->WriteString("UserItemName", "ItemName", itemName);
    ptrIni->SyncConfigFile();
}

std::string ExamItem::ReadDefaultProbeDefaultItemName(IniFile* ptrIni)
{
	return (ptrIni->ReadString("UserItemName", "ItemName"));
}

void ExamItem::WriteUserItemFlag(IniFile* ptrIni, bool flag)
{
    ptrIni->WriteBool("UserItemFlag", "UserFlag", flag);
    ptrIni->SyncConfigFile();
}

bool ExamItem::ReadUserItemFlag(IniFile* ptrIni)
{
    return (ptrIni->ReadBool("UserItemFlag", "UserFlag"));
}
#ifdef EMP_PROJECT
/*vector<ExamItem::ProjectDefaultParaItem> ExamItem::ReadProjectPara(const char* model, const char* user, IniFile* ptrIni)
{
    vector<ExamItem::ProjectDefaultParaItem> projectPara;
    if(!projectPara.empty())
        projectPara.clear();
    
    if(model == NULL || user == NULL) return projectPara;

    int index;
    ExamItem::ProjectDefaultParaItem para;
    char group_name[256]={"\0"};
    char key_name[256]={"\0"};
    //printf("++++++model:%s\n", model);

    int j = 0;
    int size;
    for(int i = 0; i < NUM_PROBE; i++)
        if(strcmp(model, PROBE_LIST[i].c_str()) == 0)
        {
            index = i;
            int freq_2d_sum = ProbeSocket::FREQ2D_SUM[index];
            int freq_doppler_sum = ProbeSocket::FREQ_DOPPLER_SUM[index];
            sprintf(group_name, "%s-%s", PROBE_LIST[index].c_str(), "Default");
            for(int i = 0; i < ProbeSocket::FREQ2D_SUM[index]; i++)
            {
                sprintf(key_name, "%s-%d-%s", "D2", ProbeSocket::FREQ2D[index][i].emit, "FilterSection");
                para.pro2d.FilterSection = ptrIni->ReadInt(group_name, key_name);
                sprintf(key_name, "%s-%d-%s", "D2", ProbeSocket::FREQ2D[index][i].emit, "BandPassW1");
                size = ptrIni->ReadDoubleList(group_name, key_name).size();
                if(size >= ExamItem::BAND_PASS_SIZE_D2)
                {
                    size = ExamItem::BAND_PASS_SIZE_D2;
                    for(j = 0; j < size; j++)
                    {
                        para.pro2d.BandPassW[j] = ptrIni->ReadDoubleList(group_name, key_name)[j];
                        //printf("+++%d----%.1f\n", j, para.pro2d.BandPassW1[j]);
                    }
                }
                else
                {
                    for(j = 0; j < size; j++)
                    {
                        para.pro2d.BandPassW[j] = ptrIni->ReadDoubleList(group_name, key_name)[j];
                        //printf("+++%d----%.1f\n", j, para.pro2d.BandPassW1[j]);
                    }
                    for(j = size; j < ExamItem::BAND_PASS_SIZE_D2; j++)
                        para.pro2d.BandPassW[j] = 0;
                }
                
                
                sprintf(key_name, "%s-%d-%s", "D2", ProbeSocket::FREQ2D[index][i].emit, "TxFNum");
                para.pro2d.txFNum = ptrIni->ReadInt(group_name, key_name);
                sprintf(key_name, "%s-%d-%s", "D2", ProbeSocket::FREQ2D[index][i].emit, "WpScale");
                para.pro2d.wpScale = ptrIni->ReadInt(group_name, key_name);
            }
        }

    return projectPara;
}
*/
/**
 * @brief write 2d/cfm/pw project para in each frequence and each probe to project default config file
 *
 * @para proParaItem it's project paras are to be written
 * @para model: name of probe
 * @para user: name of user defined or default user("Default")
 */
void ExamItem::WriteProjectPara(ExamItem::ProjectDefaultParaItem *proParaItem, const char* model, const char* user, IniFile* ptrIni)
{
    if(model == NULL || user == NULL) return;

    char group_name[256] = {"\0"}; 
    char key_name[256] = {"\0"}; 
    sprintf(group_name, "%s-%s", model, user);
    sprintf(key_name, "%s-%d-%s", "D2", proParaItem->pro2d.freq, "FilterSection");
    ptrIni->WriteInt(group_name, key_name, proParaItem->pro2d.FilterSection);
    sprintf(key_name, "%s-%d-%s", "D2", proParaItem->pro2d.freq, "BandPassW");
    char *tmp1 = group_name;
    char *tmp2 = key_name;
    ptrIni->WriteDoubleList(tmp1, tmp2, (double*)proParaItem->pro2d.BandPassW, ExamItem::BAND_PASS_SIZE_D2);
    sprintf(key_name, "%s-%d-%s", "D2", proParaItem->pro2d.freq, "wpScale");
    ptrIni->WriteDouble(group_name, key_name, proParaItem->pro2d.wpScale);
    sprintf(key_name, "%s-%d-%s", "D2", proParaItem->pro2d.freq, "txFNum");
    ptrIni->WriteDouble(group_name, key_name, proParaItem->pro2d.txFNum);
    sprintf(key_name, "%s-%d-%s", "D2", proParaItem->pro2d.freq, "2dPulse");
    ptrIni->WriteInt(group_name, key_name, proParaItem->pro2d.d2Pluse);
    sprintf(key_name, "%s-%d-%s", "D2", proParaItem->pro2d.freq, "cfmPulse");
    ptrIni->WriteInt(group_name, key_name, proParaItem->pro2d.cfmPluse);
     sprintf(key_name, "%s-%d-%s", "D2", proParaItem->pro2d.freq, "pwPulse");
    ptrIni->WriteInt(group_name, key_name, proParaItem->pro2d.pwPluse);
    sprintf(key_name, "%s-%d-%s", "D2", proParaItem->pro2d.freq, "ProbeR");
    ptrIni->WriteDouble(group_name, key_name, proParaItem->probeR);
    sprintf(key_name, "%s-%d-%s", "D2", proParaItem->pro2d.freq, "ProbeAngle");
    ptrIni->WriteDouble(group_name, key_name, proParaItem->probeAngle);
    ptrIni->SyncConfigFile();
}

/**
 * @brief remove group from project config file or project default config file
 *
 * @para model: name of probe
 * @para name: name of user defined or default user("Default")
 */
void ExamItem::RemoveUserFromFile(const char* model, const char* user, IniFile* ptrIni)
{
    if(model == NULL || user == NULL) 
    { 
        perror("input is null\n");
        return;
    }
  char group_name[256] = {"\0"};
  sprintf(group_name, "%s-%s", model, user);
  ptrIni->RemoveGroup((char*)group_name);
  ptrIni->SyncConfigFile();
}

/**
 *@brief read paramenter from debug config file
 *
 * @para debugParaItem: paramenter in debug project mode
 * @para model: name of probe
 * @para user: name of user defined or default user("Default")
 */
void ExamItem::ReadProDebugParaItem(ProjectDebugParaItem* debugPara, const char* model, const char* user, IniFile* ptrIni)
{
    char group_name[256] = {"\0"};
    char key_name[256]={"\0"};
    sprintf(group_name, "%s-%s", model, user);
    vector<double> value;
    if(!value.empty())
        value.clear();
    debugPara->depth = ptrIni->ReadInt(group_name, "depth");
    debugPara->mode = ptrIni->ReadInt(group_name, "mode");
    debugPara->freq = ptrIni->ReadInt(group_name, "freq");

    debugPara->freq_Index = ptrIni->ReadInt(group_name, "freqIndex");
    debugPara->harmonic_Index = ptrIni->ReadInt(group_name, "harmonicIndex");
    debugPara->statusTHI = ptrIni->ReadInt(group_name, "statusTHI");
    debugPara->probe_Index = ptrIni->ReadInt(group_name, "probeIndex");

    sprintf(key_name, "%s-%d-%s", "D2", debugPara->freq, "FilterSection");
    debugPara->defaultPara.pro2d.FilterSection = ptrIni->ReadInt(group_name, key_name);
    sprintf(key_name, "%s-%d-%s", "D2", debugPara->freq, "BandPassW");
    value = ptrIni->ReadDoubleList((char*)group_name, (char*)key_name);
    for(int i = 0; i < BAND_PASS_SIZE_D2; i++)
        debugPara->defaultPara.pro2d.BandPassW[i] = value[i];
    sprintf(key_name, "%s-%d-%s", "D2", debugPara->freq, "txFNum");
    debugPara->defaultPara.pro2d.txFNum = ptrIni->ReadDouble(group_name, (char*)key_name);
    sprintf(key_name, "%s-%d-%s", "D2", debugPara->freq, "wpScale");
    debugPara->defaultPara.pro2d.wpScale = ptrIni->ReadDouble(group_name, (char*)key_name);
  sprintf(key_name, "%s-%d-%s", "D2", debugPara->freq, "2dPulse");
    debugPara->defaultPara.pro2d.d2Pluse = ptrIni->ReadInt(group_name, (char*)key_name);
  sprintf(key_name, "%s-%d-%s", "D2", debugPara->freq, "cfmPulse");
    debugPara->defaultPara.pro2d.cfmPluse = ptrIni->ReadInt(group_name, (char*)key_name);
    sprintf(key_name, "%s-%d-%s", "D2", debugPara->freq, "pwPulse");
    debugPara->defaultPara.pro2d.pwPluse = ptrIni->ReadInt(group_name, (char*)key_name);
    sprintf(key_name, "%s-%d-%s", "D2", debugPara->freq, "ProbeR");
    debugPara->defaultPara.probeR = ptrIni->ReadDouble(group_name, (char*)key_name);
    sprintf(key_name, "%s-%d-%s", "D2", debugPara->freq, "ProbeAngle");
    debugPara->defaultPara.probeAngle = ptrIni->ReadDouble(group_name, (char*)key_name);

}

/**
 * @brief write project debug paramenter to debug config file
 *
 * @para debugParaItem: paramenter in debug project mode
 * @para model: name of probe
 * @para user: name of user defined or default user("Default")
 */
void ExamItem::WriteProDebugParaItem(ProjectDebugParaItem* debugPara, const char* model, const char* user, IniFile* ptrIni)
{
    if(model == NULL || user == NULL) return;

    char group_name[256] = {"\0"};
    sprintf(group_name, "%s-%s", model, user);
    ptrIni->WriteInt((char*)group_name, "depth", debugPara->depth);
    ptrIni->WriteInt((char*)group_name, "mode", debugPara->mode);
    ptrIni->WriteInt((char*)group_name, "freq", debugPara->freq);
    ptrIni->WriteInt((char*)group_name, "freqIndex", debugPara->freq_Index);
    ptrIni->WriteInt((char*)group_name, "harmonicIndex", debugPara->harmonic_Index);
    ptrIni->WriteInt((char*)group_name, "probeIndex", debugPara->probe_Index);
    ptrIni->WriteInt((char*)group_name, "statusTHI", debugPara->statusTHI);

    WriteProjectPara(&debugPara->defaultPara, model, user, ptrIni);
}
#endif
