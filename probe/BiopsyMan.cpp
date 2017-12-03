#include <iostream>
#include <sstream>
#include "BiopsyMan.h"
#include "ProbeSocket.h"
#include "ProbeMan.h"

BiopsyMan* BiopsyMan::m_ptrInstance = NULL;

/**/
BiopsyMan::BiopsyMan()
{
	m_ptrBiopsySetting=new BiopsySetting;
	m_ptrDefaultBiopsySetting=new DefaultBiopsySetting;//2016.09.21

    ProbeSocket::ProbePara curPara;
    ProbeMan::GetInstance()->GetCurProbe(curPara);
	m_curProbeModel = curPara.model;
    m_curProbeModel += curPara.factoryFlag;
    m_defaultBioBracketType=m_ptrBiopsySetting->GetDefaultBioBracketType(m_curProbeModel);
    m_curBioBracketType=m_defaultBioBracketType;
	m_curBioAngleType=m_ptrBiopsySetting->GetDefaultAngleType(m_curBioBracketType);
	m_isBioBracketChanged=false;
}
BiopsyMan::~BiopsyMan()
{
	if(m_ptrBiopsySetting!=NULL)
	{
		delete m_ptrBiopsySetting;
		m_ptrBiopsySetting=NULL;
	}
	if(m_ptrDefaultBiopsySetting!=NULL)
	{
		delete m_ptrDefaultBiopsySetting;
		m_ptrDefaultBiopsySetting=NULL;
	}

	if (m_ptrInstance != NULL)
		delete m_ptrInstance;

}
BiopsyMan* BiopsyMan::GetInstance()
{
	if (m_ptrInstance == NULL)
	{
		m_ptrInstance = new BiopsyMan;
	}

	return (m_ptrInstance);
}

string BiopsyMan::GetDefaultBioBracketTypeOfCurProbe(void)
{
	ProbeSocket::ProbePara curPara;
    ProbeMan::GetInstance()->GetCurProbe(curPara);
	m_curProbeModel = curPara.model;
    m_curProbeModel += curPara.factoryFlag;

    m_defaultBioBracketType=m_ptrBiopsySetting->GetDefaultBioBracketType(m_curProbeModel);

	return m_defaultBioBracketType;
}
vector<string> BiopsyMan::GetBioBracketTypesOfCurProbe(void)
{
	ProbeSocket::ProbePara curPara;
    ProbeMan::GetInstance()->GetCurProbe(curPara);
	m_curProbeModel = curPara.model;
    m_curProbeModel += curPara.factoryFlag;

	return (m_ptrBiopsySetting->GetBioBracketTypeOfProbe(m_curProbeModel));
}
vector<string> BiopsyMan::GetBioAngleTypesOfDefaultBioBracket(void)
{
	return (m_ptrBiopsySetting->GetBioBracketAngleType(m_defaultBioBracketType));
}
vector<string> BiopsyMan::GetBioAngleTypesOfcurBioBracket(void)
{
	return (m_ptrBiopsySetting->GetBioBracketAngleType(m_curBioBracketType));
}
string BiopsyMan::GetCurBioBracketType(void)
{
	return m_curBioBracketType;
}
int BiopsyMan::GetBioBracketTypeNumOfCurProbe(void)
{
	ProbeSocket::ProbePara curPara;
    ProbeMan::GetInstance()->GetCurProbe(curPara);
	m_curProbeModel = curPara.model;
    m_curProbeModel += curPara.factoryFlag;
	return (m_ptrBiopsySetting->GetBioBracketNum(m_curProbeModel));

}
void BiopsyMan::SetCurBioBracketType(string bioBracketType)
{
	if(m_curBioBracketType.compare(bioBracketType)!=0)
	{
		m_isBioBracketChanged=true;
	}
	else
	{
		m_isBioBracketChanged=false;
	}
	m_curBioBracketType=bioBracketType;
}
	
string BiopsyMan::GetCurBioAngleType(void)
{
	return m_curBioAngleType;
}
void BiopsyMan::SetCurBioAngleType(string angleType)
{
	m_curBioAngleType=angleType;
}
string BiopsyMan::GetDefaultAngleTypeOfCurBioBracket(void)
{
	return (m_ptrBiopsySetting->GetDefaultAngleType(m_curBioBracketType));
}
double  BiopsyMan::GetBioXOfCurBioBracketAngleType(void) //2016.08.08
{
	stringstream ss;
	string strAngleType;
	ss<<"-"<<m_curBioAngleType;
	ss>>strAngleType;
	string bracketAngle=m_curBioBracketType+strAngleType;
	return (m_ptrBiopsySetting->GetBiopsyXOfAngleType(bracketAngle));
}

double BiopsyMan::GetCurBioAngleValue(void)//2016.08.09
{
	stringstream ss;
	string strAngleType;
	ss<<"-"<<m_curBioAngleType;
	ss>>strAngleType;
	string bracketAngle=m_curBioBracketType+strAngleType;
	return (m_ptrBiopsySetting->GetBiopsyAngleValueOfAngleType(bracketAngle));

}

bool BiopsyMan::IsBioBracketChanged(void)
{
	return m_isBioBracketChanged;
}

void BiopsyMan::WriteXAngleValue(double xValue, double angleValue)
{
	stringstream ss;
	string strAngleType;
	ss<<"-"<<m_curBioAngleType;
	ss>>strAngleType;
	string bracketAngleType=m_curBioBracketType+strAngleType;

	m_ptrBiopsySetting->SetBiopsyAngleValueOfAngleType(bracketAngleType,angleValue);
	m_ptrBiopsySetting->SetBiopsyXOfAngleType(bracketAngleType,xValue);
	m_ptrBiopsySetting->SyncFile();
}

double  BiopsyMan::GetCurBioAngleValueDefault(void)
{
	stringstream ss;
	string strAngleType;
	ss<<"-"<<m_curBioAngleType;
	ss>>strAngleType;
	string bracketAngle=m_curBioBracketType+strAngleType;
	return (m_ptrDefaultBiopsySetting->GetBiopsyAngleValueOfAngleType(bracketAngle));
}

double BiopsyMan::GetDefaultBioXOfCurBioBracketAngleType(void)//2016.09.21
{
	stringstream ss;
	string strAngleType;
	ss<<"-"<<m_curBioAngleType;
	ss>>strAngleType;
	string bracketAngle=m_curBioBracketType+strAngleType;
	return (m_ptrDefaultBiopsySetting->GetBiopsyXOfAngleType(bracketAngle));
}

void BiopsyMan::SetCurBioBracketAndAngleTypeOfProbeChanged(void)
{
    string strBioType=GetDefaultBioBracketTypeOfCurProbe();
    m_curBioBracketType=strBioType;
    m_curBioAngleType=GetDefaultAngleTypeOfCurBioBracket();
}

