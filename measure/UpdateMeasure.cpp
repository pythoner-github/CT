
/*
 * 2009, 深圳恩普电子技术有限公司
 *
 * @file: UpdateMeasure.cpp
 * @brief: update display of measure result  
 * 
 * version: V1.0
 * date: 2009-7-9
 * @author: zhanglei
 */

#include <string>
#include "UpdateMeasure.h"
#include "gui_func.h"
#include "Def.h"
#include "ImageArea.h"
#include "SysGeneralSetting.h"
#include "Img2D.h"
#include "../sysMan/SysMeasurementSetting.h"
#include "MeasureMan.h"
#include "MeasureFactory.h"

deque<UpdateMeasure::Result> UpdateMeasure::m_dequeResult;

UpdateMeasure::UpdateMeasure(void)
{
    SysGeneralSetting sysSetting;
    m_ptrImg = ImageArea::GetInstance();
    m_dateFormat = sysSetting.GetDateFormat();
}

UpdateMeasure::~UpdateMeasure()
{
}

int UpdateMeasure::GetResultCount(void)
{
	return m_dequeResult.size();
}

void UpdateMeasure::PutResult(void)
{
    RefreshResult();
}

void UpdateMeasure::RefreshResult(bool update)
{
    m_ptrImg->ClearMeasureAll();
    deque<Result>::iterator i;
    int pos = 0;
    for (i=m_dequeResult.begin(); i!=m_dequeResult.end(); ++i) {
        pos += i->lines;
        m_ptrImg->DrawMeasureResult(i->text.c_str(), pos, i->lines, i->attr.cursorType, MeasureColorConvert(i->attr.confirmColor), false);

    }
	if(update)
		m_ptrImg->UpdateImgArea();
}

void UpdateMeasure::UpdateResultForChgFont(void)
{
#ifdef TRANSDUCER
    SysMeasurementSetting sysMeasure;
    int m_meaResultFontSize = sysMeasure.GetMeasureResult();
    int measure_line_max = 14;
    if(m_meaResultFontSize == 1)
    {
        measure_line_max = 10;
    }
    int lines = CountMeasureLines();
    if (lines > measure_line_max) 
    {
        while (lines > measure_line_max) {
            m_dequeResult.pop_front();
            lines = CountMeasureLines();
        }
    }
    RefreshResult();
#else
    int lines = CountMeasureLines();
    if (lines > MEASURE_RES_LINES_MAX) 
    {
        while (lines > MEASURE_RES_LINES_MAX) {
            m_dequeResult.pop_front();
            lines = CountMeasureLines();
        }
    }
    RefreshResult();
#endif
}

void UpdateMeasure::ClearCurColorPwResult()
{
    RefreshResult();
}

void UpdateMeasure::UpdateResult(string result, int cursorType, int curColor)
{
#ifdef TRANSDUCER
    SysMeasurementSetting sysMeasure;
   int  m_meaResultFontSize = sysMeasure.GetMeasureResult();
    int measure_line_max = 14;
    if(m_meaResultFontSize == 1)
    {
        measure_line_max = 10;
    }
#endif

    int lines = CountMeasureLines();
    int curLines = CountLines(result);
#ifdef TRANSDUCER
    if (curLines + lines > measure_line_max) {
        while (lines + curLines > measure_line_max) {
            m_dequeResult.pop_front();
            lines = CountMeasureLines();
        }
        RefreshResult();
    }
#else
    if (curLines + lines > MEASURE_RES_LINES_MAX) {
		while (lines + curLines > MEASURE_RES_LINES_MAX) {
			m_dequeResult.pop_front();
			lines = CountMeasureLines();
		}
		RefreshResult();
	}
#endif
    m_ptrImg->DrawMeasureResult(result.c_str(), lines+curLines, curLines, cursorType, MeasureColorConvert(curColor), true);
}

void UpdateMeasure::ClearMeasure(void)
{
    if ((int)m_dequeResult.size() < m_ptrImg->GetMeasurePosSize())
        m_ptrImg->ClearLastMeasureResult();
}

void UpdateMeasure::ClearLast()
{
    if (!m_dequeResult.empty())
    	m_dequeResult.pop_back();
    RefreshResult();
}

void UpdateMeasure::ClearPwResultUnFreeze(int cursorType)
{
    int size = m_dequeResult.size();
    int i;
    bool flag = false;
    vector<Result> vecResult;
    //printf("size:%d  type:%d\n", size, cursorType);
    if(size > 0)
    {
        for(i = 0; i <= size - 1; i++)
        {
            if(m_dequeResult[i].attr.cursorType == cursorType)
            {
                flag = true;
                break;
            }
        }

        if(flag)
        {
            for(int j = size - 1; j > i; j--)
            {
                vecResult.push_back(m_dequeResult[j]);
                m_dequeResult.pop_back();
            }
            if(m_dequeResult.size() > 0)
                m_dequeResult.pop_back();

            for(int j = vecResult.size() - 1; j >= 0; j--)
            {
                m_dequeResult.push_back(vecResult[j]);
            }

            RefreshResult();
        }
    }
}

void UpdateMeasure::ClearFirst()
{
}

void UpdateMeasure::ClearAll(bool update)
{
    m_dequeResult.clear();
    RefreshResult(update);
}

void UpdateMeasure::AddResult(string result, int cursorType, int colorConfirm)
{
#ifdef TRANSDUCER
    SysMeasurementSetting sysMeasure;
    int m_meaResultFontSize = sysMeasure.GetMeasureResult();
    int measure_line_max = 14;
    if(m_meaResultFontSize == 1)
    {
        measure_line_max = 10;
    }
#endif
    Result res;
    res.text = result;
    res.lines = CountLines(res.text);
    int lines = CountMeasureLines();
#ifdef TRANSDUCER
    while (lines + res.lines > measure_line_max) {
        m_dequeResult.pop_front();
        lines = CountMeasureLines();
    }

#else
    while (lines + res.lines > MEASURE_RES_LINES_MAX) {
        m_dequeResult.pop_front();
        lines = CountMeasureLines();
    }
#endif
    res.attr.cursorType = cursorType;
    res.attr.confirmColor = colorConfirm;
    m_dequeResult.push_back(res);
}

int UpdateMeasure::CountMeasureLines(void)
{
    deque<Result>::iterator i;
    int countLines = 0;
    for (i=m_dequeResult.begin(); i!=m_dequeResult.end(); i++) {
	countLines += i->lines;
    }
    return countLines;
}

void UpdateMeasure::D2Dist(double length, ResultAttr& attr, bool inMeasure)
{
    char buf[50];
    string units;
    double coeffi = 1.0;
    MeasureMan::GetInstance()->GetMeasureUnit( coeffi, units, CM);
    
    sprintf(buf, "%s= %3.2f%s", _("dist"), length*coeffi, units.c_str());
    string result = buf;
    if (inMeasure == true)
        UpdateResult(result, attr.cursorType, attr.curColor);
    else {
        AddResult(result, attr.cursorType, attr.confirmColor);
        PutResult();
    }
    return; //for test
}
void UpdateMeasure::DIntegralArea(double integralarea, ResultAttr& attr, bool inMeasure)
{
    char buf[50];
    string units;
    double coeffi = 1.0;
    MeasureMan::GetInstance()->GetMeasureUnit( coeffi, units, CM);
    sprintf(buf, "VTI= %3.2f%s", integralarea*coeffi, units.c_str());
    string result = buf;
    if (inMeasure == true)
	UpdateResult(result, attr.cursorType, attr.curColor);
    else {
	AddResult(result, attr.cursorType, attr.confirmColor);
	PutResult();
    }
    return; //for test
}

