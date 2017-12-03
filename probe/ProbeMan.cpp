/*
 * 2009, 深圳恩普电子技术有限公司
 *
 * @file: ProbeMan.cpp
 * @brief: Manage all socket in system. all operation on socket must delegate by class ProbeMan
 *
 * version: V1.0
 * date: 2009-5-9
 * @author: zhanglei
 */

#include "ProbeMan.h"
#include "Def.h"
#include "ProbeList.h"
#include "ProbeList-360.h"
#include "ProbeList-322.h"
#include "ProbeList-430.h"
#include "ProbeList-440.h"
#include "ProbeList-355.h"
#include "CalcTime.h"
#include "FpgaGeneral.h"
#include "IniFile.h"

ProbeMan* ProbeMan::m_ptrInstance = NULL;
#if (defined(EMP_360)||defined(EMP_440))
const int ProbeMan::CMD_READ_PARA[MAX_SOCKET] = {0x0, 0x2, 0x1, 0x3};
const int ProbeMan::SOCKET_ADDR[MAX_SOCKET] = {0x0, 0x2, 0x1, 0x3};
#elif defined(EMP_161)
const int ProbeMan::CMD_READ_PARA[MAX_SOCKET] = {0x0, 0x2, 0x1};
const int ProbeMan::SOCKET_ADDR[MAX_SOCKET] = {0x0, 0x2, 0x1};
#elif (defined(EMP_322) || defined(EMP_313))
const int ProbeMan::CMD_READ_PARA[MAX_SOCKET] = {0x0, 0x0, 0x0};//探头地址
const int ProbeMan::SOCKET_ADDR[MAX_SOCKET] = {0x2, 0x1, 0x0}; //探头座切换，继电器切换
#elif (defined(EMP_430))
const int ProbeMan::CMD_READ_PARA[MAX_SOCKET] = {0x0, 0x0, 0x0, 0x0};
const int ProbeMan::SOCKET_ADDR[MAX_SOCKET] = {0x3, 0x1, 0x2, 0x0};
#elif (defined(EMP_355))
const int ProbeMan::CMD_READ_PARA[MAX_SOCKET] = {0x2, 0x2, 0x2};// 0x3};
//const int ProbeMan::CMD_READ_PARA[MAX_SOCKET] = {0x1, 0x2, 0x0};// 0x3};
const int ProbeMan::SOCKET_ADDR[MAX_SOCKET] = {0x0, 0x2, 0x1};// 0x3};
//const int ProbeMan::SOCKET_ADDR[MAX_SOCKET] = {0x0, 0x1, 0x2};// 0x3};
#else
const int ProbeMan::CMD_READ_PARA[MAX_SOCKET] = {0x0, 0x2, 0x1, 0x3};
const int ProbeMan::SOCKET_ADDR[MAX_SOCKET] = {0x0, 0x2, 0x1, 0x3};
//const int ProbeMan::CMD_READ_PARA[MAX_SOCKET] = {0x0, 0x1, 0x2, 0x3};
//const int ProbeMan::SOCKET_ADDR[MAX_SOCKET] = {0x0, 0x1, 0x2, 0x3};
#endif

extern bool g_init;

