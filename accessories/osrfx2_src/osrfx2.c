/*****************************************************************************/
/* osrfx2.c    A Driver for the OSR USB FX2 Learning Kit device              */
/*                                                                           */
/* Copyright (C) 2006 by Robin Callender                                     */
/*                                                                           */
/* This program is free software. You can redistribute it and/or             */
/* modify it under the terms of the GNU General Public License as            */
/* published by the Free Software Foundation, version 2.                     */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*				modified by QiuJiuming		 	     */
/*****************************************************************************/

#include <linux/version.h>
//#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kref.h>
#include <linux/poll.h>
#include <asm/uaccess.h>
#include <linux/usb.h>
#if HAVE_UNLOCKED_IOCTL
#include <linux/mutex.h>
#else
#include <linux/smp_lock.h>
#endif
#include "hal.h"

#define usb_buffer_alloc(x...) usb_alloc_coherent(x)
#define usb_buffer_free(x...) usb_free_coherent(x)

/*****************************************************************************/
/* Define the vendor id and product id.                                      */
/*****************************************************************************/
#define VENDOR_ID   0x0547       
#define PRODUCT_ID  0x1005

#define DEVICE_MINOR_BASE   192

#define CYPRESS_MAX_REQSIZE	8
             
#undef TRUE
#define TRUE  (1)
#undef FALSE
#define FALSE (0)

/*****************************************************************************/
/* Define the vendor commands supported by OSR USB FX2 device.               */
/*****************************************************************************/

#define OSRFX2_SET_XFER_START       0xBE

struct mutex hwlock;

/*****************************************************************************/
/* Table of devices that work with this driver                               */
/*****************************************************************************/
static struct usb_device_id id_table [] = {
    { USB_DEVICE( VENDOR_ID, PRODUCT_ID ) },
    { },
};

MODULE_DEVICE_TABLE(usb, id_table);

/*****************************************************************************/
/* This is the private device context structure.                             */
/*****************************************************************************/
struct osrfx2 {

    struct usb_device    * udev;
    struct usb_interface * interface;
    
    /*
     * This queue is used by the poll and irq methods
     */
    wait_queue_head_t FieldEventQueue;
    
    /*
     *  Transfer Buffers
     */
    unsigned char * ep0_buffer;
    unsigned char * bulk_in_buffer;
    unsigned char * bulk_out_buffer;
    
    /*
     *  Buffer sizes
     */
    size_t ep0_size;
    size_t bulk_in_size;
    size_t bulk_out_size;
    
    /*
     *  USB Endpoints
     */
    __u8  ep0_endpointAddr;
    __u8  bulk_in_endpointAddr;
    __u8  bulk_out_endpointAddr;
    
    /*
     *  Endpoint intervals
     */
    __u8  ep0_endpointInterval;
    __u8  bulk_in_endpointInterval;
    __u8  bulk_out_endpointInterval;
    
    /*
     *  URBs
     */
    struct urb * bulk_in_urb;
    //struct urb * int_in_urb;
    struct urb * bulk_out_urb;
    
    /*
     *  Refrence counter
     */
    struct kref      kref;
    struct semaphore sem;

    /*
     *  Data from interrupt is retained here.
     */
    //struct switches_state  switches;

    unsigned char notify;

    /*
     *  Track usage of the bulk pipes: serialize each pipe's use.
     */
    atomic_t bulk_write_available;
    atomic_t bulk_read_available;

    /*
     *  Data tracking for Read/Write. 
     *  Writes will add to the pending_data count and 
     *  reads will deplete the pending_data count.
     *
     *  Note: The OSRFX2 device specs states that the firmware will buffer
     *        up-to four write packet (two on EP6 and two on EP8).
     *        The buffers can be drained by issuing reads to EP8.
     *        The fifth outstanding write packet attempt will cause the write
     *        to block, waiting for the arrival of a read request to
     *        effectively free a buffer into which the write data can be 
     *        held.
     */
    size_t  pending_data;

    /*
     *  Power Managment related fields
     */
    int   suspended;        /* boolean */

};

