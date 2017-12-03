/**
* @brief EmpAuthorization.h: interface for the CEmpAuthorization class. It can decide that whether the ultrasound system is usable, and it  generate unique register key for each computer,
* and it can generate license file, and judge that whether the inputing key is authorised.
*
* @version: 1.0
* @author: hehao
* @date: 2014-09-11
*/

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
