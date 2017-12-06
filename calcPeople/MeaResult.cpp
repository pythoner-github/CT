#include <stdio.h>
#include <gtk/gtk.h>
#include "Def.h"
#include "calcPeople/MeaResult.h"
#include "calcPeople/MeaCalcFun.h"
#include "sysMan/SysMeasurementSetting.h"
#include "calcPeople/MenuCalcNew.h"
#include "patient/PatientInfo.h"
#include "sysMan/CalcSetting.h"
#include "calcPeople/ViewReport.h"

MeaResult* MeaResult::m_ptrInstance;

MeaResult::MeaResult()
{
    InitResultInfo();
}

MeaResult::~MeaResult()
{
    if (m_ptrInstance != NULL)
        delete m_ptrInstance;
}

MeaResult* MeaResult::GetInstance()
{
    if (m_ptrInstance == NULL)
        m_ptrInstance = new MeaResult;
    return m_ptrInstance;
}

void MeaResult::InitResultInfo(void)
{
    int i;

    for (i=0; i<SECTION_END-SECTION_START; i++)
    {
        SecResultInfo[i].secOrder = SECTION_END;
        SecResultInfo[i].genStart = CALC_END;
        SecResultInfo[i].genEnd = CALC_END;
        SecResultInfo[i].singleStart = CALC_END;
        SecResultInfo[i].multiStart = CALC_END;
        SecResultInfo[i].calcStart = CALC_END;
        SecResultInfo[i].singleEnd = CALC_END;
        SecResultInfo[i].multiStart = CALC_END;
        SecResultInfo[i].calcStart = CALC_END;
        SecResultInfo[i].ptrSingleResult = NULL;
        SecResultInfo[i].ptrMultiResult = NULL;
        SecResultInfo[i].ptrCalcResult = NULL;
    }

    SecResultInfo[ABDO_M].secOrder = ABDO_M;
    SecResultInfo[ABDO_M].singleStart = ABD_MEA_START;
    SecResultInfo[ABDO_M].multiStart = ABD_MULTI_START;
    SecResultInfo[ABDO_M].calcStart = ABD_CALC_START;
    SecResultInfo[ABDO_M].singleEnd = ABD_MEA_END;
    SecResultInfo[ABDO_M].multiEnd = ABD_MULTI_END;
    SecResultInfo[ABDO_M].calcEnd = ABD_CALC_END;
    SecResultInfo[ABDO_M].genStart = ABD_MEA_START;
    SecResultInfo[ABDO_M].genEnd = ABD_MEA_END;
    SecResultInfo[ABDO_M].ptrSingleResult = (SingleResult)(&MeaResultAbd);
    SecResultInfo[ABDO_M].ptrMultiResult = (MultiResult)(&AbdResultMulti);
    SecResultInfo[ABDO_M].ptrCalcResult = (CalcResult)(&CalcResultAbd);

    SecResultInfo[ADULT_M].secOrder = ADULT_M;
    SecResultInfo[ADULT_M].singleStart = ADULT_MEA_START;
    SecResultInfo[ADULT_M].multiStart = ADULT_MULTI_START;
    SecResultInfo[ADULT_M].calcStart = ADULT_CALC_START;
    SecResultInfo[ADULT_M].singleEnd = ADULT_MEA_END;
    SecResultInfo[ADULT_M].multiEnd = ADULT_MULTI_END;
    SecResultInfo[ADULT_M].calcEnd = ADULT_CALC_END;
    SecResultInfo[ADULT_M].genStart = ADULT_MEA_START;
    SecResultInfo[ADULT_M].genEnd = ADULT_MEA_END;
    SecResultInfo[ADULT_M].ptrSingleResult = (SingleResult)(&MeaResultAdult);
    SecResultInfo[ADULT_M].ptrMultiResult = (MultiResult)(&AdultResultMulti);
    SecResultInfo[ADULT_M].ptrCalcResult = (CalcResult)(&CalcResultAdult);

    SecResultInfo[UR_M].secOrder = UR_M;
    SecResultInfo[UR_M].singleStart = UR_MEA_START;
    SecResultInfo[UR_M].multiStart = UR_MULTI_START;
    SecResultInfo[UR_M].calcStart = UR_CALC_START;
    SecResultInfo[UR_M].singleEnd = UR_MEA_END;
    SecResultInfo[UR_M].multiEnd = UR_MULTI_END;
    SecResultInfo[UR_M].calcEnd = UR_CALC_END;
    SecResultInfo[UR_M].genStart = UR_MEA_START;
    SecResultInfo[UR_M].genEnd = UR_MEA_END;
    SecResultInfo[UR_M].ptrSingleResult = (SingleResult)(&MeaResultUR);
    SecResultInfo[UR_M].ptrMultiResult = (MultiResult)(&URResultMulti);
    SecResultInfo[UR_M].ptrCalcResult = (CalcResult)(&CalcResultUR);

    SecResultInfo[OB_M].secOrder = OB_M;
    SecResultInfo[OB_M].singleStart = OB_MEA_START;
    SecResultInfo[OB_M].multiStart = OB_MULTI_START;
    SecResultInfo[OB_M].calcStart = OB_CALC_START;
    SecResultInfo[OB_M].singleEnd = OB_MEA_END;
    SecResultInfo[OB_M].multiEnd = OB_MULTI_END;
    SecResultInfo[OB_M].calcEnd = OB_CALC_END;
    SecResultInfo[OB_M].genStart = OB_MEA_START;
    SecResultInfo[OB_M].genEnd = OB_MEA_END;
    SecResultInfo[OB_M].ptrSingleResult = (SingleResult)(&MeaResultOB1);
    SecResultInfo[OB_M].ptrMultiResult = (MultiResult)(&OBResultMulti1);
    SecResultInfo[OB_M].ptrCalcResult = (CalcResult)(&CalcResultOB1);

    SecResultInfo[GYN_M].secOrder = GYN_M;
    SecResultInfo[GYN_M].singleStart = GYN_MEA_START;
    SecResultInfo[GYN_M].multiStart = GYN_MULTI_START;
    SecResultInfo[GYN_M].calcStart = GYN_CALC_START;
    SecResultInfo[GYN_M].singleEnd = GYN_MEA_END;
    SecResultInfo[GYN_M].multiEnd = GYN_MULTI_END;
    SecResultInfo[GYN_M].calcEnd = GYN_CALC_END;
    SecResultInfo[GYN_M].genStart = GYN_MEA_START;
    SecResultInfo[GYN_M].genEnd = GYN_MEA_END;
    SecResultInfo[GYN_M].ptrSingleResult = (SingleResult)(&MeaResultGYN);
    SecResultInfo[GYN_M].ptrMultiResult = (MultiResult)(&GYNResultMulti);
    SecResultInfo[GYN_M].ptrCalcResult = (CalcResult)(&CalcResultGYN);

    SecResultInfo[SP_M].secOrder = SP_M;
    SecResultInfo[SP_M].singleStart = SP_MEA_START;
    SecResultInfo[SP_M].multiStart = SP_MULTI_START;
    SecResultInfo[SP_M].calcStart = SP_CALC_START;
    SecResultInfo[SP_M].singleEnd = SP_MEA_END;
    SecResultInfo[SP_M].multiEnd = SP_MULTI_END;
    SecResultInfo[SP_M].calcEnd = SP_CALC_END;
    SecResultInfo[SP_M].genStart = SP_MEA_START;
    SecResultInfo[SP_M].genEnd = SP_MEA_END;
    SecResultInfo[SP_M].ptrSingleResult = (SingleResult)(&MeaResultSP);
    SecResultInfo[SP_M].ptrMultiResult = (MultiResult)(&SPResultMulti);
    SecResultInfo[SP_M].ptrCalcResult = (CalcResult)(&CalcResultSP);
#ifdef EMP_322
#else
    SecResultInfo[VS_M].secOrder = VS_M;
    SecResultInfo[VS_M].singleStart = VS_MEA_START;
    SecResultInfo[VS_M].multiStart = VS_MULTI_START;
    SecResultInfo[VS_M].calcStart = VS_CALC_START;
    SecResultInfo[VS_M].singleEnd = VS_MEA_END;
    SecResultInfo[VS_M].multiEnd = VS_MULTI_END;
    SecResultInfo[VS_M].calcEnd = VS_CALC_END;
    SecResultInfo[VS_M].genStart = VS_MEA_START;
    SecResultInfo[VS_M].genEnd = VS_MEA_END;
    SecResultInfo[VS_M].ptrSingleResult = (SingleResult)(&MeaResultVS);
    SecResultInfo[VS_M].ptrMultiResult = (MultiResult)(&VSResultMulti);
    SecResultInfo[VS_M].ptrCalcResult = (CalcResult)(&CalcResultVS);
#endif
    SecResultInfo[FETAL_M].secOrder = FETAL_M;
    SecResultInfo[FETAL_M].singleStart = FETAL_MEA_START;
    SecResultInfo[FETAL_M].multiStart = FETAL_MULTI_START;
    SecResultInfo[FETAL_M].calcStart = FETAL_CALC_START;
    SecResultInfo[FETAL_M].singleEnd = FETAL_MEA_END;
    SecResultInfo[FETAL_M].multiEnd = FETAL_MULTI_END;
    SecResultInfo[FETAL_M].calcEnd = FETAL_CALC_END;
    SecResultInfo[FETAL_M].genStart = FETAL_MEA_START;
    SecResultInfo[FETAL_M].genEnd = FETAL_MEA_END;
    SecResultInfo[FETAL_M].ptrSingleResult = (SingleResult)(&MeaResultFetal);
    SecResultInfo[FETAL_M].ptrMultiResult = (MultiResult)(&FetalResultMulti);
    SecResultInfo[FETAL_M].ptrCalcResult = (CalcResult)(&CalcResultFetal);
#ifdef EMP_322
#else
    SecResultInfo[TCD_M].secOrder = TCD_M;
    SecResultInfo[TCD_M].singleStart =  TCD_MEA_START;
    SecResultInfo[TCD_M].multiStart = TCD_MULTI_START;
    SecResultInfo[TCD_M].calcStart = TCD_CALC_START;
    SecResultInfo[TCD_M].singleEnd =  TCD_MEA_END;
    SecResultInfo[TCD_M].multiEnd = TCD_MULTI_END;
    SecResultInfo[TCD_M].calcEnd = TCD_CALC_END;
    SecResultInfo[TCD_M].genStart = TCD_MEA_START;
    SecResultInfo[TCD_M].genEnd = TCD_MEA_END;
    SecResultInfo[TCD_M].ptrSingleResult = (SingleResult)(&MeaResultTCD);
    SecResultInfo[TCD_M].ptrMultiResult = (MultiResult)(&TCDResultMulti);
    SecResultInfo[TCD_M].ptrCalcResult = (CalcResult)(&CalcResultTCD);
#endif
    SecResultInfo[ORTHO_M].secOrder = ORTHO_M;
    SecResultInfo[ORTHO_M].singleStart = ORTHO_MEA_START;
    SecResultInfo[ORTHO_M].multiStart = ORTHO_MULTI_START;
    SecResultInfo[ORTHO_M].calcStart = ORTHO_CALC_START;
    SecResultInfo[ORTHO_M].singleEnd = ORTHO_MEA_END;
    SecResultInfo[ORTHO_M].multiEnd = ORTHO_MULTI_END;
    SecResultInfo[ORTHO_M].calcEnd = ORTHO_CALC_END;
    SecResultInfo[ORTHO_M].genStart = ORTHO_MEA_START;
    SecResultInfo[ORTHO_M].genEnd = ORTHO_MEA_END;
    SecResultInfo[ORTHO_M].ptrSingleResult = (SingleResult)(&MeaResultOrtho);
    SecResultInfo[ORTHO_M].ptrMultiResult = (MultiResult)(&OrthoResultMulti);
    SecResultInfo[ORTHO_M].ptrCalcResult = (CalcResult)(&CalcResultOrtho);

    SecResultInfo[USER_M].secOrder = USER_M;
    SecResultInfo[USER_M].singleStart = USER_START;
//  SecResultInfo[ORTHO_M].multiStart = ORTHO_MULTI_START;
//  SecResultInfo[ORTHO_M].calcStart = ORTHO_CALC_START;
    SecResultInfo[USER_M].singleEnd = USER_END;
//  SecResultInfo[ORTHO_M].multiEnd = ORTHO_MULTI_END;
//  SecResultInfo[ORTHO_M].calcEnd = ORTHO_CALC_END;
    SecResultInfo[USER_M].genStart = USER_START;
    SecResultInfo[USER_M].genEnd = USER_END;
    SecResultInfo[USER_M].ptrSingleResult = (SingleResult)(&MeaResultUser);
//  SecResultInfo[ORTHO_M].ptrMultiResult = (MultiResult)(&OrthoResultMulti);
//  SecResultInfo[ORTHO_M].ptrCalcResult = (CalcResult)(&CalcResultOrtho);
//添加 动物超 TD
#ifdef VET
//hlx
    SecResultInfo[TD_M].secOrder = TD_M;
    SecResultInfo[TD_M].singleStart = TD_MEA_START;
    SecResultInfo[TD_M].multiStart = TD_MULTI_START;
    SecResultInfo[TD_M].calcStart = TD_CALC_START;
    SecResultInfo[TD_M].singleEnd = TD_MEA_END;
    SecResultInfo[TD_M].multiEnd = TD_MULTI_END;
    SecResultInfo[TD_M].calcEnd = TD_CALC_END;
    SecResultInfo[TD_M].genStart = TD_MEA_START;
    SecResultInfo[TD_M].genEnd = TD_MEA_END;
    SecResultInfo[TD_M].ptrSingleResult = (SingleResult)(&MeaResultTD);
    SecResultInfo[TD_M].ptrMultiResult = (MultiResult)(&TDResultMulti);
    SecResultInfo[TD_M].ptrCalcResult = (CalcResult)(&CalcResultTD);

//添加 动物超 ANOB
//hlx
    SecResultInfo[ANOB_M].secOrder = ANOB_M;
    SecResultInfo[ANOB_M].singleStart = ANOB_MEA_START;
    SecResultInfo[ANOB_M].multiStart = ANOB_MULTI_START;
    SecResultInfo[ANOB_M].calcStart = ANOB_CALC_START;
    SecResultInfo[ANOB_M].singleEnd = ANOB_MEA_END;
    SecResultInfo[ANOB_M].multiEnd = ANOB_MULTI_END;
    SecResultInfo[ANOB_M].calcEnd = ANOB_CALC_END;
    SecResultInfo[ANOB_M].genStart = ANOB_MEA_START;
    SecResultInfo[ANOB_M].genEnd = ANOB_MEA_END;
    SecResultInfo[ANOB_M].ptrSingleResult = (SingleResult)(&MeaResultAnOB);
    SecResultInfo[ANOB_M].ptrMultiResult = (MultiResult)(&AnOBResultMulti);
    SecResultInfo[ANOB_M].ptrCalcResult = (CalcResult)(&CalcResultAnOB);
#endif
}