/*****************************************************************************/
/* Forward declaration for our usb_driver definition later.                  */
/*****************************************************************************/
static struct usb_driver osrfx2_driver;

/* Send a vendor command to device */
static int vendor_command(struct usb_device *dev, unsigned char request, 
			  unsigned int value, unsigned int index,
			  void *buf, int size)
{
	return usb_control_msg(dev, usb_rcvctrlpipe(dev, 0),
			       request, 
			       USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_OTHER,
			       value, 
			       index, buf, size,
			       USB_CTRL_GET_TIMEOUT);
}

/*****************************************************************************/
/* This routine will set the bargraph LEDs.                                  */
/*                                                                           */
/* Note the two different function defintions depending on kernel version.   */
/*****************************************************************************/
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,13)
static ssize_t set_xfer_start(struct device * dev, 
                            const char * buf,
                            size_t count)
#else
static ssize_t set_xfer_start(struct device * dev, 
                            struct device_attribute * attr, 
                            const char * buf,
                            size_t count)
#endif
{
    struct usb_interface  * intf   = to_usb_interface(dev);
    struct osrfx2         * fx2dev = usb_get_intfdata(intf);
    //struct bargraph_state * packet;

    unsigned int value;
    int retval;
    char * end;
    unsigned char *iobuf;

    value = (simple_strtoul(buf, &end, 10) & 0xFF);
    if (buf == end) {
        value = 0;
    }
	/* allocate some memory for the i/o buffer*/
	iobuf = kzalloc(CYPRESS_MAX_REQSIZE, GFP_KERNEL);
/*
    retval = usb_control_msg(fx2dev->udev, 
                             usb_sndctrlpipe(fx2dev->udev, 0), 
                             OSRFX2_SET_BARGRAPH_DISPLAY, 
                             USB_DIR_OUT | USB_TYPE_VENDOR,
                             0,
                             0,
                             iobuf,//packet, 
                             CYPRESS_MAX_REQSIZE,//sizeof(*packet),
                             USB_CTRL_GET_TIMEOUT);
*/
		retval = vendor_command(fx2dev->udev, 
			  OSRFX2_SET_XFER_START, 0, 0,
			  iobuf, CYPRESS_MAX_REQSIZE);

    if (retval < 0) {
        dev_err(&fx2dev->udev->dev, "%s - retval=%d\n", 
                __FUNCTION__, retval);
    }
    
    //kfree(packet);
		kfree(iobuf);

    return count;
}

///////////////////////////////hzq///////////////////////
static ssize_t start_show(struct device * dev, 
                            struct device_attribute * attr, 
                            const char * buf)
{
    return scnprintf(buf, PAGE_SIZE, "%s\n", "osrfx2_usb");
}

/*****************************************************************************/
/* This macro creates an attribute under the sysfs directory                 */
/*   ---  /sys/bus/usb/devices/<root_hub>-<hub>:1.0/start                */
/*                                                                           */
/* The DEVICE_ATTR() will create "dev_attr_start".                        */
/* "dev_attr_start" is referenced in both probe and disconnect routines.  */
/*****************************************************************************/

static DEVICE_ATTR( start, 0660, start_show, set_xfer_start );

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
static int init_bulks(struct osrfx2 * fx2dev)
{
    fx2dev->bulk_in_buffer = kmalloc(fx2dev->bulk_in_size, GFP_KERNEL);
    if (!fx2dev->bulk_in_buffer) {
        return -ENOMEM;
    }
    fx2dev->bulk_out_buffer = kmalloc(fx2dev->bulk_out_size, GFP_KERNEL);
    if (!fx2dev->bulk_out_buffer) {
        return -ENOMEM;
    }

    //init_MUTEX( &fx2dev->sem );
    sema_init( &fx2dev->sem, 1 );
    init_waitqueue_head( &fx2dev->FieldEventQueue );

    return 0; 
}

