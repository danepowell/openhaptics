# Makefile PointManipulation

CXX=g++
CXXFLAGS+=-W -fexceptions -O2 -DNDEBUG -Dlinux -Iinclude
LIBS+=-lHD -lHDU -lSnapConstraints -lglut -lGL -lGLU -lrt -lncurses

TARGET=PointManipulation
CFLAGS+=
SRCS= \
	src/HapticDeviceManager.cpp \
	src/main.cpp \
	src/PlaneWithAxesConstraint.cpp \
	src/PointManager.cpp \
	src/PointManipulationAfx.cpp \
	src/PointManipulationUtil.cpp \
	src/ViewApparentPointConstraint.cpp

HDRS= \
	include/HapticDeviceManager.h \
	include/PlaneWithAxesConstraint.h \
	include/PointManager.h \
	include/PointManipulationAfx.h \
	include/PointManipulationUtil.h \
	include/ViewApparentPointConstraint.h
OBJS=$(SRCS:.cpp=.o)

.PHONY: all
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $(SRCS) $(LIBS)

.PHONY: clean
clean:
	-rm -f $(OBJS) $(TARGET)
