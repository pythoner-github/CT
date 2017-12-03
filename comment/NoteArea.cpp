#include <stdlib.h>
#include <goocanvas.h>
#include <gdk/gdkkeysyms.h>
#include "Def.h"
#include "display/gui_func.h"
#include "display/gui_global.h"
#include "comment/NoteArea.h"
#include "comment/MenuNote.h"
#include "keyboard/KeyValueOpr.h"
#include "keyboard/KeyFunc.h"
#include "keyboard/KeyDef.h"
#include "ViewMain.h"
#include "sysMan/SysNoteSetting.h"
#include "display/ImageArea.h"
#include "display/KnobNone.h"

extern KeyValueOpr keyInterface;
NoteArea* NoteArea::m_ptrInstance = NULL;
extern unsigned char key_note;

NoteArea* NoteArea::GetInstance()
{
	if (m_ptrInstance == NULL)
		m_ptrInstance = new NoteArea;
	return m_ptrInstance;
}

NoteArea::NoteArea(void)
{
	m_ptrImgArea = ImageArea::GetInstance();
	m_canvas = NULL;
	m_entry = NULL;
	m_itemEntry = NULL;
	m_itemImage = NULL;
	m_listItem = NULL;
	m_itemDrag = NULL;
	m_itemSel = NULL;
	m_state = NORMAL;
	m_dragX = 0.0;
	m_dragY = 0.0;
	m_itemX = 0.0;
	m_itemY = 0.0;

	GetNoteSetting();
	PRINTF("end of NoteArea struct\n");
}

NoteArea::~NoteArea()
{
	if (m_ptrInstance != NULL)
		delete m_ptrInstance;
}

GtkWidget* NoteArea::CreateEntry(PangoFontDescription *font, GdkColor *color)
{
	GtkWidget *entry = gtk_entry_new();
	g_object_set(G_OBJECT(entry),
			"has-frame", FALSE,
			NULL);
	gtk_widget_set_size_request(entry, 10, -1);
	gtk_widget_modify_font(entry, font);
	gtk_widget_modify_base(entry, GTK_STATE_NORMAL, g_gray);
	gtk_widget_modify_text(entry, GTK_STATE_NORMAL, m_color);
//	g_signal_connect(G_OBJECT(entry), "button-press-event", G_CALLBACK(HandleEntryButtonPress), this);
	g_signal_connect(G_OBJECT(entry), "changed", G_CALLBACK(HandleEntryChanged), this);
//	g_signal_connect(G_OBJECT(entry), "populate-popup", G_CALLBACK(HandleEntryPopup), this);

	return entry;
}

void NoteArea::SetNewText(const char *text)
{
	int x = m_sysCursor.x - (IMG_AREA_X + IMAGE_X);
	int y = m_sysCursor.y - (IMG_AREA_Y + IMAGE_Y);
	int h;

	GetNoteSetting();

	char font_desc[100];
	sprintf(font_desc, "%s, %d", FONT_STRING, m_sizeFont);
	PRINTF("Font desc is '%s'\n", font_desc);
	PangoFontDescription *font = pango_font_description_from_string(font_desc);

	GtkWidget *entry = CreateEntry(font, m_color);
	PangoLayout *layout = gtk_widget_create_pango_layout(entry, NULL);
	pango_layout_get_pixel_size(layout, NULL, &h);
	g_object_unref(layout);
	PRINTF("text height = %d\n", h);
	if( y+h > IMAGE_H )
	{
		y = IMAGE_H - h - 3;
	}

	// add new text to m_sysCursor
	GooCanvasItem *item = AddTextItem(text, x, y);

	gtk_widget_destroy(entry);

#if 0
	GdkCursor *cursor = gdk_cursor_new(GDK_PENCIL);
	gdk_window_set_cursor(m_canvas->window, cursor);
	gdk_cursor_unref(cursor);
#endif

	// set system cusor
	SetSystemCursor(m_sysCursor.x, m_sysCursor.y);

	// enter drag state
	StartDrag(item);
}

GtkWidget* NoteArea::Create(void)
{
	m_canvas = goo_canvas_new ();
	gtk_widget_set_size_request (m_canvas, IMG_W, IMG_H);
	//	gtk_widget_set_can_focus (m_canvas, TRUE);
	goo_canvas_set_bounds (GOO_CANVAS (m_canvas), 0, 0, IMG_W, IMG_H);

	return m_canvas;
}

