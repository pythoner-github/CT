/*
 * 2009, 深圳恩普电子技术有限公司
 *
 * @file: CalcPw.cpp
 * @brief: abstract class, in charge of cfm/pdi imaging calculation
 *
 * version: V1.0
 * date: 2009-5-21
 * @author: zhanglei
 */

#include "imageControl/CalcCfm.h"
#include "imageControl/FpgaCtrl2D.h"
#include "probe/ProbeMan.h"
#include "probe/ProbeSocket.h"
#include "imageControl/Calc2D.h"

//filter parament
const unsigned char CalcCfm::m_filterLowPass[30720] = //480*32*2
{
#include "res/filter/Convex/dync_lp_12M_0M_025_480seg.h"
};

CalcCfm::CalcCfm()
{
}
CalcCfm::~CalcCfm()
{
}

void CalcCfm::CalcBox(int lineBegin, int lineEnd, int dotBegin, int dotEnd)
{
//	printf("----------CFM BOX fpga:lineb = %d, linee = %d, dotb = %d, dote = %d\n", lineBegin, lineEnd, dotBegin, dotEnd);
#ifdef EMP_355
	int dots = IMG_H * Calc2D::INIT_SCALE;
#else
	int dots = IMG_H;
#endif
	if (dotBegin > dots)
		dotBegin = dots;
	if (dotEnd > dots)
		dotEnd = dots;
    if (dotBegin <= 0) // dotBegin send to fpga must larger than 0
        dotBegin = 1;

	///> send box size to imaging system
	m_fpga.SendScanBegin(lineBegin);
	m_fpga.SendScanEnd(lineEnd);
	m_fpga.SendSampleBegin(dotBegin);
	m_fpga.SendSampleEnd(dotEnd);
}

void CalcCfm::CalcGain(int gain)
{
	///> send color gain to imaging system
}

void CalcCfm::CalcTurbo(int level)
{
}

/*
 * @brief calc max period of cfm
 *
 * @para prf[in] cfm prf
 * @para clusterSize[in] cluster size of real-time cfm
 */
int CalcCfm::CalcMaxPeriod(int prf, int clusterSize)
{
    ProbeSocket::ProbePara para;
    ProbeMan::GetInstance()->GetCurProbe(para);
    const char* type = para.model;

    //此时本来应该是60，但是为了解决线阵左侧干扰故改成了20
#ifdef EMP_430
    int maxPeriod;
    if ((strcmp(type, "65L40E") == 0) || (strcmp(type, "65C10E") == 0) || (strcmp(type, "65C15D") == 0))
        maxPeriod = 1000000 / (prf * clusterSize) - 20;//FOC_LOW_TIME; //20; //us
    else
        maxPeriod = 1000000 / (prf * clusterSize) - FOC_LOW_TIME; //20; //us
#else
	int maxPeriod = 1000000 / (prf * clusterSize) - FOC_LOW_TIME; //20; //us
#endif

	m_fpga.SendMaxPeriod(maxPeriod);

	return prf;
}

void CalcCfm::CalcInvert(bool on)
{
}

void CalcCfm::CalcColorGrayCfm(int colorCode, int baseline, int turbo, bool invert, unsigned char colorMap[COLOR_LENGTH * COLOR_WIDTH][COLOR_DEPTH])
{
	unsigned char tempColorFpga[COLOR_DEPTH*COLOR_LENGTH]; //BGR 4*256
	int tempColor[COLOR_DEPTH][COLOR_LENGTH]; //BGR 4*256
	int i, j;

	// calc for send to fpga
	for (i = 0; i < COLOR_LENGTH; i ++)
	{
		for (j = 0; j < COLOR_DEPTH; j ++)
			tempColorFpga[i*COLOR_DEPTH + j] = i;
	}

	m_fpga.SendColorBands((INT16 *)tempColorFpga, sizeof(tempColorFpga));

	// calc for send to dsc(real color map)
	MakeCFMColormap(tempColor, baseline, colorCode, invert);
	for (j = 0; j < COLOR_LENGTH; j ++)
	{
		for (i = 0; i < COLOR_WIDTH; i ++)
		{
			colorMap[j*COLOR_WIDTH + i][0] = tempColor[2][j];
			colorMap[j*COLOR_WIDTH + i][1] = tempColor[1][j];
			colorMap[j*COLOR_WIDTH + i][2] = tempColor[0][j];
			colorMap[j*COLOR_WIDTH + i][3] = tempColor[3][j];

		}
	}
}

