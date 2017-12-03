#ifndef CALC_TIME_H
#define CALC_TIME_H

#include <sys/time.h>

class CalcTime
{
	public:
		CalcTime();
		~CalcTime() {}

		void Begin();
		float End();

	private:
		struct timeval m_start;
		struct timeval m_end;
};
#endif