void UpdateMeasure::CatHighCalc(const CalcResultInfo *calcInfo, int *calcOrder, double dataMea[], char buf[], int meaLen)
{
	char buf_tmp[30];
	
	//printf("************=========*********calcOrder= %d, CatHighCalc: %s, item= %d\n", (*calcOrder)+meaLen, calcInfo->title, calcInfo->item);

	if ((int)dataMea[(*calcOrder)+meaLen] != INVALID_VAL)
	{
        string units;
        double coeffi = 1.0;
        MeasureMan::GetInstance()->GetMeasureUnit( coeffi, units, calcInfo->unitItem);
        strcat(buf, "\n");
		strcat(buf, _(calcInfo->title));
		sprintf(buf_tmp, "= %3.2f", dataMea[(*calcOrder)+meaLen]*coeffi);
		strcat(buf, buf_tmp);
		strcat(buf, units.c_str());
	}
	(*calcOrder)++;

	if (calcInfo->ptrHCalcInfo == NULL)
		return;

	CalcInfoArray *hCalc;
	int i = 0;
	hCalc = (CalcInfoArray *)(calcInfo->ptrHCalcInfo);
//	if (calcInfo->item == ADULT_CO_A2C)
//	{
//		printf("\n=========\n");
//	}

	while ((*hCalc)[i] != NULL)
	{
		CatHighCalc((*hCalc)[i], calcOrder, dataMea, buf, meaLen);
		i++;
	}

	return;
}

void UpdateMeasure::GenDisplaySingle(const SingleItemInfo *info, double data[], ResultAttr& attr, bool inMeasure, int unit_coeffi)
{
    char buf[600];
	char buf_tmp[30];
	int i, k;
    const CalcInfoP *singleCalcInfo;
    int calcOrder = 0;
    if(info->meaType == AREA_TRACK)
    {
        if ((int)data[0] == INVALID_VAL)
            return;
    }

    string units;
   	double coeffi = 1.0;
    MeasureMan::GetInstance()->GetMeasureUnit(coeffi, units,info->unitItem);

    if ((int)data[0] == INVALID_VAL)//测量结果数据为0，则直接返回
    {
        sprintf(buf, "%s", _(info->title));
        sprintf(buf_tmp, " %s", _(" Invalid"));
        strcat(buf, buf_tmp);
    }
    else
    {
        sprintf(buf, "%s", _(info->title));
        sprintf(buf_tmp, "= %3.2f", data[0]*coeffi);
        strcat(buf, buf_tmp);
        strcat(buf, units.c_str());

    }

    if (info->ptrCalcInfo == NULL)
        goto display;

	singleCalcInfo = (CalcInfoP *)(info->ptrCalcInfo);
	
	i = 0;
	CalcInfoArray *hCalc;
	while ((*singleCalcInfo)[i] != NULL)
    {
        if ((int)data[calcOrder+MEA_SINGLE] != INVALID_VAL)
        {
            coeffi = 1.0;
            MeasureMan::GetInstance()->GetMeasureUnit(coeffi, units,(*singleCalcInfo)[i]->unitItem);
            strcat(buf, "\n");

            if (strcmp((*singleCalcInfo)[i]->title, "GW") == 0)
            {
                if ((int)data[calcOrder+MEA_SINGLE] == INVALID_VAL_GW)
                {
                    strcat(buf, _((*singleCalcInfo)[i]->title));
                    sprintf(buf_tmp, _(" Invalid"));
                }
                else
                {
                    strcat(buf, _((*singleCalcInfo)[i]->title));
                    sprintf(buf_tmp, "= %2dw%2dd", (int)data[calcOrder+MEA_SINGLE]/7, (int)data[calcOrder+MEA_SINGLE]%7);
                }
                strcat(buf, buf_tmp);
            }
            else if (strcmp((*singleCalcInfo)[i]->title, "EDD") == 0)
            {
                if ((int)data[calcOrder+MEA_SINGLE] == INVALID_VAL_GW)
                {
                    strcat(buf, _((*singleCalcInfo)[i]->title));
                    sprintf(buf_tmp, _(" Invalid  "));
                    strcat(buf, buf_tmp);
                }
                else
                {
                    strcat(buf, _((*singleCalcInfo)[i]->title));
                    if (m_dateFormat == 0)
                        sprintf(buf_tmp, "= %4d/%2d/%2d", (int)data[calcOrder+MEA_SINGLE]/10000, (int)data[calcOrder+MEA_SINGLE]%10000/100, (int)data[calcOrder+MEA_SINGLE]%100);
                    else if(m_dateFormat == 1)
                        sprintf(buf_tmp, "= %2d/%2d/%4d", (int)data[calcOrder+MEA_SINGLE]%10000/100, (int)data[calcOrder+MEA_SINGLE]%100, (int)data[calcOrder+MEA_SINGLE]/10000);
                    else if(m_dateFormat == 2)
                        sprintf(buf_tmp, "= %2d/%2d/%4d", (int)data[calcOrder+MEA_SINGLE]%100, (int)data[calcOrder+MEA_SINGLE]%10000/100, (int)data[calcOrder+MEA_SINGLE]/10000);
                    else
                        sprintf(buf_tmp, "= %4d/%2d/%2d", (int)data[calcOrder+MEA_SINGLE]/10000, (int)data[calcOrder+MEA_SINGLE]%10000/100, (int)data[calcOrder+MEA_SINGLE]%100);
                    strcat(buf, buf_tmp);
                }
            }
            else if(strcmp((*singleCalcInfo)[i]->title, "EFW") == 0)
            {
                strcat(buf, _((*singleCalcInfo)[i]->title));
                sprintf(buf_tmp, "= %3.2f", data[calcOrder+MEA_SINGLE]*coeffi);
                strcat(buf, buf_tmp);
				//sprintf(buf_tmp, "= %3.2f", data[calcOrder+MEA_SINGLE]*1000);
				//strcat(buf, "g");//units[(*singleCalcInfo)[i]->unitItem]);
				strcat(buf, units.c_str());
            }
			else
			{
				strcat(buf, _((*singleCalcInfo)[i]->title));
				sprintf(buf_tmp, "= %3.2f", data[calcOrder+MEA_SINGLE]*coeffi);
				strcat(buf, buf_tmp);
				strcat(buf, units.c_str());
            }
		}
			
		calcOrder++;
		if ((*singleCalcInfo)[i]->ptrHCalcInfo != NULL)//用计算值计算出来的计算值
		{
			hCalc = (CalcInfoArray *)((*singleCalcInfo)[i]->ptrHCalcInfo);
			k = 0;
			while ((*hCalc)[k] != NULL)
			{
				CatHighCalc((*hCalc)[k], &calcOrder, data, buf, MEA_SINGLE);
				k++;
			}
		}
		i++;
	}

display:
	
	string result = buf;
	if (inMeasure == true)
		UpdateResult(result, attr.cursorType, attr.curColor);
	else 
	{
		AddResult(result, attr.cursorType, attr.confirmColor);
		PutResult();
	}
	return; //for test
}


