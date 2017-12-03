/*
 * =====================================================================================
 *
 *       Filename:  CalcSetting.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/29/2012 04:09:16 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include <dirent.h>
#include "CalcSetting.h"
#include "Def.h"
#include "gui_global.h"
#include "gui_func.h"
#include "KeyValueOpr.h"
#include "KeyDef.h"
#include "MeasureDef.h"
#include "MeaCalcFun.h"
#include "ViewMain.h"
#include "ViewDialog.h"
#include "KeyFunc.h"
#include "FileMan.h"
#include "PeripheralMan.h"
#include "MenuCalcNew.h"
#include "ConfigToHost.h"
#include "UserSelect.h"
#include "TopArea.h"
#include "MeasureMan.h"
using std::vector;

char export_name[256];
#ifdef EMP_322
const string section[SECTION_NUM]={
    N_("Abdomen-General") , 
    N_("Gynecology") ,
    N_("Obstetrics-General") , 
    N_("Obstetrics-FetalHeart") ,
    N_("Urology") , 
    N_("Orthopedic") , 
    N_("SmallPart-Glands") , 
    N_("SmallPart-Thyroid") , 
    N_("SmallPart-Eye") , 
    N_("SmallPart-Testicle") ,
    N_("Cardiac") , 
    N_("Fetal Cardio")  
};

const char* examType_calc[] = 
{
    N_("Adult Abdomen"),
    N_("Adult Liver"),
    N_("Kid Abdomen"),
    N_("Adult Cardio"),
    N_("Kid Cardio"),
    N_("Mammary Glands"),
    N_("Thyroid"),
    N_("Eye Ball"),
    //N_("Small Part"),
    N_("Testicle"),
    N_("Gynecology"),
    N_("Early Pregnancy"),
    N_("Middle-late Pregnancy"),
    N_("Fetal Cardio"),
    N_("Kidney Ureter"),
    N_("Bladder Prostate"),
    N_("Hip Joint"),
    N_("Meniscus"),
    N_("Joint Cavity"),
    N_("Spine"),
    NULL
};
#else
#ifdef VET
const string section[SECTION_NUM]={
  N_("Abdomen-General") , 
  N_("Abdomen-Arterial") , 
  N_("Abdomen-Venous") ,
  N_("Obstetrics-Dog") , 
  N_("Obstetrics-Cat") , 
  N_("Obstetrics-Sheep") , 
  N_("Obstetrics-Swine") , 
  N_("Obstetrics-Bovine") , 
  N_("Obstetrics-Equine") , 
  N_("Urology") , 
  N_("Orthopedic") , 
  N_("SmallPart-Glands") , 
  N_("SmallPart-Thyroid") , 
  N_("SmallPart-Eye") , 
  N_("SmallPart-Testicle") ,
  N_("Vascular-LE") ,
  N_("Vascular-UE") ,
  N_("Vascular-Carotid") , 
  N_("Cardiac") , 
  N_("Fetal Cardio") , 
  N_("TCD"),
  N_("Tendon")
};

#else
const string section[SECTION_NUM]={
  N_("Abdomen-General") , 
  N_("Abdomen-Arterial") , 
  N_("Abdomen-Venous") ,
  N_("Gynecology") ,
  N_("Obstetrics-General") , 
  N_("Obstetrics-FetalHeart") ,
  N_("Urology") , 
  N_("Orthopedic") , 
  N_("SmallPart-Glands") , 
  N_("SmallPart-Thyroid") , 
  N_("SmallPart-Eye") , 
  N_("SmallPart-Testicle") ,
  N_("Vascular-LE") ,
  N_("Vascular-UE") ,
  N_("Vascular-Carotid") , 
  N_("Cardiac") , 
  N_("Fetal Cardio") , 
  N_("TCD")
};
#endif
#endif
CalcSetting g_menuCalcExamType;
CalcSetting* CalcSetting::m_ptrInstance = NULL;

CalcSetting::CalcSetting()
{
}

CalcSetting::~CalcSetting()
{
    m_ptrInstance = NULL;
}

CalcSetting* CalcSetting::GetInstance()
{
    if (m_ptrInstance == NULL)
        m_ptrInstance = new CalcSetting;

    return m_ptrInstance;
}

GtkWidget* CalcSetting::GetWindow(void)
{
    return m_win_parent;
}


void CalcSetting::CreateDefineItem_calc(vector<string>& vecExamItem_calc)
{  

    char probelist[256];
    char useritem[256];
    char department[256];
    char firstgenitem[256];
    char src_group[256];
    char userselect[256];
    char path_userselect[256];
    sprintf(path_userselect, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini_userselect(path_userselect);
    ExamItem examitem;
    vector<string> useritemgroup;
    useritemgroup = examitem.GetDefaultUserGroup();

    int group_length(0);
    group_length = useritemgroup.size();
    for (int i= 0 ; i <  group_length; i++)
	{
		if (useritemgroup[i].length() != 0)
		{
			sprintf(src_group ,"%s", useritemgroup[i].c_str());
			examitem.GetUserItem(src_group, userselect, probelist, useritem, department, firstgenitem);
			string username=useritem;
			vecExamItem_calc.push_back(username);
		}
    }
}

void CalcSetting::CreateItemList_Calc(char *department,vector<string>& vecItemCalc)
{
    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
         sprintf(path, "%s%s", CFG_RES_PATH, CALC_ITEM_PATH);
    }
    else
    {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, CALC_ITEM_FILE, username.c_str(), ".ini");
    }

    IniFile ini(path);
    IniFile *ptrIni= &ini;
    int number;
    number = ptrIni->ReadInt(department, "Number");
    for(int i=1;i<=number;i++)
    {
        char CalcNumber[256];
        sprintf(CalcNumber, "Calc%d", i);
        int item_num = ptrIni->ReadInt(department, CalcNumber);
        string item_name("");
        if(item_num < (USER_START - BASIC_MEA_END))
            item_name= ItemMenuTransEnglish(item_num);
        else
        {
            item_name = CustomItemTransName(item_num);
        }
        if(strlen(item_name.c_str()) != 0)
            vecItemCalc.push_back(item_name.c_str());
    }
}

void CalcSetting::CreateDefaultItemList_Calc(char *department,vector<string>& vecItemCalc)
{
    char path[256];
    sprintf(path, "%s%s", CFG_RES_PATH, DEFAULT_CALC_ITEM_FILE);

    IniFile ini(path);
    IniFile *ptrIni= &ini;
    int number;
    number = ptrIni->ReadInt(department, "Number");

    for(int i=1;i<=number;i++)
    {
        char CalcNumber[256];
        sprintf(CalcNumber, "Calc%d", i);
        int item_num = ptrIni->ReadInt(department, CalcNumber);
        string item_name;
        if(item_num < USER_START - BASIC_MEA_END)
            item_name= ItemMenuTransEnglish(item_num);
        else
        {
            item_name = CustomItemTransName(item_num);
        }
        vecItemCalc.push_back(item_name);
    }
}


int CalcSetting::ItemNameTransEtype(char *select_name)
{
    int select_num = -1;
    for(int i=0; i < (ABD_MEA_END - ABD_MEA_START);i++)
    {
        if(strcmp(select_name, _(AbdoInfo[i].title))==0 )
        {
            select_num = AbdoInfo[i].item - BASIC_MEA_END;
            return select_num;
        }
    }
    for(int i=0; i < (ADULT_MEA_END - ADULT_MEA_START);i++)
    {
        if(strcmp(select_name, _(AdultInfo[i].title))==0 )
        {
            select_num = AdultInfo[i].item - BASIC_MEA_END;
            return select_num;
        }

    }
    for(int i=0; i < (UR_MEA_END - UR_MEA_START);i++)
    {
        if(strcmp(select_name, _(URInfo[i].title))==0 )
        {
            select_num = URInfo[i].item - BASIC_MEA_END;
            return select_num;
        }

    }
    for(int i=0; i < (SP_MEA_END - SP_MEA_START);i++)
    {
        if(strcmp(select_name, _(SPInfo[i].title))==0 )
        {
            select_num = SPInfo[i].item - BASIC_MEA_END;
            return select_num;
        }

    }
    for(int i=0; i < (VS_MEA_END - VS_MEA_START);i++)
    {
        if(strcmp(select_name, _(VSInfo[i].title))==0 )
        {
            select_num = VSInfo[i].item - BASIC_MEA_END;
            return select_num;
        }

    }
    for(int i=0; i < (FETAL_MEA_END - FETAL_MEA_START);i++)
    {
        if(strcmp(select_name, _(FetalInfo[i].title))==0 )
        {
            select_num = FetalInfo[i].item - BASIC_MEA_END;
            return select_num;
        }

    }
    for(int i=0; i < (TCD_MEA_END - TCD_MEA_START);i++)
    {
        if(strcmp(select_name, _(TCDInfo[i].title))==0 )
        {
            select_num = TCDInfo[i].item - BASIC_MEA_END;
            return select_num;
        }

    }
    for(int i=0; i < (ORTHO_MEA_END - ORTHO_MEA_START);i++)
    {
        if(strcmp(select_name, _(OrthoInfo[i].title))==0 )
        {
            select_num = OrthoInfo[i].item - BASIC_MEA_END;
            return select_num;
        }

    }
#ifdef VET
    for(int i=0; i < (TD_MEA_END - TD_MEA_START);i++)
    {
        if(strcmp(select_name, _(TDInfo[i].title))==0 )
        {
            select_num = TDInfo[i].item - BASIC_MEA_END;
            return select_num;
        }

    }
    for(int i=0; i < (ANOB_MEA_END - ANOB_MEA_START);i++)
    {
        if(strcmp(select_name, _(AnOBInfo[i].title))==0 )
        {
            select_num = AnOBInfo[i].item - BASIC_MEA_END;
            return select_num;
        }

    }
#else
    for(int i=0; i < (OB_MEA_END - OB_MEA_START);i++)
    {
        if(strcmp(select_name, _(OBInfo[i].title))==0 )
        {
            select_num = OBInfo[i].item - BASIC_MEA_END;
            return select_num;
        }

    }
    for(int i=0; i < (GYN_MEA_END - GYN_MEA_START);i++)
    {
        if(strcmp(select_name, _(GYNInfo[i].title))==0 )
        {
            select_num = GYNInfo[i].item - BASIC_MEA_END;
            return select_num;
        }

    }
    for(int i=0; i < (EFW_MEA_END - EFW_MEA_START);i++)
    {
        if(strcmp(select_name, _(EFWInfo[i].title))==0 )
        {
            select_num = EFWInfo[i].item - BASIC_MEA_END;
            return select_num;
        }

    }
#endif
    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path, "%s%s", CFG_RES_PATH, CALC_ITEM_PATH);
    }
    else
    {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, CALC_ITEM_FILE, username.c_str(), ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni = &ini;
    int number = ptrIni->ReadInt("MaxNumber", "Number");
    for(int i=(USER_START - BASIC_MEA_END); i<=number; i++)
    {
        char CustomEtype[256];
        sprintf(CustomEtype, "CustomEtype-%d",i);
        string item_name = ptrIni->ReadString(CustomEtype, "Name");
        if(strcmp(select_name, _(item_name.c_str()))==0 )
        {
            select_num = ptrIni->ReadInt(CustomEtype, "Etype");

            return select_num;
        }

    }

	return select_num;
}

string CalcSetting::ItemMenuTransEnglish(int item_num)
{
	item_num += BASIC_MEA_END;
	string item_name("");
	if((item_num >= ABD_MEA_START)&&(item_num < ABD_MEA_END))
	{
		for(int i=0; i < (ABD_MEA_END - ABD_MEA_START);i++)
		{
			if(item_num == AbdoInfo[i].item)
			{
				item_name = AbdoInfo[i].title;
			}
		}
	}
	else if((item_num >= ADULT_MEA_START)&&(item_num < ADULT_MEA_END))
	{
		for(int i=0; i < (ADULT_MEA_END - ADULT_MEA_START);i++)
		{
			if(item_num == AdultInfo[i].item)
			{
				item_name = AdultInfo[i].title;
			}
		}
	}
	else if((item_num >= UR_MEA_START)&&(item_num < UR_MEA_END))
	{
        for(int i=0; i < (UR_MEA_END - UR_MEA_START);i++)
        {
            if(item_num == URInfo[i].item)
            {
                item_name = URInfo[i].title;
            }
        }
    }

    else if((item_num >= OB_MEA_START)&&(item_num < OB_MEA_END))
    {
        for(int i=0; i < (OB_MEA_END - OB_MEA_START);i++)
        {
            if(item_num == OBInfo[i].item)
            {
                item_name = OBInfo[i].title;
            }
        }
    }

    else if((item_num >= GYN_MEA_START)&&(item_num < GYN_MEA_END))
    {
        for(int i=0; i < (GYN_MEA_END - GYN_MEA_START);i++)
        {
            if(item_num == GYNInfo[i].item)
            {
                item_name = GYNInfo[i].title;
            }
        }
    }

    else if((item_num >= SP_MEA_START)&&(item_num < SP_MEA_END))
    {
        for(int i=0; i < (SP_MEA_END - SP_MEA_START);i++)
        {
            if(item_num == SPInfo[i].item)
            {
                item_name = SPInfo[i].title;
            }
        }
    }
    else if((item_num >= VS_MEA_START)&&(item_num < VS_MEA_END))
    {
        for(int i=0; i < (VS_MEA_END - VS_MEA_START);i++)
        {
            if(item_num == VSInfo[i].item)
            {
                item_name = VSInfo[i].title;
            }
        }
    }
    else  if((item_num >= FETAL_MEA_START)&&(item_num < FETAL_MEA_END))
    {
        for(int i=0; i < (FETAL_MEA_END - FETAL_MEA_START);i++)
        {
            if(item_num == FetalInfo[i].item)
            {
                item_name = FetalInfo[i].title;
            }
        }
    }
    else if((item_num >= TCD_MEA_START)&&(item_num < TCD_MEA_END))
    {
        for(int i=0; i < (TCD_MEA_END - TCD_MEA_START);i++)
        {
            if(item_num == TCDInfo[i].item)
            {
                item_name = TCDInfo[i].title;
            }
        }
    }
    else if((item_num >= ORTHO_MEA_START)&&(item_num < ORTHO_MEA_END))
    {
        for(int i=0; i < (ORTHO_MEA_END - ORTHO_MEA_START);i++)
        {
            if(item_num == OrthoInfo[i].item)
            {
                item_name = OrthoInfo[i].title;
            }
        }
    }
  #ifdef VET
    else if((item_num >= TD_MEA_START)&&(item_num < TD_MEA_END))
    {
        for(int i=0; i < (TD_MEA_END - TD_MEA_START);i++)
        {
            if(item_num == TDInfo[i].item)
            {
                item_name = TDInfo[i].title;
            }
        }
    }

    else if((item_num >= ANOB_MEA_START)&&(item_num < ANOB_MEA_END))
    {
        for(int i=0; i < (ANOB_MEA_END - ANOB_MEA_START);i++)
        {
            if(item_num == AnOBInfo[i].item)
            {
                item_name = AnOBInfo[i].title;
            }
        }
    }
#else
    else if((item_num >= EFW_MEA_START)&&(item_num < EFW_MEA_END))
    {
        for(int i=0; i < (EFW_MEA_END - EFW_MEA_START);i++)
        {
            if(item_num == EFWInfo[i].item)
            {
                item_name = EFWInfo[i].title;
            }
        }
    }
#endif
    return item_name;
}

string CalcSetting::CustomItemTransName(int item_num)
{
    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path, "%s%s", CFG_RES_PATH, CALC_ITEM_PATH);
    }
    else
    {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, CALC_ITEM_FILE, username.c_str(), ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni = &ini; 
    char CustomEtype[256];
    sprintf(CustomEtype, "CustomEtype-%d",item_num);
    string item_name = ptrIni->ReadString(CustomEtype, "Name");
    return item_name;
}


void CalcSetting::CreateItemList_Calc1(char *probe_exam,vector<string>& vecItemCalc1)
{   
    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path, "%s%s", CFG_RES_PATH, CALC_FILE);
    }
    else
    {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni= &ini;

    int number;
    number = ptrIni->ReadInt(probe_exam, "Number");

    if(number ==0)
        return;

    for(int i=1;i<=number;i++)
    {
        char CalcNumber[256];
        sprintf(CalcNumber, "Calc%d", i);
        int item_num = ptrIni->ReadInt(probe_exam, CalcNumber);
        string item_name;
        if(item_num < USER_START - BASIC_MEA_END)
        {
            item_name= ItemMenuTransEnglish(item_num);
        }
        else
        {
             item_name = CustomItemTransName(item_num);
        }
        vecItemCalc1.push_back(item_name);
    }
}

void CalcSetting::CreateItemList_Calc2(char *probe_exam,vector<int>& vecItemCalc1)
{   
    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path, "%s%s", CFG_RES_PATH, CALC_FILE);
    }
    else
    {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni= &ini;

    int number;
    number = ptrIni->ReadInt(probe_exam, "Number");

    if(number ==0)
        return;

    for(int i=1;i<=number;i++)
    {
        char CalcNumber[256];
        sprintf(CalcNumber, "Calc%d", i);
        int item_num = ptrIni->ReadInt(probe_exam, CalcNumber);
        vecItemCalc1.push_back(item_num);
    }
}

GtkTreeModel* CalcSetting::create_item_calc_model1()
{
    gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_calc));

    if(!exam_type_name)
        return NULL;
    char exam_type[256];
    ExamItem exam;
    exam.TransItemName(exam_type_name, exam_type);
	int sequence = GetSequence(exam_type);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_sequence_calc), sequence);

    vector<string> vecItemCalc1;
    vecItemCalc1.clear();
    CreateItemList_Calc1(exam_type, vecItemCalc1);
    int item_size(0);
    item_size = vecItemCalc1.size();

    if(vecItemCalc1.empty())
        return NULL;

    GtkTreeIter iter;
    GtkTreeStore *store = gtk_tree_store_new(1,
            G_TYPE_STRING);

    for (int i = 0; i<item_size; i++) 
    {
        gtk_tree_store_append(store, &iter, NULL);
        gtk_tree_store_set(store, &iter, 
                0,  _(vecItemCalc1[i].c_str()),
                -1);
next:
        continue;
    }
    return GTK_TREE_MODEL(store);
}


GtkTreeModel* CalcSetting::create_item_calc_model2()
{
    gchar *department_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_department_calc));
    if(department_name== NULL)
        return NULL;
    char department[50];
    for(int i=0; i<SECTION_NUM; i++)
    {
        if(strcmp(department_name, _(section[i].c_str()))==0)
        {
            strcpy(department, section[i].c_str());
            break;
        }
    }

    vector<string> vecItemCalc;
    vecItemCalc.clear();
    CreateItemList_Calc(department, vecItemCalc);
    int item_size(0);
    item_size = vecItemCalc.size();

    gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_calc));

    if(!exam_type_name)
        return NULL;
    char exam_type[256];
    ExamItem exam;
    exam.TransItemName(exam_type_name, exam_type);

    vector<string> vecItemCalc1;
    vecItemCalc1.clear();
    CreateItemList_Calc1(exam_type, vecItemCalc1);
    int item_size1(0);
    item_size1 = vecItemCalc1.size();
    int pos_num=0;
    if(!vecItemCalc1.empty())
    {

        for (int i = 0; i<item_size1; i++) 
        {
            vector<string>::iterator pos;
            pos = find(vecItemCalc.begin(), vecItemCalc.end(), vecItemCalc1[i]);
          
            if(pos != vecItemCalc.end())
            {
                vecItemCalc.erase(pos);
                pos_num++;
            } else if(pos == vecItemCalc.end())
            {    //vecItemCalc.pop_back();
               // pos_num++;
            }
        }  
    } 
    if(vecItemCalc.empty())
        return NULL;

    GtkTreeIter iter;
    GtkTreeStore *store = gtk_tree_store_new(1,
            G_TYPE_STRING);

    for (int i = 0; i<(item_size-pos_num); i++) 
    {
        gtk_tree_store_append(store, &iter, NULL);
        gtk_tree_store_set(store, &iter, 
                0,  _(vecItemCalc[i].c_str()),
                -1);
next:
        continue;
    }

    return GTK_TREE_MODEL(store);
}

void CalcSetting::CreateItemList_Delete_Calc(char *select_name, char *department, vector<int>& vecDeleteCalc, IniFile *ini)
{   
	IniFile *ptrIni= ini;
	ExamItem examitem;

	int number;
	number = ptrIni->ReadInt(department, "Number");

	if(number ==0)
		return;

	for(int i=1;i<=number;i++)
	{
		char CalcNumber[256];
        sprintf(CalcNumber, "Calc%d", i);
        int item_num= ptrIni->ReadInt(department, CalcNumber);
        string item_name;
        if(item_num < USER_START - BASIC_MEA_END)
            item_name= ItemMenuTransEnglish(item_num);
        else
        {
             item_name = CustomItemTransName(item_num);
        }
        if(strcmp(select_name, _(item_name.c_str()))!=0)
        {
            vecDeleteCalc.push_back(item_num);
        }
        else
        {
            int deletenum;
            char CalcNum[256];
            deletenum = ptrIni->ReadInt("Delete", "Number");
            sprintf(CalcNum, "Calc%d", (deletenum+1));
            ptrIni->WriteInt("Delete", CalcNum, item_num);
			ptrIni->WriteInt("Delete", "Number", (deletenum+1));
            ptrIni->SyncConfigFile();
        }
    }
}

void CalcSetting::CreateItemList_Delete_Calc1(char *select_name, string probe_exam,vector<int>& vecItemCalc1)
{   

    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path, "%s%s", CFG_RES_PATH, CALC_FILE);
    }
    else
    {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni= &ini;

    const char *probeExam = probe_exam.c_str();

    int number;
    number = ptrIni->ReadInt(probeExam, "Number");

    if(number ==0)
        return;

    for(int i=1;i<=number;i++)
    {
        char CalcNumber[256];
        sprintf(CalcNumber, "Calc%d", i);

        int item_num = ptrIni->ReadInt(probeExam, CalcNumber);
        string item_name;
        if(item_num < USER_START - BASIC_MEA_END)
        item_name= ItemMenuTransEnglish(item_num);
        else
        {
            item_name = CustomItemTransName(item_num);
        }
        
        if(strcmp(select_name, _(item_name.c_str()))!=0)
        {
            vecItemCalc1.push_back(item_num);
        }
    }
}

void CalcSetting::add_columns_calc(GtkTreeView *treeview)
{
    gint col_offset;
    GtkTreeViewColumn *column;

    m_cellrenderer_calc_text = gtk_cell_renderer_text_new();
    col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
            -1, "", m_cellrenderer_calc_text, "text", 0,NULL);
    column = gtk_tree_view_get_column (GTK_TREE_VIEW (treeview), col_offset - 1);
    gtk_tree_view_column_set_clickable (GTK_TREE_VIEW_COLUMN (column), TRUE);
}
void CalcSetting::add_columns_calc1(GtkTreeView *treeview)
{
    gint col_offset;
    GtkTreeViewColumn *column;

    m_cellrenderer_calc_text1 = gtk_cell_renderer_text_new();
    col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
            -1, "", m_cellrenderer_calc_text1, "text", 0,NULL);
    column = gtk_tree_view_get_column (GTK_TREE_VIEW (treeview), col_offset - 1);
    gtk_tree_view_column_set_clickable (GTK_TREE_VIEW_COLUMN (column), TRUE);
}


void CalcSetting::ExamCalcChanged(GtkComboBox *widget)
{
    ChangeModel2();

    // update measur sequence
    gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_calc));
    if(!exam_type_name)
        return;
   
    char exam_type[256];
    ExamItem exam;
    exam.TransItemName(exam_type_name, exam_type);
    int sequence = GetSequence(exam_type);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_sequence_calc), sequence);   
}

void CalcSetting::DepartmentCalcChanged(GtkComboBox *widget)
{
    ChangeModel();
}

void CalcSetting::MeasureSequenceChanged(GtkComboBox *widget)
{
    gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_calc));
    if(!exam_type_name)
        return;
    char exam_type[256];
    ExamItem exam;
    exam.TransItemName(exam_type_name, exam_type);
    
    int number = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_sequence_calc));
    
    char path[256];
    sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path);
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path1[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path1, "%s%s", CFG_RES_PATH, CALC_FILE);
    }
    else
    {
        sprintf(path1, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
    }
    IniFile ini(path1);
    IniFile *ptrIni = &ini; 
    ptrIni->WriteInt(exam_type, "Sequence", number);
    ptrIni->SyncConfigFile();
}


void CalcSetting::ChangeModel2(void)
{
    GtkTreeModel *model1 = create_item_calc_model1();
    gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_calc1), model1);

    GtkTreeModel *model = create_item_calc_model2();
    gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_calc), model);

}


void CalcSetting::ChangeModel(void)
{
    GtkTreeModel *model = create_item_calc_model2();
    gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_calc), model);

}

void CalcSetting::ChangeExamBox(char *check_part)
{
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_exam_calc), check_part);
}

void CalcSetting::ChangeExamBoxDelete(void)
{
    vector<string> vecExamItem_calc;
    vecExamItem_calc.clear();
    //系统默认的检查部位
    for (int i=0; i<= EXAM_NUM; i++)
    {
#ifdef EMP_322
        vecExamItem_calc.push_back(examType_calc[i]);
#else
        vecExamItem_calc.push_back(examType[i]);
#endif
    }
    //用户自定义的检查部位
    CreateDefineItem_calc(vecExamItem_calc);
    int exam_size(0);
    exam_size = vecExamItem_calc.size();
    for(int i = exam_size; i >= 0; i--)
    {
        gtk_combo_box_remove_text(GTK_COMBO_BOX (m_combobox_exam_calc), i);
    }

    for (int i = 0; i <exam_size; i++) 
    {
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_exam_calc), _(vecExamItem_calc[i].c_str()));
    }
}

void CalcSetting::ChangeExamBoxToDefault(void)
{
    //gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_exam_calc), 0);
}

void CalcSetting::ChangeModelAndLight(const char *name)
{
    GtkTreeModel *model = create_item_calc_model2();
    gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_calc), model);

    GtkTreeSelection *selection;
    GtkTreeIter iter_tmp;
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_calc)); 
    iter_tmp= InsertUniqueCalc(model, name);
    gtk_tree_selection_select_iter(selection, &iter_tmp);

    GtkTreePath *path_scroll = gtk_tree_model_get_path(model, &iter_tmp);
    gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(m_treeview_item_calc), path_scroll, NULL, FALSE, 1.0, 1.0);
    gtk_tree_path_free (path_scroll);
}


GtkWidget* CalcSetting::CreateCalcWindow(GtkWidget *parent)
{
    m_win_parent = parent;

    fixed_calc = gtk_fixed_new ();
    gtk_widget_show (fixed_calc);

    GtkWidget *label_exam_calc = gtk_label_new (_("Exam Type:"));
    gtk_misc_set_alignment (GTK_MISC(label_exam_calc), 0, 0.5);
    gtk_label_set_use_markup (GTK_LABEL (label_exam_calc), TRUE);
    gtk_widget_show (label_exam_calc);
    gtk_fixed_put (GTK_FIXED (fixed_calc), label_exam_calc, 10, 10);
    gtk_widget_set_size_request (label_exam_calc, 100+17, 30);

    m_combobox_exam_calc = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_exam_calc);
    gtk_fixed_put (GTK_FIXED (fixed_calc), m_combobox_exam_calc, 140, 10);
    gtk_widget_set_size_request (m_combobox_exam_calc, 180, 30);
    vector<string> vecExamItem_calc;
    vecExamItem_calc.clear();
   //系统默认的检查部位
    for (int i=0; i<= EXAM_NUM; i++)
    {
#ifdef EMP_322
        vecExamItem_calc.push_back(examType_calc[i]);
#else
        vecExamItem_calc.push_back(examType[i]);
#endif
    }
    //用户自定义的检查部位
    CreateDefineItem_calc(vecExamItem_calc);
    int exam_size(0);
    exam_size = vecExamItem_calc.size();

    for (int i = 0; i <exam_size; i++) 
	{
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_exam_calc), _(vecExamItem_calc[i].c_str()));
    }
    for (int i = 0; i <exam_size; i++) 
	{
		string curExamType;
		TopArea::GetInstance()->GetCheckPart(curExamType);
		if (strcmp(curExamType.c_str(), _(vecExamItem_calc[i].c_str())) == 0)
		{
			gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_exam_calc), i);
			break;
		}
	}
    g_signal_connect(m_combobox_exam_calc, "changed", G_CALLBACK(HandleExamCalcChanged), this);

    GtkWidget *label_department_calc = gtk_label_new (_("Available Items:"));
    gtk_misc_set_alignment (GTK_MISC(label_department_calc), 0, 0.5);
    gtk_label_set_use_markup (GTK_LABEL (label_department_calc), TRUE);
    gtk_widget_show (label_department_calc);
    gtk_fixed_put (GTK_FIXED (fixed_calc), label_department_calc, 10, 35);
    gtk_widget_set_size_request (label_department_calc, 150, 30);

    m_combobox_department_calc = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_department_calc);
    gtk_fixed_put (GTK_FIXED (fixed_calc), m_combobox_department_calc, 15, 80);
    gtk_widget_set_size_request (m_combobox_department_calc, 240, 30);

    for(int j = 0; j < SECTION_NUM; j++ )
    {
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_department_calc), _(section[j].c_str()));
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_department_calc), 0);
    g_signal_connect(m_combobox_department_calc, "changed", G_CALLBACK(HandleDepartmentCalcChanged), this);



    scrolledwindow_item_calc = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show (scrolledwindow_item_calc);
    gtk_fixed_put (GTK_FIXED (fixed_calc), scrolledwindow_item_calc, 15, 120);
    gtk_widget_set_size_request (scrolledwindow_item_calc, 240, 300);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_item_calc), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

     GtkTreeModel *model = create_item_calc_model2();
      m_treeview_item_calc = gtk_tree_view_new_with_model(model);


    add_columns_calc(GTK_TREE_VIEW(m_treeview_item_calc));

    gtk_widget_modify_base(m_treeview_item_calc, GTK_STATE_NORMAL, g_deep);

    gtk_container_add (GTK_CONTAINER (scrolledwindow_item_calc), m_treeview_item_calc);

    GtkTreeSelection *select;
    select = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_calc));
    gtk_tree_selection_set_mode(select, GTK_SELECTION_BROWSE);
    gtk_widget_show (m_treeview_item_calc);


     GtkWidget *label_sequence_calc = gtk_label_new (_("Measure Sequence:"));
    gtk_misc_set_alignment (GTK_MISC(label_sequence_calc), 0, 0.5);
    gtk_label_set_use_markup (GTK_LABEL (label_sequence_calc), TRUE);
    gtk_widget_show (label_sequence_calc);
    gtk_fixed_put (GTK_FIXED (fixed_calc), label_sequence_calc, 400, 10);
    gtk_widget_set_size_request (label_sequence_calc, 110+15+10, 30);

    m_combobox_sequence_calc = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_sequence_calc);
    gtk_fixed_put (GTK_FIXED (fixed_calc), m_combobox_sequence_calc, 540, 15);
    gtk_widget_set_size_request (m_combobox_sequence_calc, 180-75+10, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_sequence_calc), _("None"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_sequence_calc), _("Repeat"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_sequence_calc), _("Next"));
	//////////////////////////////
    ExamItem exam;
	char path11[256];
    sprintf(path11, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path11);
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
	string examType = exam.ReadDefaultProbeDefaultItemName(&ini1);
	char exam_type[256];
    exam.TransItemNameEng(examType.c_str(), exam_type);
    int sequence = GetSequence(exam_type);
	////////////////////////////////////////
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_sequence_calc), sequence);
    g_signal_connect(m_combobox_sequence_calc, "changed", G_CALLBACK(HandleMeasureSequenceChanged), this);


    GtkWidget *label_department_calc_select = gtk_label_new (_("Selected Items:"));
    gtk_misc_set_alignment (GTK_MISC(label_department_calc_select), 0, 0.5);
    gtk_label_set_use_markup (GTK_LABEL (label_department_calc_select), TRUE);
    gtk_widget_show (label_department_calc_select);
    gtk_fixed_put (GTK_FIXED (fixed_calc), label_department_calc_select, 400, 40);
    gtk_widget_set_size_request (label_department_calc_select, 150, 30);


    scrolledwindow_item_calc1 = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show (scrolledwindow_item_calc1);
    gtk_fixed_put (GTK_FIXED (fixed_calc), scrolledwindow_item_calc1, 400, 80);
    gtk_widget_set_size_request (scrolledwindow_item_calc1, 140+100+15, 340);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_item_calc1), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

    GtkTreeModel *model1 = create_item_calc_model1();
    m_treeview_item_calc1 = gtk_tree_view_new_with_model(model1);
    add_columns_calc1(GTK_TREE_VIEW(m_treeview_item_calc1));

    gtk_widget_modify_base(m_treeview_item_calc1, GTK_STATE_NORMAL, g_deep);

    gtk_container_add (GTK_CONTAINER (scrolledwindow_item_calc1), m_treeview_item_calc1);

    GtkTreeSelection *select1;
    select1 = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_calc1));
    gtk_tree_selection_set_mode(select, GTK_SELECTION_BROWSE);
    gtk_widget_show (m_treeview_item_calc1);


    m_button_select_one = gtk_button_new_with_mnemonic (">");
    gtk_widget_show (m_button_select_one);
    gtk_fixed_put (GTK_FIXED (fixed_calc), m_button_select_one, 290, 120);
    gtk_widget_set_size_request (m_button_select_one, 80, 30);
    g_signal_connect(m_button_select_one, "clicked", G_CALLBACK(HandleButtonSelectOneCalcClicked), this);

    m_button_select_all = gtk_button_new_with_mnemonic (">>");
    gtk_widget_show (m_button_select_all);
    gtk_fixed_put (GTK_FIXED (fixed_calc), m_button_select_all, 290, 170);
    gtk_widget_set_size_request (m_button_select_all, 80, 30);
    g_signal_connect(m_button_select_all, "clicked", G_CALLBACK(HandleButtonSelectAllCalcClicked), this);


    m_button_calc_delete = gtk_button_new_with_mnemonic ("<");
    gtk_widget_show (m_button_calc_delete);
    gtk_fixed_put (GTK_FIXED (fixed_calc), m_button_calc_delete, 290, 220);
    gtk_widget_set_size_request (m_button_calc_delete, 80, 30);
    g_signal_connect(m_button_calc_delete, "clicked", G_CALLBACK(HandleButtonBackOneClicked), this);

    m_button_calc_delete_all = gtk_button_new_with_mnemonic ("<<");
    gtk_widget_show (m_button_calc_delete_all);
    gtk_fixed_put (GTK_FIXED (fixed_calc), m_button_calc_delete_all,  290, 270);
    gtk_widget_set_size_request (m_button_calc_delete_all, 80, 30);
    g_signal_connect(m_button_calc_delete_all, "clicked", G_CALLBACK(HandleButtonBackAllClicked), this);

    m_button_calc_add = gtk_button_new_with_mnemonic (_("Add"));
    gtk_widget_show (m_button_calc_add);
    gtk_fixed_put (GTK_FIXED (fixed_calc), m_button_calc_add, 290, 320);
    gtk_widget_set_size_request (m_button_calc_add, 80, 30);
    g_signal_connect(m_button_calc_add, "clicked", G_CALLBACK(HandleButtonAddClicked), this);

    m_button_calc_delete_select = gtk_button_new_with_mnemonic (_("Delete"));
    gtk_widget_show (m_button_calc_delete_select);
    gtk_fixed_put (GTK_FIXED (fixed_calc), m_button_calc_delete_select, 290, 370);
    gtk_widget_set_size_request (m_button_calc_delete_select, 80, 30);
    g_signal_connect(m_button_calc_delete_select, "clicked", G_CALLBACK(HandleButtonDeleteSelectClicked), this);


    m_button_calc_up = gtk_button_new_with_mnemonic (_("Up"));
    gtk_widget_show (m_button_calc_up);
    gtk_fixed_put (GTK_FIXED (fixed_calc), m_button_calc_up, 680, 170);
    gtk_widget_set_size_request (m_button_calc_up, 80, 30);
    g_signal_connect(m_button_calc_up, "clicked", G_CALLBACK(HandleButtonUpClicked), this);

    m_button_calc_down = gtk_button_new_with_mnemonic (_("Down"));
    gtk_widget_show (m_button_calc_down);
    gtk_fixed_put (GTK_FIXED (fixed_calc), m_button_calc_down, 680, 220);
    gtk_widget_set_size_request (m_button_calc_down, 80, 30);
    g_signal_connect(m_button_calc_down, "clicked", G_CALLBACK(HandleButtonDownClicked), this);


    m_button_calc_export = gtk_button_new_with_mnemonic (_("Export"));
    gtk_widget_show (m_button_calc_export);
    gtk_fixed_put (GTK_FIXED (fixed_calc), m_button_calc_export, 680, 270);
    gtk_widget_set_size_request (m_button_calc_export, 80, 30);
    g_signal_connect(m_button_calc_export, "clicked", G_CALLBACK(HandleButtonExportClicked), this);

    m_button_calc_import = gtk_button_new_with_mnemonic (_("Import"));
    gtk_widget_show (m_button_calc_import);
    gtk_fixed_put (GTK_FIXED (fixed_calc), m_button_calc_import, 680, 320);
    gtk_widget_set_size_request (m_button_calc_import, 80, 30);
    g_signal_connect(m_button_calc_import, "clicked", G_CALLBACK(HandleButtonImportClicked), this);

    m_button_calc_default = gtk_button_new_with_mnemonic (_("Default Factory"));
    gtk_widget_show (m_button_calc_default);
    gtk_fixed_put (GTK_FIXED (fixed_calc), m_button_calc_default, 540, 430);
    gtk_widget_set_size_request (m_button_calc_default, 160, 30);
    g_signal_connect(m_button_calc_default, "clicked", G_CALLBACK(HandleButtonDefaultClicked), this);

    return fixed_calc;
}

const gchar* CalcSetting::GetExamName(void)
{
    return gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_calc));
}

const gchar* CalcSetting::GetDepartmentName(void)
{
     return gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_department_calc));
}

int CalcSetting::GetSequence(const char *exam_type)
{
	ExamItem exam;
	char path11[256];
    sprintf(path11, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path11);
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
	string examType = exam.ReadDefaultProbeDefaultItemName(&ini1);
	char path1[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path1, "%s%s", CFG_RES_PATH, CALC_FILE);
    }
    else
    {
        sprintf(path1, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
    }
    IniFile ini(path1);
    IniFile *ptrIni_calc = &ini;
	int squence = ptrIni_calc->ReadInt(exam_type, "Sequence");
	//printf("===================squence=%d exam_type=%s\n", squence, exam_type);
	return squence;
}

GtkTreeIter CalcSetting::InsertUniqueCalc(GtkTreeModel *model, const char *str)
{
    GtkTreeIter tmp_iter;
    char *strtmp = NULL;
    gboolean has_node = gtk_tree_model_get_iter_first(model, &tmp_iter);
    while (has_node == TRUE) 
    {
        gtk_tree_model_get(model, &tmp_iter, 0, &strtmp, -1);
        if (strcmp(strtmp, str) == 0)
            return tmp_iter;
        else
            has_node = gtk_tree_model_iter_next(model, &tmp_iter);
    }
    return tmp_iter;
}


int DefaultCalc(gpointer data)
{
    CalcSetting::GetInstance()->UpdateAllCalc();
    return 0;
}

int  AddCalc(gpointer data)
{
    CalcSetting::GetInstance()->AddItem();
    return 0;
}

int DeleteCalc(gpointer data)
{
    CalcSetting::GetInstance()->DeleteItem();
    return 0;
}

void CalcSetting::ClearAllCalc()
{
    MeasureMan::GetInstance()->ClearAllValue();
    g_menuCalc.ClearAllData();
    //clear screen 
    KeyClearScreen kcs;
    kcs.Execute();
    g_menuCalc.ChangeAllCalcItems();
    g_menuCalc.ClearAllFlag();
}

void CalcSetting::ButtonSelectOneCalcClicked(GtkButton *button)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_calc)); 
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_item_calc));

    if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE) {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_win_parent), ViewDialog::ERROR, 
                _("Please select a item before move!"), NULL); 
        return;
    }

    char* select_name;
    gtk_tree_model_get(model, &iter, 0, &select_name, -1);
    gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_calc));

    if(!exam_type_name)
        return;
    char exam_type[256];
	ExamItem exam;
    exam.TransItemName(exam_type_name, exam_type);
    vector<string> vecItem_Calc;
    vecItem_Calc.clear();
    CreateItemList_Calc1(exam_type, vecItem_Calc);

    int item_length(0);
    item_length = vecItem_Calc.size();
    if(!vecItem_Calc.empty())
    {
        for(int i=0; i<item_length; i++)
        {
            if( strcmp(select_name, _(vecItem_Calc[i].c_str())) == 0)
            {
                GtkTreeModel *model_tmp;
                GtkTreeSelection *selection;
                GtkTreeIter iter_tmp;
                selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_calc1)); 
                model_tmp= gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_item_calc1));
                 //高亮要插入的词条，并更新滚动条
                iter_tmp= InsertUniqueCalc(model_tmp, select_name);
                gtk_tree_selection_select_iter(selection, &iter_tmp);
            
                GtkTreePath *path_scroll = gtk_tree_model_get_path(model_tmp, &iter_tmp);
                gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(m_treeview_item_calc1), path_scroll, NULL, FALSE, 1.0, 1.0);
                gtk_tree_path_free (path_scroll);
                
                ViewDialog::GetInstance()->Create(GTK_WINDOW(m_win_parent), ViewDialog::INFO, _(" Item has existed. Please select item again!"), NULL);
                return;
            }
        }
    }
  
    char path[256];
    sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path);
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path1[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path1, "%s%s", CFG_RES_PATH, CALC_FILE);
    }
    else
    {
        sprintf(path1, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
    }
    IniFile ini(path1);
    IniFile *ptrIni = &ini; 
    int number;
    char SelectNum[256];
    number = ptrIni->ReadInt(exam_type, "Number");
    
    sprintf(SelectNum, "Calc%d",number+1);
    int select_num = ItemNameTransEtype(select_name);
	if (select_num == -1)
	{
		PRINTF("Fail to ItemNameTransEtype, not exist the etype!\n");
		return;
	}
   
    ptrIni->WriteInt(exam_type, SelectNum, select_num);
    ptrIni->WriteInt(exam_type, "Number", number+1);
    ptrIni->SyncConfigFile();

    GtkTreeModel *new_model1 = create_item_calc_model1();
    gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_calc1), new_model1);
    //高亮插入的词条，并更新滚动条
    GtkTreeSelection *new_selection;
    GtkTreeIter iter_new;
    new_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_calc1)); 
    iter_new= InsertUniqueCalc(new_model1, select_name);
    gtk_tree_selection_select_iter(new_selection, &iter_new);

    GtkTreePath *path_scroll = gtk_tree_model_get_path(new_model1, &iter_new);
    gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(m_treeview_item_calc1), path_scroll, NULL, TRUE, 1.0, 1.0);
    gtk_tree_path_free (path_scroll);

    //更新departent列表
    GtkTreeStore *calculate_store = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_item_calc)));
    gtk_tree_store_remove(calculate_store, &iter);

    if (g_menuCalc.GetExamItem() == exam_type)
        g_menuCalc.ChangeExamItem(exam_type);
}


void CalcSetting::ButtonSelectAllCalcClicked(GtkButton *button)
{    
    gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_calc));

    if(!exam_type_name)
        return;

	// if m_treeview_item_calc is empty
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_item_calc));
	if (gtk_tree_model_iter_n_children(model, NULL) == 0)
	{
		PRINTF("=========== tree view department items is empty!\n");
		return;
	}
    
    char exam_type[256];
    ExamItem exam;
    exam.TransItemName(exam_type_name, exam_type);
      char path11[256];
    sprintf(path11, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini11(path11);
  
    string username;
    username = exam.ReadDefaultUserSelect(&ini11);
    char path[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path, "%s%s", CFG_RES_PATH, CALC_FILE);
    }
    else
    {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni = &ini; 
    
    int Num = ptrIni->ReadInt(exam_type, "Number");
    
    vector<string> vecSelect_Calc;
    vecSelect_Calc.clear();
    CreateItemList_Calc1(exam_type, vecSelect_Calc);

    int select_length(0);
    select_length = vecSelect_Calc.size();

    gchar *department= gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_department_calc));
   char department_name[50];
    for(int i=0; i<SECTION_NUM; i++)
    {
        if(strcmp(department, _(section[i].c_str()))==0)
        {
                strcpy(department_name, section[i].c_str());
                break;
        }
    }

     char path12[256];
     if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path12, "%s%s", CFG_RES_PATH, CALC_ITEM_PATH);
    }
    else
    {
        sprintf(path12, "%s%s%s%s", CFG_RES_PATH, CALC_ITEM_FILE, username.c_str(), ".ini");
    }
   

    IniFile new_ini(path12);
    IniFile *new_ptrIni= &new_ini;
    int number = new_ptrIni->ReadInt(department_name, "Number");
    
    for(int i=1;i<=number;i++)
    {
    int writeCalc=1;
        char CalcNumber[256];
        sprintf(CalcNumber, "Calc%d", i);
        int item_num = new_ptrIni->ReadInt(department_name, CalcNumber);
        string item_name;
        if(item_num < USER_START - BASIC_MEA_END)
        item_name= ItemMenuTransEnglish(item_num);
        else
        {
           item_name = CustomItemTransName(item_num);
        }
        if(!vecSelect_Calc.empty())
        {
            for(int j=0; j<select_length; j++)
            {
                if(strcmp(_(item_name.c_str()), _(vecSelect_Calc[j].c_str())) ==0)
                {
                    writeCalc=0;
                }
            }
        }
        
        if(writeCalc)
        {
            char CalcNum[256];
            sprintf(CalcNum, "Calc%d", ++Num);
            ptrIni->WriteInt(exam_type, CalcNum, item_num);
        }

    }
    ptrIni->WriteInt(exam_type, "Number", Num);
    ptrIni->SyncConfigFile();
    
    //更新Exam Item和 Department 列表
    ChangeModel2();
	
	if (g_menuCalc.GetExamItem() == exam_type)
		g_menuCalc.ChangeExamItem(exam_type);
   
}

void CalcSetting::ButtonBackOneClicked(GtkButton *button)
{
 GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_calc1)); 
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_item_calc1));

    if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE) {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_win_parent), ViewDialog::ERROR, 
                _("Please select a item before move!"), NULL); 
        return;
    }
  GtkTreePath *path = gtk_tree_model_get_path(model, &iter);

    char* select_name;
    gtk_tree_model_get(model, &iter, 0, &select_name, -1);

    gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_calc));

    if(!exam_type_name)
        return;

    char exam_type[256];
    ExamItem exam;
    exam.TransItemName(exam_type_name, exam_type);
     char path11[256];
        sprintf(path11, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
        IniFile ini1(path11);
        string username;
        username = exam.ReadDefaultUserSelect(&ini1);
        char path1[256];
        if(strcmp(username.c_str(), "System Default") == 0)
        {
            sprintf(path1, "%s%s", CFG_RES_PATH, CALC_FILE);
        }
        else
        {
            sprintf(path1, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
        }
        IniFile ini(path1);
        IniFile *ptrIni_calc = &ini; 

     vector<int> vecDelete_Calc;
    vecDelete_Calc.clear();
    CreateItemList_Delete_Calc1(select_name, exam_type, vecDelete_Calc); 

    int item_length(0);
    item_length = vecDelete_Calc.size();
	int squence_copy;
	squence_copy= ptrIni_calc->ReadInt(exam_type, "Sequence");
    ptrIni_calc->RemoveGroup(exam_type);
	ptrIni_calc->SyncConfigFile();

    for(int i=0;i<item_length;i++)
    {
        char CalcNumber[256];
        sprintf(CalcNumber, "Calc%d", i+1);
        ptrIni_calc->WriteInt(exam_type, CalcNumber, vecDelete_Calc[i]);
    }
    
    ptrIni_calc->WriteInt(exam_type, "Number", item_length);
	ptrIni_calc->WriteInt(exam_type, "Sequence", squence_copy);
    ptrIni_calc->SyncConfigFile();

    GtkTreeModel *new_model1 = create_item_calc_model1();
    gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_calc1), new_model1);
    //设置光标，更新滚动条的值
    gtk_tree_view_set_cursor(GTK_TREE_VIEW(m_treeview_item_calc1), path, NULL, TRUE);
    gtk_adjustment_value_changed(gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolledwindow_item_calc1)));
    gtk_tree_path_free (path);

    ChangeModel();

	if (g_menuCalc.GetExamItem() == exam_type)
		g_menuCalc.ChangeExamItem(exam_type);
}


void CalcSetting::ButtonBackAllClicked(GtkButton *button)
{

    gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_calc));
    if(!exam_type_name)
        return;

    char exam_type[256];
	ExamItem exam;
	exam.TransItemName(exam_type_name, exam_type);
	char path[256];
	sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
	IniFile ini1(path);
	string username;
	username = exam.ReadDefaultUserSelect(&ini1);
	char path1[256];
	if(strcmp(username.c_str(), "System Default") == 0)
	{
		sprintf(path1, "%s%s", CFG_RES_PATH, CALC_FILE);
	}
	else
	{
		sprintf(path1, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
	}
	IniFile ini(path1);
	IniFile *ptrIni = &ini; 

	int squence_copy;
	squence_copy= ptrIni->ReadInt(exam_type, "Sequence");

	ptrIni->RemoveGroup(exam_type);

	ptrIni->WriteInt(exam_type, "Number", 0);
	ptrIni->WriteInt(exam_type, "Sequence", squence_copy);
	ptrIni->SyncConfigFile();

	//更新列表
	ChangeModel2();

	if (g_menuCalc.GetExamItem() == exam_type)
		g_menuCalc.ChangeExamItem(exam_type);
}


void CalcSetting::DeleteItem(void)
{
   
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_calc)); 
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_item_calc));

    if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE) {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_win_parent), ViewDialog::ERROR, 
                _("Please select a item before delete!"), NULL); //请先选择待插入结点的父结点
        return;
    }
    GtkTreePath *path = gtk_tree_model_get_path(model, &iter);

    char* select_name;
    gtk_tree_model_get(model, &iter, 0, &select_name, -1);
    gtk_tree_path_free (path);
	int select_num = ItemNameTransEtype(select_name);
	if (select_num == -1)
	{
		PRINTF("Fail to ItemNameTransEtype, not exist the etype!\n");
		return;
	}

    if(select_num < USER_START - BASIC_MEA_END)
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_win_parent), ViewDialog::INFO,  _(" Only Userdefined items can be deleted!"), NULL);
        return;
    }
    char path3[256];
    sprintf(path3, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path3);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);

    const gchar *department_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_department_calc));
    char department[50];
    for(int i=0; i<SECTION_NUM; i++)
    {
        if(strcmp(department_name, _(section[i].c_str()))==0)
        {
            strcpy(department, section[i].c_str());
            break;
        }
    }

    char path1[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path1, "%s%s", CFG_RES_PATH, CALC_ITEM_PATH);
    }
    else
    {
        sprintf(path1, "%s%s%s%s", CFG_RES_PATH, CALC_ITEM_FILE, username.c_str(), ".ini");
    }

    IniFile new_ini(path1);
    IniFile *ptrIni_calc= &new_ini;
 
     vector<int> vecDeleteCalc;
    vecDeleteCalc.clear();
    CreateItemList_Delete_Calc(select_name, department,vecDeleteCalc, ptrIni_calc);
    
    int item_length(0);
    item_length = vecDeleteCalc.size();
    ptrIni_calc->RemoveGroup(department);
	ptrIni_calc->SyncConfigFile();

    for(int i=0;i<item_length;i++)
    {
        char CalcNumber[256];
        sprintf(CalcNumber, "Calc%d", i+1);
        ptrIni_calc->WriteInt(department, CalcNumber, vecDeleteCalc[i]);
    }
    
    ptrIni_calc->WriteInt(department, "Number", item_length);
    ptrIni_calc->SyncConfigFile();

    //同时删除各个检查部位下的词条
     for (int i=0; i<= EXAM_NUM; i++)
    {
        char exam_type[50];
        strcpy(exam_type, ExamItem::ITEM_LIB[i].c_str());

        char path_exam[256];
        if(strcmp(username.c_str(), "System Default") == 0)
        {
            sprintf(path_exam, "%s%s", CFG_RES_PATH, CALC_FILE);
        }
        else
        {
            sprintf(path_exam, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
        }
        IniFile ini(path_exam);
        IniFile *ptrIni_calc1 = &ini; 

		vector<int> vecDelete_Select;
		vecDelete_Select.clear();
		CreateItemList_Delete_Calc1(select_name, exam_type, vecDelete_Select); 

		int item_length(0);
		item_length = vecDelete_Select.size();
		int squence_copy;
		squence_copy= ptrIni_calc1->ReadInt(exam_type, "Sequence");
		ptrIni_calc1->RemoveGroup(exam_type);
		ptrIni_calc1->SyncConfigFile();

		for(int i=0;i<item_length;i++)
		{
			char CalcNumber[256];
			sprintf(CalcNumber, "Calc%d", i+1);
			ptrIni_calc1->WriteInt(exam_type, CalcNumber, vecDelete_Select[i]);
		}

		ptrIni_calc1->WriteInt(exam_type, "Number", item_length);
		ptrIni_calc1->WriteInt(exam_type, "Sequence", squence_copy);
		ptrIni_calc1->SyncConfigFile();
    }

    //同时删除自定义的配置字段
    char CustomEtype[256];
    sprintf(CustomEtype, "CustomEtype-%d", select_num);
	ptrIni_calc->RemoveGroup(CustomEtype);
	ptrIni_calc->SyncConfigFile();

	//更新Exam Item 和Departmen 列表
	ChangeModel2();
    ClearAllCalc();
}

void CalcSetting::ButtonDeleteSelectClicked(GtkButton *button)
{
    if(g_menuCalc.IsFlagExist())
    {
        const char* info = N_("Clicking OK will clear calculated value, whether to cotunue?");

        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_win_parent), ViewDialog::QUESTION, _(info), DeleteCalc);
    }
    else
        DeleteItem();;


}

void CalcSetting::ButtonDownClicked(GtkButton *button)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_calc1)); 
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_item_calc1));

    if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE) {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_win_parent), ViewDialog::ERROR, 
                _("Please select a item before down!"), NULL); 
        return;
    }
    GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
    char *path_string = gtk_tree_path_to_string(path);
    int path_num = atoi(path_string);

    gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_calc));
    char exam_type[256];
    ExamItem exam;
    exam.TransItemName(exam_type_name, exam_type);
    vector<int> vecDelete_Calc;
    vecDelete_Calc.clear();
    CreateItemList_Calc2(exam_type, vecDelete_Calc); 

    int item_length(0);
    item_length = vecDelete_Calc.size();


    int path_total = item_length-1;
    if(path_num != path_total)
    {
        char path11[256];
        sprintf(path11, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
        IniFile ini1(path11);
        ExamItem exam;
        string username;
        username = exam.ReadDefaultUserSelect(&ini1);
        char path1[256];
        if(strcmp(username.c_str(), "System Default") == 0)
        {
            sprintf(path1, "%s%s", CFG_RES_PATH, CALC_FILE);
        }
        else
        {
            sprintf(path1, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
        }
        IniFile ini(path1);
        IniFile *ptrIni_calc = &ini; 

		int squence_copy;
		squence_copy= ptrIni_calc->ReadInt(exam_type, "Sequence");
		ptrIni_calc->RemoveGroup(exam_type);
        ptrIni_calc->SyncConfigFile();

        swap(vecDelete_Calc[path_num], vecDelete_Calc[path_num+1]);

        for(int i=0;i<item_length;i++)
        {
            char CalcNumber[256];
            sprintf(CalcNumber, "Calc%d", i+1);
            ptrIni_calc->WriteInt(exam_type, CalcNumber, vecDelete_Calc[i]);
        }

        ptrIni_calc->WriteInt(exam_type, "Number", item_length);
		ptrIni_calc->WriteInt(exam_type, "Sequence", squence_copy);
        ptrIni_calc->SyncConfigFile();

   /*     GtkTreeIter iter1;
        gtk_tree_model_iter_next(model, &iter1);
        gtk_tree_store_swap(GTK_TREE_STORE(m_treeview_item_calc1), &iter, &iter1);
     */
           //更新树
             GtkTreeModel *new_model1 = create_item_calc_model1();
             gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_calc1), new_model1);

        g_free(path_string);
        gtk_tree_path_next(path);
        gtk_tree_view_set_cursor(GTK_TREE_VIEW(m_treeview_item_calc1), path, NULL, TRUE);
        if(item_length-path_num >2)
            gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(m_treeview_item_calc1), path, NULL, TRUE, 1.0, 1.0);//移动后的选中词条置底
        else
            gtk_adjustment_value_changed(gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolledwindow_item_calc1)));//保证在最下端的词条移动后能够正常显示
        gtk_tree_path_free (path);
    }

	if (g_menuCalc.GetExamItem() == exam_type)
		g_menuCalc.ChangeExamItem(exam_type);
}

