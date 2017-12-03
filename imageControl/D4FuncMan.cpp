#include <sys/wait.h>
#include "imageControl/D4FuncMan.h"
#include "probe/ProbeMan.h"
#include <UIS4DReceiveInterface.h>
#include "display/TopArea.h"
#include "imageControl/Img2D.h"
#include "imageProc/DscMan.h"
#include "display/gui_func.h"
#include "sysMan/SysGeneralSetting.h"
#include "patient/PatientInfo.h"
#include "imageProc/FreezeMode.h"
#include <DSCTypes.h>
#include "display/ImageAreaPara.h"
#include "imageProc/ScanMode.h"
#include "imageProc/ImgProc2D.h"
#include "display/ImageAreaPara.h"
#include "keyboard/KeyValueOpr.h"
#include "keyboard/LightDef.h"
#include "imageProc/ModeStatus.h"
#include "ViewMain.h"

extern int g_tgcSlider[8];
CUIS4DReceiveInterface g_uis4d_ri;
unsigned char *g_pshm_data0, *g_pshm_data1;
int g_sem_id0, g_sem_id1;
int g_shm_id0, g_shm_id1;

D4FuncMan* D4FuncMan::m_ptrInstance = NULL;

D4FuncMan* D4FuncMan::GetInstance()
{
	if ( m_ptrInstance == NULL)
		m_ptrInstance = new D4FuncMan;
	return m_ptrInstance;
}

D4FuncMan::D4FuncMan()
{
    m_ptrDscpara = DscMan::GetInstance()->GetDscPara();
    m_tis = "";
    m_checkpart = "";
    m_ptrInstance = NULL;
    m_frameNum = 0;
    m_reverseFlag = 0;
    m_pD4bits = new unsigned char[440*256];
    memset(m_pD4bits,0,440*256);

    m_d4_mode = false;

    m_lineNo_bak = m_ptrDscpara->dcaCurScanStart;
    m_yBits = m_pD4bits + m_ptrDscpara->dcaCurScanStart*440;
}

D4FuncMan::~D4FuncMan()
{
    if (m_ptrInstance != NULL)
        delete m_ptrInstance;

    if (m_pD4bits != NULL)
        delete []m_pD4bits;
}

void D4FuncMan::GetPara(SUIS4DPARA &Para)
{
    GetProbeInfo();
    GetDepth();
    GetTIS();
    GetCheckPart();
    GetHospitalName();
#ifndef VET
    GetPatientInfo();
#endif
    GetTimeFormat();
    Para = m_Para4d;
}

void D4FuncMan::GetDataFromUsb(const unsigned char *pData)
{
    // DSCCONTROLATTRIBUTES* m_ptrDscpara = DscMan::GetInstance()->GetDscPara();
    DscMan::GetInstance()->GetWriteLock();
    unsigned int lineNo = 0;

    if (*(pData+16) == 0x02 && *(pData+17)==0xff && *(pData+18)==0x02 && *(pData+19)==0xff)
    {
        lineNo = *(pData+9);
        if (lineNo > m_ptrDscpara->dcaCurScanEnd || lineNo < m_ptrDscpara->dcaCurScanStart)
        {

            DscMan::GetInstance()->ReadWriteUnlock();
            return;
        }

        if (m_lineNo_bak > lineNo)
        {
            g_uis4d_ri.WriteDataToShm(g_sem_id0, m_pD4bits, m_frameNum, m_reverseFlag, g_pshm_data0);
            memset(m_pD4bits,0,440*256);
            m_yBits = m_pD4bits + m_ptrDscpara->dcaCurScanStart*440;
        }

        memcpy(m_yBits,pData+24,440);
        m_yBits += 440;
        m_lineNo_bak = lineNo;

        m_frameNum = *(pData+12);
        m_reverseFlag = *(pData+13);
        //printf("++++++++++++frame num:%d, reverseFlag=%d\n", data[0], data[1]);
    }

    DscMan::GetInstance()->ReadWriteUnlock();
    return;
}

