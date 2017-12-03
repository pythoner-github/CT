/************************************
 *
 *2009,深圳市恩普电子技术有限公司
 *
 *@file:	operator.cpp
 *@brief:	类operator的成员函数
 *
 *version:	V1.0
 *date:		2009-5-19
 *author:	sunxubin
 ***********************************/
#include "KeyValueOpr.h"
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <syslog.h>
#include <pthread.h>
#include <gtk/gtk.h>
#include "ScreenSaver.h"
#include "KeyDef.h"
#include "Img2D.h"

using namespace std;

KeyValueOpr g_keyInterface;

int s_fdcom;//串口设备号
static int s_enableMouse = 1;

#ifdef TRANSDUCER
volatile int Transducer=0;
#endif

KeyValueOpr::KeyValueOpr( )
{
	PRINTF("Creat the keyboard interface thread");
	WinOprStack.clear();
	m_mouseSpeed = 1;
}

void KeyValueOpr::ListLighten() //for debug
{
	vector<unsigned char>::iterator iter;
	for(iter = m_vecLighten.begin(); iter < m_vecLighten.end(); iter++)
	{
		PRINTF("Lighten: %d\n", *iter);
	}
//	printf("====\n");
}

/*****************************
 * @brief 检查灯点亮或熄灭
 * 
 * @para lightValue light value to be check
 * @retval on true-亮灯, false-灭灯
 * **************************/
bool KeyValueOpr::IsLighten(unsigned char lightValue)
{
	vector<unsigned char>::iterator iter = find(m_vecLighten.begin(), m_vecLighten.end(), lightValue);
	if(iter != m_vecLighten.end())
	{
		//printf("%d is Lighten\n", lightValue);
		return TRUE;
	}
	else
		return FALSE;
}

/*****************************
 * @brief 在容器中记录点亮的灯
 * **************************/
void KeyValueOpr::AddLighten(unsigned char lightValue)
{
//	printf("Add %d to vecotr\n", lightValue);
	vector<unsigned char>::iterator iter = find(m_vecLighten.begin(), m_vecLighten.end(), lightValue);
	if(iter == m_vecLighten.end())
		m_vecLighten.push_back(lightValue);
}

/*****************************
 * @brief 将熄灭的灯从容器中删除
 * **************************/
void KeyValueOpr::RemoveLighten(unsigned char lightValue)
{
//	printf("Remove %d from vecotr\n", lightValue);
	vector<unsigned char>::iterator iter = find(m_vecLighten.begin(), m_vecLighten.end(), lightValue);
	if(iter != m_vecLighten.end())
		m_vecLighten.erase(iter);
}

/*****************************
 * @brief 控制灯,点亮或熄灭
 * 
 * @para on true-亮灯, false-灭灯
 * @para lightValue light value to be control
 * **************************/
void KeyValueOpr::CtrlLight(bool on, unsigned char lightValue )
{
	unsigned char value;

	if (on)
	{
		AddLighten(lightValue);
		value = lightValue | 0x80;
	}
	else
	{
		RemoveLighten(lightValue);
		value = lightValue & 0x7F;

		if (lightValue == 12)
			value = 0x0c;
	}
    PRINTF("ligth value and value are :%d %d\n",lightValue,value);

    SendLightValue(value);
	usleep(20000);

}

/*****************************
 *lightValue:灯值
 * **************************/
void KeyValueOpr::SendLightValue( unsigned char lightValue )
{
	int valueLength = 1;
	PortSend( s_fdcom, &lightValue, valueLength );
}

void KeyValueOpr::Push( AbsKeyboardEvent * win_opr)
{
	WinOprStack.push_back( win_opr );
}

void KeyValueOpr::Pop( void )
{
    if (WinOprStack.size() > 1)
	WinOprStack.pop_back();
}

bool KeyValueOpr::Empty(void)
{
	return WinOprStack.empty();
}
int KeyValueOpr::Size(void)
{
	return WinOprStack.size();
}

/**************************
 *n: 第n个元素
 * ************************/
AbsKeyboardEvent * KeyValueOpr::GetElement( int n )
{
	int total = 0;

	total = WinOprStack.size();
	if( n > (total-1) )
	{
		PRINTF(" The element want to get is out of range!\n" );
		return NULL;
	}

	return WinOprStack[n];
}

void KeyValueOpr::SetMouseSpeed( int s )
{
	//PRINTF("%s: m_mouseSpeed = %d, s = %d\n", __FUNCTION__, m_mouseSpeed, s);
	if (s <= 0)
		m_mouseSpeed = 0;
	else if (s > 10)
		m_mouseSpeed = 10;
	else
		m_mouseSpeed = s;
	s_enableMouse = 1;
}

int KeyValueOpr::GetMouseSpeed(void)
{
	return m_mouseSpeed;
}