void CalcSetting::ButtonUpClicked(GtkButton *button)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_calc1)); 
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_item_calc1));

    if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE) {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_win_parent), ViewDialog::ERROR, 
                _("Please select a item before up!"), NULL); //请先选择待插入结点的父结点
        return;
    }
    GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
    char *path_string = gtk_tree_path_to_string(path);
    int path_num = atoi(path_string);
    PRINTF("---path_string:%s %d\n",path_string,path_num);

    if(path_num == 0)
        return;


    gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_calc));
    char exam_type[256];
    ExamItem exam;
    exam.TransItemName(exam_type_name, exam_type);

    vector<int> vecDelete_Calc;
    vecDelete_Calc.clear();
    CreateItemList_Calc2(exam_type, vecDelete_Calc); 

    int item_length(0);
    item_length = vecDelete_Calc.size();


    char path11[256];
    sprintf(path11, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path11);
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path1[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path1, "%s%s", CFG_RES_PATH, CALC_FILE);
    }
    else
    {
        sprintf(path1, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
    }
    IniFile ini(path1);
    IniFile *ptrIni_calc = &ini; 

	int squence_copy;
	squence_copy= ptrIni_calc->ReadInt(exam_type, "Sequence");
    ptrIni_calc->RemoveGroup(exam_type);
    ptrIni_calc->SyncConfigFile();

    swap(vecDelete_Calc[path_num], vecDelete_Calc[path_num-1]);

    for(int i=0;i<item_length;i++)
    {
        char CalcNumber[256];
        sprintf(CalcNumber, "Calc%d", i+1);
        ptrIni_calc->WriteInt(exam_type, CalcNumber, vecDelete_Calc[i]);
    }

    ptrIni_calc->WriteInt(exam_type, "Number", item_length);
	ptrIni_calc->WriteInt(exam_type, "Sequence", squence_copy);
    ptrIni_calc->SyncConfigFile();

    GtkTreeModel *new_model1 = create_item_calc_model1();
    gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_calc1), new_model1);

    g_free(path_string);
    if(gtk_tree_path_prev(path))
    {
        gtk_tree_view_set_cursor(GTK_TREE_VIEW(m_treeview_item_calc1), path, NULL, TRUE);
        if(item_length-path_num >12)
            gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(m_treeview_item_calc1), path, NULL, TRUE, 0.0, 1.0);// 移动后选中词条置顶
        else
            gtk_adjustment_value_changed(gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolledwindow_item_calc1)));//保证在最上端的词条移动时能够正确显示
    }
    gtk_tree_path_free (path);

	if (g_menuCalc.GetExamItem() == exam_type)
		g_menuCalc.ChangeExamItem(exam_type);
}

