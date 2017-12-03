#ifndef NOTEAREA_H_
#define NOTEAREA_H_

#include "goocanvas.h"
#include "FakeXEvent.h"
#include "Def.h"

#include <string>

class ImageArea;
using std::string;

class NoteArea:public FakeXEvent
{
	public:
		~NoteArea();

		struct TextInfo
		{
			string str;
			gdouble x;
			gdouble y;
			guint size; //font size 
			string color; 
		};

		struct TextItem
		{
			TextInfo info;
			GooCanvasItem *item;
		};

		static NoteArea *GetInstance();
		GtkWidget * Create(void);
		void SetNewText(const char *text);
		void Enter();
		void Exit();
		void Show(void);
		void Hide(void);
		void StartEdit(gdouble x, gdouble y);
		void EndEdit(void);
		void Focus(void);
		void Undo();

	private:
		NoteArea();
		static NoteArea* m_ptrInstance;
		ImageArea* m_ptrImgArea;

		enum {NORMAL, EDITING, DRAGING, SELECT};

		GtkWidget* m_canvas;
		GtkWidget* m_entry;
		GooCanvasItem *m_itemImage;
		GooCanvasItem *m_itemEntry;
		GList* m_listItem; 
		GooCanvasItem *m_itemDrag;
		GooCanvasItem *m_itemSel;
		guint m_state;
		gdouble m_dragX;
		gdouble m_dragY;
		gdouble m_itemX;
		gdouble m_itemY;

		POINT m_sysCursor;
		POINT m_menuCursor;

		int m_sizeFont;
		string m_strColor;
		GdkColor *m_color;

		//void Undo(void);
		void StartDrag(GooCanvasItem *item);
		void EndDrag(GooCanvasItem *item);
		void SetupTextItemHandle(GooCanvasItem *item);
		void KeyEvent(unsigned char keyValue);
		void MouseEvent(char offsetX, char offsetY);
		void PressCursor(void);
		GooCanvasItem* AddTextItem(const char *text, gdouble x, gdouble y);
		void DeleteTextItem(GooCanvasItem *item);
		void DeleteFirstTextItem(void);
		void DeleteLastTextItem(void);
		void DeleteAllTextItem(void);
		void ListAllTextItem(GList *list);	//For Test
		GtkWidget* CreateEntry(PangoFontDescription *font, GdkColor *color);
		void GetNoteSetting();
        GdkCursor* CustomCursor(void);
		//signal handle
		gboolean ImageItemButtonPress(GooCanvasItem *item, GooCanvasItem *target_item, GdkEventButton *event);
		gboolean ImageItemKeyPress(GooCanvasItem *item, GooCanvasItem *target_item, GdkEventKey *event);

		gboolean TextItemButtonPress(GooCanvasItem *item, GooCanvasItem *target_item, GdkEventButton *event);
		gboolean TextItemMotionNotify(GooCanvasItem *item, GooCanvasItem *target_item, GdkEventMotion *event);
		gboolean TextItemEnterNotify(GooCanvasItem *item, GooCanvasItem *target_item, GdkEventCrossing *event);
		gboolean TextItemLeaveNotify(GooCanvasItem *item, GooCanvasItem *target_item, GdkEventCrossing *event);
	//	gboolean TextItemKeyPress(GooCanvasItem *item, GooCanvasItem *target_item, GdkEventKey *event);

		gboolean EntryButtonPress(GtkWidget *widget, GdkEventButton *event);
		void EntryChanged(GtkEntry *entry);
		void EntryPopup(GtkEntry *entry, GtkMenu *menu);
        void FirstCharInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);

		//signal connect
		static gboolean HandleImageItemButtonPress(GooCanvasItem *item,
									  GooCanvasItem *target_item,
									  GdkEventButton *event,
									  NoteArea *data)
		{ return data->ImageItemButtonPress(item, target_item, event); }
		static gboolean HandleImageItemKeyPress(GooCanvasItem *item,
									  GooCanvasItem *target_item,
									  GdkEventKey *event,
									  NoteArea *data)
		{ return data->ImageItemKeyPress(item, target_item, event); }

		static gboolean HandleTextItemButtonPress(GooCanvasItem *item,
									  GooCanvasItem *target_item,
									  GdkEventButton *event,
									  NoteArea *data)
		{ return data->TextItemButtonPress(item, target_item, event); }
		static gboolean HandleTextItemMotionNotify(GooCanvasItem *item,
									   GooCanvasItem *target_item,
									   GdkEventMotion *event,
									   NoteArea *data)
		{ return data->TextItemMotionNotify(item, target_item, event); }
		static gboolean HandleTextItemEnterNotify(GooCanvasItem *item,
									   GooCanvasItem *target_item,
									   GdkEventCrossing *event,
									   NoteArea *data)
		{ return data->TextItemEnterNotify(item, target_item, event); }
		static gboolean HandleTextItemLeaveNotify(GooCanvasItem *item,
									   GooCanvasItem *target_item,
									   GdkEventCrossing *event,
									   NoteArea *data)
		{ return data->TextItemLeaveNotify(item, target_item, event); }
#if 0
		static gboolean HandleTextItemKeyPress(GooCanvasItem *item,
									   GooCanvasItem *target_item,
									   GdkEventKey *event,
									   NoteArea *data)
		{ return data->TextItemKeyPress(item, target_item, event); }
#endif

        static void on_entry_first_char(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, NoteArea *data)
        { data->FirstCharInsert(editable, new_text, new_text_length, position); }

        static gboolean HandleEntryButtonPress(GtkWidget *widget, GdkEventButton *event, NoteArea *data)
        { return data->EntryButtonPress(widget, event); }
        static void HandleEntryChanged(GtkEntry *entry, NoteArea *data)
		{ data->EntryChanged(entry); }
		static void HandleEntryPopup(GtkEntry *entry, GtkMenu *menu, NoteArea *data)
		{ data->EntryPopup(entry, menu); }
};

#endif
