#####################################
### iFlyteck SDK demo application ###
#####################################
# source file
SOURCE  := $(wildcard src/*.c) $(wildcard src/*.cpp)
OBJS    := $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCE)))

# target you can change test to what you want
TARGET  := iflytekDemo 
#compile and lib parameter
#CC      := ../../buildroot/output/rockchip_rk3308_release/host/usr/bin/aarch64-rockchip-linux-gnu-gcc
#CXX     := ../../buildroot/output/rockchip_rk3308_release/host/usr/bin/aarch64-rockchip-linux-gnu-g++
#LD      := ../../buildroot/output/rockchip_rk3308_release/host/usr/bin/aarch64-rockchip-linux-gnu-ld
LIBS    := -ldl -lpthread -lcae -lIvw60 -lmsc -lasound -ljson-c -lssl -lcrypto -lcurl -lm -lcjson
LDFLAGS := `pkg-config --libs gstreamer-1.0`
LDFLAGS += -L$(CAE_PATH)/

DEFINES := -fpic -Wl,-rpath=.
INCLUDE := -Iinclude 
CFLAGS  := $(shell pkg-config --cflags gstreamer-1.0) -g -Wall -O3 $(DEFINES) $(INCLUDE)

CXXFLAGS:= $(CFLAGS)

$(TARGET) : $(OBJS)
	    $(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)
.PHONY : install everything objs clean veryclean rebuild install

install:
	cp -f $(TARGET) $(TARGET_DIR)/usr/bin/
	cp -f bin/ivw_resource.jet $(TARGET_DIR)/etc/
	cp -f $(CAE_PATH)/*.so $(TARGET_DIR)/usr/lib/

everything : $(TARGET)

all : $(TARGET)

objs : $(OBJS)

rebuild: veryclean everything

clean :
	rm -rf src/*.o
	rm $(TARGET)

veryclean : clean
	rm -rf $(TARGET)
