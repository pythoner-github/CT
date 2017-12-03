#ifndef CALC_TIME_H
#define CALC_TIME_H

#include <sys/time.h>

class CalcTime
{
	public:
		CalcTime();
		~CalcTime() {}

		struct Time
		{
			int year;
			int mon;
			int day;
			int hour;
			int min;
			int sec;
		};

		void Begin();
		void Restart();
		float End();

		void GetCurrentTime(Time time);

	private:
		struct timeval m_start;
		struct timeval m_end;

		void Init();
};
#endif
