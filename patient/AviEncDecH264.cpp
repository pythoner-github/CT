#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Def.h"

//#define _GNU_SOURCE
#include <getopt.h>

#ifdef _MSC_VER
#include <io.h>     /* _setmode() */
#include <fcntl.h>  /* _O_BINARY */
#endif

#ifdef __cplusplus
#  define UINT64_C(c)	c ## ULL
#endif

extern "C"
{
#include <x264.h>
#include <libswscale/swscale.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
}
#include "patient/bs.h"
#include "patient/AviEncDecH264.h"
#define DATA_MAX 3000000

//#include "Replay.h"

typedef struct
{
    char *psz_fin;
    char *psz_fout;

    float f_fps;
    char  fcc[4];
} cfg_t;

typedef struct
{
    int i_data;
    int i_data_max;
    uint8_t *p_data;
} vbuf_t;
void vbuf_init( vbuf_t * );
void vbuf_add( vbuf_t *, int i_data, void *p_data );
void vbuf_reset( vbuf_t * );

typedef struct
{
    FILE *f;

    float f_fps;
    char  fcc[4];

    int   i_width;
    int   i_height;

    int64_t i_movi;
    int64_t i_movi_end;
    int64_t i_riff;

    int      i_frame;
    int      i_idx_max;
    uint32_t *idx;
} avi_t;
void avi_init( avi_t *, FILE *, float, char fcc[4] );
void avi_write( avi_t *, vbuf_t *, int  );
void avi_end( avi_t * );

typedef struct
{
    int i_ref_idc;  /* nal_priority_e */
    int i_type;     /* nal_unit_type_e */

    /* This data are raw payload */
    int     i_payload;
    uint8_t *p_payload;
} nal_t;

typedef struct
{
    int i_width;
    int i_height;

    int i_nal_type;
    int i_ref_idc;
    int i_idr_pic_id;
    int i_frame_num;
    int i_poc;

    int b_key;
    int i_log2_max_frame_num;
    int i_poc_type;
    int i_log2_max_poc_lsb;
} h264_t;

void h264_parser_init( h264_t * );
void h264_parser_parse( h264_t *h, nal_t *n, int *pb_nal_start );

static int nal_decode( nal_t *nal, uint8_t *p_data, int i_data );
static int  ParseNAL( nal_t *nal, avi_t *a, h264_t *h, int *pb_slice );

