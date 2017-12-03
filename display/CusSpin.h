#ifndef __CUS_SPIN__
#define __CUS_SPIN__

#include <gtk/gtk.h>
#include "Def.h"

class CusSpin
{
	public:
		CusSpin();
		~CusSpin();

		typedef void (*CusSpinFunc)(EKnobOper opr);

		struct CusSpinItem{
			const char *name;
			const char *val;
			// unsigned int width;
			// unsigned int height;
			EKnobReturn status;
			CusSpinFunc func;
		};

		GtkWidget* Create();
		void SetItem(CusSpinItem* item) { m_item = item; }
		void SetValue(const char *str, EKnobReturn flag);
		void UpdateLabel(void);
		void Show();
		
	private:
		GtkWidget* m_cusspin;
		GtkWidget* m_entry;
		GtkWidget* m_labelAdd;
		GtkWidget* m_labelSub;
		GtkWidget* m_btnAdd;
		GtkWidget* m_btnSub;
		GtkWidget *m_labelText;

		CusSpinItem* m_item;

		static void HandleBtnAdd(GtkButton *button, CusSpin *data) { data->m_item->func(ADD);}
		static void HandleBtnSub(GtkButton *button, CusSpin *data) { data->m_item->func(SUB); }
};

#endif