void NoteArea::SetupTextItemHandle(GooCanvasItem *item)
{
	g_signal_connect(G_OBJECT(item), "button-press-event", G_CALLBACK(HandleTextItemButtonPress), this);
	g_signal_connect(G_OBJECT(item), "motion-notify-event", G_CALLBACK(HandleTextItemMotionNotify), this);
	g_signal_connect(G_OBJECT(item), "enter-notify-event", G_CALLBACK(HandleTextItemEnterNotify), this);
	g_signal_connect(G_OBJECT(item), "leave-notify-event", G_CALLBACK(HandleTextItemLeaveNotify), this);
	//	g_signal_connect(G_OBJECT(item), "key-press-event", G_CALLBACK(HandleTextItemKeyPress), this);
	//	g_signal_connect(G_OBJECT(item), "button-release-event", G_CALLBACK(HandleItemButtonRelease), this);
}

#if 0
void Test()
{
	GooCanvasItem *root = goo_canvas_get_root_item (GOO_CANVAS (m_canvas));
	GtkWidget *entry = gtk_entry_new ();
	GooCanvasItem *item = goo_canvas_widget_new (root, entry, 200, 50, 100, 20, NULL);

	m_listItem = g_list_append(m_listItem, item);
}
#endif

void NoteArea::Enter()
{
	if(!GTK_WIDGET_VISIBLE(m_canvas))
		Show();

    g_keyInterface.Push(this);

    GdkCursor *cursor = CustomCursor();
    //GdkCursor *cursor = gdk_cursor_new(GDK_XTERM);
	gdk_window_set_cursor(m_canvas->window, cursor);
	gdk_cursor_unref(cursor);

	// set system cusor
	m_sysCursor.x = IMG_AREA_X+IMAGE_X+IMG_W/2;
	m_sysCursor.y = IMG_AREA_Y+IMAGE_Y+IMG_H/2;
	SetSystemCursor(m_sysCursor.x, m_sysCursor.y);

	// show MenuNote
	MenuArea::GetInstance()->ShowNoteMenu();

	// Hide KnobMenu
	KnobNoneCreate();
}

void NoteArea::Exit()
{
	if(m_state == EDITING)
	{
		Undo();
	}
	else if(m_state == DRAGING)
	{
		if(m_itemDrag)
			EndDrag(m_itemDrag);
	}
	m_state = NORMAL;

	g_keyInterface.Pop();

	// restore cursor type
	GdkCursor *cursor = gdk_cursor_new(GDK_LEFT_PTR);
	gdk_window_set_cursor(m_canvas->window, cursor);
	gdk_cursor_unref(cursor);

	// restore Multi-func status
	MultiFuncUndo();

	// restore previous menu
	MenuShowUndo();
	KnobUndo();
}

