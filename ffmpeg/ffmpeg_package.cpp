#if 1
#include "ffmpeg_package.h"
#include <errno.h>
#include <math.h>
#include <stdio.h>  
#include "common_log.h"
#include "debug_assert.h"
extern "C" {  
	#include "libavutil/opt.h"  
	#include "libavcodec/avcodec.h"  
	#include "libavformat/avformat.h"  
	#include "libswscale/swscale.h"
};	 

#pragma comment(lib, "avcodec")
#pragma comment(lib, "avformat")
#pragma comment(lib, "swscale")
#pragma comment(lib, "avutil")

static int PF(int pixFmt) {
	int avpixFmt;
	switch (pixFmt) {
		case PIXEL_FMT_YUV420P: avpixFmt = AV_PIX_FMT_YUV420P; break;
		case PIXEL_FMT_YUV422P: avpixFmt = AV_PIX_FMT_YUV422P; break;
		case PIXEL_FMT_YUV444P: avpixFmt = AV_PIX_FMT_YUV444P; break;
		case PIXEL_FMT_YUV410P: avpixFmt = AV_PIX_FMT_YUV410P; break;
		case PIXEL_FMT_YUV411P: avpixFmt = AV_PIX_FMT_YUV411P; break;
		case PIXEL_FMT_YUYV422: avpixFmt = AV_PIX_FMT_YUYV422; break;
		case PIXEL_FMT_YUVJ420P: avpixFmt = AV_PIX_FMT_YUVJ420P; break;
		case PIXEL_FMT_YUVJ422P: avpixFmt = AV_PIX_FMT_YUVJ422P; break;
		case PIXEL_FMT_YUVJ444P: avpixFmt = AV_PIX_FMT_YUVJ444P; break;
		case PIXEL_FMT_RGB24: avpixFmt = AV_PIX_FMT_RGB24; break;
		case PIXEL_FMT_BGR24: avpixFmt = AV_PIX_FMT_BGR24; break;
		default: avpixFmt = -1; break;
	}
	return avpixFmt;
}

static int save_YUV420P(const AVFrame *frame, int width, int height, unsigned char *pixelBuff, int *pixelSize) {
	const int c_PixelSizeMin = width*height*3/2;
	unsigned char *yData = frame->data[0];
	unsigned char *uData = frame->data[1];
	unsigned char *vData = frame->data[2];
	int yLS = frame->linesize[0];
	int uLS = frame->linesize[1];
	int vLS = frame->linesize[2];
	int index ;

	if (c_PixelSizeMin <= *pixelSize) {
		*pixelSize = c_PixelSizeMin;
		for (index = 0; index < height; index++)
			memcpy(pixelBuff+index*width, yData+yLS*index, width);
		for (index = 0; index < height/2; index++)
			memcpy(pixelBuff+width*height+index*width/2, uData+uLS*index, width/2);
		for (index = 0; index < height/2; index++)
			memcpy(pixelBuff+width*height*5/4+index*width/2, vData+vLS*index, width/2);
		return 0;
	}
	else
		return -1;
}

