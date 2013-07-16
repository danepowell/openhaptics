CXX=g++
CXXFLAGS+=-W -fexceptions -O2 -DNDEBUG -Dlinux -Iinclude
LIBS = -lHDU -lHD -lrt -lncurses

TARGET=ServoLoopDutyCycle
HDRS=include/StatsSampler.h
SRCS=src/ServoLoopDutyCycle.cpp \
     src/StatsSampler.cpp \
     src/conio.c
OBJS=$(patsubst %.cpp,%.o,$(patsubst %.c,%.o,$(SRCS)))

.PHONY: all
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $(SRCS) $(LIBS)

.PHONY: clean
clean:
	-rm -f $(OBJS) $(TARGET)
