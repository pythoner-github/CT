/**
* @brief DCMRegister.cpp: interface for the CDCMRegister class. It can generate unique register key for each computer,
* and it can generate license file, and judge that whether the inputing key is authorised.
*
* @version: 1.0
* @author: hehao
* @date: 2013-11-20
*/

#ifndef __DCM_REGISTER_H__
#define __DCM_REGISTER_H__

#include <string>
using namespace std;

class CDCMRegister
{
	public:
		static CDCMRegister * GetMe();
		static void Destroy();
		static void Create(string resFileDir);

		bool GenerateLicenseFile(string destFileDir);
		bool CheckAuthorize(string registerKey);
		bool IsAuthorize();

	protected:
		CDCMRegister(string resFileDir);
		virtual ~CDCMRegister();
		static CDCMRegister *m_pThis;

	private:
		CDCMRegister(const CDCMRegister &dsc);
		CDCMRegister & operator =(const CDCMRegister &dsc);
};

#endif //__DCM_REGISTER_H__