int JPG_to_Pixel(const unsigned char *jpgBuff, int jpgSize, int pixelFmt, int pixelWidth, int pixelHeight, unsigned char *pixelBuff, int *pixelSize) {	
	AVFormatContext *formatContext;
	AVInputFormat *inputFormat;
	AVIOContext *ioContext;
	AVStream *stream;
	AVCodecContext *codecContext;
	AVCodec *codec;
	AVFrame *frame, *frame2;
	AVPacket packet;
	struct SwsContext *swsContext;
	int streamIndex;
	int gotFrame;
	int codecRet;
	int result = -1;

	av_register_all();
	formatContext = avformat_alloc_context();
	ioContext = avio_alloc_context((unsigned char *)jpgBuff, jpgSize, 0, NULL, NULL, NULL, NULL);
	inputFormat = av_find_input_format("mjpeg");
	av_probe_input_buffer2(ioContext, &inputFormat, NULL, NULL, 0, 0);
	formatContext->pb = ioContext;
	formatContext->iformat = inputFormat;
	avformat_open_input(&formatContext, NULL, NULL, NULL);
	av_find_stream_info(formatContext);

	av_init_packet(&packet);
	for (streamIndex = 0; streamIndex < formatContext->nb_streams; streamIndex++) {
		av_read_frame(formatContext, &packet);
		if (formatContext->streams[streamIndex]->codec->codec_type == AVMEDIA_TYPE_VIDEO && 0 < packet.size) {
			stream = formatContext->streams[streamIndex];
			codecContext = stream->codec;
			codec = avcodec_find_decoder(codecContext->codec_id);
			avcodec_open2(codecContext, codec, NULL);
			frame = avcodec_alloc_frame();
			codecRet = avcodec_decode_video2(codecContext, frame, &gotFrame, &packet);
			if (0 <= codecRet && 1 == gotFrame) {
				frame2 = av_frame_clone(frame);
				frame2->format = PF(pixelFmt);
				swsContext = sws_getContext(codecContext->width, codecContext->height, codecContext->pix_fmt, pixelWidth, pixelHeight, (AVPixelFormat)frame2->format, SWS_BICUBIC, NULL, NULL, NULL);   
				sws_scale(swsContext, (const uint8_t *const *)frame->data, frame->linesize, 0, codecContext->height, frame2->data, frame2->linesize);  
				sws_freeContext(swsContext);

				*pixelSize = avpicture_layout((const AVPicture *)frame2, (enum AVPixelFormat)frame2->format, pixelWidth, pixelHeight, pixelBuff, *pixelSize);
				result = *pixelSize;

				av_frame_free(&frame2);
			}	
			if (1 == codecContext->refcounted_frames) av_frame_unref(frame); 
			avcodec_free_frame(&frame);
			avcodec_close(codecContext);
		}
		av_free_packet(&packet);
		if (-1 != result)
			break;
	}

	avformat_close_input(&formatContext);
	av_free(ioContext->buffer);
	av_free(ioContext);
	avformat_free_context(formatContext);
	return result;
}

int Pixel_to_JPG(const unsigned char *pixelBuff, int pixelSize, int pixelFmt, int pixelWidth, int pixelHeight, unsigned char *jpgBuff, int *jpgSize) {
	AVFormatContext *formatContext;
	AVOutputFormat *outputFormat;
	AVIOContext *ioContext;
	AVStream *stream;
	AVCodecContext *codecContext;
	AVCodec *codec;
	AVFrame *frame;
	AVPacket packet;
	int ioRet;
	int codecRet;
	int gotPacket;
	int pixelSizeMin;
	int result = -1;

	av_register_all();
	formatContext = avformat_alloc_context();
	outputFormat = av_guess_format("mjpeg", NULL, NULL);
	ioContext = avio_alloc_context(jpgBuff, *jpgSize, 0, NULL, NULL, NULL, NULL);
	formatContext->oformat = outputFormat;
	formatContext->pb = ioContext;
	stream = av_new_stream(formatContext, 0);
	codecContext = stream->codec;
	codecContext->codec_id = outputFormat->video_codec;
	codecContext->codec_type = AVMEDIA_TYPE_VIDEO;
	codecContext->pix_fmt = (enum AVPixelFormat)PF(pixelFmt);
	codecContext->width = pixelWidth;  
	codecContext->height = pixelHeight;
	codecContext->time_base.num = 1;  
	codecContext->time_base.den = 25;   
	codec = avcodec_find_encoder(codecContext->codec_id);
	avcodec_open2(codecContext, codec, NULL);

	avformat_write_header(formatContext, NULL);
	pixelSizeMin = avpicture_get_size(codecContext->pix_fmt, codecContext->width, codecContext->height);
	if (pixelSizeMin <= pixelSize) {
		av_new_packet(&packet, pixelSizeMin);
		frame = avcodec_alloc_frame();
		avpicture_fill((AVPicture *)frame, pixelBuff, codecContext->pix_fmt, codecContext->width, codecContext->height);
		codecRet = avcodec_encode_video2(codecContext, &packet, frame, &gotPacket);
		if (0 <= codecRet && 1 == gotPacket) {
			av_write_frame(formatContext, &packet);
			if (packet.size <= *jpgSize) {
				*jpgSize = packet.size;
				result = *jpgSize;
			}
		}
		avcodec_free_frame(&frame);
		av_free_packet(&packet);
	}
	av_write_trailer(formatContext);

	av_free(ioContext);
	avcodec_close(codecContext);
	avformat_free_context(formatContext);
	return result;
}

