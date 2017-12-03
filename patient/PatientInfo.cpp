/*
 * 2009, 深圳恩普电子技术有限公司
 *
 * @file: PatientInfo.cpp
 * @brief: manage patient info and patient exam info
 *
 * version: V1.0
 * date: 2009-7-21	
 * @author: zhanglei
 */

#include <glib/gstdio.h>
#include <stdlib.h>
#include "PatientInfo.h"
#include "Def.h"
#include "TopArea.h"
#include "gui_global.h"
#include "gui_func.h"
#include "Database.h"
#include "ViewReport.h"
#include "MeasureMan.h"
#include "bodymark/BodyMark.h"
#include "FileMan.h"
#include "../sysMan/SysGeneralSetting.h"

PatientInfo g_patientInfo;

PatientInfo::PatientInfo()
{
    InitExam();
    InitPatient();
    m_exist = false;
}
PatientInfo::~PatientInfo()
{
}

void PatientInfo::GetInfo(Info &info)
{
	info = m_info;
}
void PatientInfo::SetInfo(Info info)
{
    m_info = info;
    m_exist = true;
    UpdateTopArea();
}

void PatientInfo::SetInfoQA(Info info)
{
    m_info = info;
    m_exist = true;
}

/*
 * @brief clear record(image and video) in store path 0
 */
void PatientInfo::ClearRecord(void)
{
	gchar *path;
	gchar buf[100];

	path = g_build_path(G_DIR_SEPARATOR_S, STORE_PATH, "0", NULL);
	if (g_access(path, F_OK) == 0)
	{
		PRINTF("-----------------------------------------folder of store path 0 is exist, path = %s\n", path);
		sprintf(buf, "rm -rf %s\n", path);
		//if (g_remove(path) == -1)
		FileMan fm;
		if (fm.DelDirectory(buf) == -1)
		{
			perror("rmdir fail!");
			PRINTF("delete folder of store path 0 error\n");
			g_free(path);	
			return;
		}
	}

	if (g_mkdir(path, 0755) == -1)
	{
		PRINTF("create folder of store path 0 error\n");
	}
	
	g_free(path);
}

void PatientInfo::ClearExam()
{
	// clear exam info
	ClearExamInfo();

	// clear record
	ClearRecord();
	
	// clear result of measure, calc, report
}

void PatientInfo::ClearAll()
{
	// clear patient info
	ClearPatientInfo();
    
	UpdateTopArea();

	// clear exam info
	ClearExamInfo();

	// clear record
	ClearRecord();
	
	m_exist = false;
	// clear result of measure, calc, report
	MeasureMan::GetInstance()->ClearAllValue();

	// hide bodymark
	BodyMark::HideBodyMark();    
}
#ifdef VET

#else
void PatientInfo::UpdatePatInfoToTopArea(PatientInfo::Name patientName ,string sex,string age,string id)
{
    char buf[20]="\0";
    char sex_tmp[256]="\0";
    sprintf(buf,"%s",sex.c_str());
#ifdef VET
    if(strcmp(buf,"F")==0)
        sprintf(sex_tmp,"%s",_("Female "));
    else if(strcmp(buf,"M")==0)
        sprintf(sex_tmp,"%s",_("Male "));
#else
    if(strcmp(buf,"F")==0)
        sprintf(sex_tmp,"%s",_("Female"));
    else if(strcmp(buf,"M")==0)
        sprintf(sex_tmp,"%s",_("Male"));
#endif
    else if(strcmp(buf,"O")==0)
        sprintf(sex_tmp,"%s",_("Other"));

    char age_tmp[256]="\0";
    char str_tmp[256]="\0";
    char age_buf[256] = "\0";
    char tmp[256] = "\0";

     if(strlen(age.c_str())!= 0)
     {
         sprintf(age_tmp,"%c%c%c",age[0],age[1],age[2]);

         sprintf(tmp,"%d",atoi(age_tmp));
         strcat(age_buf,tmp);

         int age_len = strlen(age.c_str());
         sprintf(str_tmp,"%c",age[age_len-1]);
         if(strcmp(str_tmp,"Y")==0)
             strcat(age_buf,_("Y"));
         else if(strcmp(str_tmp,"M")==0)
             strcat(age_buf,_("M"));
         else if(strcmp(str_tmp,"D")==0)
             strcat(age_buf,_("D"));
     }
     else
     {
         sprintf(age_buf,"%s","");
     }

	 string name;
	 SysGeneralSetting sgs;
	 int lang = sgs.GetLanguage();
	 if(!lang)
		 name = patientName.first + " " + patientName.mid + " " + patientName.last;
	 else
		 name = patientName.last + " " + patientName.first + " " + patientName.mid;

    TopArea::GetInstance()->UpdatePatInfo(name.c_str(), sex_tmp, age_buf, id.c_str());
}
#endif
void PatientInfo::UpdateTopArea(void)
{
    string sex_text, name, age;

    GetSexString(m_info.p.sex, sex_text);
#ifdef VET
    string species;
    GetAnimalSpeciesString(m_info.p.species, species);
#else
    //string name;
    SysGeneralSetting sgs;    
    int lang = sgs.GetLanguage();
    if(!lang)
        name = m_info.p.name.first + " " + m_info.p.name.mid + " " + m_info.p.name.last;
    else
        name = m_info.p.name.last + " " + m_info.p.name.first + " " + m_info.p.name.mid;
#endif
    GenAgeStr(m_info.p.age, m_info.p.ageUnit, age);

#ifdef VET
    TopArea::GetInstance()->UpdatePatInfo(m_info.p.animal_name.c_str(), m_info.p.owner_name.c_str(), species.c_str(), sex_text.c_str(), age.c_str(), m_info.p.id.c_str());
#else
    TopArea::GetInstance()->UpdatePatInfo(name.c_str(), sex_text.c_str(), age.c_str(), m_info.p.id.c_str());
#endif
 
}