gboolean CalcSetting::GetSelectPath(void)
{
    m_vecPath.clear();

    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);

    char path[256];
    char userselectname2[256];
    char path3[256];
    char userselectname3[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path, "%s%s", CFG_RES_PATH, CALC_FILE);
        sprintf(path3, "%s%s", CFG_RES_PATH, CALC_ITEM_PATH);

    }
    else
    {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
        sprintf(path3, "%s%s%s%s", CFG_RES_PATH, CALC_ITEM_FILE, username.c_str(), ".ini");

    }



    m_vecPath.push_back(path);
    m_vecPath.push_back(path3);


    if(!m_vecPath.empty())
        return TRUE;
    else
        return FALSE;


}


static GCancellable* cancellable = NULL;

static void progress_callback(goffset current, goffset total, gpointer data)
{
    if(g_cancellable_is_cancelled(cancellable))
        return;

}


static gboolean LoadData(gpointer data)
{
    int cond = 0;
    int total=1;
    int count =0;
    char str_info[256], result[256];
    FileMan fm;
    PeripheralMan *ptr = PeripheralMan::GetInstance();
     vector<string> vec = CalcSetting::GetInstance()->GetSelectedVec();

     cancellable = g_cancellable_new();

     vector<string>::iterator ite = vec.begin();
     total = vec.size();
     while(ite < vec.end() && !cond)
     {

         GFile *fAbs = g_file_new_for_path((*ite).c_str());
         GFile *fParent = g_file_get_parent(fAbs);
         g_object_unref(fAbs);
         gchar *strDestParent = g_build_path(G_DIR_SEPARATOR_S, UDISK_DATA_PATH, g_file_get_basename(fParent), NULL);
        g_object_unref(fParent);
        GFile *fDest = g_file_new_for_path(strDestParent);

        //create the parent directory
        GError *err_mkdir = NULL;
        if(!g_file_make_directory_with_parents(fDest, NULL, &err_mkdir))
        {
            if(err_mkdir->code!=G_IO_ERROR_EXISTS)
            {
                PRINTF("g_file_make_directory error: %s\n", err_mkdir->message);
                sprintf(result, _("Failed to send data to USB storage!\nError: Failed to create directory."));
                cond = -1;
                g_error_free(err_mkdir);
                g_object_unref(fDest);
                g_free(strDestParent);
                break;
            }
        }
        g_object_unref(fDest);

        gchar *basename = g_path_get_basename((*ite).c_str());
      if(fm.CompareSuffix(basename, "ini") != 0)
        {
            CustomCalc::GetInstance()->SetProgressBar(0);
           count++;
        }

        //Perform copy operation

        gchar *destPath = g_build_path(G_DIR_SEPARATOR_S, strDestParent, basename, NULL);
        g_free(strDestParent);
        g_free(basename);
        PRINTF("Dest Path: %s\n", destPath);
        GFile *src = g_file_new_for_path((*ite).c_str());
        GFile *dest = g_file_new_for_path(destPath);
        g_free(destPath);

        GError *err = NULL;
        int ret = g_file_copy(src, dest, G_FILE_COPY_OVERWRITE, cancellable, progress_callback, NULL, &err);
        g_object_unref(src);
        g_object_unref(dest);
        if(!ret)
        {
            PRINTF("g_file_copy error: %s\n", err->message);
            switch(err->code)
        {
            case G_IO_ERROR_NO_SPACE:
                sprintf(result, _("Failed to send data to USB storage!\nError: No space left on storage."));
                break;
            case G_IO_ERROR_CANCELLED:
                sprintf(result, _("Failed to send data to USB storage!\nError: Operation was cancelled."));
                break;
            default:
                sprintf(result, _("Failed to send data to USB storage!"));
                break;
        }
        cond = -1;
        g_error_free(err);
        break;
        }
        ite++;
    }

    
    //Handle result
    if(!cond)
    {

        UserSelect::GetInstance()->create_exportUSB_dir(export_name);

        char path4[256];
        char path5[256];
#ifdef VET
        sprintf(path4, "%s%s%s%s%s%s", G_DIR_SEPARATOR_S, UDISK_DATA_PATH, "/", export_name,"/", "VetCalcSetting.ini");
        sprintf(path5, "%s%s%s%s%s%s", G_DIR_SEPARATOR_S, UDISK_DATA_PATH, "/", export_name,"/", "VetCalcItemSetting.ini");
#else
        sprintf(path4, "%s%s%s%s%s%s", G_DIR_SEPARATOR_S, UDISK_DATA_PATH, "/", export_name,"/", "CalcSetting.ini");
        sprintf(path5, "%s%s%s%s%s%s", G_DIR_SEPARATOR_S, UDISK_DATA_PATH, "/", export_name,"/", "CalcItemSetting.ini");
#endif
        char path1[256];
        sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
        IniFile ini1(path1);
        ExamItem exam;
        string username;
        username = exam.ReadDefaultUserSelect(&ini1);

        char userselectname[256];
        char userselectname1[256];
        char userselectname2[256];
        char userselectname3[256];
        if(strcmp(username.c_str(), "System Default") == 0)
        {
#ifdef VET
            sprintf(userselectname, "%s%s%s%s", G_DIR_SEPARATOR_S, UDISK_DATA_PATH, "/calcsetting/", "VetDSCalcSetting.ini"); 
            sprintf(userselectname1, "%s%s%s%s", G_DIR_SEPARATOR_S, UDISK_DATA_PATH, "/calcitemsetting/","VetDSCalcItemSetting.ini");  

#else
            sprintf(userselectname, "%s%s%s%s", G_DIR_SEPARATOR_S, UDISK_DATA_PATH, "/calcsetting/", "DSCalcSetting.ini"); 
            sprintf(userselectname1, "%s%s%s%s", G_DIR_SEPARATOR_S, UDISK_DATA_PATH, "/calcitemsetting/","DSCalcItemSetting.ini");  
#endif
        }
        else
        {
            sprintf(userselectname, "%s%s%s%s%s", G_DIR_SEPARATOR_S, UDISK_DATA_PATH, "/calcsetting/",username.c_str(),".ini");  
            sprintf(userselectname1, "%s%s%s%s%s", G_DIR_SEPARATOR_S, UDISK_DATA_PATH, "/calcitemsetting/",username.c_str(),".ini");  
        }

        char exam_type[256];
        char exam_name[256];
        strcpy(exam_name, CalcSetting::GetInstance()->GetExamName());
        exam.TransItemName(exam_name, exam_type);
        PRINTF("exam_type=%s\n", exam_type);
        IniFile ini_tmp(userselectname);
        IniFile *ptrIni_tmp = &ini_tmp;

        char src_group[256];
        vector<string> itemgroup;
        itemgroup = ptrIni_tmp->GetGroupName();
        int group_size(0);
        group_size =itemgroup.size();
        for(int i=0; i<group_size; i++)
        {
            sprintf(src_group ,"%s", itemgroup[i].c_str());
            PRINTF("src_group=%s\n", src_group);

            if(strcmp(src_group,exam_type) != 0)
            {
                PRINTF("src_group_delete=%s\n", src_group);
                ptrIni_tmp->RemoveGroup(src_group);
                ptrIni_tmp->SyncConfigFile();
            }
        }

        FileMan f;
        f.CopyFile(userselectname, path4);
        f.CopyFile(userselectname1, path5);

        sprintf(userselectname2, "%s%s%s", G_DIR_SEPARATOR_S, UDISK_DATA_PATH, "/calcsetting"); 
        sprintf(userselectname3, "%s%s%s", G_DIR_SEPARATOR_S, UDISK_DATA_PATH, "/calcitemsetting");  
        f.DelDirectory(userselectname2);
        f.DelDirectory(userselectname3);


        ptr->UmountUsbStorage();
        gdk_threads_enter();
        CustomCalc::GetInstance()->SetProgressBar(0.2);
        while(gtk_events_pending())
            gtk_main_iteration();
        gdk_threads_leave();	
        usleep(100000);
        gdk_threads_enter();
        CustomCalc::GetInstance()->SetProgressBar(0.4);
        while(gtk_events_pending())
            gtk_main_iteration();
        gdk_threads_leave();	
        usleep(100000);
        gdk_threads_enter();
        CustomCalc::GetInstance()->SetProgressBar(0.6);
        while(gtk_events_pending())
            gtk_main_iteration();
        gdk_threads_leave();	

        usleep(100000);
        gdk_threads_enter();
        CustomCalc::GetInstance()->SetProgressBar(0.8);
        while(gtk_events_pending())
            gtk_main_iteration();
        gdk_threads_leave();	

        usleep(100000);
        CustomCalc::GetInstance()-> ExportRightInfoNotice(_("Success to export to USB storage."));

        gdk_threads_enter();
        CustomCalc::GetInstance()->SetProgressBar(1.0);
        while(gtk_events_pending())
            gtk_main_iteration();
        gdk_threads_leave();	

        usleep(1000000);
        CustomCalc::GetInstance()->OKAndCancelClicked();  
        CustomCalc::GetInstance()-> DelayDestroyWin();
    }else
    {
        CustomCalc::GetInstance()->OKAndCancelClicked();  
        CustomCalc::GetInstance()-> ExportErrorInfoNotice(result);
    }

    return FALSE;
}