int Pixel_to_Pixel(const unsigned char *inPixBuff, int inPixSize, int inPixFmt, int inPixWidth, int inPixHeight, 
				   unsigned char *outPixBuff, int *outPixSize, int outPixFmt, int outPixWidth, int outPixHeight) {	
	AVFrame *inFrame, *outFrame;
	struct SwsContext *swsContext;
	int result = -1;

	inFrame = avcodec_alloc_frame();
	outFrame = avcodec_alloc_frame();
	avpicture_fill((AVPicture *)inFrame, inPixBuff, (AVPixelFormat)PF(inPixFmt), inPixWidth, inPixHeight);
	avpicture_fill((AVPicture *)outFrame, outPixBuff, (AVPixelFormat)PF(outPixFmt), outPixWidth, outPixHeight);
	*outPixSize = avpicture_get_size((AVPixelFormat)PF(outPixFmt), outPixWidth, outPixHeight);
	swsContext = sws_getContext(inPixWidth, inPixHeight, (AVPixelFormat)PF(inPixFmt), outPixWidth, outPixHeight, (AVPixelFormat)PF(outPixFmt), SWS_BICUBIC, NULL, NULL, NULL);   
	result = sws_scale(swsContext, (const uint8_t *const *)inFrame->data, inFrame->linesize, 0, inPixHeight, outFrame->data, outFrame->linesize);  
	sws_freeContext(swsContext);
	avcodec_free_frame(&inFrame);
	avcodec_free_frame(&outFrame);

	if (outPixHeight == result) result = 0;
	else result = -1;
	return result;
}

int BGR24_to_BMP(const unsigned char *bgr24Buff, int bgr24Size, int bgr24Width, int bgr24Height, unsigned char *bmpBuff, int *bmpSize) {
	typedef struct { 
		unsigned long  bfSize; 
		unsigned short bfReserved1; 
		unsigned short bfReserved2; 
		unsigned long  bfOffBits; 
	} BMPFILEHEADER; 
	typedef struct {
		unsigned long  biSize; 
		long           biWidth; 
		long           biHeight; 
		unsigned short biPlanes; 
		unsigned short biBitCount; 
		unsigned long  biCompression; 
		unsigned long  biSizeImage; 
		long           biXPelsPerMeter; 
		long           biYPelsPerMeter; 
		unsigned long  biClrUsed; 
		unsigned long  biClrImportant; 
	} BMPINFOHEADER; 
	BMPFILEHEADER bmpFileHeader;
	BMPINFOHEADER bmpInfoHeader;
	char bfType[2] = {'B', 'M'}; 
	int bgr24SizeMin = bgr24Width*bgr24Height*3;
	int bmpSizeMin = bgr24SizeMin+sizeof(bfType)+sizeof(BMPFILEHEADER)+sizeof(BMPINFOHEADER);
	int result = -1;

	if (bgr24SizeMin <= bgr24Size && bmpSizeMin <= *bmpSize) {
		bmpFileHeader.bfSize = bmpSizeMin;
		bmpFileHeader.bfReserved1 = 0;
		bmpFileHeader.bfReserved2 = 0;
		bmpFileHeader.bfOffBits = sizeof(bfType) + sizeof(BMPFILEHEADER) + sizeof(BMPINFOHEADER);

		memset(&bmpInfoHeader, 0, sizeof(BMPINFOHEADER));
		bmpInfoHeader.biSize = sizeof(BMPINFOHEADER);
		bmpInfoHeader.biWidth = bgr24Width;
		bmpInfoHeader.biHeight = -bgr24Height;
		bmpInfoHeader.biPlanes = 1;
		bmpInfoHeader.biBitCount = 24;
		bmpInfoHeader.biCompression = 0L;
		bmpInfoHeader.biSizeImage = bgr24SizeMin;

		memcpy(bmpBuff, bfType, sizeof(bfType));
		memcpy(bmpBuff+sizeof(bfType), &bmpFileHeader, sizeof(BMPFILEHEADER));
		memcpy(bmpBuff+sizeof(bfType)+sizeof(BMPFILEHEADER), &bmpInfoHeader, sizeof(BMPINFOHEADER));	
		memcpy(bmpBuff+sizeof(bfType)+sizeof(BMPFILEHEADER)+sizeof(BMPINFOHEADER), bgr24Buff, bgr24SizeMin);
		*bmpSize = bmpSizeMin;
		result = 0;
	}
	return result;
}

