#ifndef MYLOG_H
#define MYLOG_H

#include <fstream>

/*
 * @brief: class SysLog provide a method to save info when system running. These info is loged 
 * in log file(log/sys.log). this class is easy to use. First get a pointer(ptr) pointed to 
 * instance  of this class. then use (*ptr) as std::cout
 *
 * note 1)the instance of this class only one, so you can not create an object of this class
 * note 2)this log can not use in mult-threads, but we can deal with this problem latter.
 *
 * @para: Want to change to system log file's name and path? Change the value of FILENAME.
 */
class SysLog:public std::ofstream
{
	public:
        ~SysLog();

		static class SysLog* Instance();
	
	private:
        SysLog();
		
		static class SysLog* m_ptrInstance;
		
		void Open();
		
		#define FILENAME "./res/log/sys.log"
};



#endif
