# Makefile PointManipulation

CXX=g++
CXXFLAGS+=-W -O2 -Iinclude -DNDEBUG -Dlinux
LIBS = -lHL -lHLU -lHDU -lHD -lGL -lGLU -lglut -lrt -lncurses

TARGET=PointManipulation
SRCS= \
	src/HapticManager.cpp \
	src/PointManager.cpp \
	src/PointManipulation.cpp \
	src/PointManipulationAfx.cpp \
	src/ViewManager.cpp
HDRS= \
	include/HapticManager.h \
	include/PointManager.h \
	src/PointManipulationAfx.h \
	include/ViewManager.h
OBJS=$(SRCS:.cpp=.o)

.PHONY: all
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $(SRCS) $(LIBS)

.PHONY: clean
clean:
	-rm -f $(OBJS) $(TARGET)