/*****************************************************************************/
/* This routine will attempt to locate the required endpoints and            */
/* retain relevant information in the osrfx2 structure instance.             */
/*****************************************************************************/
static int find_endpoints(struct osrfx2 * fx2dev)
{
    struct usb_interface * interface = fx2dev->interface;
    struct usb_endpoint_descriptor * endpoint;
    unsigned char dir;
    unsigned char attr;
    int i;

    for (i=0; i < interface->cur_altsetting->desc.bNumEndpoints; i++) {

        endpoint = &interface->cur_altsetting->endpoint[i].desc;
        dir  = endpoint->bEndpointAddress & USB_DIR_IN;
        attr = endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK;

        switch ((dir << 8) + attr) {
 /*           case ((USB_DIR_OUT << 8) + USB_ENDPOINT_XFER_CONTROL) :
                fx2dev->ep0_endpointAddr = endpoint->bEndpointAddress;
                fx2dev->ep0_endpointInterval = endpoint->bInterval;
                fx2dev->ep0_size = endpoint->wMaxPacketSize;
                break;
*/
            case ((USB_DIR_IN << 8) + USB_ENDPOINT_XFER_BULK) :
                fx2dev->bulk_in_endpointAddr = endpoint->bEndpointAddress;
                fx2dev->bulk_in_endpointInterval = endpoint->bInterval;
                fx2dev->bulk_in_size = endpoint->wMaxPacketSize;
                break;
            case ((USB_DIR_OUT << 8) + USB_ENDPOINT_XFER_BULK) :
                fx2dev->bulk_out_endpointAddr = endpoint->bEndpointAddress;
                fx2dev->bulk_out_endpointInterval = endpoint->bInterval;
                fx2dev->bulk_out_size = endpoint->wMaxPacketSize;
                break;
            default:
                break;
        }
    }

    if (//fx2dev->int_in_endpointAddr   == 0 || 
        fx2dev->bulk_in_endpointAddr  == 0 ||
        fx2dev->bulk_out_endpointAddr == 0) {
        dev_err(&interface->dev, "%s - failed to find required endpoints\n", 
                __FUNCTION__);
        return -ENODEV;
    }
    return 0;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
static void osrfx2_delete(struct kref * kref)
{
    struct osrfx2 * fx2dev = container_of(kref, struct osrfx2, kref);

    usb_put_dev( fx2dev->udev );
    
    /*if (fx2dev->int_in_urb) {
        usb_free_urb(fx2dev->int_in_urb);
    }
    if (fx2dev->int_in_buffer) {
        kfree(fx2dev->int_in_buffer);
    }*/
    if (fx2dev->bulk_in_buffer) {
        kfree( fx2dev->bulk_in_buffer );
    }
    if (fx2dev->bulk_out_buffer) {
        kfree( fx2dev->bulk_out_buffer );
    }

    kfree( fx2dev );
}

/*****************************************************************************/
/* osrfx2_open                                                               */
/*                                                                           */
/* Note:                                                                     */
/*   The serialization method used below has a side-effect which I don't     */
/*   particularly care for. In effect switch_events and bulk I/O are         */
/*   mutually exclusive, e.g. an open for switch_events will exclude         */
/*   opens for bulk I/O (osrbulk) and vis-a-verse.                           */
/*                                                                           */
/* Note:                                                                     */
/*   The usb_clear_halt() is being used to effect a pipe reset.              */
/*   This will clear any residual data at the endpoint and ready it for      */
/*   the new endpoint user's data.                                           */
/*****************************************************************************/
static int osrfx2_open(struct inode * inode, struct file * file)
{
    struct usb_interface * interface;
    struct osrfx2 * fx2dev;
    int retval;
    int flags;
    
    interface = usb_find_interface(&osrfx2_driver, iminor(inode));
    if (interface == NULL) 
        return -ENODEV;

    fx2dev = usb_get_intfdata(interface);
    if (fx2dev == NULL) 
        return -ENODEV;

    /*
     *   Serialize access to each of the bulk pipes.
     */ 
    flags = (file->f_flags & O_ACCMODE);

    if ((flags == O_WRONLY) || (flags == O_RDWR)) {
        if (atomic_dec_and_test( &fx2dev->bulk_write_available ) == 0) {
            atomic_inc( &fx2dev->bulk_write_available );
            return -EBUSY;
        }

        /*
         *   The write interface is serialized, so reset bulk-out pipe (ep-6).
         */
        retval = usb_clear_halt(fx2dev->udev, fx2dev->bulk_out_endpointAddr);
        if ((retval != 0) && (retval != -EPIPE)) {
            dev_err(&interface->dev, "%s - error(%d) usb_clear_halt(%02X)\n", 
                    __FUNCTION__, retval, fx2dev->bulk_out_endpointAddr);
        }
    }

    if ((flags == O_RDONLY) || (flags == O_RDWR)) {
        if (atomic_dec_and_test( &fx2dev->bulk_read_available ) == 0) {
            atomic_inc( &fx2dev->bulk_read_available );
            if (flags == O_RDWR) 
                atomic_inc( &fx2dev->bulk_write_available );
            return -EBUSY;
        }

        /*
         *   The read interface is serialized, so reset bulk-in pipe (ep-8).
         */
        retval = usb_clear_halt(fx2dev->udev, fx2dev->bulk_in_endpointAddr);
        if ((retval != 0) && (retval != -EPIPE)) {
            dev_err(&interface->dev, "%s - error(%d) usb_clear_halt(%02X)\n", 
                    __FUNCTION__, retval, fx2dev->bulk_in_endpointAddr);
        }
    }

    /*
     *   Set this device as non-seekable.
     */ 
    retval = nonseekable_open(inode, file);
    if (retval != 0) {
        return retval;
    }

    /*
     *   Increment our usage count for the device.
     */
    kref_get(&fx2dev->kref);

    /*
     *   Save pointer to device instance in the file's private structure.
     */
    file->private_data = fx2dev;

    return 0;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
static int osrfx2_release(struct inode * inode, struct file * file)
{
    struct osrfx2 * fx2dev;
    int flags;

    fx2dev = (struct osrfx2 *)file->private_data;
    if (fx2dev == NULL)
        return -ENODEV;

    /* 
     *  Release any bulk_[write|read]_available serialization.
     */
    flags = (file->f_flags & O_ACCMODE);

    if ((flags == O_WRONLY) || (flags == O_RDWR))
        atomic_inc( &fx2dev->bulk_write_available );

    if ((flags == O_RDONLY) || (flags == O_RDWR)) 
        atomic_inc( &fx2dev->bulk_read_available );

    /* 
     *  Decrement the ref-count on the device instance.
     */
    kref_put(&fx2dev->kref, osrfx2_delete);
    
    return 0;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
static void write_bulk_backend(struct urb * urb, struct pt_regs * regs)
{
    struct osrfx2 * fx2dev = (struct osrfx2 *)urb->context;

    /* 
     *  Filter sync and async unlink events as non-errors.
     */
    if (urb->status && 
        !(urb->status == -ENOENT || 
          urb->status == -ECONNRESET ||
          urb->status == -ESHUTDOWN)) {
        dev_err(&fx2dev->interface->dev, 
                "%s - non-zero status received: %d\n",
                __FUNCTION__, urb->status);
    }
    /* 
     *  Free the spent buffer.
     */
    usb_buffer_free( urb->dev, 
                     urb->transfer_buffer_length, 
                     urb->transfer_buffer, 
                     urb->transfer_dma );
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
static ssize_t osrfx2_read(struct file * file, unsigned char * buffer, 
                           size_t count, loff_t * ppos)
{
    struct osrfx2 * fx2dev;
    int retval = 0;
    int bytes_read;
    int pipe;
    unsigned char *buff;

    fx2dev = (struct osrfx2 *)file->private_data;
    buff=kmalloc(count, GFP_KERNEL);
    pipe = usb_rcvbulkpipe(fx2dev->udev, fx2dev->bulk_in_endpointAddr),
	//printk("bulk in size = %d\n", fx2dev->bulk_in_size);
	//printk("pipe = %d, ep = %d\n", pipe, fx2dev>bulk_in_endpointAddr);

    /* 
     *  Do a blocking bulk read to get data from the device 
     */
    retval = usb_bulk_msg( fx2dev->udev, 
                           pipe,
                           buff,
                           count,
                           &bytes_read, 
                        //   0);
                           500 );	//ms

    /* 
     *  If the read was successful, copy the data to userspace 
     */
	//printk("byte read = %d\n", bytes_read);
    if (!retval) {
        if (copy_to_user(buffer, buff, bytes_read)) {
            retval = -EFAULT;
        }
        else {
            retval = bytes_read;
        }
        /*
         *  Increment the pending_data counter by the byte count received.
         */
        fx2dev->pending_data -= retval;
    }
    kfree(buff);
    return retval;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
static ssize_t osrfx2_write(struct file * file, unsigned char * user_buffer, 
                            size_t count, loff_t * ppos)

{
    struct osrfx2 * fx2dev;
    struct urb * urb = NULL;
    unsigned char * buf = NULL;
    int pipe;
    int retval = 0;

    fx2dev = (struct osrfx2 *)file->private_data;

    if (count == 0)
        return count;
    /* 
     *  Create a urb, and a buffer for it, and copy the data to the urb.
     */
    urb = usb_alloc_urb(0, GFP_KERNEL);
    if (!urb) {
        retval = -ENOMEM;
        goto error;
    }

    buf = usb_buffer_alloc( fx2dev->udev, 
                            count, 
                            GFP_KERNEL, 
                            &urb->transfer_dma );
    if (!buf) {
        retval = -ENOMEM;
        goto error;
    }

    if (copy_from_user(buf, user_buffer, count)) {
        retval = -EFAULT;
        goto error;
    }
    /* 
     *  Initialize the urb properly.
     */
    pipe = usb_sndbulkpipe( fx2dev->udev, fx2dev->bulk_out_endpointAddr );

    usb_fill_bulk_urb( urb, 
                       fx2dev->udev,
                       pipe,
                       buf, 
                       count, 
                       write_bulk_backend, 
                       fx2dev );

    urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
    /* 
     *  Send the data out the bulk port
     */
    retval = usb_submit_urb(urb, GFP_KERNEL);
    if (retval) {
        dev_err(&fx2dev->interface->dev, "%s - usb_submit_urb failed: %d\n",
                __FUNCTION__, retval);
        goto error;
    }
    /*
     *  Increment the pending_data counter by the byte count sent.
     */
    fx2dev->pending_data += count;
    /* 
     *  Release the reference to this urb, the USB core
     *  will eventually free it entirely.
     */
    usb_free_urb(urb);

    return count;

error:
    usb_buffer_free(fx2dev->udev, count, buf, urb->transfer_dma);
    usb_free_urb(urb);
    return retval;
}

/****************************************************************************/
/*	        osrfx2_ioctl                                                */
/****************************************************************************/
#if 1
//static int osrfx2_ioctl (struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
static int osrfx2_ioctl (struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret = -1;
	struct osrfx2 *fx2dev;
	struct ezusb_control usb_ctl;
	struct ezusb_control_probe usb_ctl_probe;
	const int SIZE = 64;
	unsigned char buf_tmp[SIZE];
	//int i;

	memset(buf_tmp, 0, SIZE);
	fx2dev = (struct osrfx2 *)file->private_data;
	switch(cmd)
	{
		case EZUSB_IOC_REQUEST:
			copy_from_user(&usb_ctl, (struct ezusb_control *)(arg), sizeof(usb_ctl));
			ret=vendor_command(fx2dev->udev, usb_ctl.request, (unsigned char)usb_ctl.value, 0xBEEF, buf_tmp, SIZE);
			
			if(ret<0)
				return -1;
			
			if((ret==SIZE)&&(usb_ctl.buf!=NULL))
			{
				copy_to_user((struct ezusb_control *)(arg), &usb_ctl, sizeof(usb_ctl));
				copy_to_user(usb_ctl.buf, buf_tmp, sizeof(buf_tmp));
			}
			return 0;

		case EZUSB_IOC_READPROBE:
			copy_from_user(&usb_ctl_probe, (struct ezusb_control_probe *)(arg), sizeof(usb_ctl_probe));
			ret=vendor_command(fx2dev->udev, usb_ctl_probe.request, usb_ctl_probe.value, 0xBEEF, buf_tmp, SIZE);
			
			//for(i = 0; i < 64; i++)
			//{	printk("read probe data: %d = %d\n", i, buf_tmp[i]);
			//}	
			if((ret<0) || (buf_tmp[63] != 0x5a))
			//if((ret<0))
				return -1;

			if((ret==SIZE)&&(usb_ctl_probe.buf!=NULL))
			{	
				copy_to_user((struct ezusb_control_probe *)(arg), &usb_ctl_probe, sizeof(usb_ctl_probe));
				if (usb_ctl_probe.len > SIZE)
					copy_to_user(usb_ctl_probe.buf, buf_tmp, SIZE);
				else
					copy_to_user(usb_ctl_probe.buf, buf_tmp, usb_ctl_probe.len);
			}
			return 0;

		default:
			return -ENOTTY;
	}
	return -ENOTTY;
	
}
#endif

#if 0
static int osrfx2_ioctl (struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret = -1;
	int i;
	struct osrfx2 *fx2dev;
	struct ezusb_control usb_ctl;
	struct ezusb_control_probe usb_ctl_probe;
	unsigned char buf_tmp[64];
	unsigned char buf[100];
	unsigned short offset;
	
	memset(buf_tmp, 0, 64);
	fx2dev = (struct osrfx2 *)file->private_data;
	switch(cmd)
	{
		case EZUSB_IOC_REQUEST:
			copy_from_user(&usb_ctl, (struct ezusb_control *)(arg), sizeof(usb_ctl));
			ret=vendor_command(fx2dev->udev, usb_ctl.request, (unsigned char)usb_ctl.value, 0xBEEF, buf_tmp, 64);
			
			if(ret<0)
				return ret;
			
			if((ret==64)&&(usb_ctl.buf!=NULL))
			{
				copy_to_user((struct ezusb_control *)(arg), &usb_ctl, sizeof(usb_ctl));
				copy_to_user(usb_ctl.buf, buf_tmp, sizeof(buf_tmp));
			}
			return 0;

		case EZUSB_IOC_READPROBE:
			copy_from_user(&usb_ctl_probe, (struct ezusb_control_probe *)(arg), sizeof(usb_ctl_probe));

			memset(buf, 0, usb_ctl_probe.len);

			for(offset=0;offset<(unsigned short)usb_ctl_probe.len;offset+=64)
			{
				ret=vendor_command(fx2dev->udev, usb_ctl_probe.request, (unsigned short)(usb_ctl_probe.value+offset), 0xBEEF, buf_tmp, 64);
				
				if(ret<0)
					return ret;
if((offset==64)&&(ret==64)&&(usb_ctl_probe.buf!=NULL))
{

dev_err(&fx2dev->interface->dev, "--------->ret= %d <---------\n", ret);
for(i=0;i<64;i++)
dev_err(&fx2dev->interface->dev, "--------->buf_tmp[%d]= %d <---------\n", i,buf_tmp[i]);

}
				if((ret==64)&&(usb_ctl_probe.buf!=NULL))
				{	
					//if(usb_ctl_probe.len-offset>64)
						memcpy(buf+(unsigned char)offset, buf_tmp, 64);
					//else
					//	memcpy(buf+offset, buf_tmp, usb_ctl_probe.len-offset);
				}
for(i=0;i<5000000;i++) {}
			}
			copy_to_user((struct ezusb_control_probe *)(arg), &usb_ctl_probe, sizeof(usb_ctl_probe));
			copy_to_user(usb_ctl_probe.buf, buf, sizeof(buf));
			return 0;

		default:
			return -ENOTTY;
	}
	return -ENOTTY;
	
}
#endif

/*****************************************************************************/
/* This fills-in the driver-supported file_operations fields.                */
/*****************************************************************************/
static struct file_operations osrfx2_file_ops = {
    .owner   = THIS_MODULE,
    .open    = osrfx2_open,
    .release = osrfx2_release,
    .read    = osrfx2_read,
    .write   = osrfx2_write,
    //.ioctl   = osrfx2_ioctl,
    .unlocked_ioctl   = osrfx2_ioctl,
};
 
/*****************************************************************************/
/* Usb class driver info in order to get a minor number from the usb core,   */
/* and to have the device registered with devfs and the driver core.         */
/*****************************************************************************/
static struct usb_class_driver osrfx2_class = {
//    .name       = "device/osrfx2_%d",
    .name       = "device/osrfx2_emp",
    .fops       = &osrfx2_file_ops,
    .minor_base = DEVICE_MINOR_BASE,
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,13)
    .mode       = S_IFCHR | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH,
#endif
};

/*****************************************************************************/
/* Event: un-bound device instance is querying for suitable owner driver.    */
/*****************************************************************************/
static int osrfx2_probe(struct usb_interface * interface, 
                        const struct usb_device_id * id)
{
    struct usb_device * udev = interface_to_usbdev(interface);
    struct osrfx2 * fx2dev = NULL;
    int retval;

    fx2dev = kmalloc(sizeof(struct osrfx2), GFP_KERNEL);
    if (fx2dev == NULL) {
        retval = -ENOMEM;
        goto error;
    }
    memset(fx2dev, 0, sizeof(*fx2dev));
    kref_init( &fx2dev->kref );

    fx2dev->udev = usb_get_dev(udev);
    fx2dev->interface = interface;
    fx2dev->suspended = FALSE;

    fx2dev->bulk_write_available = (atomic_t) ATOMIC_INIT(1);
    fx2dev->bulk_read_available  = (atomic_t) ATOMIC_INIT(1);

    usb_set_intfdata(interface, fx2dev);

    device_create_file(&interface->dev, &dev_attr_start);

    retval = find_endpoints( fx2dev );
    if (retval != 0) 
        goto error;

    retval = init_bulks( fx2dev );
    if (retval != 0)
        goto error;

    retval = usb_register_dev(interface, &osrfx2_class);
    if (retval != 0) {
        usb_set_intfdata(interface, NULL);
    }

    mutex_init(&hwlock);

    dev_info(&interface->dev, "OSR USB-FX2 device now attached.\n");
    return 0;

error:
    dev_err(&interface->dev, "OSR USB-FX2 device probe failed: %d.\n", retval);
    if (fx2dev) {
        kref_put( &fx2dev->kref, osrfx2_delete );
    }
    return retval;
}

/*****************************************************************************/
/* Event: device instance is being disconnected (deleted)                    */
/*****************************************************************************/
static void osrfx2_disconnect(struct usb_interface * interface)
{
    struct osrfx2 * fx2dev;

    //lock_kernel();

    fx2dev = usb_get_intfdata(interface);
    
    usb_set_intfdata(interface, NULL);

    device_remove_file(&interface->dev, &dev_attr_start);

    usb_deregister_dev(interface, &osrfx2_class);

    //unlock_kernel();

    kref_put( &fx2dev->kref, osrfx2_delete );

    dev_info(&interface->dev, "OSR USB-FX2 now disconnected.\n");
}

/*****************************************************************************/
/* This driver's usb_driver structure: ref-ed by osrfx2_init and osrfx2_exit */
/*****************************************************************************/
static struct usb_driver osrfx2_driver = {
    .name        = "osrfx2",
    .probe       = osrfx2_probe,
    .disconnect  = osrfx2_disconnect,
    .id_table    = id_table,
};

/*****************************************************************************/
/* This driver's commission routine: just register with USB subsystem.       */
/*****************************************************************************/
static int __init osrfx2_init(void)
{
    int retval;

    retval = usb_register(&osrfx2_driver);

    return retval;
}

/*****************************************************************************/
/* This driver's decommission routine: just deregister with USB subsystem.   */
/*****************************************************************************/
static void __exit osrfx2_exit(void)
{
    usb_deregister( &osrfx2_driver );
}

/*****************************************************************************/
/* Advertise this driver's init and exit routines                            */
/*****************************************************************************/
module_init( osrfx2_init );
module_exit( osrfx2_exit );

MODULE_AUTHOR("QiuJiuming");
MODULE_DESCRIPTION("A driver for the FCUSB-CY7C68013-128 board");
MODULE_LICENSE("GPL");