void UpdateMeasure::GenDisplayMulti(const MultiItemInfo *info, double dataMea[], ResultAttr& attr, bool inMeasure, int unit_coeffi[])
{
	char buf[256];
	char buf_tmp[30]={'\0'};
	int i, k;
	const CalcInfoP *multiCalcInfo;
	int calcOrder = 0;

	if ((int)dataMea[0] == INVALID_VAL)//测量结果数据为0，则直接返回
		return;

    string units;
    double coeffi = 1.0;
	buf[0] = '\0';
	if ((info->itemTitle)[0] != '\0')
	{
	    sprintf(buf, "%s", _(info->itemTitle));
	    strcat(buf, ":\n");
	}
	
	for (i=0; i<MEA_MULTI; i++)
	{
		if (((info->titleUnit)[i].title[0] != '\0') && (dataMea[i] != INVALID_VAL))
		{
            MeasureMan::GetInstance()->GetMeasureUnit(coeffi, units, info->titleUnit[i].unitItem);
			strcat(buf, _((info->titleUnit[i]).title));
			sprintf(buf_tmp, "= %3.2f", dataMea[i]*coeffi);
			strcat(buf, buf_tmp);
			strcat(buf, units.c_str());
		}

		if (i < MEA_MULTI-1)
		{
			if (((info->titleUnit)[i+1].title[0] != '\0') && (dataMea[i+1] != INVALID_VAL))
				strcat(buf, "\n");
		}
	}
	
	if (info->ptrCalcInfo == NULL)
		goto display;

	multiCalcInfo = (CalcInfoP *)(info->ptrCalcInfo);

//显示计算的结果
//	for (i=0; i<MEA_MULTI+CALC_MAX; i++)
//	{
//		printf("Multi Result: allData[%d]= %f\n", i, dataMea[i]);
//	}

	i = 0;
	CalcInfoArray *hCalc;
	while ((*multiCalcInfo)[i] != NULL)
	{
		if ((int)dataMea[calcOrder+MEA_MULTI] != INVALID_VAL)
		{
            coeffi = 1.0;
            MeasureMan::GetInstance()->GetMeasureUnit(coeffi, units, (*multiCalcInfo)[i]->unitItem);
			strcat(buf, "\n");
			strcat(buf, _((*multiCalcInfo)[i]->title));
			sprintf(buf_tmp, "= %3.2f", dataMea[calcOrder+MEA_MULTI]*coeffi);
			strcat(buf, buf_tmp);
			strcat(buf, units.c_str());
		}
		calcOrder++;

		if ((*multiCalcInfo)[i]->ptrHCalcInfo != NULL)//用计算值计算出来的计算值
		{
			hCalc = (CalcInfoArray *)((*multiCalcInfo)[i]->ptrHCalcInfo);
			k = 0;
			while ((*hCalc)[k] != NULL)
			{
				CatHighCalc((*hCalc)[k], &calcOrder, dataMea, buf, MEA_MULTI);
				k++;
			}
		}
		i++;
	}

display:
	string result = buf;
	if (inMeasure == true)
	    UpdateResult(result, attr.cursorType, attr.curColor);
	else {
	    AddResult(result, attr.cursorType, attr.confirmColor);
	    PutResult();
	}
	return; //for test
}
//髋关节测量
void UpdateMeasure::D2Hip(double angle1, double angle2, ResultAttr& attr, bool inMeasure)
{
	char buf[256];
	string result;

	if ((angle2==INVALID_VAL) && (inMeasure==true))
	{
		sprintf(buf, "α= %3.0f˚", angle1);
		result = buf; 
		UpdateResult(result, attr.cursorType, attr.curColor);
		return;
	}

	sprintf(buf, "α= %3.0f˚\nβ= %3.0f˚", angle1, angle2);

	if(inMeasure==true)
	{
		result = buf;
	    UpdateResult(result, attr.cursorType, attr.curColor);
	}
	else
	{
		string res = buf;
#ifdef VET
       result = buf;
#else
		if ((angle1>=60) && (angle2<55)) //type = Ia
			result = res + "\nType= Ia";
		else if ((angle1>=60) && (angle2>=55)&&(angle2<77)) //type = Ib
			result = res + "\nType= Ib";
		else if ((angle1>=50) && (angle1<60) && (angle2>=55)&&(angle2<77))//type = IIa or IIb
			result = res + "\nType= IIa, IIb";
		else if ((angle1>=43) && (angle1<50) && (angle2>=0) && (angle2<=77)) //type = IIc
			result = res + "\nType= IIc";
		else if ((angle1>=43) && (angle1<50) && (angle2>77)) //type = IId
			result = res + "\nType= IId";
		else if ((angle1>=0) && (angle1<43) && (angle2>77)) //type = III or IV
			result = res + "\nType= III, IV";
		else
			result = res + "\nType= XXX";
#endif
	    AddResult(result, attr.cursorType, attr.confirmColor);
	    PutResult();
	}
}

#if 0
void UpdateMeasure::D2Hip(double angle1, double angle2, ResultAttr& attr, bool inMeasure)
{
	char buf[256];
	string result;

	if ((-0.0001<angle2) && (angle2<0.0001) && (inMeasure==true))
	{
		sprintf(buf, "α= %3.2f˚", angle1);
		result = buf; 
		UpdateResult(result, attr.cursorType, attr.curColor);
		return;
	}

	if ((angle1>=60) && (angle2<55)) //type = Ia
		sprintf(buf, "α= %3.2f˚\nβ= %3.2f˚\nType= Ia", angle1, angle2);
	else if ((angle1>=60) && (angle2>=55)&&(angle2<77)) //type = Ib
		sprintf(buf, "α= %3.2f˚\nβ= %3.2f˚\nType= Ib", angle1, angle2);
	else if ((angle1>=50) && (angle1<60) && (angle2>=55)&&(angle2<77))//type = IIa or IIb
		sprintf(buf, "α= %3.2f˚\nβ= %3.2f˚\nType= IIa, IIb", angle1, angle2);
	else if ((angle1>=43) && (angle1<50) && (angle2>=0) && (angle2<=77)) //type = IId
		sprintf(buf, "α= %3.2f˚\nβ= %3.2f˚\nType= IIc", angle1, angle2);
	else if ((angle1>=43) && (angle1<50) && (angle2>77)) //type = IId
		sprintf(buf, "α= %3.2f˚\nβ= %3.2f˚\nType= IId", angle1, angle2);
	else if ((angle1>=0) && (angle1<43) && (angle2>77)) //type = III or IV
		sprintf(buf, "α= %3.2f˚\nβ= %3.2f˚\nType= III, IV", angle1, angle2);
	else
		sprintf(buf, "α= %3.2f˚\nβ= %3.2f˚\nType= XXX", angle1, angle2);

	result = buf; 

	if (inMeasure == true)
	    UpdateResult(result, attr.cursorType, attr.curColor);
	else {
	    AddResult(result, attr.cursorType, attr.confirmColor);
	    PutResult();
	}

	return;
}
#endif

void UpdateMeasure::D2LenTrack(double length, ResultAttr& attr, bool inMeasure)
{
    char buf[50];

    string units;
    double coeffi = 1.0;
    MeasureMan::GetInstance()->GetMeasureUnit( coeffi, units, CM);
    sprintf(buf, "%s= %3.2f%s", _("length"), length*coeffi, units.c_str());
    string result = buf;
    if (inMeasure == true)
        UpdateResult(result, attr.cursorType, attr.curColor);
    else {
	AddResult(result, attr.cursorType, attr.confirmColor);
	PutResult();
    }
    return; //for test
}

