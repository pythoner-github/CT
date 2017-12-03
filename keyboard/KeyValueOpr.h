//*************operator.h

#ifndef __KEY_OPERATOR_H_
#define __KEY_OPERATOR_H_
#include "AbsUpdate2D.h"
#include <gtk/gtk.h>
#include "Def.h"
#include <vector>
#include "KeyBoard.h"
#include "AbsKeyboardEvent.h"

using std::vector;

/************************************
 *KeyValueOpr:		键盘接口类
 *.WinOptStack:		用于保存各个窗口对键盘的响应函数指针的vector
 *.SendLightValue: 	接收键盘灯值,并将灯值发送给键盘
 *.Push: 			在每个窗口建立时调用此函数将本窗口对键盘的响应函数指针压入vector
 *.Pop:				在每个窗口退出是调用此函数将本窗口对键盘的响应函数指针从vector撤销
 *.GetElement:		获得vector的第n个元素，此函数暂时没用
 *.SendKeyValue:	将接收到的键值发送给各个窗口
 ***********************************/

class KeyValueOpr
{
	public:
		KeyValueOpr( );

		void CtrlLight(bool on, unsigned char lightValue );
		void Push( AbsKeyboardEvent * win_opr );
		void Pop( );
		bool Empty();
		int Size();
		AbsKeyboardEvent * GetElement( int n );
		void SendKeyValue( unsigned char *keyValue );
		void SetMouseSpeed( int s); 
		int GetMouseSpeed(void);
		bool IsLighten(unsigned char lightValue);
		void ListLighten(); //for debug
	private:
		int m_mouseSpeed;
		vector<AbsKeyboardEvent *> WinOprStack; 
		void SendLightValue( unsigned char light_value );

		vector<unsigned char> m_vecLighten;
		void AddLighten(unsigned char lightValue);
		void RemoveLighten(unsigned char lightValue);
};

extern KeyValueOpr g_keyInterface;

//获取键值并发送给各个窗口
extern gboolean GetKeyValue(GIOChannel *source, GIOCondition condition, gpointer data);

//键盘监视
extern void *KeyboardOversee( void * pKeyInterface, bool isHandShake = true);

//请求获得键盘TGC值
void KeyboardRequestTGC(void);

//键盘声音控制
void KeyboardSound(bool value);

#endif //__OPERATOR_H_

