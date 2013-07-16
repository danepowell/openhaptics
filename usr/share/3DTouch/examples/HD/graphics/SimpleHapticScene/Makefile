CC=gcc
CFLAGS+=-W -O2 -DNDEBUG -Dlinux
LIBS+=-lHD -lHDU -lSnapConstraints -lglut -lGL -lGLU -lrt -lncurses

TARGET=SimpleHapticScene
HDRS=
SRCS=SimpleHapticScene.c
OBJS=$(SRCS:.c=.o)

.PHONY: all
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $(SRCS) $(LIBS)

.PHONY: clean
clean:
	-rm -f $(OBJS) $(TARGET)
