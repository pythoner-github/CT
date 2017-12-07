#ifndef MYLOG_H
#define MYLOG_H

#include <fstream>

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