void CalcCfm::CalcColorGrayCfmVar(int colorCode, int baseline, int turbo, bool invert, unsigned char colorMap[COLOR_LENGTH * COLOR_WIDTH][COLOR_DEPTH])
{
	unsigned char tempColorFpga[COLOR_DEPTH*COLOR_LENGTH]; //BGR 4*256
	int tempColorTurbo[COLOR_DEPTH][COLOR_LENGTH][VARIANCE_GRADE]; //BGR 4*256*7
	int i, j;

	// calc for send to fpga
	for (i = 0; i < COLOR_LENGTH; i ++)
	{
		for (j = 0; j < COLOR_DEPTH; j ++)
			tempColorFpga[i*COLOR_DEPTH + j] = i;
	}

	m_fpga.SendColorBands((INT16 *)tempColorFpga, sizeof(tempColorFpga));

	// calc for send to dsc(real color map)
	MakeCFMVarColormap(tempColorTurbo, baseline, colorCode);
	double step = COLOR_LENGTH / VARIANCE_GRADE;
	int temp = 0;

	for (j = 0; j < COLOR_LENGTH; j ++)
	{
		for (i = 0; i < COLOR_WIDTH; i ++)
		{
			temp = i / step;
			colorMap[j*COLOR_WIDTH + i][0] = tempColorTurbo[2][j][temp];
			colorMap[j*COLOR_WIDTH + i][1] = tempColorTurbo[1][j][temp];
			colorMap[j*COLOR_WIDTH + i][2] = tempColorTurbo[0][j][temp];
			colorMap[j*COLOR_WIDTH + i][3] = tempColorTurbo[3][j][temp];
		}
	}
}
/*
 * @brief gererate color map of PDI
 *
 * @para baseline[in] not used now, bseline of direction PDI
 * @para turbo[in] not used now, index of turbo
 * @para direction[in] not unsed now, if or not direction PDI
 */
void CalcCfm::CalcColorGrayPdi(int colorCode, int baseline, int turbo, bool invert, unsigned char colorMap[COLOR_LENGTH * COLOR_WIDTH][COLOR_DEPTH])
{
	unsigned char tempColorFpga[COLOR_DEPTH*COLOR_LENGTH]; //BGR 4*256
	int tempColor[COLOR_DEPTH][COLOR_LENGTH]; //BGR 4*256
	int i, j;

	// calc for send to fpga
	for (i = 0; i < COLOR_LENGTH; i ++)
	{
		for (j = 0; j < COLOR_DEPTH; j ++)
			tempColorFpga[i*COLOR_DEPTH + j] = i;
	}

	m_fpga.SendColorBands((INT16 *)tempColorFpga, sizeof(tempColorFpga));

	// calc for send to dsc
	MakePDIColormap(tempColor, colorCode);
	for (j = 0; j < COLOR_LENGTH; j ++)
	{
		for (i = 0; i < COLOR_WIDTH; i ++)
		{
			colorMap[(COLOR_LENGTH - j - 1) * COLOR_WIDTH + i][0] = tempColor[2][j];
			colorMap[(COLOR_LENGTH - j - 1) * COLOR_WIDTH + i][1] = tempColor[1][j];
			colorMap[(COLOR_LENGTH - j - 1) * COLOR_WIDTH + i][2] = tempColor[0][j];
			colorMap[(COLOR_LENGTH - j - 1) * COLOR_WIDTH + i][3] = tempColor[3][j];

		}
	}
}

void CalcCfm::CalcPulseNum(int num)
{
}

void CalcCfm::CalcWallFilter(unsigned short *filter, int size)
{
	m_fpga.SendWallFilter(filter, size);
}

