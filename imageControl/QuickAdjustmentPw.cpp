#include "QuickAdjustmentPw.h"
#include "ImgPw.h"
#include "GlobalClassMan.h"
#include "HintArea.h"

// Pw
bool QuickAdjustmentPw::m_onQAPw = false;
bool QuickAdjustmentPw::m_shutInitialPw = false;

QuickAdjustmentPw* QuickAdjustmentPw::m_ptrInstance = NULL;

QuickAdjustmentPw* QuickAdjustmentPw::GetInstance()
{
	if(m_ptrInstance == NULL)
	{
		m_ptrInstance = new QuickAdjustmentPw;
	}

	return m_ptrInstance;
}

// constructor
QuickAdjustmentPw::QuickAdjustmentPw()
{
    m_spectrumSpeed = 48;;
    m_speedRatio = 0.1;
    m_pwLines = 100;  //计算一个心动周期的pw线数，设定prf=9.0kHz
    m_lineNo = 0;
    m_readPw = false;
    m_pPwBuf = NULL;
    m_shutPwChangeKnob = false;
    m_cout = 0;

	m_prfChg = 0;
	m_baselineChg = 0;
	m_refPRF = 0;
}

QuickAdjustmentPw::~QuickAdjustmentPw()
{
	if(m_ptrInstance != NULL)
	{
		delete m_ptrInstance;
		m_ptrInstance = NULL;
	}

    if (m_pPwBuf != NULL)
    {
        delete[] m_pPwBuf;
        m_pPwBuf = NULL;
    }
}


void QuickAdjustmentPw::QuickAdjustmentPwOn()
{
	if (!m_onQAPw)  //连按2次，响应先前一次的pw优化
	{
		m_shutInitialPw = true;   //屏蔽异常pw频谱线
		ImgPw* ptrImgPw = ImgPw::GetInstance();

		DscMan::GetInstance()->GetWriteLock();
		DscMan::GetInstance()->GetDsc()->ClearPWLine();
		DscMan::GetInstance()->ReadWriteUnlock();

		ptrImgPw->QAPwInitPara(m_refPRF);  //初始化prf, baseline, gain
		//DscMan::GetInstance()->GetDsc()->ClearPWLine();
		// 关闭发射脉冲占空比，获得噪声数据
		//GlobalClassMan::GetInstance()->GetCalc2D()->CalcWeightingEmitColor(0); // g30没有用这个fpga地址位，不能关闭
        ptrImgPw->QAPwCalcEmission(0);

		m_onQAPw = true;

		usleep(400000); //0.4s
		ptrImgPw->QAPwCalcEmission(1);
    }
}

void QuickAdjustmentPw::QuickAdjustmentPwOff()
{
	if (m_onQAPw)
    {
		m_onQAPw = false;
        // 正在接收pw数据时，切换到了其他模式，需关闭QAPw，初始化各变量
        SetParaChangeMode();
    }

	if (m_shutInitialPw)
		m_shutInitialPw = false;
}

// 从前端接收数据进行分析
void QuickAdjustmentPw::SendDataToQAPw(unsigned char *pPwData)
{
    ProbeSocket::ProbePara para;
	ProbeMan::GetInstance()->GetCurProbe(para);

    // 先设置缓存pw数据的参数
    if(!m_readPw)
    {
        m_spectrumSpeed = ImgPw::GetInstance()->GetKnobSpectrumSpeed();  //当前控制旋钮输入的频谱速度的重叠个数：（128-重叠个数）
        m_speedRatio = 48.0 / m_spectrumSpeed;   //频谱速度为0的重叠个数：（128-54）

        switch(para.type)
        {
            // 线阵
            case 'L':
                m_pwLines = static_cast<int>(1.5 * 8000 / m_spectrumSpeed + 90 * m_speedRatio + 0.5);  //计算线阵一个心动周期的pw线数
                break;

            // 凸阵
            case 'C':
                m_pwLines = static_cast<int>(1.8 * 8000 / m_spectrumSpeed + 90 * m_speedRatio + 0.5);  //计算凸阵阵一个心动周期的pw线数
                break;

            default:
                m_pwLines = static_cast<int>(1.5 * 8000 / m_spectrumSpeed + 90 * m_speedRatio + 0.5);
                break;
        }

        m_pPwBuf = new unsigned char[512 * m_pwLines];
        memset(m_pPwBuf, 0, 512 * m_pwLines);

        m_readPw = true;
        PRINTF("-----m_spectrumSpeed = %d\n", m_spectrumSpeed);
    }

    // 缓存pw数据
    if(m_readPw)
    {
        memcpy(m_pPwBuf + 512*m_lineNo, pPwData, 512);
        m_lineNo++;

        if (m_lineNo == static_cast<int>(110 * m_speedRatio))
        {
            m_shutInitialPw = false;
        }

        // 缓存结束
        if (m_lineNo == m_pwLines)
        {
            CalcQAPw(m_pPwBuf);

            m_lineNo = 0;
            m_onQAPw = false;
            m_readPw = false;

            delete [] m_pPwBuf;
            m_pPwBuf = NULL;

            m_shutInitialPw = true;
            m_shutPwChangeKnob = true;
        }
    }
}

