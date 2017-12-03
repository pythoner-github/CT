//Application for usb laser printer
//	error = FT_New_Face(pFTLib, "/usr/X11R6/lib/X11/fonts/TTF/gbsn00lp.ttf", 0, &pFTFace);

#include <string.h>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <glib.h>
#include <fcntl.h>
#include <unistd.h>
#include <ft2build.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <gtk/gtk.h>
#include "Def.h"
#include "PatientInfo.h"
#include "SysGeneralSetting.h"
#include "Usblp.h"
#include "ViewReport.h"
#include "ImgMan.h"
#include "ImageArea.h"
#include "TopArea.h"
#include "ViewSystem.h"
#include <pango-1.0/pango/pangoft2.h>
#include FT_FREETYPE_H

#define IMAGE_WIDTH 400
#define IMAGE_HEIGHT 288
#define BITMAP_OFFSET 4
#define FONT_DOT_RATIO(font_size) font_size*5   
#define PRINT_START_X 10
#define PRINT_START_Y 10
#define A_BITMAP_LEN ((FONT_DOT_RATIO(font_size))*(FONT_DOT_RATIO(font_size))+10)// ((FONT_DOT_RATIO(font_size)+1)*(FONT_DOT_RATIO(font_size)+1)+10)
#define BMP_HEAD_LEN 54

//#define PRINT_FONTPATH	"./res/printer/wqy-zenhei.ttf"

#if 0	//使用pango之后此段FreeType的程序不需要了
int utf8_to_unicode(const char* chars, int len, unsigned short *result, int len_out)
{
	unsigned short uc=0;
	int need=0, i, p=0;

	for (i=0; i<len; i++) 
	{
		unsigned char ch = chars[i];
		if (need) 
		{
			if ( (ch&0xc0) == 0x80 ) 
			{
				uc = (uc << 6) | (ch & 0x3f);
				need--;
				if ( !need ) 
				{
					if(p >= (len_out -1))
						break;
					result[p++] = uc;
				}
			} 
			else 
			{
				result [p++] = 0xfffd;
				need = 0;
			}
		} 
		else 
		{
			if ( ch < 128 ) 
			{
				result[p++] = ch;
			} 
			else if ( (ch&0xe0) == 0xc0 ) 
			{
				uc = ch &0x1f;
				need = 1;
			} 
			else if ( (ch&0xf0) == 0xe0 ) 
			{
				uc = ch &0x0f;
				need = 2;
			}
		}
	}
	
	return p;
}

int cn_to_unicode_ubuntu(unsigned char *utf8_text, unsigned short *unicode_text)
{
	int len = 0;
	int utf8_len = 0;

	utf8_len = strlen((char *)utf8_text)*3/2;
    len = utf8_to_unicode( (char *)utf8_text, strlen((char *)utf8_text), unicode_text, utf8_len );

	return len;
}

int cn_to_unicode_puppy(unsigned char *gb_text, unsigned short *unicode_text)
{
	int len = 0;
	int i;
	int unicode_len = 18;
	gchar *utf8_text; 

	len = strlen((char *)gb_text);
	unicode_len = len;
	utf8_text = g_convert( (char *)gb_text, len, "UTF-8", "GB18030", NULL, NULL, NULL );

	unicode_len = strlen(utf8_text);
	len = strlen( (char *)utf8_text );
	len = utf8_to_unicode( utf8_text, strlen(utf8_text), unicode_text, unicode_len );
	i = wcslen((wchar_t*)unicode_text);
	g_free( utf8_text );

	return len;
}	