ProbeMan::ProbeMan()
{
	ProbeSocket::ProbePara para;
	int i;
	int size;

    para.exist = TRUE;
#if (defined(EMP_360) || defined(EMP_161) || defined(EMP_440))
	para.model[0] = '3';
	para.model[1] = '5';
	para.model[2] = 'C';
	para.model[3] = '6';
	para.model[4] = '0';
	para.model[5] = 'E';
#elif (defined(EMP_322) || defined(EMP_313))
	para.model[0] = '3';
	para.model[1] = '5';
	para.model[2] = 'C';
	para.model[3] = '5';
	para.model[4] = '0';
	para.model[5] = 'J'; 
#elif defined(EMP_430)
	para.model[0] = '3';
	para.model[1] = '5';
	para.model[2] = 'C';
	para.model[3] = '6';
	para.model[4] = '0';
	para.model[5] = 'E';
#elif (defined(EMP_355))
	para.model[0] = '3';
	para.model[1] = '5';
	para.model[2] = 'C';
	para.model[3] = '5';
	para.model[4] = '0';
	para.model[5] = 'L';
#else
	para.model[0] = '3';
	para.model[1] = '5';
	para.model[2] = 'C';
	para.model[3] = '5';
	para.model[4] = '0';
	para.model[5] = 'K';
#endif
	para.model[6] = 0;
	para.type = 'C';
#if defined (EMP_430)
 	para.width = 6000; 
    para.array = 80;    
    para.lines = 160;   
    para.r = 60; 
#else
    para.width = 7200;
    para.array = 128;
    para.lines = 256;
    para.r = 50;    
#endif
    para.widthPhase = 0;
    para.depth = 300;
    para.freqRange[0] = 40;
    para.freqRange[1] = 120;
    para.freqDop[0] = 54;
    para.freqDop[1] = 66;
    para.vecFreqRange.clear();
    size = 7;
    for (i = 0; i < size; i ++)
    {
        para.vecFreqRange.push_back(ProbeSocket::FREQ2D[0][i]);
    }
    for (i = 0; i < ProbeSocket::MAX_HARMONIC_FREQ; i++)
    {
        para.thiFreq[i] = ProbeSocket::FREQ_THI[0][i];
    }
    para.exist = TRUE;

    ///> init all sockets
    ProbeSocket socket0(SOCKET_ADDR[0], CMD_READ_PARA[0]);	
    //socket0.SetProbePara(para);
    m_vecSockets.push_back(socket0);

    if (MAX_SOCKET > 1)
    {
        ProbeSocket socket1(SOCKET_ADDR[1], CMD_READ_PARA[1]);	
        //socket1.SetProbePara(para);
        m_vecSockets.push_back(socket1);
    }

    if (MAX_SOCKET > 2)
    {
        ProbeSocket socket2(SOCKET_ADDR[2], CMD_READ_PARA[2]);	
        //socket2.SetProbePara(para);
        m_vecSockets.push_back(socket2);
    }

    if (MAX_SOCKET > 3)
    {
        ProbeSocket socket3(SOCKET_ADDR[3], CMD_READ_PARA[3]);	
        //socket3.SetProbePara(para);
        m_vecSockets.push_back(socket3);
    }

    ///> create default socket, used when no probe is exist in all sockets 
    //35C50K
#if 0
	para.exist = TRUE;
#if (defined(EMP_360) || defined(EMP_161))
	para.model[0] = '3';
	para.model[1] = '.';
	para.model[2] = '5';
	para.model[3] = 'C';
	para.model[4] = 'V';
	para.model[5] = 0;
#elif (defined(EMP_313) || defined(EMP_322))
	para.model[0] = '3';
	para.model[1] = '5';
	para.model[2] = 'C';
	para.model[3] = '5';
	para.model[4] = '0';
	para.model[5] = 'J';
#elif (defined(EMP_430))
	para.model[0] = '3';
	para.model[1] = '5';
	para.model[2] = 'C';
	para.model[3] = '6';
	para.model[4] = '0';
	para.model[5] = 'E';
#elif (defined(EMP_355))
	para.model[0] = '3';
	para.model[1] = '5';
	para.model[2] = 'C';
	para.model[3] = '5';
	para.model[4] = '0';
	para.model[5] = 'L'; 
#else
	para.model[0] = '3';
	para.model[1] = '.';
	para.model[2] = '5';
	para.model[3] = 'C';
	para.model[4] = 'V';
	para.model[5] = 0;
#endif
	para.model[6] = 0;
	para.type = 'C';
#if defined (EMP_430)
   para.width = 6000; 
    para.array = 80;   
    para.lines = 160;  
    para.r = 60; 
#else
    para.width = 7200;
    para.array = 128;
    para.lines = 256;
    para.r = 50;
#endif
    para.widthPhase = 0;
	para.depth = 300;
	para.freqRange[0] = 40;
	para.freqRange[1] = 120;
	para.freqDop[0] = 54;
	para.freqDop[1] = 66;
	para.vecFreqRange.clear();
	size = 7;
	for (i = 0; i < size; i ++)
	{
		para.vecFreqRange.push_back(ProbeSocket::FREQ2D[0][i]);
	}
		
#endif

// 75L40K
#if 1
	para.exist = TRUE;
#if (defined(EMP_360) || defined(EMP_161))
	para.model[0] = '7';
	para.model[1] = '.';
	para.model[2] = '5';
	para.model[3] = 'L';
	para.model[4] = 'V';
	para.model[5] = 'S';
#elif (defined(EMP_430))
	para.model[0] = '6';
	para.model[1] = '5';
	para.model[2] = 'L';
	para.model[3] = '4';
	para.model[4] = '0';
	para.model[5] = 'E';
#elif (defined(EMP_355))
	para.model[0] = '7';
	para.model[1] = '0';
	para.model[2] = 'L';
	para.model[3] = '4';
	para.model[4] = '0';
	para.model[5] = 'J';
#else
	para.model[0] = '7';
	para.model[1] = '5';
	para.model[2] = 'L';
	para.model[3] = '4';
	para.model[4] = '0';
	para.model[5] = 'K';
#endif
	para.model[6] = 0;
	para.type = 'l';
#ifdef EMP_430
	para.width = 4000;
	para.array = 80;
	para.lines = 160;
#else
	para.width = 3800;
	para.array = 128;
	para.lines = 256;
#endif
	para.r = 1;
    para.widthPhase = 0;
	para.depth = 120;
	para.freqRange[0] = 100;
	para.freqRange[1] = 240;
	para.freqDop[0] = 118;
	para.freqDop[1] = 144;
    para.vecFreqRange.clear();
	size = ProbeSocket::FREQ2D_SUM[1];
	for (i = 0; i < size; i ++)
	{
		para.vecFreqRange.push_back(ProbeSocket::FREQ2D[1][i]);
	}	
    for (i = 0; i < ProbeSocket::MAX_HARMONIC_FREQ; i++)
    {
        para.thiFreq[i] = ProbeSocket::FREQ_THI[0][i];
    }

#endif

#if 0
	para.exist = TRUE;
	para.model[0] = '6';
	para.model[1] = '5';
	para.model[2] = 'C';
	para.model[3] = '1';
	para.model[4] = '0';
	para.model[5] = 'K';
	para.model[6] = 0;
	para.type = 't';
	para.width = 15000;
	para.array = 128;
	para.lines = 256;
	para.r = 10;
    para.widthPhase = 0;
	para.depth = 120;
	para.freqRange[0] = 100;
	para.freqRange[1] = 180;
	para.freqDop[0] = 98;
	para.freqDop[1] = 116;
	size = 3;
	for (i = 0; i < size; i ++)
	{
		para.vecFreqRange.push_back(ProbeSocket::FREQ2D[2][i]);
	}	
#endif

#if 0
	para.exist = TRUE;
	para.model[0] = '3';
	para.model[1] = '5';
	para.model[2] = 'C';
	para.model[3] = '2';
	para.model[4] = '0';
	para.model[5] = 'H';
	para.model[6] = 0;
	para.type = 'N';
	para.width = 8800;
	para.array = 96;
	para.lines = 192;
	para.r = 20;
    para.widthPhase = 0;
	para.depth = 300;
	para.freqRange[0] = 50;
	para.freqRange[1] = 120;
	para.freqDop[0] = 76;
	para.freqDop[1] = 86;
	size = 4;
	for (i = 0; i < size; i ++)
	{
		para.vecFreqRange.push_back(ProbeSocket::FREQ2D[3][i]);
	}	
#endif

#if 0//65C15D
	para.exist = TRUE;
	para.model[0] = '6';
	para.model[1] = '5';
	para.model[2] = 'C';
	para.model[3] = '1';
	para.model[4] = '5';
	para.model[5] = 'D';
	para.model[6] = 0;
	para.type = 'n';
	para.width = 9800;
	para.array = 80;
	para.lines = 160;
	para.r = 15;
    para.widthPhase = 0;
	para.depth = 120;
	para.freqRange[0] = 120;
	para.freqRange[1] = 180;
	para.freqDop[0] = 98;
	para.freqDop[1] = 116;
	size = 3;
	for (i = 0; i < size; i ++)
	{
		para.vecFreqRange.push_back(ProbeSocket::FREQ2D[4][i]);
	}	
#endif
#if 0//30P16A
	para.exist = TRUE;
	para.model[0] = '3';
	para.model[1] = '0';
	para.model[2] = 'P';
	para.model[3] = '1';
	para.model[4] = '6';
	para.model[5] = 'A';
	para.model[6] = 0;
	para.type = 'P';
	para.width = 9000;
	para.array = 64;
	para.lines = 256;
	para.r = 15;
    para.widthPhase = 0;
	para.depth = 120;
	para.freqRange[0] = 120;
	para.freqRange[1] = 180;
	para.freqDop[0] = 98;
	para.freqDop[1] = 116;
	size = 3;
	for (i = 0; i < size; i ++)
	{
		para.vecFreqRange.push_back(ProbeSocket::FREQ2D[4][i]);
	}	
#endif
 //10L25J
#if 0
#ifdef EMP_340
	para.exist = TRUE;
	para.model[0] = '1';
	para.model[1] = '0';
	para.model[2] = 'L';
	para.model[3] = '2';
	para.model[4] = '5';
	para.model[5] = 'J';
	para.model[6] = 0;
	para.type = 'l';
	para.width = 3800;
	para.array = 128;
	para.lines = 256;
	para.r = 1;
    para.widthPhase = 0;
	para.depth = 120;
	para.freqRange[0] = 100;
	para.freqRange[1] = 240;
	para.freqDop[0] = 118;
	para.freqDop[1] = 144;
    para.vecFreqRange.clear();
	size = ProbeSocket::FREQ2D_SUM[7];
	for (i = 0; i < size; i ++)
	{
		para.vecFreqRange.push_back(ProbeSocket::FREQ2D[7][i]);
	}	
    for (i = 0; i < ProbeSocket::MAX_HARMONIC_FREQ; i++)
    {
        para.thiFreq[i] = ProbeSocket::FREQ_THI[7][i];
    }
#endif
#endif
    //四个座都没有读取到探头时，默认读取的探头座.此时读取不到探头参数，因为cmd的数据设置为0xff了
	ProbeSocket ps(SOCKET_ADDR[2], 0Xff); //default socket is socket0, and can not read probe para(read command set to 0xff)
    //默认设置的该探头座上的参数
	ps.SetProbePara(para);
	m_vecSockets.push_back(ps);

    m_defaultSocket = MAX_SOCKET;
}

