#include "CreateAvi.h"

#define DefaultXPelsPerMeter 3780
#define DefaultYPelsPerMeter 3780
#define OUTPUT_BUF_SIZES 4096
#define AVIF_HASINDEX 16
#define AVIFWRITE(file,buf,sizeofbuf)  \
	((size_t) fwrite((const void *) (buf), (size_t) 1, (size_t) (sizeofbuf), (file)))

typedef enum {
	M_A  = 0x41,
	M_B  = 0x42,
	M_D  = 0x44,
	M_F  = 0x46,
	M_I  = 0x49,
	M_L  = 0x4c,
	M_R  = 0x52,
	M_S  = 0x53,
	M_T  = 0x54,
	M_V  = 0x56,
	M_space = 0x20,
	M_0  = 0x30,
	M_1  = 0x31,
	M_a  = 0x61,
	M_b  = 0x62,
	M_d  = 0x64,
	M_f  = 0x66,
	M_h  = 0x68,
	M_i  = 0x69,
	M_l  = 0x6c,
	M_m  = 0x6d,
	M_o  = 0x6f,
	M_r  = 0x72,
	M_s  = 0x73,
	M_t  = 0x74,
	M_v  = 0x76,
	M_x  = 0x78,
} AVI_MARKER;

typedef struct _avi_destination_mgr {
	unsigned char * next_output_byte;
	size_t free_in_buffer;
} avi_destination_mgr;

typedef struct _avi_struct{
	int IndexSize;
	int MoviSize;
	int SecondListSize;
	int FirstListSize;
	int FileSize;

	/*for input*/
	unsigned char *inputfile;
	unsigned char * inbuffer;

	/*for output*/
	FILE *outputfile;
	avi_destination_mgr *dest;
	unsigned char * outbuffer;
}avi_struct;

typedef avi_struct * avi_ptr;

int DetermineFrameDataSize( int WIDTH,int HEIGHT )
{
	int BytesPerRow = (int) ( WIDTH*24 / 8.0 );
	while( BytesPerRow % 4 )
	{
		BytesPerRow++;
	}
	return BytesPerRow*HEIGHT;
}

void empty_output_buffers (avi_ptr cinfo)
{
	avi_destination_mgr *dest = cinfo->dest;

	if (AVIFWRITE(cinfo->outputfile, cinfo->outbuffer, OUTPUT_BUF_SIZES)!=(size_t) OUTPUT_BUF_SIZES)
		exit(0);
	dest->next_output_byte = cinfo->outbuffer;
	dest->free_in_buffer = OUTPUT_BUF_SIZES;
}

void emit_1_byte (avi_ptr cinfo, int val)
{
	avi_destination_mgr * dest = cinfo->dest;
	*(dest->next_output_byte)++ = (unsigned char)val;
	if (--dest->free_in_buffer == 0)
		empty_output_buffers (cinfo);
}

void emit_2_bytes (avi_ptr cinfo, int value)
{
	emit_1_byte(cinfo, value & 0xFF);
	emit_1_byte(cinfo, (value >> 8) & 0xFF);
}

void emit_4_bytes (avi_ptr cinfo, int value)
{
	emit_1_byte(cinfo, value & 0xFF);
	emit_1_byte(cinfo, (value >> 8) & 0xFF);
	emit_1_byte(cinfo, (value >> 16) & 0xFF);
	emit_1_byte(cinfo, (value >> 24) & 0xFF);
}

void emit_markers (avi_ptr cinfo, AVI_MARKER mark)
{
	emit_1_byte(cinfo, (int) mark);
}

bool read_avifile_width(FILE *file, int& WIDTH)
{
    int tmp[2];
    int offset = 16*4; //WIDTH start from byte
    memset(&tmp, 0, sizeof(tmp) );
	if(fseek(file, offset, SEEK_SET) < 0)
	{
		perror("fseek error:");
		return false;
	}

	if(fread(&tmp, sizeof(char), 4, file) != 4)
	{
		perror("fread error:");
		return false;
	}   

    WIDTH = tmp[0];
    return true;
}

bool read_avifile_height(FILE *file, int& HEIGHT)
{
    int tmp[2];
    int offset = 17*4; //HEIGHT start from byte
    memset(&tmp, 0, sizeof(tmp) );
	if(fseek(file, offset, SEEK_SET) < 0)
	{
		perror("fseek error:");
		return false;
	}

	if(fread(&tmp, sizeof(char), 4, file) != 4)
	{
		perror("fread error:");
		return false;
	}   

    HEIGHT = tmp[0];
    return true;
}

