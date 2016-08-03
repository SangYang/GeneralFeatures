#if 1
#include "ffmpeg_package.h"
#include <errno.h>
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

int JPG_to_Pixel(const unsigned char *jpgBuff, int jpgSize, int pixelFmt, unsigned char *pixelBuff, int *pixelSize, int *pixelWidth, int *pixelHeight) {	
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
			gotFrame = 0;
			codecRet = avcodec_decode_video2(codecContext, frame, &gotFrame, &packet);
			if (0 <= codecRet && 1 == gotFrame) {
				frame2 = avcodec_alloc_frame();
				memcpy(frame2, frame, sizeof(AVFrame));
				frame2->format = PF(pixelFmt);
				swsContext = sws_getContext(codecContext->width, codecContext->height, codecContext->pix_fmt, codecContext->width, codecContext->height, (AVPixelFormat)frame2->format, SWS_BICUBIC, NULL, NULL, NULL);   
				sws_scale(swsContext, (const uint8_t *const *)frame->data, frame->linesize, 0, codecContext->height, frame2->data, frame2->linesize);  
				sws_freeContext(swsContext);

				*pixelWidth = codecContext->width;
				*pixelHeight = codecContext->height;
				result = save_YUV420P(frame2, codecContext->width, codecContext->height, pixelBuff, pixelSize);

				av_free(frame2);
			}	
			av_free(frame);
			avcodec_close(codecContext);
			break;
		}
	}

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
	uint8_t *ioBuff;
	int ioRet;
	int codecRet;
	int gotPacket;
	int pixelSizeMin;
	int result = -1;

	av_register_all();
	formatContext = avformat_alloc_context();
	outputFormat = av_guess_format("mjpeg", NULL, NULL);
	avio_open_dyn_buf(&ioContext);
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
		gotPacket = 0;
		codecRet = avcodec_encode_video2(codecContext, &packet, frame, &gotPacket);
		if (0 <= codecRet && 1 == gotPacket) {
			av_write_frame(formatContext, &packet);
			if (packet.size <= *jpgSize) {
				ioBuff = (uint8_t *)av_malloc(packet.size);
				avio_close_dyn_buf(ioContext, &ioBuff);
				formatContext->pb = NULL;
				memcpy(jpgBuff, ioBuff, packet.size);
				av_free(ioBuff);
				*jpgSize = packet.size;
				result = 0;
			}
		}
		av_free(frame);
		av_free_packet(&packet);
	}
	if (NULL != formatContext->pb) {
		ioBuff = (uint8_t *)av_malloc(1024);
		avio_close_dyn_buf(ioContext, &ioBuff);
		av_free(ioBuff);
	}
	av_write_trailer(formatContext);

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


int main(int argc, char *argv[]) {
	FILE *pixelFile, *pixelFile2, *jpgFile, *jpgFile2;
	uint8_t *pixelBuff, *pixelBuff2, *jpgBuff, *jpgBuff2;
	int jpgSize = 1024*1024*3;
	int jpgSize2 = 1024*1024*3;
	int pixelSize = 1024*1024*3;
	int pixelSize2 = 1024*1024*3;
	int pixelFmt, pixelWidth, pixelHeight;

	CRTMEM_END();
	pixelBuff = (uint8_t *)malloc(pixelSize);
	jpgBuff = (uint8_t *)malloc(jpgSize);
	jpgBuff2 = (uint8_t *)malloc(jpgSize2);
	pixelBuff2 = (uint8_t *)malloc(pixelSize2);

	pixelFile = fopen("cuc_view_480x272.yuv", "rb");
	pixelSize = fread(pixelBuff, sizeof(uint8_t), pixelSize, pixelFile);
	fclose(pixelFile);

/*
	Pixel_to_Pixel(pixelBuff, pixelSize, PIXEL_FMT_YUVJ420P, 480, 272,
		pixelBuff2, &pixelSize2, PIXEL_FMT_YUV420P, 480, 372);
*/
	Pixel_to_JPG(pixelBuff, pixelSize, PIXEL_FMT_YUVJ420P, 480, 272, jpgBuff, &jpgSize);

	jpgFile = fopen("cuc_view_480x272.jpg", "wb");
	jpgSize = fwrite(jpgBuff, sizeof(uint8_t), jpgSize, jpgFile);
	fclose(jpgFile);

	jpgFile2 = fopen("cuc_view_480x272_2.jpg", "rb");
	jpgSize2 = fread(jpgBuff2, sizeof(uint8_t), jpgSize2, jpgFile2);
	fclose(jpgFile2);

	JPG_to_Pixel(jpgBuff2, jpgSize2, PIXEL_FMT_YUV420P, pixelBuff2, &pixelSize2, &pixelWidth, &pixelHeight);

	pixelFile2 = fopen("cuc_view_480x272_2.yuv", "wb");
	pixelSize2 = fwrite(pixelBuff2, sizeof(uint8_t), pixelSize2, pixelFile2);
	fclose(pixelFile2);

	free(pixelBuff);
	free(pixelBuff2);
	free(jpgBuff);
	free(jpgBuff2);

	CRTMEM_END();

	return 0;
}
#endif


