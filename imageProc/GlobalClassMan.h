#ifndef GLOBAL_CLASS_MAN_H
#define GLOBAL_CLASS_MAN_H

#include <string>
#include "Update2D.h"
#include "UpdatePw.h"
#include "UpdateCfm.h"
#include "UpdateMix.h"
#include "Calc2D.h"
#include "CalcPw.h"
#include "CalcCfm.h"
#include "KeyValueOpr.h"

using std::string;

extern  pKeyFunc g_ptrKeyFunc;
class GlobalClassMan
{
	public:
		GlobalClassMan();
		~GlobalClassMan();
		static GlobalClassMan* GetInstance();

		Calc2D* GetCalc2D(string model);
		AbsUpdate2D* GetUpdate2D();
		AbsUpdatePw* GetUpdatePw();
		AbsUpdateCfm* GetUpdateCfm();
		AbsUpdateMix* GetUpdateMix();
		char GetProbeType();
		Calc2D* GetCalc2D();
		CalcPw* GetCalcPw(string model);
		CalcCfm* GetCalcCfm(string model);
		KeyValueOpr* GetKeyOpr();

	private:
		static GlobalClassMan* m_ptrInstance;

		Update2D m_update2D;
		UpdatePw m_updatePw;
		UpdateCfm m_updateCfm;
		UpdateMix m_updateMix;
		char m_probeType;
		Calc2D* m_ptrCalc2D;
		CalcPw m_calcPw;
		CalcCfm m_calcCfm;
		
		KeyValueOpr m_keyOpr;
};

inline AbsUpdate2D* GlobalClassMan::GetUpdate2D()
{
	return &m_update2D;
}
inline AbsUpdatePw* GlobalClassMan::GetUpdatePw()
{
	return &m_updatePw;
}
inline AbsUpdateCfm* GlobalClassMan::GetUpdateCfm()
{
	return &m_updateCfm;
}
inline AbsUpdateMix* GlobalClassMan::GetUpdateMix()
{
	return &m_updateMix;
}

inline Calc2D* GlobalClassMan::GetCalc2D()
{
	return m_ptrCalc2D;
}

inline char GlobalClassMan::GetProbeType()
{
	return m_probeType;
}

inline CalcPw* GlobalClassMan::GetCalcPw(string model)
{
	return &m_calcPw;
}
inline CalcCfm* GlobalClassMan::GetCalcCfm(string model)
{
	return &m_calcCfm;
}

inline KeyValueOpr* GlobalClassMan::GetKeyOpr()
{
	return &m_keyOpr;
}

#endif