void NoteArea::KeyEvent(unsigned char keyValue)
{
	//	FakeXEvent::KeyEvent(keyValue);
#if defined (EMP_322)
    if(keyValue==KEY_CTRL_SHIFT_SPACE)
    {
        key_note = keyValue;
        KeySwitchIM ksim;
        ksim.ExcuteChange(TRUE);
        return;
    }
#elif defined(EMP_313)
    if (keyValue == KEY_ONE)
    {
        key_note = keyValue;
        KeySwitchIM ksim;
        ksim.ExcuteChange(TRUE);
        return;
    }
#else
    if(keyValue==KEY_SHIFT_CTRL)
    {
        key_note = keyValue;
        KeySwitchIM ksim;
        ksim.ExcuteChange(TRUE);
        return;
    }
#endif
	if(FakeMouseButton(keyValue))
		return;
	if(FakeNumKey(keyValue))//数字键
		return;
	if(FakeAlphabet(keyValue))//字母键
		return;
	if(FakePunctuation(keyValue)) //符号键
		return;

	switch(keyValue)
	{
#if not defined (K24C)
#if not defined (EMP_322)
#if not defined (EMP_313)
		case KEY_MENU:
			break;
#endif
#endif
#endif
		case KEY_ESC:
			Undo();
			break;

		case KEY_TEXT:
			Exit();
			break;

		case KEY_CURSOR:
            {
                PressCursor();
            }
			break;

		case KEY_CLEAR:
			{
				if(m_state == NORMAL)
				{
					DeleteAllTextItem();

					Hide();
					KeyClearScreen kcs;
					kcs.Execute();
					gdk_threads_enter();
                    while(gtk_events_pending())
                        gtk_main_iteration();
                    gdk_threads_leave();
                    Show();
                    GdkCursor *cursor = CustomCursor();
                    //GdkCursor *cursor = gdk_cursor_new(GDK_XTERM);
                    gdk_window_set_cursor(m_canvas->window, cursor);
                    gdk_cursor_unref(cursor);

                }
            }
            break;

		case KEY_SHIFT_ALT:
		case KEY_F1:
		case KEY_F2:
		case KEY_F3:
		case KEY_F4:
		case KEY_F5:
            break;
#if defined(EMP_322)
        //case KEY_DEL:
        //    DeleteLastTextItem();
        //    ListAllTextItem(m_listItem);
        //    break;

        case KEY_CTRL_SHIFT_SPACE:
            return;
            break;
        case KEY_CAP_LOCK:
			return;
			break;
		case KEY_F6:
			break;
#elif defined(EMP_313)
		case KEY_F6:
			break;
#else
		case KEY_SHIFT_SPACE:
            break;
#endif

#ifdef EMP_355
        case KEY_F1ADD:
        case KEY_F1SUB:
        case KEY_F2ADD:
        case KEY_F2SUB:
        case KEY_F3ADD:
        case KEY_F3SUB:
        case KEY_F4ADD:
        case KEY_F4SUB:
        case KEY_F5ADD:
        case KEY_F5SUB:
            break;
#endif
		default:
			Exit();
			ViewMain::GetInstance()->KeyEvent(keyValue);
			break;
	}
}

void NoteArea::MouseEvent(char offsetX, char offsetY)
{
	if (m_state == SELECT)
	{
		fakeXMotionEventMenu(m_menuCursor.x, m_menuCursor.y, offsetX, offsetY);
	}
	else
	{
		fakeXMotionEventImage(m_sysCursor.x, m_sysCursor.y, offsetX, offsetY);

        if(m_state == EDITING)
        {
            EndEdit();
        }
    }
}

void NoteArea::PressCursor(void)
{
    if (m_state == NORMAL)
	{
		m_state = SELECT;
		m_menuCursor.x = SYSCURSOR_X;
		m_menuCursor.y = SYSCUROSR_Y;
		SetSystemCursor(m_menuCursor.x, m_menuCursor.y);
		g_menuNote.Focus();
	}
	else if(m_state == SELECT)
	{
		m_state = NORMAL;
		SetSystemCursor(m_sysCursor.x, m_sysCursor.y);
		Focus();
	}
}

void NoteArea::Show(void)
{
	GdkPixbuf *pixbuf = NULL;
	GooCanvasItem *root = goo_canvas_get_root_item (GOO_CANVAS (m_canvas));

	pixbuf = m_ptrImgArea->GetDrawingAreaImage();
	m_itemImage = goo_canvas_image_new (root, pixbuf, 0, 0, NULL);
	Focus();
	g_signal_connect (G_OBJECT(m_itemImage), "key-press-event", G_CALLBACK(HandleImageItemKeyPress), this);
	//g_signal_connect (G_OBJECT(m_itemImage), "button-press-event", G_CALLBACK(HandleImageItemButtonPress), this);

	gtk_widget_show (m_canvas);
	if(G_IS_OBJECT(pixbuf))
		g_object_unref (pixbuf);
}

