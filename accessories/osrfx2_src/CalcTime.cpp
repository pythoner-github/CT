#include <stdio.h>
#include "CalcTime.h"

/*
 * @brief begin statistic time
 */

CalcTime::CalcTime()
{
	m_start.tv_sec = 0;
	m_start.tv_usec = 0;
	
	m_end.tv_sec = 0;
	m_end.tv_usec = 0;
}

void CalcTime::Begin()
{
	gettimeofday(&m_start, NULL);
}

/*
 * @brief end statistic time, and return time used
 *
 * @retval time used
 */
float CalcTime::End()
{
	gettimeofday(&m_end, NULL);

	float timeUse;
	timeUse = (m_end.tv_sec - m_start.tv_sec)*1000000 + (m_end.tv_usec - m_start.tv_usec);
//	printf("Time use: %fus\n", timeUse);
	
	m_start = m_end;
	return timeUse;
}

