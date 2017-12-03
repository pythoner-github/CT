#ifndef _USBLP_
#define _USBLP_

//#include <vector>
//#include <string>

#define REPORT_WIDTH 900	//打印报告图片的宽度（像素点数）
#define REPORT_HEIGHT 1300	//打印报告图片的高度（像素点数）
#define BYTES_PER_PIXEL	3	//每个像素点占用的字节数
#define REPORT_WIDTH_BYTES	(REPORT_WIDTH*BYTES_PER_PIXEL)	//打印报告图片的一行的字节数
#define PRINTDIR "./res/printer/"
//#define PRINTIMAGE "/tmp/report.bmp"

#define PRINTIMAGE STORE_PATH "/0/" "report"
//#define PRINTIMAGE "/home/yang/001.png"
#define PRINT_FONTPATH	"./res/printer/wqy-zenhei.ttf"

/*
打印报告各项的内容
*/
struct print_re_item{
	char *id;
	char *name;
//hlx 
#ifdef VET
	char *species;
	char *owner;
#endif

	char *check_num;
	char *sex;
    char *ageUnit;
	char *age;
	char *section;
	char *diag_date;
	char *hospital;
	char *diag_doctor;
	char *report_doctor;
	char *ul_idea;
	char *comment;
	char *image_data[2];//报告中图像的数据
//	vector<string> imageName;
};

/*************************************************************
功能:通过Pango获取字符串（在固定字体、大小、DPI的情况下）在打印的报告中的尺寸
const char *str:在打印报告中的内容
int *width:字符串宽度_像素为单位
int *height:字符串高度_像素为单位
注意：返回值PangoLayout需要通过g_object_unref释放
*************************************************************/
PangoLayout* GetStringSizeInReport(const char *str, int *width, int *height, int font_size);

/*************************************************************
功能:打印报告
struct print_re_item item:打印报告各项的内容
int size_x:图像宽度_像素为单位
int size_y:图像高度_像素为单位
unsigned char* printer_name: 打印机名称
*************************************************************/
//extern int print_report(struct print_re_item item, int size_x, int size_y, gint8 language);
extern int print_report(int size_x, int size_y, gint8 language);
extern void itoa(int data, char *string, unsigned char length);

#endif