bool read_avifile_frames(FILE *file, int& NumberOfFrames)
{
    int tmp[2];
    int offset = 12*4; //NumberOfFrames start from byte
    memset(&tmp, 0, sizeof(tmp) );
	if(fseek(file, offset, SEEK_SET) < 0)
	{
		perror("fseek error:");
		return false;
	}

	if(fread(&tmp, sizeof(char), 4, file) != 4)
	{
		perror("fread error:");
		return false;
	}   

    NumberOfFrames = tmp[0];
    return true;
}

bool read_avifile_framerate(FILE *file, int& FRAMERATE)
{
    int tmp[2];
    int offset = 32; //FRAMERATE start from 32 byte
    memset(&tmp, 0, sizeof(tmp) );
	if(fseek(file, offset, SEEK_SET) < 0)
	{
		perror("fseek error:");
		return false;
	}

	if(fread(&tmp, sizeof(char), 4, file) != 4)
	{
		perror("fread error:");
		return false;
	}   

    FRAMERATE = (int)( 1000000.0 / (double)tmp[0] );
    return true;
}
    
bool get_avifile_info(FILE *file,int& WIDTH,int& HEIGHT,int& NumberOfFrames,int& FRAMERATE)
{
    read_avifile_width(file, WIDTH);
    read_avifile_height(file, HEIGHT);
    read_avifile_frames(file, NumberOfFrames);
    read_avifile_framerate(file, FRAMERATE); //start 32 byte
    return true;
}

void write_avifile_header (avi_ptr cinfo,int WIDTH,int HEIGHT,int NumberOfFrames,int FRAMERATE)
{
	emit_markers(cinfo, M_R);
	emit_markers(cinfo, M_I);
	emit_markers(cinfo, M_F);
	emit_markers(cinfo, M_F);
	emit_4_bytes(cinfo,cinfo->FileSize);
	emit_markers(cinfo, M_A);
	emit_markers(cinfo, M_V);
	emit_markers(cinfo, M_I);
	emit_markers(cinfo, M_space);
	emit_markers(cinfo, M_L);
	emit_markers(cinfo, M_I);
	emit_markers(cinfo, M_S);
	emit_markers(cinfo, M_T);
	emit_4_bytes(cinfo,cinfo->FirstListSize);
	emit_markers(cinfo, M_h);
	emit_markers(cinfo, M_d);
	emit_markers(cinfo, M_r);
	emit_markers(cinfo, M_l);
	emit_markers(cinfo, M_a);
	emit_markers(cinfo, M_v);
	emit_markers(cinfo, M_i);
	emit_markers(cinfo, M_h);
	emit_4_bytes(cinfo,56);
	emit_4_bytes(cinfo,(int)( 1000000.0 / (double) FRAMERATE ));
	emit_4_bytes(cinfo,0);
	emit_4_bytes(cinfo,0);
	emit_4_bytes(cinfo,AVIF_HASINDEX);
	emit_4_bytes(cinfo,NumberOfFrames);
	emit_4_bytes(cinfo,0);
	emit_4_bytes(cinfo,1);
	emit_4_bytes(cinfo,DetermineFrameDataSize(WIDTH,HEIGHT));
	emit_4_bytes(cinfo,WIDTH);
	emit_4_bytes(cinfo,HEIGHT);
	emit_4_bytes(cinfo,0);
	emit_4_bytes(cinfo,0);
	emit_4_bytes(cinfo,0);
	emit_4_bytes(cinfo,0);

	emit_markers(cinfo, M_L);
	emit_markers(cinfo, M_I);
	emit_markers(cinfo, M_S);
	emit_markers(cinfo, M_T);
	emit_4_bytes(cinfo,cinfo->SecondListSize);
	emit_markers(cinfo, M_s);
	emit_markers(cinfo, M_t);
	emit_markers(cinfo, M_r);
	emit_markers(cinfo, M_l);

	emit_markers(cinfo, M_s);
	emit_markers(cinfo, M_t);
	emit_markers(cinfo, M_r);
	emit_markers(cinfo, M_h);
	emit_4_bytes(cinfo,56);
	emit_markers(cinfo, M_v);
	emit_markers(cinfo, M_i);
	emit_markers(cinfo, M_d);
	emit_markers(cinfo, M_s);
	emit_markers(cinfo, M_D);
	emit_markers(cinfo, M_I);
	emit_markers(cinfo, M_B);
	emit_markers(cinfo, M_space);
	emit_4_bytes(cinfo,0);
	emit_4_bytes(cinfo,0);
	emit_4_bytes(cinfo,0);
	emit_4_bytes(cinfo,(int)( 1000000.0 / FRAMERATE ));
	emit_4_bytes(cinfo,1000000);
	emit_4_bytes(cinfo,0);
	emit_4_bytes(cinfo,NumberOfFrames);
	emit_4_bytes(cinfo,DetermineFrameDataSize(WIDTH,HEIGHT));
	emit_4_bytes(cinfo,0);
	emit_4_bytes(cinfo,0);
	emit_4_bytes(cinfo,0);
	emit_2_bytes(cinfo,WIDTH);
	emit_2_bytes(cinfo,HEIGHT);

	emit_markers(cinfo, M_s);
	emit_markers(cinfo, M_t);
	emit_markers(cinfo, M_r);
	emit_markers(cinfo, M_f);
	emit_4_bytes(cinfo,40);
	emit_4_bytes(cinfo,40);
	emit_4_bytes(cinfo,WIDTH);
	emit_4_bytes(cinfo,HEIGHT);
	emit_2_bytes(cinfo,1);
	emit_2_bytes(cinfo,24);
	emit_4_bytes(cinfo,0);
	emit_4_bytes(cinfo,DetermineFrameDataSize(WIDTH,HEIGHT));
	emit_4_bytes(cinfo,DefaultXPelsPerMeter);
	emit_4_bytes(cinfo,DefaultYPelsPerMeter);
	emit_4_bytes(cinfo,0);
	emit_4_bytes(cinfo,0);

	emit_markers(cinfo, M_L);
	emit_markers(cinfo, M_I);
	emit_markers(cinfo, M_S);
	emit_markers(cinfo, M_T);
	emit_4_bytes(cinfo,cinfo->MoviSize);
	emit_markers(cinfo, M_m);
	emit_markers(cinfo, M_o);
	emit_markers(cinfo, M_v);
	emit_markers(cinfo, M_i);
}

