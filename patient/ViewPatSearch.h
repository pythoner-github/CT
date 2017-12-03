#ifndef _VIEW_PAT_SEARCH_H_
#define _VIEW_PAT_SEARCH_H_

#include "FakeXEvent.h"
#include <string>
#include <vector>
#include "Database.h"

class ViewPatSearch:public FakeXEvent
{
public:
    ~ViewPatSearch();
    static ViewPatSearch* GetInstance();
    
    void CreateWindow(GtkWindow *parent, const std::vector<Database::NewPatSearchResult>& result);
    void DestroyWindow(void);
private:
    ViewPatSearch();
    static ViewPatSearch* m_ptrInstance;
    void KeyEvent(unsigned char KeyValue);
    GtkWidget* m_window;
    GtkTreeSelection *m_selected;

#ifdef VET
    enum {
	ID_COL,
	ANIMAL_COL,
        OWNER_COL,
        VARIETY_COL,
	GENDER_COL,
	BIRTHDATE_COL,
        AGE_COL,
	EXAMDATE_COL,
	EXAMID_COL,
	EXAMTYPE_COL,
	N_COLUMNS
    };
#else
    enum {
	ID_COL,
	NAME_COL,
	GENDER_COL,
	BIRTHDATE_COL,
        AGE_COL,
	EXAMDATE_COL,
	EXAMID_COL,
	EXAMTYPE_COL,
	N_COLUMNS
    };
#endif 
// signal handle
    gboolean WindowDeleteEvent(GtkWidget *widget, GdkEvent *event);
    void BtnOkClicked(GtkButton *button);
    void BtnExitClicked(GtkButton *button);
// signal connect
    static gboolean on_window_delete_event(GtkWidget *widget, GdkEvent *event, ViewPatSearch *data) { return data->WindowDeleteEvent(widget, event); }
    static void on_button_ok_clicked(GtkButton *button, ViewPatSearch *data) { data->BtnOkClicked(button); }
    static void on_button_exit_clicked(GtkButton *button, ViewPatSearch *data) { data->BtnExitClicked(button); }
};

#endif
