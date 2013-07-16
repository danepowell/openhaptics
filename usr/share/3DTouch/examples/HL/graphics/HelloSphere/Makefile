CC=gcc
CFLAGS+=-W -g -DNDEBUG -Dlinux
LIBS = -lHL -lHLU -lHDU -lHD -lGLU -lGL -lglut

TARGET=HelloSphere
HDRS=
SRCS=HelloSphere.cpp
OBJS=$(SRCS:.cpp=.o)

.PHONY: all
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $(SRCS) $(LIBS)

.PHONY: clean
clean:
	-rm -f $(OBJS) $(TARGET)