int avc_to_avi( FILE* outputfile ,FILE* inputfile ,int fps)
{
   // cfg_t cfg;
    float f_fps;
    char fcc[4];
    FILE    *fout;
    FILE    *fin;

    vbuf_t  vb;
    avi_t   avi;
    h264_t  h264;

    nal_t nal;
    int i_frame;
    int i_data;
    int b_eof;
    int b_key;
    int b_slice;
	int type;
	int i_data2;

    uint8_t data[DATA_MAX];

    fin  = inputfile;
    fout = outputfile;
    rewind(fin);//at the beginning of file
    f_fps = fps;
    memcpy( fcc, "h264", 4 );
    /* Init avi */
    avi_init( &avi, fout, f_fps, fcc );

    /* Init parser */
    h264_parser_init( &h264 );

    /* Init data */
    b_eof = 0;
    b_key = 0;
    b_slice = 0;
    i_frame = 0;
    i_data  = 0;

    /* Alloc space for a nal, used for decoding pps/sps/slice header */
    nal.p_payload = (uint8_t *)malloc( DATA_MAX );

    vbuf_init( &vb );

    /* split frame */
    while( 1 )
    {
        uint8_t *p, *p_next, *end;
        int i_size;

        /* fill buffer */
        if( i_data < DATA_MAX && !b_eof )
        {
            int i_read = fread( &data[i_data], 1, DATA_MAX - i_data, fin );
            if( i_read <= 0 )
                b_eof = 1;
            else
                i_data += i_read;
        }
        if( i_data < 3 )
            break;

        end = &data[i_data];

        /* Search begin of a NAL */
        p = &data[0];
        while( p < end - 3 )
        {
            if( p[0] == 0x00 && p[1] == 0x00 && p[2] == 0x01 )
            {
                break;
            }
            p++;
        }

        if( p >= end - 3 )
        {
            fprintf( stderr, "garbage (i_data = %d)\n", i_data );
            i_data = 0;
            continue;
        }

        /* Search end of NAL */
        p_next = p + 3;
        while( p_next < end - 3 )
        {
            if( p_next[0] == 0x00 && p_next[1] == 0x00 && p_next[2] == 0x01 )
            {
                break;
            }
            p_next++;
        }

        if( p_next == end - 3 && i_data < DATA_MAX )
            p_next = end;

        /* Compute NAL size */
        i_size = p_next - p - 3;
        if( i_size <= 0 )
        {
            if( b_eof )
                break;

            fprintf( stderr, "nal too large (FIXME) ?\n" );
            i_data = 0;
            continue;
        }

        /* Nal start at p+3 with i_size length */
        nal_decode( &nal, p +3, i_size < 2048 ? i_size : 2048 );

        b_key = h264.b_key;

		type = nal.i_type;

		i_data2 = vb.i_data;

        if( b_slice && vb.i_data && ( nal.i_type == NAL_SPS || nal.i_type == NAL_PPS ) )
        {
            avi_write( &avi, &vb, b_key );
            vbuf_reset( &vb );
            b_slice = 0;
        }

        /* Parse SPS/PPS/Slice */
        if( ParseNAL( &nal, &avi, &h264, &b_slice ) && vb.i_data > 0 )
        {
            avi_write( &avi, &vb, b_key );
            vbuf_reset( &vb );
        }

        /* fprintf( stderr, "nal:%d ref:%d\n", nal.i_type, nal.i_ref_idc ); */

        /* Append NAL to buffer */
        vbuf_add( &vb, i_size + 3, p );

        /* Remove this nal */
        memmove( &data[0], p_next, end - p_next );
        i_data -= p_next - &data[0];
    }

    if( vb.i_data > 0 )
    {
        avi_write( &avi, &vb, h264.b_key );
    }

    avi.i_width  = h264.i_width;
    avi.i_height = h264.i_height;

    avi_end( &avi );

    /* free mem */
    free( nal.p_payload );

    //fclose( fin );
    //fclose( fout );

    return 0;
}

/*****************************************************************************
 * h264_parser_*:
 *****************************************************************************/
