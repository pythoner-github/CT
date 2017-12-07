#ifndef __RECEIVE_H__
#define __RECEIVE_H__
#include <pthread.h>

class CDSC;
class CReceive
{
	public:
		enum EScanMode { MODE_2D, MODE_CFM, MODE_M, MODE_PW, MODE_PWCFM };

		CReceive(CDSC **pDSC, pthread_mutex_t &mutex1, pthread_mutex_t &mutex2);
		~CReceive();
		void SetScanMode(EScanMode mode);
		void ReceiveData(const unsigned char *pData);
};

#endif //__RECEIVE_H__