int BMP_to_BGR24(const unsigned char *bmpBuff, int bmpSize, int *bgr24Width, int *bgr24Height, unsigned char *bgr24Buff, int *bgr24Size) {
	typedef struct {
		unsigned char rgbBlue;
		unsigned char rgbGreen;
		unsigned char rgbRed;
		unsigned char rgbReserved;
	} RGBQUA;	
	typedef struct { 
		unsigned long  bfSize; 
		unsigned short bfReserved1; 
		unsigned short bfReserved2; 
		unsigned long  bfOffBits; 
	} BMPFILEHEADER; 
	typedef struct {
		unsigned long  biSize; 
		long           biWidth; 
		long           biHeight; 
		unsigned short biPlanes; 
		unsigned short biBitCount; 
		unsigned long  biCompression; 
		unsigned long  biSizeImage; 
		long           biXPelsPerMeter; 
		long           biYPelsPerMeter; 
		unsigned long  biClrUsed; 
		unsigned long  biClrImportant; 
	} BMPINFOHEADER; 
	BMPFILEHEADER bmpFileHeader;
	BMPINFOHEADER bmpInfoHeader;
	RGBQUA *rgbQuad;
	const unsigned char *bmpData;
	unsigned char *bmpData2;
	unsigned char bmpMask;
	unsigned char rgbIdx;
	char bfType[2]; 
	const int c_bfOffBits = sizeof(bfType)+sizeof(BMPFILEHEADER)+sizeof(BMPINFOHEADER);
	int bgr24SizeMin;
	int width, height;
	int bmpIdx, wIdx, hIdx, bitIdx;
	int paletteSize;
	int result = -1;

	if (c_bfOffBits <= bmpSize) {
		memcpy(bfType, bmpBuff, sizeof(bfType));
		memcpy(&bmpFileHeader, bmpBuff+sizeof(bfType), sizeof(BMPFILEHEADER));
		memcpy(&bmpInfoHeader, bmpBuff+sizeof(bfType)+sizeof(BMPFILEHEADER), sizeof(BMPINFOHEADER));
		if ('B' == bfType[0] && 'M' == bfType[1] && 0L == bmpInfoHeader.biCompression && bmpFileHeader.bfSize <= bmpSize) {
			if (0 == (bmpInfoHeader.biWidth*bmpInfoHeader.biBitCount/8)%4)
				width = bmpInfoHeader.biWidth;
			else
				width = (bmpInfoHeader.biWidth*bmpInfoHeader.biBitCount+31)/32*4;
			if (0 == bmpInfoHeader.biHeight%2)
				height = abs(bmpInfoHeader.biHeight);
			else
				height = abs(bmpInfoHeader.biHeight)+1;
			*bgr24Width = width;
			*bgr24Height = height;

			bgr24SizeMin = width*height*bmpInfoHeader.biBitCount/8;
			if (bgr24SizeMin <= *bgr24Size) {
				*bgr24Size = bgr24SizeMin;
				bmpData = bmpBuff+bmpFileHeader.bfOffBits;
				switch (bmpInfoHeader.biBitCount) {
					case 24:
						memcpy(bgr24Buff, bmpData, bgr24SizeMin);
						break;
					case 16:
						bmpData2 = (unsigned char *)malloc(bgr24SizeMin);
						for (hIdx = 0; hIdx < height; hIdx++) {
							for (wIdx = 0; wIdx < width*bmpInfoHeader.biBitCount/8; wIdx++) {
								bmpData2[hIdx*width*bmpInfoHeader.biBitCount/8+wIdx] = bmpData[(height-hIdx-1)*width*bmpInfoHeader.biBitCount/8+wIdx];
							}
						}
						for (bmpIdx = 0; bmpIdx < bgr24SizeMin; bmpIdx += 2) {
							*(bgr24Buff+0) = (bmpData2[bmpIdx]&0x1F)<<3;
							*(bgr24Buff+1) = ((bmpData2[bmpIdx]&0xE0)>>2) + ((bmpData2[bmpIdx+1]&0x03)<<6);
							*(bgr24Buff+2) = (bmpData2[bmpIdx+1]&0x7C)<<1;
							bgr24Buff += 3;
						}
						free(bmpData2);
						break;
					default:
						bmpData2 = (unsigned char *)malloc(bgr24SizeMin);
						for (hIdx = 0; hIdx < height; hIdx++) {
							for (wIdx = 0; wIdx < width*bmpInfoHeader.biBitCount/8; wIdx++) {
								bmpData2[hIdx*width*bmpInfoHeader.biBitCount/8+wIdx] = bmpData[(height-hIdx-1)*width*bmpInfoHeader.biBitCount/8+wIdx];
							}
						}
						paletteSize = pow(float(2), bmpInfoHeader.biBitCount);
						rgbQuad = (RGBQUA *)malloc(sizeof(RGBQUA)*paletteSize);
						memcpy((unsigned char *)rgbQuad, bmpData, sizeof(RGBQUA)*paletteSize);
						for (bmpIdx = 0; bmpIdx < bgr24SizeMin; bmpIdx++) {
							switch(bmpInfoHeader.biBitCount) {
								case 1: bmpMask = 0x80; break;
								case 2: bmpMask = 0xC0; break;
								case 4: bmpMask = 0xF0; break;
								case 8: bmpMask = 0xFF; break;
							}
							bitIdx = 1;
							while (bmpMask) {
								rgbIdx = bmpMask == 0xFF ? bmpData2[bmpIdx] : ((bmpData2[bmpIdx]&bmpMask)>>(8-bitIdx*bmpInfoHeader.biBitCount));
								*(bgr24Buff+0) = rgbQuad[rgbIdx].rgbBlue;
								*(bgr24Buff+1) = rgbQuad[rgbIdx].rgbGreen;
								*(bgr24Buff+2) = rgbQuad[rgbIdx].rgbRed;

								if (8 == bmpInfoHeader.biBitCount) bmpMask = 0;
								else bmpMask >>= bmpInfoHeader.biBitCount;
								bgr24Buff += 3;
								bitIdx++;
							}
						}
						free(rgbQuad);
						free(bmpData2);
						break;
				}
				result = 0;
			}
		}
	}
	return result;
}


