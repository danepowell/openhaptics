CXX=g++
CXXFLAGS+=-W -fexceptions -O2 -DNDEBUG -Dlinux
LIBS+=-lHD -lHDU -lrt -lGL -lGLU -lglut -lncurses

TARGET=CoulombForceDual
HDRS=
SRCS= \
	helper.cpp \
	main.cpp
OBJS=$(SRCS:.cpp=.o)    

.PHONY: all
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $(SRCS) $(LIBS)

.PHONY: clean
clean:
	-rm -f $(OBJS) $(TARGET)
