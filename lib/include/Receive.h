/**
* @brief Receive.h: interface for the CReceive class.
* CReceive is a class used to receive several image mode data from fpga. 
*
* @version: 1.0
* @author: hehao
* @date: 2016-04-07
*/

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
