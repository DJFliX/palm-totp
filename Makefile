OBJS = totp.o lib/TOTPAlg.o lib/sha1.o
PALMDEV_ROOT = /opt/palmdev/buildtools
TOOLCHAIN ?= $(PALMDEV_ROOT)/toolchain/bin
SDK     = $(PALMDEV_ROOT)/palm-os-sdk-master/sdk-4/include
PILRC = $(PALMDEV_ROOT)/pilrc3_3_unofficial/bin/pilrc

CC = m68k-palmos-gcc
LKR	= linker.lkr
LD = m68k-palmos-gcc

COMMON = -Wno-multichar -funsafe-math-optimizations -Os -m68000 -mno-align-int -mpcrel -fpic -fshort-enums -mshort

INCLUDES			+=	-isystem$(SDK)/Core
INCLUDES			+=	-isystem$(SDK)/Core/System
INCLUDES			+=	-isystem$(SDK)/Core/UI

CSFLAGS = -O2 -S $(INCLUDES)
CFLAGS = -O2 -palmos4 -g $(INCLUDES)

OBJRES = m68k-palmos-obj-res
BUILDPRC = build-prc

ICONTEXT = "TOTP"
APPID = TOTP
PRC = totp.prc

all: $(PRC)

$(PRC): code.stamp bin.stamp
	$(BUILDPRC) bin/$@ $(ICONTEXT) $(APPID) bin/*.grc bin/*.bin

code.stamp: totp
	(cd bin && $(OBJRES) totp)
	touch code.stamp

bin.stamp: totp.rcp
	$(PILRC) totp.rcp bin
	touch bin.stamp

totp: $(OBJS)
	mkdir -p bin
	$(CC) $(CFLAGS) $(OBJS) -o bin/$@

clean:
	rm -rf *.[oa] totp *.bin *.stamp *.grc lib/*.[oa] bin
