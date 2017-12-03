#ifndef __CDBURN_H__
#define __CDBURN_H__

#include <string>
#include <gtk/gtk.h>

using std::string;

class CdBurn
{
	public:
		~CdBurn();
		static CdBurn* GetInstance();

		gboolean StartBurn(const char* listPath);

		//private callback
		gboolean MakeIsoCallback(GIOChannel *source, GIOCondition condition);
		gboolean BurnCDCallback(GIOChannel *source, GIOCondition condition);
		gboolean BurnDVDCallback(GIOChannel *source, GIOCondition condition);

	private:
		CdBurn();
		static CdBurn* m_ptrInstance;
		string m_listPath;
		gint m_cont;
		gint m_pidMkIso;
		gint m_pidBurn;

		void HintDialog(guint timeout, int type, const char *info);
		gboolean MakeIso();
		gboolean BurnCD(const char *device);
		gboolean BurnDVD(gboolean is_blank, const char *device);
};

#endif