static int CancelLoadUSB(gpointer data)
{
    g_cancellable_cancel(cancellable);
    return 0;
}

void CalcSetting::ButtonExportClicked(GtkButton *button)
{
   CustomCalc::GetInstance()->CreateExportCalcSettingWin(CalcSetting::GetInstance()->GetWindow());
}

void CalcSetting::ButtonImportClicked(GtkButton *button)
{
    PeripheralMan *ptr = PeripheralMan::GetInstance();

    if(!ptr->CheckUsbStorageState())
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(CalcSetting::GetInstance()->GetWindow()), 
                ViewDialog::ERROR,
                _("No USB storage found!"), 
                NULL);
        return ;
    }
    else
    {
        if(!ptr->MountUsbStorage())
        {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(CalcSetting::GetInstance()->GetWindow()), 
                    ViewDialog::ERROR,
                    _("Failed to mount USB storage!"), 
                    NULL);
            return ;
        }
    }

    ConfigToHost::GetInstance()->CreateCalcImportWindow(GTK_WINDOW(CalcSetting::GetInstance()->GetWindow()));
}

void CalcSetting::AddItem(void)
{
    m_custom_calc_win.CreateCalcSettingWin(m_win_parent);
}

void CalcSetting::UpdateAllCalc()
{
    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);

    char path[256];
    char userselectname2[256];
    char path3[256];
    char userselectname3[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path, "%s%s", CFG_RES_PATH, CALC_FILE);
        sprintf(path3, "%s%s", CFG_RES_PATH, CALC_ITEM_PATH);

    }
    else
    {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
        sprintf(path3, "%s%s%s%s", CFG_RES_PATH, CALC_ITEM_FILE, username.c_str(), ".ini");

    }
    FileMan f;

    // 计算预设已选所用
    sprintf(userselectname2, "%s%s", CFG_RES_PATH, DEFAULT_CALC_FILE);
    f.CopyFile(userselectname2, path);

    //计算预设词库所用
    sprintf(userselectname3, "%s%s", CFG_RES_PATH, DEFAULT_CALC_ITEM_FILE );
    f.CopyFile(userselectname3, path3);

    //更新Exam Item 和Departmen 列表
    ChangeModel2();
    ClearAllCalc();
}

