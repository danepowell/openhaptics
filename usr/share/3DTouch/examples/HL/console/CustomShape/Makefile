CC=gcc
CFLAGS+=-W -O2 -DNDEBUG -Dlinux
LIBS = -lHL -lHLU -lHDU -lHD -lrt -lncurses

TARGET=CustomShape

HDRS= \
	HapticRings.h \
	HapticSphere.h \
	conio.h
SRCS= \
	CustomShape.cpp \
	HapticRings.cpp \
	HapticSphere.cpp \
	conio.c
OBJS=$(patsubst %.cpp,%.o,$(patsubst %.c,%.o,$(SRCS)))

.PHONY: all
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $(SRCS) $(LIBS)

.PHONY: clean
clean:
	-rm -f $(OBJS) $(TARGET)