bool PatientInfo::ArchivePatientInfo(string &errmsg)
{
	Database db;
	if (db.ArchivePat(m_info, errmsg))
	{
		db.GetExamIDCurrent(m_info.e.examNum);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void PatientInfo::ArchiveImg()
{
	gchar *path1, *path2;

	path1 = g_build_path(G_DIR_SEPARATOR_S, STORE_PATH, "0", NULL);
	path2 = g_build_path(G_DIR_SEPARATOR_S, STORE_PATH, m_info.e.examNum.c_str(), NULL);

	PRINTF("PATH1 = %s\n path2 = %s\n", path1, path2);
	// if path1 is not exist
	if (g_access(path1, F_OK) == -1)
	{
		perror("path1 is not exist, create it now\n");
		if (g_mkdir(path1, 0755) == -1)
		{
			perror("g_mkdir path1 error:");
			goto free;
		}
	}

	// if path2 is exist
	if (g_access(path2, F_OK) == 0)
	{
		perror("path2 is exist, delete it now\n");
	//	if (g_rmdir(path2) == -1)
		FileMan fm;
		if (fm.DelDirectory(path2) == -1)
		{
			perror("g_rmdir path2 error");
			goto free;
		}
	}

	if(g_rename(path1, path2) < 0)
	{
		perror("g_rename error:");
	}

free:
	g_free(path1);
	g_free(path2);
	PRINTF("END of archiveimg\n")
}

void PatientInfo::ArchiveReport()
{
	int examID = atoi(m_info.e.examNum.c_str());
	ViewReport::GetInstance()->ArchiveReport(examID);
}

///> private
void PatientInfo::InitPatient()
{
#ifdef VET
	m_info.p.animal_name = "";
	m_info.p.owner_name = "";
	m_info.p.species = -1;
#else
	m_info.p.name.last = "";
	m_info.p.name.first = "";
	m_info.p.name.mid = "";
#endif
	m_info.p.sex = -1;
	m_info.p.age = 0;
	m_info.p.ageUnit = 0;
	m_info.p.id = "";
	m_info.p.birthDate.year = 0;
	m_info.p.birthDate.month = 0;
	m_info.p.birthDate.day = 0;
	m_info.p.telephone = "";
	m_info.p.address = "";
	m_info.p.comment = "";
}

void PatientInfo::InitComment()
{
#if 0
	m_info.p.name.last = "";
	m_info.p.name.first = "";
	m_info.p.name.mid = "";
	m_info.p.sex = -1;
	m_info.p.age = 0;
	m_info.p.ageUnit = 0;
	m_info.p.id = "";
	m_info.p.birthDate.year = 0;
	m_info.p.birthDate.month = 0;
	m_info.p.birthDate.day = 0;
	m_info.p.telephone = "";
	m_info.p.address = "";
#endif
	m_info.p.comment = "";
}
void PatientInfo::InitExam()
{
	InitExamGen();
	InitExamOb();
	InitExamCar();
	InitExamUro();
}

void PatientInfo::InitExamGen()
{
	int year, mon, day;

	GetCurrentDate(year, mon, day);

	// general
	m_info.e.examNum = "0";
	m_info.e.examType = "";
	m_info.e.examDate.year = year;
	m_info.e.examDate.month = mon;
	m_info.e.examDate.day = day;
	m_info.e.height = 0;
	m_info.e.weight = 0.0;
	m_info.e.BSA = 0.0;
	m_info.e.examDoctor = "";
	m_info.e.reportDoctor = "";
	m_info.e.comment = "";
	m_info.e.imagePath = STORE_PATH;
}
void PatientInfo::InitExamOb()
{
	// ob
	m_info.ob.LMPDate.year = 0;
	m_info.ob.LMPDate.month = 0;
	m_info.ob.LMPDate.day = 0;
	m_info.ob.OvulateDate.year= 0;
	m_info.ob.OvulateDate.month = 0;
	m_info.ob.OvulateDate.day = 0;
	m_info.ob.pregCount = -1;
	m_info.ob.abnormalPregCount = -1;
	m_info.ob.fetusCount = -1;
	m_info.ob.deliveryCount = -1;
	m_info.ob.miscarryCount = -1;
}
void PatientInfo::InitExamCar()
{
	// car
	m_info.car.HR = -1;
	m_info.car.pressureHigh = -1;
	m_info.car.pressureLow = -1;
}
void PatientInfo::InitExamUro()
{
	//uro
	m_info.uro.PSA = -1;
}

void PatientInfo::ClearExamInfo()
{
	InitExam();	
}
void PatientInfo::ClearPatientInfo()
{
    InitPatient();
}


