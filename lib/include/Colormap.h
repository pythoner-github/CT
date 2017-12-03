/***************************************************************
 * Name:      Colormap.h
 * Purpose:   Code for EMP-Color Doppler Ultrasound Colormap
 * Author:    ZCT (zct826@126.com)
 * Created:   2009-04-06
 * Copyright: ZCT
 * License:
 *
 * Note:
 * table[3][256]:BGR,0~255
 * base_line:0~255
 * type:ASIA0:
 * **************************************************************/
#ifndef  DLL_API
#define  DLL_API
#endif

#ifndef	COLORMAP_H_INCLUDE
#define	COLORMAP_H_INCLUDE

#include "WinDef.h"
#include "ParaDef.h"
//#include "pseudo-colormap.h"


#ifdef __cplusplus
extern "C" {
#endif


// 产生 PDI 颜色表
// return:
// TRUE: Success
// FALSE: Failure
DLL_API BOOL MakePDIColormap(
							 int table[COLOR_NUM][COLOR_GRADE], // output: PDI 颜色表
							 int type							// intput: 类型
							);

// 产生 CFM 颜色表：不含方差信息
// return:
// TRUE: Success
// FALSE: Failure
DLL_API BOOL MakeCFMColormap(
							 int table[COLOR_NUM][COLOR_GRADE], // output: CFM 颜色表
							 int base_line,						// input: 基线［0，255］
							 int type,							// input: 
							 bool reverse = false				// input: 颜色翻转。false：不翻转
							);


// 产生 CFM 颜色表：含方差信息
// return:
// TRUE: Success
// FALSE: Failure
DLL_API BOOL MakeCFMVarColormap(
								int table[COLOR_NUM][COLOR_GRADE][VARIANCE_GRADE],	// output: CFM 颜色表，7阶绿色变化
								int base_line,										// input: 基线［0，255］
								int type											// input: 类型
							   );


// 产生伪彩颜色表（含灰度）
// return：
// TRUE: Success
// FALSE: Failure
DLL_API BOOL MakePseudoColormap(
								int table[COLOR_NUM][COLOR_GRADE],	// output：伪彩颜色表（含灰度）
								int type							// input：类型
							   );

#ifdef __cplusplus
}
#endif

#endif	// #ifndef COLORMAP_H_INCLUDE
