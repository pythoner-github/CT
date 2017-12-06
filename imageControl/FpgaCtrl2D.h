#ifndef __FPGACTRL2D_H__
#define __FPGACTRL2D_H__

#include "imageControl/DefHal.h"
//#include "AbsUltraInterface.h"

class AbsUltraInterface;

/**
* @brief send data to fpga to control image
*/
class FpgaCtrl2D
{
    public:

    FpgaCtrl2D(){}

    ~FpgaCtrl2D(){}

    /**
    * @brief send focus position FOCUS_A FOCUS_B FOCUS_C FOCUS_D
    */
    int SendFocusPosition(INT32U focusBegin[4], INT32U focusEnd[4]);

    /**
    * @brief send delay to fpga
    * @param delay[]: delay of emit focus
    * @param size: number of delay data
    * @param focus_sum: focus sum
    *
    */
    int SendEmitDelayEven(INT16U delay[], INT32U size, INT8U focusNumber, INT32U offset);
    int SendEmitDelayOdd(INT16U delay[], INT32U size, INT8U focusNumber, INT32U offset);
    int SendEmitDelayEvenSpaceCompound1(INT16U delay[], INT32U size, INT8U focusNumber, INT32U offset);
    int SendEmitDelayOddSpaceCompound1(INT16U delay[], INT32U size, INT8U focusNumber, INT32U offset);
    int SendEmitDelayEvenSpaceCompound2(INT16U delay[], INT32U size, INT8U focusNumber, INT32U offset);
    int SendEmitDelayOddSpaceCompound2(INT16U delay[], INT32U size, INT8U focusNumber, INT32U offset);

    /**
    * @brief send delay to fpga
    * @param delay[]: delay of emit focus in CFM mode
    * @param size: number of delay data
    * @param offset: size between two focus
    * @para pahseProbe: 1-phase probe flag, 0-not phase probe flag
    *
    */
    int SendEmitDelayEvenCFM(INT16U delay[], INT32U size, INT32U offset, INT8U phaseProbe);
    int SendEmitDelayOddCFM(INT16U delay[], INT32U size, INT32U offset, INT8U phaseProbe);

    /**
    * @brief send delay to fpga
    * @param delay[]: delay of emit focus in PW mode
    * @param size: number of delay data
    * @param offset: size between two focus
    * @para pahseProbe: 1-phase probe flag, 0-not phase probe flag
    *
    */
    int SendEmitDelayEvenPW(INT16U delay[], INT32U size, INT32U offset, INT8U phaseProbe);
    int SendEmitDelayOddPW(INT16U delay[], INT32U size, INT32U offset, INT8U phaseProbe);

    /**
    * @brief send tgc to fpga
    * @param tgc[]: data of tgc
    * @param size: number of tgc
    *
    */
    int SendTgc2D(INT8U tgc[], INT32U size);
    int SendTgcPw(INT8U tgc[], INT32U size); //also used in m mode
    int SendTgcCfm(INT8U tgc[], INT32U size);
    int SendTgcDigital2D(INT16U tgc[], INT32U size);
    int SendTgcDigitalCycle2D(INT16U cycle);
    int SendTgcDigitalPw(INT16U tgc[], INT32U size);
    int SendTgcDigitalCyclePw(INT16U cycle);
    int SendTgcDigitalCfm(INT16U tgc[], INT32U size);
    int SendTgcDigitalCycleCfm(INT16U cycle);

    int VideoStandard(INT16U para);
    int VGADisplay(INT16U para);
    /**
    * @brief send scanrange
    * @param scan_begin: begin of scan line  begin-8bits(8-15)
    * @param scan_end: end of scan line  end-8bits(0-7)
    *
    */
    int SendScanrange(INT32U scanBegin, INT32U scanEnd);

    /**
    * @param delay: pointer to delay of reveive focus
    * @param size: number of delay
    *
    */
    int SendReceiveDelayAn(INT16 *an, INT32U size);
    int SendReceiveDelayAnSpaceCompound1(INT16 *an, INT32U size);
    int SendReceiveDelayAnSpaceCompound2(INT16 *an, INT32U size);
    int SendReceiveDelayBn(INT16 *bn, INT32U size);
    int SendReceiveDelayBnSpaceCompound1(INT16 *bn, INT32U size);
    int SendReceiveDelayBnSpaceCompound2(INT16 *bn, INT32U size);

    /**
    * @param delay: pointer to delay of reveive focus of the fourth beam
    * @param size: number of delay
    *
    */
    int SendReceiveDelayColorAn(INT16 *an, INT32U size);
    int SendReceiveDelayColorBn(INT16 *bn, INT32U size);

