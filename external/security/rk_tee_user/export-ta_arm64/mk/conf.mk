sm := ta_arm64
sm-ta_arm64 := y
CFG_TA_FLOAT_SUPPORT := y
CFG_ARM64_ta_arm64 := y
ta_arm64-platform-cppflags := -DARM64=1 -D__LP64__=1
ta_arm64-platform-cflags := -Os -gdwarf-2 -fpie -mstrict-align
ta_arm64-platform-aflags := -gdwarf-2
CROSS_COMPILE64 ?= $(CROSS_COMPILE)
CROSS_COMPILE_ta_arm64 ?= $(CROSS_COMPILE64)

