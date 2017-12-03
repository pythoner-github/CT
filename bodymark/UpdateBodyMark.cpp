/*
 * 2009, 深圳恩普电子技术有限公司
 *
 * @file: UpdateBodyMark.cpp
 * @brief: update view for body mark
 *
 * version: V1.0
 * date: 2009-10-15
 * @author: zhanglei
 */

#include "UpdateBodyMark.h"
#include  "ImageArea.h"
#include  "ViewMain.h"

void CUpdateBodyMark::DrawBodyMark(POINT pos, GdkPixbuf* pixbuf, double scale, int colorIndex, guint index) 
{
	ImageArea::GetInstance()->DrawBodyMark(pos.x + IMAGE_X, pos.y + IMAGE_Y, pixbuf, scale, colorIndex, index);
}
void CUpdateBodyMark::DrawProbeMark(POINT pos, int direction, int maxDir, double scale, int colorIndex, guint index) 
{
	ImageArea::GetInstance()->DrawProbeMark(pos.x, pos.y, direction, maxDir, scale, colorIndex, index);
}
void CUpdateBodyMark::EnterBodyMark()
{
	MenuArea::GetInstance()->ShowBDMKMenu();
}
void CUpdateBodyMark::ExitBodyMark()
{
	MenuShowUndo();
}
void CUpdateBodyMark::HideBodyMark()
{
	ImageArea::GetInstance()->HideBodyMark();
	ClearBodyMark(4);
}
void CUpdateBodyMark::ClearBodyMark(guint index)
{
	ImageArea::GetInstance()->ClearBodyMark(index);
}