void write_index(avi_ptr cinfo,int WIDTH,int HEIGHT,int NumberOfFrames)
{
	emit_markers(cinfo, M_i);
	emit_markers(cinfo, M_d);
	emit_markers(cinfo, M_x);
	emit_markers(cinfo, M_1);

	emit_4_bytes(cinfo,cinfo->IndexSize);
	int FrameNumber;
	int FrameSize = DetermineFrameDataSize(WIDTH,HEIGHT);
	int ChunkOffset = 4;
	for( FrameNumber = 1 ; FrameNumber <= NumberOfFrames; FrameNumber++ )
	{
		emit_markers(cinfo, M_0);
		emit_markers(cinfo, M_0);
		emit_markers(cinfo, M_d);
		emit_markers(cinfo, M_b);
		emit_4_bytes(cinfo,16);
		emit_4_bytes(cinfo,ChunkOffset);
		ChunkOffset += FrameSize+8;
		emit_4_bytes(cinfo,FrameSize);
	}
}

#if 0
void CreateAvi(unsigned char * inputfile,FILE * outputfile,int WIDTH,int HEIGHT,int NumberOfFrames,int FRAMERATE)
{
	int i,j,k;
	avi_struct main_cinfo;
	avi_destination_mgr main_dest;
	avi_ptr cinfo = &main_cinfo;
	cinfo->dest = &main_dest;
	main_cinfo.outbuffer = (unsigned char *)malloc(OUTPUT_BUF_SIZES);
	main_cinfo.inbuffer = (unsigned char *)malloc(WIDTH*3);
	main_dest.next_output_byte = cinfo->outbuffer;
	main_dest.free_in_buffer = OUTPUT_BUF_SIZES;
	main_cinfo.IndexSize = (NumberOfFrames)*16;
	main_cinfo.MoviSize = 4+(NumberOfFrames)*( DetermineFrameDataSize(WIDTH,HEIGHT)+8);
	main_cinfo.SecondListSize = 4+64+48;
	main_cinfo.FirstListSize = (4+64) + (8+(main_cinfo.SecondListSize));
	main_cinfo.FileSize = 8+(8+(main_cinfo.IndexSize)) + (8+(main_cinfo.FirstListSize)) + (4+(main_cinfo.MoviSize));

	cinfo->inputfile = inputfile;

	cinfo->outputfile = outputfile;

	write_avifile_header (cinfo, WIDTH, HEIGHT, NumberOfFrames, FRAMERATE);
	for (i=0;i<NumberOfFrames;i++)
	{
		emit_markers(cinfo, M_0);
		emit_markers(cinfo, M_0);
		emit_markers(cinfo, M_d);
		emit_markers(cinfo, M_b);
		emit_4_bytes(cinfo, DetermineFrameDataSize(WIDTH,HEIGHT));
		cinfo->inputfile+=(HEIGHT-1)*WIDTH;
		for(j=0;j<HEIGHT;j++)
		{
			memcpy(cinfo->inbuffer,cinfo->inputfile,WIDTH*3);
			for(k=0;k<WIDTH*3;k++)
			{
				emit_1_byte(cinfo,(int)(cinfo->inbuffer)[k]);
			}
			cinfo->inputfile-=WIDTH*3;
		}
		cinfo->inputfile+=HEIGHT*WIDTH*3;
	}

	write_index(cinfo,WIDTH,HEIGHT,NumberOfFrames);
	AVIFWRITE(cinfo->outputfile, cinfo->outbuffer,(((size_t) (OUTPUT_BUF_SIZES))-cinfo->dest->free_in_buffer));
	free(main_cinfo.outbuffer);
	free(main_cinfo.inbuffer);
	fclose( cinfo->outputfile );
}
#endif