void NoteArea::Hide(void)
{
	//Draw string image area
#if 1
	TextInfo info;
	GdkColor *color;
	if(g_list_length(m_listItem) > 0)
	{
		GList *list = g_list_first(m_listItem);
		while(list)
		{
			info = (TextInfo)((TextItem*)list->data)->info;
			if(strcmp(info.color.c_str(), "red") == 0)
				color = g_red;
			else if(strcmp(info.color.c_str(), "lime") == 0)
				color = g_green;
			else if(strcmp(info.color.c_str(), "blue") == 0)
				color = g_blue;
			else if(strcmp(info.color.c_str(), "white") == 0)
				color = g_white;
			else
				color = g_white;
			ImageArea::GetInstance()->DrawNoteText(info.str.c_str(), info.x-1, info.y-1, color, info.size);
			list = list->next;
		}
	}
#endif

	DeleteAllTextItem();

	if(G_IS_OBJECT(m_itemEntry))
		goo_canvas_item_remove(m_itemEntry);
	m_itemEntry = NULL;

	if(G_IS_OBJECT(m_itemImage))
		goo_canvas_item_remove(m_itemImage);
	m_itemImage = NULL;

	gtk_widget_hide(m_canvas);
}

void NoteArea::Undo(void)
{
    if(m_state == NORMAL)
    {
        if(m_itemSel != NULL)
        {
            DeleteTextItem(m_itemSel);
            m_itemSel = NULL;

            GdkCursor* cursor = CustomCursor();

            //GdkCursor *cursor = gdk_cursor_new(GDK_XTERM);
            gdk_window_set_cursor(m_canvas->window, cursor);
            gdk_cursor_unref(cursor);
        }
        else
        {
            //List为空时退出注释功能
            if(g_list_length(m_listItem) == 0)
                Exit();

            DeleteLastTextItem();
            //DeleteFirstTextItem();
        }
    }
    else if(m_state == EDITING)
    {

        if(G_IS_OBJECT(m_itemEntry))
            goo_canvas_item_remove(m_itemEntry);
        m_itemEntry = NULL;

		if(G_IS_OBJECT(m_entry))
			gtk_widget_destroy(m_entry);
		m_entry = NULL;

		m_state = NORMAL;
		Focus();
	}
}

void NoteArea::ListAllTextItem(GList *list)
{
	if(g_list_length(list) > 0)
	{
		PRINTF("length = %d\n", g_list_length(list));
		list = g_list_first(list);
		while(list)
		{
			PRINTF("string=%s\n", ((TextInfo)((TextItem*)list->data)->info).str.c_str());
			list = list->next;
		}
	}
	else
		PRINTF("List is NULL\n");
}

void NoteArea::DeleteAllTextItem(void)
{
	if(g_list_length(m_listItem) > 0)
	{
		m_listItem = g_list_first(m_listItem);
		while(m_listItem)
		{
			goo_canvas_item_remove((GooCanvasItem*)((TextItem*)m_listItem->data)->item);
			delete (TextInfo*)(&(((TextItem*)m_listItem->data)->info));
			m_listItem = g_list_remove(m_listItem, (TextItem*)m_listItem->data);
		}
	}
	//	ListAllTextItem(m_listItem);
}

void NoteArea::DeleteFirstTextItem(void)
{
	GList *list = g_list_first(m_listItem);
	if(list)
	{
		goo_canvas_item_remove((GooCanvasItem*)((TextItem*)list->data)->item);
		delete (TextInfo*)&((((TextItem*)list->data)->info));
		m_listItem = g_list_remove(m_listItem, (TextItem*)list->data);
		//	ListAllTextItem(m_listItem);
	}
}

void NoteArea::DeleteLastTextItem(void)
{
	GList *list = g_list_last(m_listItem);
	if(list)
	{
		goo_canvas_item_remove((GooCanvasItem*)((TextItem*)list->data)->item);
		delete (TextInfo*)&((((TextItem*)list->data)->info));
		m_listItem = g_list_remove(m_listItem, (TextItem*)list->data);
		//	ListAllTextItem(m_listItem);
	}
}

void NoteArea::DeleteTextItem(GooCanvasItem *item)
{
	if(g_list_length(m_listItem) > 0)
	{
		GList *list = g_list_first(m_listItem);
		while(list)
		{
			if( (GooCanvasItem*)((TextItem*)list->data)->item == item )
			{
				goo_canvas_item_remove(item);
				delete (TextInfo*)&((((TextItem*)list->data)->info));
				m_listItem = g_list_remove(m_listItem, (TextItem*)list->data);
				//	ListAllTextItem(m_listItem);
				break;
			}
			list = list->next;
		}
	}
}