void unicode_to_bitmap(unsigned short *unicode_text, int len, unsigned char *bitmap_point, int font_size)
{
    int i, j, n;
    FT_Library pFTLib = NULL;
    FT_Face pFTFace = NULL;
    FT_Error error = 0;
    int glyph_index = 0;
    FT_GlyphSlot slot = 0;	
	int a_bitmap_len;

	//make a character into ImageBuffer
	error = FT_Init_FreeType(&pFTLib);
	if (error){
		pFTLib = 0;
		perror("Init library error!\n");
		return;
	}

	if(g_file_test(PRINT_FONTPATH, G_FILE_TEST_EXISTS))
	{
		error = FT_New_Face(pFTLib, PRINT_FONTPATH, 0, &pFTFace);
	}
	else
	{
		PRINTF("the fonts file is not exist!\n");
		return;
	}
	
	if(error){
		perror("font success or fail donot know!\n");
		return;
	}

	error = FT_Set_Char_Size(pFTFace, font_size<<6, font_size<<6, 300, 300);// RESOLVE, RESOLVE);
	if (error){
		perror("set dot size error!\n");
		return;
	}
	a_bitmap_len = A_BITMAP_LEN;
	
	for(n=0; n<len; n++){
		glyph_index = FT_Get_Char_Index(pFTFace,*(unicode_text+n));
		error = FT_Load_Glyph(pFTFace, glyph_index, FT_LOAD_DEFAULT);
		if (error){
			perror("load glyph error!\n");
			return;
		}
		error = FT_Render_Glyph(pFTFace->glyph, FT_RENDER_MODE_NORMAL);
		
		if(error){
			perror("render glyph error!\n");
			return;
		}
		else{
			slot = pFTFace->glyph;
				
			for (i=0; i<slot->bitmap.rows; i++){
				for (j=0; j<slot->bitmap.width; j++){
					*(bitmap_point+n*a_bitmap_len+i*FONT_DOT_RATIO(font_size)+j+BITMAP_OFFSET) = 
                                          ((255 - slot->bitmap.buffer[i*(slot->bitmap.width)+j])>220)?
						255 : (255 - slot->bitmap.buffer[i*(slot->bitmap.width)+j]);
				}
			}
					
			*(bitmap_point + n * a_bitmap_len) = slot->bitmap.width;
			*(bitmap_point + n * a_bitmap_len + 1) = slot->bitmap.rows;
			if ((*(unicode_text+n) == 19968)||(*(unicode_text+n)==45))
				*(bitmap_point + n * a_bitmap_len + 1) = slot->bitmap.rows + FONT_DOT_RATIO(font_size)/3;
			else if ((*(unicode_text+n)==8220)||(*(unicode_text+n)==8221)
                                ||(*(unicode_text+n)==8216)||(*(unicode_text+n)==8217))
				*(bitmap_point + n * a_bitmap_len + 1) = slot->bitmap.rows + FONT_DOT_RATIO(font_size)/2;
		}
		
	}
	
	FT_Done_Face(pFTFace);
	pFTFace = NULL;
	FT_Done_FreeType(pFTLib);
	pFTLib = NULL;
}

int bitmap_to_report_buffer(unsigned char *buffer, unsigned char *bitmap, int len, int font_size, int position_x, int position_y, int position_y_limit)
{
	int n, i, j, k = 0;
	int width, rows;
	int char_height = 0;//char_height also use as char width,if char width!=char height,cannot use as char width 
	int char_start_x, char_start_y;
	unsigned char *report_buffer = NULL;
	int end_y;
	int sign = 0;
	
	report_buffer = buffer;

	char_height = FONT_DOT_RATIO(font_size);
	char_start_x = position_x;
	rows = *(bitmap + 1);
	char_start_y = position_y + char_height - rows;

	for (n=0; n<len; n++){
		width = *(bitmap + n*A_BITMAP_LEN);
		rows = *(bitmap+ n*A_BITMAP_LEN + 1);
		
		char_start_y = position_y + k* char_height + char_height - rows;	

		if (char_start_y > position_y_limit)
		{
			sign = 1;
			break;
		}

		for (i=0; i<rows; i++)
			for (j=0; j<width; j++)
			{
				*(report_buffer + (char_start_y+i)*REPORT_WIDTH_BYTES + (char_start_x + j) * BYTES_PER_PIXEL) = *(bitmap + A_BITMAP_LEN*n + char_height*i+j + BITMAP_OFFSET);
				*(report_buffer + (char_start_y+i)*REPORT_WIDTH_BYTES + (char_start_x + j) * BYTES_PER_PIXEL + 1) = *(bitmap + A_BITMAP_LEN*n + char_height*i+j + BITMAP_OFFSET);
				*(report_buffer + (char_start_y+i)*REPORT_WIDTH_BYTES + (char_start_x + j) * BYTES_PER_PIXEL + 2) = *(bitmap + A_BITMAP_LEN*n + char_height*i+j + BITMAP_OFFSET);
			}
			
		char_start_x += width+2;
		if (char_start_x > (REPORT_WIDTH-30)){
			k++;
			char_start_x = PRINT_START_X;
		}
	}
	
	end_y = (sign == 0) ? (char_start_y + char_height) : char_start_y;
	return (end_y);	
}

