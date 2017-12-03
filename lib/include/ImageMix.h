/***************************************************************
 * Project:	  libImageMix.so
 * Name:      ImageMix.h
 * Purpose:   Code for Application Class
 * Author:    ZCT (zhouchuantao@emperor1997.com)
 * Created:   2010-11-24
 * Copyright: EMP
 * License:
 *
 * Note: interface for the image_enhance mix.
 * image size：628*440*3byte
 * other:该文件与libImageMix.so(.a)、libippi.so.6.1
 *       libippiv8.so.6.1一起使用
 **************************************************************/
////////////////////////////////////////////////////////////////

#ifndef  DLL_API
#define  DLL_API
#endif

#ifndef IMAGE_MIX_H_INCLUDED
#define IMAGE_MIX_H_INCLUDED


#ifdef __cplusplus
extern "C"{
#endif

// Imagemix function interface
#if 1
// 8bit  one Channel
DLL_API void ImageMixC1R(unsigned char* pImageDst, 			 		// output mixed image data
					  const unsigned char* pImageSrc0, 				// input image data
					  const unsigned char* pImageSrc1, 				// input image data
					  const int widthDst, const int heightDst, 		// destination image width and height
					  const int widthSrc0, const int heightSrc0,	// source0 image width and height
					  const int widthSrc1, const int heightSrc1, 	// source1 image width and height
					  const int roiWidthSrc, const int roiHeightSrc,// roi of source image width and height
					  unsigned char colorKey     					// key color for mask
					 );

// 24bit  three Channel
DLL_API void ImageMixC3R(unsigned char* pImageDst, 			 		// output mixed image data
					  const unsigned char* pImageSrc0, 				// input image data
					  const unsigned char* pImageSrc1, 				// input image data
					  const int widthDst, const int heightDst, 		// destination image width and height
					  const int widthSrc0, const int heightSrc0,	// source0 image width and height
					  const int widthSrc1, const int heightSrc1, 	// source1 image width and height
					  const int roiWidthSrc, const int roiHeightSrc,// roi of source image width and height
					  unsigned char colorKey[3] 					// key color for mask
					 );

// 32bit  four Channel
DLL_API void ImageMixC4R(unsigned char* pImageDst, 			 		// output mixed image data 32bits
					  const unsigned char* pImageSrc0, 				// input image data  32bits
					  const unsigned char* pImageSrc1, 				// input image data  32bits
					  const int widthDst, const int heightDst, 		// destination image width and height
					  const int widthSrc0, const int heightSrc0,	// source0 image width and height
					  const int widthSrc1, const int heightSrc1, 	// source1 image width and height
					  const int roiWidthSrc, const int roiHeightSrc,// roi of source image width and height
					  unsigned char colorKey[4] 					// key color for mask
					 );

// four and three channels(input:32bits and 24 bits  output:32bits)
DLL_API void ImageMixC4C3R(unsigned char* pImageDst, 				// output mixed image data  32bits
					  const unsigned char* pImageSrc0, 				// input image data         32bits         
					  const unsigned char* pImageSrc1, 				// input image data         24bits
					  const int widthDst, const int heightDst, 		// destination image width and height
					  const int widthSrc0, const int heightSrc0,	// source0 image width and height
					  const int widthSrc1, const int heightSrc1, 	// source1 image width and height
					  const int roiWidthSrc, const int roiHeightSrc,// roi of source image width and height
					  unsigned char colorKey[4], 					// key color for mask
                      int order[4]                                  // Src1中彩色的位置相对于Src0的位置  
					 );

// three and four channels(input:24bits and 32 bits  output:24bits)
DLL_API void ImageMixC3C4R(unsigned char* pImageDst, 	    		// output mixed image data  24bits
					  const unsigned char* pImageSrc0, 				// input image data         32bits
					  const unsigned char* pImageSrc1, 				// input image data         24bits
					  const int widthDst, const int heightDst, 		// destination image width and height
					  const int widthSrc0, const int heightSrc0,	// source0 image width and height
					  const int widthSrc1, const int heightSrc1, 	// source1 image width and height
					  const int roiWidthSrc, const int roiHeightSrc,// roi of source image width and height
					  unsigned char colorKey[3], 					// key color for mask
                      int order[3]                                  // Src1中彩色的位置相对于Src0的位置
					 );
#else
DLL_API void ImageMix(unsigned char* pImageDst, 			 		// output mixed image data
					  unsigned char* pImageSrc, 					// input image data
					  const int widthDst, const int heightDst, 		// destination image width and height
					  const int widthSrc, const int heightSrc, 		// source image width and height
					  const int roiWidthSrc, const int roiHeightSrc,// roi of source image width and height
					  unsigned char colorKey[3] 					// key color for mask
					 );

#endif
#ifdef __cplusplus
}
#endif


#endif // IMAGE_MIX_H_INCLUDE

