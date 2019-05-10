###############################################################################
#
# Installation DIRECTORIES
#
# Change these if you want to install somewhere else.

###############################################################################
#
# GENERAL
#

INCLUDES	=	-I.
LIBRARIES	=	-ldl -lm
CFLAGS		=	$(INCLUDES) -Wall  -O3
LOCAL_LDFLAGS   =      -L./ -lRK_AudioProcess 
CXXFLAGS	=	$(CFLAGS)
PLUGINS		=	./Equalizer.so
CC		=	../../../buildroot/output/rockchip_rk3308_release/host/usr/bin/aarch64-rockchip-linux-gnu-gcc
CPP		=	../../../buildroot/output/rockchip_rk3308_release/host/usr/bin/aarch64-rockchip-linux-gnu-g++
LD		=	../../../buildroot/output/rockchip_rk3308_release/host/usr/bin/aarch64-rockchip-linux-gnu-ld

###############################################################################
#
# RULES TO BUILD PLUGINS FROM C OR C++ CODE
#

./%.so:	%.c ladspa.h
	$(CC) $(CFLAGS) -o ./$*.o -c -fPIC ./$*.c
	$(LD) -o ./$*.so ./$*.o -shared $(LOCAL_LDFLAGS)


###############################################################################
#
# TARGETS
#

install:	targets

targets:	$(PLUGINS) 


###############################################################################
#
# UTILITIES
#

always:	

clean:
	-rm -f `find . -name "*.o"` ./*.so
	-rm -f `find .. -name "*~"`
	-rm -f *.bak core score.srt
	-rm -f *.bb *.bbg *.da *-ann gmon.out bb.out
	-rm -f `find .. -name "*.class"`

backup:		clean
	(cd ../../;							\
	tar czf `date '+../backup/ladspa_sdk.%Y%m%d%H%M.tgz'` ladspa_sdk/)

###############################################################################