void CalcCfm::CalcWallFilter(int wallFilterIndex, int packSize)
{
	const int order = 2;
	const int size = order + 1;
	double a[13][size] = {
		{(double)0,  (double)0,  (double)0},
		{1.000000000000000,  -1.778631777824585,   0.800802646665708}, //wn=0.05
		{1.000000000000000,  -1.690996376887443,   0.732726030371816}, //wn=0.07
		{1.000000000000000,  -1.647459981076977,   0.700896781188403}, //wn=0.08
		{1.000000000000000,  -1.561018075800718,   0.641351538057563}, //wn=0.1
		{1.000000000000000,  -1.348967745252795,   0.513981894219676}, //wn=0.15
		{1.000000000000000,  -1.142980502539901,   0.412801598096189}, //wn=0.2
		{1.000000000000000,  -0.942809041582063,   0.333333333333333}, //wn=0.25
		{1.000000000000000,  -0.747789178258504,   0.272214937925007}, //wn=0.3
		{1.000000000000000,  -0.557030997311751,   0.226966197819818}, //wn=0.35
		{1.000000000000000,  -0.369527377351241,   0.195815712655833}, //wn=0.4
		{1.000000000000000,  -0.000000000000000,   0.171572875253810}, //wn=0.5
		{1.000000000000000,   0.369527377351241,   0.195815712655833} //wn=0.6
	};

	double b[13][size] = {
		{(double)0,  (double)0,  (double)0},
		{0.894858606122573,  -1.789717212245146,   0.894858606122573}, //wn=0.05
		{0.855930601814815,  -1.711861203629630,   0.855930601814815}, //wn=0.07
		{0.837089190566345,  -1.674178381132690,   0.837089190566345}, //wn=0.08
		{0.800592403464570,  -1.601184806929141,   0.800592403464570}, //wn=0.1
		{0.715737409868118,  -1.431474819736235,   0.715737409868118}, //wn=0.15
		{0.638945525159022,  -1.277891050318045,   0.638945525159022}, //wn=0.2
		{0.569035593728849,  -1.138071187457699,   0.569035593728849}, //wn=0.25
		{0.505001029045878,  -1.010002058091756,   0.505001029045878}, //wn=0.3
		{0.445999298782892,  -0.891998597565784,   0.445999298782892}, //wn=0.35
		{0.391335772501769,  -0.782671545003537,   0.391335772501769}, //wn=0.4
		{0.292893218813453,  -0.585786437626905,   0.292893218813453}, //wn=0.5
		{0.206572083826148,  -0.413144167652296,   0.206572083826148} //wn=0.6
	};

	if (wallFilterIndex > 13)
		wallFilterIndex = 13;
	Mat matFilter(packSize, packSize, CV_64FC1, Scalar::all(0));
	CalcProjectionInitialMatrix(matFilter, order, a[wallFilterIndex], b[wallFilterIndex], packSize);

	const int coefRows = 26;
	const int coefCols = 32;
	unsigned short pWallFilterCoef[coefRows * coefCols] = {0};
	const int ZOOM_SCALE = 4096;
	for (int i = 0; i < matFilter.rows; i++)
	{
		for (int j = 0; j < matFilter.cols; j++)
		{
			pWallFilterCoef[i*coefCols + j] = (unsigned short)round(matFilter.at<double>(i, j) * ZOOM_SCALE);
		}
	}

	/*printf("===============wall filter coef: coefRows=%d coefCols=%d\n", coefRows, coefCols);
	for (int r = 0; r < coefRows; r++)
	{
		for (int c = 0; c < coefCols; c++)
		{
			printf("%d ", (short)(pWallFilterCoef[r * coefCols + c]));
		}
			printf("\n");
	}*/
	m_fpga.SendWallFilter(pWallFilterCoef, coefRows * coefCols);
}

void CalcCfm::CalcProjectionInitialMatrix(Mat matFilter, int order, double* a, double* b, int packSize)
{
	/*printf("order=%d packSize=%d\n", order, packSize);
	printf("a[0]=%.15f, a[1]=%.15f, a[2]=%.15f\n", a[0], a[1], a[2]);
	printf("b[0]=%.15f, b[1]=%.15f, b[2]=%.15f\n", b[0], b[1], b[2]);*/

	int i, j, k = 0;
	double sum = 0.0;
	for (i = 0; i < order + 1; i++)
	{
		sum += abs(b[i]);
	}
	if (sum < 0.00000001)
	{
		matFilter = Mat::eye(packSize, packSize, CV_64FC1);
		return;
	}

	Mat matF(order, order, CV_64FC1, Scalar::all(0));
	for (i = 1; i < order; i++)
	{
		matF.at<double>(i - 1, i) = 1;
		matF.at<double>(order - 1, i - 1) = -a[order+1-i];
	}
	matF.at<double>(order - 1, order - 1) = -a[1];

	Mat matQ(order ,1, CV_64FC1, Scalar::all(0));
	matQ.at<double>(order - 1, 0) = 1;

	Mat matG(1, order, CV_64FC1, Scalar::all(0));
	for (i = 0; i < order; i++)
	{
		matG.at<double>(0, i) = b[order - i] - b[0] * a[order - i];
	}

	double d = b[0];

	Mat matB(packSize, order, CV_64FC1, Scalar::all(0));
	Mat matFPower = Mat::eye(order, order, CV_64FC1);

	for (i = 0; i < packSize; i++)
	{
		matB.row(i) = matG * matFPower;
		matFPower = matFPower * matF;
	}

	Mat matC = Mat::eye(packSize, packSize, CV_64FC1);
	matC = matC * d;
	matFPower = Mat::eye(order, order, CV_64FC1);
	Mat matTmp(1, 1, CV_64FC1, Scalar::all(0));
	for (i = 1; i < packSize; i++)
	{
		for (j = 0; j < i; j++)
		{
			for (k = 0; k < (i - 1 - j); k++)
			{
				matFPower = matFPower * matF;
			}
			matTmp = matG * matFPower * matQ;
			matC.at<double>(i, j) = matTmp.at<double>(0,0);
			matFPower = Mat::eye(order, order, CV_64FC1);
		}
	}

	Mat matPackSizeEye= Mat::eye(packSize, packSize, CV_64FC1);
	matFilter = (matPackSizeEye - matB * (matB.t() * matB).inv() * matB.t()) * matC;
}