void UpdateMeasure::D2AreaDot(double area, ResultAttr& attr, bool inMeasure)
{
	char buf[60];

    string units;
    double coeffi = 1.0;
    MeasureMan::GetInstance()->GetMeasureUnit( coeffi, units, CM2);
	sprintf(buf, "%s= %3.2f%s",_("area"), area*coeffi, units.c_str());
	//sprintf(buf, "area= %3.2fcm²", area);
	string result = buf;
	if (inMeasure == true)
	    UpdateResult(result, attr.cursorType, attr.curColor);
	else
	{
	    AddResult(result, attr.cursorType, attr.confirmColor);
	    PutResult();
	}
	return; //for test
}

void UpdateMeasure::D2AreaRec(double area, ResultAttr& attr, bool inMeasure)
{
    char buf[60];
    string units;
    double coeffi = 1.0;
    MeasureMan::GetInstance()->GetMeasureUnit( coeffi, units, CM2);

    sprintf(buf, "%s= %3.2f%s",_("area"), area*coeffi, units.c_str());
    //sprintf(buf, "area= %3.2fcm²", area);
	string result = buf;
	if (inMeasure == true)
	    UpdateResult(result, attr.cursorType, attr.curColor);
	else {
	    AddResult(result, attr.cursorType, attr.confirmColor);
	    PutResult();
	}
	return; //for test    
}

void UpdateMeasure::D2Vol3Axis(char *name, double axis_x, double axis_y, double axis_z, double vol, ResultAttr& attr, bool inMeasure)
{
    char buf_tmp[50];
    char buf[256];
    string result;
    string units;
    string unit_vol;
    double coeffi = 1.0;
    double coeffi_vol = 1.0;
    MeasureMan::GetInstance()->GetMeasureUnit( coeffi, units, CM);
    MeasureMan::GetInstance()->GetMeasureUnit( coeffi_vol, unit_vol, CM3);

    if (((-0.0001<axis_y) && (axis_y<0.0001)) && ((-0.0001<axis_z) && (axis_z<0.0001)) && (inMeasure==true)) {
        if (*name == '\0')
        {
            sprintf(buf, "x= %3.2f%s", axis_x*coeffi, units.c_str());
        }
        else {
	    sprintf(buf, "%s", name);
	    strcat(buf, "\nx= ");
	    sprintf(buf_tmp, "%3.2f%s", axis_x*coeffi, units.c_str());
	    strcat(buf, buf_tmp);
	}
	result = buf; 
	UpdateResult(result, attr.cursorType, attr.curColor);
	return;
    }

    if (((-0.0001<axis_z) && (axis_z<0.0001)) && (inMeasure==true)) {
	if (*name == '\0')
	    sprintf(buf, "x= %3.2f%s\ny= %3.2f%s", axis_x*coeffi, units.c_str(), axis_y*coeffi, units.c_str());
	else {
	    sprintf(buf, "%s", name);
	    strcat(buf, "\nx= ");
	    sprintf(buf_tmp, "%3.2f%s\ny= %3.2f%s", axis_x*coeffi, units.c_str(),  axis_y*coeffi, units.c_str());
	    strcat(buf, buf_tmp);
	}
	result = buf; 
	UpdateResult(result, attr.cursorType, attr.curColor);
	return;
    }

    if (*name == '\0')
	sprintf(buf, "x= %3.2f%s\ny= %3.2f%s\nz= %3.2f%s\n%s= %3.2f%s", axis_x*coeffi, units.c_str(),axis_y*coeffi, units.c_str(), axis_z*coeffi, units.c_str(), _("vol"), vol*coeffi_vol, unit_vol.c_str());
    else {
	sprintf(buf, "%s", name);
	strcat(buf, "\nx= ");
	sprintf(buf_tmp, "%3.2f%s\ny= %3.2f%s\nz= %3.2f%s\n%s= %3.2f%s", axis_x*coeffi, units.c_str(),  axis_y*coeffi, units.c_str(),  axis_z*coeffi, units.c_str(), _("vol"), vol*coeffi_vol, unit_vol.c_str());
	strcat(buf, buf_tmp);

    }
    result = buf; 

    if (inMeasure == true)
	UpdateResult(result, attr.cursorType, attr.curColor);
    else {
	AddResult(result, attr.cursorType, attr.confirmColor);
	PutResult();
    }
    return; //for test
}

void UpdateMeasure::D2VolEllipse1(double vol, ResultAttr& attr, bool inMeasure)
{
	char buf[50];

    string unit_vol;
    double coeffi_vol = 1.0;
    MeasureMan::GetInstance()->GetMeasureUnit( coeffi_vol, unit_vol, CM3);
	sprintf(buf, "%s= %3.2f%s", _("vol"), vol*coeffi_vol, unit_vol.c_str());
	string result = buf; 

	if (inMeasure == true)
	    UpdateResult(result, attr.cursorType, attr.curColor);
	else {
	    AddResult(result, attr.cursorType, attr.confirmColor);
	    PutResult();
	}
	return;
}

void UpdateMeasure::D2VolEllipse2(double area, double dist, double vol, ResultAttr& attr, bool inMeasure)
{
	char buf[256];
    string result;
    string unit;
    string unit_area;
    string unit_vol;
    double coeffi = 1.0;
    double coeffi_area = 1.0;
    double coeffi_vol = 1.0;
    MeasureMan::GetInstance()->GetMeasureUnit( coeffi, unit, CM);
    MeasureMan::GetInstance()->GetMeasureUnit( coeffi_area, unit_area, CM2);
    MeasureMan::GetInstance()->GetMeasureUnit( coeffi_vol, unit_vol, CM3);

    if (((-0.0001<dist) && (dist<0.0001)) && ((-0.0001<vol) && (vol<0.0001)) && (inMeasure==true))
    {
        sprintf(buf, "%s= %3.2f%s", _("area"), area*coeffi_area, unit_area.c_str());
        result = buf; 
        UpdateResult(result, attr.cursorType, attr.curColor);
		return;
	}

	sprintf(buf, "%s= %3.2f%s\n%s= %3.2f%s\n%s= %3.2f%s", _("area"), area*coeffi_area, unit_area.c_str(), _("dist"), dist*coeffi, unit.c_str(),  _("vol"), vol*coeffi_vol, unit_vol.c_str());
	//sprintf(buf, "area= %3.2fcm²\ndist= %3.2fcm\nvol= %3.2fcm³", area, dist, vol);
	result = buf; 

	if (inMeasure == true)
	    UpdateResult(result, attr.cursorType, attr.curColor);
	else {
	    AddResult(result, attr.cursorType, attr.confirmColor);
	    PutResult();
	}
	return;
}

void UpdateMeasure::D2VolSpheroid(double radius, double vol, ResultAttr& attr, bool inMeasure)
{
    char buf[50];
    string unit;
    string unit_vol;
    double coeffi = 1.0;
    double coeffi_vol = 1.0;
    MeasureMan::GetInstance()->GetMeasureUnit( coeffi, unit, CM);
    MeasureMan::GetInstance()->GetMeasureUnit( coeffi_vol, unit_vol, CM3);

    sprintf(buf, "%s= %3.2f%s\n%s= %3.2f%s", _("radius"), radius*coeffi, unit.c_str(), _("vol"), vol*coeffi_vol, unit_vol.c_str());
    string result = buf; 

    if (inMeasure == true)
	    UpdateResult(result, attr.cursorType, attr.curColor);
	else {
	    AddResult(result, attr.cursorType, attr.confirmColor);
	    PutResult();
	}
	return;
}