void D4FuncMan::Exit4D()
{
    printf("\n\n ++++++++++++09001Color: Exit4D +++++++++++++++++++++++\n\n");

	D4FuncMan::GetInstance()->Set4DMode(false);

    // keyboard control
    KeyboardOversee((void *)&g_keyInterface, false);

    FreezeMode::GetInstance()->UnFreeze();

    Img2D::GetInstance()->SetLineDensity(1);
    Img2D::GetInstance()->SetMBP(1);

#ifndef EMP_322
	g_keyInterface.CtrlLight(FALSE, LIGHT_CW);
#endif

    SUIS4DPARA para;
    para = g_uis4d_ri.ReadConfigInfo();

    // achieve tgc from 4D
    g_tgcSlider[0] = para.tgc_0;
    g_tgcSlider[1] = para.tgc_1;
    g_tgcSlider[2] = para.tgc_2;
    g_tgcSlider[3] = para.tgc_3;
    g_tgcSlider[4] = para.tgc_4;
    g_tgcSlider[5] = para.tgc_5;
    g_tgcSlider[6] = para.tgc_6;
    g_tgcSlider[7] = para.tgc_7;

    Img2D::GetInstance()->SetGainFrom4D(para.gain);
    Img2D::GetInstance()->SetTGCFrom4D(g_tgcSlider);

	ScanMode::GetInstance()->Enter2D();
    //printf("%s,%d========================\n",__PRETTY_FUNCTION__,__LINE__);
#if 0
    string mode;
    mode = para.scan_mode;
    if(strcmp(mode.c_str(),"M")==0)
    {
        g_keyInterface.CtrlLight(TRUE,LIGHT_M);
        if (Img2D::GetInstance()->GetTpViewStatus() || Img2D::GetInstance()->GetEFVIStatus())
        {
            Img2D::GetInstance()->ExitTpView();
            Img2D::GetInstance()->ExitEFVI();
        }
        ScanMode::GetInstance()->EnterM();
    }
    else if(strcmp(mode.c_str(),"PW")==0)
    {
        if (Img2D::GetInstance()->GetTpViewStatus() || Img2D::GetInstance()->GetEFVIStatus())
        {
            Img2D::GetInstance()->ExitTpView();
            Img2D::GetInstance()->ExitEFVI();
        }
        ScanMode::GetInstance()->EnterPw();
    }
    else if(strcmp(mode.c_str(),"CFM")==0)
    {
        if (Img2D::GetInstance()->GetTpViewStatus() || Img2D::GetInstance()->GetEFVIStatus())
        {
            Img2D::GetInstance()->ExitTpView();
            Img2D::GetInstance()->ExitEFVI();
        }
         ScanMode::GetInstance()->EnterCfm();
    }
    else if(strcmp(mode.c_str(),"PDI")==0)
    {
        if (Img2D::GetInstance()->GetTpViewStatus() || Img2D::GetInstance()->GetEFVIStatus())
        {
            Img2D::GetInstance()->ExitTpView();
            Img2D::GetInstance()->ExitEFVI();
        }
        ScanMode::GetInstance()->EnterPdi();
    }
    else
    {
        ScanMode::GetInstance()->ExitSpecialMeasure();
#if (defined (EMP_322) || defined (EMP_313))
        ScanMode::GetInstance()->DarkAllModeLight();
        g_keyInterface.CtrlLight(TRUE,LIGHT_D2);
#endif
        ScanMode::GetInstance()->Enter2D();
    }
#endif
}

