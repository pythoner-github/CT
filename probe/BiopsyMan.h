#ifndef _BIOPSYMAN_H_
#define _BIOPSYMAN_H_

#include "BiopsySetting.h"
#include "DefaultBiopsySetting.h"
using namespace std;

class BiopsyMan
{
public:
   // BiopsyMan();
    ~BiopsyMan();
	static BiopsyMan* GetInstance();

    string GetDefaultBioBracketTypeOfCurProbe(void);
	vector<string> GetBioBracketTypesOfCurProbe(void);
    vector<string> GetBioAngleTypesOfDefaultBioBracket(void);
	vector<string> GetBioAngleTypesOfcurBioBracket(void);
	
	int GetBioBracketTypeNumOfCurProbe(void);//2016.08.31

	string GetCurBioBracketType(void);
	void SetCurBioBracketType(string bioBracketType);
	
	string GetCurBioAngleType(void);
	void SetCurBioAngleType(string angleType);

	string GetDefaultAngleTypeOfCurBioBracket(void);
	//int GetBioXOfCurBioBracketAngleType(void);//2016.08.08
    double GetBioXOfCurBioBracketAngleType(void);//2016.08.08

	//int GetCurBioAngleValue(void);//2016.08.09
    double GetCurBioAngleValue(void);//2016.08.09

//	void SetCurBioAngleValue(int value);
	bool IsBioBracketChanged(void);
	//void WriteXAngleValue(int xValue,int angleValue);
    void WriteXAngleValue(double xValue,double angleValue);
	//------------------------------------------------------//
	//int GetCurBioAngleValueDefault(void);
   double  GetCurBioAngleValueDefault(void);

	//int GetDefaultBioXOfCurBioBracketAngleType(void);//2016.09.21
    double GetDefaultBioXOfCurBioBracketAngleType(void);//2016.09.21
	//------------------------------------------------------------//
    void SetCurBioBracketAndAngleTypeOfProbeChanged(void);//2016.11.06

private:
	BiopsyMan();
	static BiopsyMan* m_ptrInstance;
	bool m_isBioBracketChanged;

	BiopsySetting *m_ptrBiopsySetting;
	string m_curProbeModel;
	string m_defaultBioBracketType;
	string m_curBioBracketType;
	string m_curBioAngleType;

	DefaultBiopsySetting *m_ptrDefaultBiopsySetting;

//	int m_curBioAngleValue;//2016.08.09

};

#endif 
