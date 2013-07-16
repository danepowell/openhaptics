# Makefile - SnapConstraints

CXX=g++
CXXFLAGS=$(CFLAGS)
AR=ar
ARFLAGS=rus

LIBDIR=/usr/lib
#DEBUG = TRUE

TARGET := SnapConstraints
TARGET := $(addprefix lib,$(TARGET))
TARGET := $(addsuffix .a,$(TARGET))

ifdef DEBUG
CFLAGS+=-W -fexceptions -g -O0 -Dlinux -D_DEBUG
else
CFLAGS+=-W -fexceptions -O2 -Dlinux -DNDEBUG
endif

SRCS= \
	CompositeConstraint.cpp \
	ConstraintHolder.cpp \
	LineConstraint.cpp \
	PlaneConstraint.cpp \
	PointConstraint.cpp \
	SnapConstraint.cpp \
	SnapConstraintsAfx.cpp \
	SnapConstraintsAPI.cpp

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
