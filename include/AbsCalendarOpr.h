#ifndef ABS_CALENDAR_H_
#define ABS_CALENDAR_H_

class AbsCalendarOpr
{
	public: 

	virtual ~AbsCalendarOpr() {}
	virtual void SetStartDate(int year, int month, int day) {}
	virtual void SetEndDate(int year, int month, int day) {}
};
#endif
