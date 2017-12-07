// 加密狗授权保护功能,只适用于SafeNet公司的Sentinel HASP

#ifndef AUTHORIZE_H
#define AUTHORIZE_H
#include <gtk/gtk.h>

class CAuthorize
{
    public:
        static CAuthorize *GetInstance();
        virtual ~CAuthorize();
        void Excute(int argc, char *argv[]);
        void AddTimeout();

        /*
         * 函数功能：验证key的有效性
         * 参数：
         *    feature：功能ID
         *  返回值:
         *    0:授权   -1:未找到设备   -2：过期  -3：未授权
         */
        int CheckAuthorize(unsigned int feature);

    private:
        CAuthorize();
        static gboolean CallbackUKey(gpointer data);

        int m_timeoutIdUkey;
};

#endif //AUTHORIZE_H