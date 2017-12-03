#include <sys/types.h>
#include <errno.h>
//#include "../probe/ProbeSocket.h"
#include "UIS4DReceiveInterface.h"
#include "../base/IniFile.h"

#include "DSC.h"
class D4FuncMan
{
    public:
        ~D4FuncMan(); 
        static D4FuncMan* GetInstance();

        void GetPara(SUIS4DPARA &Para);
        void GetDataFromUsb(const unsigned char *pData);
        void Exit4D();

        // 设置4D mode标记
        void Set4DMode(bool flag)
        {
            m_d4_mode = flag;
        }

        // 获得4D mode标记
        bool Get4DMode(){return m_d4_mode;}

    private:
        string m_tis;
        string m_checkpart;
        SUIS4DPARA m_Para4d;

        bool m_d4_mode;
     
        unsigned char *m_yBits;
        unsigned int m_lineNo_bak;
        unsigned char *m_pD4bits;
        unsigned char m_frameNum;
        unsigned char m_reverseFlag;
        DSCCONTROLATTRIBUTES* m_ptrDscpara; 

    private:
        D4FuncMan();

        void  GetProbeInfo();
        void GetTIS();
        void GetCheckPart();
        void GetHospitalName();
        void GetPatientInfo();
        void GetTimeFormat();
        void GetDepth();

        ///> extern member
        static D4FuncMan* m_ptrInstance;	
};
       