void h264_parser_init( h264_t *h )
{
    h->i_width = 0;
    h->i_height = 0;
    h->b_key = 0;
    h->i_nal_type = -1;
    h->i_ref_idc = -1;
    h->i_idr_pic_id = -1;
    h->i_frame_num = -1;
    h->i_log2_max_frame_num = 0;
    h->i_poc = -1;
    h->i_poc_type = -1;
}
void h264_parser_parse( h264_t *h, nal_t *nal, int *pb_nal_start )
{
    bs_t s;
    *pb_nal_start = 0;

    if( nal->i_type == NAL_SPS || nal->i_type == NAL_PPS )
        *pb_nal_start = 1;

    bs_init( &s, nal->p_payload, nal->i_payload );
    if( nal->i_type == NAL_SPS )
    {
        int i_tmp;

        i_tmp = bs_read( &s, 8 );
        bs_skip( &s, 1+1+1 + 5 + 8 );
        /* sps id */
        bs_read_ue( &s );

        if( i_tmp >= 100 )
        {
            bs_read_ue( &s ); // chroma_format_idc
            bs_read_ue( &s ); // bit_depth_luma_minus8
            bs_read_ue( &s ); // bit_depth_chroma_minus8
            bs_skip( &s, 1 ); // qpprime_y_zero_transform_bypass_flag
            if( bs_read( &s, 1 ) ) // seq_scaling_matrix_present_flag
            {
                int i, j;
                for( i = 0; i < 8; i++ )
                {
                    if( bs_read( &s, 1 ) ) // seq_scaling_list_present_flag[i]
                    {
                        uint8_t i_tmp = 8;
                        for( j = 0; j < (i<6?16:64); j++ )
                        {
                            i_tmp += bs_read_se( &s );
                            if( i_tmp == 0 )
                                break;
                        }
                    }
                }
            }
        }

        /* Skip i_log2_max_frame_num */
        h->i_log2_max_frame_num = bs_read_ue( &s ) + 4;
        /* Read poc_type */
        h->i_poc_type = bs_read_ue( &s );
        if( h->i_poc_type == 0 )
        {
            h->i_log2_max_poc_lsb = bs_read_ue( &s ) + 4;
        }
        else if( h->i_poc_type == 1 )
        {
            int i_cycle;
            /* skip b_delta_pic_order_always_zero */
            bs_skip( &s, 1 );
            /* skip i_offset_for_non_ref_pic */
            bs_read_se( &s );
            /* skip i_offset_for_top_to_bottom_field */
            bs_read_se( &s );
            /* read i_num_ref_frames_in_poc_cycle */
            i_cycle = bs_read_ue( &s );
            if( i_cycle > 256 ) i_cycle = 256;
            while( i_cycle > 0 )
            {
                /* skip i_offset_for_ref_frame */
                bs_read_se(&s );
            }
        }
        /* i_num_ref_frames */
        bs_read_ue( &s );
        /* b_gaps_in_frame_num_value_allowed */
        bs_skip( &s, 1 );

        /* Read size */
        h->i_width  = 16 * ( bs_read_ue( &s ) + 1 );
        h->i_height = 16 * ( bs_read_ue( &s ) + 1 );

        /* b_frame_mbs_only */
        i_tmp = bs_read( &s, 1 );
        if( i_tmp == 0 )
        {
            bs_skip( &s, 1 );
        }
        /* b_direct8x8_inference */
        bs_skip( &s, 1 );

        /* crop ? */
        i_tmp = bs_read( &s, 1 );
        if( i_tmp )
        {
            /* left */
            h->i_width -= 2 * bs_read_ue( &s );
            /* right */
            h->i_width -= 2 * bs_read_ue( &s );
            /* top */
            h->i_height -= 2 * bs_read_ue( &s );
            /* bottom */
            h->i_height -= 2 * bs_read_ue( &s );
        }

        /* vui: ignored */
    }
    else if( nal->i_type >= NAL_SLICE && nal->i_type <= NAL_SLICE_IDR )
    {
        int i_tmp;

        /* i_first_mb */
        bs_read_ue( &s );
        /* picture type */
        switch( bs_read_ue( &s ) )
        {
            case 0: case 5: /* P */
            case 1: case 6: /* B */
            case 3: case 8: /* SP */
                h->b_key = 0;
                break;
            case 2: case 7: /* I */
            case 4: case 9: /* SI */
                h->b_key = (nal->i_type == NAL_SLICE_IDR);
                break;
        }
        /* pps id */
        bs_read_ue( &s );

        /* frame num */
        i_tmp = bs_read( &s, h->i_log2_max_frame_num );

        if( i_tmp != h->i_frame_num )
            *pb_nal_start = 1;

        h->i_frame_num = i_tmp;

        if( nal->i_type == NAL_SLICE_IDR )
        {
            i_tmp = bs_read_ue( &s );
            if( h->i_nal_type == NAL_SLICE_IDR && h->i_idr_pic_id != i_tmp )
                *pb_nal_start = 1;

            h->i_idr_pic_id = i_tmp;
        }

        if( h->i_poc_type == 0 )
        {
            i_tmp = bs_read( &s, h->i_log2_max_poc_lsb );
            if( i_tmp != h->i_poc )
                *pb_nal_start = 1;
            h->i_poc = i_tmp;
        }
    }
    h->i_nal_type = nal->i_type;
    h->i_ref_idc = nal->i_ref_idc;
}

