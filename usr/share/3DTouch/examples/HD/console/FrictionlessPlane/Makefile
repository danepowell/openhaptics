CXX=g++
CXXFLAGS+=-W -fexceptions -O2 -DNDEBUG -Dlinux
LIBS = -lHDU -lHD -lrt -lncurses

TARGET=FrictionlessPlane
HDRS=
SRCS=FrictionlessPlane.cpp conio.c
OBJS=$(patsubst %.cpp,%.o,$(patsubst %.c,%.o,$(SRCS)))

.PHONY: all
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $(SRCS) $(LIBS)

.PHONY: clean
clean:
	-rm -f $(OBJS) $(TARGET)




