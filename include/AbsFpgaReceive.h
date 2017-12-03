#ifndef __ABSFPGARECEIVE_H__
#define __ABSFPGARECEIVE_H__

class AbsFpgaReceive
{
	public:

	
	/**
	* @brief service routine only used to receive data from usb device
	*/
	virtual void UsbServiceRoutine(void *arg)=0;

};



#endif 