void CreateAvi(unsigned char* inputfile[], FILE* outputfile, int WIDTH, int HEIGHT, int NumberOfFrames, int FRAMERATE)
{
	int i,j,k;
	avi_struct main_cinfo;
	avi_destination_mgr main_dest;
	avi_ptr cinfo = &main_cinfo;
	cinfo->dest = &main_dest;
	main_cinfo.outbuffer = (unsigned char *)malloc(OUTPUT_BUF_SIZES);
	main_cinfo.inbuffer = (unsigned char *)malloc(WIDTH*3);
	main_dest.next_output_byte = cinfo->outbuffer;
	main_dest.free_in_buffer = OUTPUT_BUF_SIZES;
	main_cinfo.IndexSize = (NumberOfFrames)*16;
	main_cinfo.MoviSize = 4+(NumberOfFrames)*( DetermineFrameDataSize(WIDTH,HEIGHT)+8);
	main_cinfo.SecondListSize = 4+64+48;
	main_cinfo.FirstListSize = (4+64) + (8+(main_cinfo.SecondListSize));
	main_cinfo.FileSize = 8+(8+(main_cinfo.IndexSize)) + (8+(main_cinfo.FirstListSize)) + (4+(main_cinfo.MoviSize));

	unsigned char *p;
//	cinfo->inputfile = inputfile;

	cinfo->outputfile = outputfile;

	write_avifile_header (cinfo, WIDTH, HEIGHT, NumberOfFrames, FRAMERATE);
	for (i=0;i<NumberOfFrames;i++)
	{
		p = inputfile[i];
	//	PRINTF("Frame = %d, p = 0x%x\n", i, p);

		emit_markers(cinfo, M_0);
		emit_markers(cinfo, M_0);
		emit_markers(cinfo, M_d);
		emit_markers(cinfo, M_b);
		emit_4_bytes(cinfo, DetermineFrameDataSize(WIDTH,HEIGHT));

		p += (HEIGHT-1)*WIDTH*3;

		for(j=0;j<HEIGHT;j++)
		{
			memcpy(cinfo->inbuffer, p, WIDTH*3);
#if 0
			for(k=0;k<WIDTH*3;k++)
			{
				emit_1_byte(cinfo,(int)(cinfo->inbuffer)[k]);
			}
#endif
#if 1
			for(k=0;k<WIDTH;k++)
			{	
				emit_1_byte(cinfo, (int)(cinfo->inbuffer)[3*k+2]);
				emit_1_byte(cinfo, (int)(cinfo->inbuffer)[3*k+1]);
				emit_1_byte(cinfo, (int)(cinfo->inbuffer)[3*k]);
			}
#endif
			p -= WIDTH*3;
		}
	//	cinfo->inputfile+=HEIGHT*WIDTH*3;
	}

	write_index(cinfo,WIDTH,HEIGHT,NumberOfFrames);
	AVIFWRITE(cinfo->outputfile, cinfo->outbuffer,(((size_t) (OUTPUT_BUF_SIZES))-cinfo->dest->free_in_buffer));
	free(main_cinfo.outbuffer);
	free(main_cinfo.inbuffer);
}

bool ReadAvi(FILE *fs, unsigned char* buf, unsigned int spec_frame, int width, int height)
{
	int n;
	long offset;
	unsigned int row_size = width * 3;
	unsigned int frame_size = row_size * height;
	unsigned char *ptr = buf + frame_size;
	unsigned char row_buf[row_size];

	for(n=0; n<height; n++)
	{
		offset = AVI_HEAD + (spec_frame+1) * AVI_FRAME_HEAD + spec_frame * frame_size + n * row_size;
		if(fseek(fs, offset, SEEK_SET) < 0)
		{
			perror("fseek error:");
			return false;
		}
		ptr -= row_size;

		if(fread(row_buf, sizeof(char), row_size, fs) != row_size)
		{
			perror("fread error:");
			return false;
		}
		else
		{
			for(int k=0; k<width; k++)
			{	
				*(ptr+3*k) = row_buf[3*k+2];
				*(ptr+3*k+1) = row_buf[3*k+1];
				*(ptr+3*k+2) = row_buf[3*k];
			}
		}
#if 0
		if(fread(ptr, sizeof(char), row_size, fs) != row_size)
		{
			perror("fread error:");
			return false;
		}
#endif
	}
	return true;
}