void NoteArea::GetNoteSetting()
{
	SysNoteSetting sns;

	switch(sns.GetFontSize())
	{
		case 0:	m_sizeFont = 32;	break;
		case 1:	m_sizeFont = 18;	break;
		case 2:	m_sizeFont = 12;	break;
		default:	m_sizeFont = 18;		break;
	}
	PRINTF("font_size = %d\n", m_sizeFont);

	switch(sns.GetFontColor())
	{
		case 0:
			m_color = g_red;
			m_strColor = "red";
			break;
		case 1:
			m_color = g_green;
			m_strColor = "lime";
			break;
		case 2:
			m_color = g_blue;
			m_strColor = "blue";
			break;
		case 3:
			m_color = g_white;
			m_strColor = "white";
			break;
		default:
			m_color = g_red;
			m_strColor = "red";
			break;
	}
	PRINTF("m_strColor = %s\n", m_strColor.c_str());
}

GooCanvasItem* NoteArea::AddTextItem(const char *text, gdouble x, gdouble y)
{
	char font_desc[100];

	GetNoteSetting();

	sprintf(font_desc, "%s, %d", FONT_STRING, m_sizeFont);
	PRINTF("Font desc is '%s'\n", font_desc);
	PangoFontDescription *font = pango_font_description_from_string(font_desc);
	GooCanvasItem *root = goo_canvas_get_root_item(GOO_CANVAS(m_canvas));

	GooCanvasItem *item = goo_canvas_text_new (root, text, x, y, -1,
			GTK_ANCHOR_NW,
			"font-desc", font,
			"fill-color", m_strColor.c_str(),
			NULL);

	SetupTextItemHandle(item);

	TextItem *ti = new TextItem;

	ti->info.str = text;
	ti->info.x = x;
	ti->info.y = y;
	ti->info.size = m_sizeFont;
	ti->info.color = m_strColor;
	ti->item = item;
	m_listItem = g_list_append(m_listItem, ti);

	return item;
}

void NoteArea::FirstCharInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position)
{
    g_signal_emit_by_name((gpointer)editable, "insert_text", new_text, new_text_length, position);
    return ;
}

/*
 * @brief handle show a entry on current cursor position
 */
gboolean NoteArea::ImageItemButtonPress(GooCanvasItem *item,
		GooCanvasItem *target_item,
		GdkEventButton *event)
{
	if(event->button == 1)	//left button
	{
		if(m_state == NORMAL)
		{
			StartEdit(event->x, event->y);
			return TRUE;
		}
		else if(m_state == EDITING)
		{
			EndEdit();
			return TRUE;
		}
	}

	return FALSE;
}

gboolean time_out(gpointer data)
{
#if defined(EMP_322)
    if (key_note == KEY_CTRL_SHIFT_SPACE)
#elif defined(EMP_313)
    if (key_note == KEY_ONE)
#else
    if (key_note == KEY_SHIFT_CTRL)
#endif
    {
        KeySwitchIM ksim;
        ksim.ExcuteChange(TRUE);
        return FALSE;
    }

    if(!FakeAlphabet(key_note))
    {
        if(!FakeNumKey(key_note))
        {
            FakePunctuation(key_note);//符号键
        }
    }
    return FALSE;
}

/*
 * @brief handle the key event
 */
gboolean NoteArea::ImageItemKeyPress(GooCanvasItem *item,
        GooCanvasItem *target_item,
        GdkEventKey *event)
{
    PRINTF("--------------------- keyval = %d, string = %s, name = %s\n", event->keyval, event->string, gdk_keyval_name(event->keyval));
    if(m_state == NORMAL)
    {
        PRINTF("111111111111111111111\n");
        {
            StartEdit(m_sysCursor.x-IMG_AREA_X-IMAGE_X, m_sysCursor.y-IMG_AREA_Y-IMAGE_Y-1);
#ifdef EMP_322
            /* if(!FakeAlphabet(key_note))
               {
               if(!FakeNumKey(key_note))
               {
               FakePunctuation(key_note);//符号键
               }
               }*/
#else
            g_timeout_add(200, time_out, NULL);
#endif
            gtk_editable_set_position(GTK_EDITABLE(m_entry), -1);
            return FALSE;
        }
    }

    return TRUE;
}

/*
 * @brief handle button press on GooCanvasItemText for drag/undrag item
 */
