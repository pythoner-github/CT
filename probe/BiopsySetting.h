#ifndef _BIOPSYSETTING_H_
#define _BIOPSYSETTING_H_

#include "IniFile.h"
using namespace std;

class BiopsySetting
{
public:
    BiopsySetting();
    ~BiopsySetting();

    string GetDefaultBioBracketType(const string probeModel);

	int GetBioBracketNum(const string probeModel);
	vector<string> GetBioBracketTypeOfProbe(const string probeModel);
    
	string GetDefaultAngleType(const string bioBracketType);

	int GetBioBracketAngleNum(const string bioBracketType);
	vector<string> GetBioBracketAngleType(const string bioBracketType);
	//int GetBiopsyXOfAngleType(const string bioBracketTypeAngle);
    double GetBiopsyXOfAngleType(const string bioBracketTypeAngle);	

    /*void SetBiopsyXOfAngleType(const string bioBracketTypeAngle,int value);*/
    void SetBiopsyXOfAngleType(const string bioBracketTypeAngle,double value);
	//int GetBiopsyAngleValueOfAngleType(const string bioBracketTypeAngle);
	double GetBiopsyAngleValueOfAngleType(const string bioBracketTypeAngle);
	/*void SetBiopsyAngleValueOfAngleType(const string bioBracketTypeAngle,int value);*/
void SetBiopsyAngleValueOfAngleType(const string bioBracketTypeAngle,double value);

    void SyncFile(void);
private:
    IniFile *m_ptrIni;
};

#endif 