void CalcSetting::ButtonDefaultClicked(GtkButton *button)
{
    if(g_menuCalc.IsFlagExist())
    {
        const char* info = N_("Clicking OK will clear calculated value, whether to cotunue?");

        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_win_parent), ViewDialog::QUESTION, _(info), DefaultCalc);
    }
    else
    {
        UpdateAllCalc();   
    }
}


void CalcSetting::ButtonAddClicked(GtkButton *button)
{
    if(g_menuCalc.IsFlagExist())
    {
        const char* info = N_("Clicking OK will clear calculated value, whether to cotunue?");

        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_win_parent), ViewDialog::QUESTION, _(info), AddCalc);
    }
    else
       AddItem();
}


//获得exam_type检查部位下所有测量项的item，push到vector中
void CalcSetting::GetCalcListEtype(char *exam_type, vector<int> & vecItemCalc)
{
    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path, "%s%s", CFG_RES_PATH, CALC_FILE);
    }
    else
    {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni= &ini;

    int number;
    number = ptrIni->ReadInt(exam_type, "Number");

    if(number ==0)
        return;

    for(int i=1;i<=number;i++)
    {
        char CalcNumber[256];
        sprintf(CalcNumber, "Calc%d", i);
        int item_num = ptrIni->ReadInt(exam_type, CalcNumber);
        vecItemCalc.push_back(item_num);
    }
}

//获得exam_type检查部位下的测量项总数
int CalcSetting::GetCalcListNum(char *exam_type)
{
    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path, "%s%s", CFG_RES_PATH, CALC_FILE);
    }
    else
    {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni= &ini;
    return  ptrIni->ReadInt(exam_type, "Number");

}

//获得exam_type测量的排序方法 0:none   1:repeat   2:next
int CalcSetting::GetMeasureSequence(const char *exam_type)
{
    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path, "%s%s", CFG_RES_PATH, CALC_FILE);
    }
    else
    {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni= &ini;
    return  ptrIni->ReadInt(exam_type, "Sequence");

}


/*
 *int Etype: 自定义的测量Etype
 *int measure_type: 通过Etype，需要得到的测量方式
 *string calc_name: 通过Etype，需要得到的自定义测量的名称
 */
void CalcSetting::GetCustomCalcMeasure(int Etype, int &measure_type, string &calc_name)
{
  char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path, "%s%s", CFG_RES_PATH, CALC_ITEM_PATH);
    }
    else
    {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, CALC_ITEM_FILE, username.c_str(), ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni = &ini; 
    char CustomEtype[256];
    sprintf(CustomEtype, "CustomEtype-%d",Etype);
    measure_type=ptrIni->ReadInt(CustomEtype, "Method");
    calc_name=ptrIni->ReadString(CustomEtype, "Name");
}

// 返回自定义测量项的最大值
int CalcSetting::GetCalcMaxEtype()
{
    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path, "%s%s", CFG_RES_PATH, CALC_ITEM_PATH);
    }
    else
    {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, CALC_ITEM_FILE, username.c_str(), ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni = &ini; 
    return ptrIni->ReadInt("MaxNumber", "Number");
}

//get department for custom measure
void CalcSetting::GetDepartmentForCustomMeasure(int Etype, string &department)
{
	char path1[256];
	sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
	IniFile ini1(path1);
	ExamItem exam;
	string username;
	username = exam.ReadDefaultUserSelect(&ini1);
	char path[256];
	if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path, "%s%s", CFG_RES_PATH, CALC_ITEM_PATH);
    }
    else
    {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, CALC_ITEM_FILE, username.c_str(), ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni = &ini; 
    char CustomEtype[256];
    sprintf(CustomEtype, "CustomEtype-%d",Etype-BASIC_MEA_END);
    department=ptrIni->ReadString(CustomEtype, "Department");
}


static char type_null[][20]=
{
    {""}
};

static char method_name[][20]=
{
    {N_("Distance")},
    {N_("Length")},
    {N_("Peri")},
    {N_("Area")},
    {N_("Volume")},
    {N_("Angle")},
    {N_("Ratio")},
    {N_("Depth")},
    {N_("EF")},
    {N_("Profile")},
    {N_("Histogram")},
    {N_("Time")},
    {N_("Slope")}, 
    {N_("Vel")}, 
    {N_("HR")}, 
    {N_("Acceleration")}, 
    {N_("PG Mean")}
};