void CalcCfm::CalcPacketSize(int pktSize)
{
	///> send packet size
	FpgaCtrl2D m_fpgaCtrl2D;
	m_fpgaCtrl2D.SendPacketSize((INT8U)pktSize);
}

void CalcCfm::CalcLog()
{
}
void CalcCfm::CalcBaseline()
{
}
void CalcCfm::CalcNoise()
{
}
void CalcCfm::CalcSmooth()
{
}

void CalcCfm::CalcGainGate(int gate)
{
	m_fpga.SendGainGate(gate);
}

// private
void CalcCfm::Demod()
{
}

void CalcCfm::SmatchFilter()
{
}

void CalcCfm::LowFilter()
{
}

double CalcCfm::CalcMaxVel(int prf, int colorFreq, double soundSpeed)
{
	int freq = (float)colorFreq / 20 * 1000000; //hz

	double maxVel = (soundSpeed * prf / 4 / freq) * 100; //cm/s

	return maxVel;
}

/*
 * @brief calc cluster size of real-time CFM
 *
 * @para[in] prf PRF of CFM
 * @para depth[in] current image depth, unit:mm
 * @para soundSpeed[in] sound speed ,unit:
 * @para tDepth[out] min depth to sample
 */
int CalcCfm::CalcClusterSize(int prf, int depth, double soundSpeed, int mbp, int &tDepth)
{
	int Tprf = 1000000 / prf; //us
	tDepth = depth * 2 * 1000 / soundSpeed + 5 +  FOC_LOW_TIME;//20; //us note: 15us is the comsume of fpga pipeline, 20 is comsume of emiting prepare time.

	int maxValue = 100;
	if (mbp <= 2)
#ifdef EMP_430
		maxValue = 100;
#else
		maxValue = 80;
#endif
	else
		maxValue = 100;

	if (tDepth < maxValue)
		tDepth = maxValue;

	int cluster = Tprf / tDepth;
	if (cluster == 0)
		cluster = 1;

	m_fpga.SendClusterSize(cluster);

	return cluster;
}

/*
 * @breif send cmd to fpga, witch pdi and dir pdi
 */
void CalcCfm::CalcSwitchPdi(bool dir)
{
	m_fpga.SendPdiDirMode(dir);
}

void CalcCfm::CalcLowFilter(float fccur)
{
	float fcCur = 1.0;
    fcCur = fccur;
    int freqInterval = 480/12;
    if(fcCur > 12.0)
        fcCur = 12.0;

    int segAddr = (12-fcCur)*freqInterval;
    int filterAddr = segAddr*32*2;
    const int size = FREQ_SECTION*32* sizeof(unsigned short);
    unsigned char *ptrFilter = new unsigned char[size];

    memcpy(ptrFilter, m_filterLowPass+filterAddr, 32*2);//12MHz ~ 0MHz
   // for(int i = 0; i < 32*2; i++)
   //     printf("%d, ", m_filterLowPass[filterAddr+i]);
    m_fpga.SendLowFilter((unsigned short*)ptrFilter, 32*2);

    delete []ptrFilter;
}

#if 0
void CalcCfm::DemodCalc_test(float freq[5], int freqDepth[5], bool enableHarmonic, int& cycleFilterChg, unsigned short sinStep[64])
{
	float speed = m_calcPara->soundSpeed;
	int depth = m_calcPara->depth;
    int i;
    int temp;
    float freqDemod;
    for (i = 0; i < FREQ_SECTION; i ++)
    {
        temp = i * depth / FREQ_SECTION;

        if (temp < freqDepth[0])
            freqDemod = freq[0];
        else if (temp < freqDepth[1])
            freqDemod = freq[0] - (temp-freqDepth[0])*(freq[0]-freq[1])/(freqDepth[1]-freqDepth[0]);
        else if	(temp <freqDepth[2])
            freqDemod = freq[1] - (temp-freqDepth[1])*(freq[1]-freq[2])/(freqDepth[2]-freqDepth[1]);
        else if	(temp <freqDepth[3])
            freqDemod = freq[2] - (temp-freqDepth[2])*(freq[2]-freq[3])/(freqDepth[3]-freqDepth[2]);
        else if	(temp <freqDepth[4])
            freqDemod = freq[3] - (temp-freqDepth[3])*(freq[3]-freq[4])/(freqDepth[4]-freqDepth[3]);
        else
            freqDemod = freq[4];

        sinStep[i] = 1024*4*freqDemod/SAMPLE_FREQ+0.5;
    }

	cycleFilterChg = depth*2*SAMPLE_FREQ/speed/64 + 0.5;
	if(cycleFilterChg > 511) cycleFilterChg = 511;
}
#endif