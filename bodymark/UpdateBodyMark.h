#ifndef UPDATE_BODYMARK_H
#define UPDATE_BODYMARK_H

#include <gtk/gtk.h>
#include "Def.h"
#include "display/MenuArea.h"

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