static int  ParseNAL( nal_t *nal, avi_t *a, h264_t *h, int *pb_slice )
{
    int b_flush = 0;
    int b_start;

    h264_parser_parse( h, nal, &b_start );

    if( b_start && *pb_slice )
    {
        b_flush = 1;
        *pb_slice = 0;
    }

    if( nal->i_type >= NAL_SLICE && nal->i_type <= NAL_SLICE_IDR )
        *pb_slice = 1;

    return b_flush;
}

/*****************************************************************************
 * vbuf: variable buffer
 *****************************************************************************/
void vbuf_init( vbuf_t *v )
{
    v->i_data = 0;
    v->i_data_max = 10000;
    v->p_data = (uint8_t *)malloc( v->i_data_max );
}
void vbuf_add( vbuf_t *v, int i_data, void *p_data )
{
    if( i_data + v->i_data >= v->i_data_max )
    {
        v->i_data_max += i_data;
        v->p_data = (uint8_t *)realloc( v->p_data, v->i_data_max );
    }
    memcpy( &v->p_data[v->i_data], p_data, i_data );

    v->i_data += i_data;
}
void vbuf_reset( vbuf_t *v )
{
    v->i_data = 0;
}

/*****************************************************************************
 * avi:
 *****************************************************************************/
void avi_write_uint16( avi_t *a, uint16_t w )
{
    fputc( ( w      ) & 0xff, a->f );
    fputc( ( w >> 8 ) & 0xff, a->f );
}

void avi_write_uint32( avi_t *a, uint32_t dw )
{
    fputc( ( dw      ) & 0xff, a->f );
    fputc( ( dw >> 8 ) & 0xff, a->f );
    fputc( ( dw >> 16) & 0xff, a->f );
    fputc( ( dw >> 24) & 0xff, a->f );
}

void avi_write_fourcc( avi_t *a, const char fcc[4] )
{
    fputc( fcc[0], a->f );
    fputc( fcc[1], a->f );
    fputc( fcc[2], a->f );
    fputc( fcc[3], a->f );
}

/* Flags in avih */
#define AVIF_HASINDEX       0x00000010  // Index at end of file?
#define AVIF_ISINTERLEAVED  0x00000100
#define AVIF_TRUSTCKTYPE    0x00000800  // Use CKType to find key frames?

#define AVIIF_KEYFRAME      0x00000010L /* this frame is a key frame.*/

void avi_write_header( avi_t *a )
{
    avi_write_fourcc( a, "RIFF" );
    avi_write_uint32( a, a->i_riff > 0 ? a->i_riff - 8 : 0xFFFFFFFF );

    avi_write_fourcc( a, "AVI " );

    avi_write_fourcc( a, "LIST" );
    avi_write_uint32( a,  4 + 4*16 + 12 + 4*16 + 4*12 );
    avi_write_fourcc( a, "hdrl" );

    avi_write_fourcc( a, "avih" );
    avi_write_uint32( a, 4*16 - 8 );
    avi_write_uint32( a, 1000000 / a->f_fps );
    avi_write_uint32( a, 0xffffffff );
    avi_write_uint32( a, 0 );
    avi_write_uint32( a, AVIF_HASINDEX|AVIF_ISINTERLEAVED|AVIF_TRUSTCKTYPE);
    avi_write_uint32( a, a->i_frame );
    avi_write_uint32( a, 0 );
    avi_write_uint32( a, 1 );
    avi_write_uint32( a, 1000000 );
    avi_write_uint32( a, a->i_width );
    avi_write_uint32( a, a->i_height );
    avi_write_uint32( a, 0 );
    avi_write_uint32( a, 0 );
    avi_write_uint32( a, 0 );
    avi_write_uint32( a, 0 );

    avi_write_fourcc( a, "LIST" );
    avi_write_uint32( a,  4 + 4*16 + 4*12 );
    avi_write_fourcc( a, "strl" );

    avi_write_fourcc( a, "strh" );
    avi_write_uint32( a,  4*16 - 8 );
    avi_write_fourcc( a, "vids" );
    avi_write_fourcc( a, a->fcc );
    avi_write_uint32( a, 0 );
    avi_write_uint32( a, 0 );
    avi_write_uint32( a, 0 );
    avi_write_uint32( a, 1000 );
    avi_write_uint32( a, a->f_fps * 1000 );
    avi_write_uint32( a, 0 );
    avi_write_uint32( a, a->i_frame );
    avi_write_uint32( a, 1024*1024 );
    avi_write_uint32( a, -1 );
    avi_write_uint32( a, a->i_width * a->i_height );
    avi_write_uint32( a, 0 );
    avi_write_uint16( a, a->i_width );
    avi_write_uint16( a, a->i_height );

    avi_write_fourcc( a, "strf" );
    avi_write_uint32( a,  4*12 - 8 );
    avi_write_uint32( a,  4*12 - 8 );
    avi_write_uint32( a,  a->i_width );
    avi_write_uint32( a,  a->i_height );
    avi_write_uint16( a,  1 );
    avi_write_uint16( a,  24 );
    avi_write_fourcc( a,  a->fcc );
    avi_write_uint32( a, a->i_width * a->i_height );
    avi_write_uint32( a,  0 );
    avi_write_uint32( a,  0 );
    avi_write_uint32( a,  0 );
    avi_write_uint32( a,  0 );

    avi_write_fourcc( a, "LIST" );
    avi_write_uint32( a,  a->i_movi_end > 0 ? a->i_movi_end - a->i_movi + 4: 0xFFFFFFFF );
    avi_write_fourcc( a, "movi" );
}