//value[]:传入的结果值，现在最多有两个
//item:测量项在MeasureFactoryAbdo的枚举号
//meaResult:保存测量结果的数组
//valueNum:标记本测量本测量项是一个测量值还是两个测量值:1－一个，2－两个
int MeaResult::SetLastValue(const double value[], const int item, const int valueNums)
{
    int sign;
    int itemTmp;
    int position = 0;
    int section;
    ResultMulti *ptrResultMulti = NULL;
    double (*ptrResultSingle)[MEA_TIMES] = NULL;
    int i, j;

    if (item < BASIC_MEA_END)
        return MEA_SUCCESS;

    sign = ItemLabel(item, &itemTmp, &ptrResultMulti, &ptrResultSingle, NULL, NULL, &section, FETAL_BY_SET);
    if (sign == MEA_FAIL)
        return sign;

    if (valueNums == MEA_SINGLE)
    {
        for (i=0; i<MEA_TIMES; i++)
        {
            if (((int)((*(ptrResultSingle+itemTmp))[MEA_TIMES - 1 - i]) == INVALID_VAL) || (i == MEA_TIMES - 1 ))
            {
                for (j=i; j>0; j--)
                {
                    (*(ptrResultSingle+itemTmp))[MEA_TIMES - j - 1] = (*(ptrResultSingle+itemTmp))[MEA_TIMES - j];
                }
                (*(ptrResultSingle+itemTmp))[MEA_TIMES - 1] = value[0];
                break;
            }
        }
        return MEA_SUCCESS;
    }
    else
    {
        position = itemTmp;
        for (i=0; i<MEA_TIMES; i++)
        {
            if (((*(ptrResultMulti + position)).result[MEA_TIMES - 1 - i] == INVALID_VAL) || (i == MEA_TIMES - 1 ))
            {
                for (j=i; j>0; j--)
                {
                    (*(ptrResultMulti+position)).result[MEA_TIMES - j - 1] = (*(ptrResultMulti+position)).result[MEA_TIMES - j];
                    (*(ptrResultMulti+position)).result[MEA_TIMES*2 - j - 1] = (*(ptrResultMulti+position)).result[MEA_TIMES*2 - j];
                    (*(ptrResultMulti+position)).result[MEA_TIMES*3 - j - 1] = (*(ptrResultMulti+position)).result[MEA_TIMES*3 - j];
                    (*(ptrResultMulti+position)).result[MEA_TIMES_MULTI - j - 1] = (*(ptrResultMulti+position)).result[MEA_TIMES_MULTI - j];
                }
                (*(ptrResultMulti+position)).result[MEA_TIMES - 1] = value[0];
                (*(ptrResultMulti+position)).result[MEA_TIMES*2 - 1] = value[1];
                (*(ptrResultMulti+position)).result[MEA_TIMES*3 - 1] = value[2];
                (*(ptrResultMulti+position)).result[MEA_TIMES_MULTI - 1] = value[3];
                break;
            }
        }

        return MEA_SUCCESS;
    }
}

int MeaResult::GetLastValue(const int item, double value[MEA_MULTI], int obFetal)
{
    int item_tmp;
    int position;
    ResultMulti *ptrResultMulti = NULL;
    double (*ptrResultSingle)[MEA_TIMES] = NULL;
    int sign;
    int multiVolume;
    int section;
    int i;

    sign = ItemLabel(item, &item_tmp, &ptrResultMulti, &ptrResultSingle, NULL, &multiVolume, &section, obFetal);

    if (sign == MEA_FAIL)
        return sign;

    if ( ((int)(*(ptrResultSingle + item_tmp))[MEA_TIMES - 1] != INVALID_VAL) || ((int)(*(ptrResultSingle + item_tmp))[0] == MULTI_SIGN) )//两个测量值的或单个的有数据
    {
        if ( (int)(*(ptrResultSingle + item_tmp))[0] == MULTI_SIGN )//多个测量值的
        {
            position = (int)(*(ptrResultSingle + item_tmp))[1];
            if ((position == MULTI_SIGN)||(position > multiVolume))
                return MEA_ERROR;

            if ((int)((*(ptrResultMulti + position)).result[MEA_TIMES - 1]) == INVALID_VAL || (int)((*(ptrResultMulti + position)).result[MEA_TIMES - 1]) == (double)0)//两个的没有数据
                return MEA_FAIL;

            for(i=0; i<MEA_MULTI; i++)
            {
                value[i] = (*(ptrResultMulti + position)).result[MEA_TIMES * (i+1) - 1];
            }
            return MEA_SUCCESS;
        }
        else//单个测量值的
        {
            for(i=0; i<MEA_MULTI; i++)
            {
                if (i==0)
                    value[0] = (*(ptrResultSingle + item_tmp))[MEA_TIMES - 1];
                else
                    value[i] = INVALID_VAL;
            }
            return MEA_SUCCESS;
        }
    }
    else
    {
        return MEA_FAIL;
    }
}

int MeaResult::GetAllValue(const int item, double value[MEA_MULTI][MEA_TIMES], int obFetal)
{
    int item_tmp;
    int position;
    ResultMulti *ptrResultMulti = NULL;
    double (*ptrResultSingle)[MEA_TIMES] = NULL;
    int sign;
    int multiVolume;
    int section;
    int i, j;
    sign = ItemLabel(item, &item_tmp, &ptrResultMulti, &ptrResultSingle, NULL, &multiVolume, &section, obFetal);
    if (sign == MEA_FAIL)
        return sign;

    if (((int)(*(ptrResultSingle + item_tmp))[MEA_TIMES - 1] != INVALID_VAL) || ((int)(*(ptrResultSingle + item_tmp))[0] == MULTI_SIGN ))//两个测量值的或单个的有数据
    {
        if ( (int)(*(ptrResultSingle + item_tmp))[0] == MULTI_SIGN )//多个测量值的
        {
            position = (int)(*(ptrResultSingle + item_tmp))[1];
            if ((position == MULTI_SIGN)||(position > multiVolume))
                return MEA_ERROR;

            if ((int)(*(ptrResultMulti + position)).result[MEA_TIMES - 1] == INVALID_VAL || (int)(*(ptrResultMulti + position)).result[MEA_TIMES - 1] == double(0))
                return MEA_FAIL;

            for(i=0; i<MEA_MULTI; i++)
            {
                for (j=0; j<MEA_TIMES; j++)
                {
                    value[i][j] = (*(ptrResultMulti + position)).result[MEA_TIMES * i + j];
                }
            }
            return MEA_SUCCESS;
        }
        else//单个测量值的
        {
            for(i=0; i<MEA_MULTI; i++)
            {
                for (j=0; j<MEA_TIMES; j++)
                {
                    if (i==0)
                        value[0][j] = (*(ptrResultSingle + item_tmp))[j];
                    else
                        value[i][j] = INVALID_VAL;
                }
            }
            return MEA_SUCCESS;
        }
    }
    else
    {
        return MEA_FAIL;
    }
}

