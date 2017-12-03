#include "BiopsySetting.h"
#include "../include/Def.h"

BiopsySetting::BiopsySetting()
{
    char path[256];
    sprintf(path, "%s/%s", CFG_RES_PATH, BIOPSY_SETTING_FILE);
	m_ptrIni = new IniFile(path);

}
BiopsySetting:: ~BiopsySetting()
{
	 if (m_ptrIni != NULL)
	 {
		delete m_ptrIni;
		m_ptrIni = NULL;
	 }
}
string BiopsySetting::GetDefaultBioBracketType(const string probeModel)
{
	return m_ptrIni->ReadString(probeModel.c_str(),"DefaultBracket");
}

int BiopsySetting::GetBioBracketNum(const string probeModel)
{
	return m_ptrIni->ReadInt(probeModel.c_str(),"Num");
}

vector<string> BiopsySetting::GetBioBracketTypeOfProbe(const string probeModel)
{
	vector<string> vec;
	vec.clear();
	string bracketType;
	int num=0;
	num=GetBioBracketNum(probeModel);
	for(int i=0;i<num;i++)
	{
		string tempKey="Bracket";
		char temp[32];
		sprintf(temp, "%d", i);
		tempKey=tempKey+temp;
		bracketType=m_ptrIni->ReadString(probeModel.c_str(),tempKey.c_str());
		vec.push_back(bracketType);
	}
	return vec;
}
string BiopsySetting::GetDefaultAngleType(const string bioBracketType)
{
	return m_ptrIni->ReadString(bioBracketType.c_str(),"DefaultAngleType");
}

int BiopsySetting::GetBioBracketAngleNum(const string bioBracketType)
{
	return m_ptrIni->ReadInt(bioBracketType.c_str(),"Num");
}

vector<string> BiopsySetting::GetBioBracketAngleType(const string bioBracketType)
{
	vector<string> vec;
	vec.clear();
	string angleType;
	int num=0;
	num=GetBioBracketAngleNum(bioBracketType);
	for(int i=0;i<num;i++)
	{
		string tempKey="AngleType";
		char temp[32];
		sprintf(temp, "%d", i);
		tempKey=tempKey+temp;
		angleType=m_ptrIni->ReadString(bioBracketType.c_str(),tempKey.c_str());
		vec.push_back(angleType);
	}
	return vec;

}

double BiopsySetting::GetBiopsyXOfAngleType(const string bioBracketTypeAngle)
{
	return m_ptrIni->ReadDouble(bioBracketTypeAngle.c_str(),"BiopsyX");
}

void BiopsySetting::SetBiopsyXOfAngleType(const string bioBracketTypeAngle,double value)
{
	m_ptrIni->WriteDouble(bioBracketTypeAngle.c_str(),"BiopsyX",value);
}

void BiopsySetting::SyncFile(void)
{
	m_ptrIni->SyncConfigFile();
}

double BiopsySetting::GetBiopsyAngleValueOfAngleType(const string bioBracketTypeAngle)
{
	return (m_ptrIni->ReadDouble(bioBracketTypeAngle.c_str(),"BiopsyAngle"));
}

void BiopsySetting::SetBiopsyAngleValueOfAngleType(const string bioBracketTypeAngle, double value)
{
	m_ptrIni->WriteDouble(bioBracketTypeAngle.c_str(),"BiopsyAngle",value);
}

