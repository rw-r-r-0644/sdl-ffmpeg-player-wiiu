#include "Player.hpp"
#include <whb/log.h>
#include <whb/log_udp.h>
#include <sys/iosupport.h>

#ifndef VIDEO_NAME
#define VIDEO_NAME	"big_buck_bunny_144p_h264.mov"
#endif

using namespace std;

static devoptab_t dotab_stdout;

static ssize_t wiiu_log_write (struct _reent *r, void *fd, const char *ptr, size_t len) {
	WHBLogPrintf("%*.*s", len, len, ptr);
	return len;
}

int main(int argc, const char *argv[]) {
	WHBLogUdpInit();
	memset(&dotab_stdout, 0, sizeof(devoptab_t));
	dotab_stdout.name = "stdout_udp";
	dotab_stdout.write_r = &wiiu_log_write;
	devoptab_list[STD_OUT] = &dotab_stdout;
	devoptab_list[STD_ERR] = &dotab_stdout;
	
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
		cout << "There is something wrong with your SDL Libs. Can't run" << endl;
		exit(-1);
	}
	
	Player * player = new Player(VIDEO_NAME);
	
	int res = player->allocMemory();
	if (res < 0) {
		cout << "Fatal Error";
		delete(player);
		exit(-1);
	}

	res= player->createDisplay();

	res = player->readVideoFrames();
	if (res < 0) {
		cout << "error" << endl;
		delete(player);
		exit(-1);
	}

	delete(player);

	return 0;
}