int MeaResult::GetMeanValue(const int item, double value[MEA_MULTI], int obFetal)
{
    int item_tmp;
    int position;
    ResultMulti *ptrResultMulti = NULL;
    double (*ptrResultSingle)[MEA_TIMES] = NULL;
    int sign=0;
    int multiVolume;
    int section;
    int counter = 0;
    int i, j;

    sign = ItemLabel(item, &item_tmp, &ptrResultMulti, &ptrResultSingle, NULL, &multiVolume, &section, obFetal);
    if (sign == MEA_FAIL)
        return sign;

    for (i=0; i<MEA_MULTI; i++)
    {
        value[i] = 0;
    }

    if( ((int)(*(ptrResultSingle + item_tmp))[MEA_TIMES - 1] != INVALID_VAL) || ((int)(*(ptrResultSingle + item_tmp))[0] == MULTI_SIGN) )//两个测量值的或单个的有数据
    {
        if ( (int)(*(ptrResultSingle + item_tmp))[0] == MULTI_SIGN )//多个测量值的
        {
            position = (*(ptrResultSingle + item_tmp))[1];
            if ((position == MULTI_SIGN)||(position > multiVolume))
                return MEA_ERROR;

            if ((int)(*(ptrResultMulti + position)).result[MEA_TIMES - 1] == INVALID_VAL)
                return MEA_FAIL;

            counter = 0;
            for(i=0; i<MEA_MULTI; i++)
            {
                for(j=0; j<MEA_TIMES; j++)
                {
                    if ((*(ptrResultMulti + position)).result[MEA_TIMES * (i+1) - 1 - j] != INVALID_VAL)
                    {
                        value[i] += (*(ptrResultMulti + position)).result[MEA_TIMES * (i+1) - 1 - j];
                        if (i==1) counter++;//第0个位置放计算的测量值（如RI），故测量值从第1个位置开始
                    }
                }
            }
            if (counter == 0) return MEA_FAIL;//两个的没有数据
            for(i=0; i<MEA_MULTI; i++) {value[i] /= counter;}
            return MEA_SUCCESS;
        }
        else
        {
            counter = 0;

            for(j=0; j<MEA_TIMES; j++)
            {
                if ((*(ptrResultSingle + item_tmp))[MEA_TIMES - 1 - j] != INVALID_VAL)
                {
                    value[0] += (*(ptrResultSingle + item_tmp))[MEA_TIMES - 1 - j];
                    counter++;
                }
                else
                    break;
            }

            if (counter == 0) return MEA_FAIL;//没有数据

            value[0] /= counter;
            for (i=1; i<MEA_MULTI; i++) value[i] = INVALID_VAL;
            return MEA_SUCCESS;
        }
    }
    else
        return MEA_FAIL;
}

/*
 *item: 测量项的枚举号
 *sysValue: 0－返回用户设定的值（平均值或最后值），1－指定返回最后值, 2－获取所有值, 3-获取平均值
 *value:用来装平均值或最后值，如果要求返回所有值，则设为NULL
 *allValue:用来装所有值，如果要求返回平均值或最后值，则设为NULL
 */
int MeaResult::GetValue(const int item, double value[MEA_MULTI], double allValue[MEA_MULTI][MEA_TIMES], const int sysValue, int obFetal)
{
    if (sysValue == IN_REPORT_MEAN)
    {
        SysMeasurementSetting sys;
        int method = sys.GetReportResult();//获取报告中是使用平均值还是最后值

        if (method == 0)//最后值
            return(GetLastValue(item, value, obFetal));
        else//平均值
            return(GetMeanValue(item, value, obFetal));
    }
    else if (sysValue == IN_MEASURE)
    {
        return(GetLastValue(item, value, obFetal));
    }
    else if (sysValue == IN_REPORT_ALL)
    {
        return(GetAllValue(item, allValue, obFetal));
    }
    else if (sysValue == IN_AVERAGE)
    {
        return(GetMeanValue(item, value, obFetal));
    }
    return MEA_ERROR;
}

SecSingleResult MeaResult::GetSecSingleValue(int section, int obFetal)
{
    if (section == OB_M)
    {
        unsigned int fetalOrder = 0;
        if (obFetal == FETAL_BY_SET)
            fetalOrder = g_menuCalc.GetCurrentFetal();
        else if (obFetal == FETAL1)
            fetalOrder = 0;
        else
            fetalOrder = 1;

        if (fetalOrder == 0)
            return (SecSingleResult)MeaResultOB1;
        else
            return (SecSingleResult)MeaResultOB2;
    }
    else if (section < SECTION_END)
        return SecResultInfo[section].ptrSingleResult;
    else
        return NULL;
}

SecMultiResult MeaResult::GetSecMultiValue(int section, int obFetal)
{
    if (section == OB_M)
    {
        unsigned int fetalOrder = 0;
        if (obFetal == FETAL_BY_SET)
            fetalOrder = g_menuCalc.GetCurrentFetal();
        else if (obFetal == FETAL1)
            fetalOrder = 0;
        else
            fetalOrder = 1;

        if (fetalOrder == 0)
            return (SecMultiResult)(MeaResultOB1);
        else
            return (SecMultiResult)(MeaResultOB2);
    }
    else if (section < SECTION_END)
        return SecResultInfo[section].ptrMultiResult;
    else
        return NULL;
}

SecCalcResult MeaResult::GetSecCalcValue(int section, int obFetal)
{
    if (section == OB_M)
    {
        unsigned int fetalOrder = 0;
        if (obFetal == FETAL_BY_SET)
            fetalOrder = g_menuCalc.GetCurrentFetal();
        else if (obFetal == FETAL1)
            fetalOrder = 0;
        else
            fetalOrder = 1;

        if (fetalOrder == 0)
            return (SecCalcResult)(MeaResultOB1);
        else
            return (SecCalcResult)(MeaResultOB2);
    }
    else if (section < SECTION_END)
        return SecResultInfo[section].ptrCalcResult;
    else
        return NULL;
}

int MeaResult::CalcSetValue(const double value[CALC_RESULT_CLASSES], const int calcItem, const int section)
{
    if (section == OB_M)
    {
        if ((calcItem<OB_CALC_START)||(calcItem>OB_CALC_END)) return MEA_ERROR;
        unsigned int fetalOrder = 0;
        fetalOrder = g_menuCalc.GetCurrentFetal();
        if (fetalOrder == 0)
        {
            CalcResultOB1[calcItem - OB_CALC_START][0] = value[0];
            CalcResultOB1[calcItem - OB_CALC_START][1] = value[1];
        }
        else
        {
            CalcResultOB2[calcItem - OB_CALC_START][0] = value[0];
            CalcResultOB2[calcItem - OB_CALC_START][1] = value[1];
        }
        return MEA_SUCCESS;
     }

    //hlx
#ifdef VET
    else if (section == ANOB_M)
    {
        if ((calcItem<ANOB_CALC_START)||(calcItem>ANOB_CALC_END)) return MEA_ERROR;
        CalcResultAnOB[calcItem - ANOB_CALC_START][0] = value[0];
        CalcResultAnOB[calcItem - ANOB_CALC_START][1] = value[1];
        return MEA_SUCCESS;
    }
#endif
    else if (section < SECTION_END)
    {
        if (SecResultInfo[section].ptrCalcResult != NULL)
        {
            (*(SecResultInfo[section].ptrCalcResult))[calcItem - SecResultInfo[section].calcStart][0] = value[0];
            (*(SecResultInfo[section].ptrCalcResult))[calcItem - SecResultInfo[section].calcStart][1] = value[1];
        }
        else
            return MEA_ERROR;
    }
    else
        return MEA_ERROR;

    return MEA_SUCCESS;
}

void MeaResult::CalcSetAgwValue(const double value[][CALC_RESULT_CLASSES])
{
//修改 动物超 产科
//hlx
#ifdef VET
    CalcResultAnOBAgw[0][0] = value[0][0];
    CalcResultAnOBAgw[0][1] = value[0][1];
    CalcResultAnOBAgw[1][0] = value[1][0];
    CalcResultAnOBAgw[1][1] = value[1][1];
#else
    unsigned int fetalOrder = 0;
    fetalOrder = g_menuCalc.GetCurrentFetal();
    if (fetalOrder == 0)
    {
        CalcResultOB1Agw[0][0] = value[0][0];
        CalcResultOB1Agw[0][1] = value[0][1];
        CalcResultOB1Agw[1][0] = value[1][0];
        CalcResultOB1Agw[1][1] = value[1][1];
    }
    else
    {
        CalcResultOB2Agw[0][0] = value[0][0];
        CalcResultOB2Agw[0][1] = value[0][1];
        CalcResultOB2Agw[1][0] = value[1][0];
        CalcResultOB2Agw[1][1] = value[1][1];
    }
#endif
    return;
}

int MeaResult::CalcGetLastValue(double *value, const int calcItem, const int section, int obFetal)
{
    if (section == OB_M)
    {
        if ((calcItem<OB_CALC_START)||(calcItem>OB_CALC_END)) return MEA_ERROR;
        unsigned int fetalOrder = 0;
        if (obFetal == FETAL_BY_SET)
            fetalOrder = g_menuCalc.GetCurrentFetal();
        else if (obFetal == FETAL1)
            fetalOrder = 0;
        else
            fetalOrder = 1;
        if (fetalOrder == 0)
        {
            if (((int)CalcResultOB1[calcItem - OB_CALC_START][0] == INVALID_VAL)||((int)CalcResultOB1[calcItem - OB_CALC_START][0] == INVALID_VAL_GW)) return MEA_FAIL;
            else *value = CalcResultOB1[calcItem - OB_CALC_START][0];
        }
        else
        {
            if (((int)CalcResultOB2[calcItem - OB_CALC_START][0] == INVALID_VAL)||((int)CalcResultOB2[calcItem - OB_CALC_START][0] == INVALID_VAL_GW)) return MEA_FAIL;
            else *value = CalcResultOB2[calcItem - OB_CALC_START][0];
        }
    }

    //hlx
#ifdef VET
    else if (section == ANOB_M)
    {
        if ((calcItem<ANOB_CALC_START)||(calcItem>ANOB_CALC_END)) return MEA_ERROR;
        if (((int)CalcResultAnOB[calcItem - ANOB_CALC_START][0] == INVALID_VAL)||((int)CalcResultAnOB[calcItem - ANOB_CALC_START][0] == INVALID_VAL_GW)) return MEA_FAIL;
        else *value = CalcResultAnOB[calcItem - ANOB_CALC_START][0];
    }
#endif
    else
    {
        if (section >= SECTION_END) return MEA_ERROR;
        if ((calcItem<SecResultInfo[section].calcStart)||(calcItem>SecResultInfo[section].calcEnd)) return MEA_ERROR;
        if (((int)(*(SecResultInfo[section].ptrCalcResult))[calcItem - SecResultInfo[section].calcStart][0] == INVALID_VAL)||((int)(*(SecResultInfo[section].ptrCalcResult))[calcItem - SecResultInfo[section].calcStart][0] == INVALID_VAL_GW)) return MEA_FAIL;
        else    *value = (*(SecResultInfo[section].ptrCalcResult))[calcItem - SecResultInfo[section].calcStart][0];
    }
    return MEA_SUCCESS;
}

