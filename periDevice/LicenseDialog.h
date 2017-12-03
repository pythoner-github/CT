/**
* @brief LicenseDialog.h : create a license dialog, offer the information of authorization to user.

* @author: hehao 
* @version: v1.0
* @data: 2012.10.25
*/

#ifndef LICENSEDIALOG_H
#define LICENSEDIALOG_H
#include <gtk/gtk.h>
#include "FakeXEvent.h"

class CLicenseDialog : FakeXEvent
{
	public:
		enum WarnType {NODEVICE, EXPIRE, NOAUTHORIZE};
		static CLicenseDialog *GetInstance();
		virtual ~CLicenseDialog(){}

		void Create(WarnType type, bool isInitial);

	private:
		CLicenseDialog();
		static void ButtonOkClicked(GtkButton *button, gpointer data);
		static void ButtonCloseClicked(GtkButton *button, gpointer data);
		void KeyEvent(unsigned char keyValue);

		static CLicenseDialog *m_ptrInstance;
		GtkWidget *m_window;
		GtkWidget *label_text;
		GtkWidget *button_ok;
		GtkWidget *button_close;
		bool m_isInitial;
		bool m_preCursor;
};
#endif //LICENSEDIALOG_H
