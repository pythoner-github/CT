#ifndef KEY_DEF_H
#define KEY_DEF_H

// key
#define KEY_UP 0x18
#define KEY_DOWN 0x19
#define KEY_LEFT 0x1B
#define KEY_RIGHT 0x1A
#define KEY_2D 0x80
#define KEY_M 0x83
#define KEY_B 0xEB
#define KEY_SET 0xBC
#define KEY_ESC 0x0E

#if defined(K24C)
#define KEY_DUAL 0x81 //双B
#define KEY_QUAD 0x82 //4B
#define KEY_DEPTH 0x85
#define KEY_LOCALZOOM 0xDA
#define KEY_FOCUS 0x8C
#define KEY_UPDATE 0xAB
#define KEY_PAGEUP 0xE0
#define KEY_PAGEDOWM 0xE7
#define KEY_CW 0xE6
#define KEY_PDI 0xD0
#define KEY_PW 0xD1
#define KEY_SAVE 0x91
#define KEY_ARCHIVE 0xD7
#define KEY_ENDEXAM 0xD8
#define KEY_CURSOR 0xD9
#define KEY_AUTO 0xD4
#define KEY_SHIFT_SPACE 0xED	//reserve
#define KEY_CFM 0xD2
#define KNOB_VALUE 0x0C
#define KEY_REVIEW 0xB8
#define KEY_LEFTRIGHT 0x89
#define KEY_UPDOWN 0x8A
#define KEY_P1 0xD5
#define KEY_P2 0xD6

#elif (defined(EMP_460) || defined(EMP_355))
#define KNOB_GAIN 0x0B
#define KEY_FOCUS 0xAB
#define KEY_DUAL 0x81
#define KEY_QUAD 0x82 //4B
#define KEY_F1ADD 0xB1
#define KEY_F1SUB 0xB2
#define KEY_F2ADD 0xB3
#define KEY_F2SUB 0xB4
#define KEY_F3ADD 0xB5
#define KEY_F3SUB 0xB7
#define KEY_F4ADD 0xC3
#define KEY_F4SUB 0xC4
#define KEY_F5ADD 0xC5
#define KEY_F5SUB 0xC6
#define KEY_MENU 0xB8
#define KEY_P3 0xC9
#define KEY_P1 0xC7
#define KEY_P2 0xC8
#define KEY_PAGEUP 0xE7
#define KEY_PAGEDOWM 0xE0
#define KEY_LOCALZOOM 0x8C
#define KEY_ARCHIVE 0xD7
#define KEY_ENDEXAM 0xD8
#define KEY_CURSOR 0XD9
#define KEY_UPDOWN 0x8A
#define KEY_LEFTRIGHT 0x89
#define KEY_SAVE 0x91
#define KEY_UPDATE 0xDA
#define KNOB_VALUE 0x0C     //Auto Knob
#define KEY_SHIFT_SPACE 0xDF//IM
#define KEY_CW  0xE6
#define KEY_PW   0xD1
#define KEY_PDI 0xD0
#define KEY_CFM 0xD2
#define KEY_DEPTHADD 0x85
#define KEY_DEPTHSUB 0x86
#define KEY_CTRL_D 0x8F
///////////////////monitor//////////
#define KEY_CONTRAST_ADD    0xEF
#define KEY_CONTRAST_SUB    0xDF
#define KEY_BRIGHT_ADD      0xBF
#define KEY_BRIGHT_SUB      0xCF
#define KEY_AUTOADJUST      0xF2
#define KEY_AUTOBALANCE     0xF1
#define KEY_AUTO 0xD6

#elif defined(EMP_322)
#define KEY_F6 0xE6
#define KEY_4D 0XD0
#define KEY_TSI 0xD1
#define KEY_FREQ 0xA8
#define KEY_CHROMA 0x8C
#define KEY_FOCUS 0xAB
#define KEY_DEPTHADD 0x86
#define KEY_DEPTHSUB 0x85
#define KEY_LOCALZOOM 0xB1
#define KEY_SAVE 0x8B
#define KEY_AUTO 0xB2
#define KEY_CURSOR 0xB0
#define KEY_REVIEW 0x8E
#define KEY_CTRL_SHIFT_SPACE 0xF0
#define KEY_SPACE 0x20
#define KNOB_GAIN 0xF6
#define KNOB_VALUE 0xF5
#define KEY_BB 0x81
#define KEY_BM 0x82
#define KEY_PAGEUP 0xE7
#define KEY_PAGEDOWM 0xE0
#define KEY_ARCHIVE 0x91
#define KEY_ENDEXAM 0xD7
#define KEY_FUNC 0xD2
#define KEY_UPDOWN 0x89
#define KEY_LEFTRIGHT 0x8A
//#define KEY_DEL 0xA0
#define KEY_CAP_LOCK 0xCE
#define KEY_P1 0xD5
#define KEY_P2 0xD6