int MeaResult::CalcGetMeanValue(double *value, const int calcItem, const int section, int obFetal)
{
    if (section == OB_M)
    {
        if ((calcItem<OB_CALC_START)||(calcItem>OB_CALC_END)) return MEA_ERROR;
        unsigned int fetalOrder = 0;
        if (obFetal == FETAL_BY_SET)
            fetalOrder = g_menuCalc.GetCurrentFetal();
        else if (obFetal == FETAL1)
            fetalOrder = 0;
        else
            fetalOrder = 1;

        if (fetalOrder == 0)
        {
            if (((int)CalcResultOB1[calcItem - OB_CALC_START][1] == INVALID_VAL)||((int)CalcResultOB1[calcItem - OB_CALC_START][1] == INVALID_VAL_GW)) return MEA_FAIL;
            else *value = CalcResultOB1[calcItem - OB_CALC_START][1];
        }
        else
        {
            if (((int)CalcResultOB2[calcItem - OB_CALC_START][1] == INVALID_VAL)||((int)CalcResultOB2[calcItem - OB_CALC_START][1] == INVALID_VAL_GW)) return MEA_FAIL;
            else *value = CalcResultOB2[calcItem - OB_CALC_START][1];
        }
    }
    //hlx
#ifdef VET
    else if (section == ANOB_M)
    {
        if ((calcItem<ANOB_CALC_START)||(calcItem>ANOB_CALC_END)) return MEA_ERROR;
        if (((int)CalcResultAnOB[calcItem - ANOB_CALC_START][1] == INVALID_VAL)||((int)CalcResultAnOB[calcItem - ANOB_CALC_START][1] == INVALID_VAL_GW)) return MEA_FAIL;
        else *value = CalcResultAnOB[calcItem - ANOB_CALC_START][1];
    }
#endif
    else
    {
        if (section >= SECTION_END) return MEA_ERROR;
        if ((calcItem<SecResultInfo[section].calcStart)||(calcItem>SecResultInfo[section].calcEnd)) return MEA_ERROR;
        if (((int)(*(SecResultInfo[section].ptrCalcResult))[calcItem - SecResultInfo[section].calcStart][1] == INVALID_VAL)||((int)(*(SecResultInfo[section].ptrCalcResult))[calcItem - SecResultInfo[section].calcStart][1] == INVALID_VAL_GW)) return MEA_FAIL;
        else    *value = (*(SecResultInfo[section].ptrCalcResult))[calcItem - SecResultInfo[section].calcStart][1];
    }
    return MEA_SUCCESS;
}

int MeaResult::CalcGetValue(double *value, const int calcItem, const int section, int obFetal)
{
    SysMeasurementSetting sys;
    int method = sys.GetReportResult();//获取报告中是使用平均值还是最后值

    if (method == 0)//最后值
        return(CalcGetLastValue(value, calcItem, section, obFetal));
    else//平均值
        return(CalcGetMeanValue(value, calcItem, section, obFetal));
}

int MeaResult::CalcGetAgwLastValue(double value[], const int fetalOrder)
{
    //修改 动物超 产科
    //hlx
#ifdef VET
    if ((int)CalcResultAnOBAgw[0][0] == INVALID_VAL)
        return MEA_FAIL;
    else
    {
        value[0] = CalcResultAnOBAgw[0][0];
        value[1] = CalcResultAnOBAgw[1][0];
    }
#else
    if (fetalOrder == 0)
    {
        if ((int)CalcResultOB1Agw[0][0] == INVALID_VAL)
            return MEA_FAIL;
        else
        {
            value[0] = CalcResultOB1Agw[0][0];
            value[1] = CalcResultOB1Agw[1][0];
        }
    }
    else
    {
        if ((int)CalcResultOB2Agw[0][0] == INVALID_VAL)
            return MEA_FAIL;
        else
        {
            value[0] = CalcResultOB2Agw[0][0];
            value[1] = CalcResultOB2Agw[1][0];
        }
    }
#endif
    return MEA_SUCCESS;
}

int MeaResult::CalcGetAgwMeanValue(double value[], const int fetalOrder)
{
    //修改 动物超 产科

#ifdef VET
    if ((int)CalcResultAnOBAgw[0][1] == INVALID_VAL)
        return MEA_FAIL;
    else
    {
        value[0] = CalcResultAnOBAgw[0][1];
        value[1] = CalcResultAnOBAgw[1][1];
    }
#else
    if (fetalOrder == 0)
    {
        if ((int)CalcResultOB1Agw[0][1] == INVALID_VAL)
            return MEA_FAIL;
        else
        {
            value[0] = CalcResultOB1Agw[0][1];
            value[1] = CalcResultOB1Agw[1][1];
        }
    }
    else
    {
        if ((int)CalcResultOB2Agw[0][1] == INVALID_VAL)
            return MEA_FAIL;
        else
        {
            value[0] = CalcResultOB2Agw[0][1];
            value[1] = CalcResultOB2Agw[1][1];
        }
    }
#endif
    return MEA_SUCCESS;
}

int MeaResult::CalcGetAgwValue(double value[], const int fetalOrder)
{
    SysMeasurementSetting sys;
    int method = sys.GetReportResult();//获取报告中是使用平均值还是最后值

    if (method == 0)//最后值
        return(CalcGetAgwLastValue(value, fetalOrder));
    else//平均值
        return(CalcGetAgwMeanValue(value, fetalOrder));
}

//添加 动物超 肌腱 获取严重度最后值
//hlx 12.21
#if 0
int MeaResult::CalcGetTDLastValue(float *value)
{
    int ret = 0;
    int i = 0;
    float val[MEA_MULTI];
    float totalarea = 0;
    float affectarea = 0;

    //Lesion1 Area
    ret = GetLastValue(TD_LES1_AREA, val, 1);
    if (ret == MEA_FAIL)
    {
        *value = (float)INVALID_VAL;
        return MEA_FAIL;
    }
    totalarea += val[0];

    ret = GetLastValue(TD_LES1_AREA1, val, 1);
    if (ret == MEA_FAIL)
    {
        *value = (float)INVALID_VAL;
        return MEA_FAIL;
    }
    affectarea += val[0];

    for (i=0; i<TD_MEA_END; i++)
    {
        ret = GetLastValue(TD_LES1_AREA2+i, val, 1);
        if (ret == MEA_FAIL)
        {
            affectarea += 0;
            totalarea += 0;
            continue;
        }
        if (i == TD_LES2_AREA || i == TD_LES3_AREA || i == TD_LES4_AREA || i == TD_LES5_AREA ||
                i == TD_LES6_AREA || i == TD_LES7_AREA || i == TD_LES8_AREA || i == TD_LES9_AREA ||
                i == TD_LES10_AREA || i == TD_LES11_AREA)
        {
            totalarea += val[0];
        }
        else
        {
            affectarea += val[0];
        }
    }

    *value = 100 * affectarea / totalarea;

    return MEA_SUCCESS;
}
#endif
#ifdef VET
int MeaResult::CalcGetTDLastValue(double *total_area, double *affect_area, double *value)
{
    int ret = 0;
    int i = 0;
    double val[MEA_MULTI];
    double totalarea = 0;
    double affectarea = 0;

    for (i=TD_MEA_START; i<TD_MEA_END; i++)
    {
        ret = GetLastValue(i, val, 1);
        if (ret == MEA_FAIL)
        {
            affectarea += 0;
            totalarea += 0;
            continue;
        }

        if (i == TD_LES1_AREA || i == TD_LES2_AREA || i == TD_LES3_AREA || i == TD_LES4_AREA || i == TD_LES5_AREA ||
                i == TD_LES6_AREA || i == TD_LES7_AREA || i == TD_LES8_AREA || i == TD_LES9_AREA ||
                i == TD_LES10_AREA || i == TD_LES11_AREA)
        {
            totalarea += val[0];
        }
        else
        {
            affectarea += val[0];
        }
    }
        if (totalarea == 0 || affectarea == 0)
        {
            *value = (double)INVALID_VAL;
            *total_area = (double)INVALID_VAL;
            *affect_area = (double)INVALID_VAL;
            return MEA_FAIL;
        }

    *value = 100 * affectarea / totalarea;
        *total_area = totalarea;
        *affect_area = affectarea;

    return MEA_SUCCESS;
}

//添加 动物超 肌腱  获取严重度的平均值

#if 0
int MeaResult::CalcGetTDMeanValue(float *value)
{
    int ret = 0;
    int i = 0;
    float val[MEA_MULTI];
    float totalarea = 0;
    float affectarea = 0;

    //Lesion1 Area
    ret = GetMeanValue(TD_LES1_AREA, val, 1);
    if (ret == MEA_FAIL)
    {
        *value = (float)INVALID_VAL;
        return MEA_FAIL;
    }
    totalarea += val[0];

    ret = GetMeanValue(TD_LES1_AREA1, val, 1);
    if (ret == MEA_FAIL)
    {
        *value = (float)INVALID_VAL;
        return MEA_FAIL;
    }
    affectarea += val[0];

    for (i=TD_LES1_AREA2; i<TD_MEA_END; i++)
    {
        ret = GetMeanValue(TD_LES1_AREA2, val, 1);
        if (ret == MEA_FAIL)
        {
            affectarea += 0;
            totalarea += 0;
            continue;
        }
        if (i == TD_LES2_AREA || i == TD_LES3_AREA || i == TD_LES4_AREA || i == TD_LES5_AREA ||
                i == TD_LES6_AREA || i == TD_LES7_AREA || i == TD_LES8_AREA || i == TD_LES9_AREA ||
                i == TD_LES10_AREA || i == TD_LES11_AREA)
        {
            totalarea += val[0];
        }
        else
        {
            affectarea += val[0];
        }
    }

    *value = 100 * affectarea / totalarea;

    return MEA_SUCCESS;
}
#endif
int MeaResult::CalcGetTDMeanValue(double *total_area, double *affect_area, double *value)
{
    int ret = 0;
    int i = 0;
    double val[MEA_MULTI];
    double totalarea = 0;
    double affectarea = 0;

    for (i=TD_MEA_START; i<TD_MEA_END; i++)
    {
        ret = GetMeanValue(i, val, 1);
        if (ret == MEA_FAIL)
        {
            affectarea += 0;
            totalarea += 0;
            continue;
        }
        if (i == TD_LES1_AREA || i == TD_LES2_AREA || i == TD_LES3_AREA || i == TD_LES4_AREA || i == TD_LES5_AREA ||
                i == TD_LES6_AREA || i == TD_LES7_AREA || i == TD_LES8_AREA || i == TD_LES9_AREA ||
                i == TD_LES10_AREA || i == TD_LES11_AREA)
        {
            totalarea += val[0];
        }
        else
        {
            affectarea += val[0];
        }
    }
        if (totalarea == 0 || affectarea == 0)
        {
            *value = (double)INVALID_VAL;
            *total_area = (double)INVALID_VAL;
            *affect_area = (double)INVALID_VAL;
            return MEA_FAIL;
        }

    *value = 100 * affectarea / totalarea;
        *total_area = totalarea;
        *affect_area = affectarea;

    return MEA_SUCCESS;
}

//添加 动物超 肌腱 获取严重度值
//hlx 12.21
#if 0
int MeaResult::CalcGetTDValue(float *value)
{
    SysMeasurementSetting sys;
    int method = sys.GetReportResult();

    if (method == 0)
        return CalcGetTDLastValue(value);
    else
        return CalcGetTDMeanValue(value);
}
#endif
int MeaResult::CalcGetTDValue(double *total, double *affect, double *value)
{
    SysMeasurementSetting sys;
    int method = sys.GetReportResult();

    if (method == 0)
        return CalcGetTDLastValue(total, affect, value);
    else
        return CalcGetTDMeanValue(total, affect, value);
}
#endif
void MeaResult::CalcSetSPSA(double spsa)
{
    CalcResultURSPSA[0] = spsa;
}