    /**
    * @param aperture: pinter to aperture data of reveive focus
    * @param size: number of aperture
    *
    */
    int SendReceiveAperture(INT8U *aperture, INT32U size);
    int SendReceiveApertureU16(INT16 *aperture, INT32U size);
    int SendReceiveApertureSpaceCompound1(INT8U *aperture, INT32U size);
    int SendReceiveApertureSpaceCompound2(INT8U *aperture, INT32U size);
    int SendReceiveApertureColor(INT8U *aperture, INT32U size);
    int SendReceiveApertureSpaceCompound1U16(INT16 *aperture, INT32U size);
    int SendReceiveApertureSpaceCompound2U16(INT16 *aperture, INT32U size);
    int SendReceiveApertureColorU16(INT16 *aperture, INT32U size);

    /**
    * @param clearAddr (0--15), clear address of combined demod.
    * @param data (0--15), data of combined demod.
    *
    */
    int SendDemodCombine(INT16U data);
    int SendDemodCombineBuf(INT16U* data, INT32U size);
    int SendDemodBufFreqCompound(INT16U* data, INT32U size, INT32U order);

    /**
    * @param speed  8bits(8--15)
    *
    */
    int SendBmSpeed(INT32U speed);

    /**
    * @brief 60m 460点
    * @param sample--16bits(0-15)
    */
    int SendSample48m(INT32U sample);

    /**
    * @brief 60m 64dots
    * @param sample  16bits(0-15)
    */
    int SendSampleApertureBW(INT32U sample);

    /**
    * @brief 60m 64dots color
    * @param sample  16bits(0-15)
    */
    int SendSampleApertureColor(INT32U sample);

    /*
     * @brief pulse cycle, pulse width when output power if 100%
     */
    int SendPulseCycle(INT32U cycle);
    int SendPulseCycleFreqCompound(INT32U cycle, INT32U order);
    int PulseWidthNew(INT32U width, INT32U clear);
    int SendPulseWidthFreqCompound(INT32U pulseWidth, INT32U order);
    /*
     * @brief color pulse cycle, pulse width when output power if 100%
     */
    int SendColorPulseCycle(INT32U cycle);

    /**
    * @param black-white emit weighting value 8bits(0-7)
    *
    */
    int SendPulseWidth(INT8U *data, INT32U size);
    int SendPulseWidthFreqCompound(INT8U *data, INT32U size, INT32U order);

    /**
    * @param color emit weighting value 8bits(0-7)
    *
    */
    int SendColorPulseWidth(INT8U *data, INT32U size);

    /**
    * @param sum  8bits(8-15)--real focus sum(1-4)
    *
    */
    int SendFocusSum(INT8U sum);

    /**
    * @param max_period  16bits(0-15) focus pulse 协议上为2bits
    */
    int SendMaxPeriod(INT32U maxPeriod);
    int SendTgcMaxPeriod(INT32U maxPeriod);

    /**
    * @param level  8bits(0-7) level of dynamic range
    *
    */
    int SendDynamicRange(INT8U level);

    /**
    * @param line  8bits(0-7) bm line
    *
    */
    int SendBmLine(INT32U line);

    /**
    * @param filter[]: filter data 16bits
    * @param size: number of data for one probe one scale
    *
    */
    int SendFilter(INT16U *filter, INT32U size);
    int SendFilterCompoundFreq1(INT16U *filter, INT32U size);
    int SendFilterCompoundFreq2(INT16U *filter, INT32U size);
    int SendFilterColor(INT16U *filter, INT32U size);

    int SendPowerOff(void);

    /**
    * @brief tell fpga to begin emit
    * 8bits(0~7)
    *
    */
    int SendBeginEmit(void);

    /**
    * @brief send work model to fpga   ADDFPGA: 16
    * @param wk_model 0000:reserve, 0001:2D, 0010:CFM, 0011:CDE, 0100:CFM+PW, 0101:B+PW
    */
    int SendWorkModel(INT32U wkModel);

    /**
    * @brief send packet size
    * @param size 5bits (4-8)
    */
    int SendPacketSize(INT8U size);

    /**
    * @brief send log table data
    */
    int SendLogAddrData(INT32U size, INT16U *logData);

    /**
    * @brief  PH control label
    * @param phLabel 1bits 1:相控阵  0：非相控阵
    */
    int SendPhaseControl(INT32U phLabel);

    /**
    * @brief  pulse num
    * @param num 3bits (3-5)
    */
    int Send2DPulseNum(INT32U num);

    /*
     * 5bits(11-15)
     */
    int SendPWPulseNum(INT32U num);

    /*
     * 5bits(6-10)
     */
    int SendCFMPulseNum(INT32U num);

    /**
    * @brief send line density status
    * @param line_density 0: low line desity 1: high  8bits(0-7)
    */
    int SendLineDensity(INT32U lineDensity);

