/**************************************************
 * 版权所有:深圳市恩普电子技术有限公司 (C) 2012
 * 文件名：UIS4DReceiveInterface.h  
 * 实现功能：定义与软件的通信接口函数
 * 作者：ZCT
 * 版本：v2.0
 * 日期：2012.2.16
 * ************************************************/
#ifndef _UIS4D_RECEIVEINTERFACE_H 
#define _UIS4D_RECEIVEINTERFACE_H
#include <iostream>
#include <string.h> 
#include <gtk/gtk.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
using namespace std; 

#define SHMKEY0 0x5006
#define SHMKEY1 0x5007
#define SEMKEY0 0x8006
#define SEMKEY1 0x8007
#define SCREENSAVER 0xff

#define QUIT_NUM 100  
#define FILE_PATH "/tmp/data.ini"

typedef struct tagUIS4DPara
{ 
    char type;           // 探头类型   C:凸帧  L：线阵 v:容积探头 
    int lines;           // 原始数据的扫描线数
    int pr;              // 凸阵半径（mm）
    int pw;              // 线阵有效的物理宽度（mm)
    int depth;           // 当前深度
    int date_format;     // 时间格式
    int fps;             // 当前界面显示的帧率
    int fps_real;        // 当前实际的帧率
    int scan_range_begin;// 扫描线开始位置
    int scan_range_end;  // 扫描线结束位置
    int samples;         // 采样点
    int language;        // 语言 0-english / 1-chinese
    int scan_angle;      // 当前扫描角度
    int image_enhance;   // 图像增强参数
    int framecorr_para;  // 帧相关参数
    int linecorr_para;   // 线相关参数
    int max_period;
    int tx;
    int mbp;
	int ezusb_id;		// EZUSB Device ID

    //TGC
    int depthMax;
    int imgScale;
    int gain;

    int tgc_0;
    int tgc_1;
    int tgc_2;
    int tgc_3;
    int tgc_4;
    int tgc_5;
    int tgc_6;
    int tgc_7;

    string scan_mode;    // scan mode: 2D, PW, CFM, PDI, M
    string check_part;   // 检查科别
    string tis;          //
    string theta;        // 凸阵有效扫描角度（rad）
    string fr;           // 扇扫半径（mm*100）
    string model;        // 当前探头标识  如：75L40K
    string sex;          // 病人性别
    string age;          // 病人年龄
    string id;           // 病人id
    string name;         // 病人姓名
    string hospital_name;// 医院名称
    string exam_id;      // 病人检查号
} SUIS4DPARA;

class CUIS4DReceiveInterface
{
    private:
        int m_shm_id0;
        int m_sem_id0;
        unsigned char *m_shm_data0;
    private:
        int SetSEMValue(int sem_id, int val);
        int GetSEMValue(int sem_id);
        void DelSEM(int sem_id);

    public:
        CUIS4DReceiveInterface();
        ~CUIS4DReceiveInterface();

        void IPC4DInit();                       // 创建2个share memory。
        void IPC4DDestroy();                    // 撤销共享内存的映射

        // 获得共享内存ID
        int GetSHMID(
                int index                      // ID索引 0:0号共享内存  1：1号共享内存
                );                

        // 获得共享内存数据
        unsigned char *GetSHMData(
                int index                      // 0：0号共享内存数据  1：1号共享内存数据
                );

        // P操作
        int SemaphoreP(
                int sem_id                     // 0: 0号信号量  1：1号信号量 
                );   

        // V操作
        int SemaphoreV(
                int sem_id                     // 0: 0号信号量  1：1号信号量 
                );   

        // 获得信号量ID
        int GetSEMID(
                int index                     // 0: 0号信号量  1：1号信号量       
                );  

        // 父进程中的API接口
        // 写共享内存
        void WriteDataToShm(
                int g_sem_id0,                // 共享内存ID
                unsigned char *pd4data,       // 源数据首地址
                unsigned char frame_num,      // 当前帧号
                unsigned char reverse_flag,   // 探头翻转标记
                unsigned char *g_pshm_data0   // 共享内存首地址
                );


        // 保存配置文件信息
        void SaveConfigInfo(SUIS4DPARA para);
        // 读取配置文件信息
        SUIS4DPARA ReadConfigInfo();
};

#endif