void D4FuncMan::GetProbeInfo()
{
    ProbeSocket::ProbePara para;
    ProbeMan::GetInstance()->GetCurProbe(para);

    int pr = 0;
    int pw = 0;
    char *tmp1  = new char[6];
    string fr;
    string theta;

    int scanangle = 0;
    Img2D::GetInstance()->GetScanAngle(scanangle);
    scanangle = scanangle * 100;

    if (para.type == 'T' || para.type == 't' || para.type == 'N' || para.type == 'n')    {
        para.type = 'C';
    }

    if ('C' == para.type)         //  convert
    {
        pr = para.r;
        fr = "1000";
        memset(tmp1, 0, 6);
        sprintf(tmp1,"%d", para.width);
        theta = tmp1;//para.width;
    }
    else  if('l' == para.type || 'L' == para.type)    // linear
    {
        fr = "0";
        pr = 0;
        pw = para.width/100;
    }
    else if ('V' == para.type)
    {
        pr = para.r;
        char *tmp  = new char[5];
        memset(tmp, 0, 5);
        sprintf(tmp,"%d", para.volumeR);
        fr = tmp;
        memset(tmp1, 0, 6);
        sprintf(tmp1,"%d", para.width);
        theta = tmp1;

        delete []tmp;
    }
    else
        para.type = 'C';

    delete []tmp1;

    //fps
    int fps = Img2D::GetInstance()->CalcFPS();

    // scan range
    int scan_range[2];
    Img2D::GetInstance()->GetDisplayScanRange(scan_range);

    // angle
    float angle;
    Img2D::GetInstance()->GetDisplayScanAngle(angle);
    angle = angle * 100;

    // samples
    DSCCONTROLATTRIBUTES* m_ptrDscPara = DscMan::GetInstance()->GetDscPara();
    int samples = m_ptrDscPara->dcaSamplePoints;

    m_Para4d.type = para.type;
    m_Para4d.lines = para.lines;
    m_Para4d.pr = pr / 100;
    m_Para4d.fr = fr;
    m_Para4d.pw = pw;
    m_Para4d.theta = theta;
    //m_Para4d.depth = para.depth;
    m_Para4d.model = para.model;
    m_Para4d.fps_real = ImageAreaPara::GetInstance()->GetGenFps();
    m_Para4d.fps = fps;//实际计算的帧率
    m_Para4d.fps_real = ImageAreaPara::GetInstance()->GetGenFps();
    m_Para4d.scan_range_begin = scan_range[0];
    m_Para4d.scan_range_end = scan_range[1];
    m_Para4d.samples = samples;
    m_Para4d.scan_angle = scanangle;
    m_Para4d.scan_angle = (int)angle;

    return;
}

void D4FuncMan::GetTIS()
{
    TopArea::GetInstance()->GetTIS(m_tis);
    m_Para4d.tis = m_tis;
    return;
}

void D4FuncMan::GetCheckPart()
{
    TopArea::GetInstance()->GetCheckPart(m_checkpart);
    m_Para4d.check_part = m_checkpart;
    return;
}

void D4FuncMan::GetDepth()
{
    int depth;
    depth = Img2D::GetInstance()->GetDepth();
    //TopArea::GetInstance()->GetDepth(depth);
    printf("depth:%d\n", depth);
    m_Para4d.depth = depth;
}

void D4FuncMan::GetHospitalName()
{
    string m_hospitalname;
    TopArea::GetInstance()->GetHospitalName(m_hospitalname);
    m_Para4d.hospital_name = m_hospitalname;
    return;
}
#ifndef VET
void D4FuncMan::GetPatientInfo()
{
    PatientInfo::Info info;
    g_patientInfo.GetInfo(info);
    PatientInfo::Patient pat = info.p;
    string name = pat.name.last + " " + pat.name.first + " " + pat.name.mid;
    string id_index = info.e.examNum;

    string age;
    GenAgeStr(pat.age, pat.ageUnit, age);
    m_Para4d.age = age;

    string sex;
    GetSexString(pat.sex, sex);
    m_Para4d.name = name;
    m_Para4d.sex = sex;
    m_Para4d.id = pat.id;
    m_Para4d.exam_id = id_index;
    return;
}
#endif

void D4FuncMan::GetTimeFormat()
{
    SysGeneralSetting sys;
    int date_format = sys.GetDateFormat();
    int language = sys.GetLanguage();
    m_Para4d.date_format = date_format;
    m_Para4d.language = language;
    m_Para4d.image_enhance = ImgProc2D::GetInstance()->GetImgEhnLevel();
    m_Para4d.framecorr_para = 32; // for test LWF
    m_Para4d.linecorr_para = 32; // for test LWF
    m_Para4d.date_format = date_format;
    return;
}