#elif defined(EMP_313)
#define KEY_BB 0x81
#define KEY_BM 0x82
#define KEY_REVIEW 0x8E //读取
#define KEY_ARCHIVE 0xBD   //快存
#define KEY_ENDEXAM 0x91 //存储
//#define KEY_ENDEXAM 0xF9 //存储
#define KEY_SAVE 0xF9 //打印
//#define KEY_SAVE 0x91 //打印
#define KEY_CURSOR 0xB0 //Distance
#define KEY_LOCALZOOM 0x8C
#define KEY_DEPTH 0x85
#define KEY_FOCUS 0xAB
#define KEY_FREQ 0xA8
#define KEY_F6 0xE6
#define KEY_UPDOWN 0x8A
#define KEY_LEFTRIGHT 0x89
#define KNOB_VALUE 0xF5
#define KEY_PAGEUP 0xE7
#define KEY_PAGEDOWM 0xE0
#define KEY_SPACE 0x20 //写探头
#define KEY_ONE 0x97 //中英文切换
#define KEY_P1 0xD5
#define KEY_P2 0xD6

#else
#define KEY_CW 0xE6
#define KEY_PW 0xD1
#define KEY_PDI 0xD0
#define KEY_SAVE 0x91
#define KEY_ARCHIVE 0xD7
#define KEY_ENDEXAM 0xD8
#define KEY_CURSOR 0xD9
#define KEY_CFM 0xD2
#define KEY_BBL 0x81
#define KEY_BBR 0x82
#define KEY_DEPTHADD 0x85
#define KEY_DEPTHSUB 0xD3
#define KEY_LOCALZOOM 0x8C
#define KEY_UPDATE 0xDA
#define KEY_REVIEW 0xAB
#define KEY_PAGEUP 0xE7
#define KEY_PAGEDOWM 0xE0
#define KEY_SHIFT_SPACE 0xED	//reserve
#define KEY_AUTO 0xD4
#define KNOB_VALUE 0x0C
#define KEY_MENU 0xB8
#define KEY_LEFTRIGHT 0x8A
#define KEY_UPDOWN 0x89
#define KEY_P1 0xD5
#define KEY_P2 0xD6

#endif

#define KEY_INVERT 0xEC
#define KEY_MEASURE 0xAF
#define KEY_BODYMARK 0xAE
#define KEY_PATIENT 0xB9
#define KEY_REPORT 0xBA
#define KEY_CALC 0xBB
#define KEY_SYSTEM 0xBE
#define KEY_PROBE 0xB6
#define KEY_TEXT 0xA9
#define KEY_ARROW 0xAA
#define KEY_CLEAR 0x8D
#define KEY_FREEZE 0x84
#define KEY_POWER 0x01
#define KEY_F1 0xE1
#define KEY_F2 0xE2
#define KEY_F3 0xE3
#define KEY_F4 0xE4
#define KEY_F5 0xE5
//#define KEY_P1 0xD5
//#define KEY_P2 0xD6
#define KEY_PWRADD 0xDD
#define KEY_PWRSUB 0xDE
#define KEY_MBP 0xE9

#if (defined(EMP_460) || defined(EMP_355))
#define KEY_SHIFT_CTRL 0xF0 //CTRL_SPACE
#define KEY_VOLUMEADD 0xDC
#define KEY_VOLUMESUB 0xDB

#else
#define KEY_SHIFT_CTRL 0xDF	//IM
#define KEY_VOLUMEADD 0xDB
#define KEY_VOLUMESUB 0xDC

#endif
#define KEY_SHIFT_ALT 0xEA	//reserve
//#define KEY_SHIFT_SPACE 0xED	//reserve

// knob
#define KNOB_F1 0x01
#define KNOB_F2 0x02
#define KNOB_F3 0x03
#define KNOB_F4 0x04
#define KNOB_F5 0x05
#define KNOB_M 0x06
#define KNOB_CW 0x07
#define KNOB_PDI 0x08
#define KNOB_PW 0x09
#define KNOB_CFM 0x0A
#define KNOB_2D 0x0B
//#define KNOB_VALUE 0x0C

// slider
#define SLIDER_OFF 0xA1

// Sound
#define KEY_SOUND_ON 0x6E
#define KEY_SOUND_OFF 0x6F

//Dicom
#define KEY_D 0x64
#define KEY_I 0x69
#define KEY_C 0x63
#define KEY_O 0x6F
#define KEY_DICOM_M 0x6D
#define KEY_E 0x65

//Superuser

#define KEY_S 0x73
#define KEY_SUPER_E 0x65
#define KEY_SUPER_M 0x6D
#define KEY_SUPER_P 0x70
#define KEY_SUPER_R 0x72
#define KEY_SUPER_O 0x6F
#define KEY_SUPER_1 0x31
#define KEY_SUPER_9 0x39
#define KEY_SUPER_7 0x37

//test for lhm

#define KEY_TEST_1 0x2C
#define KEY_TEST_2 0x2E
#define KEY_TEST_3 0x3B
#define KEY_TEST_4 0x27
#endif