const CustomTypeAndMethod CustomEtypeArray[ETYPE_NUM] = 
{
    {0, N_("Dist-Dot"), DIST_DOT},
    {0, N_("Dist-Line"), DIST_LINE},
#ifndef EMP_322
    {1, N_("Length-Track"), LENGTH_TRACK},
    {1, N_("Length-Dot"), LENGTH_DOT},
#endif
    {1, N_("Integral-Track"), INTEGRAL_TRACK},
    {2, N_("Peri-Track"), PERI_TRACK},
    {2, N_("Peri-Ellipse"), PERI_ELLIPSE},
    {3, N_("Area-Track"), AREA_TRACK},
#ifndef EMP_322
    {3, N_("Area-Dot"), AREA_TRACK},
#endif
    {3, N_("Area-Rectangle"), AREA_REC},
    {3, N_("Area-Ellipse"), AREA_ELLIPSE},
    {4, N_("Vol-3Axis"), VOL_3AXIS},
    {4, N_("Vol-Ellipse1"), VOL_ELLIPSE1},
    {4, N_("Vol-Ellipse2"), VOL_ELLIPSE2},
    {5, N_("Angle-3Dot"), ANGLE_3DOT},
    {5, N_("Angle-2Line"), ANGLE_2LINE},
    {6, N_("Angusty-Dist-Dot"), ANGUSTY_DIST_DOT},
    {6, N_("Ratio-Dist-Dot"), RATIO_DIST_DOT},
    {6, N_("Angusty-Area"), ANGUSTY_AREA},
    {6, N_("Ratio-Area"), RATIO_AREA},
    {6, N_("Ratio-D-P"), RATIO_D_P},
    {6, N_("Ratio-Vol"), RATIO_VOL},
    {7, N_("Depth-Dist"), DEPTH_DIST},
    {7, N_("Depth-Dist-M"), DEPTH_DIST_M},
    {8, N_("EF"), EF},
    {9, N_("Profile"), PROFILE},
    {10, N_("Histogram"), HISTOGRAM},
    {11, N_("Time-M"), TIME_M},
#ifndef EMP_322
    {11, N_("Time-D"), TIME_D},
#endif
    {12, N_("Slope"), SLOPE},
    {13, N_("Vel-M"), VEL_M},
#ifdef EMP_322
    {14, N_("HR-M"), HR_M}
#else
    {13, N_("Vel-D"), VEL_D},
    {14, N_("HR-M"), HR_M},
#endif
#ifndef EMP_322
    {15, N_("Acceleration"), ACCEL},
    {16, N_("PG Mean"), PGMEAN}
#endif
};

/*

const CustomTypeAndMethod CustomEtypeArray[ETYPE_NUM] = 
{
    {0, N_("DIST_DOT"), DIST_DOT},
    {0, N_("DIST_LINE"), DIST_LINE},
    {1, N_("LENGTH_TRACK"), LENGTH_TRACK},
    {1, N_("LENGTH_DOT"), LENGTH_DOT},
    {1, N_("INTEGRAL_TRACK"), INTEGRAL_TRACK},
    {2, N_("PERI_TRACK"), PERI_TRACK},
    {2, N_("PERI_ELLIPSE"), PERI_ELLIPSE},
    {3, N_("AREA_TRACK"), AREA_TRACK},
    {3, N_("AREA_DOT"), AREA_TRACK},
    {3, N_("AREA_REC"), AREA_REC},
    {3, N_("AREA_ELLIPSE"), AREA_ELLIPSE},
    {4, N_("VOL_3AXIS"), VOL_3AXIS},
    {4, N_("VOL_ELLIPSE1"), VOL_ELLIPSE1},
    {4, N_("VOL_ELLIPSE2"), VOL_ELLIPSE2},
    {5, N_("ANGLE_3DOT"), ANGLE_3DOT},
    {5, N_("ANGLE_2LINE"), ANGLE_2LINE},
    {6, N_("ANGUSTY_DIST_DOT"), ANGUSTY_DIST_DOT},
    {6, N_("RATIO_DIST_DOT"), RATIO_DIST_DOT},
    {6, N_("ANGUSTY_AREA"), ANGUSTY_AREA},
    {6, N_("RATIO_AREA"), RATIO_AREA},
    {6, N_("RATIO_D_P"), RATIO_D_P},
    {6, N_("RATIO_VOL"), RATIO_VOL},
    {7, N_("DEPTH_DIST"), DEPTH_DIST},
    {7, N_("DEPTH_DIST_M"), DEPTH_DIST_M},
    {8, N_("EF"), EF},
    {9, N_("PROFILE"), PROFILE},
    {10, N_("HISTOGRAM"), HISTOGRAM},
    {11, N_("TIME_M"), TIME_M},
    {11, N_("TIME_D"), TIME_D},
    {12, N_("SLOPE"), SLOPE},
    {13, N_("VEL_M"), VEL_M},
    {13, N_("VEL_D"), VEL_D},
    {14, N_("HR_M"), HR_M},
    {15, N_("ACCEL"), ACCEL},
    {16, N_("PGMEAN"), PGMEAN},
};
*/

/*

const char *custom_method[][6]=
{
    {N_("DIST_DOT"),N_("DIST_LINE")},
    {"LENGTH_TRACK", "LENGTH_DOT", "INTEGRAL_TRACK"},
    {"PERI_TRACK", "PERI_ELLIPSE"},
    {"AREA_TRACK", "AREA_DOT", "AREA_REC", "AREA_ELLIPSE" },
    {"VOL_3AXIS", "VOL_ELLIPSE1", "VOL_ELLIPSE2" },
    {"ANGLE_3DOT", "ANGLE_2LINE"},
    {"ANGUSTY_DIST_DOT", "RATIO_DIST_DOT", "ANGUSTY_AREA", "RATIO_AREA", "RATIO_D_P", "RATIO_VOL"},
    {"DEPTH_DIST", "DEPTH_DIST_M"},
    {"EF"},
    {"PROFILE"},
    {"HISTOGRAM"},
    {"TIME_M", "TIME_D"},
    {"SLOPE"},
    {"VEL_M", "VEL_D"},
    {"HR_M"},
    {"ACCEL"},
    {"PGMEAN"}
};

const CustomEtype CustomEtypeArray[ETYPE_NUM] = 
{
    {"DIST_DOT",DIST_DOT},
    {"DIST_LINE",DIST_LINE},
    {"LENGTH_TRACK", LENGTH_TRACK},
    {"LENGTH_DOT",LENGTH_DOT},
    {"INTEGRAL_TRACK", INTEGRAL_TRACK},
    {"PERI_TRACK",PERI_TRACK},
    {"PERI_ELLIPSE", PERI_ELLIPSE},
    {"AREA_TRACK",AREA_TRACK},
    {"AREA_DOT",AREA_TRACK},
    {"AREA_REC",AREA_REC},
    {"AREA_ELLIPSE", AREA_ELLIPSE},
    {"VOL_3AXIS",VOL_3AXIS},
    {"VOL_ELLIPSE1",VOL_ELLIPSE1},
    {"VOL_ELLIPSE2",VOL_ELLIPSE2},
    {"ANGLE_3DOT",ANGLE_3DOT},
    {"ANGLE_2LINE", ANGLE_2LINE},
    {"ANGUSTY_DIST_DOT",ANGUSTY_DIST_DOT},
    {"RATIO_DIST_DOT",RATIO_DIST_DOT},
    {"ANGUSTY_AREA", ANGUSTY_AREA},
    {"RATIO_AREA", RATIO_AREA},
    {"RATIO_D_P",RATIO_D_P},
    {"RATIO_VOL", RATIO_VOL},
    {"DEPTH_DIST",DEPTH_DIST},
    {"DEPTH_DIST_M", DEPTH_DIST_M},
    {"EF", EF},
    {"PROFILE", PROFILE},
    {"HISTOGRAM", HISTOGRAM},
    {"TIME_M", TIME_M},
    {"TIME_D",TIME_D},
    {"SLOPE",SLOPE},
    {"VEL_M",VEL_M},
    {"VEL_D", VEL_D},
    {"HR_M", HR_M},
    {"ACCEL", ACCEL},
    {"PGMEAN", PGMEAN},
};
*/
CustomCalc* CustomCalc::m_ptrInstance = NULL;

CustomCalc::CustomCalc()
{
}

CustomCalc::~CustomCalc()
{
    if (m_ptrInstance != NULL)
        delete m_ptrInstance;
}

CustomCalc* CustomCalc::GetInstance()
{
    if (m_ptrInstance == NULL)
        m_ptrInstance = new CustomCalc;
    return m_ptrInstance;
}

GtkWidget* CustomCalc::CreateExportCalcSettingWin(GtkWidget *parent)
{
    GtkWidget *window_calc_export;
    GtkWidget *fixed1;
   // GtkWidget *button_ok;
    GtkWidget *alignment1;
    GtkWidget *hbox2;
    GtkWidget *image1;
    GtkWidget *label_OK;
    GtkWidget *hseparator1;
  //  GtkWidget *button_cancel;
    GtkWidget *alignment2;
    GtkWidget *hbox3;
    GtkWidget *image2;
    GtkWidget *label_Cancel;
    GtkWidget *label_type;
    GtkWidget *label_method;
    GtkWidget *label_name;
    GtkWidget *label_name_notice;
    GtkWidget *label_notice;

    window_calc_export = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (window_calc_export, 400, 420-80-40);
    gtk_window_set_title (GTK_WINDOW (window_calc_export), _("Export"));
    gtk_window_set_transient_for(GTK_WINDOW(window_calc_export), GTK_WINDOW(parent));
    gtk_window_set_modal(GTK_WINDOW(window_calc_export), TRUE);
    gtk_window_set_position (GTK_WINDOW (window_calc_export), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable (GTK_WINDOW (window_calc_export), FALSE);

    fixed1 = gtk_fixed_new ();
    gtk_widget_show (fixed1);
    gtk_container_add (GTK_CONTAINER (window_calc_export), fixed1);

	label_name_notice = gtk_label_new (_("Please input name for exporting data:"));
	gtk_misc_set_alignment (GTK_MISC(label_name_notice), 0, 0.5);
	gtk_label_set_use_markup (GTK_LABEL (label_name_notice), TRUE);
	gtk_widget_show (label_name_notice);
	gtk_fixed_put (GTK_FIXED (fixed1), label_name_notice, 20, 10);
	gtk_widget_set_size_request (label_name_notice, 250, 30);

	label_name = gtk_label_new (_("Data Name:"));
	gtk_misc_set_alignment (GTK_MISC(label_name), 0, 0.5);
	gtk_label_set_use_markup (GTK_LABEL (label_name), TRUE);
	gtk_widget_show (label_name);
	gtk_fixed_put (GTK_FIXED (fixed1), label_name, 20, 50);
	gtk_widget_set_size_request (label_name, 85, 30);

     m_entry_export_name = gtk_entry_new ();
     gtk_entry_set_text(GTK_ENTRY(m_entry_export_name), CalcSetting::GetInstance()->GetExamName());
     gtk_entry_set_max_length(GTK_ENTRY(m_entry_export_name), 30);
     gtk_widget_show (m_entry_export_name);
     gtk_fixed_put (GTK_FIXED (fixed1), m_entry_export_name, 115, 50);
     gtk_widget_set_size_request (m_entry_export_name, 200, 30);
     gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_export_name), 9679);
     gtk_widget_grab_focus(m_entry_export_name);
     g_signal_connect_after(G_OBJECT(m_entry_export_name), "focus-in-event", G_CALLBACK(on_entry_focus_in), this);

	 label_notice = gtk_label_new (_("Please insert USB storage before export! \nClick Export will export current exam data to USB storage!"));
	 gtk_misc_set_alignment (GTK_MISC(label_notice), 0, 0.5);
	 gtk_label_set_use_markup (GTK_LABEL (label_notice), TRUE);
	 gtk_widget_show (label_notice);
     gtk_fixed_put (GTK_FIXED (fixed1), label_notice, 20, 80);
     gtk_widget_set_size_request (label_notice, 300, 60);

     m_progress_bar = gtk_progress_bar_new();
     gtk_widget_hide(m_progress_bar);
     gtk_fixed_put(GTK_FIXED(fixed1), m_progress_bar, 60, 150);
     gtk_widget_set_size_request(m_progress_bar, 280, 20);

     img_right = gtk_image_new_from_stock (GTK_STOCK_APPLY, GTK_ICON_SIZE_BUTTON);
     gtk_widget_hide (img_right);
     gtk_fixed_put (GTK_FIXED (fixed1), img_right, 40, 180);

     img_error = gtk_image_new_from_stock (GTK_STOCK_STOP, GTK_ICON_SIZE_BUTTON);
     gtk_widget_hide (img_error);
     gtk_fixed_put (GTK_FIXED (fixed1), img_error, 40, 180);

     img_load = gtk_image_new_from_stock (GTK_STOCK_YES, GTK_ICON_SIZE_BUTTON);
     gtk_widget_hide (img_load);
     gtk_fixed_put (GTK_FIXED (fixed1), img_load, 40, 180);

     m_label_notice = gtk_label_new ("");
     gtk_widget_hide (m_label_notice);
     gtk_fixed_put (GTK_FIXED (fixed1), m_label_notice, 55, 180);
     gtk_widget_set_size_request (m_label_notice, 265, 30);

     hseparator1 = gtk_hseparator_new ();
     gtk_widget_show (hseparator1);
     gtk_fixed_put (GTK_FIXED (fixed1), hseparator1, 20, 220);
     gtk_widget_set_size_request (hseparator1, 360, 16);

   GtkWidget *imageOK = gtk_image_new_from_stock (GTK_STOCK_OPEN, GTK_ICON_SIZE_BUTTON);
    GtkWidget *labelOK = gtk_label_new_with_mnemonic (_("Export"));
    button_ok = create_button_icon(labelOK, imageOK);
    gtk_fixed_put (GTK_FIXED (fixed1), button_ok, 136, 360-80-35);
    gtk_widget_set_size_request (button_ok, 100, 30);
    g_signal_connect ( G_OBJECT(button_ok), "clicked", G_CALLBACK(HandleButtonExportNameOKClicked), this);

    GtkWidget *imageCancel = gtk_image_new_from_stock (GTK_STOCK_CANCEL, GTK_ICON_SIZE_BUTTON);
    GtkWidget *labelCancel = gtk_label_new_with_mnemonic (_("Cancel"));
    button_cancel = create_button_icon(labelCancel, imageCancel);
    gtk_fixed_put (GTK_FIXED (fixed1), button_cancel, 280, 360-80-35);
    gtk_widget_set_size_request (button_cancel, 100, 30);
    g_signal_connect ( G_OBJECT(button_cancel), "clicked", G_CALLBACK(HandleButtonCancelClicked), this);

    gtk_widget_show_all(window_calc_export);
    gtk_widget_hide (img_right);
    gtk_widget_hide (img_error);
    gtk_widget_hide (img_load);
    m_window = window_calc_export;
     g_signal_connect(G_OBJECT(m_window), "delete-event", G_CALLBACK(on_window_delete_event), this); 
     g_keyInterface.Push(this);
     SetSystemCursorToCenter();
     return window_calc_export;
}


