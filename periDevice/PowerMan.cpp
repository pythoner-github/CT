/*
 * 2012,深圳恩普电子技术有限公司
 *
 * @file: PowerMan.cpp
 * @brief: 根据当前机器上盖的开合状态，控制显示屏的电源。
 *         上盖合上关电源，上盖打开开电源。
 * @version: 1.0
 * @date: 2012-5-16
 * @author: zhanglei
 */

#include <glib.h>
#include "PowerMan.h"
#include "../include/Def.h"
#include "../periDevice/IoCtrl.h"
#include "../imageProc/GlobalClassMan.h"

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