gboolean NoteArea::TextItemButtonPress(GooCanvasItem *item,
        GooCanvasItem *target_item,
        GdkEventButton *event)
{
    if (event->button == 1)	//left button
    {
#if 0
        //grab item
        if(event->type == GDK_2BUTTON_PRESS && m_state == NORMAL) //double clicked
        {
            m_itemDrag = item;
            m_dragX = event->x;
            m_dragY = event->y;

            g_object_get (G_OBJECT (item),
                    "x", &m_itemX,
                    "y", &m_itemY,
                    NULL);

            GdkCursor *cursor = gdk_cursor_new(GDK_HAND1);
            goo_canvas_pointer_grab (GOO_CANVAS(m_canvas), item,
                    GdkEventMask(GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK),
                    cursor, event->time);
            gdk_cursor_unref(cursor);

            m_state = DRAGING;
            return TRUE;
        }
        else if(event->type == GDK_BUTTON_PRESS)
#endif
            //grab item
            if(event->type == GDK_BUTTON_PRESS)
            {
                if(m_state == NORMAL)
                {
                    m_itemDrag = item;
                    m_dragX = event->x;
                    m_dragY = event->y;

                    g_object_get (G_OBJECT (item),
                            "x", &m_itemX,
                            "y", &m_itemY,
                            NULL);

                    GdkCursor *cursor = gdk_cursor_new(GDK_HAND1);
                    goo_canvas_pointer_grab (GOO_CANVAS(m_canvas), item,
                            GdkEventMask(GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK),
                            cursor, event->time);
                    gdk_cursor_unref(cursor);

                    m_state = DRAGING;
                    return TRUE;
                }
                //ungrab item
                else if(m_state == DRAGING)
                {
                    if (m_itemDrag == item && m_itemDrag != NULL)
                    {
                        goo_canvas_pointer_ungrab (GOO_CANVAS(m_canvas), m_itemDrag, event->time);
                        g_object_get (G_OBJECT (item),
                                "x", &m_itemX,
                                "y", &m_itemY,
                                NULL);

                        //change the item in GList
                        if(g_list_length(m_listItem) > 0)
                        {
                            GList *list = g_list_first(m_listItem);
                            while(list)
                            {
                                if(item == ((TextItem*)list->data)->item)
                                {
                                    gint pos = g_list_position(m_listItem, list);
									TextInfo pInfo = ((TextInfo)((TextItem*)list->data)->info);
									TextItem *ti = new TextItem;

									ti->info.str = pInfo.str;
									ti->info.x = m_itemX;
									ti->info.y = m_itemY;
									ti->info.size = pInfo.size;
									ti->info.color = pInfo.color;
									ti->item = item;

									delete (TextInfo*)&((((TextItem*)list->data)->info));
									m_listItem = g_list_remove(m_listItem, (TextItem*)list->data);
									m_listItem = g_list_insert(m_listItem, ti, pos);

									break;
								}
								list = list->next;
							}
						}

						m_itemDrag = NULL;
						m_state = NORMAL;
						Focus();
						return TRUE;
					}
				}
			}
	}
	else if(event->button == 3) //right button
	{
#if 0	//moved to undo
		//remove draged item
		if(event->type == GDK_BUTTON_PRESS)
		{
			if (m_itemDrag == item && m_itemDrag != NULL)
			{
				goo_canvas_pointer_ungrab (GOO_CANVAS(m_canvas), m_itemDrag, event->time);
				DeleteTextItem(m_itemDrag);
				m_itemDrag = NULL;
				m_state = NORMAL;

				PRINTF("change to pencil!\n");
				GdkCursor *cursor = gdk_cursor_new(GDK_XTERM);
				gdk_window_set_cursor(m_canvas->window, cursor);
				gdk_cursor_unref(cursor);
				return TRUE;
			}
		}
#endif
	}

	return FALSE;
}

/*
 * @brief handle move the draged GooCanvasItemText
 */
gboolean NoteArea::TextItemMotionNotify(GooCanvasItem *item,
		GooCanvasItem *target_item,
		GdkEventMotion *event)
{
	if (m_itemDrag == item && m_itemDrag != NULL)
	{
		gdouble rel_x = event->x - m_dragX;
		gdouble rel_y = event->y - m_dragY;

		g_object_set (G_OBJECT (item), "x", m_itemX + rel_x, "y", m_itemY + rel_y, NULL);

		return TRUE;
	}

	return FALSE;
}

