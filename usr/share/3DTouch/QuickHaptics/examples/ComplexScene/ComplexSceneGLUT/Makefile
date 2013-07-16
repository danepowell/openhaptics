CC=gcc
#CFLAGS+=-W -O2 -DNDEBUG -Dlinux
CFLAGS+=-W -g -DNDEBUG -Dlinux
LIBS = -lHL -lHLU -lHDU -lHD -lGL -lGLU -lglut -lQH -lQHGLUTWrapper -lrt -lncurses 
INCLUDES = -I/usr/include/QH
TARGET=ComplexSceneGLUT
HDRS=
SRCS=src/main.cpp
OBJS=$(SRCS:.cpp=.o)

.PHONY: all
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $(SRCS) $(LIBS) $(INCLUDES)

.PHONY: clean
clean:
	-rm -f $(OBJS) $(TARGET)