void KeyValueOpr::SendKeyValue( unsigned char *keyValue )
{
	int total;

	if (WinOprStack.empty())
	{
		PRINTF( "there is no window in stack!" );
        return;
    }
    total = WinOprStack.size();
    PRINTF( "** sendkeyvalue: 0x%x,0x%x,0x%x **\n", keyValue[0], keyValue[1],keyValue[2] );
    switch(keyValue[0])
    {
        ///接收到TGC发送给vector底窗口即主窗口
        case 0x07:		
            if ((keyValue[2]>=0xA1)&&(keyValue[2]<=0xA9))		
               {
#ifdef TRANSDUCER
                   WinOprStack[0]->SliderEvent(keyValue[1], keyValue[2]);
                   if(keyValue[2]==0xA9)
                   {
                      	Transducer=(int)(unsigned char)keyValue[1];
                        Img2D::GetInstance()->ChangeTransducer(Transducer);
                   }

#else					
 					WinOprStack[0]->SliderEvent(keyValue[1], keyValue[2]);
#endif
               }
            break;

            //接收到其他发送给vector顶窗口即当前活动窗口
        case 0x0:
            WinOprStack[total-1]->KeyEvent(keyValue[1]);
            break;

        case 0x39:
            WinOprStack[total-1]->KnobEvent(keyValue[1], keyValue[2]);
            break;

        case 0xF5:
        case 0xF6:
            WinOprStack[total-1]->KnobEvent(keyValue[0], keyValue[1]);
            //printf("send value is :%x--%x\n",keyValue[0],keyValue[1]);
            break;

        case 0x08:
        case 0x18:
        case 0x28:
        case 0x38:
            PRINTF("m_mouseSpeed = %d, s_enableMouse = %d\n", m_mouseSpeed, s_enableMouse);
            if (m_mouseSpeed == 0)
			{
				int x = ((int)(char)keyValue[1]) * 2;
				int y = ((int)(char)keyValue[2]) * 2;
				if (x > 127)
					x = 127;
				else if (x < -128)
					x = -128;
				if (y > 127)
					y = 127;
				else if (y < -128)
					y = -128;

#ifdef EMP_355
				WinOprStack[total-1]->MouseEvent(-x, -y);
#else
				WinOprStack[total-1]->MouseEvent(x, y);
#endif
			}
			else if (s_enableMouse == m_mouseSpeed)
            {
#ifdef EMP_355
				WinOprStack[total-1]->MouseEvent(-keyValue[1], -keyValue[2]);
#else
                WinOprStack[total-1]->MouseEvent(keyValue[1], keyValue[2]);
#endif
                s_enableMouse = 1;
            }
            else
            {
                s_enableMouse ++;
            }
            break;

        default:
            break;
    }
    return;
}

/***************************
 *interface: KeyValueOpr对象指针
 *fdcom:	 串口设备号
 * ***********************/
gboolean GetKeyValue(GIOChannel *source, GIOCondition condition, gpointer data)
{
	struct timeval timeout;
	int i = 0;
	int len = 0;
	int lenTotal = 0;
	unsigned char recvbuf[3];
	KeyValueOpr *pKeyInterface;
	
	timeout.tv_sec = 5;			// timeout = 5 second.
	timeout.tv_usec = 0;
	
	for( i=0; i<3; i++ )
	{
		recvbuf[i] = 0;
	}

	len = PortRecv(s_fdcom, recvbuf, 1, timeout);
	if(len == 0)
		return FALSE;

	ScreenSaver::GetInstance()->Reset();

	switch(recvbuf[0])
	{
	case 0x00:
    case 0xF5: //value键
    case 0xF6: //gain键
		lenTotal = 1;
		break;
	case 0x07: //tgc
		lenTotal = 2;
		break;
	case 0x39: //G70 旋钮键
		lenTotal = 2;
		break;
	case 0x08:
	case 0x18:
	case 0x28:
	case 0x38:
		lenTotal = 2;
		break;
	}

	unsigned char *pBuf = recvbuf+1;
	while(lenTotal)
	{
		len = PortRecv(s_fdcom, pBuf, lenTotal, timeout);//接收键值
		lenTotal -= len;
		pBuf += len;
	}

    PRINTF( "** getkeyvalue: 0x%x,0x%x,0x%x **\n",recvbuf[0], recvbuf[1],recvbuf[2] );
    pKeyInterface = (KeyValueOpr *)data;
    pKeyInterface->SendKeyValue( recvbuf );//发送键值

	return TRUE;
}

//设置键盘串口并对串口进行监视
void *KeyboardOversee( void *pKeyInterface, bool isHandShake)
{
	portinfo_t portinfo = { '0', 19200, '8', '0', '0', '0', '0', '0', '1', 0 };

	s_fdcom = PortOpen( &portinfo ); 
	if(s_fdcom < 0)
	{
		exit(EXIT_FAILURE);	
	}

	PRINTF("s_fdcom= %d\n", s_fdcom );

	PortSet( s_fdcom, &portinfo );//键盘串口设置
	PRINTF( "set serial port success" );

	if (isHandShake)
	{
		if( KbdHandShake(s_fdcom) )//键盘初始化握手
		{
			//exit(EXIT_FAILURE);
		}
	}

	//键盘监视
	UartOversee( s_fdcom, pKeyInterface );
	
	return NULL;
}

void KeyboardRequestTGC()
{
	unsigned char init_comm = 0x35;
	PortSend(s_fdcom, &init_comm, 1);
}

void KeyboardSound(bool value)
{
    unsigned char soundValue;
    if (value)
	soundValue = KEY_SOUND_ON;
    else 
	soundValue = KEY_SOUND_OFF;
	
    PortSend(s_fdcom, &soundValue, 1);
}

