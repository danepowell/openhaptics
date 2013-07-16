# Makefile - HelloSphereDual

CC=gcc
CFLAGS+=-W -O2 -DNDEBUG -Dlinux
LIBS = -lHL -lHLU -lHDU -lHD -lGL -lGLU -lglut -lrt -lncurses

TARGET=HelloSphereDual
HDRS=
SRCS=HelloSphereDual.cpp
OBJS=$(SRCS:.cpp=.o)

.PHONY: all
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $(SRCS) $(LIBS)

.PHONY: clean
clean:
	-rm -f $(OBJS) $(TARGET)