int main(int argc, char *argv[]) {
	FILE *pixelFile, *pixelFile2, *jpgFile, *jpgFile2, *bmpFile;
	uint8_t *pixelBuff, *pixelBuff2, *jpgBuff, *jpgBuff2, *bmpBuff;
	int jpgSize = 1024*1024*3;
	int jpgSize2 = 1024*1024*3;
	int pixelSize = 1024*1024*3;
	int pixelSize2 = 1024*1024*3;
	int bmpSize = 1024*1024*3;
	int pixelFmt, pixelWidth, pixelHeight;

	CRTMEM_END();
	pixelBuff = (uint8_t *)malloc(pixelSize);
	jpgBuff = (uint8_t *)malloc(jpgSize);
	jpgBuff2 = (uint8_t *)malloc(jpgSize2);
	pixelBuff2 = (uint8_t *)malloc(pixelSize2);
	bmpBuff = (uint8_t *)malloc(bmpSize);

	pixelFile = fopen("ffmpeg_480x272_YUVJ420P.yuv", "rb");
	pixelSize = fread(pixelBuff, sizeof(uint8_t), pixelSize, pixelFile);
	fclose(pixelFile);

	Pixel_to_Pixel(pixelBuff, pixelSize, PIXEL_FMT_YUVJ420P, 480, 272,
		pixelBuff2, &pixelSize2, PIXEL_FMT_BGR24, 444, 272);

	pixelFile2 = fopen("ffmpeg_444x272_BGR24.rgb", "wb");
	pixelSize2 = fwrite(pixelBuff2, sizeof(uint8_t), pixelSize2, pixelFile2);
	fclose(pixelFile2);

	BGR24_to_BMP(pixelBuff2, pixelSize2, 444, 272, bmpBuff, &bmpSize);

	bmpFile = fopen("ffmpeg_444x272_.bmp", "wb");
	bmpSize = fwrite(bmpBuff, sizeof(uint8_t), bmpSize, bmpFile);
	fclose(bmpFile);

	bmpFile = fopen("down32.bmp", "rb");
	bmpSize = fread(bmpBuff, sizeof(uint8_t), bmpSize, bmpFile);
	fclose(bmpFile);

	pixelSize2 = 1024*1024*3;
	memset(pixelBuff2, 0, pixelSize2);
	BMP_to_BGR24(bmpBuff, bmpSize, &pixelWidth, &pixelHeight, pixelBuff2, &pixelSize2);
	printf("[BMP_to_BGR24]width=%d, height=%d\n", pixelWidth, pixelHeight);

	pixelFile2 = fopen("ffmpeg_444x272_bgr24.rgb", "wb");
	pixelSize2 = fwrite(pixelBuff2, sizeof(uint8_t), pixelSize2, pixelFile2);
	fclose(pixelFile2);

	Pixel_to_Pixel(pixelBuff2, pixelSize2, PIXEL_FMT_BGR24, pixelWidth, pixelHeight,
		pixelBuff, &pixelSize, PIXEL_FMT_YUV444P, 444, 272);

	pixelFile = fopen("ffmpeg_444x272_YUV444P.yuv", "wb");
	pixelSize = fwrite(pixelBuff, sizeof(uint8_t), pixelSize, pixelFile);
	fclose(pixelFile);

	Pixel_to_JPG(pixelBuff, pixelSize, PIXEL_FMT_YUVJ420P, 480, 272, jpgBuff, &jpgSize);

	jpgFile = fopen("ffmpeg_480x272_YUVJ420P.jpg", "wb");
	jpgSize = fwrite(jpgBuff, sizeof(uint8_t), jpgSize, jpgFile);
	fclose(jpgFile);

	jpgFile2 = fopen("ffmpeg_480x272_YUVJ420P_2.jpg", "rb");
	jpgSize2 = fread(jpgBuff2, sizeof(uint8_t), jpgSize2, jpgFile2);
	fclose(jpgFile2);

	JPG_to_Pixel(jpgBuff2, jpgSize2, PIXEL_FMT_YUV420P, 500, 272, pixelBuff2, &pixelSize2);

	pixelFile2 = fopen("ffmpeg_500x272_YUV420P.yuv", "wb");
	pixelSize2 = fwrite(pixelBuff2, sizeof(uint8_t), pixelSize2, pixelFile2);
	fclose(pixelFile2);

	free(pixelBuff);
	free(pixelBuff2);
	free(jpgBuff);
	free(jpgBuff2);
	free(bmpBuff);

	CRTMEM_END();

	return 0;
}
#endif