GtkWidget* CustomCalc::CreateCalcSettingWin(GtkWidget *parent)
{
    GtkWidget *window_calc_add;
    GtkWidget *button_ok;
    GtkWidget *alignment1;
    GtkWidget *hbox2;
    GtkWidget *image1;
    GtkWidget *label_OK;
    GtkWidget *hseparator1;
    GtkWidget *button_cancel;
    GtkWidget *alignment2;
    GtkWidget *hbox3;
    GtkWidget *image2;
    GtkWidget *label_Cancel;
    GtkWidget *label_name;

    window_calc_add = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (window_calc_add, 400, 280);
    gtk_window_set_title (GTK_WINDOW (window_calc_add), _("CalcSetting"));
    gtk_window_set_transient_for(GTK_WINDOW(window_calc_add), GTK_WINDOW(parent));
    gtk_window_set_modal(GTK_WINDOW(window_calc_add), TRUE);
    gtk_window_set_position (GTK_WINDOW (window_calc_add), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable (GTK_WINDOW (window_calc_add), FALSE);

    fixed1 = gtk_fixed_new ();
    gtk_widget_show (fixed1);
    gtk_container_add (GTK_CONTAINER (window_calc_add), fixed1);


    label_name = gtk_label_new (_("Item:")); 
    gtk_widget_show (label_name);
    gtk_fixed_put (GTK_FIXED (fixed1), label_name, 8, 24);
    gtk_widget_set_size_request (label_name, 120, 30);

    m_entry_name = gtk_entry_new ();
    gtk_entry_set_max_length(GTK_ENTRY(m_entry_name), 10);
    gtk_widget_show (m_entry_name);
    gtk_fixed_put (GTK_FIXED (fixed1), m_entry_name, 152, 24);
    gtk_widget_set_size_request (m_entry_name, 200, 30);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_name), 9679);
    gtk_widget_grab_focus(m_entry_name);
    g_signal_connect_after(G_OBJECT(m_entry_name), "focus-in-event", G_CALLBACK(on_entry_focus_in), this);

    label_type = gtk_label_new (_("Type:"));
    gtk_widget_show (label_type);
    gtk_fixed_put (GTK_FIXED (fixed1), label_type, 8, 80-8);
    gtk_widget_set_size_request (label_type, 120, 30);

    m_combobox_type = gtk_combo_box_new_text();
    gtk_widget_show(m_combobox_type);
    gtk_fixed_put (GTK_FIXED (fixed1), m_combobox_type, 152, 80-8);
    gtk_widget_set_size_request (m_combobox_type, 200, 30);
    for(int i=0; i<MAX_METHOD; i++)
    {
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_type), _(method_name[i]));
    }
	gtk_combo_box_set_active(GTK_COMBO_BOX (m_combobox_type), 0);
    g_signal_connect(m_combobox_type, "changed", G_CALLBACK(HandleComboboxTypeChanged), this); 



    label_method = gtk_label_new (_("Method:"));
    gtk_widget_show (label_method);
    gtk_fixed_put (GTK_FIXED (fixed1), label_method, 8, 136);
    gtk_widget_set_size_request (label_method, 120, 30);

    m_combobox_method = gtk_combo_box_new_text();
    gtk_widget_show (m_combobox_method);
	gtk_fixed_put (GTK_FIXED (fixed1), m_combobox_method, 152, 136);
	gtk_widget_set_size_request (m_combobox_method, 200, 30);

    for(int i=0;i<ETYPE_NUM;i++)
    {
        if(0 == CustomEtypeArray[i].type) 
        {
            gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_method), _(CustomEtypeArray[i].name.c_str()));
        }
    }
    /*
    for (int j = 0; j < 6; j++)
	{
		if(custom_method[0][j]==NULL)
			break;
		gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_method), _(custom_method[0][j]));
	}*/

	gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_method), 0);

    button_ok = gtk_button_new ();
    gtk_widget_show (button_ok);
    gtk_fixed_put (GTK_FIXED (fixed1), button_ok, 136, 224);
    gtk_widget_set_size_request (button_ok, 100, 30);
    g_signal_connect(button_ok, "clicked", G_CALLBACK(HandleButtonOKClicked), this);

    alignment1 = gtk_alignment_new (0.5, 0.5, 0, 0);
    gtk_widget_show (alignment1);
    gtk_container_add (GTK_CONTAINER (button_ok), alignment1);

    hbox2 = gtk_hbox_new (FALSE, 2);
    gtk_widget_show (hbox2);
    gtk_container_add (GTK_CONTAINER (alignment1), hbox2);

    image1 = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_BUTTON);
    gtk_widget_show (image1);
    gtk_box_pack_start (GTK_BOX (hbox2), image1, FALSE, FALSE, 0);

    label_OK = gtk_label_new_with_mnemonic (_("OK"));
    gtk_widget_show (label_OK);
    gtk_box_pack_start (GTK_BOX (hbox2), label_OK, FALSE, FALSE, 0);

    hseparator1 = gtk_hseparator_new ();
    gtk_widget_show (hseparator1);
    gtk_fixed_put (GTK_FIXED (fixed1), hseparator1, 8, 200);
    gtk_widget_set_size_request (hseparator1, 384, 16);

    button_cancel = gtk_button_new ();
    gtk_widget_show (button_cancel);
    gtk_fixed_put (GTK_FIXED (fixed1), button_cancel, 280, 224);
    gtk_widget_set_size_request (button_cancel, 100, 30);
    g_signal_connect(button_cancel, "clicked", G_CALLBACK(HandleButtonCancelClicked), this);

    alignment2 = gtk_alignment_new (0.5, 0.5, 0, 0);
    gtk_widget_show (alignment2);
    gtk_container_add (GTK_CONTAINER (button_cancel), alignment2);

    hbox3 = gtk_hbox_new (FALSE, 2);
    gtk_widget_show (hbox3);
    gtk_container_add (GTK_CONTAINER (alignment2), hbox3);

    image2 = gtk_image_new_from_stock ("gtk-cancel", GTK_ICON_SIZE_BUTTON);
    gtk_widget_show (image2);
    gtk_box_pack_start (GTK_BOX (hbox3), image2, FALSE, FALSE, 0);

    label_Cancel = gtk_label_new_with_mnemonic (_("Cancel"));
    gtk_widget_show (label_Cancel);
    gtk_box_pack_start (GTK_BOX (hbox3), label_Cancel, FALSE, FALSE, 0);

    gtk_widget_show_all(window_calc_add);
    m_window = window_calc_add;
    g_signal_connect(G_OBJECT(m_window), "delete-event", G_CALLBACK(on_window_delete_event), this); 

    g_keyInterface.Push(this);
    SetSystemCursorToCenter();

    return window_calc_add;
}

void CustomCalc::ComboboxTypeChanged(GtkComboBox *combobox)
{

    int active = gtk_combo_box_get_active(GTK_COMBO_BOX(combobox));
    if (active ==-1)
        return ;
   
	int i = 0;
	for(i = 5; i >= 0; i--)
		gtk_combo_box_remove_text(GTK_COMBO_BOX(m_combobox_method), i);
/*
    for (i = 0; i < 6; i++)
    {
        if(custom_method[active][i]==NULL)
            break;
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_method), _(custom_method[active][i]));
    }
*/

  for(int i=0;i<ETYPE_NUM;i++)
    {
        if(active == CustomEtypeArray[i].type) 
        {
            gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_method), _(CustomEtypeArray[i].name.c_str()));
        }
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_method), 0);

}

void CustomCalc::EntryItemInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position)
{
    gint old_text_length = strlen(gtk_entry_get_text(GTK_ENTRY(editable)));
    if(old_text_length + new_text_length > gtk_entry_get_max_length(GTK_ENTRY(editable))) {
        gtk_signal_emit_stop(GTK_OBJECT(editable), g_signal_lookup("insert-text", GTK_TYPE_EDITABLE));
    }
}

void CustomCalc::EntryItemFocusIn(GtkEditable *editable, GdkEventFocus *event)
{
    if (KeySwitchIM::m_imOn)
    {
        KeySwitchIM ksim;
        ksim.ExcuteChange(TRUE);
    }
}


void CustomCalc::ButtonCancelClicked(GtkButton *button)
{
    DestroyWin();
}


void CustomCalc::HideOKAndCancelClicked()
{
    gtk_widget_set_sensitive(button_ok, FALSE);
    gtk_widget_set_sensitive(button_cancel, FALSE);
}

void CustomCalc::OKAndCancelClicked()
{
    gtk_widget_set_sensitive(button_ok, TRUE);
    gtk_widget_set_sensitive(button_cancel, TRUE);

}

void CustomCalc::SetProgressBar(double fraction)
{
    //gtk_progress_bar_set_pulse_step(GTK_PROGRESS_BAR(m_progress_bar), 0.1);
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(m_progress_bar), fraction);
    char text_buf[10];
    sprintf(text_buf, "%d%%", (int)(fraction*100));
    gtk_progress_bar_set_text(GTK_PROGRESS_BAR(m_progress_bar), text_buf);
    gtk_widget_show(m_progress_bar);
}


void CustomCalc::ExportRightInfoNotice(char *result)
{
    gtk_label_set_text(GTK_LABEL(m_label_notice), (_(result)));
    gtk_label_set_line_wrap_mode(GTK_LABEL(m_label_notice), PANGO_WRAP_WORD);
    gtk_widget_hide (img_error);
    gtk_widget_hide (img_load);
    gtk_widget_show(m_label_notice);   
    gtk_widget_show(img_right);
}

void CustomCalc::ExportErrorInfoNotice(char *result)
{
    gtk_label_set_text(GTK_LABEL(m_label_notice), (_(result)));
    gtk_label_set_line_wrap_mode(GTK_LABEL(m_label_notice), PANGO_WRAP_WORD);
    gtk_widget_hide (img_right);
    gtk_widget_hide (img_load);
    gtk_widget_show(m_label_notice);   
    gtk_widget_show(img_error);
}

void CustomCalc::ExportLoadInfoNotice(char *result)
{
    gtk_label_set_text(GTK_LABEL(m_label_notice), (_(result)));
    gtk_label_set_line_wrap_mode(GTK_LABEL(m_label_notice), PANGO_WRAP_WORD);
    gtk_widget_hide (img_right);
    gtk_widget_hide (img_error);
    gtk_widget_show(m_label_notice);   
    gtk_widget_show(img_load);

}
extern int num;
extern vector<int> vecAdd;
extern int item_num_exist;

bool CustomCalc::RenameCompare(char * name_copy)
{
    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char userselectname1[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(userselectname1, "%s%s", CFG_RES_PATH, CALC_ITEM_PATH);
    }
    else
    {
        sprintf(userselectname1, "%s%s%s%s", CFG_RES_PATH, CALC_ITEM_FILE, username.c_str(), ".ini");
    }

    IniFile ini_add4(userselectname1);
    IniFile *ptrIni_add4 = &ini_add4; 
    vector<string> useritemgroup;
    useritemgroup = ptrIni_add4->GetGroupName();
    char src_group[256];
    int group_length(0);
    group_length = useritemgroup.size();
    for (int i= 0 ; i <  group_length; i++)
    {
        sprintf(src_group ,"%s", useritemgroup[i].c_str());
        int number1;
        number1 = ptrIni_add4->ReadInt(src_group, "Number");

        for(int i=1;i<=number1;i++)
        {
            char CalcNumber[256];
            sprintf(CalcNumber, "Calc%d", i);
            int item_num2 = ptrIni_add4->ReadInt(src_group, CalcNumber);
            string item_name1;
            if(item_num2 < (USER_START - BASIC_MEA_END))
                item_name1= CalcSetting::GetInstance()->ItemMenuTransEnglish(item_num2);
            else
            {
                item_name1 = CalcSetting::GetInstance()->CustomItemTransName(item_num2);
            }
            if(strcmp(item_name1.c_str(),name_copy) == 0)
            {
                return false; 

            }
        }
    }
    return true;
}

void CustomCalc::ImportWrite(string item_name, int &item_num)
{
    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path2[256];
    char path3[256];
    char userselectname[256];
    char userselectname1[256];
#ifdef VET
    sprintf(path2, "%s%s", CALC_TMP_DATA_PATH, "/VetCalcSetting.ini"); 
    sprintf(path3, "%s%s", CALC_TMP_DATA_PATH, "/VetCalcItemSetting.ini"); 
#else
    sprintf(path2, "%s%s", CALC_TMP_DATA_PATH, "/CalcSetting.ini"); 
    sprintf(path3, "%s%s", CALC_TMP_DATA_PATH, "/CalcItemSetting.ini"); 
#endif
    FileMan f;
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(userselectname, "%s%s", CFG_RES_PATH, CALC_FILE);
        sprintf(userselectname1, "%s%s", CFG_RES_PATH, CALC_ITEM_PATH);
    }
    else
    {
        sprintf(userselectname, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
        sprintf(userselectname1, "%s%s%s%s", CFG_RES_PATH, CALC_ITEM_FILE, username.c_str(), ".ini");
    }

    IniFile ini_add2(userselectname1);
    IniFile *ptrIni_add2 = &ini_add2; 
    int ItemAllNum;
    ItemAllNum=ptrIni_add2->ReadInt("MaxNumber", "Number");
    int ItemDeleteNum=ptrIni_add2->ReadInt("Delete", "Number");

    IniFile ini_add1(path3);
    IniFile *ptrIni_add1 = &ini_add1; 
    char CustomEtype[256];
    sprintf(CustomEtype, "CustomEtype-%d",item_num);
    int method_index = ptrIni_add1->ReadInt(CustomEtype, "Method");
    int type_index = ptrIni_add1->ReadInt(CustomEtype, "MeasureType");
    string department_tmp = ptrIni_add1->ReadString(CustomEtype, "Department");
    char department_in[256];
    strcpy(department_in, department_tmp.c_str());
    if(ItemDeleteNum)
    {
        char CalcNumber1[256];
        sprintf(CalcNumber1, "Calc%d", ItemDeleteNum);
        int item_num1=ptrIni_add2->ReadInt("Delete", CalcNumber1);
        ptrIni_add2->RemoveString("Delete", CalcNumber1);
        ptrIni_add2->WriteInt("Delete", "Number", ItemDeleteNum-1);
        ptrIni_add2->SyncConfigFile();

        char CustomEtype2[256];
        sprintf(CustomEtype2, "CustomEtype-%d",item_num1);
        ptrIni_add2->WriteInt(CustomEtype2, "Etype", item_num1);
        ptrIni_add2->WriteString(CustomEtype2, "Name", item_name.c_str());
        ptrIni_add2->WriteInt(CustomEtype2, "Method", method_index);
        ptrIni_add2->WriteInt(CustomEtype2, "MeasureType", type_index);
        ptrIni_add2->WriteString(CustomEtype2, "Department", department_in);
        ptrIni_add2->SyncConfigFile();
        ItemAllNum = item_num1;
        item_num =item_num1;
    } 
    else
    {
        char CustomEtype1[256];
        sprintf(CustomEtype1, "CustomEtype-%d",++ItemAllNum);
        ptrIni_add2->WriteInt(CustomEtype1, "Etype", ItemAllNum);
        ptrIni_add2->WriteString(CustomEtype1, "Name", item_name.c_str());
        ptrIni_add2->WriteInt(CustomEtype1, "Method", method_index);
        ptrIni_add2->WriteInt(CustomEtype1, "MeasureType", type_index);
        ptrIni_add2->WriteInt("MaxNumber", "Number", ItemAllNum);
        ptrIni_add2->WriteString(CustomEtype1, "Department", department_in);
        ptrIni_add2->SyncConfigFile();
        item_num = ItemAllNum;
    }
    int number;
    char SelectNum[256];
    number = ptrIni_add2->ReadInt(department_in, "Number");
    sprintf(SelectNum, "Calc%d",number+1);
    ptrIni_add2->WriteInt(department_in, SelectNum, ItemAllNum);
    ptrIni_add2->WriteInt(department_in, "Number", number+1);
    ptrIni_add2->SyncConfigFile();

    vecAdd.push_back(item_num);
}

