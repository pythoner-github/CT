/**************************************************
 * 版权所有：深圳市恩普电子技术有限公司(C) 2012
 * 文件名：IMT.h  
 * 实现功能：Intima Media Thickness
 * 作者：ZCT
 * 版本：V1.1
 * 日期：2012.4.2
 * ************************************************/
#ifndef _IMH_H 
#define _IMH_H

typedef unsigned char IMT_BYTE;
typedef unsigned int IMT_UINT;

typedef struct tagIMTPara
{ 
    float imt_max;	           // IMT MAX
    float imt_min;		       // IMT MIN
    float imt_std;		       // IMT 标准差
    float imt_mean;		       // IMT 平均值
    float imt_length;		   // IMT 测量长度
    float imt_quality;	 	   // IMT 品质因子
} SIMTPARA;

class CIMT
{       
    private:
        static CIMT *m_ptrInstance;
        bool m_status; //  CalcTrack 函数返回状态标记,true:success  false:fail
        unsigned int m_width;
        unsigned int m_height;
        IMT_BYTE *m_pspace_data;   // 原始数据
        IMT_BYTE *m_pspace_tmp;    // 原始数据临时备份
        IMT_BYTE *m_pspace_mask;
    private:
        CIMT();

        // 初始化 
        void Initialization(void);    

        //  预处理
        void Preprocess(IMT_BYTE *roi); 

        //
        void Mask(IMT_BYTE *mask, IMT_BYTE *img, IMT_UINT w, IMT_UINT h, float *mu, IMT_UINT k);

        //
        void UpdateMeans(IMT_BYTE *img, IMT_UINT *hstgrm, float *dmu, IMT_UINT k);

        // ind图像微量中所有出现次数非0的值
        // ind_num是ind数组的大小
        void Cluster(IMT_UINT *ind, IMT_UINT ind_num, float *mu, IMT_UINT k);

        // find max value
        IMT_UINT Max(IMT_BYTE *pdata, IMT_UINT len);

        // get histogram 
        void GetHstgrm(IMT_BYTE *img, IMT_UINT *hstgrm, IMT_UINT len);

        // K-Means 分离器
        void KMeans(IMT_BYTE *mask, IMT_BYTE *roi, IMT_UINT h, IMT_UINT w, IMT_UINT class_num);

        // 线性拟合
        float LineFitting(IMT_UINT x[], IMT_UINT y[], IMT_UINT len);

        // 计算IM厚度
        void CalThickness(float thickness[], const IMT_UINT *intima, const IMT_UINT *adventitia, IMT_UINT len);

        // 计算品质因子
        float CalSSIM(const IMT_BYTE *img1, const IMT_BYTE *img2, IMT_UINT len);

    public:
        // 获得该类的实例
        static CIMT * GetInstance();       

        // 析构函数
        ~CIMT();       

        // 描记线计算
        void CalcTrack(
                IMT_UINT *intima,       // 内膜描迹曲线首地址 
                IMT_UINT *adventitia,   // 外膜描迹曲线首地址
                const IMT_BYTE *roi,    // 感兴趣区域首地址
                IMT_UINT h,             // 感兴趣区域高度
                IMT_UINT w,             // 感兴趣区域宽度
                bool dir                // 描迹方向  true:down  false:up
                );

        // 统计数据，最大，最小，标准差，平均和品质因子，部分单位是像素值
        bool AnalyseData(
                SIMTPARA &imt_para,           // 统计数据
                const IMT_BYTE *curdata,      // 当前图像ROI首地址
                const IMT_BYTE *predata,      // 先前图像ROI首地址
                const IMT_UINT *intima,       // 内膜描迹曲线首地址
                const IMT_UINT *adventitia,   // 外膜描迹曲线首地址
                IMT_UINT len                  // 描迹宽(长)度
                );
};
#endif
