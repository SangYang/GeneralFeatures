#ifndef SSY_FFMPEG_PACKAGE_H
#define SSY_FFMPEG_PACKAGE_H


#define PIXEL_FMT_YUV420P  0  ///< planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples)
#define PIXEL_FMT_YUV422P  1  ///< planar YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)
#define PIXEL_FMT_YUV444P  2  ///< planar YUV 4:4:4, 24bpp, (1 Cr & Cb sample per 1x1 Y samples)
#define PIXEL_FMT_YUV410P  3  ///< planar YUV 4:1:0,  9bpp, (1 Cr & Cb sample per 4x4 Y samples)
#define PIXEL_FMT_YUV411P  4  ///< planar YUV 4:1:1, 12bpp, (1 Cr & Cb sample per 4x1 Y samples)
#define PIXEL_FMT_YUYV422  5  ///< packed YUV 4:2:2, 16bpp, Y0 Cb Y1 Cr
#define PIXEL_FMT_YUVJ420P 6  ///< planar YUV 4:2:0, 12bpp, full scale (JPEG)
#define PIXEL_FMT_YUVJ422P 7  ///< planar YUV 4:2:2, 16bpp, full scale (JPEG)
#define PIXEL_FMT_YUVJ444P 8  ///< planar YUV 4:4:4, 24bpp, full scale (JPEG)
#define PIXEL_FMT_RGB24    9  ///< packed RGB 8:8:8, 24bpp, RGBRGB...
#define PIXEL_FMT_BGR24    10 ///< packed RGB 8:8:8, 24bpp, BGRBGR...

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	int JPG_to_Pixel(const unsigned char *jpgBuff, int jpgSize, int pixelFmt, int pixelWidth, int pixelHeight, unsigned char *pixelBuff, int *pixelSize);	
	int Pixel_to_JPG(const unsigned char *pixelBuff, int pixelSize, int pixelFmt, int pixelWidth, int pixelHeight, unsigned char *jpgBuff, int *jpgSize);
	int Pixel_to_Pixel(const unsigned char *inPixBuff, int inPixSize, int inPixFmt, int inPixWidth, int inPixHeight, 
					   unsigned char *outPixBuff, int *outPixSize, int outPixFmt, int outPixWidth, int outPixHeight);
	int BGR24_to_BMP(const unsigned char *bgr24Buff, int bgr24Size, int bgr24Width, int bgr24Height, unsigned char *bmpBuff, int *bmpSize);
	int BMP_to_BGR24(const unsigned char *bmpBuff, int bmpSize, int *bgr24Width, int *bgr24Height, unsigned char *bgr24Buff, int *bgr24Size);

#ifdef __cplusplus
};
#endif // __cplusplus


#endif // SSY_FFMPEG_PACKAGE_H

