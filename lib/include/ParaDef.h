/***************************************************************
 * Name:      ParaDef.h
 * Purpose:   Code for definition of variances
 * Author:    ZCT (zct826@126.com)
 * Created:   2009-04-06
 * Copyright: ZCT
 * License:
 **************************************************************/

#include "WinDef.h"

#ifndef	PARADEF_H_INCLUDE
#define	PARADEF_H_INCLUDE

// 第一个字节：类别（能量，血流，伪彩）
// 第二个字节：子类别（亚洲）
#define COLORMAP_PDI_ASIA0		0x0000	// 能量颜色表类型0：亚洲
#define COLORMAP_PDI_ASIA1		0x0001	// 能量颜色表类型1：亚洲
#define COLORMAP_PDI_ASIA2		0x0002	// 能量颜色表类型2：亚洲
#define COLORMAP_PDI_ASIA3		0x0003	// 能量颜色表类型3：亚洲
#define COLORMAP_PDI_ASIA4		0x0004	// 能量颜色表类型4：亚洲
#define COLORMAP_PDI_ASIA5		0x0005	// 能量颜色表类型

#define COLORMAP_CFM_ASIA0		0x1000	// 血流颜色表类型0：亚洲
#define COLORMAP_CFM_ASIA1		0x1001	// 血流颜色表类型1：亚洲
#define COLORMAP_CFM_ASIA2		0x1002	// 血流颜色表类型2：亚洲
#define COLORMAP_CFM_ASIA3		0x1003	// 血流颜色表类型3：亚洲
#define COLORMAP_CFM_ASIA4		0x1004	// 血流颜色表类型4：亚洲
#define COLORMAP_CFM_ASIA5		0x1005	// 血流颜色表类型5：亚洲
#define COLORMAP_CFMVAR_ASIA0	0x11000	// 血流颜色表类型0：亚洲，含方差

#define GRAYMAP				0xFFFF	// 灰度表
#define SIEMENS_PURPLE			0xF001	// 西门子紫色
#define SIEMENS_HYACINTH		0xF002	// 西门子橘红色
#define SIEMENS_CYANINE			0xF0003	// 西门子兰青色
#define SIEMENS_KHAKI			0xF0004	// 西门子土黄色
#define ROTBRAUM				0xF0005	// 红棕色
#define HYACINTH				0xF0006	// 橘红色
#define BLOND					0xF0007	// 金黄色
#define SIXCOLOR				0xF0008	// 6色
#define EFOVCOLOR				0xF0009 // EFOV speed color


#define COLOR_MAX				255		// 颜色最大值
#define COLOR_MIN				0		// 颜色最小值
#define COLOR_NUM				4		// 颜色种类。4--含透明色
#define COLOR_GRADE				256		// 颜色表级数
#define VARIANCE_GRADE			7		// 方差级数


#define PI					3.1415926	// 圆周率
#define eps					0.000000001	// 精度


typedef struct tagRGBQUAD
{
	BYTE rgbBlue;		// 蓝色的亮度(值范围为0-255) 
	BYTE rgbGreen;		// 绿色的亮度(值范围为0-255) 
	BYTE rgbRed;		// 红色的亮度(值范围为0-255) 
	BYTE rgbReserved;	// 保留，必须为0
} RGBQUAD;


#endif 	// #ifndef PARADEF_H_INCLUDE