    /**
    * @brief send reverse THI status
    * @param on 0: off 1: on 8bits(0-7)
    */
    int SendReverseTHI(INT32U on);
    int SendSpaceCompound(INT32U on);
    int SendFreqCompund(INT32U on);

    /**
    * @brief send high filter datas
    * 16 16bits
    */
    int SendMatchFilter(INT16U *filter,INT32U size);
    int SendMatchFilterFreqCompound1(INT16U *filter,INT32U size);
    int SendMatchFilterFreqCompound2(INT16U *filter,INT32U size);
    int SendMatchFilterPw(INT16U *filter,INT32U size);

    int SendValid2DImage(int start2D, int startColor);

    /*
     * @brief control parameter for receive of BW
     */
    int SendPhaseFocStart(INT16 time);
    int SendPhaseWrOffset(INT16 offset);
    int SendPhaseStartK(INT16 start);

    /*
     * @brief control parameter for receive of color
     */
    int SendPhaseFocStartColor(INT16 time);
    int SendPhaseWrOffsetColor(INT16 offset);
    int SendPhaseStartKColor(INT16 start);

    /*
     * @brief control parameter for receive of compound
     */
    int SendSpaceCompound1FocStart(INT16 time);
    int SendSpaceCompound1WrOffset(INT16 offset);
    int SendSpaceCompound1StartK(INT16 start);
    int SendSpaceCompound2FocStart(INT16 time);
    int SendSpaceCompound2WrOffset(INT16 offset);
    int SendSpaceCompound2StartK(INT16 start);

    //only for test
    int SendPulseWidthBak(INT32U width);
    int SendColorPulseWidthBak(INT32U width);

    /*
     * @brief paramter of compound imaging
     */
    int SendSpaceCompoundAdderLine(INT16 line);
    int SendSpaceCompoundAdderDot(INT16 dot);
    int SendSpaceCompoundStartSamp(INT16 start);
    int SendSpaceCompound1AdderLine(INT16 line);
    int SendSpaceCompound1AdderDot(INT16 dot);
    int SendSpaceCompound1StartSamp(INT16 start);
    int SendSpaceCompound2AdderLine(INT16 line);
    int SendSpaceCompound2AdderDot(INT16 dot);
    int SendSpaceCompound2StartSamp(INT16 start);

    int SendSpaceCompoundAdder(INT16U *value, INT32U size);

    int SendSpaceCompoundCoef(INT16 value);
    int SendSpaceCompound1Coef(INT16 value);
    int SendSpaceCompound2Coef(INT16 value);
    int SendSpaceCompoundBase(INT16 value);

    int SendFreqCompoundCoef(INT16U* value, INT32U size);
        int SendVPSamplesCnt(INT32U cnt);
        int SendVPAngleSpeed(INT32U cnt);
    int SendCWReceive(INT8U* value, INT32U size);

    int SendExtendImaging(INT32U on);
    int SendExtendImagingSample(INT16U* value, INT32U size);

#ifdef EMP_PROJECT
    int SendWaveData(INT8U *emitWave, INT32U pulseWidth, INT32U pulseNum, INT16U size);
    int Send2DPulseNumFreq0(INT32U pulseNum);
    int Send2DPulseNumFreq1(INT32U pulseNum);
    int Send2DPulseNumFreq2(INT32U pulseNum);
    int SendCfmPulseNum(INT32U pulseNum);
    int SendPwPulseNum(INT32U pulseNum);
    int Send2DPulseWidthFreq0(INT32U pulseWidth);
    int Send2DPulseWidthFreq1(INT32U pulseWidth);
    int Send2DPulseWidthFreq2(INT32U pulseWidth);
    int SendCfmPulseWidth(INT32U pulseWidth);
    int SendPwPulseWidth(INT32U pulseWidth);
    int SendCwPulseWidth(INT32U pulseWidth);
    int SendEmissionPulseDataFreq0(INT8U *data, INT32U size);
    int SendEmissionPulseDataFreq1(INT8U *data, INT32U size);
    int SendEmissionPulseDataFreq2(INT8U *data, INT32U size);
    /**
     * 对数转换系数coef (0-15bit)
     */
    int SendLogCoef(INT32U coef);
    /**
     * 动态范围系数coef (0-15bit)
     */
    int SendDynamicRangeCoef(INT32U coef);

    /**
     * 数字增益序列
     */
    int SendTgc(INT16U *tgc, INT32U size);
    /**
     * 频率复合1数字增益序列
     */
    int SendFreqCompoundTgc1(INT16U *tgc, INT32U size);
    /**
     * 频率复合2数字增益序列
     */
    int SendFreqCompoundTgc2(INT16U *tgc, INT32U size);
#endif