void CustomCalc::ImportRenameCopy(string item_name)
{

    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path2[256];
    char path3[256];
    char userselectname[256];
    char userselectname1[256];
#ifdef VET
    sprintf(path2, "%s%s", CALC_TMP_DATA_PATH, "/VetCalcSetting.ini"); 
sprintf(path3, "%s%s", CALC_TMP_DATA_PATH, "/VetCalcItemSetting.ini"); 

#else
    sprintf(path2, "%s%s", CALC_TMP_DATA_PATH, "/CalcSetting.ini"); 
    sprintf(path3, "%s%s", CALC_TMP_DATA_PATH, "/CalcItemSetting.ini"); 
#endif
    FileMan f;
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(userselectname, "%s%s", CFG_RES_PATH, CALC_FILE);
        sprintf(userselectname1, "%s%s", CFG_RES_PATH, CALC_ITEM_PATH);
    }
    else
    {
        sprintf(userselectname, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
        sprintf(userselectname1, "%s%s%s%s", CFG_RES_PATH, CALC_ITEM_FILE, username.c_str(), ".ini");
    }

    IniFile ini_add2(userselectname1);
    IniFile *ptrIni_add2 = &ini_add2; 
	int ItemAllNum;
	ItemAllNum=ptrIni_add2->ReadInt("MaxNumber", "Number");
	int ItemDeleteNum=ptrIni_add2->ReadInt("Delete", "Number");

	IniFile ini_add1(path3);
	IniFile *ptrIni_add1 = &ini_add1; 
	char CustomEtype[256];
	sprintf(CustomEtype, "CustomEtype-%d",item_num_exist);
    int method_index = ptrIni_add1->ReadInt(CustomEtype, "Method");
    int type_index = ptrIni_add1->ReadInt(CustomEtype, "MeasureType");
    string department_tmp = ptrIni_add1->ReadString(CustomEtype, "Department");
    char department_in[256];
    strcpy(department_in, department_tmp.c_str());
    if(ItemDeleteNum)
    {
        char CalcNumber1[256];
        sprintf(CalcNumber1, "Calc%d", ItemDeleteNum);
        int item_num1=ptrIni_add2->ReadInt("Delete", CalcNumber1);
        ptrIni_add2->RemoveString("Delete", CalcNumber1);
        ptrIni_add2->WriteInt("Delete", "Number", ItemDeleteNum-1);
        ptrIni_add2->SyncConfigFile();

        char CustomEtype2[256];
        sprintf(CustomEtype2, "CustomEtype-%d",item_num1);
        ptrIni_add2->WriteInt(CustomEtype2, "Etype", item_num1);
        ptrIni_add2->WriteString(CustomEtype2, "Name", item_name.c_str());
        ptrIni_add2->WriteInt(CustomEtype2, "Method", method_index);
        ptrIni_add2->WriteInt(CustomEtype2, "MeasureType", type_index);
        ptrIni_add2->WriteString(CustomEtype2, "Department", department_in);
        ptrIni_add2->SyncConfigFile();
        ItemAllNum = item_num1;
        item_num_exist =item_num1;
    } 
    else
    {
        char CustomEtype1[256];
        sprintf(CustomEtype1, "CustomEtype-%d",++ItemAllNum);
        ptrIni_add2->WriteInt(CustomEtype1, "Etype", ItemAllNum);
        ptrIni_add2->WriteString(CustomEtype1, "Name", item_name.c_str());
        ptrIni_add2->WriteInt(CustomEtype1, "Method", method_index);
        ptrIni_add2->WriteInt(CustomEtype1, "MeasureType", type_index);
        ptrIni_add2->WriteInt("MaxNumber", "Number", ItemAllNum);
        ptrIni_add2->WriteString(CustomEtype1, "Department", department_in);
        ptrIni_add2->SyncConfigFile();
        item_num_exist = ItemAllNum;
    }
    int number;
    char SelectNum[256];
    number = ptrIni_add2->ReadInt(department_in, "Number");
    sprintf(SelectNum, "Calc%d",number+1);
    ptrIni_add2->WriteInt(department_in, SelectNum, ItemAllNum);
    ptrIni_add2->WriteInt(department_in, "Number", number+1);
    ptrIni_add2->SyncConfigFile();

    vecAdd.push_back(item_num_exist);

}

static gboolean Destroy(gpointer data)
{
    ConfigToHost::GetInstance()->DestroyWindow();
    return FALSE;
}

void CustomCalc::ImportSuccess(void)
{

    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char userselectname[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(userselectname, "%s%s", CFG_RES_PATH, CALC_FILE);
    }
    else
    {
        sprintf(userselectname, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
    }

    char exam_name[256];
    char exam_type[256];
    strcpy(exam_name, CalcSetting::GetInstance()->GetExamName());
    exam.TransItemName(exam_name, exam_type);
    IniFile ini_add3(userselectname);
    IniFile *ptrIni_add3 = &ini_add3; 
    ptrIni_add3->RemoveGroup(exam_type);
    ptrIni_add3->SyncConfigFile();
    int item_length(0);
    item_length = vecAdd.size();
    for(int i=1;i<=item_length;i++)
    {
        char CalcNumber1[256];
        sprintf(CalcNumber1, "Calc%d", i);
        ptrIni_add3->WriteInt(exam_type, CalcNumber1, vecAdd[(i-1)]);
    }
    ptrIni_add3->WriteInt(exam_type, "Number", item_length);
    ptrIni_add3->SyncConfigFile();

    char userselectname2[256];
    sprintf(userselectname2, "%s", CALC_TMP_DATA_PATH); 
   FileMan f;

   f.DelDirectory(userselectname2);



   usleep(100000);
  
   ConfigToHost::GetInstance()->ExportRightInfoNotice(_("Success to import from USB storage."));


   ConfigToHost::GetInstance()->OKAndCancelClicked();  

   g_timeout_add(1000, Destroy, NULL);


   usleep(400000);
   CalcSetting::GetInstance()->ChangeModel2();

    PeripheralMan *ptr = PeripheralMan::GetInstance();
    ptr->UmountUsbStorage();
    g_menuCalc.ClearAllFlag();
    g_menuCalc.ChangeAllCalcItems();

}

void CustomCalc::ButtonImportNameOK()
{
    string item_name = CalcSetting::GetInstance()->CustomItemTransName(item_num_exist);

    char name_copy[256];
    for(int i=1; i<100;i++)
    {
        sprintf(name_copy, "%s(%d)", item_name.c_str(), i);
        if(RenameCompare(name_copy))
        {
            break;
        }
        if(i==99)
        {
            sprintf(name_copy, "%s(1)", item_name.c_str());
        }
    }
    item_name = name_copy; 

    ImportRenameCopy(item_name);
}

void CustomCalc::ButtonExportNameOKClicked(GtkButton *button)
{
    const char* tmp1_name = gtk_entry_get_text(GTK_ENTRY(m_entry_export_name));

    if (tmp1_name == NULL || strlen(tmp1_name) == 0)
    {
        CustomCalc::GetInstance()-> ExportErrorInfoNotice(_("Please Input Name!"));
        return;

    }
    strcpy(export_name, tmp1_name);
    PeripheralMan *ptr = PeripheralMan::GetInstance();
    vector<string> vec = CalcSetting::GetInstance()->GetSelectedVec();

    if(!ptr->CheckUsbStorageState())
    {
        CustomCalc::GetInstance()-> ExportErrorInfoNotice(_("No USB storage found!"));
        return ;
    }
    else
    {
        if(!ptr->MountUsbStorage())
        {
            CustomCalc::GetInstance()-> ExportErrorInfoNotice(_("Failed to mount USB storage!"));
            return ;
        }
    }
    UserSelect::GetInstance()->create_udisk_data_dir();//addec by LL


    int tmp=0; 
    struct dirent *ent;
    char udisk_path[256];
    sprintf(udisk_path, "%s%s", G_DIR_SEPARATOR_S, UDISK_DATA_PATH);
    DIR *dir = opendir(udisk_path);
    while( (ent = readdir(dir)) != NULL)
    {
        PRINTF("ent->name=%s\n", ent->d_name);
        if(ent->d_name[0]=='.')
            continue;
        if(strcmp(ent->d_name, tmp1_name)==0)
        {
            CustomCalc::GetInstance()-> ExportErrorInfoNotice(_("The name has existed, please rename!"));
            tmp=1;
            break;
        }
    }
    closedir(dir);
    if(tmp)
        return;


    if(CalcSetting::GetInstance()->GetSelectPath())
    {
        HideOKAndCancelClicked();  
        CustomCalc::GetInstance()-> ExportLoadInfoNotice(_("Loading......"));
        g_timeout_add(1000, LoadData, NULL);
    }
}


void CustomCalc::ButtonOKClicked(GtkButton *button)
{
    string name = gtk_entry_get_text(GTK_ENTRY(m_entry_name));
	trim(name);
	const char *tmp_name = name.c_str();

    if (tmp_name == NULL || strlen(tmp_name) == 0)
    {
		gtk_entry_set_text(GTK_ENTRY(m_entry_name), "");
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::INFO, _("Please Input Name!"), NULL);
        return;
    }

    int index = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_method));
	if (index == -1)
	{
		ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, 
                    _("Please select measure method!"), NULL);
		return;
	}

    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path, "%s%s", CFG_RES_PATH, CALC_ITEM_PATH);
    }
    else
    {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, CALC_ITEM_FILE, username.c_str(), ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni = &ini; 
   


    vector<string> useritemgroup;
    useritemgroup = ptrIni->GetGroupName();
    char src_group[256];
    int group_length(0);
    group_length = useritemgroup.size();
    for (int i= 0 ; i <  group_length; i++)
    {
        sprintf(src_group ,"%s", useritemgroup[i].c_str());
        int number1;
        number1 = ptrIni->ReadInt(src_group, "Number");

        for(int i=1;i<=number1;i++)
        {
            char CalcNumber[256];
            sprintf(CalcNumber, "Calc%d", i);
            int item_num = ptrIni->ReadInt(src_group, CalcNumber);
            string item_name;
            if(item_num < (USER_START - BASIC_MEA_END))
                item_name= CalcSetting::GetInstance()->ItemMenuTransEnglish(item_num);
            else
            {
                item_name = CalcSetting::GetInstance()->CustomItemTransName(item_num);
            }
            PRINTF("item_name= %s\n", item_name.c_str());
            PRINTF("item_tmp_name =%s\n", tmp_name);
            if((strcmp(tmp_name, item_name.c_str()) == 0) ||(strcmp(tmp_name, _(item_name.c_str())) == 0))
            {
                gtk_entry_set_text(GTK_ENTRY(m_entry_name), "");
                ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::INFO, _("The name has existed, please rename!"), NULL);
                return;
            }
        }

    }
    
    int ItemDeleteNum=ptrIni->ReadInt("Delete", "Number");

    int ItemAllNum;
    ItemAllNum=ptrIni->ReadInt("MaxNumber", "Number");
    if((ItemDeleteNum==0)&&(ItemAllNum >= (USER_START -BASIC_MEA_END + MAX_USER_CALC_NUM)))
    {
          ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, 
                    _("The defined items have reached the maximum!"), NULL); 
            return;
    }
    
    int type_index = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_type));
    if (type_index == -1)
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, 
                _("Please select measure type!"), NULL);
        return;
    }

    int type_method_index = 0;
    gchar *method_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_method));
    for(int i=0;i<ETYPE_NUM;i++)
    {
        if(strcmp(_(method_name), _(CustomEtypeArray[i].name.c_str())) == 0)
        {
            type_method_index = CustomEtypeArray[i].etype;
            break;
        }
    }
    /* for(int i=0;i<ETYPE_NUM;i++)
    {
        if(strcmp(custom_method[type_index][index], CustomEtypeArray[i].name.c_str()) == 0)
        {
            type_method_index = CustomEtypeArray[i].etype;
            break;
        }
    }
    */
    const gchar *department_name = CalcSetting::GetInstance()->GetDepartmentName();
    char department[50];
    for(int i=0; i<SECTION_NUM; i++)
    {
        if(strcmp(department_name, _(section[i].c_str()))==0)
        {
            strcpy(department, section[i].c_str());
            break;
        }
    }

    bool NewNumber = false;
    if(ItemDeleteNum)
    {
        char CalcNumber[256];
        sprintf(CalcNumber, "Calc%d", ItemDeleteNum);
        int item_num=ptrIni->ReadInt("Delete", CalcNumber);
        ptrIni->RemoveString("Delete", CalcNumber);
        ptrIni->WriteInt("Delete", "Number", ItemDeleteNum-1);
        ptrIni->SyncConfigFile();

        char CustomEtype[256];
        sprintf(CustomEtype, "CustomEtype-%d",item_num);
        ptrIni->WriteInt(CustomEtype, "Etype", item_num);
        ptrIni->WriteString(CustomEtype, "Name", tmp_name);
        ptrIni->WriteInt(CustomEtype, "Method", type_method_index);
        ptrIni->WriteInt(CustomEtype, "MeasureType", index);
        ptrIni->WriteString(CustomEtype, "Department", department);
        ptrIni->SyncConfigFile();
        ItemAllNum = item_num;
    } 
    else
    {
        NewNumber = true;
        char CustomEtype[256];
        sprintf(CustomEtype, "CustomEtype-%d",++ItemAllNum);
        ptrIni->WriteInt(CustomEtype, "Etype", ItemAllNum);
        ptrIni->WriteString(CustomEtype, "Name", tmp_name);
        ptrIni->WriteInt(CustomEtype, "Method", type_method_index);
        ptrIni->WriteInt(CustomEtype, "MeasureType", index);
        ptrIni->WriteInt("MaxNumber", "Number", ItemAllNum);
        ptrIni->WriteString(CustomEtype, "Department", department);
        ptrIni->SyncConfigFile();
    }

    int number;
    char SelectNum[256];
    number = ptrIni->ReadInt(department, "Number");
    sprintf(SelectNum, "Calc%d",number+1);
    ptrIni->WriteInt(department, SelectNum, ItemAllNum);
    ptrIni->WriteInt(department, "Number", number+1);
    ptrIni->SyncConfigFile();

    DestroyWin();
    CalcSetting::GetInstance()->ChangeModelAndLight(tmp_name);
    CalcSetting::GetInstance()->ClearAllCalc();
}


void CustomCalc::DestroyWin(void)
{
    if (GTK_IS_WIDGET(m_window))
    {
        g_keyInterface.Pop();
        gtk_widget_destroy(m_window);
        if (g_keyInterface.Size() == 1)
            SetSystemCursor(SYSCURSOR_X, SYSCUROSR_Y);
    }
}

void CustomCalc::DelayDestroyWin(void)
{
    if (GTK_IS_WIDGET(m_window))
    {
        g_keyInterface.Pop();
        gtk_widget_destroy(m_window);
        if (g_keyInterface.Size() == 1)
            SetSystemCursor(SYSCURSOR_X, SYSCUROSR_Y);
    }
}


gboolean CustomCalc::WindowDeleteEvent(GtkWidget *widget, GdkEvent *event)
{
    DestroyWin();
    return FALSE;
}

static gboolean ExitCustomeCalc(gpointer data)
{
    CustomCalc *tmp = (CustomCalc *)data;
    tmp->DestroyWin();
    return FALSE;
}

void CustomCalc::KeyEvent(unsigned char keyValue)
{
    //FakeXEvent::KeyEvent(keyValue);
#if defined(EMP_322)
    if(keyValue==KEY_CTRL_SHIFT_SPACE)
    {
        KeySwitchIM ksim;
        ksim.ExcuteChange(FALSE);
        return;
    }
#elif defined(EMP_313)
    {
        if (keyValue == KEY_ONE)
        {
            KeySwitchIM ksim;
            ksim.ExcuteChange(FALSE);
            return;
        }
    }
#else
    if(keyValue==KEY_SHIFT_CTRL)
    {
        KeySwitchIM ksim;
        ksim.ExcuteChange(FALSE);
        return;
    }
#endif
    if(FakeMouseButton(keyValue))
        return;
    if(FakeNumKey(keyValue))//数字键
        return;
    if(FakeAlphabet(keyValue))//字母键
        return;
    if(FakePunctuation(keyValue)) //符号键
        return;

    switch(keyValue)
    {
        case KEY_ESC:
            g_timeout_add(100, ExitCustomeCalc, this);
            FakeEscKey();
            break;
    }
}
