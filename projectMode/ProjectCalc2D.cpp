#include "projectMode/ProjectCalc2D.h"
#include "projectMode/KnobProjectMode.h"
#include "imageControl/Img2D.h"
#include "display/TopArea.h"
#include "probe/ExamItem.h"
#include "imageProc/ScanMode.h"
#include "probe/ProbeSocket.h"
#ifdef CT_PROJECT
ProjectCalc2D* ProjectCalc2D::m_ptrInstance = NULL;
FpgaCtrl2D ProjectCalc2D::m_fpga;

const float ProjectCalc2D::EPSINON = 0.00001;
ProjectCalc2D* ProjectCalc2D::GetInstance()
{
    if (m_ptrInstance == NULL)
    m_ptrInstance  = new ProjectCalc2D;
    return m_ptrInstance;
}

ProjectCalc2D::ProjectCalc2D(void)
{
    DscMan* ptrDscMan = DscMan::GetInstance();
    m_ptrDscPara = ptrDscMan->GetDscPara();

    m_probeIndex = 0;
    m_harmonic = false;
    m_emitIndex = 1;
    m_freqIndex = 0;
    m_ptrDsc = NULL;
    m_ptrCalc = NULL;
}

ProjectCalc2D::~ProjectCalc2D()
{
    if (m_ptrInstance != NULL)
    {
        delete m_ptrInstance;
        m_ptrInstance = NULL;
    }
}

void ProjectCalc2D::SetProjectCalc2D(Calc2D* calc)
{
    m_ptrCalc = calc;

    // init 2D calculative para in project mode
    m_projectCalcPara.onDemodParaPrintf = false;
    m_projectCalcPara.filterSecIndex = 0;
    for (int i = 0; i < 5; i++)
    {
        m_projectCalcPara.bandPassW[i] = 5.0;
    }

    m_projectCalcPara.txFNum = 4.0;
    m_projectCalcPara.wpScale= 1.0;
   m_projectCalcPara.d2Pluse = 1;
    m_projectCalcPara.pwPluse = 1;
    m_projectCalcPara.cfmPluse = 1;
  //ProbeSocket::ProbePara p
    //ProbeMan::GetInstance()->GetCurProbe(para);

    m_projectCalcPara.probeAngle=1000;// para.width;
    m_projectCalcPara.probeR= 1000;//para.r;

#if 0
    m_projectCalcPara.windowFuncBPIndex = 0;
    m_projectCalcPara.windowFuncLPIndex = 0;
#endif
    m_ptrCalc->SetProjectCalcPara(&m_projectCalcPara);
}

void ProjectCalc2D::RefreshProjectCalc(const char* model, int freqIndex, bool harmonic)
{
    for (int i = 0; i < NUM_PROBE; ++i)
    {
        if (strcmp(model, PROBE_LIST[i].c_str()) == 0)
        {
            m_probeIndex = i;
            break;
        }
    }
    m_freqIndex = freqIndex;
    m_harmonic = harmonic;
    if(harmonic)
    {
        for(int i = 0; i < 5; i++)
        {
            m_projectCalcPara.bandPassW[i] = float(ProbeSocket::BAND_PASS_FILTER_FC[m_probeIndex][freqIndex][i] );
        }
    }
    else
    {
        for(int i = 0; i < 5; i++)
        {
            m_projectCalcPara.bandPassW[i] = float(ProbeSocket::BAND_PASS_FILTER_FC_BASE_FREQ[m_probeIndex][freqIndex][i] );
        }
    }
    m_projectCalcPara.txFNum = 4.0;
    m_projectCalcPara.wpScale = 1.0;
    m_projectCalcPara.d2Pluse = 1;
    m_projectCalcPara.pwPluse = 1;
    m_projectCalcPara.cfmPluse= 1;
    ProbeSocket::ProbePara para;
    ProbeMan::GetInstance()->GetCurProbe(para);
    if((strcmp(para.model, "30P16N") ==0)||(strcmp(para.model, "50P10N")==0))
        m_projectCalcPara.probeAngle= para.widthPhase;
    else
        m_projectCalcPara.probeAngle= para.width;
    m_projectCalcPara.probeR= para.r;

}

