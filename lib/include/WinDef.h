/***************************************************************
 * Name:      WinDef.h
 * Purpose:   Code for compatible between VC and GCC
 * Author:    ZCT (zct826@126.com)
 * Created:   2008-07-31
 * Copyright: ZCT
 * License:
 **************************************************************/

#ifndef WINSTYLE_H_INCLUDED
#define WINSTYLE_H_INCLUDED


#ifndef BYTE
#define BYTE        unsigned char
#endif

#ifndef DWORD
#define DWORD       unsigned int
#endif

#ifndef LONG
#define LONG        int
#endif

#ifndef UINT
#define UINT        unsigned int
#endif

#ifndef WORD
#define WORD        unsigned short int
#endif

#ifndef LPSTR
#define LPSTR       char*
#endif

#ifndef BOOL
#define BOOL        int
#endif

#ifndef INT 
#define INT         int
#endif


#ifndef	FALSE
#define FALSE		0
#endif
#ifndef	TRUE
#define TRUE		1
#endif

typedef void *HANDLE;



#endif // WINSTYLE_H_INCLUDED
