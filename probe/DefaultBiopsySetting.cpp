#include "probe/DefaultBiopsySetting.h"
#include "Def.h"

DefaultBiopsySetting::DefaultBiopsySetting()
{
    char path[256];
    sprintf(path, "%s/%s", CFG_RES_PATH, DEFAULT_SETTING_FILE);
    m_ptrIni = new IniFile(path);

}
DefaultBiopsySetting:: ~DefaultBiopsySetting()
{
     if (m_ptrIni != NULL)
     {
        delete m_ptrIni;
        m_ptrIni = NULL;
     }
}

/*int DefaultBiopsySetting::GetBiopsyXOfAngleType(const string bioBracketTypeAngle)
{
    return m_ptrIni->ReadInt(bioBracketTypeAngle.c_str(),"BiopsyX");
}*/
double DefaultBiopsySetting::GetBiopsyXOfAngleType(const string bioBracketTypeAngle)
{
    return m_ptrIni->ReadDouble(bioBracketTypeAngle.c_str(),"BiopsyX");
}

/*int DefaultBiopsySetting::GetBiopsyAngleValueOfAngleType(const string bioBracketTypeAngle)
{
    return (m_ptrIni->ReadInt(bioBracketTypeAngle.c_str(),"BiopsyAngle"));
}*/

double  DefaultBiopsySetting::GetBiopsyAngleValueOfAngleType(const string bioBracketTypeAngle)
{
    return (m_ptrIni->ReadDouble(bioBracketTypeAngle.c_str(),"BiopsyAngle"));
}