ProbeMan::~ProbeMan()
{
	if (m_ptrInstance != NULL)
		delete m_ptrInstance;
}

///> public 
ProbeMan* ProbeMan::GetInstance()
{
	if (m_ptrInstance == NULL)
	{
		m_ptrInstance = new ProbeMan;
	}

	return (m_ptrInstance);
}
/*
 * @brief power on or off HV(high voltage)
 *
 * @para on represent HV status
 *  on = TRUE, power on HV
 *  on = FALSE, power off HV
 */
void ProbeMan::ActiveHV(bool on)
{
   PRINTF("enter the probeMan high value = %d\n",on);
	m_vecSockets[m_curSocket].ActiveHV(on);
}

/*
 * @brief get probe parmeter of current active probe, if no probe is existed.
 *  attention: return default probe parameter
 *
 *  @para save probe paremter returned.
 */
void ProbeMan::GetCurProbe(ProbeSocket::ProbePara &para)
{
#if 0
    char str_path[256];  
    sprintf(str_path, "%s/%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini(str_path);
#endif
    m_vecSockets[m_curSocket].GetProbePara(para);
    PRINTF("-----current model %d %s\n", m_curSocket, para.model);
}

/*
 * @brief get default probe parameter when no probe is plugged into system
 */
void ProbeMan::GetDefaultProbe(ProbeSocket::ProbePara &para)
{
	m_vecSockets[MAX_SOCKET].GetProbePara(para);
}
/*
 * @brief get para of all probe and saved in para[]
 *
 * @para para[0] saved the para of first probe
 */
void ProbeMan::GetAllProbe(ProbeSocket::ProbePara para[MAX_SOCKET])
{
    char defaultModel[256];
    if (g_init)
    {
        char path[256];
        sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
        IniFile ini(path);
        strcpy(defaultModel, ReadDefaultProbe(&ini).c_str());
    }
#ifdef EMP_355
        m_vecSockets[2].ActiveSocket();
        usleep(430000);//
#endif
    int i;
    for (i = 0; i < MAX_SOCKET; i ++)
	{
#if (defined(EMP_322) || defined(EMP_313)|| defined(EMP_430)) 
        m_vecSockets[i].ActiveSocket();
        usleep(900000);//
#endif
		
#ifdef EMP_355
        m_vecSockets[i].ActiveSocket();
        usleep(430000);//
#endif
		m_vecSockets[i].ReadProbePara();
		m_vecSockets[i].GetProbePara(para[i]);
        if (g_init)
        {
            if (strcmp(defaultModel, para[i].model) == 0)
                m_defaultSocket = i;
		}
	}
#ifdef EMP_355
	if ((para[0].type==para[1].type)&&(para[1].type==para[2].type))       //the same as one probe addr
	{
		para[1].exist=false;
		para[2].exist=false;
	}
#endif
   //g_init = false;
}
//test for A60
void ProbeMan::GetOneProbe(ProbeSocket::ProbePara para[MAX_SOCKET], int socket)
{
    m_vecSockets[socket].ActiveSocket();
    usleep(900000); //900ms
    m_vecSockets[socket].ReadProbePara();
    m_vecSockets[socket].GetProbePara(para[socket]);
}

/*
 * @brief set designated socket as current
 *
 * @para socket designate socket number
 *  range(0 ~ MAX_SOCKET)
 *  Attention: socket = MAX_SOCKET only used when no probe is eixsted.
 */
int ProbeMan::SetProbeSocket(int socket)
{
	///> select socket
	if (socket <= MAX_SOCKET)
	{
		m_curSocket = socket;
	}
	else
	{
		PRINTF("ProbeMan: socket is out of range\n");
		return FALSE;
	}

    m_vecSockets[m_curSocket].ActiveSocket();
	return TRUE;
}

void ProbeMan::WriteProbeManual()
{
    unsigned char* buf = NULL;
    // EMP-360
    //buf = Probe35CV;
    //buf = Probe35MC;
    //buf = Probe75LVS;
    //buf = Probe65VMC;
    //buf = Probe65MC;
    //buf = Probe30P16A;

    // EMP-161
    //buf = Probe35CV;
    //buf = Probe35MC;
    //buf = Probe75LVS;
    //buf = Probe65VMC;
 
    // EMP-322
    //buf = Probe35C50J;
    //buf = Probe35C20G;
    //buf = Probe75L40J;
    //buf = Probe65C10J;
    //buf = Probe65C20G;
    //buf = Probe90L40J;
   

    // EMP-340
    buf = Probe35C50K;
    //buf = Probe35C20H;
    //buf = Probe75L40K;
    //buf = Probe65C10K;
    //buf = Probe65C15D;
    //buf = Probe30P16A;
   // buf = Probe90L25K;
    //buf = Probe10L25K;
    //buf = Probe35D40J;

	//printf("Begin Write Probe!\n");
	m_vecSockets[0].ActiveSocket();
	usleep(100000);

	int i;
	for (i = 0; i < MAX_SOCKET; i ++)
	{
#if (defined(EMP_430) || defined(EMP_322))
        if (i > 0)
        {
            m_vecSockets[i].ActiveSocket();
            usleep(100000);
        }
#endif
        m_vecSockets[i].WriteProbePara(buf, PROBE_INFO_SIZE);
        usleep(100000);
    }
    PRINTF("end Write Probe!\n");
}

void ProbeMan::WriteProbe(int type)
{
    unsigned char* buf = NULL;

#if (defined(EMP_360) || defined(EMP_161))
    switch((EProbeList360)type)
    {
        case P35CV:
            buf = Probe35CV;
            break;

        case P75LVS:
            buf = Probe75LVS;
            break;

        case P65VMC:
            buf = Probe65VMC;
            break;

        case P35MC:
            buf = Probe35MC;
            break;

        case P65MC:
            buf = Probe65MC;
            break;

        case PT30P16A:
            buf = Probe30P16A;
            break;

        case PT35D40JSNR:
            buf = Probe35D40JSNR;
            break;

        case PT35D40JNDK:
            buf = Probe35D40JNDK;
            break;

        default:
            buf = Probe35CV;
            break;
    }
#elif (defined(EMP_322) || defined(EMP_313))
    switch((EProbeList322)type)
    {

        case P35C50J:
            buf = Probe35C50J;
            break;

        case P75L40J:
            buf = Probe75L40J;
            break;

        case P65C10J:
            buf = Probe65C10J;
            break;

        case P35C20G:
            buf = Probe35C20G;
            break;

        case P65C20G:
            buf = Probe65C20G;
            break;

        case P90L40J:
            buf = Probe90L40J;
            break;

        case PL30P16A:
            buf = Probe30P16A;
            break;
        
        case PL35D40JSNR:
            buf = Probe35D40JSNR;
            break;

        case PL35D40JNDK:
            buf = Probe35D40JNDK;
            break;

        default:
            buf = Probe35C50J;
            break;
    }
#elif defined(EMP_430)
    switch((EProbeList430)type)
    {
        case P35C60E:
            buf = Probe35C60E;
            break;

        case P65L40E:
            buf = Probe65L40E;
            break;

        case P65C10E:
            buf = Probe65C10E;
            break;
        
        case PT65C15D:
            buf = ProbeT65C15D;
            break;

        case P35D50D:
            buf = Probe35D50D;
            break;

        default:
            buf = Probe35C60E;
            break;
    }

#elif defined(EMP_440)
    switch((EProbeList440)type)
    {
        
        case PT65C15D:
            buf = ProbeT65C15D;
            break;

         case P35CV:
            buf = Probe35CV;
            break;

        case P65VMC:
            buf = Probe65VMC;
            break;

        case P75LVS:
            buf = Probe75LVS;
            break;

        default:
            buf = Probe35CV;
            break;
    }
#elif defined(EMP_355)
    switch((EProbeList355)type)
    {
        case P35C50LAPX:
            buf = Probe35C50LAPX;
            break;
        
        case P35C50LEMP:
            buf = Probe35C50LEMP;
            break;

        case P35C20I:
            buf = Probe35C20I;
            break;

        case P70L40J:
            buf = Probe70L40J;
            break;
        
        case P75L40JAPX:
            buf = Probe75L40JAPX;
            break;
        
        case P75L40JEMP:
            buf = Probe75L40JEMP;
            break;

        case P70L60J:
            buf = Probe70L60J;
            break;

        case P90L40J_1:
            buf = Probe90L40J_1;
            break;

        case P65C10L:
            buf = Probe65C10L;
            break;

        case P65C15E_80:
            buf = Probe65C15E_80;
            break;

        case P65C15E_128:
            buf = Probe65C15E_128;
            break;

        case P30P16B:
           buf = Probe30P16B;
            break;

        case P10L25K_1:
            buf = Probe10L25K_1;
            break;

        case P65C10I:
            buf = Probe65C10I;
            break;
#ifdef VET
        case P55L60G:
            buf = Probe55L60G;
            break;
#endif 
      
        default:
            buf = Probe35C50LAPX;
            break;
    }
#else
    switch((EProbeList340)type)
    {
        case P35C50K:
            buf = Probe35C50K;
            break;

        case P75L40K:
            buf = Probe75L40K;
            break;

        case P65C10K:
            buf = Probe65C10K;
            break;

        case P35C20H:
            buf = Probe35C20H;
            break;

        case P65C15D:
            buf = Probe65C15D;
            break;

        case P30P16A:
            buf = Probe30P16A;
            break;

        case P90L25K:
            buf = Probe90L25K;
            break;

        case P10L25J:
            buf = Probe10L25K;
            break;

        case P35D40JSNR:
            buf = Probe35D40JSNR;
            break;
        
        case P35D40JNDK:
            buf = Probe35D40JNDK;
            break;
#ifdef VET
        case P55L60H:
            buf = Probe55L60H;
            break;
#else
        case P65C10H:
            buf = Probe65C10H;
            break;
#endif

        default:
            buf = Probe35C50K;
            break;
    }
#endif

    PRINTF("write probe: buf = %s, size = %d\n", buf, PROBE_INFO_SIZE);
    m_vecSockets[0].ActiveSocket();
    usleep(100000);

	int i;
	for (i = 0; i < MAX_SOCKET; i ++)
	{
#if (defined(EMP_430) || defined(EMP_322))
        if (i > 0)
        {
            m_vecSockets[i].ActiveSocket();
            usleep(100000);
        }
#endif
		m_vecSockets[i].WriteProbePara(buf, PROBE_INFO_SIZE);
		usleep(100000);
	}
}

/**
 *write probe to file
 */
void ProbeMan::WriteDefaultProbe(const char* probeModel, IniFile* ptrIni)
{
// write default probe from file
	ptrIni->WriteString("ProbeModel", "ProbeModel", probeModel);
    ptrIni->SyncConfigFile();
}

/**
 * read the last probe before close machine, and switch to this probe when open machine
 */
string ProbeMan::ReadDefaultProbe(IniFile* ptrIni)
{
	// read default probe from file
    PRINTF("probemodel:%s\n", ptrIni->ReadString("ProbeModel", "ProbeModel").c_str());
	return (ptrIni->ReadString("ProbeModel", "ProbeModel"));
}

void ProbeMan::VerifyProbeName(const char *fromType, char *toType)
{
   sprintf(toType, "%s", fromType);
}

/**
 *@brief: read the probe alias for oem machine
 */
void ProbeMan::ReadProbeAlias(IniFile *ini, const char *model, char *alias)
{
    sprintf(alias, "%s", (char*)ini->ReadString(model, "Alias").c_str());
}

/**
 *@brief: write the probe alias for oem machine
 */
void ProbeMan::WriteProbeAlias(IniFile *ini, const char *model, char *alias)
{
    ini->WriteString(model, "Alias", alias);
    ini->SyncConfigFile();
}

