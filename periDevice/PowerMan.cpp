#include <glib.h>
#include "periDevice/PowerMan.h"
#include "Def.h"
#include "periDevice/IoCtrl.h"
#include "imageProc/GlobalClassMan.h"

PowerMan* PowerMan::m_ptrInstance = NULL;

PowerMan::PowerMan()
{
}

PowerMan::~PowerMan()
{
    if (m_ptrInstance != NULL)
    {
        delete m_ptrInstance;
        m_ptrInstance = NULL;
    }
}

PowerMan* PowerMan::GetInstance()
{
    if (m_ptrInstance == NULL)
    {
        m_ptrInstance = new PowerMan;
    }

    return m_ptrInstance;
}

void PowerMan::DisplayBatteryPower(void)
{
    int powerPercent(0);
    IoCtrl *ioct;
   // powerPercent = ioct->GetRemainBatteryCapacity();

    /*AbsUpdateMix *ptrUpdate = GlobalClassMan::GetInstance()->GetUpdateMix();
    ptrUpdate->BatteryPower(powerPercent);*/
    printf("batter:%d\n",powerPercent);
}