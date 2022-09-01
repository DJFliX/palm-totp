OBJS = totp.o TOTPAlg.o sha1.o
TOOLCHAIN ?= /opt/palmdev/buildtools/toolchain/bin
SDK     = /opt/palmdev/buildtools/palm-os-sdk-master/sdk-3.1/include
PILRC = /opt/palmdev/buildtools/pilrc3_3_unofficial/bin/pilrc

CC = m68k-palmos-gcc
LKR	= linker.lkr
LD = m68k-palmos-gcc

COMMON = -Wno-multichar -funsafe-math-optimizations -Os -m68000 -mno-align-int -mpcrel -fpic -fshort-enums -mshort

#uncomment this if you want to build a gdb debuggable version
#DEFINES = -DDEBUG
#INCLUDES			+=	-isystem$(SDK)/
INCLUDES			+=	-isystem$(SDK)/Core
INCLUDES            +=  -isystem$(SDK)/Core/International
INCLUDES			+=	-isystem$(SDK)/Core/Hardware
INCLUDES			+=	-isystem$(SDK)/Core/System
INCLUDES			+=	-isystem$(SDK)/Core/UI
# INCLUDES			+=	-isystem$(SDK)/Dynamic
# INCLUDES			+=	-isystem$(SDK)/Libraries
# INCLUDES			+=	-isystem$(SDK)/Libraries/PalmOSGlue

CSFLAGS = -O2 -S $(INCLUDES)
CFLAGS = -O2 -palmos3.1 -g $(INCLUDES)

OBJRES = m68k-palmos-obj-res
BUILDPRC = build-prc

ICONTEXT = "TOTP"
APPID = TOTP
PRC = totp.prc

all: $(PRC)

.S.o:
	$(CC) $(TARGETFLAGS) -c $<

.c.s:
	$(CC) $(CSFLAGS) $<

$(PRC): code.stamp bin.stamp
	$(BUILDPRC) $@ $(ICONTEXT) $(APPID) *.grc *.bin

code.stamp: totp
	$(OBJRES) totp
	touch code.stamp

bin.stamp: totp.rcp
	$(PILRC) totp.rcp
	touch bin.stamp

totp: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

fullclean:
	rm -rf *.[oa] totp *.bin *.stamp *.grc totp.prc

clean:
	rm -rf *.[oa] totp *.bin *.stamp *.grc

