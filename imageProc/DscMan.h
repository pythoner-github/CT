#ifndef DSC_MAN_H
#define DSC_MAN_H

#include <pthread.h>
#include "DSCTypes.h"
#include "ImageArea.h"
#include "DSC.h"
#include "FpgaReceive.h"
#include "Receive.h"

class DscMan
{
	public:
		enum EDSCObj{B, BB, B4, M, PW, CFMB, CFMBB, CFMB4, PWCFM, CFMVS2D, MANATOMIC, EFOV };

		~DscMan();
		static DscMan* GetInstance();
		
		void SendDataToDsc(const unsigned char *pData);

		// DSC para
		UPDATEFRAME GetFrameUpdateFunFreeze();
		DSCCONTROLATTRIBUTES* GetDscPara();
		CDSC* GetDsc();

		// pthread comunication
		bool GetWriteLock(); 
		bool ReadWriteUnlock(); 		

		// replay resource lock
		bool GetReplayWriteLock();
		bool ReadWriteReplayUnlock();

		// DSC object
		CDSC* CreateDscObj(EDSCObj type);
	private:
		DscMan();

		static DscMan* m_ptrInstance;

		DSCCONTROLATTRIBUTES m_dscPara;
		CDSC* m_ptrDsc;
#ifdef EMP_355
        CReceive *m_receive;
#endif
		FpgaReceive m_fpgaReceive;

		//pthread
		bool m_lockIsCreate; // if or not the lock is create.
		pthread_mutex_t m_pthreadLock;
#ifdef EMP_355
		pthread_mutex_t m_pthreadLock1;
		pthread_mutex_t m_pthreadLock2;
#endif
		// replay 
		bool m_replayLockIsCreate; // if or not the lock is create.
		pthread_mutex_t m_replayLock;
		
		void InitDscPara();
};


///> func
inline UPDATEFRAME DscMan::GetFrameUpdateFunFreeze()
{
	return ImageArea::DrawImgDataFreeze;
}

inline DSCCONTROLATTRIBUTES* DscMan::GetDscPara()
{
	return &m_dscPara;
}

inline CDSC* DscMan::GetDsc()
{
	return m_ptrDsc;
}

/*
 * @brief used for commnication among multi-thread process
 */

inline bool DscMan::GetWriteLock()
{
#ifdef EMP_355
    pthread_mutex_lock(&m_pthreadLock1);
#endif
	if (m_lockIsCreate)
	{
		if (pthread_mutex_lock(&m_pthreadLock) == 0)
		{
			return TRUE;
		}
	}
	return FALSE;
}
inline bool DscMan::ReadWriteUnlock()
{
#ifdef EMP_355
    pthread_mutex_unlock(&m_pthreadLock1);
#endif
	if (m_lockIsCreate)
	{
		if (pthread_mutex_unlock(&m_pthreadLock) == 0)
		{
			return TRUE;
		}
	}
	return FALSE;
}

inline bool DscMan::GetReplayWriteLock()
{
	if (m_replayLockIsCreate)
	{
		if (pthread_mutex_lock(&m_replayLock) == 0)
		{
			return TRUE;
		}
	}
	return FALSE;
}
inline bool DscMan::ReadWriteReplayUnlock()
{
	if (m_replayLockIsCreate)
	{
		if (pthread_mutex_unlock(&m_replayLock) == 0)
		{
			return TRUE;
		}
	}
	return FALSE;
}

#endif