    private:
    enum
    {
        FOC_A=0,
        FOC_B=1,
        FOC_C=2,
        FOC_D=3,
        SAMPLE_48=4,//
        //SAMPLE=0x10,
        FPGA_ADDR=5,
        FPGA_DATA=6,
        TGC_DATA_T=7,
        IMAGE_CTL=8,//
        PROBE_EMIT1=9,//
        FOCUS_ADDR=10,  //
        //FOCUS_DATA_1 = 11,  //
        FOCUS_An = 11,  //
        APERTURE_DATA = 12,//
        //FOCUS_DATA_2 = 13,//
        //FOCUS_DATA_3 = 14,//
        //FOCUS_DATA_4 = 15,//
        FOCUS_Bn = 13,//
        PHASE_ADDR = 14,
        PHASE_DATA = 15,

        WORK_MODEL_T=16,   //
        LOG_ADDR_T=17,
        LOG_DATA_T=18,
        FILTER_ADDR=19, //
        FILTER_DATA=20, //
      CW_RECEIVE_ADDR=28,
      CW_RECEIVE_DATA=29,
      MATCH_FILTER_DATA=24,
      MATCH_FILTER_ADDR=33,

        MAXPERIOD_ADDR=37,
        PHASE_PULSE_T=38,    //
        PROBE_EMIT2=39,     //
        WEIGHTING_EMIT=43,
        DEMOD_COMBINED_DATA=50,//
        DEMOD_COMBINED_ADDR=51,
        SCAN_RANGE=52,
        BM_LINE=53,//
        EDGE_ENHANCE=54, //
        RUN_EMIT=55,      //
        COMPOUND_ADDR=56,
        FUNCTION_SWITCH=57,   //
        HFILTER_ADDR_T=58,
        HFILTER_DATA_T=59,
        COMPOUND_DATA=60,
        VPSAMPLECNT = 61,
        VPPARA = 62
    };

    struct FpgaAddrVariable2D
    {
        INT32U m_focus1;
        INT32U m_focus2;
        INT32U m_focus3;
        INT32U m_focus4;
        INT32U m_sample;
        INT32U m_fpgaAddr;
        INT32U m_fpgaData;
        INT32U m_scanRange;
        INT32U m_focusAddr;
        INT32U m_focusData;
        INT32U m_apertureData;
        INT32U m_sample48;
        INT32U m_demode;
        INT32U m_imageX;
        INT32U m_probeEmit1;
        INT32U m_probeEmit2;
        INT32U m_imageCtl; //dynamic range
        INT32U m_bmLine;
        INT32U m_filterAddr;
        INT32U m_filterData;
        INT32U m_edgeEnhance;
        INT32U m_runEmit;
        INT32U m_compound;
        INT32U m_clkNum;

        INT32U m_wkModel; //EMP-2900
        INT32U m_num;
        INT32U m_pulseSum;
        INT32U m_tissueHarmonic;
        INT32U m_functionSwitch;
        INT32U m_highPassFilter;
        INT32U m_highPass;
        INT32U m_logAddrData;
        INT32U m_vpsamplescnt;
        INT32U m_vp_para;

    };

    static struct FpgaAddrVariable2D fpga;
    static AbsUltraInterface* m_ptrInterface;

    int SendSampleCombined(INT16U clearAddr, INT16U sampleData);
    int TgcCombine(INT32 clearAddr, INT8U tgc[], INT32U size);
    int PhaseCombine(INT16U clearAddr, INT16U para);
    int EmitDelay(INT16U delay[], INT32U size, INT32U clear);
    int PulseCycle(INT32U cycle, INT32U clear);
    int PulseWidth(INT8U *value, INT32U size, INT32U clear);
    int ReceiveDelayAn(INT16 *bn, INT32U size, INT32U clear);
    int ReceiveDelayBn(INT16 *bn, INT32U size, INT32U clear);
    int CompoundImageCombine(INT16U clear, INT16U para);
    int CompoundImageCombineBuf(INT16U clear, INT16U* value, INT32U size);
    int FunctonSwitch(INT32U value, INT32U clear);
    int MatchFilter(INT16U *filter, INT32U size, INT32U clear);
    int DynamicFilter(INT16U *filter, INT32U size, INT32U offset);
    int ReceiveAperture(INT8U *aperture, INT32U size, INT32U clear);
    int ReceiveApertureU16(INT16 *aperture, INT32U size, INT32U clear);
    int DemodCombineBuf(INT16U* data, INT32U size, INT32U clear);
    int CwCombine(INT16U clear, INT16U para);

#ifdef EMP_PROJECT
    ///emission wave paraments
    int EmissionPulseNum(INT32U clear, INT32U pulseNum);
    int EmissionPulseWidth(INT32U clear, INT32U pulseWidth);
    int EmissionPulseData(INT32U clear, INT8U *waveData, INT32U size);
    int TgcSequence(INT32U clear, INT16U *tgc, INT32U size);
#endif
};

#endif