#ifndef _BATTERY__H_
#define _BATTERY__H_

class Battery
{
    public:
        Battery(){  }
        ~Battery(){  }
        int GetCapacity();

    private:
        //static unsigned char ADDR = 0x0b;
       // static unsigned char GET_CHARGE = 0x0d;
        static unsigned char ADDR;
        static unsigned char GET_CHARGE ;
};

#endif