void UpdateMeasure::D2Angle3Dot(double dist_left, double dist_right, double angle, ResultAttr& attr, bool inMeasure)
{
    char buf[256];
    string result;
    string unit;
    double coeffi = 1.0;
    MeasureMan::GetInstance()->GetMeasureUnit( coeffi, unit, CM);

    if (dist_right==INVALID_VAL && inMeasure==true)
	{
	    sprintf(buf, "%s= %3.2f%s",_("dist1"), dist_left*coeffi, unit.c_str());
	    result = buf; 
	    UpdateResult(result);
	    return;
	}

	sprintf(buf, "%s= %3.2f%s\n%s= %3.2f%s\n%s= %3.2f˚", _("dist1"), dist_left*coeffi, unit.c_str(), _("dist2"), dist_right*coeffi, unit.c_str(), _("angle"), angle);
	result = buf;

	if (inMeasure == true)
	    UpdateResult(result, attr.cursorType, attr.curColor);
	else {
	    AddResult(result, attr.cursorType, attr.confirmColor);
	    PutResult();
	}
	return;
}

void UpdateMeasure::D2Angle2Line(double angle, ResultAttr& attr, bool inMeasure)
{
	char buf[50];
	sprintf(buf, "%s= %3.2f˚", _("angle"), angle);
	string result = buf;
	if (inMeasure == true)
	    UpdateResult(result, attr.cursorType, attr.curColor);
	else {
	    AddResult(result, attr.cursorType, attr.confirmColor);
	    PutResult();
	}
	return; //for test
}

void UpdateMeasure::D2RatioDist(double dist1, double dist2, double ratio, ResultAttr& attr, bool inMeasure)
{
    char buf[256];
    string result;
    string unit;
    double coeffi = 1.0;
    MeasureMan::GetInstance()->GetMeasureUnit( coeffi, unit, CM);

    if (((-0.0001<dist2) && (dist2<0.0001)) && ((-0.0001<ratio) && (ratio<0.0001)) && (inMeasure==true))
	{
		sprintf(buf, "%s= %3.2f%s", _("dist1"), dist1*coeffi, unit.c_str());
		result = buf; 
		UpdateResult(result, attr.cursorType, attr.curColor);
        return;
    }
    if((int)ratio ==INVALID_VAL)
    {

        sprintf(buf, "%s= %3.2f%s\n%s= %3.2f%s\n%s %s", _("dist1"), dist1*coeffi, unit.c_str(), _("dist2"), dist2*coeffi, unit.c_str(), _("ratio"), _(" Invalid"));
    }
    else
    {
        sprintf(buf, "%s= %3.2f%s\n%s= %3.2f%s\n%s= %3.2f", _("dist1"), dist1*coeffi, unit.c_str(), _("dist2"), dist2*coeffi, unit.c_str(), _("ratio"), ratio);
    }
    result = buf; 

    if (inMeasure == true)
        UpdateResult(result, attr.cursorType, attr.curColor);
    else {
        AddResult(result, attr.cursorType, attr.confirmColor);
        PutResult();
    }

	return; //for test
}

void UpdateMeasure::D2RatioDistPeri(double dist, double perimeter, double ratio, ResultAttr& attr, bool inMeasure)
{
    char buf[256];
    string result;
    string unit;
    double coeffi = 1.0;
    MeasureMan::GetInstance()->GetMeasureUnit( coeffi, unit, CM);
    
    if (((-0.0001<perimeter) && (perimeter<0.0001)) && ((-0.0001<ratio) && (ratio<0.0001)) && (inMeasure==true))
	{
		sprintf(buf, "%s= %3.2f%s", _("dist"), dist*coeffi, unit.c_str());
		result = buf; 
		UpdateResult(result, attr.cursorType, attr.curColor);
        return;
    }
    if((int)ratio ==INVALID_VAL)
    {
        sprintf(buf, "%s= %3.2f%s\n%s= %3.2f%s\n%s %s", _("dist"), dist*coeffi, unit.c_str(), _("peri"), perimeter*coeffi, unit.c_str(), _("ratio"), _(" Invalid"));
    }
    else
    {
        sprintf(buf, "%s= %3.2f%s\n%s= %3.2f%s\n%s= %3.2f", _("dist"), dist*coeffi, unit.c_str(),  _("peri"), perimeter*coeffi, unit.c_str(), _("ratio"), ratio);
    }
    result = buf; 

	if (inMeasure == true)
	    UpdateResult(result, attr.cursorType, attr.curColor);
	else {
	    AddResult(result, attr.cursorType, attr.confirmColor);
	    PutResult();
	}

	return; //for test
}

void UpdateMeasure::D2AngustyDist(double dist1, double dist2, double angusty, ResultAttr& attr, bool inMeasure)
{
	char buf[256];
	string result;
    string unit;
    double coeffi = 1.0;
    MeasureMan::GetInstance()->GetMeasureUnit( coeffi, unit, CM);

	if (((-0.0001<dist2) && (dist2<0.0001)) && ((-0.0001<angusty) && (angusty<0.0001)) && (inMeasure==true))
	{
	    sprintf(buf, "%s= %3.2f%s", _("dist1"), dist1*coeffi, unit.c_str());
	    result = buf; 
	    UpdateResult(result, attr.cursorType, attr.curColor);
	    return;
	}


    if((int)angusty == INVALID_VAL)
    {
        sprintf(buf, "%s= %3.2f%s\n%s= %3.2f%s\n%s %s", _("dist1"),dist1*coeffi, unit.c_str(),  _("dist2"), dist2*coeffi, unit.c_str(), _("angusty"), _(" Invalid"));
    }
    else
    {
        sprintf(buf, "%s= %3.2f%s\n%s= %3.2f%s\n%s= %3.2f", _("dist1"),dist1*coeffi, unit.c_str(),  _("dist2"), dist2*coeffi, unit.c_str(), _("angusty"), angusty);
    }
    //sprintf(buf, "dist1= %3.2fcm\ndist2= %3.2fcm\nangusty= %3.2f", dist1, dist2, angusty);
    result = buf; 

	if (inMeasure == true)
	    UpdateResult(result, attr.cursorType, attr.curColor);
	else {
	    AddResult(result, attr.cursorType, attr.confirmColor);
	    PutResult();
	}

	return; //for test
}

void UpdateMeasure::D2RatioAreaTrack(double area1, double area2, double ratio, bool inMeasure)
{
	return;
}