void ProjectCalc2D::InitKnobPara(void)
{
    ///> demod section
    m_projectCalcPara.filterSecIndex = 1;
    int section = m_projectCalcPara.filterSecIndex;
    EKnobReturn ret = OK;
    if (section == 0)
        ret = MIN;
    else if (section == (MAX_DEMOD_SECTION - 1))
        ret = MAX;
    else
        ret = OK;
    UpdateDemodSection(section, ret);

    ///> band pass filter data which section is eqaul zero
    float bpW0 = m_projectCalcPara.bandPassW[0];
    if (bpW0 >= -EPSINON && bpW0 <= EPSINON)
        ret = MIN;
    else if (bpW0 == (MAX_BAND_PASS_W1 - 1.0))
        ret = MAX;
    else
        ret = OK;
    UpdateBandPassW(bpW0, ret);

    ///> BPW2 which section is eqaul zero
    float tx = m_projectCalcPara.txFNum;;
    if (tx >= -EPSINON && tx <= EPSINON)
        ret = MIN;
    else if (tx == (MAX_TX_F_NUM - 1.0))
        ret = MAX;
    else
        ret = OK;
    UpdateTxFNum(tx, ret);

    ///> low pass data which section is equal zero
    float wp = m_projectCalcPara.wpScale;
    if (wp >= -EPSINON && wp <= EPSINON)
        ret = MIN;
    else if (wp == (MAX_WP_SCALE - 1.0))
        ret = MAX;
    else
        ret = OK;
    UpdateWpScale(wp, ret);

    float d2 = m_projectCalcPara.d2Pluse;
    if (d2 >= -EPSINON && d2 <= EPSINON)
        ret = MIN;
    else if (d2 == (MAX_2D_PLUSE - 1.0))
        ret = MAX;
    else
        ret = OK;
    Update2DPulse(d2, ret);

     float pw = m_projectCalcPara.pwPluse;
    if (pw >= -EPSINON && pw <= EPSINON)
        ret = MIN;
    else if (pw == (MAX_PW_PLUSE - 1.0))
        ret = MAX;
    else
        ret = OK;
    UpdatePwPulse(pw, ret);

     float cfm = m_projectCalcPara.cfmPluse;
    if (cfm >= -EPSINON && cfm <= EPSINON)
        ret = MIN;
    else if (cfm == (MAX_CFM_PLUSE - 1.0))
        ret = MAX;
    else
        ret = OK;
    UpdateCfmPulse(cfm, ret);

    float r = m_projectCalcPara.probeR;
    if (r >= -EPSINON && r <= EPSINON)
        ret = MIN;
    else if (r == (MAX_PROBE_R - 1.0))
        ret = MAX;
    else
        ret = OK;
    UpdateProbeR(r, ret);

 float angle = m_projectCalcPara.probeAngle;
    if (angle >= -EPSINON && angle <= EPSINON)
        ret = MIN;
    else if (angle == (MAX_PROBE_ANGLE - 1.0))
        ret = MAX;
    else
        ret = OK;
    UpdateProbeAngle(angle, ret);

    m_fpga.Send2DPulseNum(d2);
    m_fpga.Send2DPulseNum(cfm);
    m_fpga.Send2DPulseNum(pw);
    m_ptrCalc->CalcFilter();
    m_ptrCalc->CalcEmitDelay();
}

void ProjectCalc2D::RefreshKnobPara(void)
{
    ///> demod section
    int section = m_projectCalcPara.filterSecIndex;
    EKnobReturn ret = OK;
    if (section == 0)
        ret = MIN;
    else if (section == (MAX_DEMOD_SECTION - 1))
        ret = MAX;
    else
        ret = OK;
    UpdateDemodSection(section, ret);
    float fc;
    if(section <= 0)
    {
        if(m_harmonic)
        {
            fc  = float(ProbeSocket::BAND_PASS_FILTER_FC[m_probeIndex][m_freqIndex][0] );
        }
        else
        {
            fc = float(ProbeSocket::BAND_PASS_FILTER_FC_BASE_FREQ[m_probeIndex][m_freqIndex][0] );
        }

    }
    else
    {
        if(m_harmonic)
        {
            fc  = float(ProbeSocket::BAND_PASS_FILTER_FC[m_probeIndex][m_freqIndex][m_projectCalcPara.filterSecIndex-1] );
        }
        else
        {
            fc = float(ProbeSocket::BAND_PASS_FILTER_FC_BASE_FREQ[m_probeIndex][m_freqIndex][m_projectCalcPara.filterSecIndex-1] );
        }

         }
    ///> band pass filter data which section is eqaul zero
    if (fc>= -EPSINON && fc <= EPSINON)
        ret = MIN;
    else if (fc == (MAX_BAND_PASS_W1 - 1.0))
        ret = MAX;
    else
        ret = OK;
    UpdateBandPassW(fc, ret);

    m_ptrCalc->CalcFilter();
}