void gb_to_report_buffer(unsigned char *buffer, char *gb_string, int font_size, int position_x, int position_y, int position_y_limit)
{
	unsigned char gb_text_tmp[1203];
	unsigned short unicode_text[1203];
	unsigned char *gb_text;
	unsigned char *gb_text_sign_point;
	unsigned char *report_buffer = NULL;
	unsigned char *bitmap_buffer = NULL;
	int len, gb_len;
	int i, j;
	int enter_num = 1;
	int position_x_tmp, position_y_tmp; 

	position_x_tmp = position_x;
	position_y_tmp = position_y;

	gb_text_sign_point = (unsigned char *)gb_string;
	gb_text = (unsigned char *)gb_string;
	report_buffer = buffer;

	gb_len = strlen(gb_string);

	if (gb_len > 950) //600 -- 3
		gb_len = 950;

	for (i=0; i<gb_len; i++)
	{
		if (*(gb_string + i) == '\n')
		{
			enter_num++;
			i++;
		}
	}
	
	for (j=0; j<enter_num; j++)
	{
		for (i=0; i<1203; i++)
		{
			gb_text_tmp[i] = '\0';
			unicode_text[i] = '\0';
		}

		for (i=0; i<gb_len; i++)
		{
			if ((*gb_text_sign_point) == '\n')
			{
				gb_text_sign_point += 1;
				gb_text_tmp[i] = '\0';
				break;
			}
			else
			{
				gb_text_tmp[i] = *gb_text_sign_point;
				gb_text_sign_point++;
			}

		}

		gb_text_tmp[950] = '\0';

		gb_text = gb_text_tmp;
		len = cn_to_unicode_ubuntu(gb_text, unicode_text);
		if (len>400)
		{
			len = 400;
			unicode_text[400] = '\0';
		}

		bitmap_buffer = (unsigned char *)malloc(A_BITMAP_LEN*len);
		for (i=0; i<A_BITMAP_LEN*len; i++){
			*(bitmap_buffer + i) = 255;
		}

		unicode_to_bitmap((unsigned short *)unicode_text, len, bitmap_buffer, font_size);

		position_y_tmp = bitmap_to_report_buffer(report_buffer, bitmap_buffer, len, font_size, position_x_tmp, position_y_tmp, position_y_limit);

		free(bitmap_buffer);
	}
}
#endif

void strings_link(unsigned char *string1, unsigned char *string2, unsigned char *string)
{
	int i;
	int len1, len2;
	

	len1 = strlen((char *)string1);
	for (i=0; i<len1; i++){
		*(string + i) = *(string1 + i);
	}
	*(string + len1) = 0x20;
	
	len2 = strlen((char *)string2);
	for (i=0; i<len2; i++){
		*(string + len1 + i + 1) = *(string2 + i);
	}
	
	*(string + len1 + len2 + 1) = '\0';
	*(string + len1 + len2 + 2) = '\0';	
}

void BitmapToReportBuffer(FT_Bitmap* bitmap, unsigned char *report_buffer, int x, int y)
{
	int i, j;
	int bitmapW, bitmapH;
	unsigned char *bmpOnBuffStart;//bmp在report_buffer的行的起始地址

	bitmapW = bitmap->pitch;
	bitmapH = bitmap->rows;
//	PRINTF("bitmapW= %d, bitmapH= %d, x= %d, y= %d\n", bitmapW, bitmapH, x, y);

	bmpOnBuffStart = report_buffer + y * REPORT_WIDTH * BYTES_PER_PIXEL;
	for (j=0; j<bitmapH; j++)
	{
		for (i=0; i<bitmapW; i++)
		{
			*(bmpOnBuffStart + (REPORT_WIDTH*j+x+i)*BYTES_PER_PIXEL) = ~(*(bitmap->buffer + bitmapW*j + i));
			*(bmpOnBuffStart + (REPORT_WIDTH*j+x+i)*BYTES_PER_PIXEL + 1) = ~(*(bitmap->buffer + bitmapW*j + i));
			*(bmpOnBuffStart + (REPORT_WIDTH*j+x+i)*BYTES_PER_PIXEL + 2) = ~(*(bitmap->buffer + bitmapW*j + i));
		}
	}
}

PangoLayout* GetStringSizeInReport(const char *str, int *width, int *height, int font_size)
{
	const int xdpi = 96;
	const int ydpi = 96;
	PangoFontMap *PFM = pango_ft2_font_map_new();
	pango_ft2_font_map_set_resolution(PANGO_FT2_FONT_MAP(PFM), xdpi, ydpi);
	PangoContext *context = pango_font_map_create_context(PFM);
	pango_context_set_base_gravity(context, PANGO_GRAVITY_SOUTH);
	pango_context_set_gravity_hint(context, PANGO_GRAVITY_HINT_NATURAL);
	pango_context_set_language(context, pango_language_get_default());
	pango_context_set_base_dir(context, PANGO_DIRECTION_LTR);

	PangoLayout *layout = pango_layout_new(context);
	pango_layout_set_text(layout, str, -1);
	pango_layout_set_auto_dir(layout, 1);
	pango_layout_set_ellipsize(layout, PANGO_ELLIPSIZE_NONE);
	pango_layout_set_justify(layout, 0);
	pango_layout_set_single_paragraph_mode(layout, 0);
	pango_layout_set_wrap(layout, PANGO_WRAP_WORD_CHAR);
	pango_layout_set_alignment(layout, PANGO_ALIGN_LEFT);
	pango_layout_set_width(layout, (REPORT_WIDTH-40*2)*PANGO_SCALE);

	char fontString[100];
	sprintf(fontString, "DejaVu Sans Mono, Book, %d\n", font_size);
	PangoFontDescription *des = pango_font_description_from_string(fontString);
	pango_layout_set_font_description(layout, des);
	pango_layout_set_markup (layout, str, -1);
	pango_font_description_free(des);

	pango_layout_get_pixel_size(layout, width, height);

	g_object_unref(PFM);
	g_object_unref(context);

	return layout;
}