// 正在接收pw数据时，切换到了其他模式，需关闭QAPw，初始化各变量
void QuickAdjustmentPw::SetParaChangeMode()
{
    if (m_readPw)
    {
        m_lineNo = 0;
        m_readPw = false;

        if (m_pPwBuf != NULL)
        {
            delete[] m_pPwBuf;
            m_pPwBuf = NULL;
        }
    }
}

// 由于控件调节后，pw数据会出现异常，将初始更新的pw数据不发给DSC
void QuickAdjustmentPw::SetParaChangeKnob(int pwFlag)
{
    if (4 == pwFlag)
    {
        if (m_shutPwChangeKnob)
        {
            m_cout++;
            if (m_cout == 49)
            {
                m_shutInitialPw = false;
                m_shutPwChangeKnob = false;
                m_cout = 0;
            }
        }
    }
}

void QuickAdjustmentPw::CalcQAPw(unsigned char* pImgData)
{
	int height = ImgPw::GetInstance()->GetPwDots();
    int width = m_pwLines;

	if(height>256)
		height = 256;
	unsigned char* ptrPwData = new unsigned char[width*height];
	memset(ptrPwData, 0, width*height);
	int i;
	for (i = 0; i < width; i++)
	{
		memcpy(ptrPwData+i*height, pImgData+24+i*512, height);
	}

	//PRINTF("---width = %d\n", width);

	AnalysisPw(ptrPwData, height, width);

	ImgPw::GetInstance()->QAPwSetPara(m_prfChg, m_baselineChg);

	delete []ptrPwData;
	ptrPwData = NULL;
}

// 均值，最小值、最大值、和计算
// flag为0表示计算均值，flag为1表示计算最小值，
// flag为2表示计算最大值, flag为3表示计算和
float QuickAdjustmentPw::MeanMaxMinCal(float *pSrc, int size, int flag)
{
	int i;
	float val;
	switch (flag)
        {
		case 3:
			{
				val = 0.0;
				for (i = 0; i < size; i++)
				{
					val = val + pSrc[i];
				}
			}
			break;
		case 2:
			{
				val = 0.0;
				for (i = 0; i < size; i++)
				{
					if (pSrc[i] > val)
						val = pSrc[i];
				}
			}
			break;
		case 1:
			{
				val = 10000.0;
				for (i = 0; i < size; i++)
				{
					if (pSrc[i] < val)
						val = pSrc[i];
				}
			}
			break;
		case 0:
			{
				val = 0.0;
				for (i = 0; i < size; i++)
				{
					val = val + pSrc[i];
				}
				val = val / size;
			}
			break;
	}
	return val;
}