/*
 * @brief handle change the cursor for hinting when cursor enter the GooCanvasItemText
 */
gboolean NoteArea::TextItemEnterNotify(GooCanvasItem *item,
		GooCanvasItem *target_item,
		GdkEventCrossing *event)
{
	GdkCursor *cursor = gdk_cursor_new(GDK_HAND2);
	gdk_window_set_cursor(m_canvas->window, cursor);
	gdk_cursor_unref(cursor);

	m_itemSel = item;
	return TRUE;
}

/*
 * @brief handle restore the cursor when cursor leave the GooCanvasItemText
 */
gboolean NoteArea::TextItemLeaveNotify(GooCanvasItem *item,
        GooCanvasItem *target_item,
        GdkEventCrossing *event)
{
   GdkCursor* cursor = CustomCursor();
    //GdkCursor *cursor = gdk_cursor_new(GDK_XTERM);
    gdk_window_set_cursor(m_canvas->window, cursor);
    gdk_cursor_unref(cursor);

    m_itemSel = NULL;
	return TRUE;

}

#if 0
/*
 * @bried handle remove the draged GooCanvasItemText when "Esc" pressed
 */
gboolean NoteArea::TextItemKeyPress(GooCanvasItem *item,
		GooCanvasItem *target_item,
		GdkEventKey *event)
{
	PRINTF("key press\n");
	if(m_state == DRAGING && event->keyval == GDK_Escape)
	{
		if (m_itemDrag == item && m_itemDrag != NULL)
		{
			goo_canvas_pointer_ungrab (GOO_CANVAS(m_canvas), m_itemDrag, event->time);
			goo_canvas_item_remove(m_itemDrag);
			m_listItem = g_list_remove(m_listItem, m_itemDrag);
			m_itemDrag = NULL;
			return TRUE;
		}
	}

	return FALSE;
}
#endif

void NoteArea::StartEdit(gdouble x, gdouble y)
{
	GetNoteSetting();

	char font_desc[100];
	sprintf(font_desc, "%s, %d", FONT_STRING, m_sizeFont);
	PRINTF("Font desc is '%s'\n", font_desc);
	PangoFontDescription *font = pango_font_description_from_string(font_desc);
	GooCanvasItem *root = goo_canvas_get_root_item(GOO_CANVAS(m_canvas));

	m_entry = CreateEntry(font, m_color);

	int h;
	PangoLayout *layout = gtk_widget_create_pango_layout(m_entry, NULL);
	pango_layout_get_pixel_size(layout, NULL, &h);
	g_object_unref(layout);
	PRINTF("text height = %d\n", h);

    if(m_sizeFont ==0)
    {
          if( double(y+h) > IMAGE_H)
        {
       y = IMAGE_H - h -20;
        }
    }
    else if(m_sizeFont == 1)
    {
        if( double(y+h) > IMAGE_H )
        {
            y = IMAGE_H - h - 1;
            //    y = IMAGE_H -h-3;
        }
    }
    else
    {
           if( double(y+h) > IMAGE_H )
        {
            y = IMAGE_H - h +20;

        }
    }
    m_itemEntry = goo_canvas_widget_new(root,
            m_entry,
            x, y,
            -1, -1,
            NULL);
    gtk_widget_grab_focus(m_entry);
    //gtk_widget_show(m_itemEntry);
    m_state = EDITING;
    //	goo_canvas_item_raise(m_itemEntry, m_itemImage);
}

void NoteArea::EndEdit()
{
	const gchar *str;
	gdouble x, y;

	if(!m_itemEntry || !m_entry)
		return;

	g_object_get(G_OBJECT(m_itemEntry), "x", &x, "y", &y, NULL);
	str = gtk_entry_get_text(GTK_ENTRY(m_entry));

	if(strlen(str) > 0)
	{
		AddTextItem(str, x-1, y+1);
        //	AddTextItem(str, x-1, y+3);	//向左下微调位置
	}

	if(G_IS_OBJECT(m_itemEntry))
		goo_canvas_item_remove(m_itemEntry);
	m_itemEntry = NULL;

	if(G_IS_OBJECT(m_entry))
		gtk_widget_destroy(m_entry);
	m_entry = NULL;

	m_state = NORMAL;
	Focus();
}

