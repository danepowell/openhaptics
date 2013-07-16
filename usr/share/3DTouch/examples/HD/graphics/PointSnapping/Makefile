CXX=g++
CXXFLAGS+=-W -fexceptions -O2 -DNDEBUG -Dlinux -Iinclude
LIBS+=-lHD -lHDU -lSnapConstraints -lglut -lGL -lGLU -lrt -lncurses

TARGET=PointSnapping
HDRS= \
	include/HapticDeviceManager.h \
	include/PointManager.h \
	include/PointSnappingAfx.h \
	include/PointSnappingUtil.h \
	include/ViewApparentPointConstraint.h
SRCS= \
	src/HapticDeviceManager.cpp \
	src/main.cpp \
	src/PointManager.cpp \
	src/PointSnappingAfx.cpp \
	src/PointSnappingUtil.cpp \
	src/ViewApparentPointConstraint.cpp
OBJS=$(SRCS:.cpp=.o)

.PHONY: all
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $(SRCS) $(LIBS)

.PHONY: clean
clean:
	-rm -f $(OBJS) $(TARGET)
