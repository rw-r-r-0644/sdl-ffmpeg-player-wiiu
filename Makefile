BASEDIR := $(dir $(firstword $(MAKEFILE_LIST)))
VPATH := $(BASEDIR)

CXXFLAGS	+=	-Wall -O3 -Wno-deprecated-declarations
LDFLAGS	+=	$(WUT_NEWLIB_LDFLAGS) $(WUT_DEVOPTAB_LDFLAGS) \
			-lcoreinit -lvpad -lsysapp -lsndcore2 -lnsysnet -lproc_ui -lgx2 -lgfd -lwhb \
			-lavformat -lavutil -lavcodec -lavfilter -lswscale -lswresample -lSDL2 -lzlib125 -lm

player.rpx: Player.o main.o

clean:
	rm -rf ./*.o ./*.d ./*.rpx

include $(WUT_ROOT)/share/wut.mk
PORTLIBS :=	$(DEVKITPRO)/portlibs/ppc
LDFLAGS += -L$(PORTLIBS)/lib
CXXFLAGS += -I$(PORTLIBS)/include