void MeaResult::CalcSetPSAD(double psad)
{
    CalcResultURSPSA[1] = psad;
}

void MeaResult::CalcGetSPSA(double &spsa, double &psad)
{
    spsa = CalcResultURSPSA[0];
    if(spsa == INVALID_VAL)
    {
        PatientInfo::Info info;
        g_patientInfo.GetInfo(info);
        if(info.uro.PSA > 0)
        {
            spsa = info.uro.PSA;
            CalcResultURSPSA[0] = spsa;
        }
    }
    psad = CalcResultURSPSA[1];
}

void MeaResult::ClearAllValue(void)
{
    AbdInit();
    AdultInit();
    URInit();
    OBInit();
    GYNInit();
    SPInit();
    VSInit();
    FetalInit();
    TCDInit();
    OrthoInit();
    UserInit();
#ifdef VET
//添加 动物超 肌腱
//hlx
    TDInit();
//hlx
    AnOBInit();
#endif
    //add by jhuang begin
    ViewReport *ptrRes = ViewReport::GetInstance();
    ptrRes->DefaultCalcArr();
    //add by jhuang end
}

/*
//判断一个科别是否为以多项测量的测量项为主的科别
int MeaResult::JudgeMultiSec(int section)
{
    if ((section == VS_M) || (section == TCD_M) || (section == ORTHO_M))
        return 1;
    else
        return 0;
}
*/

/*
 *输入：
 * item: 传入的single或multi的item
 * obFetal: 传入是胎儿1还是胎儿2
 *
 *输出：
 * item_tmp: 返回item与本科别起始位置的距离，为item - itemStart
 * ptrResultMulti: 返回本科别multi结果的存储起始位置
 * ptrResultSingle：返回本科别single结果的存储起始位置
 * singleVolume: 返回本科别single项目的多少
 * multiVolume: 返回本科别multi项目的多少
 * section: 返回本科别的枚举号
 * 返回值: 执行是否成功
 */
int MeaResult::ItemLabel(const int item, int *item_tmp, ResultMulti **ptrResultMulti, double (**ptrResultSingle)[MEA_TIMES], int *singleVolume, int *multiVolume, int *section, int obFetal)
{
    int i;
    int singleMulti = 0;//Single或Multi: 0-Single, 1-Multi

    for (i=0; i<SECTION_END-SECTION_START; i++)
    {
        if ((item >= SecResultInfo[i].singleStart) && (item < SecResultInfo[i].singleEnd))
        {
            *section = SecResultInfo[i].secOrder;
            if (item_tmp != NULL)
                *item_tmp = item - SecResultInfo[*section].singleStart;
            singleMulti = 0;
            break;
        }

        if ((item >= SecResultInfo[i].multiStart) && (item < SecResultInfo[i].multiEnd))
        {
            *section = SecResultInfo[i].secOrder;
            if (item_tmp != NULL)
                *item_tmp = item - SecResultInfo[*section].multiStart;
            singleMulti = 1;
            break;
        }
    }
    if (i == SECTION_END - SECTION_START)
        return MEA_FAIL;

//  if (item_tmp != NULL)
//      *item_tmp = item - SecResultInfo[*section].genStart;

    if (*section != OB_M)
    {
        if (ptrResultSingle != NULL)
            *ptrResultSingle = *(SecResultInfo[*section].ptrSingleResult);
        if (ptrResultMulti != NULL)
            *ptrResultMulti = *(SecResultInfo[*section].ptrMultiResult);
        if (singleVolume != NULL)
            *singleVolume = SecResultInfo[*section].singleEnd - SecResultInfo[*section].singleStart;
        if (multiVolume != NULL)
            *multiVolume = SecResultInfo[*section].multiEnd - SecResultInfo[*section].multiStart;
    }
    else
    {
        unsigned int fetalOrder = 0;
        if (obFetal == FETAL_BY_SET)
            fetalOrder = g_menuCalc.GetCurrentFetal();
        else if (obFetal == FETAL1)
            fetalOrder = 0;
        else
            fetalOrder = 1;

        if (fetalOrder == 0)
        {
            if (ptrResultSingle != NULL)
                *ptrResultSingle = *(SecResultInfo[*section].ptrSingleResult);
            if (ptrResultMulti != NULL)
                *ptrResultMulti = *(SecResultInfo[*section].ptrMultiResult);
            if (singleVolume != NULL)
                *singleVolume = SecResultInfo[*section].singleEnd - SecResultInfo[*section].singleStart;
            if (multiVolume != NULL)
                *multiVolume = SecResultInfo[*section].multiEnd - SecResultInfo[*section].multiStart;
        }
        else
        {
            if (ptrResultSingle != NULL)
                *ptrResultSingle = MeaResultOB2;
            if (ptrResultMulti != NULL)
                *ptrResultMulti = OBResultMulti2;
            if (singleVolume != NULL)
                *singleVolume = SecResultInfo[*section].singleEnd - SecResultInfo[*section].singleStart;
            if (multiVolume != NULL)
                *multiVolume = SecResultInfo[*section].multiEnd - SecResultInfo[*section].multiStart;
        }
    }
    return MEA_SUCCESS;
}

#if 0
int MeaResult::ItemLabelSingle(const int item, int *item_tmp, float (**ptrResultSingle)[MEA_TIMES], int *singleVolume, int *section, int obFetal)
{
    int i;

    for (i=0; i<SECTION_END-SECTION_START; i++)
    {
        if ((item >= SecResultInfo[i].singleStart) && (item < SecResultInfo[i].singleEnd))
        {
            *section = SecResultInfo[i].secOrder;
            break;
        }
    }
    if (i == SECTION_END - SECTION_START)
        return MEA_FAIL;

    if (*section != OB_M)
    {
        *item_tmp = item - SecResultInfo[*section].singleStart;
        *ptrResultSingle = *(SecResultInfo[*section].ptrSingleResult);
        *singleVolume = SecResultInfo[*section].singleEnd - SecResultInfo[*section].singleStart;
    }
    else
    {
        unsigned int fetalOrder = 0;
        if (obFetal == FETAL_BY_SET)
            fetalOrder = g_menuCalc.GetCurrentFetal();
        else if (obFetal == FETAL1)
            fetalOrder = 0;
        else
            fetalOrder = 1;

        if (fetalOrder == 0)
        {
            *item_tmp = item - SecResultInfo[*section].singleStart;
            *ptrResultSingle = *(SecResultInfo[*section].ptrSingleResult);
            *singleVolume = SecResultInfo[*section].singleEnd - SecResultInfo[*section].singleStart;
        }
        else
        {
            *item_tmp = item - SecResultInfo[*section].singleStart;
            *ptrResultSingle = MeaResultOB2;
            *singleVolume = SecResultInfo[*section].singleEnd - SecResultInfo[*section].singleStart;
        }

    }
    return MEA_SUCCESS;
}

//int ItemLabel(int item, ResultMulti **ptrResultMulti, float *(*ptrResultSingle)[MEA_TIMES], int *volume)
int MeaResult::ItemLabelMulti(const int item, int *item_tmp, ResultMulti **ptrResultMulti, int *volume, int *section, int obFetal)
{
    int i;

    for (i=0; i<SECTION_END-SECTION_START; i++)
    {
        if ((item >= SecResultInfo[i].multiStart) && (item < SecResultInfo[i].multiEnd))
        {
            *section = SecResultInfo[i].secOrder;
            break;
        }
    }
    if (i == SECTION_END - SECTION_START)
        return MEA_FAIL;

    if (*section != OB_M)
    {
        *item_tmp = item - SecResultInfo[i].multiStart;
        *ptrResultMulti = *(SecResultInfo[*section].ptrMultiResult);
        *volume = SecResultInfo[i].multiEnd - SecResultInfo[i].multiStart;
    }
    else
    {
        unsigned int fetalOrder = 0;
        if (obFetal == FETAL_BY_SET)
            fetalOrder = g_menuCalc.GetCurrentFetal();
        else if (obFetal == FETAL1)
            fetalOrder = 0;
        else
            fetalOrder = 1;
        if (fetalOrder == 0)
        {
            *item_tmp = item - SecResultInfo[i].multiStart;
            *ptrResultMulti = *(SecResultInfo[*section].ptrMultiResult);
            *volume = SecResultInfo[i].multiEnd - SecResultInfo[i].multiStart;
        }
        else
        {
            *item_tmp = item - SecResultInfo[i].multiStart;
            *ptrResultMulti = OBResultMulti2;
            *volume = SecResultInfo[i].multiEnd - SecResultInfo[i].multiStart;
        }
    }
    return MEA_SUCCESS;
}
#endif

int MeaResult::ItemLabelCalc(const int item, int *section)
{
    int i;

    for (i=0; i<SECTION_END-SECTION_START; i++)
    {
        if ((item >= SecResultInfo[i].calcStart) && (item < SecResultInfo[i].calcEnd))
        {
            *section = SecResultInfo[i].secOrder;
            break;
        }
    }
    if (i == SECTION_END - SECTION_START)
        return MEA_FAIL;

    return MEA_SUCCESS;
}

void MeaResult::InitSign(double singleResult[MEA_TIMES], const int position)
{
    singleResult[0] = MULTI_SIGN;
    singleResult[1] = (double)position;
    singleResult[2] = MULTI_SIGN;
}

void MeaResult::AbdInit(void)
{
    int i, j;

    for (j=0; j<MEA_TIMES; j++)
    {
        for (i=0; i<ABD_MEA_END - ABD_MEA_START; i++)
        {
            MeaResultAbd[i][j] = (double)INVALID_VAL;
        }
    }

    for (j=0; j<MEA_TIMES_MULTI; j++)
    {
        for (i=0; i<ABD_MULTI_END - ABD_MULTI_START; i++)
        {
            AbdResultMulti[i].result[j] = (double)INVALID_VAL;
        }
    }

    for (i=0; i<ABD_CALC_END - ABD_CALC_START; i++)
    {
        CalcResultAbd[i][0] = (double)INVALID_VAL;
        CalcResultAbd[i][1] = (double)INVALID_VAL;
    }

    int offset;
    for (i=0; i<ABD_MULTI_END - ABD_MULTI_START; i++)
    {
        if ( AbdoMultiInfo[i].multiItem == i + ABD_MULTI_START )
            InitSign(MeaResultAbd[AbdoMultiInfo[i].item-ABD_MEA_START], i);
        else
        {
            offset = FindDoubleMeaInfo(ABDO_M, i + ABD_MULTI_START);
            if (offset == -1)
            {
                PRINTF("There is error in AbdoMultiInfo!\n");
            }
            else
                InitSign(MeaResultAbd[AbdoMultiInfo[offset].item-ABD_MEA_START], i);
        }
    }
}