void avi_write_idx( avi_t *a )
{
    avi_write_fourcc( a, "idx1" );
    avi_write_uint32( a,  a->i_frame * 16 );
    fwrite( a->idx, a->i_frame * 16, 1, a->f );
}

void avi_init( avi_t *a, FILE *f, float f_fps, char fcc[4] )
{
    a->f = f;
    a->f_fps = f_fps;
    memcpy( a->fcc, fcc, 4 );
    a->i_width = 0;
    a->i_height = 0;
    a->i_frame = 0;
    a->i_movi = 0;
    a->i_riff = 0;
    a->i_movi_end = 0;
    a->i_idx_max = 0;
    a->idx = NULL;

    avi_write_header( a );

    a->i_movi = ftell( a->f );
}

static void avi_set_dw( void *_p, uint32_t dw )
{
    uint8_t *p = (uint8_t *)_p;

    p[0] = ( dw      )&0xff;
    p[1] = ( dw >> 8 )&0xff;
    p[2] = ( dw >> 16)&0xff;
    p[3] = ( dw >> 24)&0xff;
}

void avi_write( avi_t *a, vbuf_t *v, int b_key )
{
    int64_t i_pos = ftell( a->f );

    /* chunk header */
    avi_write_fourcc( a, "00dc" );
    avi_write_uint32( a, v->i_data );

    fwrite( v->p_data, v->i_data, 1, a->f );

    if( v->i_data&0x01 )
    {
        /* pad */
        fputc( 0, a->f );
    }

    /* Append idx chunk */
    if( a->i_idx_max <= a->i_frame )
    {
        a->i_idx_max += 1000;
        a->idx = (uint32_t *)realloc( a->idx, a->i_idx_max * 16 );
    }

    memcpy( &a->idx[4*a->i_frame+0], "00dc", 4 );
    avi_set_dw( &a->idx[4*a->i_frame+1], b_key ? AVIIF_KEYFRAME : 0 );
    avi_set_dw( &a->idx[4*a->i_frame+2], i_pos );
    avi_set_dw( &a->idx[4*a->i_frame+3], v->i_data );

    a->i_frame++;
}

void avi_end( avi_t *a )
{
    a->i_movi_end = ftell( a->f );

    /* write index */
    avi_write_idx( a );

    a->i_riff = ftell( a->f );

    /* Fix header */
    fseek( a->f, 0, SEEK_SET );
    avi_write_header( a );

    fprintf( stderr, "avi file written\n" );
    fprintf( stderr, "  - codec: %4.4s\n", a->fcc );
    fprintf( stderr, "  - size: %dx%d\n", a->i_width, a->i_height );
    fprintf( stderr, "  - fps: %.3f\n", a->f_fps );
    fprintf( stderr, "  - frames: %d\n", a->i_frame );
}