void NoteArea::StartDrag(GooCanvasItem *item)
{
	m_state = NORMAL;

	GdkEventButton *event = (GdkEventButton*)gdk_event_new(GDK_BUTTON_PRESS);
	event->type = GDK_BUTTON_PRESS;
	event->button = 1;	 //left button
	event->x = m_sysCursor.x - IMAGE_X - IMG_AREA_X + 0.5;
	event->y = m_sysCursor.y - IMAGE_Y - IMG_AREA_Y + 0.5;
	event->time = GDK_CURRENT_TIME;

	TextItemButtonPress(item, item, event);
	gdk_event_free((GdkEvent*)(event));
}

void NoteArea::EndDrag(GooCanvasItem *item)
{
	m_state = DRAGING;

	GdkEventButton *event = (GdkEventButton*)gdk_event_new(GDK_BUTTON_PRESS);
	event->type = GDK_BUTTON_PRESS;
	event->button = 1;	 //left button
	event->time = GDK_CURRENT_TIME;

	TextItemButtonPress(item, item, event);
	gdk_event_free((GdkEvent*)(event));
}

/*
 * @brief handle add entry item for input text
 */
gboolean NoteArea::EntryButtonPress(GtkWidget *widget, GdkEventButton *event)
{
if(event->type == GDK_BUTTON_PRESS)
	{
		if(event->button == 1)	//left button
		{
			EndEdit();
			return TRUE;
		}
	}

	return FALSE;
}

/*
 * @brief handle auto changed the size of entry item when the text changed
 */
void NoteArea::EntryChanged(GtkEntry *entry)
{
	int w;
	PangoLayout *layout = gtk_entry_get_layout(entry);
	pango_layout_get_pixel_size(layout, &w, NULL);
	g_object_set(G_OBJECT(m_itemEntry), "width", MAX(double(w), 10.0), NULL);
}

/*
 * @biref handle popup menu when right button pressed in entry
 */
void NoteArea::EntryPopup(GtkEntry *entry, GtkMenu *menu)
{
	g_signal_stop_emission_by_name (entry, "populate-popup");
}

void NoteArea::Focus(void)
{
	goo_canvas_grab_focus ((GooCanvas*)m_canvas, m_itemImage); //if not grab focus, cannot recive key-press-event
}

GdkCursor* NoteArea::CustomCursor(void)
{
    char logo_file[255];
    GetNoteSetting();
    if(strcmp(m_strColor.c_str(),"red")==0)
        sprintf(logo_file, "%s/%s", CFG_RES_PATH, "res/icon/red.png");
    else if(strcmp(m_strColor.c_str(),"blue")==0)
        sprintf(logo_file, "%s/%s", CFG_RES_PATH, "res/icon/blue.png");
    else if(strcmp(m_strColor.c_str(),"lime")==0)
        sprintf(logo_file, "%s/%s", CFG_RES_PATH, "res/icon/green.png");
    else
        sprintf(logo_file, "%s/%s", CFG_RES_PATH, "res/icon/white.png");

    GError *error = NULL;
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(logo_file, &error);
    int Width;
    int Height;

    if(m_sizeFont==32)
    {
        Width = gdk_pixbuf_get_width(pixbuf) *1.4;
        Height = gdk_pixbuf_get_height(pixbuf) * 1.5;

    }
    else if(m_sizeFont==12)
    {
       Width = gdk_pixbuf_get_width(pixbuf) *1.0;
        Height = gdk_pixbuf_get_height(pixbuf) * 0.6;
    }
    else
    {
        Width = gdk_pixbuf_get_width(pixbuf) *1.0;
        Height = gdk_pixbuf_get_height(pixbuf) * 0.9;

    }
    GdkPixbuf *pixbuf_scale = gdk_pixbuf_scale_simple(pixbuf, Width, Height, GDK_INTERP_NEAREST);

    GdkDisplay *display = gdk_display_get_default();

   GdkCursor* cursor = gdk_cursor_new_from_pixbuf(display, pixbuf_scale,2,2);
    return cursor;
}