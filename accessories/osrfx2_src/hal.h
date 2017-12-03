#ifndef __HAL_H__
#define __HAL_H__


struct ezusb_control
{
	unsigned char request;//command
	int value; //value to set
	int len; // len of date returned in buf; 
	unsigned char *buf;
};

struct ezusb_control_probe
{
	unsigned char request;//command
	unsigned short value; //value to set
	int len; // len of date returned in buf; 
	unsigned char *buf;
};

struct ezusb_bulk_out
{
	unsigned char data_type;//bits of data,BITS8-8bits BITS16-16bits BITS32-32bits
	unsigned char para_len; //length of data in type of data_type
	unsigned char fpga_addr;// fpga address of receive data
	unsigned char *buf; //buffer of data send to fpga
};

#define OUR_MAGIC       'o'
#define EZUSB_IOC_MAXNR	3
#define EZUSB_IOC_REQUEST 		_IOW(OUR_MAGIC, 1, struct ezusb_control)
#define EZUSB_IOC_READPROBE		_IOW(OUR_MAGIC, 2, struct ezusb_control_probe)
#define EZUSB_IOC_BULK_READ		_IOR(OUR_MAGIC, 3, struct ezusb_bulk_out)

#define  PktPerBlock	16

//define FPGA addr
//#define FPGA_BASE_ADDR0   0x28000000
#define FOC_A           0x00
#define FOC_B           0x04
#define FOC_C           0x08
#define FOC_D           0x0c
#define SAMPLE          0x10
#define FPGA_ADDR       0x14
#define FPGA_DATA       0x18
#define SCAN_RANGE      0x1c
#define FOCUS_ADDR      0x28
#define FOCUS_DATA      0x2c
#define APERTURE_DATA   0x30
#define SAMPLE_48       0x34
#define DEMODE          0x38

//#define FPGA_BASE_ADDR1 0x00
#define SAMPLE_1024     0x40
#define PROBE_EMIT1     0x44
#define PROBE_EMIT2     0x48
#define IMAGE_CTL       0x4c
#define BM_LINE         0x50
#define FILTER_ADDR     0x54
#define FILTER_DATA     0x58
#define EDGE_ENHANCE    0x5c
#define RUN_EMIT        0x60
#define CLK_NUM         0x64


#endif
