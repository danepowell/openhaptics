# Makefile - HDU

CXX=g++
CXXFLAGS=$(CFLAGS)
AR=ar
ARFLAGS=rus

LIBDIR=/usr/local/lib
#DEBUG = TRUE

TARGET := HDU
TARGET := $(addprefix lib,$(TARGET))
TARGET := $(addsuffix .a,$(TARGET))

ifdef DEBUG
CFLAGS+=-W -fexceptions -g -O0 -Dlinux -D_DEBUG
else
CFLAGS+=-W -fexceptions -O2 -Dlinux -DNDEBUG
endif

SRCS= \
	hduDecompose.cpp \
	hduLine.cpp \
	hduLineSegment.cpp \
	hduMatrix.cpp \
	hduPlane.cpp \
	hduQuaternion.cpp \
	hduRecord.cpp \
	hdu.cpp \
	hduAfx.cpp \
	hduError.cpp \
	hduHapticDevice.cpp

OBJS=$(SRCS:.cpp=.o)

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	-rm -f $(OBJS) $(TARGET)

.PHONY: install
install: all
	install -m 755 -o 0 -g -d $(LIBDIR)
	install -m 755 -o 0 -g 0 $(TARGET) $(LIBDIR)