void UpdateMeasure::D2RatioArea(double area1, double area2, double ratio, ResultAttr& attr, bool inMeasure)
{
	char buf[256];
	string result;

    string unit;
    double coeffi = 1.0;
    MeasureMan::GetInstance()->GetMeasureUnit( coeffi, unit, CM2);
	if (((-0.0001<area2) && (area2<0.0001)) && ((-0.0001<ratio) && (ratio<0.0001)) && (inMeasure==true))
	{
		sprintf(buf, "%s= %3.2f%s", _("area1"), area1*coeffi, unit.c_str());
		//sprintf(buf, "area1= %3.2fcm²", area1);
		result = buf; 
		UpdateResult(result, attr.cursorType, attr.curColor);
		return;
	}
    if((int)ratio ==INVALID_VAL)
    {
        sprintf(buf, "%s= %3.2f%s\n%s= %3.2f%s\n%s %s", _("area1"), area1*coeffi, unit.c_str(), _("area2"), area2*coeffi, unit.c_str(), _("ratio"), _(" Invalid"));
    }
    else
    {
        sprintf(buf, "%s= %3.2f%s\n%s= %3.2f%s\n%s= %3.2f", _("area1"), area1*coeffi, unit.c_str(), _("area2"), area2*coeffi, unit.c_str(), _("ratio"), ratio);
    }
    //sprintf(buf, "area1= %3.2fcm²\narea2= %3.2fcm²\nratio= %3.2f", _("area1"), area1, _("area2"), area2, _("ratio"), ratio);
	result = buf; 

	if (inMeasure == true)
	    UpdateResult(result, attr.cursorType, attr.curColor);
	else {
	    AddResult(result, attr.cursorType, attr.confirmColor);
	    PutResult();
	}

	return;
}

void UpdateMeasure::D2AngustyArea(double area1, double area2, double angusty, ResultAttr& attr, bool inMeasure)
{
	char buf[256];
	string result;

    string unit;
    double coeffi = 1.0;
    MeasureMan::GetInstance()->GetMeasureUnit( coeffi, unit, CM2);
	if (((-0.0001<area2) && (area2<0.001)) && ((-0.0001<angusty) && (angusty<0.001)) && (inMeasure==true))
	{
		//sprintf(buf, "area1= %3.2fcm²", area1);
		sprintf(buf, "%s= %3.2f%s", _("area1"), area1*coeffi, unit.c_str());
		result = buf; 
		UpdateResult(result, attr.cursorType, attr.curColor);
        return;
    }
    if((int)angusty == INVALID_VAL)
    {
        sprintf(buf, "%s= %3.2f%s\n%s= %3.2f%s\n%s %s", _("area1"), area1*coeffi, unit.c_str(), _("area2"), area2*coeffi, unit.c_str(),  _("angusty"), _(" Invalid"));
    }
    else
    {
        sprintf(buf, "%s= %3.2f%s\n%s= %3.2f%s\n%s= %3.2f", _("area1"), area1*coeffi, unit.c_str(), _("area2"), area2*coeffi, unit.c_str(), _("angusty"), angusty);
    }
    //sprintf(buf, "area1= %3.2fcm²\narea2= %3.2fcm²\nangusty= %3.2f", area1, area2, angusty);
    result = buf; 

	if (inMeasure == true)
	    UpdateResult(result, attr.cursorType, attr.curColor);
	else {
	    AddResult(result, attr.cursorType, attr.confirmColor);
	    PutResult();
	}

	return;
}

void UpdateMeasure::D2RatioVol(double vol1, double vol2, double ratio, ResultAttr& attr, bool inMeasure)
{
	char buf[256];
	string result;

    string unit;
    double coeffi = 1.0;
    MeasureMan::GetInstance()->GetMeasureUnit( coeffi, unit, CM3);
	if (((-0.0001<vol2) && (vol2<0.0001)) && ((-0.0001<ratio) && (ratio<0.0001)) && (inMeasure==true))
	{
		sprintf(buf, "%s= %3.2f%s", _("vol1"), vol1*coeffi, unit.c_str());
		result = buf; 
		UpdateResult(result, attr.cursorType, attr.curColor);
		return;
	}

    if((int)ratio ==INVALID_VAL)
    {
        sprintf(buf, "%s= %3.2f%s\n%s= %3.2f%s\n%s %s", _("vol1"), vol1*coeffi, unit.c_str(), _("vol2"), vol2*coeffi, unit.c_str(), _("ratio"), _(" Invalid"));
    }
    else
    {
        sprintf(buf, "%s= %3.2f%s\n%s= %3.2f%s\n%s= %3.2f", _("vol1"), vol1*coeffi, unit.c_str(), _("vol2"), vol2*coeffi, unit.c_str(), _("ratio"), ratio);
    }
    //sprintf(buf, "vol1= %3.2fcm³\nvol2= %3.2fcm³\nratio= %3.2f", vol1, vol2, ratio);
	result = buf; 

	if (inMeasure == true)
		UpdateResult(result, attr.cursorType, attr.curColor);
	else {
	    AddResult(result, attr.cursorType, attr.confirmColor);
	    PutResult();
	}

	return;
}

void UpdateMeasure::D2RatioAngle(double angle1, double angle2, double ratio, ResultAttr& attr, bool inMeasure)
{
	char buf[256];
	string result;

	if (((-0.0001<angle2) && (angle2<0.0001)) && ((-0.0001<ratio) && (ratio<0.0001)) && (inMeasure==true))
	{
		sprintf(buf, "%s= %3.2f˚", _("angle1"), angle1);
		result = buf; 
		UpdateResult(result, attr.cursorType, attr.curColor);
		return;
	}

    if((int)ratio ==INVALID_VAL)
    {
        sprintf(buf, "%s= %3.2f˚\n%s= %3.2f˚\n%s %s", _("angle1"), angle1, _("angle2"), angle2, _("ratio"), _(" Invalid"));
    }
    else
    {
        sprintf(buf, "%s= %3.2f˚\n%s= %3.2f˚\n%s= %3.2f", _("angle1"), angle1, _("angle2"), angle2, _("ratio"), ratio);
    }
    result = buf; 

	if (inMeasure == true)
	    UpdateResult(result, attr.cursorType, attr.curColor);
	else {
	    AddResult(result, attr.cursorType, attr.confirmColor);
	    PutResult();
	}

	return;
}

void UpdateMeasure::MEF(double lvidd, double lvids, double lvvd, double lvvs, double ef, ResultAttr& attr, bool inMeasure)
{
	char buf[256];
	string result;
    string unit;
    string unit_vol;
    double coeffi = 1.0;
    double coeffi_vol = 1.0;
    MeasureMan::GetInstance()->GetMeasureUnit( coeffi, unit, CM);
    MeasureMan::GetInstance()->GetMeasureUnit( coeffi_vol, unit_vol, CM3);

	if ((((unsigned int)lvids == INVALID_VAL)) && (inMeasure==true))
	{
		sprintf(buf, "%s= %3.2f%s\n%s= %3.2f%s", _("LVIDd"), lvidd*coeffi, unit.c_str(), _("EDV"), lvvd*coeffi_vol, unit_vol.c_str());
		result = buf; 
		UpdateResult(result, attr.cursorType, attr.curColor);
		return;
	}

	sprintf(buf, "%s= %3.2f%s\n%s=%3.2f%s\n%s= %3.2f%s\n%s= %3.2f%s\n%s= %3.2f%%", _("LVIDd"), lvidd*coeffi, unit.c_str(), _("LVODs"), lvids*coeffi, unit.c_str(), _("EDV"), lvvd*coeffi_vol, unit_vol.c_str(), _("ESV"), lvvs*coeffi_vol, unit_vol.c_str(), _("EF"), ef);
	result = buf; 

	if (inMeasure == true)
	    UpdateResult(result, attr.cursorType, attr.curColor);
	else {
	    AddResult(result, attr.cursorType, attr.confirmColor);
	    PutResult();
	}

	return;
}

///< M
// void UpdateMeasure::MTime(double time, bool inMeasure)
// {
// 	char buf[50];

// 	sprintf(buf, "time= %3.2fs", time);
// 	string result = buf;
// 	if (inMeasure == true)
// 		UpdateResult(result);
// 	else {
// 		AddResult(result);
// 		PutResult();
// 	}