int ReportItemToBitmap(const char *str, unsigned char *report_buffer, int x, int y, int font)
{
	int width, height;
	PangoLayout *layout = GetStringSizeInReport(str, &width, &height, font);
//	PRINTF("Bitmap width = %d, height = %d\n", width, height);

	FT_Bitmap* bitmap;
	bitmap = g_slice_new(FT_Bitmap);

	bitmap->width = width;
	bitmap->rows = height;
	bitmap->pitch = (bitmap->width+3) & ~3;
	guchar *graybitmap = (guchar*)g_malloc(bitmap->pitch*bitmap->rows);
	memset(graybitmap, 0x00, bitmap->pitch*bitmap->rows);
	bitmap->buffer = graybitmap;
	bitmap->num_grays = 256;
	bitmap->pixel_mode = ft_pixel_mode_grays;//FT_PIXEL_MODE_GRAY;

	pango_ft2_render_layout(bitmap, layout, 0, 0);
	BitmapToReportBuffer(bitmap, report_buffer, x, y);
	g_free(graybitmap);

	g_object_unref(layout);

	return height;
}

void report_to_bitmap(struct print_re_item Item, unsigned char *report_buffer, int size_x, int size_y, int lenOfLine, gint8 language)
{
	int i, j;
	char *string_buffer;
//	char *image_end[2];
	int head_x = 250, head_y = 20;
	int offset_x1, offset_x2, offset_x3, offset_x4, offset_x5, offset_x6, offset_xi[2];
	int offset_y1, offset_y2, offset_y3, offset_y4, offset_y5, offset_y6, offset_y7;
//	unsigned char upside_max_point, downside_max_point;
	char report_head[6][50] ={{"REPORT OF ULTRASOUND"},{"超声医学影像报告单"}, {"       Отчет"} ,{"RAPORT BADANIA USG"}, {"RAPPORT D'ECHOGRAPHIE"}, {"BERICHT DES ULTRASCHALLS"}};
	char hospital[6][100] = {{"Hospital: "},{"医院名称："}, {"Больница: "} ,{"Szpital: "}, {"Hôpital: "}, {"Krankenhaus: "}};
	char diag_date[6][50] = {{"Exam Date: "},{"诊断日期："}, {"Дата: "}, {"Data egzaminu: "}, {"Date exam.: "}, {"Untersuchungsdatum: "}};
	char id[6][50] = {{"ID: "},{"ID："}, {"ID: "}, {"ID: "}, {"ID: "}, {"ID: "}};
	char check_num[6][50] = {{"Exam ID: "},{"检查号："}, {"Exam ID: "}, {"ID badania: "}, {"ID exam.: "}, {"Untersuchungs-ID: "}};
//hlx
#ifdef VET
	char name[7][50] = {{"Animal Name : "},{"动物姓名 ：   "},{"Animal Name : "},{"Animal Name : "},{"Animal Name : "},{"Animal Name : "},{"Animal Name : "}};
	char owner[7][50] = {{"Owner : "}, {"主人 ：   "},{"Owner : "},{"Owner : "},{"Owner : "},{"Owner : "},{"Owner : "}};
	char species[7][50] = {{"Species : "}, {"物种 ：   "}, {"Species : "},{"Species : "},{"Species : "},{"Species : "},{"Species : "}};
#else
	char name[6][50] = {{"Name: "},{"姓名："}, {"Имя: "}, {"Nazwa:"}, {"Nom: "}, {"Name: "}};
#endif
	//char sex[6][50] = {{"Sex: "},{"性别："}, {"Пол: "}, {"Płeć:"}, {"Sexe: "}, {"Geschlecht: "}};
	char sex[6][50] = {{"Gender: "},{"性别："}, {"Пол: "}, {"Płeć:"}, {"Sexe: "}, {"Geschl: "}};
	char age[6][50] = {{"Age: "},{"年龄："}, {"Лет: "}, {"Wiek: "}, {"Age: "}, {"Age: "}};
	char section[6][50] = {{"Exam Type: "},{"检查项目: "}, {"Тип: "}, {"Typ badania: "}, {"Type exam.: "}, {"Unt.typ: "}};
	//char section[6][50] = {{"Section: "},{"科别："}, {"Отдел: "}, {"Sekcja: "}, {"Section: "}, {"Abschnitt: "}};
	char ul_idea[6][20] = {{"Descriptor: "},{"超声所见："}, {"Описание: "}, {"Opisujący: "}, {"Descripteur: "}, {"Beschreibung: "}};
	char comment[6][30] = {{"Comments: "},{"诊断意见："}, {"Комментарий: "}, {"komentarz: "}, {"Commentaire: "}, {}};
	char diag_doctor[6][50] = {{"Diagnostician: "},{"诊断医生："},  {"Диагност: "}, {"Diagnosta: "}, {"Vérifier docteur: "}, {"Untersuchungsarzt: "} };
	char report_doctor[6][50] = {{"Physician: "},{"主治医生："}, {"Терапевт: "}, {"Pracownik: "}, {"Médecin: "}, {"Arzt: "}};
		unsigned char bmp_head[BMP_HEAD_LEN]=
		{66,77,38,143,53,0,0,0,0,0,54,0,0,0,40,0,
		0,0,132,3,0,0,20,5,0,0,1,0,24,0,0,0,
		0,0,240,142,53,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0};

	offset_x1 = 0;
	offset_x2 = offset_x1 + 280;
    if (EN == language)
    {
        offset_x3 = offset_x1 + 180;
        offset_x4 = offset_x3 + 150;
        offset_x5 = offset_x4 + 250-20;
        offset_x6 = offset_x1 + 380;
    }
    else if(ZH == language)
    {
        offset_x3 = offset_x1 + 200;
        offset_x4 = offset_x3 + 200;
        offset_x5 = offset_x4 + 200;
        offset_x6 = offset_x1 + 380;
    }
    else if (RU == language)
    {
        offset_x3 = offset_x1 + 280;
        offset_x4 = offset_x3 + 100;
        offset_x5 = offset_x4 + 280;
        offset_x6 = offset_x1 + 380;
    }
    else if (PL == language)
    {
        offset_x3 = offset_x1 + 170+30;
        offset_x4 = offset_x3 + 110;
        offset_x5 = offset_x4 + 250-40;
        offset_x6 = offset_x1 + 380;
    }
    else if (FR == language)
    {
        offset_x3 = offset_x1 + 170+30;
        offset_x4 = offset_x3 + 110;
        offset_x5 = offset_x4 + 250;
        offset_x6 = offset_x1 + 380;
    }
#if 0
    else if (DE == language)
    {
        offset_x3 = offset_x1 + 280;
        offset_x4 = offset_x3 + 100;
        offset_x5 = offset_x4 + 280;
        offset_x6 = offset_x1 + 380;
    }
#endif

	
	offset_y1 = head_y + 90;
	offset_y2 = offset_y1 + 40;
	offset_y3 = offset_y2 + 35;
	offset_y4 = offset_y3 + 45;
	offset_y5 = offset_y4 + 400;
	offset_y6 = offset_y5 + 355;
	offset_y7 = offset_y6 + 195;
	
	int microOffset = 10;
	int fontSize = 28;	
	ReportItemToBitmap(report_head[language], report_buffer, head_x, head_y+microOffset, fontSize);
	
	fontSize = 14;	
	string_buffer = strcat( hospital[language], (char *)Item.hospital );
	ReportItemToBitmap(string_buffer, report_buffer, offset_x1, offset_y1+microOffset, fontSize);

	string_buffer = strcat( diag_date[language], (char *)Item.diag_date );
	ReportItemToBitmap(string_buffer, report_buffer, offset_x5, offset_y1+microOffset, fontSize);
	
	string_buffer = strcat( id[language], (char *)Item.id );
	ReportItemToBitmap(string_buffer, report_buffer, offset_x1, offset_y2+microOffset, fontSize);

	string_buffer = strcat( check_num[language], (char *)Item.check_num );
  	ReportItemToBitmap(string_buffer, report_buffer, offset_x5 + 80, offset_y3+microOffset, fontSize);

//hlx
#ifdef VET
	string_buffer = strcat( species[language], (char *)Item.species );
	ReportItemToBitmap(string_buffer, report_buffer, offset_x3+20, offset_y2+microOffset, fontSize);

	string_buffer = strcat( name[language], (char *)Item.name );//////////////
	ReportItemToBitmap(string_buffer, report_buffer, offset_x4, offset_y2+microOffset, fontSize);

	string_buffer = strcat( owner[language], (char *)Item.owner );
	ReportItemToBitmap(string_buffer, report_buffer, offset_x5+40, offset_y2+microOffset, fontSize);
#else
	string_buffer = strcat( name[language], (char *)Item.name );//////////////
	ReportItemToBitmap(string_buffer, report_buffer, offset_x2, offset_y2+microOffset, fontSize);
#endif


	
	string_buffer = strcat( sex[language], (char *)Item.sex );
	ReportItemToBitmap(string_buffer, report_buffer, offset_x1, offset_y3+microOffset, fontSize);
#ifdef VET
	string_buffer = strcat( age[language], (char *)Item.age );
	ReportItemToBitmap(string_buffer, report_buffer, offset_x3+20, offset_y3+microOffset, fontSize);
#else
	string_buffer = strcat( age[language], (char *)Item.age );
	ReportItemToBitmap(string_buffer, report_buffer, offset_x3, offset_y3+microOffset, fontSize);
#endif
	

	string_buffer = strcat( section[language], (char *)Item.section );
	ReportItemToBitmap(string_buffer, report_buffer, offset_x4, offset_y3+microOffset, fontSize);

	int ulIdeaH;
	ReportItemToBitmap(ul_idea[language], report_buffer, offset_x1, offset_y5+microOffset, fontSize);
	ulIdeaH = ReportItemToBitmap(Item.ul_idea, report_buffer, offset_x1+40, offset_y5+30+microOffset, fontSize);
	
//	offset_y6 = offset_y5 + 30 + ulIdeaH + microOffset;
	ReportItemToBitmap(comment[language], report_buffer, offset_x1, offset_y6+microOffset, fontSize);
	ReportItemToBitmap(Item.comment, report_buffer, offset_x1+40, offset_y6+30+microOffset, fontSize);
	
	string_buffer = strcat( diag_doctor[language], (char *)Item.diag_doctor );
	ReportItemToBitmap(string_buffer, report_buffer, offset_x1, offset_y7+microOffset, fontSize);

	string_buffer = strcat( report_doctor[language], (char *)Item.report_doctor );
	ReportItemToBitmap(string_buffer, report_buffer, offset_x6, offset_y7+microOffset, fontSize);

	for(i=0; i<REPORT_WIDTH_BYTES; i++){
		*(report_buffer + (offset_y2 + 3)*REPORT_WIDTH_BYTES + i) = 30;
		*(report_buffer + (offset_y4 + 3)*REPORT_WIDTH_BYTES + i) = 30;
		*(report_buffer + (offset_y7 - 4)*REPORT_WIDTH_BYTES + i) = 30;
	}

	if (Item.image_data[0] == NULL)
		goto ImageEnd;
	
	if (Item.image_data[1] == NULL)
	{
		offset_xi[0] = 250;
		for (i=0; i<size_y; i++)
		{
			for(j=0; j<size_x; j++)
			{
				*(report_buffer + (offset_y4+20+i)*REPORT_WIDTH_BYTES + (offset_xi[0] + j) * BYTES_PER_PIXEL) = *(Item.image_data[0] + i*lenOfLine + j*3);
				*(report_buffer + (offset_y4+20+i)*REPORT_WIDTH_BYTES + (offset_xi[0] + j) * BYTES_PER_PIXEL + 1) = *(Item.image_data[0] + i*lenOfLine + j*3 + 1);
				*(report_buffer + (offset_y4+20+i)*REPORT_WIDTH_BYTES + (offset_xi[0] + j) * BYTES_PER_PIXEL + 2) = *(Item.image_data[0] + i*lenOfLine + j*3 + 2);
			}
		}
	}
	else
	{
		offset_xi[0] = 10;
		offset_xi[1] = 450;
		for (i=0; i<size_y; i++)
		{
			for(j=0; j<size_x; j++)
			{
				*(report_buffer + (offset_y4+20+i)*REPORT_WIDTH_BYTES + (offset_xi[0] + j) * BYTES_PER_PIXEL) = *(Item.image_data[0] + i*lenOfLine + j*3);
				*(report_buffer + (offset_y4+20+i)*REPORT_WIDTH_BYTES + (offset_xi[0] + j) * BYTES_PER_PIXEL + 1) = *(Item.image_data[0] + i*lenOfLine + j*3 + 1);
				*(report_buffer + (offset_y4+20+i)*REPORT_WIDTH_BYTES + (offset_xi[0] + j) * BYTES_PER_PIXEL + 2) = *(Item.image_data[0] + i*lenOfLine + j*3 + 2);
				*(report_buffer + (offset_y4+20+i)*REPORT_WIDTH_BYTES + (offset_xi[1] + j) * BYTES_PER_PIXEL) = *(Item.image_data[1] + i*lenOfLine + j*3);
				*(report_buffer + (offset_y4+20+i)*REPORT_WIDTH_BYTES + (offset_xi[1] + j) * BYTES_PER_PIXEL + 1) = *(Item.image_data[1] + i*lenOfLine + j*3 + 1);
				*(report_buffer + (offset_y4+20+i)*REPORT_WIDTH_BYTES + (offset_xi[1] + j) * BYTES_PER_PIXEL + 2) = *(Item.image_data[1] + i*lenOfLine + j*3 + 2);
			}
		}
	}

ImageEnd:
	unsigned char tmp[REPORT_WIDTH_BYTES];
	for (i=REPORT_HEIGHT; i>0; i--)
	{
	//	for (j=REPORT_WIDTH_BYTES; j>0; j--)
		for (j=0; j<REPORT_WIDTH_BYTES; j++)
		{
			tmp[j] = *(report_buffer + i*REPORT_WIDTH_BYTES - 1 - j);
//			*(report_buffer + i*REPORT_WIDTH_BYTES - j + BMP_HEAD_LEN) = *(report_buffer + i*REPORT_WIDTH_BYTES - j);
		}
		for (j=0; j<REPORT_WIDTH_BYTES; j++)
		{
			*(report_buffer + (i-1)*REPORT_WIDTH_BYTES + j + BMP_HEAD_LEN) =  tmp[j];
		}
	}
	
	for (i=0; i<BMP_HEAD_LEN; i++)
	{
		*(report_buffer + i) = bmp_head[i];
	}
}