void MeaResult::AdultInit(void)
{
    int i, j;

    for (j=0; j<MEA_TIMES; j++)
    {
        for (i=0; i<ADULT_MEA_END - ADULT_MEA_START; i++)
        {
            MeaResultAdult[i][j] = (double)INVALID_VAL;
        }
    }

    for (j=0; j<MEA_TIMES_MULTI; j++)
    {
        for (i=0; i<ADULT_MULTI_END - ADULT_MULTI_START; i++)
        {
            AdultResultMulti[i].result[j] = (double)INVALID_VAL;
        }
    }

    for (i=0; i<ADULT_CALC_END - ADULT_CALC_START; i++)
    {
        CalcResultAdult[i][0] = (double)INVALID_VAL;
        CalcResultAdult[i][1] = (double)INVALID_VAL;
    }
    int offset;
    for (i=0; i<ADULT_MULTI_END - ADULT_MULTI_START; i++)
    {
        if ( AdultMultiInfo[i].multiItem == i + ADULT_MULTI_START)
            InitSign(MeaResultAdult[AdultMultiInfo[i].item-ADULT_MEA_START], i);
        else
        {
            offset = FindDoubleMeaInfo(ADULT_M, i + ADULT_MULTI_START);
            if (offset == -1)
            {
                PRINTF("There is error in AdultMultiInfo!\n");
            }
            else
                InitSign(MeaResultAdult[AdultMultiInfo[offset].item-ADULT_MEA_START], i);
        }
    }
}

void MeaResult::URInit(void)
{
    int i, j;
    int offset;

    for (j=0; j<MEA_TIMES; j++)
    {
        for (i=0; i<UR_MEA_END - UR_MEA_START; i++)
        {
            MeaResultUR[i][j] = (double)INVALID_VAL;
        }
    }

    for (j=0; j<MEA_TIMES_MULTI; j++)
    {
        for (i=0; i<UR_MULTI_END - UR_MULTI_START; i++)
        {
            URResultMulti[i].result[j] = (double)INVALID_VAL;
        }
    }

    for (i=0; i<UR_CALC_END - UR_CALC_START; i++)
    {
        CalcResultUR[i][0] = (double)INVALID_VAL;
        CalcResultUR[i][1] = (double)INVALID_VAL;
    }
    for (i=0; i<UR_MULTI_END - UR_MULTI_START; i++)
    {
        if (URMultiInfo[i].multiItem == i + UR_MULTI_START)
            InitSign(MeaResultUR[URMultiInfo[i].item-UR_MEA_START], i);
        else
        {
            offset = FindDoubleMeaInfo(UR_M, i + UR_MULTI_START);
            if (offset == -1)
            {
                PRINTF("There is error in URMultiInfo!\n");
            }
            else
                InitSign(MeaResultUR[URMultiInfo[offset].item-UR_MEA_START], i);
        }
    }

    CalcResultURSPSA[0] = (double)INVALID_VAL;
    CalcResultURSPSA[1] = (double)INVALID_VAL;
}

void MeaResult::OBInit(void)
{
    int i, j;
    int offset;

    for (j=0; j<MEA_TIMES; j++)
    {
        for (i=0; i<OB_MEA_END - OB_MEA_START; i++)
        {
            MeaResultOB1[i][j] = (double)INVALID_VAL;
            MeaResultOB2[i][j] = (double)INVALID_VAL;
        }
    }

    for (j=0; j<MEA_TIMES_MULTI; j++)
    {
        for (i=0; i<OB_MULTI_END - OB_MULTI_START; i++)
        {
            OBResultMulti1[i].result[j] = (double)INVALID_VAL;
            OBResultMulti2[i].result[j] = (double)INVALID_VAL;
        }
    }

    for (i=0; i<OB_CALC_END - OB_CALC_START; i++)
    {
        CalcResultOB1[i][0] = (double)INVALID_VAL;
        CalcResultOB1[i][1] = (double)INVALID_VAL;
        CalcResultOB2[i][0] = (double)INVALID_VAL;
        CalcResultOB2[i][1] = (double)INVALID_VAL;
    }

    for (i=0; i<OB_MULTI_END - OB_MULTI_START; i++)
    {
        if ( OBMultiInfo[i].multiItem == i + OB_MULTI_START)
        {
            InitSign(MeaResultOB1[OBMultiInfo[i].item-OB_MEA_START], i);
            InitSign(MeaResultOB2[OBMultiInfo[i].item-OB_MEA_START], i);
        }
        else
        {
            offset = FindDoubleMeaInfo(OB_M, i + OB_MULTI_START);
            if (offset == -1)
            {
                PRINTF("There is error in OBMultiInfo!\n");
            }
            else
            {
                InitSign(MeaResultOB1[OBMultiInfo[offset].item-OB_MEA_START], i);
                InitSign(MeaResultOB2[OBMultiInfo[offset].item-OB_MEA_START], i);
            }
        }
    }

    CalcResultOB1Agw[0][0] = (double)INVALID_VAL;
    CalcResultOB1Agw[0][1] = (double)INVALID_VAL;
    CalcResultOB1Agw[1][0] = (double)INVALID_VAL;
    CalcResultOB1Agw[1][1] = (double)INVALID_VAL;
    CalcResultOB2Agw[0][0] = (double)INVALID_VAL;
    CalcResultOB2Agw[0][1] = (double)INVALID_VAL;
    CalcResultOB2Agw[1][0] = (double)INVALID_VAL;
    CalcResultOB2Agw[1][1] = (double)INVALID_VAL;
}

void MeaResult::GYNInit(void)
{
    int i, j;
    int offset;

    for (j=0; j<MEA_TIMES; j++)
    {
        for (i=0; i<GYN_MEA_END - GYN_MEA_START; i++)
        {
            MeaResultGYN[i][j] = (double)INVALID_VAL;
        }
    }

    for (j=0; j<MEA_TIMES_MULTI; j++)
    {
        for (i=0; i<GYN_MULTI_END - GYN_MULTI_START; i++)
        {
            GYNResultMulti[i].result[j] = (double)INVALID_VAL;
        }
    }

    for (i=0; i<GYN_CALC_END - GYN_CALC_START; i++)
    {
        CalcResultGYN[i][0] = (double)INVALID_VAL;
        CalcResultGYN[i][1] = (double)INVALID_VAL;
    }

    for (i=0; i<GYN_MULTI_END - GYN_MULTI_START; i++)
    {
        if ( GYNMultiInfo[i].multiItem == i + GYN_MULTI_START)
            InitSign(MeaResultGYN[GYNMultiInfo[i].item-GYN_MEA_START], i);
        else
        {
            offset = FindDoubleMeaInfo(GYN_M, i + GYN_MULTI_START);
            if (offset == -1)
            {
                PRINTF("There is error in GYNMultiInfo!\n");
            }
            else
                InitSign(MeaResultGYN[GYNMultiInfo[offset].item-GYN_MEA_START], i);
        }
    }
}

void MeaResult::SPInit(void)
{
    int i, j;
    int offset;

    for (j=0; j<MEA_TIMES; j++)
    {
        for (i=0; i<SP_MEA_END - SP_MEA_START; i++)
        {
            MeaResultSP[i][j] = (double)INVALID_VAL;
        }
    }

    for (j=0; j<MEA_TIMES_MULTI; j++)
    {
        for (i=0; i<SP_MULTI_END - SP_MULTI_START; i++)
        {
            SPResultMulti[i].result[j] = (double)INVALID_VAL;
        }
    }

    for (i=0; i<SP_CALC_END - SP_CALC_START; i++)
    {
        CalcResultSP[i][0] = (double)INVALID_VAL;
        CalcResultSP[i][1] = (double)INVALID_VAL;
    }

    for (i=0; i<SP_MULTI_END - SP_MULTI_START; i++)
    {
        if ( SPMultiInfo[i].multiItem == i + SP_MULTI_START)
            InitSign(MeaResultSP[SPMultiInfo[i].item-SP_MEA_START], i);
        else
        {
            offset = FindDoubleMeaInfo(SP_M, i + SP_MULTI_START);
            if (offset == -1)
            {
                PRINTF("There is error in SPMultiInfo!\n");
            }
            else
                InitSign(MeaResultSP[SPMultiInfo[offset].item-SP_MEA_START], i);
        }
    }
}
void MeaResult::VSInit(void)
{
    int i, j;
    int offset;

    for (j=0; j<MEA_TIMES; j++)
    {
        for (i=0; i<VS_MEA_END - VS_MEA_START; i++)
        {
            MeaResultVS[i][j] = (double)INVALID_VAL;
        }
    }

    for (j=0; j<MEA_TIMES_MULTI; j++)
    {
        for (i=0; i<VS_MULTI_END - VS_MULTI_START; i++)
        {
            VSResultMulti[i].result[j] = (double)INVALID_VAL;
        }
    }

    for (i=0; i<VS_CALC_END - VS_CALC_START; i++)
    {
        CalcResultVS[i][0] = (double)INVALID_VAL;
        CalcResultVS[i][1] = (double)INVALID_VAL;
    }

    for (i=0; i<VS_MULTI_END - VS_MULTI_START; i++)
    {
        if ( VSMultiInfo[i].multiItem == i + VS_MULTI_START)
            InitSign(MeaResultVS[VSMultiInfo[i].item-VS_MEA_START], i);
        else
        {
            offset = FindDoubleMeaInfo(VS_M, i + VS_MULTI_START);
            if (offset != -1)
                InitSign(MeaResultVS[VSMultiInfo[offset].item-VS_MEA_START], i);
        }
    }
}
void MeaResult::FetalInit(void)
{
    int i, j;
    int offset;

    for (j=0; j<MEA_TIMES; j++)
    {
        for (i=0; i<FETAL_MEA_END - FETAL_MEA_START; i++)
        {
            MeaResultFetal[i][j] = (double)INVALID_VAL;
        }
    }

    for (j=0; j<MEA_TIMES_MULTI; j++)
    {
        for (i=0; i<FETAL_MULTI_END - FETAL_MULTI_START; i++)
        {
            FetalResultMulti[i].result[j] = (double)INVALID_VAL;
        }
    }

    for (i=0; i<FETAL_CALC_END - FETAL_CALC_START; i++)
    {
        CalcResultFetal[i][0] = (double)INVALID_VAL;
        CalcResultFetal[i][1] = (double)INVALID_VAL;
    }

    for (i=0; i<FETAL_MULTI_END - FETAL_MULTI_START; i++)
    {
        if ( FetalMultiInfo[i].multiItem == i + FETAL_MULTI_START)
            InitSign(MeaResultFetal[FetalMultiInfo[i].item-FETAL_MEA_START], i);
        else
        {
            offset = FindDoubleMeaInfo(FETAL_M, i + FETAL_MULTI_START);
            if (offset == -1)
            {
                PRINTF("There is error in FetalMultiInfo!\n");
            }
            else
                InitSign(MeaResultFetal[FetalMultiInfo[offset].item-FETAL_MEA_START], i);
        }
    }
}
void MeaResult::TCDInit(void)
{
    int i, j;
    int offset;

    for (j=0; j<MEA_TIMES; j++)
    {
        for (i=0; i<TCD_MEA_END - TCD_MEA_START; i++)
        {
            MeaResultTCD[i][j] = (double)INVALID_VAL;
        }
    }

    for (j=0; j<MEA_TIMES_MULTI; j++)
    {
        for (i=0; i<TCD_MULTI_END - TCD_MULTI_START; i++)
        {
            TCDResultMulti[i].result[j] = (double)INVALID_VAL;
        }
    }

    for (i=0; i<TCD_CALC_END - TCD_CALC_START; i++)
    {
        CalcResultTCD[i][0] = (double)INVALID_VAL;
        CalcResultTCD[i][1] = (double)INVALID_VAL;
    }

    for (i=0; i<TCD_MULTI_END - TCD_MULTI_START; i++)
    {
        if ( TCDMultiInfo[i].multiItem == i + TCD_MULTI_START)
            InitSign(MeaResultTCD[TCDMultiInfo[i].item-TCD_MEA_START], i);
        else
        {
            offset = FindDoubleMeaInfo(TCD_M, i + TCD_MULTI_START);
            if (offset != -1)
                InitSign(MeaResultTCD[TCDMultiInfo[offset].item-TCD_MEA_START], i);
        }
    }

}
void MeaResult::OrthoInit(void)
{
    int i, j;
    int offset;

    for (j=0; j<MEA_TIMES; j++)
    {
        for (i=0; i<ORTHO_MEA_END - ORTHO_MEA_START; i++)
        {
            MeaResultOrtho[i][j] = (double)INVALID_VAL;
        }
    }

    for (j=0; j<MEA_TIMES_MULTI; j++)
    {
        for (i=0; i<ORTHO_MULTI_END - ORTHO_MULTI_START; i++)
        {
            OrthoResultMulti[i].result[j] = (double)INVALID_VAL;
        }
    }

    for (i=0; i<ORTHO_CALC_END - ORTHO_CALC_START; i++)
    {
        CalcResultOrtho[i][0] = (double)INVALID_VAL;
        CalcResultOrtho[i][1] = (double)INVALID_VAL;
    }

    for (i=0; i<ORTHO_MULTI_END - ORTHO_MULTI_START; i++)
    {
        if ( OrthoMultiInfo[i].multiItem == i + ORTHO_MULTI_START)
            InitSign(MeaResultOrtho[OrthoMultiInfo[i].item-ORTHO_MEA_START], i);
        else
        {
            offset = FindDoubleMeaInfo(ORTHO_M, i + ORTHO_MULTI_START);
            if (offset != -1)
                InitSign(MeaResultOrtho[OrthoMultiInfo[offset].item-ORTHO_MEA_START], i);
        }
    }
}
#ifdef VET
//添加 动物超 TD测量结果初始化
//hlx 12.21
void MeaResult::TDInit(void)
{
    int i, j;
    int offset;

    for (j=0; j<MEA_TIMES; j++)
    {
        for (i=0; i<TD_MEA_END - TD_MEA_START; i++)
        {
            MeaResultTD[i][j] = (double)INVALID_VAL;
        }
    }

    for (i=0; i<TD_CALC_END - TD_CALC_START; i++)
    {
        CalcResultTD[i][0] = (double)INVALID_VAL;
        CalcResultTD[i][1] = (double)INVALID_VAL;
    }

    for (i=0; i<TD_MULTI_END - TD_MULTI_START; i++)
    {
        if ( TDMultiInfo[i].multiItem == i + TD_MULTI_START)
            InitSign(MeaResultTD[TDMultiInfo[i].item-TD_MEA_START], i);
        else
        {
            offset = FindDoubleMeaInfo(TD_M, i + TD_MULTI_START);
            if (offset != -1)
                InitSign(MeaResultTD[TDMultiInfo[offset].item-TD_MEA_START], i);
        }
    }
}

