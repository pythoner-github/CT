#ifndef _POWER_MAN_H__
#define _POWER_MAN_H__

class PowerMan
{
    public:
        ~PowerMan();
        static PowerMan* GetInstance();
        void DisplayBatteryPower(void);

    private:
        static PowerMan* m_ptrInstance;
        PowerMan();
};

#endif