void itoa(int data, char *string, unsigned char length)
{
	memset(string, 0x0, length);
	sprintf(string, "%d", data);
}
//extern int ReportToBitmap(struct print_re_item Item, unsigned char *report_buffer, int size_x, int size_y, int lenOfLine, gint8 language);

//int print_report(struct print_re_item item, int size_x, int size_y, gint8 language)
int print_report( int size_x, int size_y, gint8 language)
{
#if 1
	int ret, fd;
	unsigned char *report_buffer;
//	char id[20], name[60], check_num[15]={0}, sex[20], age[15], section[20], diag_date[15], diag_doctor[50], report_doctor[50], ul_idea[2000], comment[2000];
    char id[20], name[60], check_num[15]={0}, sex[20], age[15], section[50], diag_date[30], diag_doctor[50], report_doctor[50], ul_idea[2000], comment[2000];

//	char tmp[15];
//hlx
#ifdef VET
	char owner[60];
	char species[60];
	string str_species;
#endif

	PatientInfo::Info report_info;
	g_patientInfo.GetInfo(report_info); 
	strcpy(id, report_info.p.id.c_str());
#ifdef VET
	sprintf(name, "%s", report_info.p.animal_name.c_str());
	sprintf(owner, "%s", report_info.p.owner_name.c_str());
	GetAnimalSpeciesString(report_info.p.species, str_species);
	sprintf(species, "%s", str_species.c_str());
#else
	sprintf(name, "%s %s %s", report_info.p.name.last.c_str(), report_info.p.name.first.c_str(), report_info.p.name.mid.c_str());
#endif
	//sprintf(name, "%s %s %s", report_info.p.name.last.c_str(), report_info.p.name.first.c_str(), report_info.p.name.mid.c_str());
   	strcpy(check_num, report_info.e.examNum.c_str());
#ifdef VET
    if (report_info.p.sex == 0)
	    strcpy(sex, _("Female "));
	else if (report_info.p.sex == 1)
	    strcpy(sex, _("Male "));
#else
    if (report_info.p.sex == 0)
	    strcpy(sex, _("Female"));
	else if (report_info.p.sex == 1)
	    strcpy(sex, _("Male"));
#endif
	else
	    strcpy(sex, _("Other"));


//	itoa(report_info.p.age, tmp, 14);
//	strcpy(age, tmp);
    string unit;
    switch(report_info.p.ageUnit)
    {
        case 0:
            unit = _("Y"); break;
        case 1:
            unit = _("M"); break;
        case 2:
            unit = _("D"); break;
        default:
            unit = _("Y"); break;
    }
	sprintf(age, "%d%s", report_info.p.age,unit.c_str());
	//printf("age is************************** %s\n", age);

//	strcpy(section, ViewReport::GetInstance()->GetCheckPart());
	string part;
	TopArea::GetInstance()->GetCheckPart(part);
	strcpy(section, part.c_str());
//	PRINTF("exam type is %s\n", section);
	
	char dayTmp[4], monthTmp[4], yearTmp[6];
	itoa(report_info.e.examDate.day, dayTmp, 4);
	if (strlen(dayTmp)==1){dayTmp[2]='\0', dayTmp[1] = dayTmp[0]; dayTmp[0] = '0';}
	itoa(report_info.e.examDate.month, monthTmp, 4);
	if (strlen(monthTmp)==1){monthTmp[2]='\0', monthTmp[1] = monthTmp[0]; monthTmp[0] = '0';}
	itoa(report_info.e.examDate.year, yearTmp, 6);

	SysGeneralSetting sys;
	int date_format = sys.GetDateFormat();
	if (date_format == 1)
		sprintf(diag_date, "%s-%s-%s", monthTmp, dayTmp, yearTmp);
	else if (date_format == 2)
		sprintf(diag_date, "%s-%s-%s", dayTmp, monthTmp, yearTmp);
	else
		sprintf(diag_date, "%s-%s-%s", yearTmp, monthTmp, dayTmp);
	
	string hospital;
	SysGeneralSetting get_hospital;
	get_hospital.GetHospital(hospital);

	strcpy(diag_doctor, report_info.e.examDoctor.c_str());
	strcpy(report_doctor, report_info.e.reportDoctor.c_str());

	snprintf(ul_idea, sizeof(ul_idea), ViewReport::GetInstance()->GetIndication());
	snprintf(comment, sizeof(comment), ViewReport::GetInstance()->GetComments());
//	strncpy(ul_idea, ViewReport::GetInstance()->GetIndication(), 1000-1);
//	strncpy(comment, ViewReport::GetInstance()->GetComments(), 1000-1);
	
	struct print_re_item item;
    memset(&item, 0, sizeof(item));

	item.id = id;
	item.name = name;

//hlx
#ifdef VET
	item.owner = owner;
	item.species = species;
#endif
	item.check_num = check_num;
	item.sex = sex;
	item.age = age;
	item.section = section;
	item.diag_date = diag_date;
	item.hospital = (char*)(hospital.c_str());
	item.diag_doctor = diag_doctor;
	item.report_doctor = report_doctor;
	item.ul_idea = ul_idea;
	item.comment = comment;
	vector<string> imageName;
	imageName = ViewReport::GetInstance()->GetSelectImage();
	ImgMan::ImgItem printImg[2];
	float imgScale = 0.7;
	int lenOfLine = 0;
	int imageNums = imageName.size();
	if (imageNums > 2)
		imageNums = 2;	//现在最多能传入2幅图片
	GdkPixbuf *buf[2];
	int i, j;
	for (i=0; i<2; i++) item.image_data[i] = NULL;

	for (i=0; i<imageNums; i++)//现在最多能传入2幅图片
	{
		if(ImgMan::GetInstance()->ReadSnap(imageName[i].c_str(), &printImg[i]) == 0)
		{
			buf[i] = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, IMG_W*imgScale, IMG_H*imgScale);
			gdk_pixbuf_scale(printImg[i].pixbuf, buf[i], 0, 0, IMG_W*imgScale, IMG_H*imgScale, (-80)*imgScale, (-120)*imgScale, imgScale, imgScale, GDK_INTERP_HYPER);
			lenOfLine = gdk_pixbuf_get_rowstride(buf[i]);
			item.image_data[i] = (char*)gdk_pixbuf_get_pixels(buf[i]);
			g_object_unref(printImg[i].pixbuf);
		}
		else
		{
			if(i>0)
			{
				for(j=0; j<i; j++)
					g_object_unref(buf[j]);
			}
			PRINTF("%s: ReadSnap %d Error!\n", __FUNCTION__, i);
			return -1;
		}
	}

	report_buffer = (unsigned char *)malloc(REPORT_WIDTH_BYTES*REPORT_HEIGHT + BMP_HEAD_LEN);
	memset(report_buffer, 255, REPORT_WIDTH_BYTES*REPORT_HEIGHT + BMP_HEAD_LEN);
	report_to_bitmap(item, report_buffer, IMG_W*imgScale, IMG_H*imgScale, lenOfLine, language);
//	ReportToBitmap(item, report_buffer, IMG_W*imgScale, IMG_H*imgScale, lenOfLine, language);

	fd = open(PRINTIMAGE, O_RDWR|O_CREAT|O_TRUNC, 00777);
	if (fd < 0){
		free(report_buffer);
		for (i=0; i<2; i++)
		{
			if (item.image_data[i] == NULL)
				break;
			g_object_unref(buf[i]);
		}
		return(-1);
	}

	ret = write( fd, report_buffer, REPORT_WIDTH_BYTES*REPORT_HEIGHT + BMP_HEAD_LEN );
	if (ret < REPORT_WIDTH_BYTES*REPORT_HEIGHT + BMP_HEAD_LEN){
		free(report_buffer);
		for (i=0; i<2; i++)
		{
			if (item.image_data[i] == NULL)
				break;
			g_object_unref(buf[i]);
		}
		close(fd);
		return(-1);
	}
	fchmod( fd, 00777);
	free(report_buffer);
	for (i=0; i<2; i++)
	{
		if (item.image_data[i] == NULL)
			break;
		g_object_unref(buf[i]);
	}
	close(fd);

#endif
	return(0);
}




