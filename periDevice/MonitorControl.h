#ifndef __MONITORCONTROL_H__
#define __MONITORCONTROL_H__


class MonitorControl
{
    public:

        MonitorControl();
        ~MonitorControl();

        int SetBrightAdd(void);
        int SetBrightSub(void);
        int SetContrastAdd(void);
        int SetContrastSub(void);
        int PowerOff(void);
        int PowerOn(void);
        int AutoAdjust(void);
        int AutoBalance(void);

    private:
        int DisplayWrite(unsigned char reg, unsigned char value = 0xff );//jufeng
        int DisplayWrite1(unsigned char reg);//ruixian


#define VIDEO_ADDR1 0x37//睿显
#define BRIGHT_ADD      0x03
#define BRIGHT_SUB      0x01
#define CONTRAST_ADD    0x07
#define CONTRAST_SUB    0x05
#define AUTO_ADJUST     0x09
#define AUTO_BALANCE    0X11
#define POWER_ON        0x15
#define POWER_OFF       0x17
#define USER_RESET      0x13

#define VIDEO_ADDR2 0x55//巨峰
#define INVALID_VALUE 0xff

#define SET_BRIGHT_ADD      0x11
#define SET_BRIGHT_SUB      0x13
#define SET_CONTRAST_ADD    0x17
#define SET_CONTRAST_SUB    0x1F
#define SET_AUTO_ADJUST     0x31
#define SET_AUTO_BALANCE    0X33
#define SET_POWER_ON        0x14
#define SET_POWER_OFF       0x15

};

#endif