#if 1
EKnobReturn ProjectCalc2D::ChangeDemodSection(EKnobOper oper)
{
    ///> test filter
    int index = m_projectCalcPara.filterSecIndex;
    if (oper == ROTATE)
    {
        if(index < (MAX_DEMOD_SECTION - 1))
        {
            index += 1;
        }
        else
        {
            index = 0;
        }
    }

    else if (oper == ADD)
    {
        if (index < (MAX_DEMOD_SECTION - 1))
        {
            index += 1;
        }
        else
        {
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (index >= 1)
        {
            index -= 1;
        }
        else
        {
            return (MIN);
        }
    }
    else
    {
        return ERROR;
    }

    m_projectCalcPara.filterSecIndex = index;
    m_ptrCalc->CalcFilter();

    EKnobReturn ret = OK;
    if (index == 0)
        ret = MIN;
    else if (index == (MAX_DEMOD_SECTION - 1))
        ret = MAX;
    else
        ret = OK;

    ///> draw
    char value[64];
    sprintf(value, "%d", index);
//  if(KnobProjectMode::GetInstance()->GetPrint(0))
        //dayin

//  KnobProjectMode::GetInstance()->SetValue(0, value, ret);
//  ShowCurPageParaValue(0);

    RefreshKnobPara();

    return (ret);
}

EKnobReturn ProjectCalc2D::ChangeDemodParaPrintf()
{
    m_projectCalcPara.onDemodParaPrintf = !m_projectCalcPara.onDemodParaPrintf;
    char strHint[256];
    if (m_projectCalcPara.onDemodParaPrintf)
        sprintf(strHint, "Demodulation Para Printf is on !");
    else
        sprintf(strHint, "Demodulation Para Printf is off !");

    HintArea::GetInstance()->UpdateHint(_(strHint), 0);
    m_ptrCalc->CalcFilter();

#if 0
    bool ret = KnobProjectMode::GetInstance()->GetPrint(10);
    ret = !ret;
    KnobProjectMode::GetInstance()->SetPrint(10, ret);
#endif

    return PRESS;
}

EKnobReturn ProjectCalc2D::ChangeBandPassFc(EKnobOper oper)
{
    ///> test
    float index;
    if(m_projectCalcPara.filterSecIndex > 0)
        index = m_projectCalcPara.bandPassW[m_projectCalcPara.filterSecIndex-1];

    if (oper == ROTATE)
    {
        if(index <(MAX_BAND_PASS_W1 - 1))
        {
            index += 1;
        }
        else
        {
            index = 0;
        }
    }

    else if (oper == ADD)
    {
        if (index < (MAX_BAND_PASS_W1 - 1))
        {
            index += 1;
        }
        else
        {
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (index > 0)
        {
            index -= 1;
        }
        else
        {
            return (MIN);
        }
    }
    else
    {
        return ERROR;
    }

    if(m_projectCalcPara.filterSecIndex > 0)
        m_projectCalcPara.bandPassW[m_projectCalcPara.filterSecIndex-1] = index;
    m_ptrCalc->CalcFilter();

    EKnobReturn ret = OK;
    if (index == 0)
        ret = MIN;
    else if (index == (MAX_BAND_PASS_W1 - 1))
        ret = MAX;
    else
        ret = OK;

    ///> draw
    char value[64];
    sprintf(value, "%.2f", index);
    KnobProjectMode::GetInstance()->SetValue(1, value, ret);
    ShowCurPageParaValue(0);
    return (ret);
}

EKnobReturn ProjectCalc2D::ChangeTxFNum(EKnobOper oper)
{
    ///> test
    float index;
       index = m_projectCalcPara.txFNum;

    if (oper == ROTATE)
    {
        if(index <(MAX_TX_F_NUM - 1))
        {
            index += 0.1;
        }
        else
        {
            index = 0;
        }
    }

    else if (oper == ADD)
    {
        if (index < (MAX_TX_F_NUM - 1))
        {
            index += 0.1;
        }
        else
        {
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (index > 0)
        {
            index -= 0.1;
        }
        else
        {
            return (MIN);
        }
    }
    else
    {
        return ERROR;
    }

    m_projectCalcPara.txFNum = index;

    m_ptrCalc->CalcEmitDelay();
    EKnobReturn ret = OK;
    if (index == 0)
        ret = MIN;
    else if (index == (MAX_TX_F_NUM - 1))
        ret = MAX;
    else
        ret = OK;

    ///> draw
    char value[64];
    sprintf(value, "%.2f", index);
//  if(KnobProjectMode::GetInstance()->GetPrint(0))
    {
        //dayin
    }
    KnobProjectMode::GetInstance()->SetValue(2, value, ret);
//  ShowCurPageParaValue(0);
    return (ret);
}

EKnobReturn ProjectCalc2D::ChangeWpScale(EKnobOper oper)
{
    ///> test
    float index;
    index = m_projectCalcPara.wpScale;

    if (oper == ROTATE)
    {
        if(index <(MAX_WP_SCALE - 1))
        {
            index += 0.05;
        }
        else
        {
            index = 0;
        }
    }

    else if (oper == ADD)
    {
        if (index < (MAX_WP_SCALE - 1))
        {
            index += 0.05;
        }
        else
        {
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (index > 0)
        {
            index -= 0.05;
        }
        else
        {
            return (MIN);
        }
    }
    else
    {
        return ERROR;
    }

    m_projectCalcPara.wpScale = index;
    m_ptrCalc->CalcFilter();

    EKnobReturn ret = OK;
    if (index == 0)
        ret = MIN;
    else if (index == (MAX_WP_SCALE - 1))
        ret = MAX;
    else
        ret = OK;

    ///> draw
    char value[64];
    sprintf(value, "%.2f", index);
    KnobProjectMode::GetInstance()->SetValue(3, value, ret);
//  ShowCurPageParaValue(0);
    return (ret);
}

EKnobReturn ProjectCalc2D::Change2DPulse(EKnobOper oper)
{
    int index;
    index = m_projectCalcPara.d2Pluse;

    if (oper == ADD)
    {
        if (index < (MAX_2D_PLUSE - 1))
        {
            index += 1;
        }
        else
        {
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (index > 1)
        {
            index -= 1;
        }
        else
        {
            return (MIN);
        }
    }
    else
    {
        return ERROR;
    }

    m_projectCalcPara.d2Pluse = index;
    m_fpga.Send2DPulseNum(index);

    EKnobReturn ret = OK;
    if (index == 0)
        ret = MIN;
    else if (index == (MAX_2D_PLUSE - 1))
        ret = MAX;
    else
        ret = OK;

        ///> draw
    char value[64];
    sprintf(value, "%d", index);
    KnobProjectMode::GetInstance()->SetValue(4, value, ret);

    return (ret);
}

EKnobReturn ProjectCalc2D::ChangePwPulse(EKnobOper oper)
{
    int  index;
    index = m_projectCalcPara.pwPluse;

    if (oper == ADD)
    {
        if (index < (MAX_PW_PLUSE - 1))
        {
            index += 1;
        }
        else
        {
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (index > 1)
        {
            index -= 1;
        }
        else
        {
            return (MIN);
        }
    }
    else
    {
        return ERROR;
    }

    m_projectCalcPara.pwPluse = index;
    m_fpga.SendPWPulseNum(index);

    EKnobReturn ret = OK;
    if (index == 0)
        ret = MIN;
    else if (index == (MAX_PW_PLUSE - 1))
        ret = MAX;
    else
        ret = OK;

    char value[64];
    sprintf(value, "%d", index);
    KnobProjectMode::GetInstance()->SetValue(5, value, ret);

    return (ret);
}

EKnobReturn ProjectCalc2D::ChangeCfmPulse(EKnobOper oper)
{
    int  index;
    index = m_projectCalcPara.cfmPluse;

    if (oper == ADD)
    {
        if (index < (MAX_CFM_PLUSE - 1))
        {
            index += 1;
        }
        else
        {
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (index > 1)
        {
            index -= 1;
        }
        else
        {
            return (MIN);
        }
    }
    else
    {
        return ERROR;
    }

    m_projectCalcPara.cfmPluse = index;
    m_fpga.SendCFMPulseNum(index);

    EKnobReturn ret = OK;
    if (index == 0)
        ret = MIN;
    else if (index == (MAX_CFM_PLUSE - 1))
        ret = MAX;
    else
        ret = OK;

    char value[64];
    sprintf(value, "%d", index);
    KnobProjectMode::GetInstance()->SetValue(6, value, ret);

    return (ret);

}
EKnobReturn ProjectCalc2D::ChangeProbeR(EKnobOper oper)
{
   int index;
    index = m_projectCalcPara.probeR;
    printf("index = %d\n", index);
    if (oper == ADD)
    {
        if (index < (MAX_PROBE_R - 1))
        {
            index += 10;
        }
        else
        {
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (index > 10)
        {
            index -= 10;
        }
        else
        {
            return (MIN);
        }
    }
    else
    {
        return ERROR;
    }

    printf("index = %d\n", index);
    m_projectCalcPara.probeR = index;
   //m_pKps.ProbeRAndAngle();
  //  m_ptrCalc->CalcFilter();
    Img2D::GetInstance()->InitProbeCalc2D();

    m_ptrCalc->CalcEmitDelay();
    EKnobReturn ret = OK;
    if (index == 0)
        ret = MIN;
    else if (index == (MAX_PROBE_R - 1))
        ret = MAX;
    else
        ret = OK;

    printf("index = %d\n", index);
    char value[50];
    sprintf(value, "%d", index);
    printf("value = %s\n", value);
    KnobProjectMode::GetInstance()->SetValue(7, value, ret);
    return (ret);

}

EKnobReturn ProjectCalc2D::ChangeProbeAngle(EKnobOper oper)
{

     int index;
    index = m_projectCalcPara.probeAngle;
    if (oper == ADD)
    {
        if (index < (MAX_PROBE_ANGLE - 1))
        {
            index += 10;
        }
        else
        {
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (index > 10)
        {
            index -= 10;
        }
        else
        {
            return (MIN);
        }
    }
    else
    {
        return ERROR;
    }

    m_projectCalcPara.probeAngle = index;

//  m_pKps.ProbeRAndAngle();
  //  m_ptrCalc->CalcFilter();

    Img2D::GetInstance()->InitProbeCalc2D();
    m_ptrCalc->CalcEmitDelay();
    EKnobReturn ret = OK;
    if (index == 0)
        ret = MIN;
    else if (index == (MAX_PROBE_ANGLE - 1))
        ret = MAX;
    else
        ret = OK;

    char value[64];
    sprintf(value, "%d", index);
    KnobProjectMode::GetInstance()->SetValue(8, value, ret);
    return (ret);
}

#if 0
EKnobReturn ProjectCalc2D::ChangeBandPassWindowFunc(EKnobOper oper)
{
    ///> test receive aperture calc window function
    int index = m_projectCalcPara.windowFuncBPIndex;
    if (oper == ROTATE)
    {
        if(index <(MAX_BP_WINDOW_FUNC - 1))
        {
            index += 1;
        }
        else
        {
            index = 0;
        }
    }

    else if (oper == ADD)
    {
        if (index < (MAX_BP_WINDOW_FUNC - 1))
        {
            index += 1;
        }
        else
        {
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (index > 0)
        {
            index -= 1;
        }
        else
        {
            return (MIN);
        }
    }
    else
    {
        return ERROR;
    }

    m_projectCalcPara.windowFuncBPIndex = index;
    m_ptrCalc->CalcFilter();

    EKnobReturn ret = OK;
    if (index == 0)
        ret = MIN;
    else if (index == (MAX_BP_WINDOW_FUNC - 1))
        ret = MAX;
    else
        ret = OK;

    ///> draw
    char value[64];
    sprintf(value, "%d", index);
    KnobProjectMode::GetInstance()->SetValue(5, value, ret);
    ShowCurPageParaValue(1);
    return (ret);
}

EKnobReturn ProjectCalc2D::ChangeLowPassWindowFunc(EKnobOper oper)
{
    ///> test receive aperture calc window function
    int index = m_projectCalcPara.windowFuncLPIndex;
    if (oper == ROTATE)
    {
        if(index <(MAX_LP_WINDOW_FUNC - 1))
        {
            index += 1;
        }
        else
        {
            index = 0;
        }
    }

    else if (oper == ADD)
    {
        if (index < (MAX_LP_WINDOW_FUNC - 1))
        {
            index += 1;
        }
        else
        {
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (index > 0)
        {
            index -= 1;
        }
        else
        {
            return (MIN);
        }
    }
    else
    {
        return ERROR;
    }

    m_projectCalcPara.windowFuncLPIndex = index;
    m_ptrCalc->CalcFilter();

    EKnobReturn ret = OK;
    if (index == 0)
        ret = MIN;
    else if (index == (MAX_LP_WINDOW_FUNC - 1))
        ret = MAX;
    else
        ret = OK;

    ///> draw
    char value[64];
    sprintf(value, "%d", index);
    KnobProjectMode::GetInstance()->SetValue(6, value, ret);
    ShowCurPageParaValue(1);
    return (ret);
}

EKnobReturn ProjectCalc2D::ChangeEmissionPara(EKnobOper oper)
{
    int index = m_emitIndex;
    if (oper == ROTATE)
    {
        if(index <= (ProbeDataPackageMan::MAX_GROUP - 1))
        {
            index += 1;
        }
        else
        {
            index = 1;
        }
    }

    else if (oper == ADD)
    {
        if (index <= (ProbeDataPackageMan::MAX_GROUP - 1))
        {
            index += 1;
        }
        else
        {
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (index > 1)
        {
            index -= 1;
        }
        else
        {
            return (MIN);
        }
    }
    else
    {
        return ERROR;
    }
    m_emitIndex = index;
    ProbeDataPackageMan::GetInstance()->SendEmissionPara(index);

    EKnobReturn ret = OK;
    if (index == 1)
        ret = MIN;
    else if (index == ProbeDataPackageMan::MAX_GROUP)
        ret = MAX;
    else
        ret = OK;

    ///> draw
    char value[64];
    sprintf(value, "%d", index);
    KnobProjectMode::GetInstance()->SetValue(7, value, ret);
    return (ret);
}
#endif
#endif

void ProjectCalc2D::SaveCalcPara2D(char* name)
{
    char path[256];
    sprintf(path, "%s%s", CFG_RES_PATH, PROJECT_DEBUG_FILE);
    IniFile ini(path);

    Img2D* ptrImg2D = Img2D::GetInstance();
    string probe_type = TopArea::GetInstance()->GetProbeType();
    int probeIndex = 0;
    int harmonicIndex = ptrImg2D->GetHarmonicFreqIndex();
    int freqIndex = ptrImg2D->GetFreqIndex();
    int i;
    for (i = 0; i < NUM_PROBE; ++i)
    {
        if (strcmp(probe_type.c_str(), PROBE_LIST[i].c_str()) == 0)
            probeIndex = i;
    }

    ExamItem em;
    ExamItem::ProjectDebugParaItem debugParaItem;

    debugParaItem.mode = ScanMode::D2;
    debugParaItem.depth = ptrImg2D->GetDepthIndex();
    debugParaItem.statusTHI = ptrImg2D->GetStatusTHI();
    debugParaItem.freq_Index = freqIndex;
    debugParaItem.harmonic_Index = harmonicIndex;
    debugParaItem.probe_Index = probeIndex;

    if (debugParaItem.statusTHI)
        debugParaItem.freq = ProbeSocket::FREQ_THI[probeIndex][freqIndex];
    else
        debugParaItem.freq = ProbeSocket::FREQ2D[probeIndex][freqIndex].emit;

    // write demod filter para
    debugParaItem.defaultPara.mode = ScanMode::D2;
    debugParaItem.defaultPara.pro2d.freq = debugParaItem.freq;
        debugParaItem.defaultPara.pro2d.FilterSection = m_projectCalcPara.filterSecIndex;
    for (i = 0; i < ExamItem::BAND_PASS_SIZE_D2; i++)
    {
        debugParaItem.defaultPara.pro2d.BandPassW[i] = m_projectCalcPara.bandPassW[i];
    }

    debugParaItem.defaultPara.pro2d.txFNum = m_projectCalcPara.txFNum;
    debugParaItem.defaultPara.pro2d.wpScale = m_projectCalcPara.wpScale;
    debugParaItem.defaultPara.pro2d.d2Pluse = m_projectCalcPara.d2Pluse;
    debugParaItem.defaultPara.pro2d.pwPluse = m_projectCalcPara.pwPluse;
    debugParaItem.defaultPara.pro2d.cfmPluse = m_projectCalcPara.cfmPluse;
    debugParaItem.defaultPara.probeR = m_projectCalcPara.probeR;
    debugParaItem.defaultPara.probeAngle = m_projectCalcPara.probeAngle;

    char user[128];
    int len = strlen(probe_type.c_str());
        strncpy(user, name+len+1, strlen(name)-len);

    em.WriteProDebugParaItem(&debugParaItem, probe_type.c_str(), user, &ini);
}

void ProjectCalc2D::ReadCalcPara2D(const char* name)
{
    int i;
    char path[256];
    sprintf(path, "%s%s", CFG_RES_PATH, PROJECT_DEBUG_FILE);
    IniFile ini(path);

    ProbeSocket::ProbePara para;
    ProbeMan::GetInstance()->GetCurProbe(para);

    char user[128];
    int len = strlen(para.model);
    strncpy(user, name+len+1, strlen(name)-len);

    ExamItem em;
    ExamItem::ProjectDebugParaItem debugParaItem;
    em.ReadProDebugParaItem(&debugParaItem, para.model, user, &ini);
     if(debugParaItem.statusTHI)
    {
      printf("enter thi ----------------------\n");
        Img2D::GetInstance()->ChangeHTIForProject(debugParaItem.harmonic_Index);
    }
    else
    {
        Img2D::GetInstance()->ChangeFreqForProject(debugParaItem.freq_Index);
    }
    Img2D::GetInstance()->Depth(debugParaItem.depth);

    // read demod filter para
    m_projectCalcPara.filterSecIndex = debugParaItem.defaultPara.pro2d.FilterSection;
    for (i = 0; i < ExamItem::BAND_PASS_SIZE_D2; i++)
    {
        m_projectCalcPara.bandPassW[i] = debugParaItem.defaultPara.pro2d.BandPassW[i];
    }

    m_projectCalcPara.txFNum = debugParaItem.defaultPara.pro2d.txFNum;
    m_projectCalcPara.wpScale = debugParaItem.defaultPara.pro2d.wpScale;
    m_projectCalcPara.d2Pluse = debugParaItem.defaultPara.pro2d.d2Pluse;
    m_projectCalcPara.cfmPluse = debugParaItem.defaultPara.pro2d.cfmPluse;
    m_projectCalcPara.pwPluse = debugParaItem.defaultPara.pro2d.pwPluse;
    m_projectCalcPara.probeR = debugParaItem.defaultPara.probeR;
    m_projectCalcPara.probeAngle = debugParaItem.defaultPara.probeAngle;
    // realize Func
    Img2D::GetInstance()->InitProbeCalc2D();
    m_ptrCalc->CalcEmitDelay();
    m_ptrCalc->CalcFilter();
    m_fpga.Send2DPulseNum(m_projectCalcPara.d2Pluse);
    m_fpga.SendPWPulseNum(m_projectCalcPara.pwPluse);
    m_fpga.SendCFMPulseNum(m_projectCalcPara.cfmPluse);

    // draw --- NULL
    char value[64];
    EKnobReturn ret = OK;

    if (m_projectCalcPara.filterSecIndex > 0)
    {
        sprintf(value, "%d", m_projectCalcPara.filterSecIndex);
        KnobProjectMode::GetInstance()->SetValue(0, value, ret);
        sprintf(value, "%.2f",m_projectCalcPara.bandPassW[m_projectCalcPara.filterSecIndex-1]);
        KnobProjectMode::GetInstance()->SetValue(1, value, ret);
        sprintf(value, "%.2f",m_projectCalcPara.txFNum);
        KnobProjectMode::GetInstance()->SetValue(2, value, ret);
        sprintf(value, "%.2f",m_projectCalcPara.wpScale);
        KnobProjectMode::GetInstance()->SetValue(3, value, ret);
        sprintf(value, "%.2f",m_projectCalcPara.d2Pluse);
        KnobProjectMode::GetInstance()->SetValue(4, value, ret);
        sprintf(value, "%.2f",m_projectCalcPara.pwPluse);
        KnobProjectMode::GetInstance()->SetValue(5, value, ret);
        sprintf(value, "%.2f",m_projectCalcPara.cfmPluse);
        KnobProjectMode::GetInstance()->SetValue(6, value, ret);
        sprintf(value, "%.2f",m_projectCalcPara.probeR);
        KnobProjectMode::GetInstance()->SetValue(7, value, ret);
        sprintf(value, "%.2f",m_projectCalcPara.probeAngle);
        KnobProjectMode::GetInstance()->SetValue(8, value, ret);
    }
    else
    {
        for (i = 0; i < 9; i++)
    {
        sprintf(value, "%s", "");
        KnobProjectMode::GetInstance()->SetValue(i, value, ret);
    }

    }
    // show cur page para vaule
    unsigned int curLevel;
    curLevel = KnobProjectMode::GetInstance()->GetCurPageLevel();
    ShowCurPageParaValue(curLevel);

}

void ProjectCalc2D::ShowCurPageParaValue(int row)
{
    switch(row)
    {
        case 0:
            {
                float curW1 = 0.0, curW2 = 0.0, curWp = 0.0;
                char strHint[256];
                if (m_projectCalcPara.filterSecIndex > 0)
                {
                    curW1 = m_projectCalcPara.bandPassW[m_projectCalcPara.filterSecIndex - 1];

                  //  sprintf(strHint, "Demod Section = %d\n W1 = %.1f {%.1f, %.1f, %.1f, %.1f, %.1f}\n-----------------------------\n", m_projectCalcPara.filterSecIndex, curW1, m_projectCalcPara.bandPassW[0], m_projectCalcPara.bandPassW[1], m_projectCalcPara.bandPassW[2], m_projectCalcPara.bandPassW[3], m_projectCalcPara.bandPassW[4]);
                    sprintf(strHint, "FC {%.1f, %.1f, %.1f, %.1f, %.1f}\n-----------------------------\n",   m_projectCalcPara.bandPassW[0], m_projectCalcPara.bandPassW[1], m_projectCalcPara.bandPassW[2], m_projectCalcPara.bandPassW[3], m_projectCalcPara.bandPassW[4]);

                }
                else
                {
                   float w1[5], w2[5], wp[5];
                    if(m_harmonic)
                    {
                        for(int i = 0; i < 5; i++)
                        {
                            w1[i] = float(ProbeSocket::BAND_PASS_FILTER_FC[m_probeIndex][m_freqIndex][i] );
                        }
                    }
                    else
                    {
                        for(int i = 0; i < 5; i++)
                        {
                            w1[i] = float(ProbeSocket::BAND_PASS_FILTER_FC_BASE_FREQ[m_probeIndex][m_freqIndex][i] );
                        }
                    }
                    curW1 = w1[0];
                    sprintf(strHint, "Demod Section = %d\n W1 = %.1f {%.1f, %.1f, %.1f, %.1f, %.1f}\n-----------------------------\n", m_projectCalcPara.filterSecIndex, curW1, w1[0], w1[1], w1[2], w1[3], w1[4]);
                }

                HintArea::GetInstance()->UpdateHint(_(strHint), 0);
            }
            break;
        case 1:
            {
             //   char strHint[256];
             //   sprintf(strHint, "BandPass WinFunc = %d\n LowPass WinFunc = %d\n------------------------------\n", m_projectCalcPara.windowFuncBPIndex, m_projectCalcPara.windowFuncLPIndex);
              //  HintArea::GetInstance()->UpdateHint(_(strHint), 0);
            }
            break;
    }
}

///> update knob area

void ProjectCalc2D::UpdateDemodSection(int section, EKnobReturn ret)
{
    char value[32];
    sprintf(value, "%d", section);
    KnobProjectMode::GetInstance()->SetValue(0, value, ret);
    ShowCurPageParaValue(0);
}

void ProjectCalc2D::UpdateBandPassW(float data, EKnobReturn ret)
{
    char value[32];
    sprintf(value, "%.2f", data);
    KnobProjectMode::GetInstance()->SetValue(1, value, ret);
    ShowCurPageParaValue(0);
}

void ProjectCalc2D::UpdateTxFNum(float data, EKnobReturn ret)
{
    char value[32];
    sprintf(value, "%.2f", data);
    KnobProjectMode::GetInstance()->SetValue(2, value, ret);
    ShowCurPageParaValue(0);
}

void ProjectCalc2D::UpdateWpScale(float data, EKnobReturn ret)
{
    char value[32];
    sprintf(value, "%.2f", data);
    KnobProjectMode::GetInstance()->SetValue(3, value, ret);
    ShowCurPageParaValue(0);
}

void ProjectCalc2D::Update2DPulse(float data, EKnobReturn ret)
{
    char value[32];
    sprintf(value, "%.2f", data);
    KnobProjectMode::GetInstance()->SetValue(4, value, ret);
    //ShowCurPageParaValue(0);
}

void ProjectCalc2D::UpdatePwPulse(float data, EKnobReturn ret)
{
    char value[32];
    sprintf(value, "%.2f", data);
    KnobProjectMode::GetInstance()->SetValue(5, value, ret);
  //  ShowCurPageParaValue(0);
}

void ProjectCalc2D::UpdateCfmPulse(float data, EKnobReturn ret)
{
    char value[32];
    sprintf(value, "%.2f", data);
    KnobProjectMode::GetInstance()->SetValue(6, value, ret);
  //  ShowCurPageParaValue(0);
}

void ProjectCalc2D::UpdateProbeR(float data, EKnobReturn ret)
{
    char value[32];
    sprintf(value, "%.2f", data);
    KnobProjectMode::GetInstance()->SetValue(7, value, ret);
   // ShowCurPageParaValue(0);
}
void ProjectCalc2D::UpdateProbeAngle(float data, EKnobReturn ret)
{
    char value[32];
    sprintf(value, "%.2f", data);
    KnobProjectMode::GetInstance()->SetValue(8, value, ret);
 //   ShowCurPageParaValue(0);
}

#endif