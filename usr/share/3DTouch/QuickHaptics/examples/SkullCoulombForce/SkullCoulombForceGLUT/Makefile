CC=gcc
CFLAGS+=-W -g -DNDEBUG -Dlinux
CFLAGS+=-I/usr/include/QH
LIBS = -lQHGLUTWrapper -lQH -lHL -lHLU -lHDU -lHD -lGL -lGLU -lglut -lrt -lncurses

TARGET=SkullCoulombForceGLUT
HDRS=
SRCS=src/main.cpp
OBJS=$(SRCS:.cpp=.o)

.PHONY: all
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $(SRCS) $(LIBS)

.PHONY: clean
clean:
	-rm -f $(OBJS) $(TARGET)