//添加 动物超 产科
//hlx
void MeaResult::AnOBInit(void)
{
    int i, j;
    int offset;

    for (j=0; j<MEA_TIMES; j++)
    {
        for (i=0; i<ANOB_MEA_END - ANOB_MEA_START; i++)
        {
            MeaResultAnOB[i][j] = (double)INVALID_VAL;
        }
    }

    for (i=0; i<ANOB_CALC_END - ANOB_CALC_START; i++)
    {
        CalcResultAnOB[i][0] = (double)INVALID_VAL;
        CalcResultAnOB[i][1] = (double)INVALID_VAL;
    }

    for (i=0; i<ANOB_MULTI_END - ANOB_MULTI_START; i++)
    {
        if ( AnOBMultiInfo[i].multiItem == i + ANOB_MULTI_START)
            InitSign(MeaResultAnOB[AnOBMultiInfo[i].item-ANOB_MEA_START], i);
        else
        {
            offset = FindDoubleMeaInfo(ANOB_M, i + ANOB_MULTI_START);
            if (offset == -1)
                PRINTF("There is error in TDMultiInfo!\n");
            else
                InitSign(MeaResultAnOB[AnOBMultiInfo[offset].item-ANOB_MEA_START], i);
        }
    }
}
#endif
void MeaResult::UserInit(void)
{
    int i, j;

    for (j=0; j<MEA_TIMES; j++)
    {
        //for (i=0; i<ABD_MEA_START - 1; i++)
        for (i=0; i< USER_END - USER_START; i++)
        {
            MeaResultUser[i][j] = (double)INVALID_VAL;
        }
    }

#if 0
    for (j=0; j<MEA_TIMES_MULTI; j++)
    {
        for (i=0; i<ABD_MULTI_END - ABD_MULTI_START; i++)
        {
            AbdResultMulti[i].result[j] = (float)INVALID_VAL;
        }
    }

    for (i=0; i<ABD_CALC_END - ABD_CALC_START; i++)
    {
        CalcResultAbd[i][0] = (float)INVALID_VAL;
        CalcResultAbd[i][1] = (float)INVALID_VAL;
    }

    int offset;
    for (i=0; i<ABD_MULTI_END - ABD_MULTI_START; i++)
    {
        if ( AbdoMultiInfo[i].multiItem == i + ABD_MULTI_START )
            InitSign(MeaResultAbd[AbdoMultiInfo[i].item-ABD_MEA_START], i);
        else
        {
            offset = FindDoubleMeaInfo(ABDO_M, i + ABD_MULTI_START);
            if (offset == -1)
            {
                PRINTF("There is error in AbdoMultiInfo!\n");
            }
            else
                InitSign(MeaResultAbd[AbdoMultiInfo[offset].item-ABD_MEA_START], i);
        }
    }
#endif
}

int MeaResult::FindSingleMeaInfo(int section, int item)
{
    int itemTmp;
    int i = 0, j = 0;

    switch (section)
    {
        case ABDO_M:
            itemTmp = item - ABD_MEA_START;
            i = j = itemTmp;
            while ((i>=0) || (j < (ABD_MEA_END - ABD_MEA_START)))
            {
                if ((i==0) && (j==ABD_MEA_END - ABD_MEA_START - 1))
                    break;
                if (item != AbdoInfo[i].item)
                {
                    if (i>0) i--;
                }
                else
                    return i;

                if (item != AbdoInfo[j].item)
                {
                    if (j < (ABD_MEA_END - ABD_MEA_START - 1)) j++;
                }
                else
                    return j;
            }
            return(-1);
            break;

        case ADULT_M:
            itemTmp = item - ADULT_MEA_START;
            i = j = itemTmp;
            while ((i>=0) || (j < (ADULT_MEA_END - ADULT_MEA_START)))
            {
                if ((i==0) && (j==ADULT_MEA_END - ADULT_MEA_START - 1))
                    break;
                if (item != AdultInfo[i].item)
                {
                    if (i>0) i--;
                }
                else
                    return i;

                if (item != AdultInfo[j].item)
                {
                    if (j < (ADULT_MEA_END - ADULT_MEA_START - 1)) j++;
                }
                else
                    return j;
            }
            return(-1);
            break;

        case UR_M:
            itemTmp = item - UR_MEA_START;
            i = j = itemTmp;
            while ((i>=0) || (j < (UR_MEA_END - UR_MEA_START)))
            {
                if ((i==0) && (j==UR_MEA_END - UR_MEA_START - 1))
                    break;
                if (item != URInfo[i].item)
                {
                    if (i>0) i--;
                }
                else
                    return i;

                if (item != URInfo[j].item)
                {
                    if (j < (UR_MEA_END - UR_MEA_START - 1)) j++;
                }
                else
                    return j;
            }
            return(-1);
            break;

        case OB_M:
            itemTmp = item - OB_MEA_START;
            i = j = itemTmp;
            while ((i>=0) || (j < (OB_MEA_END - OB_MEA_START)))
            {
                if ((i==0) && (j==OB_MEA_END - OB_MEA_START - 1))
                    break;
                if (item != OBInfo[i].item)
                {
                    if (i>0) i--;
                }
                else
                    return i;

                if (item != OBInfo[j].item)
                {
                    if (j < (OB_MEA_END - OB_MEA_START - 1)) j++;
                }
                else
                    return j;
            }
            return(-1);
            break;

        case GYN_M:
            itemTmp = item - GYN_MEA_START;
            i = j = itemTmp;
            while ((i>=0) || (j < (GYN_MEA_END - GYN_MEA_START)))
            {
                if ((i==0) && (j==GYN_MEA_END - GYN_MEA_START - 1))
                    break;
                if (item != GYNInfo[i].item)
                {
                    if (i>0) i--;
                }
                else
                    return i;

                if (item != GYNInfo[j].item)
                {
                    if (j < (GYN_MEA_END - GYN_MEA_START - 1)) j++;
                }
                else
                    return j;
            }
            return(-1);
            break;

        case SP_M:
            itemTmp = item - SP_MEA_START;
            i = j = itemTmp;
            while ((i>=0) || (j < (SP_MEA_END - SP_MEA_START)))
            {
                if ((i==0) && (j==SP_MEA_END - SP_MEA_START - 1))
                    break;
                if (item != SPInfo[i].item)
                {
                    if (i>0) i--;
                }
                else
                    return i;

                if (item != SPInfo[j].item)
                {
                    if (j < (SP_MEA_END - SP_MEA_START - 1)) j++;
                }
                else
                    return j;
            }
            return(-1);
            break;
#ifdef EMP_322
#else
        case VS_M:
            itemTmp = item - VS_MEA_START;
            i = j = itemTmp;
            while ((i>=0) || (j < (VS_MEA_END - VS_MEA_START)))
            {
                if ((i==0) && (j==VS_MEA_END - VS_MEA_START - 1))
                    break;
                if (item != VSInfo[i].item)
                {
                    if (i>0) i--;
                }
                else
                    return i;

                if (item != VSInfo[j].item)
                {
                    if (j < (VS_MEA_END - VS_MEA_START - 1)) j++;
                }
                else
                    return j;
            }
            return(-1);
            break;
#endif
        case FETAL_M:
            itemTmp = item - FETAL_MEA_START;
            i = j = itemTmp;
            while ((i>=0) || (j < (FETAL_MEA_END - FETAL_MEA_START)))
            {
                if ((i==0) && (j==FETAL_MEA_END - FETAL_MEA_START - 1))
                    break;
                if (item != FetalInfo[i].item)
                {
                    if (i>0) i--;
                }
                else
                    return i;

                if (item != FetalInfo[j].item)
                {
                    if (j < (FETAL_MEA_END - FETAL_MEA_START - 1)) j++;
                }
                else
                    return j;
            }
            return(-1);
            break;
#ifdef EMP_322
#else
        case TCD_M:
            itemTmp = item - TCD_MEA_START;
            i = j = itemTmp;
            while ((i>=0) || (j < (TCD_MEA_END - TCD_MEA_START)))
            {
                if ((i==0) && (j==TCD_MEA_END - TCD_MEA_START - 1))
                    break;
                if (item != TCDInfo[i].item)
                {
                    if (i>0) i--;
                }
                else
                    return i;

                if (item != TCDInfo[j].item)
                {
                    if (j < (TCD_MEA_END - TCD_MEA_START - 1)) j++;
                }
                else
                    return j;
            }
            return(-1);
            break;
#endif
        case ORTHO_M:
            itemTmp = item - ORTHO_MEA_START;
            i = j = itemTmp;
            while ((i>=0) || (j < (ORTHO_MEA_END - ORTHO_MEA_START)))
            {
                if ((i==0) && (j==ORTHO_MEA_END - ORTHO_MEA_START - 1))
                    break;
                if (item != OrthoInfo[i].item)
                {
                    if (i>0) i--;
                }
                else
                    return i;

                if (item != OrthoInfo[j].item)
                {
                    if (j < (ORTHO_MEA_END - ORTHO_MEA_START - 1)) j++;
                }
                else
                    return j;
            }
            return(-1);
            break;

//hlx
#ifdef VET
        case TD_M:
            itemTmp = item - TD_MEA_START;//TD_MULTI_START;
            i = j = itemTmp;
            //while ((i>=0) || (j<(TD_MULTI_END-TD_MULTI_START)))
            while ((i>=0) || (j<(TD_MEA_END-TD_MEA_START)))
            {
                //if ((i==0) && (j==TD_MULTI_END-TD_MULTI_START-1))
                if ((i==0) && (j==TD_MEA_END-TD_MEA_START-1))
                    break;
                //if (item != TDMultiInfo[i].item)
                if (item != TDInfo[i].item)
                {
                    if (i>0)i--;
                }
                else
                    return i;
                //if (item != TDMultiInfo[i].item)
                if (item != TDInfo[i].item)
                {
                    //if (j<(TD_MULTI_END-TD_MULTI_START-1)) j++;
                    if (j<(TD_MEA_END-TD_MEA_START-1)) j++;
                }
                else
                    return j;
            }
            return (-1);
            break;

    //vet OB
    case ANOB_M:
            itemTmp = item - ANOB_MEA_START;
            i = j = itemTmp;
            while ((i>=0) || (j<(ANOB_MEA_END-TD_MEA_START)))
            {
                if ((i==0) && (j==ANOB_MEA_END-ANOB_MEA_START-1))
                    break;
                if (item != AnOBInfo[i].item)
                {
                    if (i>0)i--;
                }
                else
                    return i;
                if (item != AnOBInfo[i].item)
                {
                    if (j<(ANOB_MEA_END-ANOB_MEA_START-1))j++;
                }
                else
                    return j;
            }
            return (-1);
            break;

#endif
        case USER_M:
            itemTmp = item-USER_START ;
            i = j = itemTmp;
            while ((i>=0) || (j < (USER_END - USER_START)))
            {
                if ((i==0) && (j== (USER_END -USER_START -1)))
                    break;
                if (item != userDef_new[i].item)
                {
                    if (i>0) i--;
                }
                else
                    return i;

                if (item != userDef_new[j].item)
                {
                    if (j < (USER_END - USER_START -1)) j++;
                }
                else
                    return j;
            }
            return(-1);
            break;

        default:
            return(-1);
    }
    return(-1);
}

