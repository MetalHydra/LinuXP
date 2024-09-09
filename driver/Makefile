CFLAGS=--sysroot $(SYSROOT) -g -Wall -static
LDLIBS=-lrt

KDIR := ~/embedded/Projekt/linux/
PWD := $(shell pwd)
obj-m := blinkdrvad.o 

CXX = $(CROSS_COMPILE)g++
CC = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)as
AR = $(CROSS_COMPILE)ar
NM = $(CROSS_COMPILE)nm
LD = $(CROSS_COMPILE)ld
OBJDUMP = $(CROSS_COMPILE)objdump
OBJCOPY = $(CROSS_COMPILE)objcopy
RANLIB = $(CROSS_COMPILE)ranlib
STRIP = $(CROSS_COMPILE)strip

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	rm -rf Module.symvers
	rm -rf .Module.*
	rm -rf .modules.order*
	rm -rf modules.order
	rm -rf *.mod *.mod.* *.cmd
	


clean:
	rm -rf *.ko *.o
