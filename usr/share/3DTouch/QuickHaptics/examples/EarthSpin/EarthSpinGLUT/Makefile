CC=gcc
CFLAGS+=-W -g -DNDEBUG -Dlinux
INCLUDE=-I/usr/include/QH
LIBS = -lQHGLUTWrapper -lQH -lHL -lHLU -lHDU -lHD -lGL -lGLU -lglut -lrt -lncurses

TARGET=EarthSpinGLUT
HDRS=
SRCS=src/main.cpp
OBJS=$(SRCS:.cpp=.o)

.PHONY: all
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $(SRCS) $(LIBS) $(INCLUDE)

.PHONY: clean
clean:
	-rm -f $(OBJS) $(TARGET)
