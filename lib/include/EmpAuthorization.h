#ifndef __EMP_AUTHORIZATION_H__
#define __EMP_AUTHORIZATION_H__

#include <string>
using namespace std;

class KeyValueOpr;
class CEmpAuthorization
{
	public:
		enum { PERMANENTDAY = 3650 };
		static void Create(KeyValueOpr *keyInterface, string registerFileDir, int machineType);
		static void Destroy();
		static int GetRestTime(void); //in day

	protected:
		CEmpAuthorization(KeyValueOpr *keyInterface, string registerFileDir);
		virtual ~CEmpAuthorization();
		static CEmpAuthorization *m_pThis;

	private:
		CEmpAuthorization(const CEmpAuthorization &dsc);
		CEmpAuthorization & operator =(const CEmpAuthorization &dsc);
};
#endif //__EMP_AUTHORIZATION_H__