// 	return;
// }

void UpdateMeasure::MVel(double vel, ResultAttr& attr, bool inMeasure)
{
	char buf[50];
    string unit;
    double coeffi = 1.0;
    MeasureMan::GetInstance()->GetMeasureUnit( coeffi, unit, CMS);


	sprintf(buf, "%s= %3.2f%s", _("vel"), vel*coeffi, unit.c_str());
	string result = buf;
	if (inMeasure == true)
	    UpdateResult(result, attr.cursorType, attr.curColor);
	else {
	    AddResult(result, attr.cursorType, attr.confirmColor);
	    PutResult();
	}

	return;   	
}
void UpdateMeasure::MDepth(double depth, ResultAttr& attr, bool inMeasure)
{
	char buf[50];
     string unit;
    double coeffi = 1.0;
    MeasureMan::GetInstance()->GetMeasureUnit( coeffi, unit, CM);

    sprintf(buf, "%s= %3.2f%s", _("dist"), depth*coeffi, unit.c_str());
	string result = buf;
	if (inMeasure == true)
	    UpdateResult(result, attr.cursorType, attr.curColor);
	else {
	    AddResult(result, attr.cursorType, attr.confirmColor);
	    PutResult();
	}

	return;
}

void UpdateMeasure::MSlope(double slope, ResultAttr& attr, bool inMeasure)
{
    char buf[50];
    string unit;
    double coeffi = 1.0;
    MeasureMan::GetInstance()->GetMeasureUnit( coeffi, unit, CMS);

    sprintf(buf, "%s= %3.2f%s", _("slope"), slope*coeffi, unit.c_str());
    string result = buf;
	if (inMeasure == true)
	    UpdateResult(result, attr.cursorType, attr.curColor);
	else {
	    AddResult(result, attr.cursorType, attr.confirmColor);
	    PutResult();
	}

	return;
}


///< D
// void UpdateMeasure::DVel(double vel, bool inMeasure)
// {
// 	char buf[50];
// 	sprintf(buf, "+ vel= %3.2fcm/s", vel);
// 	string result = buf;
// 	if (inMeasure == true)
// 		UpdateResult(result);
// 	else {
// 		AddResult(result);
// 		PutResult();
// 	}

// 	return;
// }

// void UpdateMeasure::DPSED(char *name, double ps, double ed, bool inMeasure)
// {
// 	char buf_tmp[50];
// 	char buf[256];
// 	string result;

// 	if (ed==INVALID_VAL && inMeasure==true)
// 	{
// 		if (*name == '\0')
// 			sprintf(buf, "+ PS= %3.2fcm/s", ps);
// 		else
// 		{
// 			sprintf(buf, "+ ");
// 			strcat(buf, name);
// 			strcat(buf, ": PS= ");
// 			sprintf(buf_tmp, "%3.2fcm/s", ps);
// 			strcat(buf, buf_tmp);

// 		}
// 		result = buf; 
// 		UpdateResult(result);
// 		return;
// 	}
// 	if (*name == '\0')
// 		sprintf(buf, "+ PS= %3.2fcm/s, ED= %3.2fcm/s", ps, ed);
// 	else
// 	{
// 		sprintf(buf, "+ ");
// 		strcat(buf, name);
// 		strcat(buf, ": ");
// 		sprintf(buf_tmp, "PS= %3.2fcm/s, ED= %3.2fcm/s", ps, ed);
// 		strcat(buf, buf_tmp);

// 	}
// 	result = buf; 

// 	if (inMeasure == true)
// 		UpdateResult(result);
// 	else {
// 		AddResult(result);
// 		PutResult();
// 	}
// 	return; //for test
// }


// void UpdateMeasure::DRI(char *name, double ps, double ed, double ri, bool inMeasure)
// {
// 	char buf_tmp[50];
// 	char buf[256];
// 	string result;

// 	if (ed==INVALID_VAL && inMeasure==true)
// 	{
// 		if (*name == '\0')
// 			sprintf(buf, "+ PS= %3.2fcm/s", ps);
// 		else
// 		{
// 			sprintf(buf, "+ ");
// 			strcat(buf, name);
// 			strcat(buf, ": PS= ");
// 			sprintf(buf_tmp, "%3.2fcm/s", ps);
// 			strcat(buf, buf_tmp);
// 		}
// 		result = buf; 
// 		UpdateResult(result);
// 		return;
// 	}
// 	if (*name == '\0')
// 		sprintf(buf, "+ PS= %3.2fcm/s, ED= %3.2fcm/s, RI= %3.2f", ps, ed, ri);
// 	else
// 	{
// 		sprintf(buf, "+ ");
// 		strcat(buf, name);
// 		strcat(buf, ": ");
// 		sprintf(buf_tmp, "PS= %3.2fcm/s, ED= %3.2fcm/s, RI= %3.2f", ps, ed, ri);
// 		strcat(buf, buf_tmp);
// 	}
// 	result = buf; 

// 	if (inMeasure == true)
// 		UpdateResult(result);
// 	else {
// 		AddResult(result);
// 		PutResult();
// 	}
// 	return; //for test
// }


void UpdateMeasure::DManual(double ps, double ed, double sd, double ri, double m_tamax, double m_pi, int m_hr, ResultAttr& attr, bool inMeasure)
{
    char buf[200];
    string result;
    string unit;
    double coeffi = 1.0;
    MeasureMan::GetInstance()->GetMeasureUnit( coeffi, unit, CMS);

    sprintf(buf, "%s= %3.2f%s\n%s= %3.2f%s\n%s= %3.2f\n%s= %3.2f\n%s= %3.2f%s\n%s= %3.2f\n%s=%dbpm", _("PS"), ps*coeffi, unit.c_str(), _("ED"), ed*coeffi, unit.c_str(), _("SD"), sd, _("RI"), ri, _("TAMAX"), m_tamax*coeffi, unit.c_str(), _("PI"), m_pi, _("HR"), m_hr);
	//sprintf(buf, "PS= %3.2fcm/s\nED= %3.2fcm/s\nS/D= %3.2f\nRI= %3.2f\nTAMAX= %3.2fcm/s\nPI= %3.2f\nHR=%dbpm", ps, ed, sd, ri, m_tamax, m_pi, m_hr);
	result = buf;

	if (inMeasure == true)
	    UpdateResult(result, attr.cursorType, attr.curColor);
	else
	{
	    AddResult(result, attr.cursorType, attr.confirmColor);
	    PutResult();
	}
	return; //for test
}

bool UpdateMeasure::DTrace(char buf[], ResultAttr& attr, bool inMeasure)
{
	string result;

	result = buf;
	if(result.size()==0)
		return false;

	if (inMeasure == true)
	    UpdateResult(result, attr.cursorType, attr.curColor);
	else
	{
	    AddResult(result, attr.cursorType, attr.confirmColor);
	    PutResult();
	}
	return true; 
}

bool UpdateMeasure::DTraceAdjust(char buf[], ResultAttr& attr, bool inMeasure)
{
    string result;

    result = buf;
    if(result.size()==0)
        return false;
    else
        ClearLast();

	if (inMeasure == true)
	    UpdateResult(result, attr.cursorType, attr.curColor);
	else
	{
	    AddResult(result, attr.cursorType, attr.confirmColor);
	    PutResult();
	}
    return true; 
}