/*****************************************************************************
 * nal:
 *****************************************************************************/
int nal_decode( nal_t *nal, uint8_t  *p_data, int i_data )
{
    uint8_t *src = p_data;
    uint8_t *end = &src[i_data];
    uint8_t *dst = nal->p_payload;

    nal->i_type    = src[0]&0x1f;
    nal->i_ref_idc = (src[0] >> 5)&0x03;

    src++;

    while( src < end )
    {
        if( src < end - 3 && src[0] == 0x00 && src[1] == 0x00  && src[2] == 0x03 )
        {
            *dst++ = 0x00;
            *dst++ = 0x00;

            src += 3;
            continue;
        }
        *dst++ = *src++;
    }

	nal->i_payload = dst - nal->p_payload;
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

typedef struct
{
    int width;
    int height;
    int frame_total;
    int frame_rate;
    unsigned char **data;
    FILE* outputfile;
    FILE* tmp;
}video_info_t;

//参数设置
static int x264_param_init( x264_param_t * param ,video_info_t* info)
{
    /*
    x264_param_default( param );
    param->rc.f_rf_constant= 0 ;
    param->i_width         = info->width;
    param->i_height       = info->height;
    param->i_fps_num       = info->frame_rate;
    */
    x264_param_default_preset(param,"veryfast","zerolatency");
    //param->rc.f_rf_constant= 0 ;
    param->i_threads =1;
    param->i_width         = info->width;
    param->i_height       = info->height;
    param->i_fps_num       = info->frame_rate;
    param->i_fps_den =1;
    // Intra refres:
    param->i_keyint_max = info->frame_rate;
    param->b_intra_refresh =1;
    param->rc.i_rc_method = X264_RC_CRF;
    //param->rc.f_rf_constant =25;
    param->rc.f_rf_constant= 0 ;
    param->rc.f_rf_constant_max =35;
    //For streaming:
    param->b_repeat_headers =1;
    param->b_annexb =1;
    x264_param_apply_profile(param,"baseline");
    return 0;
}

//编一帧图片
//返回一帧大小
static int encode_frame( x264_t* encoder, x264_picture_t* pic_in, FILE* tmp )
{
    x264_picture_t pic_out;
    x264_nal_t* nal;
    int i_nal;
    int i_frame_size = 0;

    i_frame_size = x264_encoder_encode( encoder, &nal, &i_nal, pic_in, &pic_out );

    if ( i_frame_size > 0 )
    {
        //do some thing for save:from nal[0].p_payload
        //                        to   file
        //                        size i_frame_size
        if ( fwrite( nal[0].p_payload, i_frame_size, 1, tmp) )
            return i_frame_size;
        else
        {
            PRINTF("write failed \n");
            return -1;
        }
    }
    else
    {
            PRINTF("encode_frame failed \n");
            return -1;
    }
}

//编码
static int encode( x264_param_t* param, video_info_t *info )
{
    int frame;
    int srcstride = info->width*3;
    unsigned char* data[4]={NULL, NULL,NULL,NULL};
    x264_t* encoder = x264_encoder_open( param );
    x264_picture_t pic_in;

    x264_picture_alloc( &pic_in, X264_CSP_I420 , info->width, info->height );

    struct SwsContext* convertCtx = sws_getContext( info->width,
                                                    info->height ,
                                                    PIX_FMT_RGB24,
                                                    info->width,
                                                    info->height,
                                                    PIX_FMT_YUV420P,
                                                    SWS_FAST_BILINEAR,
                                                    NULL, NULL, NULL);

    //each frame
    for (frame = 0; frame < info->frame_total; frame++)
    {
        //取一帧图片
        data[0] = info->data[frame];
        //数据转换RGB2YUV420
        //av_image_fill_pointers(data, PIX_FMT_RGB24,info->height, info->data[frame],&srcstride);
        sws_scale(convertCtx, (const uint8_t* const*)data, &srcstride, 0, info->height, pic_in.img.plane, pic_in.img.i_stride );
        //编一帧图片
        encode_frame( encoder, &pic_in, info->tmp);
    }

    //释放资源
    if (convertCtx)
    {
        sws_freeContext(convertCtx);
    }

    x264_picture_clean( &pic_in );
    x264_encoder_close( encoder );

    return 0;
}

//////////////////////////
//
//
/////////////////////////
int CreateAviEncode( unsigned char* inputdata[], FILE * outputfile,
                     int WIDTH, int HEIGHT, int frame_total, int frame_rate)
{
    x264_param_t param;
    video_info_t video_info;
    FILE * tmp;

    tmp = fopen("./tmp.264","w+");

    video_info.width       = WIDTH;
    video_info.height      = HEIGHT;
    video_info.frame_total = frame_total;
    video_info.frame_rate  = frame_rate;
    video_info.data        = inputdata;
    video_info.outputfile  = outputfile;
    video_info.tmp         = tmp;

    int a = 3;
    int b = 4;
    int c= a+b;
    printf("a:%d, b:%d, a+b=c:%d\n", a, b, c);
    //参数设置
    x264_param_init( &param, &video_info);
    int sub = b - a;
    printf("sub:%d\n", sub);
    //编码
    encode( &param, &video_info );
    //封装
    avc_to_avi(  outputfile, tmp,frame_rate);

    fclose(tmp);

    return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

static int open_codec_context(int* stream_idx, AVFormatContext *fmt_ctx, enum AVMediaType type )
{
    int ret;
    AVStream *st;
    AVCodecContext *dec_ctx = NULL;
    AVCodec *dec = NULL;

    ret = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
    if (ret < 0)
    {
        PRINTF("Could not find %s stream in input file \n",
                av_get_media_type_string(type));
        return ret;
    }
    else
    {
        *stream_idx = ret;
        st = fmt_ctx->streams[*stream_idx];

        /*find decoder for the steam*/
        dec_ctx = st->codec;
        dec = avcodec_find_decoder(dec_ctx->codec_id);
        if(!dec)
        {
            PRINTF("failed to find %s codec\n",
                    av_get_media_type_string(type));
            return ret;
        }
        /*open decoder*/
        if ((ret = avcodec_open2(dec_ctx, dec, NULL)) < 0)
        {
            PRINTF("fail to open %scodec \n",
                    av_get_media_type_string(type));
            return ret;
        }
    }

    return 0;
}

#if 0
int save_to_replay(uint8_t *buf, int width, int height)
{
    // add image to replay memory
        Image::AutoTracePara para;
		Image image((unsigned int*)buf, para);
		Replay::GetInstance()->AddOneCineImg(image);
        return 0;
}

#endif
bool ReadAviData(const char *src_filename, uint8_t *buf,int frame_total, int width, int height, PTR_SAVE ptrFunc)
{
    int ret = 0;
    AVFormatContext *fmt_ctx = NULL;
    AVCodecContext *video_dec_ctx = NULL;
    AVFrame *frame = NULL, *frame_rgb = NULL;
    AVStream *video_stream = NULL;
    AVPacket pkt;

    struct SwsContext *sws_ctx = NULL;

    int video_stream_idx = -1;
    int picture_size;
    int frame_finished;
   // const char *src_filename = "./1.avi";
    uint8_t *rgb_buf = NULL;
    //注册视频文件
    av_register_all();

    //打开视频文件及分配空间给context
    if ( avformat_open_input(&fmt_ctx, src_filename, NULL, NULL) < 0 )
    {
        PRINTF("av open input file failed!\n");
        //exit(1);
        ret = -1;
        goto end;
    }

    //获取流信息
    if ( avformat_find_stream_info(fmt_ctx, NULL) < 0 )
    {
        PRINTF("av find stream info failed!\n");
        //exit(1);
        ret = -1;
        goto end;
    }

    //取得视频流的上下文
    if (open_codec_context(&video_stream_idx, fmt_ctx, AVMEDIA_TYPE_VIDEO ) >= 0)
    {
        video_stream = fmt_ctx->streams[video_stream_idx];
        video_dec_ctx = video_stream->codec;

        /* allocate image where the decoded image will be put */
       // video_dst_bufsize = av_image_alloc(video_dst_data, video_dst_linesize,
       //                      video_dec_ctx->width, video_dec_ctx->height,
         //                    video_dec_ctx->pix_fmt, 1);
    }

    frame = avcodec_alloc_frame();
    frame_rgb = avcodec_alloc_frame();
    if ( (frame == NULL) || (frame_rgb == NULL) )
    {
        PRINTF("could not allocate frame\n");
        //exit(1);
        ret = -1;
        goto end;
    }

    picture_size = avpicture_get_size(PIX_FMT_RGB24, video_dec_ctx->width, video_dec_ctx->height);
    rgb_buf = (uint8_t*)av_malloc(picture_size);
    if ( rgb_buf == NULL)
    {
        PRINTF("av malloc failed!\n");
        //exit(1);
        ret = -1;
        goto end;
    }
    avpicture_fill( (AVPicture*)frame_rgb, rgb_buf , PIX_FMT_RGB24, video_dec_ctx->width, video_dec_ctx->height);
    //设置图像转换上下文/
    sws_ctx = sws_getContext( video_dec_ctx->width, video_dec_ctx->height, video_dec_ctx->pix_fmt,
                              video_dec_ctx->width, video_dec_ctx->height, PIX_FMT_RGB24,
                              SWS_BICUBIC, NULL, NULL, NULL);

    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;

    //decode each frame
   // int i= 0;
  // FILE *fp =  fopen("844x560_t.yuv","wb+");
    while (av_read_frame(fmt_ctx, &pkt) >= 0)
    {
        //decode one frame
        if(pkt.stream_index == video_stream_idx)
        {
           ret = avcodec_decode_video2(video_dec_ctx, frame, &frame_finished, &pkt);
           if (ret < 0)
           {
                PRINTF("faid to decode frame\n");
           }

           if (frame_finished)
           {

         //   savefortest(frame,fp);

         // printf("%d ",frame->key_frame);

               ret = sws_scale(sws_ctx, (const uint8_t * const*)frame->data,frame->linesize, 0,
                       video_dec_ctx->height, frame_rgb->data,frame_rgb->linesize);
               //   printf("ret = %d\n",ret);
               //  char pic[200];
               //    sprintf(pic,"./pic/%d.bmp",i++);
               //  save_to_bmp(pic, frame_rgb->data[0], video_dec_ctx->width, video_dec_ctx->height,24 );
               if (frame_total == 0)
               {
                   //save the data
                   memcpy(buf, frame_rgb->data[0], picture_size);
                   av_free_packet(&pkt);
                   break;
               }
               else
               {
                   //save_to_replay(frame_rgb->data[0], video_dec_ctx->width, video_dec_ctx->height);
                   ptrFunc(frame_rgb->data[0], video_dec_ctx->width, video_dec_ctx->height);
               }
           }
        }
       // if (i == 10)break;
        av_free_packet(&pkt);
    }

   // fclose(fp);

end:
    //释放资源.......
    if (sws_ctx)
    {
        sws_freeContext(sws_ctx);
    }
    if (frame)
    {
        av_free(frame);
    }
    if(frame_rgb)
    {
        av_free(frame_rgb);
    }
    if (video_dec_ctx)
    {
        avcodec_close(video_dec_ctx);
    }

	if (rgb_buf)
	{
		av_free(rgb_buf);
	}

	if (fmt_ctx)
	{
		avformat_close_input(&fmt_ctx);
	}

    if (ret<0)
    {
        return false;
    }
    else
    {
        return true;
    }
}