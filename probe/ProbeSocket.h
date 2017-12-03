#ifndef PROBE_SOCKET_H
#define PROBE_SOCKET_H

#include <string>
#include <vector>
#include "periDevice/IoCtrl.h"
//#include "UIS4DReceiveInterface.h"

using std::string;
using std::vector;

///> total number of probe
#if defined(EMP_360)
//static const int NUM_PROBE = 6;
static const int NUM_PROBE = 7;
#elif defined(EMP_161)
static const int NUM_PROBE = 5; //4; ///< total number of probe
#elif (defined(EMP_322) || defined(EMP_313))
static const int NUM_PROBE = 8; //7; ///< total number of probe
#elif defined(EMP_430)
static const int NUM_PROBE = 5; ///< total number of probe
#elif defined(EMP_440)
static const int NUM_PROBE = 4; ///< total number of probe
#elif defined(EMP_355)
static const int NUM_PROBE = 11; //11;//11; //11; ///< total number of probe
#else
static const int NUM_PROBE = 10; ///< total number of probe
#endif
extern string PROBE_LIST[NUM_PROBE]; // probe's name list
#if defined(EMP_430)
enum PROBE_TYPE{CONVEX_35R60, LINEAR_65W40, TV_65R10, TINYCONVEX_65R15, VOLUME_35R50}; ///< order must identical to PROBE_LIST
#else
enum PROBE_TYPE{CONVEX_35R50, LINEAR_75W40, TV_65R10, TINYCONVEX_35R20, TINYCONVEX_65R15, PHASE_30P16A, LINEAR_90W25, LINEAR_10W25, VOLUME_35R40}; ///< order must identical to PROBE_LIST
#endif

class ProbeSocket
{
	public:
		///> new type
		static const int MAX_DOP_FREQ = 2;
		static const int MAX_HARMONIC_FREQ = 3;//4;
        static const int MAX_FC = 5;
		//static const int MAX_DOP_FREQ = 1;

		struct FreqPair
		{
			int emit; // real freq * 20
			int receive; //real freq * 20
		};

		struct ProbePara
		{
			bool exist; ///< is probe exist? TRUE=exist; FALSE=exist;
			char model[11]; ///< probe model, for example "35C50K"
			char type; ///< probe type, for example'C'
			int array; ///< probe array, for example "128"
			int lines; ///< scan lines, "256"
			int width; ///< (width of probe linear, or angle of probe convex/TV) * 100
			int r; ///< radius of probe convex/TV(not used in probe linear) //mm
            int widthPhase; ///< width of phase probe, only used for phase probe //mm
			int depth; ///< max depth of probe. //mm
			int freqRange[2];
			vector<FreqPair> vecFreqRange; ///< range of frequency*20
			int freqDop[MAX_DOP_FREQ]; ///< range of frequency*20
			int volumeR; ///< scan radius of volume probe, unit:realR * 100
			int volumeAngle; ///< scan angle of volume probe, unit:realAngle * 100
			int thiFreq[MAX_HARMONIC_FREQ]; ///< range of harmonic frequency*20
            char factoryFlag[4];//vender,for example "EMP"
		};

		static const int MAX_2D_FREQ = 10;
		static const int FREQ2D_SUM[NUM_PROBE];
		static FreqPair FREQ2D[NUM_PROBE][MAX_2D_FREQ];
        static const int BAND_PASS_FILTER_FC_BASE_FREQ[NUM_PROBE][MAX_2D_FREQ][MAX_FC];
        static const int BAND_PASS_FILTER_FC1_BASE_FREQ[NUM_PROBE][MAX_2D_FREQ][MAX_FC];
#ifdef EMP_355
		static const int BAND_PASS_FILTER_FC_DOPPLER[NUM_PROBE][MAX_DOP_FREQ];
#endif
        static const int BAND_PASS_FILTER_FC2_BASE_FREQ[NUM_PROBE][MAX_2D_FREQ][MAX_FC];
        static const int DYNAMIC_FILTER_FC_BASE_FREQ[NUM_PROBE][MAX_2D_FREQ][MAX_FC];

        static const int FREQ_DOPPLER_SUM[NUM_PROBE];
		static const int FREQ_DOPPLER[NUM_PROBE][MAX_DOP_FREQ];
        static const int FREQ_THI_SUM[NUM_PROBE];
        static const int FREQ_THI[NUM_PROBE][MAX_HARMONIC_FREQ];
        static const int BAND_PASS_FILTER_FC[NUM_PROBE][MAX_HARMONIC_FREQ][MAX_FC];
        static const int BAND_PASS_FILTER_FC1[NUM_PROBE][MAX_HARMONIC_FREQ][MAX_FC];
        static const int BAND_PASS_FILTER_FC2[NUM_PROBE][MAX_HARMONIC_FREQ][MAX_FC];
        static const int DYNAMIC_FILTER_FC[NUM_PROBE][MAX_HARMONIC_FREQ][MAX_FC];

		///>constructure
		ProbeSocket(int socketAddr, int cmdRead)
			:m_socketAddr(socketAddr), m_cmdRead(cmdRead)
		{
		}

		~ProbeSocket()
		{
		}

		///> func
		void ReadProbePara();
		void ActiveSocket();
		void ActiveHV(bool on);
		void GetProbePara(ProbePara &para);
		void SetProbePara(ProbePara para);
		void WriteProbePara(unsigned char* buf, unsigned int len);
        string GetProbeFactoryFlag(char* flag, char* model);

	private:
		static const int PARA_LEN = 64;
		//static const int PARA_LEN = 30;

		int m_socketAddr;
		int m_cmdRead;
		ProbePara m_para;
       // SUIS4DPARA m_para4d;
		static IoCtrl m_io;

        int GetProbeIndex(char* model, unsigned char probeType);
        void GetFreq2D(char *model, unsigned char probeType, vector<FreqPair> &freqPair);
        void GetHarmonicFreq(char* model, unsigned char probeType);
};

#endif