void UpdateMeasure::DSD(double ps, double ed, double sd, ResultAttr& attr, bool inMeasure)
{
	char buf[256];
    string result;
    string unit;
    double coeffi = 1.0;
    MeasureMan::GetInstance()->GetMeasureUnit( coeffi, unit, CMS);

	if (ed==INVALID_VAL && inMeasure==true)
	{
		sprintf(buf, "%s= %3.2f%s", _("PS"), ps*coeffi, unit.c_str());
		result = buf; 
		UpdateResult(result, attr.cursorType, attr.curColor);
		return;
	}

	sprintf(buf, "%s= %3.2f%s\n%s= %3.2f%s\n%s= %3.2f", _("PS"), ps*coeffi, unit.c_str(), _("ED"), ed*coeffi, unit.c_str(), _("S/D"), sd);
	result = buf; 

	if (inMeasure == true)
	    UpdateResult(result, attr.cursorType, attr.curColor);
	else {
	    AddResult(result, attr.cursorType, attr.confirmColor);
	    PutResult();
	}
	return; //for test
}

// void UpdateMeasure::DRISD(char *name, double ps, double ed, double ri, double sd, bool inMeasure)
// {
// 	char buf_tmp[50];
// 	char buf[256];
// 	string result;

// 	if (ed==INVALID_VAL && inMeasure==true)
// 	{
// 		if (*name == '\0')
// 			sprintf(buf, "+ PS= %3.2fcm/s", ps);
// 		else
// 		{
// 			sprintf(buf, "+ ");
// 			strcat(buf, name);
// 			strcat(buf, ": PS= ");
// 			sprintf(buf_tmp, "%3.2fcm/s", ps);
// 			strcat(buf, buf_tmp);
// 		}
// 		result = buf; 
// 		UpdateResult(result);
// 		return;
// 	}
// 	if (*name == '\0')
// 		sprintf(buf, "+ PS= %3.2fcm/s, ED= %3.2fcm/s, RI= %3.2f", ps, ed, ri);
// 	else
// 	{
// 		sprintf(buf, "+ ");
// 		strcat(buf, name);
// 		strcat(buf, ": ");
// 		sprintf(buf_tmp, "PS= %3.2fcm/s, ED= %3.2fcm/s, RI= %3.2f, SD= %3.2f", ps, ed, ri, sd);
// 		strcat(buf, buf_tmp);
// 	}
// 	result = buf; 

// 	if (inMeasure == true)
// 		UpdateResult(result);
// 	else {
// 		AddResult(result);
// 		PutResult();
// 	}
// 	return; //for test
// }

// void UpdateMeasure::DTime(double time, bool inMeasure)
// {
// 	char buf[50];
// 	sprintf(buf, "+ time= %3.2fs", time);
// 	string result = buf;
// 	if (inMeasure == true)
// 		UpdateResult(result);
// 	else {
// 		AddResult(result);
// 		PutResult();
// 	}

// 	return;
// }

// void UpdateMeasure::DAccel(double accel, bool inMeasure)
// {
// 	char buf[50];
// 	sprintf(buf, "+ accel= %3.2fcm/s²", accel);
// 	string result = buf;
// 	if (inMeasure == true)
// 		UpdateResult(result);
// 	else {
// 		AddResult(result);
// 		PutResult();
// 	}

// 	return;
// }

void UpdateMeasure::DPI(double ps, double ed, double tamax, double pi, ResultAttr& attr, bool inMeasure)
{
    if(pi == INVALID_VAL)
        return;
    char buf[256];
    string unit;
    double coeffi = 1.0;
    MeasureMan::GetInstance()->GetMeasureUnit( coeffi, unit, CMS);

    sprintf(buf, "%s= %3.2f%s\n%s= %3.2f%s\n%s= %3.2f\n%s= %3.2f", _("PS"), ps*coeffi, unit.c_str(), _("ED"), ed*coeffi, unit.c_str(), _("TAMAX"), tamax, _("PI"), pi);
    string result = buf;
    if (inMeasure == true)
	UpdateResult(result, attr.cursorType, attr.curColor);
    else {
	AddResult(result, attr.cursorType, attr.confirmColor);
	PutResult();
    }

    return;
}

// void UpdateMeasure::DHR(double hr, bool inMeasure)
// {
// 	char buf[50];
// 	sprintf(buf, "+ HR= %3.2fbpm", hr);
// 	string result = buf;
// 	if (inMeasure == true)
// 		UpdateResult(result);
// 	else {
// 		AddResult(result);
// 		PutResult();
// 	}

// 	return;
// }

// void UpdateMeasure::DPGmax(const char *name, double vel, double pg, bool inMeasure)
// {
// 	char buf[256];

// 	sprintf(buf, "+ ");
// 	strcat(buf, name);
// 	strcat(buf, ": ");
// 	sprintf(buf, "Vel= %3.2fcm/s, PGmax= %3.2fPa", vel, pg);
// 	string result = buf;
// 	if (inMeasure == true)
// 		UpdateResult(result);
// 	else {
// 		AddResult(result);
// 		PutResult();
// 	}

// 	return;
// }

void UpdateMeasure::DPGmean(double pg, ResultAttr& attr, bool inMeasure)
{
    char buf[50];
    sprintf(buf, "%s= %3.2fmmHg", _("PGmean"), pg);
    string result = buf;
    if (inMeasure == true)
	UpdateResult(result, attr.cursorType, attr.curColor);
    else {
	AddResult(result, attr.cursorType, attr.confirmColor);
	PutResult();
    }

    return;
}
void UpdateMeasure::IMTResult(SIMTPARA imt_para, ResultAttr& attr, bool update)
{
    int curType = MeasureFactory::GetInstance()->GetType();
    char buf[300]={0};
    double scale[4];
    Img2D::GetInstance()->GetAllScale2D(scale);
  switch(curType)
  {
      case D2_IMT_ABOVE :
          snprintf(buf, 300, "%s= %3.2fmm\n%s= %3.2fmm\n%s= %3.2fmm\n%s= %3.2fmm\n%s= %3.2f \n%s= %3.2fmm", 
                  _("IMT-Above MAX "), imt_para.imt_max*scale[0], _("IMT-Above MIN "), imt_para.imt_min*scale[0], _("IMT-Above STD "), imt_para.imt_std*scale[0], _("IMT-Above MEAN "), imt_para.imt_mean*scale[0],
                  _("IMT-Above QUALITY "), imt_para.imt_quality,  _("IMT-Above LENGTH "), imt_para.imt_length*scale[0]);
          break;
      case D2_IMT_BELOW :
          snprintf(buf, 300, "%s= %3.2fmm\n%s= %3.2fmm\n%s= %3.2fmm\n%s= %3.2fmm\n%s= %3.2f \n%s= %3.2fmm", 
                  _("IMT-Below MAX "), imt_para.imt_max*scale[0], _("IMT-Below MIN "), imt_para.imt_min*scale[0], _("IMT-Below STD "), imt_para.imt_std*scale[0], _("IMT-Below MEAN "), imt_para.imt_mean*scale[0],
                  _("IMT-Below QUALITY "), imt_para.imt_quality,  _("IMT-Below LENGTH "), imt_para.imt_length*scale[0]);
          break;
  }
    string result = buf;
    if (update)
        UpdateResult(result, attr.cursorType, attr.curColor);
    else
    {    
        AddResult(result, attr.cursorType, attr.confirmColor);
        PutResult();
    }
    return;
}