int MeaResult::FindDoubleMeaInfo(int section, int multiItem)
{
    int itemTmp;
    int i = 0, j = 0;

    switch (section)
    {
        case ABDO_M:
            itemTmp = multiItem - ABD_MULTI_START;
            i = j = itemTmp;
            while ((i>=0) || (j < (ABD_MULTI_END - ABD_MULTI_START)))
            {
                if ((i==0) && (j==ABD_MULTI_END - ABD_MULTI_START - 1))
                    break;
                if (multiItem != AbdoMultiInfo[i].multiItem)
                {
                    if (i>=0) i--;
                }
                else
                    return i;

                if (multiItem != AbdoMultiInfo[j].multiItem)
                {
                    if (j < (ABD_MULTI_END - ABD_MULTI_START)) j++;
                }
                else
                    return j;
            }
            return(-1);
            break;

        case ADULT_M:
            itemTmp = multiItem - ADULT_MULTI_START;
            i = j = itemTmp;
            while ((i>=0) || (j < (ADULT_MULTI_END - ADULT_MULTI_START)))
            {
                if ((i==0) && (j==ADULT_MULTI_END - ADULT_MULTI_START - 1))
                    break;
                if (multiItem != AdultMultiInfo[i].multiItem)
                {
                    if (i>=0) i--;
                }
                else
                    return i;

                if (multiItem != AdultMultiInfo[j].multiItem)
                {
                    if (j < (ADULT_MULTI_END - ADULT_MULTI_START)) j++;
                }
                else
                    return j;
            }
            return(-1);
            break;

        case UR_M:
            itemTmp = multiItem - UR_MULTI_START;
            i = j = itemTmp;
            while ((i>=0) || (j < (UR_MULTI_END - UR_MULTI_START)))
            {
                if ((i==0) && (j==UR_MULTI_END - UR_MULTI_START - 1))
                    break;
                if (multiItem != URMultiInfo[i].multiItem)
                {
                    if (i>=0) i--;
                }
                else
                    return i;

                if (multiItem != URMultiInfo[j].multiItem)
                {
                    if (j < (UR_MULTI_END - UR_MULTI_START)) j++;
                }
                else
                    return j;
            }
            break;

        case OB_M:
            itemTmp = multiItem - OB_MULTI_START;
            i = j = itemTmp;
            while ((i>=0) || (j < (OB_MULTI_END - OB_MULTI_START)))
            {
                if ((i==0) && (j==OB_MULTI_END - OB_MULTI_START - 1))
                {
#ifdef EMP_322
                    return 0;
#else
                    break;
#endif
                }
                if (multiItem != OBMultiInfo[i].multiItem)
                {
                    if (i>=0) i--;
                }
                else
                    return i;

                if (multiItem != OBMultiInfo[j].multiItem)
                {
                    if (j < (OB_MULTI_END - OB_MULTI_START)) j++;
                }
                else
                    return j;
            }
            return(-1);
            break;

        case SP_M:
            itemTmp = multiItem - SP_MULTI_START;
            i = j = itemTmp;
            while ((i>=0) || (j < (SP_MULTI_END - SP_MULTI_START)))
            {
                if ((i==0) && (j==SP_MULTI_END - SP_MULTI_START - 1))
                    break;
                if (multiItem != SPMultiInfo[i].multiItem)
                {
                    if (i>=0) i--;
                }
                else
                    return i;

                if (multiItem != SPMultiInfo[j].multiItem)
                {
                    if (j < (SP_MULTI_END - SP_MULTI_START)) j++;
                }
                else
                    return j;
            }
            return(-1);
            break;

        case GYN_M:
            itemTmp = multiItem - GYN_MULTI_START;
            i = j = itemTmp;
            while ((i>=0) || (j < (GYN_MULTI_END - GYN_MULTI_START)))
            {
                if ((i==0) && (j==GYN_MULTI_END - GYN_MULTI_START - 1))
                    break;
                if (multiItem != GYNMultiInfo[i].multiItem)
                {
                    if (i>=0) i--;
                }
                else
                    return i;

                if (multiItem != GYNMultiInfo[j].multiItem)
                {
                    if (j < (GYN_MULTI_END - GYN_MULTI_START)) j++;
                }
                else
                    return j;
            }
            return(-1);
            break;
#ifdef EMP_322
#else
        case VS_M:
            itemTmp = multiItem - VS_MULTI_START;
            i = j = itemTmp;
            while ((i>=0) || (j < (VS_MULTI_END - VS_MULTI_START)))
            {
                if ((i==0) && (j==VS_MULTI_END - VS_MULTI_START - 1))
                    break;
                if (multiItem != VSMultiInfo[i].multiItem)
                {
                    if (i>=0) i--;
                }
                else
                    return i;

                if (multiItem != VSMultiInfo[j].multiItem)
                {
                    if (j < (VS_MULTI_END - VS_MULTI_START)) j++;
                }
                else
                    return j;
            }
            return(-1);
            break;
#endif
        case FETAL_M:
            itemTmp = multiItem - FETAL_MULTI_START;
            i = j = itemTmp;
            while ((i>=0) || (j < (FETAL_MULTI_END - FETAL_MULTI_START)))
            {
                if ((i==0) && (j==FETAL_MULTI_END - FETAL_MULTI_START - 1))
                    break;
                if (multiItem != FetalMultiInfo[i].multiItem)
                {
                    if (i>=0) i--;
                }
                else
                    return i;

                if (multiItem != FetalMultiInfo[j].multiItem)
                {
                    if (j < (FETAL_MULTI_END - FETAL_MULTI_START)) j++;
                }
                else
                    return j;
            }
            return(-1);
            break;
#ifdef EMP_322
#else
        case TCD_M:
            itemTmp = multiItem - TCD_MULTI_START;
            i = j = itemTmp;
            while ((i>=0) || (j < (TCD_MULTI_END - TCD_MULTI_START)))
            {
                if ((i==0) && (j==TCD_MULTI_END - TCD_MULTI_START - 1))
                    break;
                if (multiItem != TCDMultiInfo[i].multiItem)
                {
                    if (i>=0) i--;
                }
                else
                    return i;

                if (multiItem != TCDMultiInfo[j].multiItem)
                {
                    if (j < (TCD_MULTI_END - TCD_MULTI_START)) j++;
                }
                else
                    return j;
            }
            return(-1);
            break;
#endif
        case ORTHO_M:
            itemTmp = multiItem - ORTHO_MULTI_START;
            i = j = itemTmp;
            while ((i>=0) || (j < (ORTHO_MULTI_END - ORTHO_MULTI_START)))
            {
                if ((i==0) && (j==ORTHO_MULTI_END - ORTHO_MULTI_START - 1))
                    break;
                if (multiItem != OrthoMultiInfo[i].multiItem)
                {
                    if (i>=0) i--;
                }
                else
                    return i;

                if (multiItem != OrthoMultiInfo[j].multiItem)
                {
                    if (j < (ORTHO_MULTI_END - ORTHO_MULTI_START)) j++;
                }
                else
                    return j;
            }
            return(-1);
            break;
#ifdef VET
        case TD_M:
            itemTmp = multiItem - TD_MULTI_START;
            i = j = itemTmp;
            while ((i>=0) || (j<(TD_MULTI_END-TD_MULTI_START)))
            {
                if ((i==0) && (j==TD_MULTI_END-TD_MULTI_START-1))
                    break;
                if (multiItem != TDMultiInfo[i].multiItem)
                {
                    if (i>0)i--;
                }
                else
                    return i;
                if (multiItem != TDMultiInfo[i].multiItem)
                {
                    if (j<(TD_MULTI_END-TD_MULTI_START-1)) j++;
                }
                else
                    return j;
            }
            return (-1);
            break;
#endif
        default:
            return(-1);
    }
    return(-1);
}