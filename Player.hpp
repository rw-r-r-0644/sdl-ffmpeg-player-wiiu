#include <iostream>
#include <assert.h>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavfilter/avfilter.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavutil/avutil.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
}
#include "SDL2/SDL.h"
#include "SDL2/SDL_thread.h"
#include "SDL2/SDL_syswm.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_audio.h"

#define SDL_MAIN_HANDLED
#define ERROR_SIZE 128
#define FORMATO AV_PIX_FMT_RGB24
#define SDL_AUDIO_BUFFER_SIZE 1024;
#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000

typedef struct _AudioPacket
	{
		AVPacketList *first, *last;
		int nb_packets, size;
  		SDL_mutex *mutex;
  		SDL_cond *cond;
	} AudioPacket;

class Player {

public:
	
	//construtor
	Player(std::string endereco) {

		audioStream = -1;

		//init ffmpeg
		av_register_all();

		//open video
		int res = avformat_open_input(&pFormatCtx, endereco.c_str(), NULL, NULL);

		//check video opened
		if (res!=0){
			showError(res);
			exit(-1);
		}

		//get video info
		res = avformat_find_stream_info(pFormatCtx, NULL);
		if (res < 0) {
			showError(res);
			exit(-1);
		}

		//get video stream
		videoStream = obtainCodecParameters();
		if (videoStream == -1) {
			std::cout << "Error opening your video using AVCodecParameters, does not have codecpar_type type AVMEDIA_TYPE_VIDEO" << std::endl;
			exit(-1);
		}

		if (readVideoCodec() < 0) exit(-1);

	}

	~Player(void) {

		av_free(buffer);
		av_free(pFrameRGB);

		// Free the YUV frame
		av_free(pFrame);

		// Close the codecs
		avcodec_close(pCodecCtx);

		// Close the video file
		avformat_close_input(&pFormatCtx);

	}

	
	void showPlayingVideoInfo(void);
	int allocMemory(void);
	int readVideoFrames(void);
	int createDisplay(void);
	
	static int getAudioPacket(AudioPacket*, AVPacket*, int);

private:
	
	void memsetAudioPacket(AudioPacket * pq);
	//stores the index of the given Stream to be transmitted
	int videoStream;

	//audio stream
	int audioStream;

	//contains information about the video file, including codecs, etc.
	AVFormatContext *pFormatCtx = NULL;

	// contains information about the video codec, obtained through
	// pFormatCtx-> streams [i] -> codecpar
	// looking at the codec_type and seeing if video transmission of type AVMEDIA_TYPE_VIDEO
	AVCodecParameters *pCodecParameters = NULL;

	//Audio COdec Parametrs
	AVCodecParameters *pCodecAudioParameters = NULL;

	//information from the codecParameters, but copied them. pCodecParameters serves as a backup of the video information
	AVCodecContext *pCodecCtx = NULL;

	AVCodecContext *pCodecAudioCtx = NULL;

	SDL_AudioSpec wantedSpec = { 0 }, audioSpec = { 0 };

	//save the video codec
	AVCodec *pCodec = NULL;

	//save audio codec
	AVCodec *pAudioCodec = NULL;

	//structure that holds the frame
	AVFrame *pFrame = NULL;

	//structure that holds the RGB frame
	AVFrame *pFrameRGB = NULL;

	//buffer for reading frames
	uint8_t *buffer = NULL;

	//structure that stores the conversion to RGB
	struct SwsContext *sws_ctx = NULL;

	// surface windows to display the video
	// can have multiple screens
	SDL_Window *screen;

	SDL_Renderer *renderer;

	SDL_Texture* bmp;
	
	//displays the error with respect to its respective code
	void showError(int err);

	int obtainCodecParameters(void);

	int readVideoCodec(void);

	int PacketQueuePut(AudioPacket *, const AVPacket *);

	void initAudioPacket(AudioPacket *); 

	int putAudioPacket(AudioPacket *, AVPacket *); 

};