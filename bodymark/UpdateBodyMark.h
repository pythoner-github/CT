/**
* @brief AbsUpdateBodyMark.h: interface for the CAbsUpdateBodyMark class.
* CAbsUpdateBodyMakr is an interface class, it only define a set of actions
* for drawing probe mar arrow and bodymark, don't implement.
*
* @version: 1.0
* @author:zhanglei
* @date: 2009-9-17
*/

#ifndef UPDATE_BODYMARK_H
#define UPDATE_BODYMARK_H

#include <gtk/gtk.h>
#include "Def.h"
#include  "MenuArea.h"

class CUpdateBodyMark
{
public:
	void DrawBodyMark(POINT pos, GdkPixbuf* pixbuf, double scale, int colorIndex, guint index);
	void DrawProbeMark(POINT pos, int direction, int maxDir, double scale, int colorIndex, guint index);
	static void EnterBodyMark();
	static void ExitBodyMark();
	void HideBodyMark();
	void ClearBodyMark(guint index);

private:
	static MenuArea::EMenuType m_menuType;
};


#endif //UPDATE_BODYMARK_H