void QuickAdjustmentPw::AnalysisPw(unsigned char *srcData, int height, int width)
{
	// 线阵、凸阵
	int invalidDataSize = static_cast<int>(90*m_speedRatio + 5 + 0.5);
	int noiseLine = static_cast<int>(55*m_speedRatio + 0.5);

	PRINTF("----noiseLine = %d, invalidDataSize = %d\n", noiseLine, invalidDataSize);

	int i,j,k,k0,k1,flag = 0;
	int startPos1, startPos2, num = 0;
	float signalTh, noiseTh, noiseMean = 0, noiseMax = 5.0;

	startPos1 = height / 2 - 3;
	startPos2 = height / 2 + 5;

	float *forwardMaxIndex = new float[width - invalidDataSize];
	float *reverseMaxIndex = new float[width - invalidDataSize];

	const int noiseSize = 3;
    unsigned char noisePwData;

	for (i = 0; i < width-invalidDataSize; i++)
	{
		forwardMaxIndex[i] = startPos1;
		reverseMaxIndex[i] = startPos2 ;
	}

	// calc noise
	for (i = noiseLine; i < noiseLine+noiseSize; i++)
	{
		for (j = 0; j < height; j++)
		{
			noisePwData = srcData[i * height + j];
			if (noisePwData > 0)
			{
				noiseMean = noiseMean + noisePwData;
				num++;
				if (noisePwData > noiseMax)
					noiseMax = noisePwData;
			}
		}
	}
	if (num > 0)
	{
		noiseMean = noiseMean / num;
	}
	else
	{
		noiseMean = 2;
	}

	//噪声阈值
	noiseTh = ((4*noiseMean) < noiseMax)? (4*noiseMean) : noiseMax;
	signalTh = ((6*noiseMean) > noiseMax)? (6*noiseMean) : noiseMax;

	// 搜寻频谱图的信号边界
	for (j = invalidDataSize; j < width; j++)
	{
		k0 = startPos1;
		for (i = startPos1; i >= 0; i--)
		{
			if (srcData[i+j*height] > noiseTh)
			{
				forwardMaxIndex[j-invalidDataSize] = i;
				k0 = i;
			}
			else
			{
				if ( (i-2)>=0 && (i+2)<=startPos1 )
				{
					if (srcData[i-2+j*height]<noiseTh && srcData[i-1+j*height]<noiseTh && srcData[i+1+j*height]<noiseTh && srcData[i+2+j*height]<noiseTh)
						break;
				}
			}
		}
		//考虑空窗的影响
		flag = 0;  //标记是否存在空窗，0表示边界点在空窗内
		for (k = k0; k >= 0; k--)
		{
			if ( (k-2)>=0 && (k+2)<=k0 )
			{
				if (srcData[k-2+j*height]>signalTh && srcData[k-1+j*height]>signalTh && srcData[k+1+j*height]>signalTh && srcData[k+2+j*height]>signalTh)
				{
					forwardMaxIndex[j-invalidDataSize] = k+2;
					flag = 1;
				}
				else
					if ((flag == 0) && (k < height / 4.0))
						break;
			}
		}
	}

	for (j = invalidDataSize; j < width; j++)
	{
		k1 = startPos2;
		for (i = startPos2; i < height; i++)
		{
			if (srcData[i+j*height] > noiseTh)
			{
				reverseMaxIndex[j-invalidDataSize] = i;
				k1 = i;
			}
			else
			{
				if ( (i-2)>=startPos2 && (i+2)<height )
				{
					if (srcData[i-2+j*height]<noiseTh && srcData[i-1+j*height]<noiseTh && srcData[i+1+j*height]<noiseTh && srcData[i+2+j*height]<noiseTh)
						break;
				}
			}
		}

		//考虑空窗的影响
		flag = 0;  //标记是否存在空窗
		for (k = k1; k < height; k++)
		{
			if ( (k-2)>=k1 && (k+2)<height )
			{
				if (srcData[k-2+j*height]>signalTh && srcData[k-1+j*height]>signalTh && srcData[k+1+j*height]>signalTh && srcData[k+2+j*height]>signalTh)
				{
					forwardMaxIndex[j-invalidDataSize] = k+2;
					flag = 1;
				}
				else
					if ((flag == 0) && (k < height * 3 / 4.0))
						break;
			}
		}
		//PRINTF("----rM[%d] = %.2f", j-invalidDataSize, reverseMaxIndex[j-invalidDataSize]);
	}

	// set prf and baseline
	float meanForwardMaxIndex, meanReverseMaxIndex;
	float maxForwardMaxIndex, maxReverseMaxIndex;
	float distMax, distForward, distReverse, distMean;

	meanForwardMaxIndex = MeanMaxMinCal(forwardMaxIndex, width-invalidDataSize, 0);
	meanReverseMaxIndex = MeanMaxMinCal(reverseMaxIndex, width-invalidDataSize, 0);
	maxForwardMaxIndex = MeanMaxMinCal(forwardMaxIndex, width-invalidDataSize, 1);
	maxReverseMaxIndex = MeanMaxMinCal(reverseMaxIndex, width-invalidDataSize, 2);
	PRINTF("---meanF=%.2f, meanR=%.2f, maxF=%.2f, maxR=%.2f\n", meanForwardMaxIndex, meanReverseMaxIndex, maxForwardMaxIndex, maxReverseMaxIndex);

	distForward = fabs(height/2.0 - maxForwardMaxIndex);
	distReverse = fabs(maxReverseMaxIndex - height/2.0);
	distMax = maxReverseMaxIndex - maxForwardMaxIndex;
	distMean = meanReverseMaxIndex - meanForwardMaxIndex;

	float distOpt;
	ProbeSocket::ProbePara para;
	ProbeMan::GetInstance()->GetCurProbe(para);
	switch(para.type)
	{
		case 'C':
			distOpt = 5.0 * height / 12.0;  //凸阵信号边界的合适位置
			break;
		case 'L':
			distOpt = 8.0 * height / 12.0;  //线阵信号边界的合适位置
			break;
		default:
			distOpt = 8.0 * height / 12.0;  //线阵信号边界的合适位置
			break;
	}

	float ratioPrfChg;
	if (distForward > distReverse)
	{
		distOpt = distOpt * distMean / distMax;
		ratioPrfChg = distMean / distOpt;
		m_prfChg = m_refPRF * ratioPrfChg;
		m_baselineChg = - static_cast<int>((distForward - distMax/2)/(height/16.0 * ratioPrfChg) + 0.8);
	}
	else
	{
		distOpt = distOpt * distMean / distMax;
		ratioPrfChg = distMean / distOpt;
		m_prfChg = m_refPRF * ratioPrfChg;
		m_baselineChg = static_cast<int>((distReverse - distMax/2)/(height/16.0 * ratioPrfChg) + 0.8);
	}

	//凸阵约束条件
	switch(para.type)
	{
		case 'C':
			if (m_prfChg < 1500)
				m_prfChg = 1500;
			break;
		case 'L':
			break;
		default:
			break;
	}

	PRINTF("-----noiseMean = %.2f, noiseMax = %.2f, noiseTh = %.2f, signalTh = %.2f, ratioPrfChg = %.2f, refPRF = %d\n", noiseMean, noiseMax, noiseTh, signalTh, ratioPrfChg, m_refPRF);
	PRINTF("---distOpt = %.2f, distMax=%.2f, distForward=%.2f, distReverse=%.2f\n", distOpt, distMax, distForward, distReverse);

	delete[] forwardMaxIndex;
	forwardMaxIndex = NULL;
	delete[] reverseMaxIndex;
	reverseMaxIndex = NULL;
}

void QuickAdjustmentPw::SaveImage(unsigned char *pBits, int size)
{
    /*
	char strHint[256];
	char filename[256];
	int year, month, day, hour, minute, second;
	GetCurrentDateTime(year, month, day, hour, minute, second);
	//int spectrumSpeed = ImgPw::GetInstance()->GetKnobSpectrumSpeed();

	sprintf(filename, "%4d%2d%2d%2d%2d%2d %d speed %d.bin", year, month, day, hour, minute, second, size, m_spectrumSpeed);
	FILE *file = fopen(filename, "w+b");
	fwrite(pBits, 1, size, file);
	fclose(file);
	sprintf(strHint, "Save succefully! filename is %s\n", filename);
	gdk_threads_enter();
	HintArea::GetInstance()->UpdateHint(_(strHint), 1);
	gdk_threads_leave();*/
}