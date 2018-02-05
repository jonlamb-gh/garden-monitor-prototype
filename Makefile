##########################################################
# makefile for garden-monitor
##########################################################

TARGET := bin/garden-monitor

SRCS := \
		src/atimer.c \
		src/pio.c \
		src/pio_ring.c \
		src/pio_filter.c \
		src/font.c \
		src/measurement_plot.c \
		src/gui.c \
		src/touch_input.c \
		src/main.c

OBJS := $(SRCS:.c=.o)
DEPS := $(SRCS:.c=.dep)
XDEPS := $(wildcard $(DEPS))

CC = gcc

CCFLAGS = -std=gnu99

CCFLAGS += -Wall -Wextra -Werror \
		-Wformat=2 -Wno-unused-parameter -Wshadow \
		-Wwrite-strings -Wstrict-prototypes -Wold-style-definition \
		-Wnested-externs -Wmissing-include-dirs

LDFLAGS =

INCLUDES = -Iinclude \
        -I/opt/vc/include \
        -I/opt/vc/include/interface/vmcs_host/linux \
        -I/opt/vc/include/interface/vcos/pthreads

LIBS = -L/opt/vc/lib \
        -lEGL -lGLESv2 -lbcm_host \
        -lpthread -lrt -lm \
        -lshapes -lao -lphidget22 -lpopt -lzlog

all: dirs $(TARGET)

ifneq ($(XDEPS),)
include $(XDEPS)
endif

dirs::
	mkdir -p bin

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(OBJS): %.o: %.c %.dep
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

$(DEPS): %.dep: %.c Makefile
	$(CC) $(CCFLAGS) $(INCLUDES) -MM $< > $@

install:
	cp $(TARGET) /usr/local/bin/

clean:
	-rm -f src/*.o
	-rm -f src/*.dep
	-rm -f $(TARGET)
